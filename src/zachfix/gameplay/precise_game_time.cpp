#include "zachfix/gameplay/precise_game_time.h"

#include <Windows.h>
#include <MinHook.h>
#include <float.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

namespace
{
static_assert(sizeof(void*) == 4, "Precise game-time fix requires the 32-bit ZachFix build.");

// FUN_00401F50 returns an integer microsecond timestamp in EDX:EAX after doing
// the QPF/QPC conversion through x87. The address is identical in both builds.
using NativeMicrosecondClockFn = unsigned long long (__cdecl*)();

// FUN_00701040 (Steam) / FUN_00700FA0 (GOG) returns seconds in x87 ST0. MSVC
// x86 uses the same ST0 return convention for double, and every known caller
// immediately consumes/stores the result as a double-duration source.
using NativeGameTimeSecondsFn = double (__cdecl*)(int initializeFrequency);

NativeMicrosecondClockFn g_originalMicrosecondClock = nullptr;
NativeGameTimeSecondsFn g_originalGameTimeSeconds = nullptr;
void* g_microsecondClockTarget = nullptr;
void* g_gameTimeSecondsTarget = nullptr;
const char* g_buildName = nullptr;
uintptr_t g_microsecondClockRva = 0;
uintptr_t g_gameTimeSecondsRva = 0;
std::atomic_bool g_active{ false };
bool g_microsecondHookCreated = false;
bool g_gameTimeSecondsHookCreated = false;

struct PreciseGameTimeAddresses
{
    uintptr_t microsecondClockRva = 0;
    uintptr_t gameTimeSecondsRva = 0;
};

PreciseGameTimeAddresses ResolveAddresses(const DpBuildProfile* build)
{
    PreciseGameTimeAddresses result{};
    if (build == nullptr)
        return result;

    switch (build->build)
    {
    case DpBuild::Steam101b:
        result.microsecondClockRva = 0x00001F50; // FUN_00401F50
        result.gameTimeSecondsRva = 0x00301040;  // FUN_00701040
        break;
    case DpBuild::Gog101b:
        result.microsecondClockRva = 0x00001F50; // FUN_00401F50
        result.gameTimeSecondsRva = 0x00300FA0;  // FUN_00700FA0
        break;
    default:
        break;
    }

    return result;
}

bool ValidateMicrosecondClock(const unsigned char* target)
{
    // Both supported builds begin identically:
    //   sub esp,10h
    //   lea eax,[esp]
    //   push eax
    //   call [QueryPerformanceFrequency]
    //   lea ecx,[esp+8]
    //   push ecx
    //   call [QueryPerformanceCounter]
    static constexpr unsigned char kExpected[] = {
        0x83, 0xEC, 0x10,
        0x8D, 0x04, 0x24,
        0x50,
        0xFF, 0x15, 0x78, 0xE0, 0x76, 0x00,
        0x8D, 0x4C, 0x24, 0x08,
        0x51,
        0xFF, 0x15, 0x80, 0xE0, 0x76, 0x00,
    };

    return target != nullptr &&
        std::memcmp(target, kExpected, sizeof(kExpected)) == 0;
}

bool ValidateGameTimeSeconds(const unsigned char* target)
{
    // Steam FUN_00701040 and GOG FUN_00700FA0 are byte-identical. The helper
    // either initializes DP's QPF global or computes absolute QPC / QPF in x87.
    static constexpr unsigned char kExpected[] = {
        0x55,
        0x8B, 0xEC,
        0x83, 0xEC, 0x08,
        0x83, 0x7D, 0x08, 0x00,
        0x74, 0x11,
        0x68, 0xF0, 0xFF, 0x4A, 0x01,
        0xFF, 0x15, 0x78, 0xE0, 0x76, 0x00,
        0xD9, 0xEE,
        0xEB, 0x17,
        0xEB, 0x15,
        0x8D, 0x45, 0xF8,
        0x50,
        0xFF, 0x15, 0x80, 0xE0, 0x76, 0x00,
        0xDF, 0x6D, 0xF8,
        0xDF, 0x2D, 0xF0, 0xFF, 0x4A, 0x01,
        0xDE, 0xF9,
        0x8B, 0xE5,
        0x5D,
        0xC3,
    };

    return target != nullptr &&
        std::memcmp(target, kExpected, sizeof(kExpected)) == 0;
}

class ScopedPc53
{
public:
    ScopedPc53()
    {
        if (_controlfp_s(&entryControlWord_, 0, 0) != 0)
            return;

        unsigned int ignored = 0;
        if (_controlfp_s(&ignored, _PC_53, _MCW_PC) == 0)
            armed_ = true;
    }

    ~ScopedPc53()
    {
        if (!armed_)
            return;

        // Restore only the precision-control field. If native code changes
        // exception masks or rounding mode, retain those changes exactly as the
        // unhooked function would have done.
        unsigned int ignored = 0;
        _controlfp_s(
            &ignored,
            entryControlWord_ & _MCW_PC,
            _MCW_PC);
    }

