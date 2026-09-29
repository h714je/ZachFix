#include "zachfix/input/input_mode.h"

#include "zachfix/core/config.h"
#include "zachfix/input/gamepad_backend.h"
#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"
#include "zachfix/input/native_gamepad.h"

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
using InputUpdateFn = void (__thiscall*)(void* self, void* actionState);

std::atomic_bool g_installed{ false };
unsigned char* g_useJoyMode = nullptr;
JoyGetPosExFn g_joyGetPosEx = nullptr;
InputUpdateFn g_originalInputUpdate = nullptr;
bool g_useNativeGamepadActivity = false;
bool g_autoSwitchEnabled = false;

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

std::array<GamepadState, kMaxGamepads> g_lastGamepad{};
std::array<bool, kMaxGamepads> g_haveGamepadBaseline{};

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

bool LegacyAxisMovedMeaningfully(DWORD current, DWORD previous)
{
    return AbsDiff(current, kAxisCenter) >= kAxisActivityDistance &&
           AbsDiff(current, previous) >= kAxisNoiseDelta;
}

bool LegacyGamepadBecameActive(const JoySnapshot& current, const JoySnapshot& previous)
{
    const DWORD newlyPressed = current.buttons & ~previous.buttons;
    if (newlyPressed != 0)
        return true;

    if (current.pov != JOY_POVCENTERED && current.pov != previous.pov)
        return true;

    return LegacyAxisMovedMeaningfully(current.x, previous.x) ||
           LegacyAxisMovedMeaningfully(current.y, previous.y) ||
           LegacyAxisMovedMeaningfully(current.z, previous.z) ||
           LegacyAxisMovedMeaningfully(current.r, previous.r) ||
           LegacyAxisMovedMeaningfully(current.u, previous.u) ||
           LegacyAxisMovedMeaningfully(current.v, previous.v);
}

unsigned int AbsDiffSigned(std::int32_t a, std::int32_t b)
{
    const std::int32_t delta = a - b;
    return static_cast<unsigned int>(delta >= 0 ? delta : -delta);
}

bool NativeStickAxisMovedMeaningfully(
    std::int16_t current,
    std::int16_t previous,
    bool legacyPositiveCenterBias)
{
    // Preserve the old synthetic-WinMM AutoSwitch thresholds exactly while
    // reading the canonical provider state directly. X/Z used 32768+raw with
    // a 32767 activity center, hence the historical +1 bias. Y/R did not.
    const std::int32_t centered = static_cast<std::int32_t>(current) +
        (legacyPositiveCenterBias ? 1 : 0);
    const unsigned int distance = static_cast<unsigned int>(
        centered >= 0 ? centered : -centered);
    return distance >= kAxisActivityDistance &&
           AbsDiffSigned(current, previous) >= kAxisNoiseDelta;
}

DWORD NativeTriggerActivityMagnitude(std::uint8_t value)
{
    // Preserve the historical native-gamepad AutoSwitch thresholds. The
    // Xbox360 profile treats raw > 30 as a digital trigger press; PC keeps
    // the old continuous 32767..65535 compatibility magnitude.
    if (g_config.gamepadInputProfile == GamepadInputProfile::Xbox360)
        return value > 30u ? 32768u : 0u;

    return (static_cast<DWORD>(value) * 32768u) / 255u;
}

bool NativeTriggerMovedMeaningfully(std::uint8_t current, std::uint8_t previous)
{
    const DWORD currentMagnitude = NativeTriggerActivityMagnitude(current);
    const DWORD previousMagnitude = NativeTriggerActivityMagnitude(previous);
    return currentMagnitude >= kAxisActivityDistance &&
           AbsDiff(currentMagnitude, previousMagnitude) >= kAxisNoiseDelta;
}

int NativeGamepadPov(std::uint32_t buttons)
{
    const bool up = (buttons & GamepadButton_DpadUp) != 0;
    const bool down = (buttons & GamepadButton_DpadDown) != 0;
    const bool left = (buttons & GamepadButton_DpadLeft) != 0;
    const bool right = (buttons & GamepadButton_DpadRight) != 0;

    if (up && !down)
    {
        if (right && !left) return 4500;
        if (left && !right) return 31500;
        return 0;
    }
    if (down && !up)
    {
        if (right && !left) return 13500;
        if (left && !right) return 22500;
        return 18000;
    }
    if (right && !left)
        return 9000;
    if (left && !right)
        return 27000;
    return -1;
}

