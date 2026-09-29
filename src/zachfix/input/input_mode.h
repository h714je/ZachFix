#pragma once

// Runtime bridge around Deadly Premonition's central input update. When
// AutoSwitch is enabled it observes both input families before the update and
// flips the vanilla USEJOY byte. Native gamepad mode also uses the same hook
// to rebuild the active controller's 0x6C logical-action record afterward.
bool InstallInputUpdateBridge();

// Reads DP's current vanilla USEJOY byte. Returns false only when the
// supported executable/mode byte has not been resolved.
bool TryGetVanillaInputMode(bool& controller);
