#include "world_streaming.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <cmath>
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
std::atomic_bool g_worldDetailExtensionReady{ false };
std::mutex g_worldDetailPatchMutex;
std::mutex g_worldMainFrustumPatchMutex;
std::mutex g_worldObjectActivationPatchMutex;
std::mutex g_worldObjectLodHookMutex;
std::mutex g_worldInteriorOcclusionBridgeMutex;

// Research-only frustum helper hook. It stays behaviorally native unless the
// Diagnostics switch is enabled at runtime.
using WorldFrustumCullFn = bool (__thiscall*)(
    void* renderer,
    const float* planes,
    const float* center,
    const float* extents,
    int firstPlane,
    int endPlane);

WorldFrustumCullFn g_originalWorldFrustumCull = nullptr;
std::atomic_bool g_worldFrustumCullResearchHookReady{ false };
std::atomic_bool g_worldDisableFrustumCullResearch{ false };
std::atomic_ullong g_worldFrustumCullBypassedRejects{ 0 };

// Private far-plane values for CRdCamera main-frustum classes 3/4/5.
// FUN_006B62E0's three corresponding FLD operands are redirected here once;
// runtime mode changes then update only aligned 32-bit data. Classes 0/1/2
// stay on the game's original 200000/80000/20000 sources.
alignas(4) volatile LONG g_worldMainFrustumFarBits[3] = {};
bool g_worldMainFrustumOperandsPatched = false;

// Raw 4-byte storage intentionally read by DP through `FLD dword ptr [absolute]`.
// The instruction operand is redirected once; hot apply then changes only this
// aligned value instead of rewriting executable code on every Apply.
alignas(4) volatile LONG g_worldObjectActivationThresholdBits = 0;
bool g_worldObjectActivationOperandPatched = false;

// The interior visibility-volume callsite is patched only once at startup.
// Runtime/F10 toggles then update this aligned flag atomically instead of
// rewriting executable code while the game may be executing it.
alignas(4) volatile LONG g_worldInteriorOcclusionEnabled = 1;
std::atomic_bool g_worldInteriorOcclusionBridgeReady{ false };
uintptr_t g_worldInteriorOcclusionOriginalTarget = 0;
uintptr_t g_worldInteriorOcclusionCallsiteRva = 0;

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
            g_mainExeBase + build->world.cellDetailClassifyRva);

    if (memcmp(target, signature, sizeof(signature)) != 0)
    {
        AppendLog(
            "[World] ERROR: Cell-detail classifier signature mismatch; "
            "extension disabled.\n");
        return false;
    }

    // Extended world detail is one feature with two consumers: the bulk
    // classifier hook above and the incremental state-machine immediate below.
    // Validate both sites before installing either half so a build mismatch
    // can never leave HighDetailDistanceScale partially available.
    static const unsigned char incrementalPrefix[] = {
        0xC7, 0x86, 0xB4, 0x87, 0x02, 0x00
    };
    unsigned char* incrementalInstruction =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + build->world.incrementalOuterClassifyRva);
    if (memcmp(
            incrementalInstruction,
            incrementalPrefix,
            sizeof(incrementalPrefix)) != 0 ||
        incrementalInstruction[7] != 0 ||
        incrementalInstruction[8] != 0 ||
        incrementalInstruction[9] != 0 ||
        (incrementalInstruction[6] != 0u &&
         incrementalInstruction[6] != 1u))
    {
        AppendLog(
            "[World] ERROR: Incremental detail instruction signature mismatch; "
            "world-detail extension disabled atomically.\n");
        return false;
    }

    const MH_STATUS createStatus =
        MH_CreateHook(
            target,
            reinterpret_cast<void*>(&HookWorldCellDetailClassify),
            reinterpret_cast<void**>(&g_originalWorldCellDetailClassify));

    if (createStatus != MH_OK &&
        !(createStatus == MH_ERROR_ALREADY_CREATED &&
          g_originalWorldCellDetailClassify != nullptr))
    {
        AppendLog(
            "[World] ERROR: MH_CreateHook failed or target is owned by "
            "another hook for cell-detail classifier.\n");
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(target);

    if (enableStatus != MH_OK &&
        enableStatus != MH_ERROR_ENABLED)
    {
        if (createStatus == MH_OK)
            MH_RemoveHook(target);
        AppendLog(
            "[World] ERROR: MH_EnableHook failed for cell-detail classifier.\n");
        return false;
    }

    g_worldDetailExtensionReady.store(true, std::memory_order_release);
    AppendLog(
        "[World] Runtime world-detail transaction prepared (Original/Extended switch available).\n");

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

    // Extended detail is valid only when the complete two-part implementation
    // was prepared. Original mode remains usable even if preparation failed,
    // so callers can always fail closed to native behavior.
    if (scale >= 2 &&
        !g_worldDetailExtensionReady.load(std::memory_order_acquire))
    {
        AppendLog(
            "[World] ERROR: Extended world detail requested but the complete "
            "transaction is unavailable; keeping native detail behavior.\n");
        return false;
    }

    static const unsigned char prefix[] = {
        0xC7, 0x86, 0xB4, 0x87, 0x02, 0x00
    };

    unsigned char* instruction =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + build->world.incrementalOuterClassifyRva);

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


