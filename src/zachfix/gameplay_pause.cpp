#include "gameplay_pause.h"

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

struct PauseHookEntry
{
    HMODULE module = nullptr;
    const char* name = nullptr;
    void* detour = nullptr;
    void** original = nullptr;
    void* target = nullptr;
};

bool PauseHookTrampolinesAreClear(PauseHookEntry* hooks, size_t hookCount)
{
    for (size_t i = 0; i < hookCount; ++i)
    {
        if (hooks[i].original != nullptr && *hooks[i].original != nullptr)
            return false;
    }
    return true;
}

bool ResolvePauseHookTargets(PauseHookEntry* hooks, size_t hookCount)
{
    for (size_t i = 0; i < hookCount; ++i)
    {
        PauseHookEntry& entry = hooks[i];
        if (entry.module == nullptr)
        {
            char text[192] = {};
            sprintf_s(text, "[Pause] WARNING: Module unavailable for %s; timer hook transaction aborted.\n",
                      entry.name);
            AppendLog(text);
            return false;
        }

        FARPROC proc = GetProcAddress(entry.module, entry.name);
        if (proc == nullptr)
        {
            char text[192] = {};
            sprintf_s(text, "[Pause] WARNING: GetProcAddress(%s) failed; timer hook transaction aborted.\n",
                      entry.name);
            AppendLog(text);
            return false;
        }

        entry.target = reinterpret_cast<void*>(proc);
    }

    return true;
}

