#pragma once

#include "zachfix/core/config.h"

#include <cstdint>

struct GamepadState;

// Installs ZachFix's native gamepad integration for the supported Deadly
// Premonition Steam/GOG executables. DP keeps its vanilla controller action,
// configJ binding, 0x6C logical-record, staging and commit logic; the selected
// provider (XInput or SDL3) supplies canonical GamepadState directly to the
// native controller action helpers without a synthetic JOYINFOEX.
bool InstallNativeGamepadBackend();
bool IsNativeGamepadBackendAvailable();

// Polls one provider slot through the native ZachFix integration layer.
// This preserves active-controller ownership, stick caches, vehicle trigger
// state, and vibration handoff.
bool PollNativeGamepadState(std::uint32_t index, GamepadState& state);

// Rebuilds the currently selected controller's native 0x6C logical-action
// record from canonical GamepadState after DP has run its normal input update.
// The original update still owns keyboard/mouse, focus suppression, configJ and
// action-helper semantics; this function replaces only controller acquisition.
void ApplyNativeGamepadInputRecord(void* inputState, void* actionState);

// Hot-applies the production gamepad behavior switches. Hooks remain
// installed for the session; these functions only change the runtime path
// selected on the next input update/consumer call.
void ApplyGamepadInputProfile(GamepadInputProfile profile);
void ApplyAnalogVehicleTriggers(bool enabled);
void ApplyVehicleTriggerDeadzone(UINT deadzone);

// Runtime installation state for startup diagnostics. "Available" means the
// three-consumer executable patch committed successfully.
bool IsAnalogVehicleTriggerPatchAvailable();


// Raw original-Xbox combat-strafe shoulder semantics. This path is available
// only while native gamepad input is active, the Xbox360 profile is selected, and
// vanilla USEJOY is in controller mode.
enum class XboxCombatStrafeInput
{
    None,
    Left,
    Right
};

XboxCombatStrafeInput PollXboxCombatStrafeInput();
void ResetXboxCombatStrafeInput();

// Native rumble settings are fully live while the native gamepad backend is active.
// Strength is clamped to 0..1. Disabling vibration immediately stops motors;
// re-enabling or changing strength reapplies the current native actuator state.
bool ApplyNativeVibrationSettings(bool enabled, float strength);
bool IsNativeVibrationAvailable();
bool RunNativeVibrationTestPulse();
void PollNativeVibrationTestPulse();

// Keeps native gameplay rumble aligned with DP's live USEJOY mode. Switching
// to keyboard/mouse stops the active provider motors immediately; switching back
// to controller reapplies the still-current native actuator state.
void NotifyNativeVibrationInputModeChanged(bool controller);
