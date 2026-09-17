#pragma once

// Save I/O tracing, difficulty-profile routing, and transactional protection
// for dp.sav. SaveSafety itself never rewrites save fields: DP writes its
// normal bytes to a temp file, ZachFix validates them, backs up the previous
// profile save, then commits.
bool InstallSaveDiagHooks();
