#pragma once

// Installs ZachFix's native XInput backend for the supported Deadly
// Premonition Steam/GOG executables. DP keeps its vanilla controller action and
// binding logic; ZachFix supplies a synthetic JOYINFOEX view backed by XInput,
// remaps the legacy axis semantics, and restores DP's surviving native rumble.
bool InstallNativeXInputBackend();

// Native rumble settings are fully live while the XInput backend is active.
// Strength is clamped to 0..1. Disabling vibration immediately stops motors;
// re-enabling or changing strength reapplies the current native actuator state.
bool ApplyNativeVibrationSettings(bool enabled, float strength);
bool IsNativeVibrationAvailable();
