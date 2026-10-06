#include "zachfix/render/frame_limiter.h"

#include "zachfix/core/logging.h"
#include "zachfix/render/long_session_audit.h"

#include <atomic>
#include <cstdint>
#include <cstdio>

namespace
{
constexpr DWORD kHighResolutionTimerFlag = 0x00000002u;
constexpr LONGLONG kHundredNanosecondsPerSecond = 10000000LL;
constexpr UINT kMinimumFrameRateLimit = 30;
constexpr UINT kMaximumFrameRateLimit = 240;

std::atomic<UINT> g_frameRateLimit{ 0 };
std::atomic<unsigned long long> g_frameLimiterGeneration{ 1 };

struct ThreadPacerState
{
    unsigned long long generation = 0;
    UINT framesPerSecond = 0;
    LONGLONG frequency = 0;
    LONGLONG deadline = 0;
    LONGLONG baseTicks = 0;
    UINT remainderTicks = 0;
    UINT remainderAccumulator = 0;
    HANDLE timer = nullptr;
    bool timerInitialized = false;

    ~ThreadPacerState()
    {
        if (timer != nullptr)
            CloseHandle(timer);
    }
};

thread_local ThreadPacerState g_pacer;

bool QueryClock(LONGLONG& counter, LONGLONG& frequency)
{
    LARGE_INTEGER current = {};
    LARGE_INTEGER freq = {};
    if (!QueryPerformanceCounter(&current) ||
        !QueryPerformanceFrequency(&freq) ||
        freq.QuadPart <= 0)
    {
        return false;
    }

    counter = current.QuadPart;
    frequency = freq.QuadPart;
    return true;
}

void AdvanceDeadline(ThreadPacerState& state)
{
    state.deadline += state.baseTicks;
    state.remainderAccumulator += state.remainderTicks;
    if (state.remainderAccumulator >= state.framesPerSecond)
    {
        ++state.deadline;
        state.remainderAccumulator -= state.framesPerSecond;
    }
}

HANDLE GetWaitTimer(ThreadPacerState& state)
{
    if (state.timerInitialized)
        return state.timer;

    state.timerInitialized = true;
    state.timer = CreateWaitableTimerExW(
        nullptr,
        nullptr,
        kHighResolutionTimerFlag,
        TIMER_MODIFY_STATE | SYNCHRONIZE);

    if (state.timer == nullptr)
    {
        // Older Windows builds may reject the high-resolution flag. A normal
        // waitable timer is still preferable to a fixed Sleep(16) loop, and
        // the final QPC spin below corrects short scheduling error.
        state.timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    }

    return state.timer;
}

void WaitUntil(ThreadPacerState& state, LONGLONG target)
{
    LARGE_INTEGER current = {};
    if (!QueryPerformanceCounter(&current))
        return;

    // Leave roughly the final 0.5 ms to QPC polling. The coarse wait avoids a
    // full-frame busy spin while the short tail keeps the deadline precise.
    const LONGLONG spinTicks = state.frequency / 2000;
    const LONGLONG coarseTarget = target - spinTicks;

    if (current.QuadPart < coarseTarget)
    {
        const LONGLONG remainingTicks = coarseTarget - current.QuadPart;
        LONGLONG relative100ns =
            (remainingTicks * kHundredNanosecondsPerSecond) / state.frequency;
        if (relative100ns < 1)
            relative100ns = 1;

        HANDLE timer = GetWaitTimer(state);
        if (timer != nullptr)
        {
            LARGE_INTEGER due = {};
            due.QuadPart = -relative100ns;
            if (SetWaitableTimer(timer, &due, 0, nullptr, nullptr, FALSE))
                WaitForSingleObject(timer, INFINITE);
        }
        else
        {
            const DWORD sleepMs = static_cast<DWORD>(
                (remainingTicks * 1000LL) / state.frequency);
            if (sleepMs > 1)
                Sleep(sleepMs - 1);
        }
    }

    do
    {
        if (!QueryPerformanceCounter(&current))
            return;

        if (current.QuadPart >= target)
            return;

        // Yield while there is still appreciable time left; the final few
        // scheduler quanta are intentionally a short spin for stable pacing.
        if (target - current.QuadPart > state.frequency / 10000)
            SwitchToThread();
    } while (true);
}

void ResetThreadSchedule(
    ThreadPacerState& state,
    unsigned long long generation,
    UINT framesPerSecond,
    LONGLONG now,
    LONGLONG frequency)
{
    state.generation = generation;
    state.framesPerSecond = framesPerSecond;
    state.frequency = frequency;
    state.deadline = now;
    state.baseTicks = frequency / framesPerSecond;
    state.remainderTicks = static_cast<UINT>(frequency % framesPerSecond);
    state.remainderAccumulator = 0;
    AdvanceDeadline(state);
}
}

