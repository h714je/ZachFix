#include "zachfix/gameplay/aim_fpu_fix.h"

#include <Windows.h>
#include <MinHook.h>
#include <float.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "zachfix/core/config.h"
#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

namespace
{
using AimMode2HandlerFn = void (__cdecl*)();

AimMode2HandlerFn g_originalAimMode2Handler = nullptr;
void* g_aimMode2Target = nullptr;
const char* g_buildName = nullptr;
uintptr_t g_aimMode2Rva = 0;
std::atomic_bool g_available{ false };
std::atomic_bool g_active{ false };

void __cdecl HookAimMode2Handler()
{
    unsigned int entryControlWord = 0;
    const bool haveEntryControlWord =
        _controlfp_s(&entryControlWord, 0, 0) == 0;

    // The native mode-2 handoff clamps a float32 accumulator, then compares it
    // for exact equality against a still-live x87 limit. Extended precision can
    // make those numerically equivalent values compare unequal. PC24 makes the
    // retained x87 value obey the same float32 precision as the stored clamp.
    unsigned int ignored = 0;
    _controlfp_s(&ignored, _PC_24, _MCW_PC);

    g_originalAimMode2Handler();

    // Keep the fix local to aim. Restore only the precision-control field;
    // preserve any exception-mask or rounding-mode changes made by native code.
    if (haveEntryControlWord)
    {
        _controlfp_s(
            &ignored,
            entryControlWord & _MCW_PC,
            _MCW_PC);
    }
}

uintptr_t ResolveAimMode2HandlerRva(const DpBuildProfile* build)
{
    if (build == nullptr)
        return 0;

    switch (build->build)
    {
    case DpBuild::Steam101b:
        return 0x0013B8B0; // FUN_0053B8B0
    case DpBuild::Gog101b:
        return 0x0013B980; // FUN_0053B980
    default:
        return 0;
    }
}

bool ValidateAimMode2Prologue(const unsigned char* target)
{
    // Both supported builds begin identically:
    //   sub esp,24h
    //   push esi
    //   mov esi,[00BE1EA4h]
    static constexpr unsigned char kExpected[] = {
        0x83, 0xEC, 0x24,
        0x56,
        0x8B, 0x35, 0xA4, 0x1E, 0xBE, 0x00,
    };

    return target != nullptr &&
        std::memcmp(target, kExpected, sizeof(kExpected)) == 0;
}
} // namespace

bool PrepareAimFpuPrecisionFix()
{
    if (g_available.load(std::memory_order_acquire))
        return true;

    const DpBuildProfile* build = GetDpBuildProfile();
    const uintptr_t rva = ResolveAimMode2HandlerRva(build);

    if (build == nullptr || rva == 0 || g_mainExeBase == 0)
    {
        AppendLog(
            "[Aim][Experimental] x87 PC24 precision guard unavailable on unsupported build.\n");
        return false;
    }

    auto* target = reinterpret_cast<unsigned char*>(g_mainExeBase + rva);
    if (!ValidateAimMode2Prologue(target))
    {
        AppendLog(
            "[Aim][Experimental] x87 PC24 precision guard signature mismatch; feature unavailable.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookAimMode2Handler),
        reinterpret_cast<void**>(&g_originalAimMode2Handler));

    if (createStatus != MH_OK)
    {
        char text[176] = {};
        sprintf_s(
            text,
            "[Aim][Experimental] x87 PC24 precision guard MH_CreateHook failed: %d.\n",
            static_cast<int>(createStatus));
        AppendLog(text);
        g_originalAimMode2Handler = nullptr;
        return false;
    }

    g_aimMode2Target = target;
    g_buildName = build->name;
    g_aimMode2Rva = rva;
    g_active.store(false, std::memory_order_release);
    g_available.store(true, std::memory_order_release);

    char text[288] = {};
    sprintf_s(
        text,
        "[Aim][Experimental] x87 PC24 precision guard ready on %s at DP.exe+0x%08lX; disabled until requested by config/Apply.\n",
        g_buildName,
        static_cast<unsigned long>(g_aimMode2Rva));
    AppendLog(text);
    return true;
}

bool ApplyAimFpuPrecisionFix(bool enabled)
{
    if (!g_available.load(std::memory_order_acquire) || g_aimMode2Target == nullptr)
    {
        g_active.store(false, std::memory_order_release);
        g_config.experimentalAimFpuPrecisionFix = false;
        return !enabled;
    }

    const bool wasActive = g_active.load(std::memory_order_acquire);
    if (wasActive == enabled)
    {
        g_config.experimentalAimFpuPrecisionFix = enabled;
        return true;
    }

    const MH_STATUS status = enabled
        ? MH_EnableHook(g_aimMode2Target)
        : MH_DisableHook(g_aimMode2Target);
    const bool statusOk = enabled
        ? (status == MH_OK || status == MH_ERROR_ENABLED)
        : (status == MH_OK || status == MH_ERROR_DISABLED);

    if (!statusOk)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Aim][Experimental] x87 PC24 precision guard MH_%sHook failed: %d; previous state retained.\n",
            enabled ? "Enable" : "Disable",
            static_cast<int>(status));
        AppendLog(text);
        g_config.experimentalAimFpuPrecisionFix = wasActive;
        return false;
    }

    g_active.store(enabled, std::memory_order_release);
    g_config.experimentalAimFpuPrecisionFix = enabled;

    char text[320] = {};
    sprintf_s(
        text,
        "[Aim][Experimental] x87 PC24 precision guard %s on %s at DP.exe+0x%08lX; caller precision is restored after each mode-2 update.\n",
        enabled ? "ENABLED" : "DISABLED",
        g_buildName ? g_buildName : "supported build",
        static_cast<unsigned long>(g_aimMode2Rva));
    AppendLog(text);
    return true;
}

bool IsAimFpuPrecisionFixAvailable()
{
    return g_available.load(std::memory_order_acquire);
}

bool IsAimFpuPrecisionFixActive()
{
    return IsAimFpuPrecisionFixAvailable() &&
           g_active.load(std::memory_order_acquire);
}