bool IsWorldDetailExtensionAvailable()
{
    return g_worldDetailExtensionReady.load(std::memory_order_acquire);
}

unsigned int GetWorldDetailDistanceScale()
{
    return g_worldDetailScale.load(std::memory_order_acquire);
}


// -----------------------------------------------------------------------------
// Native CRdCamera main-frustum distance floor
// -----------------------------------------------------------------------------
//
// FUN_006B62E0 builds six native main-frustum variants with far planes:
//   class 0 = 200000
//   class 1 =  80000
//   class 2 =  20000
//   class 3 =   5000
//   class 4 =   1000
//   class 5 =    500
//
// Object code keeps selecting the same native class via FUN_006BB4F0. ZachFix
// redirects only the class 3/4/5 FLD operands to private storage and raises
// their minimum far plane by mode. No object flags, frustum tests, streaming,
// activation or LOD selectors are bypassed.
namespace
{
const char* WorldMainFrustumModeName(unsigned int mode)
{
    switch (mode)
    {
    case 0: return "Original";
    case 1: return "Extended";
    case 2: return "Extended Plus";
    case 3: return "Extreme";
    default: return "Unknown";
    }
}

void FloatToLongBits(float value, LONG& bits)
{
    static_assert(sizeof(value) == sizeof(bits));
    memcpy(&bits, &value, sizeof(bits));
}
}

