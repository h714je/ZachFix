#include "world_streaming.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <atomic>
#include <mutex>

// -----------------------------------------------------------------------------
// World detail range
// -----------------------------------------------------------------------------
//
// A build-specific DP.exe routine classifies an active mode-2 streaming cell.
// Original result:
//   0 -> cell belongs to the inner 2x2 set, use table row {0..5} (full detail)
//   1 -> cell is only in the outer 4x4 set, use table row {6,7} (reduced detail)
//
// The hook preserves the game's active-cell footprint and lifetime logic.
// It only promotes cells that are already present in manager+0x2815C[16].
// This avoids expanding fixed-size streaming arrays or inventing new cells.
using WorldCellDetailClassifyFn = int (__thiscall*)(
    void* manager,
    int cellId
);

static WorldCellDetailClassifyFn g_originalWorldCellDetailClassify = nullptr;
std::atomic_uint g_worldDetailScale{ 1 };
std::mutex g_worldDetailPatchMutex;
std::mutex g_worldObjectActivationPatchMutex;

// Raw 4-byte storage intentionally read by DP through `FLD dword ptr [absolute]`.
// The instruction operand is redirected once; hot apply then changes only this
// aligned value instead of rewriting executable code on every Apply.
alignas(4) volatile LONG g_worldObjectActivationThresholdBits = 0;
bool g_worldObjectActivationOperandPatched = false;

static int __fastcall HookWorldCellDetailClassify(
    void* manager,
    void*,
    int cellId)
{
    const int original =
        g_originalWorldCellDetailClassify(manager, cellId);

    if (g_worldDetailScale.load(std::memory_order_acquire) < 2 ||
        original == 0 ||
        manager == nullptr ||
        cellId < 0)
    {
        return original;
    }

    // manager+0x2815C is the current 16-entry outer mode-2 cell set.
    // The original 4-entry high-detail/core set lives at manager+0x2819C.
    const int* outerCells =
        reinterpret_cast<const int*>(
            reinterpret_cast<const unsigned char*>(manager) + 0x2815C);

    for (int i = 0; i < 16; ++i)
    {
        if (outerCells[i] == cellId)
        {
            // Promote this already-active outer cell to the same content row
            // used by the original inner/core cells.
            return 0;
        }
    }

    return original;
}

bool PrepareWorldCellDetailClassifyHook()
{
    if (!InitializeMainExeInfo())
    {
        AppendLog(
            "[World] ERROR: DP.exe info unavailable; "
            "high-detail streaming extension disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog(
            "[World] ERROR: Unsupported DP.exe build; "
            "high-detail streaming extension disabled.\n");
        return false;
    }

    static const unsigned char signature[] = {
        0x83, 0xB9, 0xCC, 0x83, 0x02, 0x00, 0x00,
        0x75, 0x5F,
        0xA1, 0x74, 0x60, 0x8A, 0x00
    };

    unsigned char* target =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + build->worldCellDetailClassifyRva);

    if (memcmp(target, signature, sizeof(signature)) != 0)
    {
        AppendLog(
            "[World] ERROR: Cell-detail classifier signature mismatch; "
            "extension disabled.\n");
        return false;
    }

    const MH_STATUS createStatus =
        MH_CreateHook(
            target,
            reinterpret_cast<void*>(&HookWorldCellDetailClassify),
            reinterpret_cast<void**>(&g_originalWorldCellDetailClassify));

    if (createStatus != MH_OK &&
        createStatus != MH_ERROR_ALREADY_CREATED)
    {
        AppendLog(
            "[World] ERROR: MH_CreateHook failed for cell-detail classifier.\n");
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(target);

    if (enableStatus != MH_OK &&
        enableStatus != MH_ERROR_ENABLED)
    {
        AppendLog(
            "[World] ERROR: MH_EnableHook failed for cell-detail classifier.\n");
        return false;
    }

    AppendLog(
        "[World] Runtime world-detail hook prepared (Original/Extended switch available).\n");

    return true;
}



// Incremental streaming path fix.
//
// The bulk/initial path calls the build-specific classifier hooked above.
// While moving through the world, a build-specific incremental state
// machine duplicates the inner-vs-outer classification inline. The relevant
// outer branch writes 1 to [esi+0x287B4] for the reduced row, while the inner
// branch writes 0 for the full-detail row.
//
// The incremental fix changes only the immediate value in the outer branch from 1 to 0.
// The active 4x4 footprint, fixed arrays, cell IDs and streaming lifetime
// remain untouched.
bool ApplyWorldDetailDistanceScale(unsigned int scale)
{
    if (scale < 1 || scale > 2)
        return false;

    std::lock_guard<std::mutex> lock(g_worldDetailPatchMutex);

    if (!InitializeMainExeInfo())
    {
        AppendLog("[World] ERROR: DP.exe info unavailable; runtime detail switch failed.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[World] ERROR: Unsupported DP.exe build; runtime detail switch failed.\n");
        return false;
    }

    static const unsigned char prefix[] = {
        0xC7, 0x86, 0xB4, 0x87, 0x02, 0x00
    };

    unsigned char* instruction =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + build->worldIncrementalOuterClassifyRva);

    if (memcmp(instruction, prefix, sizeof(prefix)) != 0)
    {
        AppendLog("[World] ERROR: Incremental detail instruction signature mismatch.\n");
        return false;
    }

    // The immediate is a DWORD, but valid values are only 0 and 1, so the
    // upper three bytes must stay zero. Change only the low byte. Besides being
    // sufficient, this avoids an unaligned 32-bit hot write if the streaming
    // state machine happens to execute on another thread during an F10 apply.
    const unsigned char desiredImmediate = scale >= 2 ? 0u : 1u;
    const unsigned char currentImmediate = instruction[6];
    if (instruction[7] != 0 || instruction[8] != 0 || instruction[9] != 0 ||
        (currentImmediate != 0u && currentImmediate != 1u))
    {
        AppendLog("[World] ERROR: Incremental detail immediate has an unexpected value.\n");
        return false;
    }

    if (currentImmediate != desiredImmediate)
    {
        DWORD oldProtect = 0;
        if (!VirtualProtect(
                instruction + 6,
                sizeof(unsigned char),
                PAGE_EXECUTE_READWRITE,
                &oldProtect))
        {
            AppendLog("[World] ERROR: VirtualProtect failed for runtime detail switch.\n");
            return false;
        }

        instruction[6] = desiredImmediate;
        FlushInstructionCache(GetCurrentProcess(), instruction + 6, 1);

        DWORD ignored = 0;
        if (!VirtualProtect(
                instruction + 6, sizeof(unsigned char), oldProtect, &ignored))
        {
            AppendLog(
                "[World] WARNING: Could not restore incremental detail code protection after hot apply.\n");
        }
    }

    g_worldDetailScale.store(scale, std::memory_order_release);
    g_config.highDetailDistanceScale = scale;

    char text[192] = {};
    sprintf_s(
        text,
        "[World] Runtime detail distance set to %u (%s). Changes appear on subsequent cell transitions.\n",
        scale,
        scale >= 2 ? "extended 4x4" : "original 2x2");
    AppendLog(text);

    return true;
}


