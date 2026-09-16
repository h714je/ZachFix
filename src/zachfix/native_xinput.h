#pragma once

// Installs ZachFix's native XInput backend for the supported Deadly
// Premonition Steam executable. DP keeps its vanilla controller action and
// binding logic; ZachFix only supplies a synthetic JOYINFOEX view backed by
// XInput and remaps the legacy axis semantics at DP's common evaluator.
bool InstallNativeXInputBackend();
