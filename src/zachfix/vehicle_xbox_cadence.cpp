#include "vehicle_xbox_cadence.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
static_assert(sizeof(void*) == 4, "Deadly Premonition vehicle hook requires the Win32 build");

constexpr std::uint32_t kPlayerVehicleFlag = 0x00008000u;
constexpr size_t kVehicleFlagsOffset = 0x434;
constexpr size_t kVehicleActiveOffset = 0x12F0;
constexpr double kXboxLogicTickSeconds = 1.0 / 30.0;
constexpr double kDiscontinuitySeconds = 1.0;
constexpr float kMaxAccumulatedDelta60 = 8.0f;
constexpr unsigned int kMaxLoggedWindows = 20;

using VehiclePhysicsDispatchFn = void (__fastcall*)(void* car);

VehiclePhysicsDispatchFn g_originalVehiclePhysicsDispatch = nullptr;
bool g_enabled = false;
uintptr_t g_frameDeltaAddress = 0;
LARGE_INTEGER g_qpcFrequency{};
LARGE_INTEGER g_previousQpc{};
bool g_havePreviousQpc = false;
void* g_currentPlayerCar = nullptr;
double g_wallAccumulator = 0.0;
float g_delta60Accumulator = 0.0f;

ULONGLONG g_windowStartMs = 0;
unsigned int g_loggedWindows = 0;
std::uint64_t g_inputCalls = 0;
std::uint64_t g_logicCalls = 0;
std::uint64_t g_skippedCalls = 0;
double g_passedDeltaSum = 0.0;
float g_passedDeltaMin = 0.0f;
float g_passedDeltaMax = 0.0f;

bool MatchesBytes(uintptr_t address, const unsigned char* expected, size_t size)
{
    return address != 0 &&
        std::memcmp(reinterpret_cast<const void*>(address), expected, size) == 0;
}

bool IsLivePlayerCar(void* car)
{
    if (car == nullptr)
        return false;

    const auto* bytes = static_cast<const unsigned char*>(car);
    if (*(bytes + kVehicleActiveOffset) == 0)
        return false;

    std::uint32_t flags = 0;
    std::memcpy(&flags, bytes + kVehicleFlagsOffset, sizeof(flags));
    return (flags & kPlayerVehicleFlag) != 0;
}

float ReadGameDelta60()
{
    if (g_frameDeltaAddress == 0)
        return 0.0f;
    return *reinterpret_cast<const float*>(g_frameDeltaAddress);
}

void ResetCadenceState(void* car, const LARGE_INTEGER& now)
{
    g_currentPlayerCar = car;
    g_previousQpc = now;
    g_havePreviousQpc = true;
    g_wallAccumulator = 0.0;
    g_delta60Accumulator = 0.0f;
}

void AddDelta60(float value)
{
    if (!std::isfinite(value) || value < 0.0f)
        return;
    g_delta60Accumulator = std::min(
        kMaxAccumulatedDelta60,
        g_delta60Accumulator + value);
}

void RecordPassedDelta(float value)
{
    if (!std::isfinite(value))
        return;
    if (g_logicCalls == 0)
    {
        g_passedDeltaMin = value;
        g_passedDeltaMax = value;
    }
    else
    {
        g_passedDeltaMin = std::min(g_passedDeltaMin, value);
        g_passedDeltaMax = std::max(g_passedDeltaMax, value);
    }
    g_passedDeltaSum += static_cast<double>(value);
}

void ResetWindowStats()
{
    g_inputCalls = 0;
    g_logicCalls = 0;
    g_skippedCalls = 0;
    g_passedDeltaSum = 0.0;
    g_passedDeltaMin = 0.0f;
    g_passedDeltaMax = 0.0f;
}

