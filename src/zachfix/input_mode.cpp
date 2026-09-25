#include "input_mode.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"
#include "native_xinput.h"

#include <Windows.h>
#include <mmsystem.h>
#include <MinHook.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
// USEJOY and the central CInput update are resolved from the detected build.
//
// USEJOY comes from configJ/UconfigJ. The central input update and many
// per-action helpers read this same byte to choose keyboard/mouse or controller
// evaluation. Hooking the update lets us choose the mode before DP consumes the
// input for the current frame, rather than one frame later in Present().
constexpr unsigned char kInputUpdateSignature[] = {
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x34, 0x89, 0x4D, 0xD4
};

using JoyGetPosExFn = MMRESULT (WINAPI*)(UINT, LPJOYINFOEX);
using InputUpdateFn = void (__thiscall*)(void* self);

std::atomic_bool g_installed{ false };
unsigned char* g_useJoyMode = nullptr;
JoyGetPosExFn g_joyGetPosEx = nullptr;
InputUpdateFn g_originalInputUpdate = nullptr;

bool g_haveKeyboardBaseline = false;
std::array<bool, 256> g_keyDown{};

bool g_haveCursorBaseline = false;
POINT g_lastCursor{};

struct JoySnapshot
{
    DWORD x = 32767;
    DWORD y = 32767;
    DWORD z = 32767;
    DWORD r = 32767;
    DWORD u = 32767;
    DWORD v = 32767;
    DWORD buttons = 0;
    DWORD pov = JOY_POVCENTERED;
};

std::array<JoySnapshot, 4> g_lastJoy{};
std::array<bool, 4> g_haveJoyBaseline{};

bool g_haveObservedMode = false;
bool g_lastObservedMode = false;

constexpr LONG kMouseActivityPixels = 2;
constexpr DWORD kAxisCenter = 32767;
constexpr DWORD kAxisActivityDistance = 8000;
constexpr DWORD kAxisNoiseDelta = 1500;

bool ResolveUseJoyModePointer()
{
    if (g_useJoyMode != nullptr)
        return true;

    if (!InitializeMainExeInfo())
        return false;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
        return false;

    g_useJoyMode = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->input.useJoyModeRva);
    return true;
}

unsigned int AbsDiff(DWORD a, DWORD b)
{
    return a >= b ? a - b : b - a;
}

bool AxisMovedMeaningfully(DWORD current, DWORD previous)
{
    return AbsDiff(current, kAxisCenter) >= kAxisActivityDistance &&
           AbsDiff(current, previous) >= kAxisNoiseDelta;
}

bool GamepadBecameActive(const JoySnapshot& current, const JoySnapshot& previous)
{
    const DWORD newlyPressed = current.buttons & ~previous.buttons;
    if (newlyPressed != 0)
        return true;

    if (current.pov != JOY_POVCENTERED && current.pov != previous.pov)
        return true;

    return AxisMovedMeaningfully(current.x, previous.x) ||
           AxisMovedMeaningfully(current.y, previous.y) ||
           AxisMovedMeaningfully(current.z, previous.z) ||
           AxisMovedMeaningfully(current.r, previous.r) ||
           AxisMovedMeaningfully(current.u, previous.u) ||
           AxisMovedMeaningfully(current.v, previous.v);
}

void SetVanillaInputMode(bool controller, const char* reason)
{
    if (!g_useJoyMode)
        return;

    const bool current = *g_useJoyMode != 0;
    if (current == controller)
    {
        g_lastObservedMode = controller;
        g_haveObservedMode = true;
        return;
    }

    *g_useJoyMode = controller ? 1u : 0u;
    g_lastObservedMode = controller;
    g_haveObservedMode = true;
    NotifyNativeVibrationInputModeChanged(controller);

    char text[192] = {};
    sprintf_s(
        text,
        "[Input][Mode] Auto switch: %s -> %s (%s).\n",
        current ? "Controller" : "KeyboardMouse",
        controller ? "Controller" : "KeyboardMouse",
        reason ? reason : "activity");
    AppendLog(text);
}

bool PollKeyboardActivity()
{
    BYTE keyState[256] = {};
    if (!GetKeyboardState(keyState))
        return false;

    bool active = false;

    // Transitions, not held state: a key already being held must not steal the
    // mode back every frame after the player starts using a controller.
    for (size_t vk = 1; vk < g_keyDown.size(); ++vk)
    {
        const bool down = (keyState[vk] & 0x80u) != 0;

        if (g_haveKeyboardBaseline && down && !g_keyDown[vk])
            active = true;

        g_keyDown[vk] = down;
    }

    g_haveKeyboardBaseline = true;
    return active;
}

bool PollMouseActivity()
{
    POINT current = {};
    if (!GetCursorPos(&current))
        return false;

    if (!g_haveCursorBaseline)
    {
        g_lastCursor = current;
        g_haveCursorBaseline = true;
        return false;
    }

    const LONG dx = current.x - g_lastCursor.x;
    const LONG dy = current.y - g_lastCursor.y;
    g_lastCursor = current;

    return dx >= kMouseActivityPixels || dx <= -kMouseActivityPixels ||
           dy >= kMouseActivityPixels || dy <= -kMouseActivityPixels;
}

