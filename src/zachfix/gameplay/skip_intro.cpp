#include "zachfix/gameplay/skip_intro.h"

#include <Windows.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

namespace
{
struct SkipIntroAddress
{
    uintptr_t instructionRva = 0;
};

std::atomic_bool g_available{ false };
std::atomic_bool g_active{ false };

SkipIntroAddress ResolveAddress(const DpBuildProfile* build)
{
    if (build == nullptr)
        return {};

    switch (build->build)
    {
    case DpBuild::Steam101b:
        // 00643F2D: MOV dword ptr [014736D8],000000B3
        return { 0x00243F2Du };
    case DpBuild::Gog101b:
        // 00643E7D: MOV dword ptr [014736D8],000000B3
        return { 0x00243E7Du };
    default:
        return {};
    }
}

bool ValidateSite(const unsigned char* site, unsigned char& currentStage)
{
    if (site == nullptr)
        return false;

    // C7 05 D8 36 47 01 xx 00 00 00
    //     MOV dword ptr [014736D8], xx
    // C7 05 D4 36 47 01 B2 00 00 00
    //     MOV dword ptr [014736D4], B2
    //
    // xx is either vanilla 0xB3 or 0x00 when the long-standing manual community
    // skip-intro edit has already been applied to the executable on disk.
    static constexpr unsigned char kPrefix[] = {
        0xC7, 0x05, 0xD8, 0x36, 0x47, 0x01,
    };
    static constexpr unsigned char kSuffix[] = {
        0x00, 0x00, 0x00,
        0xC7, 0x05, 0xD4, 0x36, 0x47, 0x01, 0xB2, 0x00, 0x00, 0x00,
    };

    if (std::memcmp(site, kPrefix, sizeof(kPrefix)) != 0 ||
        std::memcmp(site + 7, kSuffix, sizeof(kSuffix)) != 0)
    {
        return false;
    }

    currentStage = site[6];
    return currentStage == 0xB3u || currentStage == 0x00u;
}

bool WriteStageByte(unsigned char* target, unsigned char value)
{
    DWORD oldProtect = 0;
    if (!VirtualProtect(target, 1, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    *target = value;

    DWORD ignoredProtect = 0;
    // The byte write has already committed. A protection-restore failure must
    // not make the caller believe vanilla state is still present.
    VirtualProtect(target, 1, oldProtect, &ignoredProtect);
    FlushInstructionCache(GetCurrentProcess(), target, 1);
    return true;
}
} // namespace

bool InstallSkipIntroPatch(bool enabled)
{
    g_available.store(false, std::memory_order_release);
    g_active.store(false, std::memory_order_release);

    const DpBuildProfile* build = GetDpBuildProfile();
    const SkipIntroAddress address = ResolveAddress(build);
    if (build == nullptr || g_mainExeBase == 0 || address.instructionRva == 0)
    {
        AppendLog("[Startup] Skip Intro unavailable on unsupported DP.exe build.\n");
        return false;
    }

    auto* site = reinterpret_cast<unsigned char*>(
        g_mainExeBase + address.instructionRva);

    unsigned char currentStage = 0;
    if (!ValidateSite(site, currentStage))
    {
        AppendLog(
            "[Startup] Skip Intro signature mismatch; vanilla startup flow left untouched.\n");
        return false;
    }

    g_available.store(true, std::memory_order_release);

    if (!enabled)
    {
        // Do not restore 0xB3 when another tool/manual edit already changed the
        // executable. ZachFix owns only changes it makes in this process.
        if (currentStage == 0x00u)
        {
            AppendLog(
                "[Startup] Skip Intro disabled in ZachFix, but DP.exe is already externally patched to skip the intro; leaving it unchanged.\n");
            g_active.store(true, std::memory_order_release);
        }
        else
        {
            AppendLog("[Startup] Skip Intro disabled; vanilla startup flow retained.\n");
        }
        return true;
    }

    if (currentStage == 0xB3u)
    {
        // Immediate dword begins at instruction+6. Only its low byte differs.
        if (!WriteStageByte(site + 6, 0x00u))
        {
            AppendLog(
                "[Startup] Skip Intro patch write failed; vanilla startup flow retained.\n");
            return false;
        }
    }

    g_active.store(true, std::memory_order_release);

    char text[320] = {};
    sprintf_s(
        text,
        "[Startup] Skip Intro active on %s at DP.exe+0x%08lX%s.\n",
        build->name,
        static_cast<unsigned long>(address.instructionRva + 6u),
        currentStage == 0x00u ? " (startup state was already patched to 0)" : "");
    AppendLog(text);
    return true;
}

bool IsSkipIntroPatchAvailable()
{
    return g_available.load(std::memory_order_acquire);
}

bool IsSkipIntroPatchActive()
{
    return g_active.load(std::memory_order_acquire);
}
