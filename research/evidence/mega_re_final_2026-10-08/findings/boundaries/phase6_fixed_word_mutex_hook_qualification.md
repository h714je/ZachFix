# Fixed-word mutex hook qualification — fresh sequence 143

**STEAM_PC only.** The previously retained hook card was unverified/unpromoted at the sequence-142 quality stop. Fresh primary personally checked exact PE bytes and ASM lines/text for 22 instructions / 62 bytes before any new body acquisition. Direct import-directory resolution is also personally reproduced. Receipts: `scratch/phase6_entry_retained_personal_replay_seq0143.json`, `scratch/phase6_import_and_sort_personal_seq0143.json`; independent scout `scratch/phase6_seq0143_hook_qualification.json` / `.md`. No neighboring, worker, caller, OS implementation or GOG body.

## VERIFIED: exact available acquisition/release attempts

00712D80 compares fixed DWORD 014B0068 to zero. Its zero arm supplies three zero arguments to imported CreateMutexA and publishes raw EAX to that DWORD, without an own success check. Both arms then supply current [014B0068] and DWORD -1 to imported WaitForSingleObject. The wait result is not inspected locally and remains in EAX at return. Saved EBP is not a receiver; no ECX use appears.

00712DB0 independently reloads [014B0068], supplies it to imported ReleaseMutex and returns without an own result check; raw API EAX remains. It does not clear the cached word or close a handle. No ECX receiver use appears.

The DWORD is thus a cached raw mutex-handle carrier at these selected boundaries. This does not prove a valid/current handle, coherent initialization under concurrency, successful ownership acquisition, successful release, cleanup, OS-handle lifetime, or a complete mutex protocol.

## VERIFIED: syntactic connection to accepted local RMW

Accepted C0252 / 00712D50 directly calls 00712D80 before old-value capture and the reload/AND/store, then 00712DB0 before returning the captured value. Its selected 014940F8/-1 use is an identity mask transform. Fresh hook qualification replaces opaque-hook uncertainty with exact API-attempt geometry in that local call chain.

This is a **control-flow bracket**, not a proof that the protected operation executes under acquired mutex ownership. The local wrapper does not establish successful wait/release, correct serialization, memory-order guarantees, admission, scheduling, worker readiness or global coordination success. Hook bodies do not themselves use 014940F8.

## Bounded negative and residual

**STRONG_INFERENCE:** this seam is generic Win32 mutex-backed implementation plumbing rather than a newly discovered game manager/service. The architecture-changing coordination hypothesis does not survive at the selected scope. Deprioritize further support-callee descent.

**UNKNOWN:** a relationship to callback word 0148132C, task/event integration, ownership, current generation, last user or retirement. No literal/data join appears in the two hook bodies or accepted local wrapper. This is a selected-scope negative, not a universal absence claim. Callback/record publication remains independent.

All accepted C0249/C0252 facts and stronger limits remain; no PhysicsCore+67CC8 alias or C0121 disproof. Prior PLATFORM_WIN32 classifications are refined, not counted as newly classified UNKNOWNs. All 88 carries and Phase 5 selected-static acceptance remain unchanged; Phase 7 has not begun.