bool PollGamepadActivity()
{
    if (!g_joyGetPosEx)
        return false;

    bool active = false;

    for (UINT joyId = 0; joyId < static_cast<UINT>(g_lastJoy.size()); ++joyId)
    {
        JOYINFOEX info = {};
        info.dwSize = sizeof(info);
        info.dwFlags = JOY_RETURNALL;

        if (g_joyGetPosEx(joyId, &info) != JOYERR_NOERROR)
            continue;

        const JoySnapshot current = {
            info.dwXpos,
            info.dwYpos,
            info.dwZpos,
            info.dwRpos,
            info.dwUpos,
            info.dwVpos,
            info.dwButtons,
            info.dwPOV
        };

        if (!g_haveJoyBaseline[joyId])
        {
            g_lastJoy[joyId] = current;
            g_haveJoyBaseline[joyId] = true;
            continue;
        }

        if (GamepadBecameActive(current, g_lastJoy[joyId]))
            active = true;

        // Keep every connected pad baseline fresh even if one already became
        // active in this input update.
        g_lastJoy[joyId] = current;
    }

    return active;
}

void ObserveExternalModeChange()
{
    if (!g_useJoyMode)
        return;

    const bool mode = *g_useJoyMode != 0;
    if (!g_haveObservedMode)
    {
        g_lastObservedMode = mode;
        g_haveObservedMode = true;

        char text[160] = {};
        sprintf_s(
            text,
            "[Input][Mode] Initial vanilla USEJOY mode observed: %s.\n",
            mode ? "Controller" : "KeyboardMouse");
        AppendLog(text);
        return;
    }

    if (mode == g_lastObservedMode)
        return;

    g_lastObservedMode = mode;
    NotifyNativeVibrationInputModeChanged(mode);

    char text[176] = {};
    sprintf_s(
        text,
        "[Input][Mode] Vanilla USEJOY changed externally to %s.\n",
        mode ? "Controller" : "KeyboardMouse");
    AppendLog(text);
}

void PollAndSelectInputMode()
{
    if (!g_useJoyMode)
        return;

    ObserveExternalModeChange();

    // Poll every source on every DP input update to keep baselines current, but
    // only the inactive side may take ownership. This prevents stick drift,
    // held keys, and DP's own mouse recenter from fighting the active device.
    const bool gamepadActivity = PollGamepadActivity();
    const bool keyboardActivity = PollKeyboardActivity();
    const bool mouseActivity = PollMouseActivity();

    const bool currentController = *g_useJoyMode != 0;
    if (currentController)
    {
        if (keyboardActivity || mouseActivity)
        {
            SetVanillaInputMode(
                false,
                mouseActivity ? "mouse activity" : "keyboard activity");
        }
    }
    else if (gamepadActivity)
    {
        SetVanillaInputMode(true, "gamepad activity");
    }
}

void __fastcall HookInputUpdate(void* self, void*)
{
    if (g_installed.load(std::memory_order_acquire))
        PollAndSelectInputMode();

    g_originalInputUpdate(self);
}

} // namespace

bool InstallInputModeAutoSwitch()
{
    if (!g_config.autoInputModeSwitch)
        return true;

    if (!ResolveUseJoyModePointer())
    {
        AppendLog(
            "[Input][Mode] ERROR: Unsupported/unavailable DP.exe USEJOY byte; "
            "auto switch disabled.\n");
        return false;
    }

    HMODULE winmm = GetModuleHandleW(L"winmm.dll");
    if (!winmm)
        winmm = LoadLibraryW(L"winmm.dll");

    if (winmm)
    {
        g_joyGetPosEx = reinterpret_cast<JoyGetPosExFn>(
            GetProcAddress(winmm, "joyGetPosEx"));
    }

    if (!g_joyGetPosEx)
    {
        AppendLog(
            "[Input][Mode] WARNING: joyGetPosEx unavailable; "
            "gamepad cannot activate auto switch.\n");
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog(
            "[Input][Mode] ERROR: Unsupported DP.exe build; auto switch disabled.\n");
        return false;
    }

    auto* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->input.inputUpdateRva);
    if (std::memcmp(target, kInputUpdateSignature, sizeof(kInputUpdateSignature)) != 0)
    {
        char errorText[192] = {};
        sprintf_s(
            errorText,
            "[Input][Mode] ERROR: Input-update signature mismatch at "
            "DP.exe+0x%08lX; auto switch disabled.\n",
            static_cast<unsigned long>(build->input.inputUpdateRva));
        AppendLog(errorText);
        return false;
    }

    MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookInputUpdate),
        reinterpret_cast<void**>(&g_originalInputUpdate));

    if (status != MH_OK)
    {
        AppendLog("[Input][Mode] ERROR: input-update MH_CreateHook failed.\n");
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        MH_RemoveHook(target);
        g_originalInputUpdate = nullptr;
        AppendLog("[Input][Mode] ERROR: input-update MH_EnableHook failed.\n");
        return false;
    }

    g_installed.store(true, std::memory_order_release);

    char readyText[224] = {};
    sprintf_s(
        readyText,
        "[Input][Mode] Auto switch installed before DP input update "
        "(USEJOY=DP.exe+0x%08lX, input=DP.exe+0x%08lX).\n",
        static_cast<unsigned long>(build->input.useJoyModeRva),
        static_cast<unsigned long>(build->input.inputUpdateRva));
    AppendLog(readyText);
    return true;
}


bool TryGetVanillaInputMode(bool& controller)
{
    if (!ResolveUseJoyModePointer())
        return false;

    controller = *g_useJoyMode != 0;
    return true;
}
