#pragma once

// Save I/O tracing plus transactional protection for the supported dp.sav.
// Save data itself is never rewritten: DP writes its normal bytes to a temp
// file, ZachFix validates them, backs up the previous live save, then commits.
bool InstallSaveDiagHooks();
