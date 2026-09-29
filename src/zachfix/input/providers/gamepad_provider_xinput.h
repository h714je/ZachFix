#pragma once

#include "zachfix/input/gamepad_backend.h"

bool InitializeXInputGamepadProvider();
bool PollXInputGamepadState(std::uint32_t index, GamepadState& state);
bool XInputGamepadProviderSupportsVibration();
bool SetXInputGamepadVibration(
    std::uint32_t index,
    std::uint16_t leftMotor,
    std::uint16_t rightMotor);