// Native per-object activation distance.
//
// DP's active-list builder loads one squared-distance threshold from a shared
// constant. The shared constant has other users, so ZachFix leaves it intact
// and redirects only this one FLD operand to private 4-byte storage. The rest
// of the game's filtering, spatial registration and rendering pipeline remains
// unchanged.
bool ApplyWorldObjectActivationDistanceScale(unsigned int scale)
{
    if (scale < 1 || scale > 2)
        return false;

    std::lock_guard<std::mutex> lock(g_worldObjectActivationPatchMutex);

    if (!InitializeMainExeInfo())
    {
        AppendLog("[World] ERROR: DP.exe info unavailable; object activation distance switch failed.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || build->worldObjectActivationThresholdLoadRva == 0)
    {
        AppendLog("[World] ERROR: Unsupported DP.exe build; object activation distance switch failed.\n");
        return false;
    }

    unsigned char* instruction =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + build->worldObjectActivationThresholdLoadRva);

    // Steam: D9 05 B4 3E 77 00  D9 9D 20 D9 FF FF
    // GOG:   D9 05 A4 3E 77 00  D9 9D 20 D9 FF FF
    static const unsigned char prefix[] = { 0xD9, 0x05 };
    static const unsigned char suffix[] = { 0xD9, 0x9D, 0x20, 0xD9, 0xFF, 0xFF };
    if (memcmp(instruction, prefix, sizeof(prefix)) != 0 ||
        memcmp(instruction + 6, suffix, sizeof(suffix)) != 0)
    {
        AppendLog("[World] ERROR: Object activation threshold load signature mismatch.\n");
        return false;
    }

    const float radius = 1000.0f * static_cast<float>(scale);
    const float thresholdSq = radius * radius;
    LONG thresholdBits = 0;
    static_assert(sizeof(thresholdBits) == sizeof(thresholdSq));
    memcpy(&thresholdBits, &thresholdSq, sizeof(thresholdBits));

    // Publish a valid threshold before the one-time operand redirect so a
    // concurrent native update can never observe the private storage as zero.
    InterlockedExchange(&g_worldObjectActivationThresholdBits, thresholdBits);

    if (!g_worldObjectActivationOperandPatched)
    {
        const uintptr_t originalAbsolute =
            *reinterpret_cast<const uint32_t*>(instruction + 2);
        const uintptr_t expectedOriginal =
            build->build == DpBuild::Steam101b ? 0x00773EB4u : 0x00773EA4u;

        if (originalAbsolute != expectedOriginal)
        {
            AppendLog("[World] ERROR: Object activation threshold source address mismatch.\n");
            return false;
        }

        const uint32_t replacementAbsolute =
            static_cast<uint32_t>(
                reinterpret_cast<uintptr_t>(&g_worldObjectActivationThresholdBits));

        DWORD oldProtect = 0;
        if (!VirtualProtect(
                instruction + 2,
                sizeof(replacementAbsolute),
                PAGE_EXECUTE_READWRITE,
                &oldProtect))
        {
            AppendLog("[World] ERROR: VirtualProtect failed for object activation threshold redirect.\n");
            return false;
        }

        memcpy(instruction + 2, &replacementAbsolute, sizeof(replacementAbsolute));
        FlushInstructionCache(GetCurrentProcess(), instruction, 6);

        DWORD ignored = 0;
        if (!VirtualProtect(
                instruction + 2,
                sizeof(replacementAbsolute),
                oldProtect,
                &ignored))
        {
            AppendLog("[World] WARNING: Could not restore code protection after object activation threshold redirect.\n");
        }

        g_worldObjectActivationOperandPatched = true;
    }

    g_config.objectActivationDistanceScale = scale;

    char text[224] = {};
    sprintf_s(
        text,
        "[World] Native object activation radius set to %.0f units (thresholdSq=%.0f, scale=%u).\n",
        static_cast<double>(radius),
        static_cast<double>(thresholdSq),
        scale);
    AppendLog(text);
    return true;
}