bool ApplyWorldMainFrustumDistanceMode(unsigned int mode)
{
    if (mode > 3)
        return false;

    std::lock_guard<std::mutex> lock(g_worldMainFrustumPatchMutex);

    if (!InitializeMainExeInfo())
    {
        AppendLog("[World] ERROR: DP.exe info unavailable; main-frustum distance switch failed.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[World] ERROR: Unsupported DP.exe build; main-frustum distance switch failed.\n");
        return false;
    }

    // Effective far planes for native classes 3, 4 and 5. This implements a
    // visibility-distance floor while leaving classes 0/1/2 unchanged.
    static constexpr float kFarByMode[4][3] = {
        {  5000.0f,  1000.0f,   500.0f },
        {  5000.0f,  1000.0f,  1000.0f },
        {  5000.0f,  5000.0f,  5000.0f },
        { 20000.0f, 20000.0f, 20000.0f }
    };

    LONG newBits[3] = {};
    for (int i = 0; i < 3; ++i)
        FloatToLongBits(kFarByMode[mode][i], newBits[i]);

    // Publish valid data before the one-time operand redirect. Every
    // intermediate write is itself a valid native-style far plane, so a
    // concurrent camera update can never observe a torn float.
    for (int i = 0; i < 3; ++i)
        InterlockedExchange(&g_worldMainFrustumFarBits[i], newBits[i]);

    if (!g_worldMainFrustumOperandsPatched)
    {
        unsigned char* instructions[3] = {};
        for (int i = 0; i < 3; ++i)
        {
            if (build->world.mainFrustumFarLoadRvas[i] == 0 ||
                build->world.mainFrustumFarSourceAddresses[i] == 0)
            {
                AppendLog("[World] ERROR: Main-frustum build profile is incomplete.\n");
                return false;
            }

            instructions[i] = reinterpret_cast<unsigned char*>(
                g_mainExeBase + build->world.mainFrustumFarLoadRvas[i]);

            if (instructions[i][0] != 0xD9 || instructions[i][1] != 0x05)
            {
                AppendLog("[World] ERROR: Main-frustum far-plane FLD signature mismatch.\n");
                return false;
            }

            const uint32_t originalAbsolute =
                *reinterpret_cast<const uint32_t*>(instructions[i] + 2);
            if (originalAbsolute != build->world.mainFrustumFarSourceAddresses[i])
            {
                AppendLog("[World] ERROR: Main-frustum far-plane source address mismatch.\n");
                return false;
            }
        }

        unsigned char* firstOperand = instructions[0] + 2;
        unsigned char* lastOperandEnd = instructions[2] + 6;
        const SIZE_T patchSpan =
            static_cast<SIZE_T>(lastOperandEnd - firstOperand);

        DWORD oldProtect = 0;
        if (!VirtualProtect(
                firstOperand,
                patchSpan,
                PAGE_EXECUTE_READWRITE,
                &oldProtect))
        {
            AppendLog("[World] ERROR: VirtualProtect failed for main-frustum operand redirects.\n");
            return false;
        }

        for (int i = 0; i < 3; ++i)
        {
            const uint32_t replacementAbsolute = static_cast<uint32_t>(
                reinterpret_cast<uintptr_t>(&g_worldMainFrustumFarBits[i]));
            memcpy(instructions[i] + 2, &replacementAbsolute, sizeof(replacementAbsolute));
        }

        FlushInstructionCache(
            GetCurrentProcess(),
            firstOperand,
            patchSpan);

        DWORD ignored = 0;
        if (!VirtualProtect(
                firstOperand,
                patchSpan,
                oldProtect,
                &ignored))
        {
            AppendLog("[World] WARNING: Could not restore code protection after main-frustum operand redirects.\n");
        }

        g_worldMainFrustumOperandsPatched = true;
    }

    g_config.mainFrustumDistanceMode = mode;

    char text[320] = {};
    sprintf_s(
        text,
        "[World] Main frustum distance mode set to %s: class3=%.0f, class4=%.0f, class5=%.0f; classes0/1/2 remain 200000/80000/20000.\n",
        WorldMainFrustumModeName(mode),
        static_cast<double>(kFarByMode[mode][0]),
        static_cast<double>(kFarByMode[mode][1]),
        static_cast<double>(kFarByMode[mode][2]));
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
    if (build == nullptr || build->world.objectActivationThresholdLoadRva == 0)
    {
        AppendLog("[World] ERROR: Unsupported DP.exe build; object activation distance switch failed.\n");
        return false;
    }

    unsigned char* instruction =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + build->world.objectActivationThresholdLoadRva);

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
            build->world.objectActivationThresholdSourceAddress;

        if (expectedOriginal == 0 || originalAbsolute != expectedOriginal)
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


