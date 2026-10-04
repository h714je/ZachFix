#pragma once

// Installs a narrowly scoped vanilla-gameplay guard for supported DP.exe builds.
// The guarded instruction computes a planar movement rate as
// horizontalDisplacement / frameDelta. DP stores frameDelta in 60 Hz-normalized
// units (elapsedSeconds * 60), so 1.0f represents one nominal 60 Hz update.
// DP can produce an exact zero-delta update while the tracked object either
// remains still or has already moved. Vanilla then produces NaN from 0/0 or INF
// from finite/0; the latter can become NaN in downstream movement math and reach
// the game's deliberate invalid-float busy-loop sentinel.
//
// Production fix semantics:
//   frameDelta == +/-0 -> use one nominal 60 Hz tick (rate = displacement)
//   frameDelta != 0    -> execute original math unchanged
//
// The patch is executable-build and byte-signature gated. The build check uses
// in-memory PE SizeOfImage/TimeDateStamp rather than a whole-file hash, so
// unrelated LAA / No Intro executable edits remain compatible.
bool InstallVanillaZeroDeltaNaNFix();

// Cheap Present-time diagnostic. The injected game-code stub only increments an
// atomic counter, so it does not call C/C++ or disturb live SIMD/FPU state.
void PollVanillaZeroDeltaNaNFixLog();