bool NativeGamepadBecameActive(
    const GamepadState& current,
    const GamepadState& previous)
{
    constexpr std::uint32_t kLegacyButtonMask = (1u << 10) - 1u;

    // DP's temporary WinMM view exposes canonical bits 0..9 as buttons
    // and the D-pad separately as POV. Keep those contracts exact.
    const std::uint32_t currentButtons = current.buttons & kLegacyButtonMask;
    const std::uint32_t previousButtons = previous.buttons & kLegacyButtonMask;
    if ((currentButtons & ~previousButtons) != 0)
        return true;

    const int currentPov = NativeGamepadPov(current.buttons);
    const int previousPov = NativeGamepadPov(previous.buttons);
    if (currentPov >= 0 && currentPov != previousPov)
        return true;

    return NativeStickAxisMovedMeaningfully(
               current.leftX, previous.leftX, true) ||
           NativeStickAxisMovedMeaningfully(
               current.leftY, previous.leftY, false) ||
           NativeStickAxisMovedMeaningfully(
               current.rightX, previous.rightX, true) ||
           NativeStickAxisMovedMeaningfully(
               current.rightY, previous.rightY, false) ||
           NativeTriggerMovedMeaningfully(
               current.leftTrigger, previous.leftTrigger) ||
           NativeTriggerMovedMeaningfully(
               current.rightTrigger, previous.rightTrigger);
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

bool PollNativeGamepadActivity()
{
    bool active = false;

    for (std::uint32_t user = 0; user < kMaxGamepads; ++user)
    {
        GamepadState current = {};
        if (!PollNativeGamepadState(user, current))
            continue;

        if (!g_haveGamepadBaseline[user])
        {
            g_lastGamepad[user] = current;
            g_haveGamepadBaseline[user] = true;
            continue;
        }

        if (NativeGamepadBecameActive(current, g_lastGamepad[user]))
            active = true;

        g_lastGamepad[user] = current;
    }

    return active;
}

bool PollLegacyGamepadActivity()
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

        if (LegacyGamepadBecameActive(current, g_lastJoy[joyId]))
            active = true;

        // Keep every connected pad baseline fresh even if one already became
        // active in this input update.
        g_lastJoy[joyId] = current;
    }

    return active;
}

bool PollGamepadActivity()
{
    return g_useNativeGamepadActivity
        ? PollNativeGamepadActivity()
        : PollLegacyGamepadActivity();
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

void __fastcall HookInputUpdate(void* self, void*, void* actionState)
{
    if (g_installed.load(std::memory_order_acquire) && g_autoSwitchEnabled)
        PollAndSelectInputMode();

    g_originalInputUpdate(self, actionState);

    // Preserve DP's native update for keyboard/mouse, focus handling and record
    // initialization, then replace only the active controller's
    // logical-action fields from canonical GamepadState. This is a no-op when
    // the native backend is unavailable or USEJOY is in keyboard/mouse mode.
    ApplyNativeGamepadInputRecord(self, actionState);
}

} // namespace

bool InstallInputUpdateBridge()
{
    g_autoSwitchEnabled = g_config.autoInputModeSwitch;
    const bool nativeRecordBridgeNeeded =
        g_config.nativeGamepadEnabled && IsNativeGamepadBackendAvailable();

    // The central input-update hook now owns two independent jobs:
    //   1) optional AutoSwitch before DP evaluates the frame;
    //   2) native GamepadState -> DP 0x6C controller-record rebuild after it.
    // Keep the hook even when AutoSwitch is disabled if native input needs it.
    if (!g_autoSwitchEnabled && !nativeRecordBridgeNeeded)
        return true;

    if (!ResolveUseJoyModePointer())
    {
        AppendLog(
            "[Input][Mode] ERROR: Unsupported/unavailable DP.exe USEJOY byte; "
            "input-update bridge disabled.\n");
        return false;
    }

    g_useNativeGamepadActivity = nativeRecordBridgeNeeded;

    if (g_autoSwitchEnabled)
    {
        if (g_useNativeGamepadActivity)
        {
            AppendLog(
                "[Input][Mode] Auto switch gamepad activity source: "
                "native GamepadState provider.\n");
        }
        else
        {
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
                    "legacy gamepad cannot activate auto switch.\n");
            }
            else
            {
                AppendLog(
                    "[Input][Mode] Auto switch gamepad activity source: "
                    "legacy WinMM fallback.\n");
            }
        }
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog(
            "[Input][Mode] ERROR: Unsupported DP.exe build; input-update bridge disabled.\n");
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
            "DP.exe+0x%08lX; input-update bridge disabled.\n",
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
        const MH_STATUS removeStatus = MH_RemoveHook(target);
        if (removeStatus == MH_OK || removeStatus == MH_ERROR_NOT_CREATED)
        {
            g_originalInputUpdate = nullptr;
        }
        else
        {
            AppendLog(
                "[Input][Mode] ERROR: input-update rollback could not remove the hook; trampoline retained for safety.\n");
        }
        AppendLog("[Input][Mode] ERROR: input-update MH_EnableHook failed.\n");
        return false;
    }

    g_installed.store(true, std::memory_order_release);

    char readyText[288] = {};
    sprintf_s(
        readyText,
        "[Input][Mode] Input-update bridge installed "
        "(AutoSwitch=%s NativeGamepad=%s, USEJOY=DP.exe+0x%08lX, input=DP.exe+0x%08lX).\n",
        g_autoSwitchEnabled ? "true" : "false",
        nativeRecordBridgeNeeded ? "true" : "false",
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
