# In-place tracker: separate application raw-pointer deallocation boundary

**STEAM_PC, Phase6 sequence139.** Main personally checked exactly0040CE60's118-byte body and caller window[00401422,0040142F),13 bytes: scratch/phase6_seq0138_tracker_main.json. No application expansion, helper/free implementation, slot body, GOG or runtime source. New source is selected by sequence138, not a reopening of the shell teardown.

## VERIFIED: selected acquired-root relay

00401422 calls accepted00403690;00401427 copies its returned EAX into ECX;00401429 calls0040CE60. Accepted getter returns fixed00BD9E20 via00BDA0C0. This is an explicit application shutdown-function-to-tracker consumer join, not an address-adjacency assumption. The narrow window does not establish complete shutdown chronology, actual execution or earlier subsystem completion.

## VERIFIED: own tracked-entry deallocation requests

0040CE60 saves incoming receiver R. It captures a signed count N=([R+10]-[R+C]) arithmetic-shifted-right2. If N>0, i starts0; each iteration reloads current begin/end for an unsigned i-versus-current-count check, potentially calls opaque0074F89B, then loads P_i=[current_begin+4i] and passes P_i to metadata-named_free0074F25D at0040CE99. It increments i and repeats under signed i<N using the initially captured N. This is local pointer-entry traversal with raw deallocation-boundary calls. It does not dereference P_i or invoke a pointee vtable/deleting destructor.

After the loop/rejoin, it captures current end in EBP, performs further begin/end checks with the same opaque helper, loads begin and reference-cell field0, and supplies those values plus a stack-output address to opaque0040D580 at0040CECF with ECX=R. Own code returns normally. Do not rename this last call “clear,” “erase all,” or “backing release” without its source; there is no own field-clear instruction in this body.

The current-bound rechecks do not prove safe concurrent mutation: the helper effect is opaque, the loop's bound is captured separately, and no locking/generation contract is established. The supplied_free name is boundary metadata, not proof of successful deallocation or allocation matching.

## STRONG_INFERENCE: architectural role and preserved prior result

Together with the accepted shared malloc/zero-fill/append seam, fixed tracker root and this explicit shutdown consumer, the structure is compatible with a raw-allocation tracking/retirement registry. This is a materially new lifecycle boundary: allocation tracking and later application-requested per-entry raw deallocation have separate paths from CRT-exit backing/reference-cell teardown.

C0243's exact statement remains true: selected0040CE20 contains no own pointee loop. The new result disproves only an extrapolation from that local absence to “the tracker has no separate element retirement route”; no accepted broader absence claim is silently rewritten. It does not establish a class destructor, universal managed-object owner or full service subsystem.

## UNKNOWN / exact stop edges

- Actual application invocation and full shutdown order, worker/callback completion, last use and current generation.
- Exclusive ownership, live element validity, duplicates, earlier frees/removal and successful allocation-matched deallocation.
- Opaque0040D580 range state after the boundary;0074F89B effect.
- Full pointer insertion/removal policy, named class identity and GOG correspondence.

Do not reopen the shell target, full application shutdown body or old free census. A genuinely useful later source would establish duplicate/removal/allocator pairing or the exact range mutation; its architectural value must outrank independent cold residuals. These are static/unvisited versus runtime-dependent gaps, not blanket exhaustion. All88 inherited guards remain unchanged. Phase5 remains CLOSED/ACCEPTED; noPhase7.

## Validation correction

First sequence139ledger validation failed: BND-236 consumerCRT_STL_RUNTIME was outside the controlled subsystem endpoint set. Corrected to internalAPPLICATION_LIFECYCLE boundary; the named_free terminal remains in the dataflow, not a newly invented subsystem. Original failed receipt reports/PHASE6_VALIDATION_2026-10-05_SEQ0139.txt preserved. This is a taxonomy correction, not changed executable evidence.
