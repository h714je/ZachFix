#include "zachfix/gameplay/vanilla_nan_fix.h"

#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
// Original instructions (16 bytes) at the build-specific speed-divide site:
//   fld  dword ptr [esp+10h]
//   fdiv dword ptr [014AFFE0h]
//   fstp dword ptr [esi+4E4h]
constexpr unsigned char kExpectedSpeedDivideBytes[] = {
    0xD9, 0x44, 0x24, 0x10,
    0xD8, 0x35, 0xE0, 0xFF, 0x4A, 0x01,
    0xD9, 0x9E, 0xE4, 0x04, 0x00, 0x00
};

volatile LONG g_zeroDeltaNaNPrevented = 0;
LONG g_zeroDeltaNaNLastLogged = 0;
LONG g_zeroDeltaNaNLastObserved = 0;
unsigned int g_zeroDeltaNaNQuietPolls = 0;
uintptr_t g_zeroDeltaNaNPatchRva = 0;

// A bad frame can repeat the exact zero-delta condition for dozens of Presents.
// Keep the first hit visible, aggregate active bursts, then flush any tail
// after a short quiet period so the final total still reaches the log.
constexpr LONG kZeroDeltaNaNLogBatch = 16;
constexpr unsigned int kZeroDeltaNaNQuietFlushPolls = 60;

void Emit8(unsigned char*& cursor, unsigned char value)
{
    *cursor++ = value;
}

void Emit32(unsigned char*& cursor, std::uint32_t value)
{
    std::memcpy(cursor, &value, sizeof(value));
    cursor += sizeof(value);
}

void EmitRel32(unsigned char*& cursor, unsigned char opcode, const void* destination)
{
    Emit8(cursor, opcode);

    const uintptr_t after =
        reinterpret_cast<uintptr_t>(cursor + sizeof(std::uint32_t));
    const uintptr_t dest = reinterpret_cast<uintptr_t>(destination);
    const std::uint32_t rel = static_cast<std::uint32_t>(dest - after);

    Emit32(cursor, rel);
}

bool BuildZeroDeltaStub(
    unsigned char* stub,
    size_t stubCapacity,
    uintptr_t frameDeltaAddress,
    uintptr_t returnAddress)
{
    if (stub == nullptr || stubCapacity < 96)
        return false;

    unsigned char* cursor = stub;

    // Preserve the integer state that the replaced x87-only sequence did not
    // modify. The two pushes shift original [esp+10h] to [esp+18h].
    Emit8(cursor, 0x9C); // pushfd
    Emit8(cursor, 0x50); // push eax

    // mov eax,[frameDeltaAddress] ; abs(dt bits) == 0 ?
    Emit8(cursor, 0xA1); Emit32(cursor, static_cast<std::uint32_t>(frameDeltaAddress));
    Emit8(cursor, 0x25); Emit32(cursor, 0x7FFFFFFFu);

    // jne normalPath (short jump; patched once the label is known)
    Emit8(cursor, 0x75);
    unsigned char* jneDeltaDisp = cursor++;

    // DP's timing loop stores frameDelta as elapsedSeconds * 60, so 1.0f is
    // one nominal 60 Hz update. If the timer reports an exact +/-0 delta,
    // preserve the measured planar displacement as the one-tick movement rate
    // instead of executing distance / 0. This covers both the old 0/0 -> NaN
    // case and the now-confirmed finite/0 -> INF path that can become NaN later.
    //
    // distance / 1.0f is exactly distance, so copy the numerator directly.
    Emit8(cursor, 0xD9); Emit8(cursor, 0x44); Emit8(cursor, 0x24); Emit8(cursor, 0x18);
    Emit8(cursor, 0xD9); Emit8(cursor, 0x9E); Emit32(cursor, 0x000004E4u); // fstp [esi+4E4]

    // lock inc dword ptr [g_zeroDeltaNaNPrevented]
    // This changes EFLAGS, which are still saved by the leading pushfd.
    Emit8(cursor, 0xF0); Emit8(cursor, 0xFF); Emit8(cursor, 0x05);
    Emit32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_zeroDeltaNaNPrevented)));

    Emit8(cursor, 0x58); // pop eax
    Emit8(cursor, 0x9D); // popfd
    EmitRel32(cursor, 0xE9, reinterpret_cast<const void*>(returnAddress));

    unsigned char* normalPath = cursor;

    // Original semantics whenever frameDelta is nonzero.
    // fld [original esp+10h] -> [esp+18h] while eax/eflags are saved.
    Emit8(cursor, 0xD9); Emit8(cursor, 0x44); Emit8(cursor, 0x24); Emit8(cursor, 0x18);
    Emit8(cursor, 0xD8); Emit8(cursor, 0x35);
    Emit32(cursor, static_cast<std::uint32_t>(frameDeltaAddress));
    Emit8(cursor, 0xD9); Emit8(cursor, 0x9E); Emit32(cursor, 0x000004E4u);
    Emit8(cursor, 0x58); // pop eax
    Emit8(cursor, 0x9D); // popfd
    EmitRel32(cursor, 0xE9, reinterpret_cast<const void*>(returnAddress));

    const std::intptr_t deltaRel = normalPath - (jneDeltaDisp + 1);
    if (deltaRel < -128 || deltaRel > 127)
    {
        return false;
    }

    *jneDeltaDisp = static_cast<unsigned char>(static_cast<std::int8_t>(deltaRel));

    return static_cast<size_t>(cursor - stub) <= stubCapacity;
}
} // namespace

