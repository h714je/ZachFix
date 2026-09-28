#pragma once

// Validates the two native main-tick CInput CALL sites and prepares a
// reversible hot-apply switch. Preparation itself never changes code order.
bool PrepareLowLatencyInputOrdering();

// Hot-applies poll->commit when enabled and restores the exact vanilla
// commit->poll pair when disabled. Returns false on any verification failure.
bool ApplyLowLatencyInputOrdering(bool enabled);

bool IsLowLatencyInputOrderingAvailable();
bool IsLowLatencyInputOrderingActive();
