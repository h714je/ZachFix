#pragma once

// Installs a narrowly scoped vanilla-gameplay guard for the Steam DP.exe build.
// The guarded instruction computes actor speed as distance / frameDelta.
// DP can intentionally produce a zero-delta frame during timing-loop startup;
// if the actor also did not move, the original code performs 0/0 and stores NaN.
//
// Production fix semantics:
//   distance == +/-0 AND frameDelta == +/-0 -> store speed 0.0f
//   every other input                           -> execute original math
//
// The patch is executable-build and byte-signature gated. The build check uses
// in-memory PE SizeOfImage/TimeDateStamp rather than a whole-file hash, so
// unrelated LAA / No Intro executable edits remain compatible.
bool InstallVanillaZeroDeltaNaNFix();

// Cheap Present-time diagnostic. The injected game-code stub only increments an
// atomic counter, so it does not call C/C++ or disturb live SIMD/FPU state.
void PollVanillaZeroDeltaNaNFixLog();