void LogWindowIfDue()
{
    if (g_loggedWindows >= kMaxLoggedWindows)
        return;

    const ULONGLONG now = GetTickCount64();
    if (g_windowStartMs == 0)
    {
        g_windowStartMs = now;
        ResetWindowStats();
        return;
    }

    const ULONGLONG elapsedMs = now - g_windowStartMs;
    if (elapsedMs < 1000)
        return;

    const double seconds = static_cast<double>(elapsedMs) / 1000.0;
    const double invSeconds = seconds > 0.0 ? 1.0 / seconds : 0.0;
    const double avgDelta = g_logicCalls != 0
        ? g_passedDeltaSum / static_cast<double>(g_logicCalls)
        : 0.0;

    char text[512] = {};
    sprintf_s(
        text,
        "[VehicleXboxCadence] window=%u inputHz=%.1f logicHz=%.1f skippedHz=%.1f "
        "passedDelta60 avg/min/max=%.4f/%.4f/%.4f wallCarryMs=%.3f.\n",
        ++g_loggedWindows,
        static_cast<double>(g_inputCalls) * invSeconds,
        static_cast<double>(g_logicCalls) * invSeconds,
        static_cast<double>(g_skippedCalls) * invSeconds,
        avgDelta,
        static_cast<double>(g_passedDeltaMin),
        static_cast<double>(g_passedDeltaMax),
        g_wallAccumulator * 1000.0);
    AppendLog(text);

    g_windowStartMs = now;
    ResetWindowStats();
}

void CallOriginalWithDelta(void* car, float delta60)
{
    const float originalDelta = ReadGameDelta60();
    if (g_frameDeltaAddress != 0 && std::isfinite(delta60))
        *reinterpret_cast<float*>(g_frameDeltaAddress) = delta60;

    g_originalVehiclePhysicsDispatch(car);

    if (g_frameDeltaAddress != 0)
        *reinterpret_cast<float*>(g_frameDeltaAddress) = originalDelta;
}

void __fastcall HookVehiclePhysicsDispatch(void* car)
{
    if (!g_enabled || g_qpcFrequency.QuadPart <= 0)
    {
        g_originalVehiclePhysicsDispatch(car);
        return;
    }

    if (!IsLivePlayerCar(car))
    {
        if (car == g_currentPlayerCar)
        {
            g_currentPlayerCar = nullptr;
            g_havePreviousQpc = false;
            g_wallAccumulator = 0.0;
            g_delta60Accumulator = 0.0f;
        }
        g_originalVehiclePhysicsDispatch(car);
        return;
    }

    ++g_inputCalls;

    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);
    const float currentDelta60 = ReadGameDelta60();

    // A newly selected/spawned player car gets one immediate native update.
    // This avoids delaying initialization while establishing the 30 Hz phase.
    if (!g_havePreviousQpc || g_currentPlayerCar != car)
    {
        ResetCadenceState(car, now);
        RecordPassedDelta(currentDelta60);
        ++g_logicCalls;
        g_originalVehiclePhysicsDispatch(car);
        LogWindowIfDue();
        return;
    }

    const long long ticks = now.QuadPart - g_previousQpc.QuadPart;
    g_previousQpc = now;
    if (ticks <= 0)
    {
        AddDelta60(currentDelta60);
        ++g_skippedCalls;
        LogWindowIfDue();
        return;
    }

    const double wallSeconds = static_cast<double>(ticks) /
        static_cast<double>(g_qpcFrequency.QuadPart);

    // Debugger pauses / load discontinuities should not create a giant burst.
    // Xbox consumes a delayed counter interval as one update, so keep one
    // native update and re-establish the cadence phase instead of catch-up loops.
    if (!std::isfinite(wallSeconds) || wallSeconds >= kDiscontinuitySeconds)
    {
        ResetCadenceState(car, now);
        RecordPassedDelta(currentDelta60);
        ++g_logicCalls;
        g_originalVehiclePhysicsDispatch(car);
        LogWindowIfDue();
        return;
    }

    g_wallAccumulator += wallSeconds;
    AddDelta60(currentDelta60);

    if (g_wallAccumulator + 1.0e-6 < kXboxLogicTickSeconds)
    {
        ++g_skippedCalls;
        LogWindowIfDue();
        return;
    }

    // Consume all complete 30 Hz wall-clock slots but execute the retained
    // Xbox-era routine once, with the accumulated game-time scalar. This
    // mirrors the original console timer gate: one post-gate update receives
    // the elapsed discrete tick count rather than multiple catch-up calls.
    const double completeTicks = std::floor(g_wallAccumulator / kXboxLogicTickSeconds);
    g_wallAccumulator -= completeTicks * kXboxLogicTickSeconds;
    if (g_wallAccumulator < 0.0)
        g_wallAccumulator = 0.0;

    const float accumulatedDelta60 = g_delta60Accumulator;
    g_delta60Accumulator = 0.0f;

    RecordPassedDelta(accumulatedDelta60);
    ++g_logicCalls;
    CallOriginalWithDelta(car, accumulatedDelta60);
    LogWindowIfDue();
}
} // namespace

