#include "physics_realtime_ab.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
static_assert(sizeof(void*) == 4, "PhysX 2.8.1 desktop ABI requires the Win32 build");

constexpr float kGameDeltaUnitsPerSecond = 60.0f;
constexpr int kFixedPhysicsMaxSubsteps = 4;
constexpr int kPlayerSceneIndex = 0;
constexpr unsigned int kMaxLoggedWindows = 30;
constexpr uintptr_t kSceneGetTimingVtableOffset = 0x14C;
constexpr uintptr_t kSceneSimulateVtableOffset = 0x230;
constexpr float kSpecialSimulateThreshold = 0.100001f;
constexpr float kDeltaEpsilon = 0.000001f;

using QueuePhysicsSceneFn = void (__cdecl*)(void* scene, float normalizedDelta, int maxIter, int sceneIndex);
using ConfigurePhysicsTimingFn = void (__cdecl*)(void* scene, int maxIter, int sceneIndex);
using SceneSimulateFn = void (__thiscall*)(void* scene, float elapsedTime);
using SceneGetTimingFn = void (__thiscall*)(
    const void* scene,
    float* maxTimestep,
    uint32_t* maxIter,
    int* method,
    uint32_t* numSubSteps);

QueuePhysicsSceneFn g_originalQueuePhysicsScene = nullptr;
ConfigurePhysicsTimingFn g_originalConfigurePhysicsTiming = nullptr;
SceneSimulateFn g_originalSceneSimulate = nullptr;

bool g_realTimeABEnabled = false;
LARGE_INTEGER g_qpcFrequency{};
LARGE_INTEGER g_previousScene0QueueQpc{};
bool g_havePreviousScene0QueueQpc = false;

std::atomic<uintptr_t> g_scene0Address{ 0 };
std::atomic_bool g_simulateHookAttempted{ false };
std::atomic_bool g_simulateHookInstalled{ false };
std::atomic<uint64_t> g_simulateCalls{ 0 };
std::atomic<uint64_t> g_specialSimulateCalls{ 0 };
std::atomic<uint64_t> g_simulateElapsedNs{ 0 };

std::atomic<uint64_t> g_configureCalls{ 0 };
std::atomic<int> g_lastConfigureNativeMaxIter{ 0 };
std::atomic<int> g_lastConfigureEffectiveMaxIter{ 0 };

ULONGLONG g_windowStartMs = 0;
unsigned int g_loggedWindows = 0;

struct ScalarStats
{
    uint64_t count = 0;
    double sum = 0.0;
    float min = 0.0f;
    float max = 0.0f;

    void Add(float value)
    {
        if (!std::isfinite(value))
            return;
        if (count == 0)
        {
            min = value;
            max = value;
        }
        else
        {
            min = std::min(min, value);
            max = std::max(max, value);
        }
        sum += static_cast<double>(value);
        ++count;
    }

    double Average() const
    {
        return count != 0 ? sum / static_cast<double>(count) : 0.0;
    }

    void Reset()
    {
        count = 0;
        sum = 0.0;
        min = 0.0f;
        max = 0.0f;
    }
};

struct QueueWindowStats
{
    uint64_t calls = 0;
    uint64_t wallClockCalls = 0;
    uint64_t nativeSpecialCalls = 0;
    ScalarStats rawDelta60;
    ScalarStats convertedSeconds;
    ScalarStats wallSeconds;
    ScalarStats passedElapsed;
    ScalarStats nativeMaxIter;

    void Reset()
    {
        calls = 0;
        wallClockCalls = 0;
        nativeSpecialCalls = 0;
        rawDelta60.Reset();
        convertedSeconds.Reset();
        wallSeconds.Reset();
        passedElapsed.Reset();
        nativeMaxIter.Reset();
    }
};

QueueWindowStats g_queueStats{};

bool MatchesBytes(uintptr_t address, const unsigned char* expected, size_t size)
{
    return address != 0 &&
        std::memcmp(reinterpret_cast<const void*>(address), expected, size) == 0;
}

