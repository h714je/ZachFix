#pragma once

// Research-only PhysX Real-Time A/B v2.1. Hooks the mapped PC scene-queue and
// timing path. When enabled, only scene 0 ordinary updates use QPC wall time;
// zero/reset/special semantics remain native and fixed-step maxIter is raised to 4.
bool InstallPhysXRealTimeAB();
