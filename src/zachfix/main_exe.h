#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <atomic>

enum class DpBuild
{
    Unknown,
    Steam101b,
    Gog101b
};

struct DifficultyBuildProfile
{
    uintptr_t selectorRva;
    uintptr_t nativeNewGameStateWriteRva;
    uintptr_t historicalSwapWriteRva;
    uintptr_t menuResetWriteRva;
    uintptr_t stateHandlerWriteRva;
};

struct HouseListFixBuildProfile
{
    // Native CLevel loader that maps the current level key through resource
    // 0x39DF (UPDATA/PRM/HOUSE_LIST.NOD) into day/night node state.
    uintptr_t levelConfigLoadRva;

    // Helpers used by that loader to reproduce its exact lookup key.
    uintptr_t levelActiveVariantRva;
    uintptr_t levelResourceViewRva;
    uintptr_t levelResourceKeyRva;

    // Native cache name lookup. The production fix hooks this only to capture
    // the live resource-manager pointer that owns resource 0x39DF.
    uintptr_t resourceNameLookupRva;
};

struct DpBuildProfile
{
    DpBuild build;
    const char* name;

    DWORD timeDateStamp;
    size_t sizeOfImage;

    uintptr_t direct3DCreate9IatRva;
    uintptr_t speedDivideRva;
    uintptr_t frameDeltaRva;
    uintptr_t controllerBindingEvaluatorRva;

    // PC-only stick post-processor. It applies a second +/-16 deadzone,
    // /109 renormalization and a 0.5-per-update slew to the four stick axes.
    // The Xbox360 runtime profile restores exact Xbox normalized stick floats
    // after this function while leaving downstream routing untouched.
    uintptr_t stickAxisPostProcessorRva;

    // Float stick getter used by gameplay/camera consumers. The production
    // profile hook applies the confirmed Xbox aim shaping only to the known
    // live-aim right-stick callers.
    uintptr_t stickFloatGetterRva;
    uintptr_t rightStickAimCallerRvas[4];

    uintptr_t useJoyModeRva;
    uintptr_t inputUpdateRva;
    uintptr_t worldCellDetailClassifyRva;
    uintptr_t worldIncrementalOuterClassifyRva;

    // Single FLD in the native active-list builder that seeds the squared
    // per-object activation radius. ZachFix redirects only this operand to a
    // private runtime value; the shared game constant remains untouched.
    uintptr_t worldObjectActivationThresholdLoadRva;

    // Absolute source operand expected in the original FLD instruction above.
    // This stays in the build profile so Steam/GOG data mappings are not
    // reconstructed locally in world_streaming.cpp.
    uint32_t worldObjectActivationThresholdSourceAddress;

    // Confirmed outer-world interior visibility-volume callsite used by the
    // production disappearing-prop fix. The shared frustum helper is kept
    // separately for the optional Diagnostics-only research bypass.
    uintptr_t worldInteriorOcclusionCallsiteRva;
    uintptr_t worldFrustumCullRva;

    // Surviving native vibration path. ZachFix opens CRdInput's disabled PC
    // actuator gate and forwards the final two-channel state to XInput.
    uintptr_t rdInputSetActuatorRva;
    uintptr_t inputActuatorSetSecondRva;

    // Player-car controller path convergence point. Director's Cut has
    // already collapsed LT/RT to float 0.0/1.0 here; the production bridge
    // can restore the original continuous trigger values locally.
    uintptr_t vehicleAnalogInputInjectRva;

    // Production repair for the Director's Cut HOUSE_LIST.NOD key-endian regression.
    HouseListFixBuildProfile houseListFix;

    // Native difficulty/title mappings. Keep these alongside the rest of the
    // supported executable profile instead of rebuilding a second Steam/GOG
    // table in difficulty.cpp.
    DifficultyBuildProfile difficulty;
};

extern uintptr_t g_mainExeBase;
extern size_t g_mainExeSize;
extern DWORD g_mainExeTimeDateStamp;
extern std::atomic_bool g_mainExeInfoValid;

// Pure PE-header lookup used by the earliest DllMain hook. This performs no
// logging, file I/O, allocation, or synchronization.
const DpBuildProfile* DetectDpBuildProfile(HMODULE module);

// Cached main-executable information for normal worker-thread initialization.
bool InitializeMainExeInfo();
const DpBuildProfile* GetDpBuildProfile();
bool IsMainExeAddress(const void* address);
