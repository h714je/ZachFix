#include "zachfix/input/gamepad_backend.h"

#include "zachfix/input/providers/gamepad_provider_sdl.h"
#include "zachfix/input/providers/gamepad_provider_xinput.h"
#include "zachfix/core/logging.h"

namespace
{
GamepadBackendType g_requestedBackend = GamepadBackendType::Auto;
GamepadBackendType g_activeBackend = GamepadBackendType::XInput;
bool g_backendAvailable = false;

bool TryInitialize(GamepadBackendType backend)
{
    switch (backend)
    {
    case GamepadBackendType::SDL:
        if (InitializeSdlGamepadProvider())
        {
            g_activeBackend = GamepadBackendType::SDL;
            g_backendAvailable = true;
            return true;
        }
        return false;

    case GamepadBackendType::XInput:
        if (InitializeXInputGamepadProvider())
        {
            g_activeBackend = GamepadBackendType::XInput;
            g_backendAvailable = true;
            return true;
        }
        return false;

    case GamepadBackendType::Auto:
        break;
    }
    return false;
}
} // namespace

const char* GamepadBackendName(GamepadBackendType backend)
{
    switch (backend)
    {
    case GamepadBackendType::SDL: return "SDL3";
    case GamepadBackendType::Auto: return "Auto";
    case GamepadBackendType::XInput:
    default:
        return "XInput";
    }
}

bool InitializeGamepadBackend(GamepadBackendType requestedBackend)
{
    if (g_backendAvailable)
        return true;

    g_requestedBackend = requestedBackend;

    if (requestedBackend == GamepadBackendType::Auto)
    {
        AppendLog(
            "[Input][GamepadBackend] Auto provider selection: trying SDL3 first.\n");
        if (TryInitialize(GamepadBackendType::SDL))
            return true;

        AppendLog(
            "[Input][GamepadBackend] SDL3 unavailable; falling back to XInput.\n");
        return TryInitialize(GamepadBackendType::XInput);
    }

    return TryInitialize(requestedBackend);
}

bool IsGamepadBackendAvailable()
{
    return g_backendAvailable;
}

GamepadBackendType GetActiveGamepadBackend()
{
    return g_activeBackend;
}

const char* GetActiveGamepadBackendName()
{
    return g_backendAvailable ? GamepadBackendName(g_activeBackend) : "None";
}

bool PollGamepadState(std::uint32_t index, GamepadState& state)
{
    state = {};
    if (!g_backendAvailable)
        return false;

    switch (g_activeBackend)
    {
    case GamepadBackendType::SDL:
    {
        const bool result = PollSdlGamepadState(index, state);
        if (!result && g_requestedBackend == GamepadBackendType::Auto &&
            SdlGamepadProviderInitializationFailed())
        {
            AppendLog(
                "[Input][GamepadBackend] SDL3 initialization failed in Auto mode; switching to XInput.\n");
            g_backendAvailable = false;
            if (!TryInitialize(GamepadBackendType::XInput))
                return false;
            return PollXInputGamepadState(index, state);
        }
        return result;
    }
    case GamepadBackendType::XInput:
        return PollXInputGamepadState(index, state);
    case GamepadBackendType::Auto:
    default:
        return false;
    }
}

bool GamepadBackendSupportsVibration()
{
    if (!g_backendAvailable)
        return false;

    switch (g_activeBackend)
    {
    case GamepadBackendType::SDL:
        return SdlGamepadProviderSupportsVibration();
    case GamepadBackendType::XInput:
        return XInputGamepadProviderSupportsVibration();
    case GamepadBackendType::Auto:
    default:
        return false;
    }
}

bool SetGamepadVibration(
    std::uint32_t index,
    std::uint16_t leftMotor,
    std::uint16_t rightMotor)
{
    if (!g_backendAvailable)
        return false;

    switch (g_activeBackend)
    {
    case GamepadBackendType::SDL:
    {
        const bool result = SetSdlGamepadVibration(index, leftMotor, rightMotor);
        if (!result && g_requestedBackend == GamepadBackendType::Auto &&
            SdlGamepadProviderInitializationFailed())
        {
            AppendLog(
                "[Input][GamepadBackend] SDL3 initialization failed during vibration in Auto mode; switching to XInput.\n");
            g_backendAvailable = false;
            if (!TryInitialize(GamepadBackendType::XInput))
                return false;
            return SetXInputGamepadVibration(index, leftMotor, rightMotor);
        }
        return result;
    }
    case GamepadBackendType::XInput:
        return SetXInputGamepadVibration(index, leftMotor, rightMotor);
    case GamepadBackendType::Auto:
    default:
        return false;
    }
}