void* VtableMethod(void* object, uintptr_t byteOffset)
{
    if (object == nullptr)
        return nullptr;
    void** vtable = *reinterpret_cast<void***>(object);
    if (vtable == nullptr)
        return nullptr;
    return vtable[byteOffset / sizeof(void*)];
}

bool IsOrdinaryUpdate(float normalizedDelta, int maxIter)
{
    return std::isfinite(normalizedDelta) && normalizedDelta > kDeltaEpsilon &&
        maxIter >= 1 && maxIter <= kFixedPhysicsMaxSubsteps;
}

float MeasureScene0WallSeconds(float fallbackSeconds)
{
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    float result = fallbackSeconds;
    if (g_havePreviousScene0QueueQpc && g_qpcFrequency.QuadPart > 0)
    {
        const long long ticks = now.QuadPart - g_previousScene0QueueQpc.QuadPart;
        if (ticks > 0)
        {
            const double seconds = static_cast<double>(ticks) /
                static_cast<double>(g_qpcFrequency.QuadPart);
            if (seconds > 0.0 && seconds < 10.0 && std::isfinite(seconds))
                result = static_cast<float>(seconds);
        }
    }

    g_previousScene0QueueQpc = now;
    g_havePreviousScene0QueueQpc = true;
    return result;
}

void ReadScene0Timing(float& step, uint32_t& maxIter, int& method, uint32_t& subSteps, bool& valid)
{
    valid = false;
    void* scene = reinterpret_cast<void*>(g_scene0Address.load(std::memory_order_acquire));
    if (scene == nullptr)
        return;

    void* methodAddress = VtableMethod(scene, kSceneGetTimingVtableOffset);
    if (methodAddress == nullptr)
        return;

    const auto getTiming = reinterpret_cast<SceneGetTimingFn>(methodAddress);
    getTiming(scene, &step, &maxIter, &method, &subSteps);
    valid = true;
}

void __fastcall HookSceneSimulate(void* scene, void*, float elapsedTime)
{
    const uintptr_t sceneAddress = reinterpret_cast<uintptr_t>(scene);
    if (sceneAddress != 0 && sceneAddress == g_scene0Address.load(std::memory_order_relaxed))
    {
        g_simulateCalls.fetch_add(1, std::memory_order_relaxed);
        if (elapsedTime > kSpecialSimulateThreshold)
            g_specialSimulateCalls.fetch_add(1, std::memory_order_relaxed);

        if (std::isfinite(elapsedTime) && elapsedTime >= 0.0f)
        {
            const double ns = static_cast<double>(elapsedTime) * 1000000000.0;
            if (ns >= 0.0 && ns <= static_cast<double>(UINT64_MAX))
                g_simulateElapsedNs.fetch_add(static_cast<uint64_t>(ns + 0.5), std::memory_order_relaxed);
        }
    }

    g_originalSceneSimulate(scene, elapsedTime);
}

void EnsureSimulateHook(void* scene)
{
    if (scene == nullptr || g_simulateHookInstalled.load(std::memory_order_acquire))
        return;
    if (g_simulateHookAttempted.exchange(true, std::memory_order_acq_rel))
        return;

    void* target = VtableMethod(scene, kSceneSimulateVtableOffset);
    if (target == nullptr)
    {
        AppendLog("[PhysXRTAB] WARNING: live NxScene::simulate slot was null; simulate telemetry unavailable.\n");
        return;
    }

    MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookSceneSimulate),
        reinterpret_cast<void**>(&g_originalSceneSimulate));
    if (status != MH_OK)
    {
        char text[224] = {};
        sprintf_s(text,
            "[PhysXRTAB] WARNING: MH_CreateHook(NxScene::simulate=%p) failed: %d; queue telemetry remains active.\n",
            target, static_cast<int>(status));
        AppendLog(text);
        return;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        MH_RemoveHook(target);
        g_originalSceneSimulate = nullptr;
        char text[224] = {};
        sprintf_s(text,
            "[PhysXRTAB] WARNING: MH_EnableHook(NxScene::simulate=%p) failed: %d; queue telemetry remains active.\n",
            target, static_cast<int>(status));
        AppendLog(text);
        return;
    }

    g_simulateHookInstalled.store(true, std::memory_order_release);
    char text[224] = {};
    sprintf_s(text,
        "[PhysXRTAB] Low-overhead scene-0 NxScene::simulate telemetry armed at %p (vtable +0x230).\n",
        target);
    AppendLog(text);
}

