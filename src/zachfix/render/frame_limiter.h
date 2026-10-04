#pragma once

#include <Windows.h>

// 0 disables pacing. Nonzero values are applied immediately and paced against
// QueryPerformanceCounter deadlines at the outermost game Present boundary.
// Returns the effective limit. Invalid nonzero values fail closed to Off.
UINT SetFrameRateLimit(UINT framesPerSecond);
UINT GetFrameRateLimit();
void PaceFrameRateLimit();
