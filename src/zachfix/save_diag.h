#pragma once

// Save I/O tracing and transactional protection for vanilla savedata\dp.sav.
// SaveSafety never rewrites save fields: DP writes its normal bytes to a temp
// file, ZachFix validates them, backs up the previous live save, then commits.
bool InstallSaveDiagHooks();