void LogWindowIfDue()
{
    if (g_loggedWindows >= kMaxLoggedWindows)
        return;

    const ULONGLONG now = GetTickCount64();
    if (g_windowStartMs == 0)
    {
        g_windowStartMs = now;
        g_queueStats.Reset();
        g_simulateCalls.store(0, std::memory_order_relaxed);
        g_specialSimulateCalls.store(0, std::memory_order_relaxed);
        g_simulateElapsedNs.store(0, std::memory_order_relaxed);
        g_configureCalls.store(0, std::memory_order_relaxed);
        return;
    }

    const ULONGLONG elapsedMs = now - g_windowStartMs;
    if (elapsedMs < 1000)
        return;

    const QueueWindowStats snapshot = g_queueStats;
    g_queueStats.Reset();
    g_windowStartMs = now;
    const unsigned int windowNumber = ++g_loggedWindows;

    const uint64_t simulateCalls = g_simulateCalls.exchange(0, std::memory_order_acq_rel);
    const uint64_t specialSimulateCalls = g_specialSimulateCalls.exchange(0, std::memory_order_acq_rel);
    const uint64_t simulateElapsedNs = g_simulateElapsedNs.exchange(0, std::memory_order_acq_rel);
    const uint64_t configureCalls = g_configureCalls.exchange(0, std::memory_order_acq_rel);
    const int nativeConfigureMaxIter = g_lastConfigureNativeMaxIter.load(std::memory_order_relaxed);
    const int effectiveConfigureMaxIter = g_lastConfigureEffectiveMaxIter.load(std::memory_order_relaxed);

    float timingStep = 0.0f;
    uint32_t timingMaxIter = 0;
    int timingMethod = -1;
    uint32_t timingSubSteps = 0;
    bool timingValid = false;
    ReadScene0Timing(timingStep, timingMaxIter, timingMethod, timingSubSteps, timingValid);

    const double seconds = static_cast<double>(elapsedMs) / 1000.0;
    const double invSeconds = seconds > 0.0 ? 1.0 / seconds : 0.0;
    const double simElapsedAverage = simulateCalls != 0
        ? (static_cast<double>(simulateElapsedNs) / 1000000000.0) / static_cast<double>(simulateCalls)
        : 0.0;

    char text[1500] = {};
    sprintf_s(
        text,
        "[PhysXRTAB] window=%u elapsed=%llums mode=%s scene=0 ptr=%p "
        "queueHz=%.1f wallClockHz=%.1f nativeSpecialHz=%.1f rawDelta60 avg/min/max=%.6f/%.6f/%.6f "
        "legacySec avg=%.7f wallSec avg/min/max=%.7f/%.7f/%.7f passed avg/min/max=%.7f/%.7f/%.7f "
        "queueNativeMaxIter avg/min/max=%.2f/%.0f/%.0f configHz=%.1f native/effectiveMaxIter=%d/%d "
        "timing step=%.8f maxIter=%u method=%d subSteps=%u simulateHz=%.1f specialHz=%.1f simElapsedAvg=%.7f.\n",
        windowNumber,
        static_cast<unsigned long long>(elapsedMs),
        g_realTimeABEnabled ? "REALTIME-QPC" : "NATIVE",
        reinterpret_cast<void*>(g_scene0Address.load(std::memory_order_relaxed)),
        static_cast<double>(snapshot.calls) * invSeconds,
        static_cast<double>(snapshot.wallClockCalls) * invSeconds,
        static_cast<double>(snapshot.nativeSpecialCalls) * invSeconds,
        snapshot.rawDelta60.Average(),
        static_cast<double>(snapshot.rawDelta60.min),
        static_cast<double>(snapshot.rawDelta60.max),
        snapshot.convertedSeconds.Average(),
        snapshot.wallSeconds.Average(),
        static_cast<double>(snapshot.wallSeconds.min),
        static_cast<double>(snapshot.wallSeconds.max),
        snapshot.passedElapsed.Average(),
        static_cast<double>(snapshot.passedElapsed.min),
        static_cast<double>(snapshot.passedElapsed.max),
        snapshot.nativeMaxIter.Average(),
        static_cast<double>(snapshot.nativeMaxIter.min),
        static_cast<double>(snapshot.nativeMaxIter.max),
        static_cast<double>(configureCalls) * invSeconds,
        nativeConfigureMaxIter,
        effectiveConfigureMaxIter,
        timingValid ? static_cast<double>(timingStep) : 0.0,
        timingValid ? timingMaxIter : 0,
        timingValid ? timingMethod : -1,
        timingValid ? timingSubSteps : 0,
        static_cast<double>(simulateCalls) * invSeconds,
        static_cast<double>(specialSimulateCalls) * invSeconds,
        simElapsedAverage);
    AppendLog(text);
}

