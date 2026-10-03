#pragma once

// Restart-only restoration of the original Xbox 360 surround-routing semantics
// that were partially lost in the Director's Cut PC port. The production path
// leaves stereo output untouched and only alters the confirmed surround update
// paths on supported Steam/GOG 1.01b executables.
void ConfigureSurroundAudioFix(bool requested);

bool IsSurroundAudioFixAvailable();
bool IsSurroundAudioFixActive();
