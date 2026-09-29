#include "zachfix/input/input_latency.h"

#include "zachfix/core/config.h"
#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>

namespace
{
constexpr unsigned char kNearCallOpcode = 0xE8;

std::atomic_bool g_available{false};
std::atomic_bool g_active{false};
unsigned char* g_firstCallsite = nullptr;
unsigned char* g_secondCallsite = nullptr;
uintptr_t g_commitTarget = 0;
uintptr_t g_pollTarget = 0;
const char* g_buildName = nullptr;
uintptr_t g_firstCallsiteRva = 0;
uintptr_t g_secondCallsiteRva = 0;

bool IsRangeInsideMainExe(uintptr_t address, size_t size)
{
    if (address == 0 || size == 0 || g_mainExeBase == 0 || g_mainExeSize == 0)
        return false;

    const uintptr_t end = address + size;
    const uintptr_t imageEnd = g_mainExeBase + g_mainExeSize;
    return end >= address && address >= g_mainExeBase && end <= imageEnd;
}

bool DecodeNearCallTarget(const unsigned char* callsite, uintptr_t& target)
{
    if (!callsite || callsite[0] != kNearCallOpcode)
        return false;

    std::int32_t displacement = 0;
    std::memcpy(&displacement, callsite + 1, sizeof(displacement));

    const uintptr_t nextInstruction =
        reinterpret_cast<uintptr_t>(callsite) + 5u;
    target = static_cast<uintptr_t>(
        static_cast<std::intptr_t>(nextInstruction) + displacement);
    return true;
}

bool EncodeNearCallDisplacement(
    const unsigned char* callsite,
    uintptr_t target,
    std::int32_t& displacement)
{
    if (!callsite)
        return false;

    const std::intptr_t nextInstruction = static_cast<std::intptr_t>(
        reinterpret_cast<uintptr_t>(callsite) + 5u);
    const std::intptr_t delta =
        static_cast<std::intptr_t>(target) - nextInstruction;

    if (delta < std::numeric_limits<std::int32_t>::min() ||
        delta > std::numeric_limits<std::int32_t>::max())
    {
        return false;
    }

    displacement = static_cast<std::int32_t>(delta);
    return true;
}

bool ReadCurrentPair(uintptr_t& firstTarget, uintptr_t& secondTarget)
{
    return DecodeNearCallTarget(g_firstCallsite, firstTarget) &&
           DecodeNearCallTarget(g_secondCallsite, secondTarget);
}

bool PairMatches(bool enabled, uintptr_t firstTarget, uintptr_t secondTarget)
{
    return enabled
        ? firstTarget == g_pollTarget && secondTarget == g_commitTarget
        : firstTarget == g_commitTarget && secondTarget == g_pollTarget;
}

bool WritePair(bool enabled)
{
    const uintptr_t desiredFirst = enabled ? g_pollTarget : g_commitTarget;
    const uintptr_t desiredSecond = enabled ? g_commitTarget : g_pollTarget;

    uintptr_t observedFirst = 0;
    uintptr_t observedSecond = 0;
    if (!ReadCurrentPair(observedFirst, observedSecond))
    {
        AppendLog(
            "[Input][LowLatency] ERROR: current main-tick CALL pair could not be decoded; no change made.\n");
        return false;
    }

    if (PairMatches(enabled, observedFirst, observedSecond))
        return true;

    // A live transition is accepted only from the other complete known state.
    // A mixed or externally modified pair fails closed instead of guessing.
    if (!PairMatches(!enabled, observedFirst, observedSecond))
    {
        char text[384] = {};
        sprintf_s(
            text,
            "[Input][LowLatency] ERROR: main-tick CALL pair is neither vanilla nor the known low-latency state on %s; no change made. first=DP.exe+0x%08lX second=+0x%08lX.\n",
            g_buildName ? g_buildName : "unknown build",
            observedFirst >= g_mainExeBase
                ? static_cast<unsigned long>(observedFirst - g_mainExeBase)
                : 0ul,
            observedSecond >= g_mainExeBase
                ? static_cast<unsigned long>(observedSecond - g_mainExeBase)
                : 0ul);
        AppendLog(text);
        return false;
    }

    std::int32_t firstDisplacement = 0;
    std::int32_t secondDisplacement = 0;
    if (!EncodeNearCallDisplacement(
            g_firstCallsite, desiredFirst, firstDisplacement) ||
        !EncodeNearCallDisplacement(
            g_secondCallsite, desiredSecond, secondDisplacement))
    {
        AppendLog(
            "[Input][LowLatency] ERROR: replacement CALL displacement is out of range; no change made.\n");
        return false;
    }

    std::int32_t oldFirstDisplacement = 0;
    std::int32_t oldSecondDisplacement = 0;
    std::memcpy(
        &oldFirstDisplacement,
        g_firstCallsite + 1,
        sizeof(oldFirstDisplacement));
    std::memcpy(
        &oldSecondDisplacement,
        g_secondCallsite + 1,
        sizeof(oldSecondDisplacement));

    unsigned char* regionStart =
        g_firstCallsite < g_secondCallsite ? g_firstCallsite : g_secondCallsite;
    unsigned char* regionEnd =
        g_firstCallsite < g_secondCallsite
            ? g_secondCallsite + 5
            : g_firstCallsite + 5;
    const SIZE_T regionSize = static_cast<SIZE_T>(regionEnd - regionStart);

    DWORD oldProtect = 0;
    if (!VirtualProtect(
            regionStart,
            regionSize,
            PAGE_EXECUTE_READWRITE,
            &oldProtect))
    {
        AppendLog(
            "[Input][LowLatency] ERROR: VirtualProtect failed; no change made.\n");
        return false;
    }

    // F10 hot-apply runs from DP's render/presentation path after the current
    // game update has passed these main-tick callsites. Change only the rel32
    // operands; E8 opcodes and ECX setup remain byte-for-byte native.
    std::memcpy(
        g_firstCallsite + 1,
        &firstDisplacement,
        sizeof(firstDisplacement));
    std::memcpy(
        g_secondCallsite + 1,
        &secondDisplacement,
        sizeof(secondDisplacement));
    FlushInstructionCache(GetCurrentProcess(), regionStart, regionSize);

    uintptr_t verifiedFirst = 0;
    uintptr_t verifiedSecond = 0;
    const bool verified =
        ReadCurrentPair(verifiedFirst, verifiedSecond) &&
        PairMatches(enabled, verifiedFirst, verifiedSecond);

    if (!verified)
    {
        // Transactional rollback while the page is still writable.
        std::memcpy(
            g_firstCallsite + 1,
            &oldFirstDisplacement,
            sizeof(oldFirstDisplacement));
        std::memcpy(
            g_secondCallsite + 1,
            &oldSecondDisplacement,
            sizeof(oldSecondDisplacement));
        FlushInstructionCache(GetCurrentProcess(), regionStart, regionSize);
        AppendLog(
            "[Input][LowLatency] ERROR: post-write verification failed; previous CALL pair restored.\n");
    }

    DWORD ignoredProtect = 0;
    if (!VirtualProtect(
            regionStart,
            regionSize,
            oldProtect,
            &ignoredProtect))
    {
        AppendLog(
            "[Input][LowLatency] WARNING: could not restore executable-page protection after hot-apply.\n");
    }

    return verified;
}
} // namespace

