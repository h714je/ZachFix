#pragma once

#include "config.h"

// Installs ZachFix's native XInput backend for the supported Deadly
// Premonition Steam/GOG executables. DP keeps its vanilla controller action and
// binding logic; ZachFix supplies a synthetic JOYINFOEX view backed by XInput,
// remaps the legacy axis semantics, and restores DP's surviving native rumble.
bool InstallNativeXInputBackend();
bool IsNativeXInputBackendAvailable();

// Hot-applies the production gamepad behavior switches. Hooks remain
// installed for the session; these functions only change the runtime path
// selected on the next input update/consumer call.
void ApplyGamepadInputProfile(GamepadInputProfile profile);
void ApplyAnalogVehicleTriggers(bool enabled);
void ApplyVehicleTriggerDeadzone(UINT deadzone);

// Runtime installation state for startup diagnostics. "Available" means the
// three-consumer executable patch committed successfully.
bool IsAnalogVehicleTriggerPatchAvailable();

// Native rumble settings are fully live while the XInput backend is active.
// Strength is clamped to 0..1. Disabling vibration immediately stops motors;
// re-enabling or changing strength reapplies the current native actuator state.
bool ApplyNativeVibrationSettings(bool enabled, float strength);
bool IsNativeVibrationAvailable();
bool RunNativeVibrationTestPulse();

// Keeps native gameplay rumble aligned with DP's live USEJOY mode. Switching
// to keyboard/mouse stops the active XInput motors immediately; switching back
// to controller reapplies the still-current native actuator state.
void NotifyNativeVibrationInputModeChanged(bool controller);
