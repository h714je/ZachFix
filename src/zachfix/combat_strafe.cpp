#include "combat_strafe.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"
#include "native_xinput.h"

#include <Windows.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
static_assert(sizeof(void*) == 4, "Combat strafe bridge requires the 32-bit ZachFix build.");

constexpr std::uint32_t kCombatCapability = 0x2000u;
constexpr std::uint32_t kCombatFlag = 0x10u;
constexpr std::ptrdiff_t kCombatFlagOffset = 0x638;
constexpr int kStateStrafeLeft = 0x09;
constexpr int kStateStrafeRight = 0x0A;

// Common Director's Cut instruction at the Xbox-equivalent strafe ingress
// point. The trampoline runs the recovered Xbox gate first, then executes this
// original CMP and returns immediately after it.
constexpr unsigned char kExpectedHookBytes[] = {
    0x83, 0xBE, 0x54, 0x06, 0x00, 0x00, 0x00
};

using PlayerCapabilityFn = int (__thiscall*)(void* player, unsigned int capability);
using PlayerStateTransitionFn = void (__thiscall*)(void* player, int state);

std::atomic_bool g_available{false};
std::atomic_bool g_enabled{false};
unsigned char* g_stub = nullptr;
PlayerCapabilityFn g_playerCapability = nullptr;
PlayerStateTransitionFn g_playerStateTransition = nullptr;

bool IsRangeInsideMainExe(uintptr_t address, size_t size)
{
    if (address == 0 || size == 0 || g_mainExeBase == 0 || g_mainExeSize == 0)
        return false;

    const uintptr_t end = address + size;
    const uintptr_t imageEnd = g_mainExeBase + g_mainExeSize;
    return end >= address && address >= g_mainExeBase && end <= imageEnd;
}

void __cdecl RunCombatStrafeIngress(void* player)
{
    if (!player || !g_enabled.load(std::memory_order_acquire) ||
        !g_playerCapability || !g_playerStateTransition)
    {
        return;
    }

    // Poll first even outside combat so a shoulder edge that happened in an
    // ineligible state cannot be replayed later when the combat gate opens.
    const XboxCombatStrafeInput input = PollXboxCombatStrafeInput();
    if (input == XboxCombatStrafeInput::None)
        return;

    const auto* bytes = static_cast<const unsigned char*>(player);
    const std::uint32_t flags = *reinterpret_cast<const std::uint32_t*>(
        bytes + kCombatFlagOffset);
    if ((flags & kCombatFlag) == 0)
        return;

    if (g_playerCapability(player, kCombatCapability) == 0)
        return;

    switch (input)
    {
    case XboxCombatStrafeInput::Left:
        g_playerStateTransition(player, kStateStrafeLeft);
        break;
    case XboxCombatStrafeInput::Right:
        g_playerStateTransition(player, kStateStrafeRight);
        break;
    default:
        break;
    }
}

void Emit8(unsigned char*& cursor, unsigned char value)
{
    *cursor++ = value;
}

void Emit32(unsigned char*& cursor, std::uint32_t value)
{
    std::memcpy(cursor, &value, sizeof(value));
    cursor += sizeof(value);
}

bool BuildStub(uintptr_t resumeAddress)
{
    constexpr size_t kStubCapacity = 64;
    g_stub = static_cast<unsigned char*>(VirtualAlloc(
        nullptr,
        kStubCapacity,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE));
    if (!g_stub)
    {
        AppendLog("[Player][CombatStrafe] ERROR: VirtualAlloc failed; restoration unavailable.\n");
        return false;
    }

    unsigned char* cursor = g_stub;

    // Preserve the Player-update machine state around the C++ helper. The
    // original CMP below then recreates the flags expected by the untouched
    // Director's Cut continuation.
    Emit8(cursor, 0x9C); // pushfd
    Emit8(cursor, 0x60); // pushad
    Emit8(cursor, 0x56); // push esi (Player*)
    Emit8(cursor, 0xB8); // mov eax, imm32
    Emit32(cursor, static_cast<std::uint32_t>(
        reinterpret_cast<uintptr_t>(&RunCombatStrafeIngress)));
    Emit8(cursor, 0xFF); Emit8(cursor, 0xD0); // call eax
    Emit8(cursor, 0x83); Emit8(cursor, 0xC4); Emit8(cursor, 0x04); // add esp,4
    Emit8(cursor, 0x61); // popad
    Emit8(cursor, 0x9D); // popfd

    for (unsigned char byte : kExpectedHookBytes)
        Emit8(cursor, byte);

    // Absolute return avoids rel32 range assumptions between the EXE and DLL.
    Emit8(cursor, 0x68); // push imm32
    Emit32(cursor, static_cast<std::uint32_t>(resumeAddress));
    Emit8(cursor, 0xC3); // ret

    DWORD oldProtect = 0;
    if (!VirtualProtect(g_stub, kStubCapacity, PAGE_EXECUTE_READ, &oldProtect))
    {
        VirtualFree(g_stub, 0, MEM_RELEASE);
        g_stub = nullptr;
        AppendLog("[Player][CombatStrafe] ERROR: Could not make trampoline executable.\n");
        return false;
    }

    FlushInstructionCache(GetCurrentProcess(), g_stub, kStubCapacity);
    return true;
}