bool PrepareLowLatencyInputOrdering()
{
    if (g_available.load(std::memory_order_acquire))
        return true;

    if (!InitializeMainExeInfo())
    {
        AppendLog(
            "[Input][LowLatency] ERROR: DP.exe information unavailable; hot-apply unavailable.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (!build)
    {
        AppendLog(
            "[Input][LowLatency] ERROR: unsupported DP.exe build; hot-apply unavailable.\n");
        return false;
    }

    const auto& input = build->input;
    auto* firstCallsite = reinterpret_cast<unsigned char*>(
        g_mainExeBase + input.mainTickInputCommitCallsiteRva);
    auto* secondCallsite = reinterpret_cast<unsigned char*>(
        g_mainExeBase + input.mainTickInputPollCallsiteRva);
    const uintptr_t commitTarget = g_mainExeBase + input.inputCommitRva;
    const uintptr_t pollTarget = g_mainExeBase + input.inputPollWrapperRva;

    if (!IsRangeInsideMainExe(reinterpret_cast<uintptr_t>(firstCallsite), 5) ||
        !IsRangeInsideMainExe(reinterpret_cast<uintptr_t>(secondCallsite), 5) ||
        !IsRangeInsideMainExe(commitTarget, 1) ||
        !IsRangeInsideMainExe(pollTarget, 1))
    {
        AppendLog(
            "[Input][LowLatency] ERROR: build-profile address outside DP.exe; hot-apply unavailable.\n");
        return false;
    }

    uintptr_t observedFirst = 0;
    uintptr_t observedSecond = 0;
    if (!DecodeNearCallTarget(firstCallsite, observedFirst) ||
        !DecodeNearCallTarget(secondCallsite, observedSecond) ||
        observedFirst != commitTarget ||
        observedSecond != pollTarget)
    {
        char text[384] = {};
        sprintf_s(
            text,
            "[Input][LowLatency] ERROR: vanilla main-tick CALL verification failed on %s; hot-apply unavailable. first=DP.exe+0x%08lX expected +0x%08lX, second=+0x%08lX expected +0x%08lX.\n",
            build->name,
            observedFirst >= g_mainExeBase
                ? static_cast<unsigned long>(observedFirst - g_mainExeBase)
                : 0ul,
            static_cast<unsigned long>(input.inputCommitRva),
            observedSecond >= g_mainExeBase
                ? static_cast<unsigned long>(observedSecond - g_mainExeBase)
                : 0ul,
            static_cast<unsigned long>(input.inputPollWrapperRva));
        AppendLog(text);
        return false;
    }

    g_firstCallsite = firstCallsite;
    g_secondCallsite = secondCallsite;
    g_commitTarget = commitTarget;
    g_pollTarget = pollTarget;
    g_buildName = build->name;
    g_firstCallsiteRva = input.mainTickInputCommitCallsiteRva;
    g_secondCallsiteRva = input.mainTickInputPollCallsiteRva;
    g_active.store(false, std::memory_order_release);
    g_available.store(true, std::memory_order_release);

    char text[352] = {};
    sprintf_s(
        text,
        "[Input][LowLatency] Reversible main-tick ordering ready on %s: vanilla commit@DP.exe+0x%08lX -> poll@+0x%08lX; runtime hot-apply available.\n",
        build->name,
        static_cast<unsigned long>(g_firstCallsiteRva),
        static_cast<unsigned long>(g_secondCallsiteRva));
    AppendLog(text);
    return true;
}

bool ApplyLowLatencyInputOrdering(bool enabled)
{
    if (!g_available.load(std::memory_order_acquire))
    {
        g_config.lowLatencyInput = false;
        g_active.store(false, std::memory_order_release);
        return !enabled;
    }

    uintptr_t currentFirst = 0;
    uintptr_t currentSecond = 0;
    if (ReadCurrentPair(currentFirst, currentSecond) &&
        PairMatches(enabled, currentFirst, currentSecond))
    {
        g_active.store(enabled, std::memory_order_release);
        g_config.lowLatencyInput = enabled;
        return true;
    }

    if (!WritePair(enabled))
    {
        uintptr_t firstTarget = 0;
        uintptr_t secondTarget = 0;
        const bool active =
            ReadCurrentPair(firstTarget, secondTarget) &&
            PairMatches(true, firstTarget, secondTarget);
        g_active.store(active, std::memory_order_release);
        g_config.lowLatencyInput = active;
        return false;
    }

    g_active.store(enabled, std::memory_order_release);
    g_config.lowLatencyInput = enabled;

    char text[320] = {};
    sprintf_s(
        text,
        "[Input][LowLatency] Same-frame input ordering %s on %s: %s.\n",
        enabled ? "ENABLED" : "DISABLED",
        g_buildName ? g_buildName : "supported build",
        enabled
            ? "main tick now polls first, then commits the fresh snapshot"
            : "exact vanilla commit-then-poll ordering restored");
    AppendLog(text);
    return true;
}

bool IsLowLatencyInputOrderingAvailable()
{
    return g_available.load(std::memory_order_acquire);
}

bool IsLowLatencyInputOrderingActive()
{
    return IsLowLatencyInputOrderingAvailable() &&
           g_active.load(std::memory_order_acquire);
}