UINT SetFrameRateLimit(UINT framesPerSecond)
{
    UINT effectiveLimit = framesPerSecond;
    if (effectiveLimit != 0 &&
        (effectiveLimit < kMinimumFrameRateLimit ||
         effectiveLimit > kMaximumFrameRateLimit))
    {
        char warning[192] = {};
        sprintf_s(
            warning,
            sizeof(warning),
            "[Display] WARNING: rejected invalid frame limit %u; limiter disabled.\n",
            effectiveLimit);
        AppendLog(warning);
        effectiveLimit = 0;
    }

    const UINT previous = g_frameRateLimit.exchange(
        effectiveLimit,
        std::memory_order_acq_rel);

    if (previous == effectiveLimit)
        return effectiveLimit;

    g_frameLimiterGeneration.fetch_add(1, std::memory_order_acq_rel);

    char text[160] = {};
    if (effectiveLimit == 0)
    {
        sprintf_s(text, sizeof(text), "[Display] Internal frame limiter disabled.\n");
    }
    else
    {
        sprintf_s(
            text,
            sizeof(text),
            "[Display] Internal frame limiter set to %u FPS (QPC deadline pacing).\n",
            effectiveLimit);
    }
    AppendLog(text);
    return effectiveLimit;
}

UINT GetFrameRateLimit()
{
    return g_frameRateLimit.load(std::memory_order_acquire);
}

void PaceFrameRateLimit()
{
    const UINT framesPerSecond = GetFrameRateLimit();
    if (framesPerSecond == 0)
    {
        g_pacer.generation = 0;
        return;
    }

    LONGLONG now = 0;
    LONGLONG frequency = 0;
    if (!QueryClock(now, frequency))
        return;

    const unsigned long long generation =
        g_frameLimiterGeneration.load(std::memory_order_acquire);

    if (g_pacer.generation != generation ||
        g_pacer.framesPerSecond != framesPerSecond ||
        g_pacer.frequency != frequency)
    {
        ResetThreadSchedule(
            g_pacer,
            generation,
            framesPerSecond,
            now,
            frequency);
        return;
    }

    // If frame production missed an entire period, do not perform catch-up
    // waits. Re-anchor the next presentation deadline to the current time.
    const LONGLONG oneFrame = g_pacer.baseTicks + 1;
    if (now > g_pacer.deadline + oneFrame)
    {
        g_pacer.deadline = now;
        g_pacer.remainderAccumulator = 0;
        AdvanceDeadline(g_pacer);
        return;
    }

    if (now < g_pacer.deadline)
    {
        const LONGLONG waitStart = now;
        WaitUntil(g_pacer, g_pacer.deadline);

        LARGE_INTEGER waitEnd = {};
        if (QueryPerformanceCounter(&waitEnd) && waitEnd.QuadPart > waitStart)
        {
            RecordLongSessionLimiterWait(
                waitEnd.QuadPart - waitStart,
                frequency);
        }
    }

    AdvanceDeadline(g_pacer);
}