bool InstallVanillaZeroDeltaNaNFix()
{
    if (!InitializeMainExeInfo())
    {
        AppendLog(
            "[Stability] DP.exe info unavailable; zero-delta NaN guard disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog(
            "[Stability] Unsupported DP.exe build; zero-delta NaN guard disabled.\n");
        return false;
    }

    auto* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->runtime.speedDivideRva);
    const uintptr_t returnAddress =
        reinterpret_cast<uintptr_t>(target) + sizeof(kExpectedSpeedDivideBytes);
    const uintptr_t frameDeltaAddress =
        g_mainExeBase + build->runtime.frameDeltaRva;

    if (std::memcmp(
            target,
            kExpectedSpeedDivideBytes,
            sizeof(kExpectedSpeedDivideBytes)) != 0)
    {
        AppendLog(
            "[Stability] Speed-divide signature mismatch; zero-delta NaN guard disabled.\n");
        return false;
    }

    constexpr size_t kStubCapacity = 128;
    auto* stub = static_cast<unsigned char*>(VirtualAlloc(
        nullptr,
        kStubCapacity,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE));

    if (stub == nullptr)
    {
        AppendLog(
            "[Stability] VirtualAlloc failed; zero-delta NaN guard disabled.\n");
        return false;
    }

    if (!BuildZeroDeltaStub(
            stub,
            kStubCapacity,
            frameDeltaAddress,
            returnAddress))
    {
        VirtualFree(stub, 0, MEM_RELEASE);
        AppendLog(
            "[Stability] Failed to build zero-delta guard stub; patch disabled.\n");
        return false;
    }

    DWORD stubOldProtect = 0;
    if (!VirtualProtect(stub, kStubCapacity, PAGE_EXECUTE_READ, &stubOldProtect))
    {
        VirtualFree(stub, 0, MEM_RELEASE);
        AppendLog(
            "[Stability] Failed to protect zero-delta guard stub; patch disabled.\n");
        return false;
    }

    FlushInstructionCache(GetCurrentProcess(), stub, kStubCapacity);

    unsigned char patch[sizeof(kExpectedSpeedDivideBytes)] = {};
    std::memset(patch, 0x90, sizeof(patch));
    patch[0] = 0xE9;

    const uintptr_t afterJump = reinterpret_cast<uintptr_t>(target + 5);
    const uintptr_t stubAddress = reinterpret_cast<uintptr_t>(stub);
    const std::uint32_t rel = static_cast<std::uint32_t>(stubAddress - afterJump);
    std::memcpy(patch + 1, &rel, sizeof(rel));

    DWORD oldProtect = 0;
    if (!VirtualProtect(
            target,
            sizeof(patch),
            PAGE_EXECUTE_READWRITE,
            &oldProtect))
    {
        VirtualFree(stub, 0, MEM_RELEASE);
        AppendLog(
            "[Stability] VirtualProtect failed at speed divide; patch disabled.\n");
        return false;
    }

    std::memcpy(target, patch, sizeof(patch));
    FlushInstructionCache(GetCurrentProcess(), target, sizeof(patch));

    DWORD ignored = 0;
    VirtualProtect(target, sizeof(patch), oldProtect, &ignored);

    g_zeroDeltaNaNPatchRva = build->runtime.speedDivideRva;

    char installText[256] = {};
    sprintf_s(
        installText,
        "[Stability] Vanilla zero-delta speed fix installed at "
        "DP.exe+0x%08lX (frameDelta=0 uses one nominal 60 Hz tick).\n",
        static_cast<unsigned long>(build->runtime.speedDivideRva));
    AppendLog(installText);
    return true;
}

void PollVanillaZeroDeltaNaNFixLog()
{
    const LONG hits = InterlockedCompareExchange(
        &g_zeroDeltaNaNPrevented,
        0,
        0);

    if (hits == g_zeroDeltaNaNLastLogged)
    {
        g_zeroDeltaNaNLastObserved = hits;
        g_zeroDeltaNaNQuietPolls = 0;
        return;
    }

    if (hits != g_zeroDeltaNaNLastObserved)
    {
        g_zeroDeltaNaNLastObserved = hits;
        g_zeroDeltaNaNQuietPolls = 0;
    }
    else if (g_zeroDeltaNaNQuietPolls < kZeroDeltaNaNQuietFlushPolls)
    {
        ++g_zeroDeltaNaNQuietPolls;
    }

    const LONG pending = hits - g_zeroDeltaNaNLastLogged;
    const bool firstHit = g_zeroDeltaNaNLastLogged == 0;
    const bool batchReady = pending >= kZeroDeltaNaNLogBatch;
    const bool quietFlush =
        g_zeroDeltaNaNQuietPolls >= kZeroDeltaNaNQuietFlushPolls;

    if (!firstHit && !batchReady && !quietFlush)
        return;

    const LONG previous = g_zeroDeltaNaNLastLogged;
    g_zeroDeltaNaNLastLogged = hits;
    g_zeroDeltaNaNQuietPolls = 0;

    char text[256] = {};
    sprintf_s(
        text,
        "[Stability] Prevented vanilla zero-delta speed divide-by-zero "
        "(%ld new, %ld total, DP.exe+0x%08lX).\n",
        hits - previous,
        hits,
        static_cast<unsigned long>(g_zeroDeltaNaNPatchRva + 4));
    AppendLog(text);
}
