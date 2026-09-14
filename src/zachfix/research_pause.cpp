#include "research_pause.h"

#include "logging.h"
#include "main_exe.h"

#include <MinHook.h>
#include <intrin.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <mutex>

namespace
{
using QueryPerformanceCounterFn = BOOL (WINAPI*)(LARGE_INTEGER*);
using GetTickCountFn = DWORD (WINAPI*)();
using GetTickCount64Fn = ULONGLONG (WINAPI*)();
using TimeGetTimeFn = DWORD (WINAPI*)();

QueryPerformanceCounterFn g_originalQueryPerformanceCounter = nullptr;
GetTickCountFn g_originalGetTickCount = nullptr;
GetTickCount64Fn g_originalGetTickCount64 = nullptr;
TimeGetTimeFn g_originalTimeGetTime = nullptr;

std::atomic_bool g_pauseHooksInstalled{ false };
std::atomic_bool g_pauseActive{ false };
std::atomic_uint g_installedHookCount{ 0 };
std::mutex g_pauseTransitionMutex;
std::atomic_ullong g_qpcGameCalls{ 0 };
std::atomic_ullong g_tick32GameCalls{ 0 };
std::atomic_ullong g_tick64GameCalls{ 0 };
std::atomic_ullong g_timeGetTimeGameCalls{ 0 };

std::atomic_llong g_qpcOffset{ 0 };
std::atomic_llong g_qpcPauseReal{ 0 };
std::atomic_llong g_qpcFrozenVirtual{ 0 };

std::atomic_uint g_tick32Offset{ 0 };
std::atomic_uint g_tick32PauseReal{ 0 };
std::atomic_uint g_tick32FrozenVirtual{ 0 };

std::atomic_ullong g_tick64Offset{ 0 };
std::atomic_ullong g_tick64PauseReal{ 0 };
std::atomic_ullong g_tick64FrozenVirtual{ 0 };

std::atomic_uint g_timeGetTimeOffset{ 0 };
std::atomic_uint g_timeGetTimePauseReal{ 0 };
std::atomic_uint g_timeGetTimeFrozenVirtual{ 0 };

bool IsGameCaller(void* returnAddress)
{
    if (!g_mainExeInfoValid && !InitializeMainExeInfo())
        return false;

    const uintptr_t address = reinterpret_cast<uintptr_t>(returnAddress);
    return address >= g_mainExeBase && address < g_mainExeBase + g_mainExeSize;
}

BOOL WINAPI HookQueryPerformanceCounter(LARGE_INTEGER* value)
{
    if (value == nullptr || g_originalQueryPerformanceCounter == nullptr)
        return FALSE;

    if (!IsGameCaller(_ReturnAddress()))
        return g_originalQueryPerformanceCounter(value);

    g_qpcGameCalls.fetch_add(1, std::memory_order_relaxed);
    if (g_pauseActive.load(std::memory_order_acquire))
    {
        value->QuadPart = g_qpcFrozenVirtual.load(std::memory_order_relaxed);
        return TRUE;
    }

    LARGE_INTEGER real{};
    const BOOL ok = g_originalQueryPerformanceCounter(&real);
    if (ok)
        value->QuadPart = real.QuadPart - g_qpcOffset.load(std::memory_order_relaxed);
    return ok;
}

DWORD WINAPI HookGetTickCount()
{
    if (g_originalGetTickCount == nullptr)
        return 0;

    if (!IsGameCaller(_ReturnAddress()))
        return g_originalGetTickCount();

    g_tick32GameCalls.fetch_add(1, std::memory_order_relaxed);
    if (g_pauseActive.load(std::memory_order_acquire))
        return g_tick32FrozenVirtual.load(std::memory_order_relaxed);

    const DWORD real = g_originalGetTickCount();
    return real - g_tick32Offset.load(std::memory_order_relaxed);
}

ULONGLONG WINAPI HookGetTickCount64()
{
    if (g_originalGetTickCount64 == nullptr)
        return 0;

    if (!IsGameCaller(_ReturnAddress()))
        return g_originalGetTickCount64();

    g_tick64GameCalls.fetch_add(1, std::memory_order_relaxed);
    if (g_pauseActive.load(std::memory_order_acquire))
        return g_tick64FrozenVirtual.load(std::memory_order_relaxed);

    const ULONGLONG real = g_originalGetTickCount64();
    return real - g_tick64Offset.load(std::memory_order_relaxed);
}

DWORD WINAPI HookTimeGetTime()
{
    if (g_originalTimeGetTime == nullptr)
        return 0;

    if (!IsGameCaller(_ReturnAddress()))
        return g_originalTimeGetTime();

    g_timeGetTimeGameCalls.fetch_add(1, std::memory_order_relaxed);
    if (g_pauseActive.load(std::memory_order_acquire))
        return g_timeGetTimeFrozenVirtual.load(std::memory_order_relaxed);

    const DWORD real = g_originalTimeGetTime();
    return real - g_timeGetTimeOffset.load(std::memory_order_relaxed);
}

bool InstallOneHook(HMODULE module, const char* name, void* detour, void** original)
{
    if (module == nullptr)
        return false;

    FARPROC proc = GetProcAddress(module, name);
    if (proc == nullptr)
        return false;

    const MH_STATUS createStatus = MH_CreateHook(
        reinterpret_cast<void*>(proc), detour, original);
    if (createStatus != MH_OK && createStatus != MH_ERROR_ALREADY_CREATED)
    {
        char text[192] = {};
        sprintf_s(text, "[Pause] WARNING: MH_CreateHook(%s) failed: %d.\n",
                  name, static_cast<int>(createStatus));
        AppendLog(text);
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(reinterpret_cast<void*>(proc));
    if (enableStatus != MH_OK && enableStatus != MH_ERROR_ENABLED)
    {
        char text[192] = {};
        sprintf_s(text, "[Pause] WARNING: MH_EnableHook(%s) failed: %d.\n",
                  name, static_cast<int>(enableStatus));
        AppendLog(text);
        return false;
    }

    g_installedHookCount.fetch_add(1, std::memory_order_relaxed);
    return true;
}

void CapturePauseAnchors()
{
    if (g_originalQueryPerformanceCounter != nullptr)
    {
        LARGE_INTEGER real{};
        if (g_originalQueryPerformanceCounter(&real))
        {
            g_qpcPauseReal.store(real.QuadPart, std::memory_order_relaxed);
            g_qpcFrozenVirtual.store(
                real.QuadPart - g_qpcOffset.load(std::memory_order_relaxed),
                std::memory_order_relaxed);
        }
    }

    if (g_originalGetTickCount != nullptr)
    {
        const DWORD real = g_originalGetTickCount();
        g_tick32PauseReal.store(real, std::memory_order_relaxed);
        g_tick32FrozenVirtual.store(
            real - g_tick32Offset.load(std::memory_order_relaxed),
            std::memory_order_relaxed);
    }

    if (g_originalGetTickCount64 != nullptr)
    {
        const ULONGLONG real = g_originalGetTickCount64();
        g_tick64PauseReal.store(real, std::memory_order_relaxed);
        g_tick64FrozenVirtual.store(
            real - g_tick64Offset.load(std::memory_order_relaxed),
            std::memory_order_relaxed);
    }

    if (g_originalTimeGetTime != nullptr)
    {
        const DWORD real = g_originalTimeGetTime();
        g_timeGetTimePauseReal.store(real, std::memory_order_relaxed);
        g_timeGetTimeFrozenVirtual.store(
            real - g_timeGetTimeOffset.load(std::memory_order_relaxed),
            std::memory_order_relaxed);
    }
}

void AccumulatePausedDuration()
{
    if (g_originalQueryPerformanceCounter != nullptr)
    {
        LARGE_INTEGER real{};
        if (g_originalQueryPerformanceCounter(&real))
        {
            const LONGLONG elapsed =
                real.QuadPart - g_qpcPauseReal.load(std::memory_order_relaxed);
            if (elapsed > 0)
                g_qpcOffset.fetch_add(elapsed, std::memory_order_relaxed);
        }
    }

    if (g_originalGetTickCount != nullptr)
    {
        const DWORD real = g_originalGetTickCount();
        const DWORD elapsed = real - g_tick32PauseReal.load(std::memory_order_relaxed);
        g_tick32Offset.fetch_add(elapsed, std::memory_order_relaxed);
    }

    if (g_originalGetTickCount64 != nullptr)
    {
        const ULONGLONG real = g_originalGetTickCount64();
        const ULONGLONG elapsed = real - g_tick64PauseReal.load(std::memory_order_relaxed);
        g_tick64Offset.fetch_add(elapsed, std::memory_order_relaxed);
    }

    if (g_originalTimeGetTime != nullptr)
    {
        const DWORD real = g_originalTimeGetTime();
        const DWORD elapsed = real - g_timeGetTimePauseReal.load(std::memory_order_relaxed);
        g_timeGetTimeOffset.fetch_add(elapsed, std::memory_order_relaxed);
    }
}
} // namespace

bool InitializeResearchPauseHooks()
{
    if (g_pauseHooksInstalled.load(std::memory_order_acquire))
        return true;

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Pause] WARNING: DP.exe info unavailable; gameplay timer pause disabled.\n");
        return false;
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    HMODULE winmm = GetModuleHandleW(L"winmm.dll");
    if (winmm == nullptr)
        winmm = LoadLibraryW(L"winmm.dll");

