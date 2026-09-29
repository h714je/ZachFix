#include "zachfix/input/providers/gamepad_provider_xinput.h"

#include "zachfix/core/logging.h"

#include <Windows.h>
#include <Xinput.h>

#include <cstdio>

namespace
{
using XInputGetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
using XInputSetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_VIBRATION*);

HMODULE g_xinputModule = nullptr;
XInputGetStateFn g_xinputGetState = nullptr;
XInputSetStateFn g_xinputSetState = nullptr;

std::uint32_t MapXInputButtons(WORD buttons)
{
    std::uint32_t mapped = 0;
    auto setButton = [&](WORD xinputMask, GamepadButton button)
    {
        if ((buttons & xinputMask) != 0)
            mapped |= static_cast<std::uint32_t>(button);
    };

    setButton(XINPUT_GAMEPAD_A, GamepadButton_A);
    setButton(XINPUT_GAMEPAD_B, GamepadButton_B);
    setButton(XINPUT_GAMEPAD_X, GamepadButton_X);
    setButton(XINPUT_GAMEPAD_Y, GamepadButton_Y);
    setButton(XINPUT_GAMEPAD_LEFT_SHOULDER, GamepadButton_LeftShoulder);
    setButton(XINPUT_GAMEPAD_RIGHT_SHOULDER, GamepadButton_RightShoulder);
    setButton(XINPUT_GAMEPAD_BACK, GamepadButton_Back);
    setButton(XINPUT_GAMEPAD_START, GamepadButton_Start);
    setButton(XINPUT_GAMEPAD_LEFT_THUMB, GamepadButton_LeftThumb);
    setButton(XINPUT_GAMEPAD_RIGHT_THUMB, GamepadButton_RightThumb);
    setButton(XINPUT_GAMEPAD_DPAD_UP, GamepadButton_DpadUp);
    setButton(XINPUT_GAMEPAD_DPAD_DOWN, GamepadButton_DpadDown);
    setButton(XINPUT_GAMEPAD_DPAD_LEFT, GamepadButton_DpadLeft);
    setButton(XINPUT_GAMEPAD_DPAD_RIGHT, GamepadButton_DpadRight);
    return mapped;
}
} // namespace

bool InitializeXInputGamepadProvider()
{
    if (g_xinputGetState != nullptr)
        return true;

    static const wchar_t* kCandidates[] = {
        L"xinput1_4.dll",
        L"xinput1_3.dll",
        L"xinput9_1_0.dll"
    };

    for (const wchar_t* candidate : kCandidates)
    {
        HMODULE module = LoadLibraryW(candidate);
        if (module == nullptr)
            continue;

        auto* getState = reinterpret_cast<XInputGetStateFn>(
            GetProcAddress(module, "XInputGetState"));
        if (getState == nullptr)
        {
            FreeLibrary(module);
            continue;
        }

        g_xinputModule = module;
        g_xinputGetState = getState;
        g_xinputSetState = reinterpret_cast<XInputSetStateFn>(
            GetProcAddress(module, "XInputSetState"));

        char modulePath[MAX_PATH] = {};
        if (GetModuleFileNameA(
                module,
                modulePath,
                static_cast<DWORD>(sizeof(modulePath))) != 0)
        {
            char text[MAX_PATH + 112] = {};
            sprintf_s(
                text,
                "[Input][GamepadBackend] XInput provider: %s\n",
                modulePath);
            AppendLog(text);
        }
        else
        {
            AppendLog("[Input][GamepadBackend] XInput provider loaded.\n");
        }

        if (g_xinputSetState == nullptr)
        {
            AppendLog(
                "[Input][Vibration] WARNING: XInput provider has no "
                "XInputSetState; native rumble restoration will stay disabled.\n");
        }
        return true;
    }

    AppendLog(
        "[Input][GamepadBackend] ERROR: no usable XInput provider found.\n");
    return false;
}

bool PollXInputGamepadState(std::uint32_t index, GamepadState& state)
{
    state = {};

    if (g_xinputGetState == nullptr || index >= kMaxGamepads)
        return false;

    XINPUT_STATE xinput = {};
    if (g_xinputGetState(index, &xinput) != ERROR_SUCCESS)
        return false;

    const XINPUT_GAMEPAD& pad = xinput.Gamepad;
    state.leftX = pad.sThumbLX;
    state.leftY = pad.sThumbLY;
    state.rightX = pad.sThumbRX;
    state.rightY = pad.sThumbRY;
    state.leftTrigger = pad.bLeftTrigger;
    state.rightTrigger = pad.bRightTrigger;
    state.buttons = MapXInputButtons(pad.wButtons);
    state.stateSequence = xinput.dwPacketNumber;
    return true;
}

bool XInputGamepadProviderSupportsVibration()
{
    return g_xinputSetState != nullptr;
}

bool SetXInputGamepadVibration(
    std::uint32_t index,
    std::uint16_t leftMotor,
    std::uint16_t rightMotor)
{
    if (g_xinputSetState == nullptr || index >= kMaxGamepads)
        return false;

    XINPUT_VIBRATION vibration = {};
    vibration.wLeftMotorSpeed = leftMotor;
    vibration.wRightMotorSpeed = rightMotor;
    return g_xinputSetState(index, &vibration) == ERROR_SUCCESS;
}
