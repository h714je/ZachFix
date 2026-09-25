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

struct RuntimeBuildProfile
{
    uintptr_t direct3DCreate9IatRva;
    uintptr_t speedDivideRva;
    uintptr_t frameDeltaRva;
    uintptr_t currentGameStateGetterRva;
};

struct InputBuildProfile
{
    uintptr_t controllerBindingEvaluatorRva;

    // PC-only stick post-processor. It applies a second +/-16 deadzone,
    // /109 renormalization and a 0.5-per-update slew to the four stick axes.
    uintptr_t stickAxisPostProcessorRva;

    // Float stick getter used by gameplay/camera consumers. The production
    // profile hook applies the confirmed Xbox aim shaping only to the known
    // live-aim right-stick callers.
    uintptr_t stickFloatGetterRva;
    uintptr_t rightStickAimCallerRvas[4];

    uintptr_t useJoyModeRva;
    uintptr_t inputUpdateRva;

    // Surviving native vibration path. ZachFix opens CRdInput's disabled PC
    // actuator gate and forwards the final two-channel state to XInput.
    uintptr_t rdInputSetActuatorRva;
    uintptr_t inputActuatorSetSecondRva;

    // Player-car controller path convergence point. Director's Cut has
    // already collapsed LT/RT to float 0.0/1.0 here; the production bridge
    // can restore the original continuous trigger values locally.
    uintptr_t vehicleAnalogInputInjectRva;
};

struct PlayerBuildProfile
{
    // Common Player-update tail where the original Xbox build invokes the
    // combat-strafe ingress helper before final state bookkeeping.
    uintptr_t combatStrafeHookRva;

    // Native PC primitives reused by the recovered Xbox gate.
    uintptr_t combatCapabilityRva;
    uintptr_t stateTransitionRva;
};

struct WorldBuildProfile
{
    uintptr_t cellDetailClassifyRva;
    uintptr_t incrementalOuterClassifyRva;

    // Three FLD absolute-source instructions inside CRdCamera::update that
    // seed native main-frustum classes 3, 4 and 5.
    uintptr_t mainFrustumFarLoadRvas[3];
    uint32_t mainFrustumFarSourceAddresses[3];

    // Single FLD in the native active-list builder that seeds the squared
    // per-object activation radius, plus its expected absolute source.
    uintptr_t objectActivationThresholdLoadRva;
    uint32_t objectActivationThresholdSourceAddress;

    // Native renderer helper that writes object+0x20 =
    // cameraDistance / (resourceScale * 25).
    uintptr_t objectLodMetricRva;

    // Alternate low-detail 3D residency path.
    uintptr_t spatialResidencyRva;
    uintptr_t residencySetTargetRva;
    uintptr_t residencyFocusPositionRva;
    uintptr_t objectRangeStartRva;
    uintptr_t objectRangeEndRva;

    // Outer-world interior visibility-volume callsite and shared frustum
    // helper used by the optional Diagnostics research bypass.
    uintptr_t interiorOcclusionCallsiteRva;
    uintptr_t frustumCullRva;
};

struct DpBuildProfile
{
    DpBuild build;
    const char* name;

    DWORD timeDateStamp;
    size_t sizeOfImage;

    RuntimeBuildProfile runtime;
    InputBuildProfile input;
    PlayerBuildProfile player;
    WorldBuildProfile world;

    HouseListFixBuildProfile houseListFix;
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