    ScopedPc53(const ScopedPc53&) = delete;
    ScopedPc53& operator=(const ScopedPc53&) = delete;

private:
    unsigned int entryControlWord_ = 0;
    bool armed_ = false;
};

unsigned long long __cdecl HookMicrosecondClock()
{
    ScopedPc53 precision;
    return g_originalMicrosecondClock();
}

double __cdecl HookGameTimeSeconds(int initializeFrequency)
{
    ScopedPc53 precision;
    return g_originalGameTimeSeconds(initializeFrequency);
}

void RollBackCreatedHooks()
{
    if (g_gameTimeSecondsHookCreated && g_gameTimeSecondsTarget != nullptr)
    {
        MH_DisableHook(g_gameTimeSecondsTarget);
        MH_RemoveHook(g_gameTimeSecondsTarget);
    }

    if (g_microsecondHookCreated && g_microsecondClockTarget != nullptr)
    {
        MH_DisableHook(g_microsecondClockTarget);
        MH_RemoveHook(g_microsecondClockTarget);
    }

    g_originalGameTimeSeconds = nullptr;
    g_originalMicrosecondClock = nullptr;
    g_gameTimeSecondsTarget = nullptr;
    g_microsecondClockTarget = nullptr;
    g_gameTimeSecondsHookCreated = false;
    g_microsecondHookCreated = false;
    g_active.store(false, std::memory_order_release);
}
} // namespace

bool InstallPreciseGameTimeFix()
{
    if (g_active.load(std::memory_order_acquire))
        return true;

    const DpBuildProfile* build = GetDpBuildProfile();
    const PreciseGameTimeAddresses addresses = ResolveAddresses(build);

    if (build == nullptr ||
        g_mainExeBase == 0 ||
        addresses.microsecondClockRva == 0 ||
        addresses.gameTimeSecondsRva == 0)
    {
        AppendLog(
            "[Timing] Precise game-time fix unavailable on unsupported build.\n");
        return false;
    }

    auto* microsecondTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + addresses.microsecondClockRva);
    auto* secondsTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + addresses.gameTimeSecondsRva);

    if (!ValidateMicrosecondClock(microsecondTarget) ||
        !ValidateGameTimeSeconds(secondsTarget))
    {
        AppendLog(
            "[Timing] Precise game-time fix signature mismatch; native timing left untouched.\n");
        return false;
    }

    g_microsecondClockTarget = microsecondTarget;
    g_gameTimeSecondsTarget = secondsTarget;

    MH_STATUS status = MH_CreateHook(
        g_microsecondClockTarget,
        reinterpret_cast<void*>(&HookMicrosecondClock),
        reinterpret_cast<void**>(&g_originalMicrosecondClock));
    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Timing] Precise game-time microsecond hook creation failed: %d.\n",
            static_cast<int>(status));
        AppendLog(text);
        RollBackCreatedHooks();
        return false;
    }
    g_microsecondHookCreated = true;

    status = MH_CreateHook(
        g_gameTimeSecondsTarget,
        reinterpret_cast<void*>(&HookGameTimeSeconds),
        reinterpret_cast<void**>(&g_originalGameTimeSeconds));
    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Timing] Precise game-time master-clock hook creation failed: %d.\n",
            static_cast<int>(status));
        AppendLog(text);
        RollBackCreatedHooks();
        return false;
    }
    g_gameTimeSecondsHookCreated = true;

    status = MH_EnableHook(g_microsecondClockTarget);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Timing] Precise game-time microsecond hook enable failed: %d.\n",
            static_cast<int>(status));
        AppendLog(text);
        RollBackCreatedHooks();
        return false;
    }

    status = MH_EnableHook(g_gameTimeSecondsTarget);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Timing] Precise game-time master-clock hook enable failed: %d.\n",
            static_cast<int>(status));
        AppendLog(text);
        RollBackCreatedHooks();
        return false;
    }

    g_buildName = build->name;
    g_microsecondClockRva = addresses.microsecondClockRva;
    g_gameTimeSecondsRva = addresses.gameTimeSecondsRva;
    g_active.store(true, std::memory_order_release);

    char text[512] = {};
    sprintf_s(
        text,
        "[Timing] Vanilla long-uptime QPC precision fix active on %s: "
        "DP.exe+0x%08lX and DP.exe+0x%08lX run in scoped x87 PC53; "
        "the caller's precision-control bits are restored after each call.\n",
        g_buildName,
        static_cast<unsigned long>(g_microsecondClockRva),
        static_cast<unsigned long>(g_gameTimeSecondsRva));
    AppendLog(text);
    return true;
}

bool IsPreciseGameTimeFixActive()
{
    return g_active.load(std::memory_order_acquire);
}
