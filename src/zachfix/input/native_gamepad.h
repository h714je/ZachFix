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
// This updates per-user observation/stick caches only; gameplay ownership,
// shared triggers, and vibration handoff are reconciled after DP's update.
bool PollNativeGamepadState(std::uint32_t index, GamepadState& state);

// Marks gameplay ownership unresolved for the input update that is about to
// run. This boundary must execute before mode observation/AutoSwitch. The
// post-update record bridge is the only positive owner/rumble/trigger publish
// point; explicit zero cleanup remains allowed while reconciliation is pending.
void BeginNativeGamepadInputUpdate();

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
// Strength is clamped to 0..1. Enabled+strength publish coherently under the
// native lifecycle lock. A real enabled->disabled transition performs explicit
// captured-target cleanup even while owner reconciliation is pending; a
// strength-only edit does not truncate an independent diagnostic pulse.
bool ApplyNativeVibrationSettings(bool enabled, float strength);
bool IsNativeVibrationAvailable();
bool RunNativeVibrationTestPulse();
void PollNativeVibrationTestPulse();

// Synchronizes the native lifecycle with DP's live USEJOY mode. Mode observation
// is idempotent and independent of AutoSwitch: a real Controller->Keyboard
// transition performs explicit cleanup, while Controller mode alone never
// authorizes nonzero replay before the post-update selected-owner commit.
void NotifyNativeVibrationInputModeChanged(bool controller);
