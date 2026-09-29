#pragma once

// Installs the original Xbox combat-strafe ingress bridge at the preserved
// Player-update tail. The bridge is build/signature gated and remains a no-op
// until enabled at runtime.
bool PrepareCombatStrafeRestoration();
void ApplyCombatStrafeRestoration(bool enabled);
bool IsCombatStrafeRestorationAvailable();
bool IsCombatStrafeRestorationActive();
