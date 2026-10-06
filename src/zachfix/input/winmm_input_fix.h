#pragma once

#include <Windows.h>
#include <mmsystem.h>

// Prepares the shared legacy joystick path used by both DP.exe and ZachFix's
// AutoSwitch fallback. When enabled, DP.exe's joyGetPosEx IAT entry is wrapped
// so repeated JOYERR_PARMS results are cached per slot until WM_DEVICECHANGE.
bool InstallLegacyJoystickPollingFix(bool enabled);

// Uses the same per-slot suppression state for ZachFix-owned legacy polling.
// When the fix is disabled this is a transparent call to winmm!joyGetPosEx.
MMRESULT PollLegacyJoystickSafely(UINT joyId, LPJOYINFOEX info);

// Attach event-driven invalidation to DP's real top-level game window. This is
// independent of the optional ImGui UI and should be called after CreateDevice.
bool AttachLegacyJoystickDeviceNotifications(HWND window);

bool IsLegacyJoystickPollingFunctionAvailable();
bool IsLegacyJoystickPollingFixAvailable();
bool IsLegacyJoystickPollingFixActive();
