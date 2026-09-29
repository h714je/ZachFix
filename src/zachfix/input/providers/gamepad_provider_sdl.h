#pragma once

#include "zachfix/input/gamepad_backend.h"

bool InitializeSdlGamepadProvider();
bool SdlGamepadProviderInitializationFailed();
bool PollSdlGamepadState(std::uint32_t index, GamepadState& state);
bool SdlGamepadProviderSupportsVibration();
bool SetSdlGamepadVibration(
    std::uint32_t index,
    std::uint16_t leftMotor,
    std::uint16_t rightMotor);
