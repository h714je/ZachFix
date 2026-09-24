#pragma once

// Extends the near/full residency footprint for objects that use DP's pinned
// alternate low-detail 3D representation. 1 keeps native behavior; 2/3/4
// request the full model progressively farther away through DP's own residency
// setter and high-detail streaming path.
bool ApplyWorldAlternate3DDistanceScale(unsigned int scale);
unsigned int GetWorldAlternate3DDistanceScale();
bool IsWorldAlternate3DExtensionAvailable();