    InstallOneHook(
        kernel32, "QueryPerformanceCounter",
        reinterpret_cast<void*>(&HookQueryPerformanceCounter),
        reinterpret_cast<void**>(&g_originalQueryPerformanceCounter));
    InstallOneHook(
        kernel32, "GetTickCount",
        reinterpret_cast<void*>(&HookGetTickCount),
        reinterpret_cast<void**>(&g_originalGetTickCount));
    InstallOneHook(
        kernel32, "GetTickCount64",
        reinterpret_cast<void*>(&HookGetTickCount64),
        reinterpret_cast<void**>(&g_originalGetTickCount64));
    InstallOneHook(
        winmm, "timeGetTime",
        reinterpret_cast<void*>(&HookTimeGetTime),
        reinterpret_cast<void**>(&g_originalTimeGetTime));

    const UINT count = g_installedHookCount.load(std::memory_order_relaxed);
    const bool available = count != 0;
    g_pauseHooksInstalled.store(available, std::memory_order_release);

    char text[224] = {};
    sprintf_s(text,
              available
                  ? "[Pause] Gameplay timer freeze ready (%u timer hooks). Known limitation: some cutscenes can hang.\n"
                  : "[Pause] WARNING: No supported game timer hooks were installed.\n",
              count);
    AppendLog(text);
    return available;
}