bool InstallVehicleXboxCadenceFix()
{
    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
    {
        AppendLog("[VehicleXboxCadence] ERROR: unsupported DP.exe build; cadence fix not installed.\n");
        return false;
    }

    g_enabled = g_config.vehicleXboxTickCadence;
    if (!g_enabled)
    {
        AppendLog("[VehicleXboxCadence] Xbox-like player-car 30 Hz phase-5 cadence: OFF.\n");
        return true;
    }

    QueryPerformanceFrequency(&g_qpcFrequency);
    if (g_qpcFrequency.QuadPart <= 0)
    {
        AppendLog("[VehicleXboxCadence] ERROR: QueryPerformanceFrequency failed; cadence fix not installed.\n");
        return false;
    }

    g_frameDeltaAddress = g_mainExeBase + build->frameDeltaRva;
    const uintptr_t dispatchAddress = g_mainExeBase + build->vehiclePhysicsDispatchRva;

    // Steam/GOG homologs are byte-identical at entry:
    // cmp byte ptr [ecx+12F0h],0 ; jz ; mov eax,[ecx+434h] ; test eax,8000h
    static constexpr unsigned char kDispatchSignature[] = {
        0x80, 0xB9, 0xF0, 0x12, 0x00, 0x00, 0x00,
        0x74, 0x2A,
        0x8B, 0x81, 0x34, 0x04, 0x00, 0x00,
        0xA9, 0x00, 0x80, 0x00, 0x00
    };

    if (!MatchesBytes(dispatchAddress, kDispatchSignature, sizeof(kDispatchSignature)))
    {
        AppendLog("[VehicleXboxCadence] ERROR: player CObjectCar physics-dispatch signature mismatch; cadence fix not installed.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        reinterpret_cast<void*>(dispatchAddress),
        reinterpret_cast<void*>(&HookVehiclePhysicsDispatch),
        reinterpret_cast<void**>(&g_originalVehiclePhysicsDispatch));
    if (createStatus != MH_OK)
    {
        char text[256] = {};
        sprintf_s(text,
            "[VehicleXboxCadence] ERROR: MH_CreateHook(dispatch) failed: %d.\n",
            static_cast<int>(createStatus));
        AppendLog(text);
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(reinterpret_cast<void*>(dispatchAddress));
    if (enableStatus != MH_OK)
    {
        MH_RemoveHook(reinterpret_cast<void*>(dispatchAddress));
        g_originalVehiclePhysicsDispatch = nullptr;
        char text[256] = {};
        sprintf_s(text,
            "[VehicleXboxCadence] ERROR: MH_EnableHook(dispatch) failed: %d.\n",
            static_cast<int>(enableStatus));
        AppendLog(text);
        return false;
    }

    char text[768] = {};
    sprintf_s(
        text,
        "[VehicleXboxCadence] Static-RE Xbox cadence repair armed on %s at DP.exe+0x%08lX: "
        "only active 0x8000 player CObjectCar branch is gated to 30 Hz wall cadence; skipped render-frame gameDelta60 values are accumulated (max 8) "
        "and supplied temporarily to the original dispatcher. NPC/alternate branches, other CObjectCar phases, rendering, steering formulas, tire formulas, "
        "and native motor/brake delta scaling are unchanged.\n",
        build->name,
        static_cast<unsigned long>(build->vehiclePhysicsDispatchRva));
    AppendLog(text);

    if (!g_config.physXRealTimeAB)
    {
        AppendLog(
            "[VehicleXboxCadence] WARNING: Physics.PhysXRealTimeAB=false. Xbox-like car logic cadence is active, "
            "but native PC scene-0 PhysX time remains render-FPS-dependent; enable PhysXRealTimeAB for the intended combined repair.\n");
    }

    return true;
}
