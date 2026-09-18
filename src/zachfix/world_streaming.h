#pragma once

bool PrepareWorldCellDetailClassifyHook();
bool ApplyWorldDetailDistanceScale(unsigned int scale);
bool ApplyWorldObjectActivationDistanceScale(unsigned int scale);

// Production fix for the confirmed Director's Cut interior visibility-volume
// regression. This patches only the outer-world callsite and is fully reversible.
bool ApplyWorldInteriorOcclusionFix(bool enabled);

// Research-only global frustum bypass. The hook is installed lazily on first
// enable, defaults OFF, is runtime-only, and is never persisted to ZachFix.ini.
bool PrepareWorldFrustumCullResearchHook();
bool IsWorldFrustumCullResearchHookReady();
bool GetWorldFrustumCullDisabledResearch();
void SetWorldFrustumCullDisabledResearch(bool disabled);
unsigned long long GetWorldFrustumCullBypassedRejects();
