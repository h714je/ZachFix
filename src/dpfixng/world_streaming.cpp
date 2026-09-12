#include "world_streaming.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>
#include <cstring>
#include <cstdint>
#include <cstdio>

// -----------------------------------------------------------------------------
// World detail range
// -----------------------------------------------------------------------------
//
// DP.exe+0x001E6D40 classifies an active mode-2 streaming cell.
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

static constexpr uintptr_t kWorldCellDetailClassifyRva = 0x001E6D40;

static int __fastcall HookWorldCellDetailClassify(
    void* manager,
    void*,
    int cellId)
{
    const int original =
        g_originalWorldCellDetailClassify(manager, cellId);

    if (g_config.highDetailDistanceScale < 2 ||
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

    if (g_mainExeSize != 0x010B5000 ||
        g_mainExeTimeDateStamp != 0x529721DC)
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
            g_mainExeBase + kWorldCellDetailClassifyRva);

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
// The bulk/initial path calls DP.exe+0x001E6D40, which is hooked above.
// While moving through the world, DP.exe+0x001EBFC0 runs an incremental
// state machine and duplicates the inner-vs-outer classification inline:
//
//   0x5EC370..0x5EC37D  search current cell in manager+0x2819C[4]
//   0x5EC37F            mov [esi+0x287B4], 1   ; outer / reduced row
//   0x5EC39E            mov [esi+0x287B4], 0   ; inner / full row
//
// The incremental fix changes only the immediate value in the outer branch from 1 to 0.
// The active 4x4 footprint, fixed arrays, cell IDs and streaming lifetime
// remain untouched.
static constexpr uintptr_t kWorldIncrementalOuterClassifyRva = 0x001EC37F;

bool ApplyWorldDetailDistanceScale(unsigned int scale)
{
    if (scale < 1 || scale > 2)
        return false;

    if (!InitializeMainExeInfo())
    {
        AppendLog("[World] ERROR: DP.exe info unavailable; runtime detail switch failed.\n");
        return false;
    }

    if (g_mainExeSize != 0x010B5000 ||
        g_mainExeTimeDateStamp != 0x529721DC)
    {
        AppendLog("[World] ERROR: Unsupported DP.exe build; runtime detail switch failed.\n");
        return false;
    }

    static const unsigned char prefix[] = {
        0xC7, 0x86, 0xB4, 0x87, 0x02, 0x00
    };

    unsigned char* instruction =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + kWorldIncrementalOuterClassifyRva);

    if (memcmp(instruction, prefix, sizeof(prefix)) != 0)
    {
        AppendLog("[World] ERROR: Incremental detail instruction signature mismatch.\n");
        return false;
    }

    const DWORD desiredImmediate = (scale >= 2) ? 0u : 1u;
    const DWORD currentImmediate =
        *reinterpret_cast<const DWORD*>(instruction + 6);

    if (currentImmediate != 0u && currentImmediate != 1u)
    {
        AppendLog("[World] ERROR: Incremental detail immediate has an unexpected value.\n");
        return false;
    }

    if (currentImmediate != desiredImmediate)
    {
        DWORD oldProtect = 0;
        if (!VirtualProtect(
                instruction + 6,
                sizeof(DWORD),
                PAGE_EXECUTE_READWRITE,
                &oldProtect))
        {
            AppendLog("[World] ERROR: VirtualProtect failed for runtime detail switch.\n");
            return false;
        }

        *reinterpret_cast<DWORD*>(instruction + 6) = desiredImmediate;
        FlushInstructionCache(GetCurrentProcess(), instruction, 10);

        DWORD ignored = 0;
        VirtualProtect(instruction + 6, sizeof(DWORD), oldProtect, &ignored);
    }

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