bool PatchHookSite(unsigned char* target)
{
    unsigned char patch[sizeof(kExpectedHookBytes)] = {};
    patch[0] = 0x68; // push imm32
    const std::uint32_t stubAddress = static_cast<std::uint32_t>(
        reinterpret_cast<uintptr_t>(g_stub));
    std::memcpy(patch + 1, &stubAddress, sizeof(stubAddress));
    patch[5] = 0xC3; // ret
    patch[6] = 0x90; // nop

    DWORD oldProtect = 0;
    if (!VirtualProtect(target, sizeof(patch), PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    std::memcpy(target, patch, sizeof(patch));
    FlushInstructionCache(GetCurrentProcess(), target, sizeof(patch));

    DWORD ignored = 0;
    if (!VirtualProtect(target, sizeof(patch), oldProtect, &ignored))
    {
        AppendLog(
            "[Player][CombatStrafe] WARNING: Could not restore hook-site protection.\n");
    }

    return true;
}
} // namespace

bool PrepareCombatStrafeRestoration()
{
    if (g_available.load(std::memory_order_acquire))
        return true;

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Player][CombatStrafe] ERROR: DP.exe info unavailable.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (!build)
    {
        AppendLog("[Player][CombatStrafe] Unsupported DP.exe build; restoration unavailable.\n");
        return false;
    }

    if (!IsNativeXInputBackendAvailable())
    {
        AppendLog("[Player][CombatStrafe] Native XInput is inactive; restoration bridge not installed.\n");
        return false;
    }

    const uintptr_t hookAddress = g_mainExeBase + build->player.combatStrafeHookRva;
    const uintptr_t capabilityAddress =
        g_mainExeBase + build->player.combatCapabilityRva;
    const uintptr_t transitionAddress =
        g_mainExeBase + build->player.stateTransitionRva;

    if (!IsRangeInsideMainExe(hookAddress, sizeof(kExpectedHookBytes)) ||
        !IsRangeInsideMainExe(capabilityAddress, 1) ||
        !IsRangeInsideMainExe(transitionAddress, 1))
    {
        AppendLog("[Player][CombatStrafe] ERROR: Build-profile address outside DP.exe.\n");
        return false;
    }

    auto* target = reinterpret_cast<unsigned char*>(hookAddress);
    if (std::memcmp(
            target,
            kExpectedHookBytes,
            sizeof(kExpectedHookBytes)) != 0)
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[Player][CombatStrafe] ERROR: Player-tail signature mismatch at DP.exe+0x%08lX; restoration unavailable.\n",
            static_cast<unsigned long>(build->player.combatStrafeHookRva));
        AppendLog(text);
        return false;
    }

    g_playerCapability =
        reinterpret_cast<PlayerCapabilityFn>(capabilityAddress);
    g_playerStateTransition =
        reinterpret_cast<PlayerStateTransitionFn>(transitionAddress);

    const uintptr_t resumeAddress = hookAddress + sizeof(kExpectedHookBytes);
    if (!BuildStub(resumeAddress))
        return false;

    if (!PatchHookSite(target))
    {
        VirtualFree(g_stub, 0, MEM_RELEASE);
        g_stub = nullptr;
        g_playerCapability = nullptr;
        g_playerStateTransition = nullptr;
        AppendLog("[Player][CombatStrafe] ERROR: Could not install Player-tail bridge.\n");
        return false;
    }

    g_available.store(true, std::memory_order_release);

    char text[320] = {};
    sprintf_s(
        text,
        "[Player][CombatStrafe] Xbox ingress bridge installed for %s at DP.exe+0x%08lX; gate=capability 0x2000 + Player+0x638 bit 0x10, input=physical LB/RB edges.\n",
        build->name,
        static_cast<unsigned long>(build->player.combatStrafeHookRva));
    AppendLog(text);
    return true;
}

void ApplyCombatStrafeRestoration(bool enabled)
{
    g_config.restoreCombatStrafe = enabled;
    g_enabled.store(enabled, std::memory_order_release);
    ResetXboxCombatStrafeInput();

    char text[192] = {};
    sprintf_s(
        text,
        "[Player][CombatStrafe] Original Xbox combat strafe %s.\n",
        enabled ? "enabled" : "disabled");
    AppendLog(text);
}

bool IsCombatStrafeRestorationAvailable()
{
    return g_available.load(std::memory_order_acquire);
}

bool IsCombatStrafeRestorationActive()
{
    return IsCombatStrafeRestorationAvailable() &&
           g_enabled.load(std::memory_order_acquire);
}
