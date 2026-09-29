#pragma once

bool PrepareWorldCellDetailClassifyHook();
bool ApplyWorldDetailDistanceScale(unsigned int scale);
bool IsWorldDetailExtensionAvailable();
unsigned int GetWorldDetailDistanceScale();
bool ApplyWorldMainFrustumDistanceMode(unsigned int mode);
bool ApplyWorldObjectActivationDistanceScale(unsigned int scale);
bool ApplyWorldObjectLodDistanceScale(unsigned int scale);

// Production fix for the confirmed Director's Cut interior visibility-volume
// regression. The callsite is redirected once at startup; runtime toggles then
// update only an atomic data flag while the disabled path tail-calls native code.
bool PrepareWorldInteriorOcclusionFixBridge();
bool ApplyWorldInteriorOcclusionFix(bool enabled);
bool IsWorldInteriorOcclusionFixAvailable();
bool IsWorldInteriorOcclusionFixActive();

// Research-only global frustum bypass. The hook is installed lazily on first
// enable, defaults OFF, is runtime-only, and is never persisted to ZachFix.ini.
bool PrepareWorldFrustumCullResearchHook();
bool IsWorldFrustumCullResearchHookReady();
bool GetWorldFrustumCullDisabledResearch();
void SetWorldFrustumCullDisabledResearch(bool disabled);
unsigned long long GetWorldFrustumCullBypassedRejects();
