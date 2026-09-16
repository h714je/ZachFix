#pragma once

// Runtime bridge around Deadly Premonition's vanilla USEJOY switch.
// ZachFix observes both input families immediately before DP's central input
// update and flips the game's own mode byte to the most recently active side.
bool InstallInputModeAutoSwitch();

// Reads DP's current vanilla USEJOY byte. Returns false only when the
// supported executable/mode byte has not been resolved.
bool TryGetVanillaInputMode(bool& controller);