void SetResearchPauseActive(bool active)
{
    std::lock_guard<std::mutex> lock(g_pauseTransitionMutex);

    if (active == g_pauseActive.load(std::memory_order_acquire))
        return;

    if (active)
    {
        if (!g_pauseHooksInstalled.load(std::memory_order_acquire) &&
            !InitializeResearchPauseHooks())
        {
            return;
        }

        CapturePauseAnchors();
        g_pauseActive.store(true, std::memory_order_release);
        AppendLog("[Pause] Gameplay timer freeze enabled for DP.exe timer calls.\n");
        return;
    }

    // Keep DP's virtual game clock continuous after tuning. Otherwise the game
    // would observe the whole tuning interval as one giant frame delta.
    AccumulatePausedDuration();
    g_pauseActive.store(false, std::memory_order_release);
    AppendLog("[Pause] Gameplay timer freeze disabled; paused duration removed from virtual clocks.\n");
}

bool IsResearchPauseActive()
{
    return g_pauseActive.load(std::memory_order_acquire);
}

ResearchPauseStats GetResearchPauseStats()
{
    ResearchPauseStats stats{};
    stats.hooksInstalled = g_pauseHooksInstalled.load(std::memory_order_acquire);
    stats.active = g_pauseActive.load(std::memory_order_acquire);
    stats.installedHooks = g_installedHookCount.load(std::memory_order_relaxed);
    stats.qpcCalls = g_qpcGameCalls.load(std::memory_order_relaxed);
    stats.tick32Calls = g_tick32GameCalls.load(std::memory_order_relaxed);
    stats.tick64Calls = g_tick64GameCalls.load(std::memory_order_relaxed);
    stats.timeGetTimeCalls = g_timeGetTimeGameCalls.load(std::memory_order_relaxed);
    return stats;
}