bool RemoveCreatedPauseHooks(PauseHookEntry* hooks, size_t createdCount)
{
    bool rollbackClean = true;
    for (size_t i = 0; i < createdCount; ++i)
    {
        if (hooks[i].target == nullptr)
            continue;

        const MH_STATUS removeStatus = MH_RemoveHook(hooks[i].target);
        if (removeStatus == MH_OK || removeStatus == MH_ERROR_NOT_CREATED)
        {
            if (hooks[i].original != nullptr)
                *hooks[i].original = nullptr;
            continue;
        }

        rollbackClean = false;
        char text[224] = {};
        sprintf_s(text,
                  "[Pause] ERROR: Timer hook rollback could not remove %s (%d); trampoline retained.\n",
                  hooks[i].name, static_cast<int>(removeStatus));
        AppendLog(text);
    }

    return rollbackClean;
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

bool InitializeGameplayPauseHooks()
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

    PauseHookEntry hooks[] = {
        { kernel32, "QueryPerformanceCounter",
          reinterpret_cast<void*>(&HookQueryPerformanceCounter),
          reinterpret_cast<void**>(&g_originalQueryPerformanceCounter) },
        { kernel32, "GetTickCount",
          reinterpret_cast<void*>(&HookGetTickCount),
          reinterpret_cast<void**>(&g_originalGetTickCount) },
        { kernel32, "GetTickCount64",
          reinterpret_cast<void*>(&HookGetTickCount64),
          reinterpret_cast<void**>(&g_originalGetTickCount64) },
        { winmm, "timeGetTime",
          reinterpret_cast<void*>(&HookTimeGetTime),
          reinterpret_cast<void**>(&g_originalTimeGetTime) }
    };
    constexpr size_t hookCount = sizeof(hooks) / sizeof(hooks[0]);

    g_pauseHooksInstalled.store(false, std::memory_order_release);
    g_installedHookCount.store(0, std::memory_order_relaxed);

    // A non-null trampoline while the transaction is not marked installed can
    // only mean an earlier rollback could not remove one of its hooks. Do not
    // overwrite that trampoline or attempt a second transaction on top of it.
    if (!PauseHookTrampolinesAreClear(hooks, hookCount))
    {
        AppendLog("[Pause] ERROR: Residual timer hook detected after rollback; refusing to retry pause hook installation.\n");
        return false;
    }

    // Phase 0: resolve the complete required timer set before touching MinHook.
    // A missing API/module must leave DP's native clocks entirely untouched.
    if (!ResolvePauseHookTargets(hooks, hookCount))
        return false;

    // Phase 1: create every required hook before enabling any of them. This
    // guarantees that a create failure cannot leave a partially-active timer
    // virtualization set behind.
    size_t createdCount = 0;
    for (size_t i = 0; i < hookCount; ++i)
    {
        PauseHookEntry& entry = hooks[i];
        const MH_STATUS createStatus = MH_CreateHook(
            entry.target, entry.detour, entry.original);
        if (createStatus != MH_OK)
        {
            char text[224] = {};
            sprintf_s(text,
                      "[Pause] WARNING: MH_CreateHook(%s) failed: %d; rolling back timer hook transaction.\n",
                      entry.name, static_cast<int>(createStatus));
            AppendLog(text);

            RemoveCreatedPauseHooks(hooks, createdCount);
            return false;
        }

        ++createdCount;
    }

    // Phase 2: enable the fully-created set. If one enable fails, disable the
    // already-enabled subset first and then remove every hook from this batch.
    size_t enabledCount = 0;
    for (size_t i = 0; i < hookCount; ++i)
    {
        PauseHookEntry& entry = hooks[i];
        const MH_STATUS enableStatus = MH_EnableHook(entry.target);
        if (enableStatus != MH_OK)
        {
            char text[224] = {};
            sprintf_s(text,
                      "[Pause] WARNING: MH_EnableHook(%s) failed: %d; rolling back timer hook transaction.\n",
                      entry.name, static_cast<int>(enableStatus));
            AppendLog(text);

            for (size_t j = 0; j < enabledCount; ++j)
            {
                const MH_STATUS disableStatus = MH_DisableHook(hooks[j].target);
                if (disableStatus != MH_OK && disableStatus != MH_ERROR_DISABLED)
                {
                    char rollbackText[224] = {};
                    sprintf_s(rollbackText,
                              "[Pause] ERROR: Timer hook rollback could not disable %s (%d).\n",
                              hooks[j].name, static_cast<int>(disableStatus));
                    AppendLog(rollbackText);
                }
            }

            RemoveCreatedPauseHooks(hooks, createdCount);
            return false;
        }

        ++enabledCount;
    }

    // Publish availability only after the complete timer set is live.
    g_installedHookCount.store(static_cast<UINT>(hookCount), std::memory_order_relaxed);
    g_pauseHooksInstalled.store(true, std::memory_order_release);

    char text[224] = {};
    sprintf_s(text,
              "[Pause] Gameplay timer freeze ready (%u timer hooks; transaction committed). Known limitation: some cutscenes can hang.\n",
              static_cast<unsigned>(hookCount));
    AppendLog(text);
    return true;
}

void SetGameplayPauseActive(bool active)
{
    std::lock_guard<std::mutex> lock(g_pauseTransitionMutex);

    if (active == g_pauseActive.load(std::memory_order_acquire))
        return;

    if (active)
    {
        if (!g_pauseHooksInstalled.load(std::memory_order_acquire) &&
            !InitializeGameplayPauseHooks())
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

GameplayPauseStats GetGameplayPauseStats()
{
    GameplayPauseStats stats{};
    stats.hooksInstalled = g_pauseHooksInstalled.load(std::memory_order_acquire);
    stats.active = g_pauseActive.load(std::memory_order_acquire);
    stats.installedHooks = g_installedHookCount.load(std::memory_order_relaxed);
    stats.qpcCalls = g_qpcGameCalls.load(std::memory_order_relaxed);
    stats.tick32Calls = g_tick32GameCalls.load(std::memory_order_relaxed);
    stats.tick64Calls = g_tick64GameCalls.load(std::memory_order_relaxed);
    stats.timeGetTimeCalls = g_timeGetTimeGameCalls.load(std::memory_order_relaxed);
    return stats;
}