// -----------------------------------------------------------------------------
// Native PC object LOD distance scale
// -----------------------------------------------------------------------------
//
// DP computes object+0x20 as cameraDistance / (resourceScale * 25.0f) and its
// native renderer consumes that metric when choosing the existing LOD resource
// records. Dividing only this metric by N delays the same native transitions by
// approximately Nx without changing streaming, activation, resource flags or
// mesh-selection code.
namespace
{
using WorldObjectLodMetricFn = void (__thiscall*)(
    void* renderer,
    void* object,
    const void* cameraPosition);

WorldObjectLodMetricFn g_originalWorldObjectLodMetric = nullptr;
std::atomic_uint g_worldObjectLodDistanceScale{ 1 };
std::atomic_bool g_worldObjectLodHookReady{ false };

void __fastcall HookWorldObjectLodMetric(
    void* renderer,
    void*,
    void* object,
    const void* cameraPosition)
{
    g_originalWorldObjectLodMetric(renderer, object, cameraPosition);

    if (object == nullptr)
        return;

    const unsigned int scale =
        g_worldObjectLodDistanceScale.load(std::memory_order_acquire);
    if (scale <= 1)
        return;

    auto* bytes = static_cast<unsigned char*>(object);
    float metric = 0.0f;
    memcpy(&metric, bytes + 0x20, sizeof(metric));
    if (!std::isfinite(metric) || metric <= 0.0f)
        return;

    metric /= static_cast<float>(scale);
    memcpy(bytes + 0x20, &metric, sizeof(metric));
}

bool PrepareWorldObjectLodHook()
{
    if (g_worldObjectLodHookReady.load(std::memory_order_acquire))
        return true;

    if (!InitializeMainExeInfo())
    {
        AppendLog("[World] ERROR: DP.exe info unavailable; object LOD distance switch failed.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || build->world.objectLodMetricRva == 0)
    {
        AppendLog("[World] ERROR: Unsupported DP.exe build; object LOD distance switch failed.\n");
        return false;
    }

    static const unsigned char signature[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x18, 0x89, 0x4D, 0xE8,
        0x8B, 0x45, 0x08, 0x8B, 0x88, 0x84, 0x00, 0x00, 0x00
    };

    auto* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->world.objectLodMetricRva);
    if (memcmp(target, signature, sizeof(signature)) != 0)
    {
        AppendLog("[World] ERROR: Object LOD metric signature mismatch.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookWorldObjectLodMetric),
        reinterpret_cast<void**>(&g_originalWorldObjectLodMetric));
    if (createStatus != MH_OK)
    {
        AppendLog("[World] ERROR: Could not create object LOD metric hook.\n");
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(target);
    if (enableStatus != MH_OK)
    {
        MH_RemoveHook(target);
        g_originalWorldObjectLodMetric = nullptr;
        AppendLog("[World] ERROR: Could not enable object LOD metric hook.\n");
        return false;
    }

    g_worldObjectLodHookReady.store(true, std::memory_order_release);

    char text[224] = {};
    sprintf_s(
        text,
        "[World] Native object LOD metric hook ready on %s at DP.exe+0x%08llX.\n",
        build->name,
        static_cast<unsigned long long>(build->world.objectLodMetricRva));
    AppendLog(text);
    return true;
}
} // namespace

bool ApplyWorldObjectLodDistanceScale(unsigned int scale)
{
    if (scale < 1 || scale > 4)
        return false;

    std::lock_guard<std::mutex> lock(g_worldObjectLodHookMutex);

    if (scale > 1 && !PrepareWorldObjectLodHook())
        return false;

    g_worldObjectLodDistanceScale.store(scale, std::memory_order_release);
    g_config.objectLodDistanceScale = scale;

    char text[192] = {};
    sprintf_s(
        text,
        "[World] Object LOD distance scale set to %ux (%s native PC LOD metric).\n",
        scale,
        scale == 1 ? "original" : "extended");
    AppendLog(text);
    return true;
}


// -----------------------------------------------------------------------------
// Interior visibility-volume regression fix
// -----------------------------------------------------------------------------
//
// Confirmed Director's Cut path:
//   normal frustum -> PASS
//   outer-world custom visibility volume -> REJECT
//   mesh-list builder is never reached
//
// The production callsite is patched exactly once during initialization:
//
//   original callsite -> WorldInteriorOcclusionBridge
//
// The bridge is intentionally tiny and preserves the original call contract.
// When the fix is enabled it returns true and performs the callee's RET 10h
// cleanup locally. When disabled it tail-jumps to the exact native callee, so
// the original return address, arguments, stack cleanup, and volume-test logic
// remain untouched. F10 therefore changes only g_worldInteriorOcclusionEnabled.
//
// Everything around this call remains native, including normal frustum culling,
// streaming, LOD, object activation, and every other caller of the volume test.
#if defined(_M_IX86)
__declspec(naked) void WorldInteriorOcclusionBridge()
{
    __asm
    {
        cmp dword ptr [g_worldInteriorOcclusionEnabled], 0
        je nativePath

        mov al, 1
        ret 10h

    nativePath:
        jmp dword ptr [g_worldInteriorOcclusionOriginalTarget]
    }
}
#endif

bool PrepareWorldInteriorOcclusionFixBridge()
{
    if (g_worldInteriorOcclusionBridgeReady.load(std::memory_order_acquire))
        return true;

    std::lock_guard<std::mutex> lock(g_worldInteriorOcclusionBridgeMutex);
    if (g_worldInteriorOcclusionBridgeReady.load(std::memory_order_relaxed))
        return true;

#if !defined(_M_IX86)
    AppendLog(
        "[World][Occlusion] ERROR: Interior visibility bridge requires the supported x86 build.\n");
    return false;
#else
    if (!InitializeMainExeInfo())
    {
        AppendLog(
            "[World][Occlusion] ERROR: DP.exe info unavailable; interior visibility bridge not installed.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog(
            "[World][Occlusion] ERROR: Unsupported DP.exe build; interior visibility bridge not installed.\n");
        return false;
    }

    const uintptr_t callsiteRva = build->world.interiorOcclusionCallsiteRva;
    if (callsiteRva == 0)
    {
        AppendLog(
            "[World][Occlusion] ERROR: No interior-occlusion callsite is mapped for this build.\n");
        return false;
    }

    static const unsigned char originalBytes[5] = {
        0xE8, 0xCC, 0x84, 0x00, 0x00
    };

    unsigned char* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + callsiteRva);

    // Startup is the only supported installation point. Requiring the exact
    // vanilla CALL keeps the patch fail-closed if another mod owns this site.
    if (std::memcmp(target, originalBytes, sizeof(originalBytes)) != 0)
    {
        char text[240] = {};
        sprintf_s(
            text,
            "[World][Occlusion] ERROR: Signature mismatch at DP.exe+0x%08llX; bridge not installed.\n",
            static_cast<unsigned long long>(callsiteRva));
        AppendLog(text);
        return false;
    }

    std::int32_t nativeRelative = 0;
    std::memcpy(&nativeRelative, target + 1, sizeof(nativeRelative));

    const std::intptr_t nativeTargetValue =
        reinterpret_cast<std::intptr_t>(target + sizeof(originalBytes)) +
        static_cast<std::intptr_t>(nativeRelative);
    void* nativeTarget = reinterpret_cast<void*>(nativeTargetValue);

    if (!IsMainExeAddress(nativeTarget))
    {
        AppendLog(
            "[World][Occlusion] ERROR: Native visibility-volume target is outside DP.exe; bridge not installed.\n");
        return false;
    }

    // In 32-bit x86 a rel32 CALL uses modulo-2^32 address arithmetic, so the
    // full process address space is representable by the four displacement bytes.
    const std::uintptr_t bridgeAddress =
        reinterpret_cast<std::uintptr_t>(&WorldInteriorOcclusionBridge);
    const std::uint32_t bridgeRelativeBits = static_cast<std::uint32_t>(
        bridgeAddress -
        reinterpret_cast<std::uintptr_t>(target + sizeof(originalBytes)));

    unsigned char bridgeCall[5] = { 0xE8, 0, 0, 0, 0 };
    std::memcpy(
        bridgeCall + 1,
        &bridgeRelativeBits,
        sizeof(bridgeRelativeBits));

    // Publish everything the bridge needs before executable code can reach it.
    g_worldInteriorOcclusionOriginalTarget =
        reinterpret_cast<std::uintptr_t>(nativeTarget);
    g_worldInteriorOcclusionCallsiteRva = callsiteRva;
    InterlockedExchange(
        &g_worldInteriorOcclusionEnabled,
        g_config.fixInteriorOcclusionBugs ? 1 : 0);

    DWORD oldProtect = 0;
    if (!VirtualProtect(
            target,
            sizeof(bridgeCall),
            PAGE_EXECUTE_READWRITE,
            &oldProtect))
    {
        g_worldInteriorOcclusionOriginalTarget = 0;
        g_worldInteriorOcclusionCallsiteRva = 0;
        AppendLog(
            "[World][Occlusion] ERROR: VirtualProtect failed; interior visibility bridge not installed.\n");
        return false;
    }

    std::memcpy(target, bridgeCall, sizeof(bridgeCall));
    FlushInstructionCache(GetCurrentProcess(), target, sizeof(bridgeCall));

    DWORD ignored = 0;
    if (!VirtualProtect(
            target,
            sizeof(bridgeCall),
            oldProtect,
            &ignored))
    {
        AppendLog(
            "[World][Occlusion] WARNING: Could not restore code protection after bridge installation.\n");
    }

    g_worldInteriorOcclusionBridgeReady.store(true, std::memory_order_release);

    char text[288] = {};
    sprintf_s(
        text,
        "[World][Occlusion] Runtime bridge installed once at DP.exe+0x%08llX -> native target DP.exe+0x%08llX; F10 toggles are data-only.\n",
        static_cast<unsigned long long>(callsiteRva),
        static_cast<unsigned long long>(
            reinterpret_cast<std::uintptr_t>(nativeTarget) - g_mainExeBase));
    AppendLog(text);
    return true;
#endif
}

bool ApplyWorldInteriorOcclusionFix(bool enabled)
{
    if (!g_worldInteriorOcclusionBridgeReady.load(std::memory_order_acquire))
    {
        AppendLog(
            "[World][Occlusion] ERROR: Runtime bridge is not ready; interior occlusion fix not changed.\n");
        return false;
    }

    InterlockedExchange(
        &g_worldInteriorOcclusionEnabled,
        enabled ? 1 : 0);
    g_config.fixInteriorOcclusionBugs = enabled;

    char text[256] = {};
    sprintf_s(
        text,
        "[World][Occlusion] Interior visibility-volume fix %s via runtime bridge at DP.exe+0x%08llX; normal frustum culling remains native.\n",
        enabled ? "ENABLED" : "DISABLED",
        static_cast<unsigned long long>(g_worldInteriorOcclusionCallsiteRva));
    AppendLog(text);
    return true;
}


bool IsWorldInteriorOcclusionFixAvailable()
{
    return g_worldInteriorOcclusionBridgeReady.load(std::memory_order_acquire);
}

bool IsWorldInteriorOcclusionFixActive()
{
    return IsWorldInteriorOcclusionFixAvailable() &&
        InterlockedCompareExchange(
            &g_worldInteriorOcclusionEnabled, 0, 0) != 0;
}


// -----------------------------------------------------------------------------
// Research-only global frustum bypass
// -----------------------------------------------------------------------------

static bool __fastcall HookWorldFrustumCullResearch(
    void* renderer,
    void*,
    const float* planes,
    const float* center,
    const float* extents,
    int firstPlane,
    int endPlane)
{
    const bool nativeResult = g_originalWorldFrustumCull(
        renderer,
        planes,
        center,
        extents,
        firstPlane,
        endPlane);

    if (!nativeResult &&
        g_worldDisableFrustumCullResearch.load(std::memory_order_acquire))
    {
        g_worldFrustumCullBypassedRejects.fetch_add(
            1,
            std::memory_order_relaxed);
        return true;
    }

    return nativeResult;
}

bool PrepareWorldFrustumCullResearchHook()
{
    if (g_worldFrustumCullResearchHookReady.load(std::memory_order_acquire))
        return true;

    if (!InitializeMainExeInfo())
    {
        AppendLog(
            "[World][FrustumResearch] DP.exe info unavailable; research bypass disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog(
            "[World][FrustumResearch] Unsupported DP.exe build; research bypass disabled.\n");
        return false;
    }

    const uintptr_t helperRva = build->world.frustumCullRva;
    if (helperRva == 0)
        return false;

    static const unsigned char signature[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x20,
        0x89, 0x4D, 0xF0,
        0x33, 0xC0,
        0x74, 0x05
    };

    unsigned char* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + helperRva);
    if (std::memcmp(target, signature, sizeof(signature)) != 0)
    {
        AppendLog(
            "[World][FrustumResearch] Helper signature mismatch; research bypass disabled.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookWorldFrustumCullResearch),
        reinterpret_cast<void**>(&g_originalWorldFrustumCull));
    if (createStatus != MH_OK &&
        !(createStatus == MH_ERROR_ALREADY_CREATED &&
          g_originalWorldFrustumCull != nullptr))
    {
        AppendLog(
            "[World][FrustumResearch] MH_CreateHook failed or target is owned by another hook; research bypass disabled.\n");
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(target);
    if (enableStatus != MH_OK &&
        enableStatus != MH_ERROR_ENABLED)
    {
        if (createStatus == MH_OK)
            MH_RemoveHook(target);
        AppendLog(
            "[World][FrustumResearch] MH_EnableHook failed; research bypass disabled.\n");
        return false;
    }

    g_worldFrustumCullResearchHookReady.store(true, std::memory_order_release);

    char text[224] = {};
    sprintf_s(
        text,
        "[World][FrustumResearch] Global frustum helper hook ready at DP.exe+0x%08llX (bypass default OFF, runtime-only).\n",
        static_cast<unsigned long long>(helperRva));
    AppendLog(text);
    return true;
}

bool IsWorldFrustumCullResearchHookReady()
{
    return g_worldFrustumCullResearchHookReady.load(std::memory_order_acquire);
}

bool GetWorldFrustumCullDisabledResearch()
{
    return g_worldDisableFrustumCullResearch.load(std::memory_order_acquire);
}

void SetWorldFrustumCullDisabledResearch(bool disabled)
{
    if (disabled &&
        !IsWorldFrustumCullResearchHookReady() &&
        !PrepareWorldFrustumCullResearchHook())
    {
        AppendLog(
            "[World][FrustumResearch] Bypass requested, but the research hook could not be installed.\n");
        return;
    }

    g_worldDisableFrustumCullResearch.store(disabled, std::memory_order_release);
    AppendLog(disabled
        ? "[World][FrustumResearch] ALL hooked frustum rejects are now force-passed (research only, not persisted).\n"
        : "[World][FrustumResearch] Frustum helper restored to native results.\n");
}

unsigned long long GetWorldFrustumCullBypassedRejects()
{
    return g_worldFrustumCullBypassedRejects.load(std::memory_order_relaxed);
}
