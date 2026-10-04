#pragma once

#include <Windows.h>

// 0 disables pacing. Nonzero values are applied immediately and paced against
// QueryPerformanceCounter deadlines at the outermost game Present boundary.
void SetFrameRateLimit(UINT framesPerSecond);
UINT GetFrameRateLimit();
void PaceFrameRateLimit();