void __cdecl HookQueuePhysicsScene(void* scene, float normalizedDelta, int maxIter, int sceneIndex)
{
    if (sceneIndex != kPlayerSceneIndex)
    {
        g_originalQueuePhysicsScene(scene, normalizedDelta, maxIter, sceneIndex);
        return;
    }

    g_scene0Address.store(reinterpret_cast<uintptr_t>(scene), std::memory_order_release);
    EnsureSimulateHook(scene);

    const float convertedSeconds = normalizedDelta / kGameDeltaUnitsPerSecond;
    const float wallSeconds = MeasureScene0WallSeconds(convertedSeconds);
    const bool ordinaryUpdate = IsOrdinaryUpdate(normalizedDelta, maxIter);

    float passedElapsed = normalizedDelta;
    if (g_realTimeABEnabled)
    {
        if (std::fabs(normalizedDelta) <= kDeltaEpsilon)
        {
            passedElapsed = 0.0f;
        }
        else if (ordinaryUpdate)
        {
            passedElapsed = wallSeconds;
        }
        // Reset/special calls outside the ordinary 1..4 update contract retain
        // their native units/semantics rather than being silently reinterpreted.
    }

    ++g_queueStats.calls;
    g_queueStats.rawDelta60.Add(normalizedDelta);
    g_queueStats.convertedSeconds.Add(convertedSeconds);
    g_queueStats.nativeMaxIter.Add(static_cast<float>(maxIter));
    if (ordinaryUpdate)
    {
        ++g_queueStats.wallClockCalls;
        g_queueStats.wallSeconds.Add(wallSeconds);
    }
    else
    {
        ++g_queueStats.nativeSpecialCalls;
    }
    g_queueStats.passedElapsed.Add(passedElapsed);

    g_originalQueuePhysicsScene(scene, passedElapsed, maxIter, sceneIndex);
    LogWindowIfDue();
}

void __cdecl HookConfigurePhysicsTiming(void* scene, int maxIter, int sceneIndex)
{
    if (sceneIndex != kPlayerSceneIndex)
    {
        g_originalConfigurePhysicsTiming(scene, maxIter, sceneIndex);
        return;
    }

    const int effectiveMaxIter =
        g_realTimeABEnabled && maxIter >= 1 && maxIter <= kFixedPhysicsMaxSubsteps
            ? kFixedPhysicsMaxSubsteps
            : maxIter;

    g_lastConfigureNativeMaxIter.store(maxIter, std::memory_order_relaxed);
    g_lastConfigureEffectiveMaxIter.store(effectiveMaxIter, std::memory_order_relaxed);
    g_configureCalls.fetch_add(1, std::memory_order_relaxed);
    g_originalConfigurePhysicsTiming(scene, effectiveMaxIter, sceneIndex);
}
} // namespace

