#pragma once

// Prepares the build/signature-gated mode-2 aim hook without enabling it.
// The hook can then be toggled live through ApplyAimFpuPrecisionFix().
bool PrepareAimFpuPrecisionFix();

// Experimental, reversible runtime switch. When enabled, x87 single-precision
// (PC24) is forced only while DP's native camera mode-2 aim handler executes.
// The caller's previous precision-control bits are restored immediately after.
bool ApplyAimFpuPrecisionFix(bool enabled);

bool IsAimFpuPrecisionFixAvailable();
bool IsAimFpuPrecisionFixActive();
