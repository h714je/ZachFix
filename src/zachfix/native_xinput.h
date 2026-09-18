#pragma once

#include "config.h"

// Installs ZachFix's native XInput backend for the supported Deadly
// Premonition Steam/GOG executables. DP keeps its vanilla controller action and
// binding logic; ZachFix supplies a synthetic JOYINFOEX view backed by XInput,
// remaps the legacy axis semantics, and restores DP's surviving native rumble.
bool InstallNativeXInputBackend();

// Hot-applies the production gamepad behavior switches. Hooks remain
// installed for the session; these functions only change the runtime path
// selected on the next input update/consumer call.
void ApplyGamepadInputProfile(GamepadInputProfile profile);
void ApplyAnalogVehicleTriggers(bool enabled);
void ApplyVehicleTriggerDeadzone(UINT deadzone);

// Native rumble settings are fully live while the XInput backend is active.
// Strength is clamped to 0..1. Disabling vibration immediately stops motors;
// re-enabling or changing strength reapplies the current native actuator state.
bool ApplyNativeVibrationSettings(bool enabled, float strength);
bool IsNativeVibrationAvailable();
bool RunNativeVibrationTestPulse();