bool InstallPhysXRealTimeAB()
{
    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
    {
        AppendLog("[PhysXRTAB] ERROR: unsupported DP.exe build; PhysX Real-Time A/B v2.1 not installed.\n");
        return false;
    }

    g_realTimeABEnabled = g_config.physXRealTimeAB;
    QueryPerformanceFrequency(&g_qpcFrequency);

    const uintptr_t queueAddress = g_mainExeBase + build->physicsQueueSceneRva;
    const uintptr_t timingAddress = g_mainExeBase + build->physicsConfigureTimingRva;

    static constexpr unsigned char kQueueSignature[] = {
        0xA1, 0x08, 0xA0, 0xBD, 0x00,
        0x25, 0x03, 0x00, 0x00, 0x80,
        0x79, 0x05, 0x48, 0x83, 0xC8, 0xFC, 0x40
    };
    static constexpr unsigned char kTimingSignature[] = {
        0x55, 0x8B, 0xEC,
        0x81, 0xEC, 0xF4, 0x00, 0x00, 0x00
    };

    if (!MatchesBytes(queueAddress, kQueueSignature, sizeof(kQueueSignature)) ||
        !MatchesBytes(timingAddress, kTimingSignature, sizeof(kTimingSignature)))
    {
        AppendLog("[PhysXRTAB] ERROR: physics queue/configure signature mismatch; A/B v2.1 not installed.\n");
        return false;
    }

    MH_STATUS status = MH_CreateHook(
        reinterpret_cast<void*>(queueAddress),
        reinterpret_cast<void*>(&HookQueuePhysicsScene),
        reinterpret_cast<void**>(&g_originalQueuePhysicsScene));
    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(text, "[PhysXRTAB] ERROR: MH_CreateHook(queue) failed: %d.\n", static_cast<int>(status));
        AppendLog(text);
        return false;
    }

    status = MH_CreateHook(
        reinterpret_cast<void*>(timingAddress),
        reinterpret_cast<void*>(&HookConfigurePhysicsTiming),
        reinterpret_cast<void**>(&g_originalConfigurePhysicsTiming));
    if (status != MH_OK)
    {
        MH_RemoveHook(reinterpret_cast<void*>(queueAddress));
        char text[192] = {};
        sprintf_s(text, "[PhysXRTAB] ERROR: MH_CreateHook(configure) failed: %d.\n", static_cast<int>(status));
        AppendLog(text);
        return false;
    }

    status = MH_EnableHook(reinterpret_cast<void*>(timingAddress));
    if (status != MH_OK)
    {
        MH_RemoveHook(reinterpret_cast<void*>(timingAddress));
        MH_RemoveHook(reinterpret_cast<void*>(queueAddress));
        AppendLog("[PhysXRTAB] ERROR: configure hook enable failed; A/B v2.1 rolled back.\n");
        return false;
    }

    status = MH_EnableHook(reinterpret_cast<void*>(queueAddress));
    if (status != MH_OK)
    {
        MH_DisableHook(reinterpret_cast<void*>(timingAddress));
        MH_RemoveHook(reinterpret_cast<void*>(timingAddress));
        MH_RemoveHook(reinterpret_cast<void*>(queueAddress));
        AppendLog("[PhysXRTAB] ERROR: queue hook enable failed; A/B v2.1 rolled back.\n");
        return false;
    }

    char text[640] = {};
    sprintf_s(
        text,
        "[PhysXRTAB] PhysX Real-Time A/B v2.1 armed on %s: mode=%s, queue=DP.exe+0x%08lX, configure=DP.exe+0x%08lX. "
        "Only scene 0 is mutated/telemetred. REALTIME uses QPC wall time for ordinary non-zero updates, keeps zero/reset/special queue semantics native, "
        "and promotes ordinary scene-0 maxIter 1..4 to 4.\n",
        build->name,
        g_realTimeABEnabled ? "REALTIME-QPC" : "NATIVE",
        static_cast<unsigned long>(build->physicsQueueSceneRva),
        static_cast<unsigned long>(build->physicsConfigureTimingRva));
    AppendLog(text);
    return true;
}
