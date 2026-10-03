# Research Heuristics

These heuristics are derived from the completed static-analysis batches. They guide prioritization; they are not semantic conclusions about unexamined functions.

## Evidence and identity

1. Keep `(build, address, original auto-name, canonical name, homology group)` separate in every promotion.
2. Require assembly or raw-byte confirmation when a decompiler boundary affects control flow, especially `noreturn` markings and split functions.
3. Treat old reports as search hints. Promote only after current executable evidence agrees.
4. Record disproven interpretations as negative constraints rather than deleting them.
5. Do not infer object identity from nearby constructors/destructors; track the actual `this` register or stack-local address.

## Resource and object reconstruction

6. Start at the verified callback seam (`00408310` Steam / `004082D0` GOG), then follow each extension branch to allocation, vtable write, parser, and first consumer.
7. For a resource branch, collect the tuple: extension/name test, helper calls, allocation size, vtable write, parser magic/header, callback/ownership handoff, and consumer subsystem. Do not promote a class from the extension string alone.
8. Keep archive records, manager records, child records, and utility objects as separate candidate types even when sizes or fields look similar.
9. For manager reconstruction, search constructor arguments, fixed-stride allocation, callback storage, mutex creation, registration writers, cleanup, and global/root references as one lifecycle cluster.
10. For child-record semantics, begin with allocation and field initialization, then inspect every helper that receives the child base; defer names such as frame, tile, mip, or layer until a discriminating consumer is found.

## Cross-build and boundaries

11. Compare normalized call neighborhoods, constants, field offsets, and vtable relations before accepting Steam/GOG homology.
12. Prioritize edges crossing subsystem boundaries over isolated leaf helpers: resource-to-object, object-to-PhysX, animation-to-actor, world-to-resource, and UI-to-game state.
13. When a decompiler body is truncated, use raw continuation and callers/callees to restore the boundary before interpreting semantics.
14. Use centrality to choose targets, but do not name a high-fanout function from fan-out alone; large initializers and compiler-generated dispatchers can look alike.

## Scalable census strategy

15. Classify obvious CRT/STL, import thunks, Win32, D3D9, PhysX, audio, and middleware code coarsely and move on once evidence is adequate.
16. Promote game-owned roots only when there is a concrete boundary, lifecycle, table, vtable, or repeated subsystem signal.
17. Use global read/write clusters to connect functions, then verify the root initializer and ownership before assigning semantic labels.
18. Maintain a concise but complete per-batch checkpoint using the current template/receipt: source universe, exact addresses, files changed, confidence limits, counters, journal/validation continuity, and one successor action.
19. If a question is revisited three times without new discriminating evidence, move it to BLOCKED with the missing evidence stated precisely.
20. Prefer a narrow branch slice across both PC builds over a broad one-build decompilation pass; paired evidence exposes false homology and decompiler artifacts early.

## Current application after 2026-10-02 reconciliation

The former immediate XAM/XCA/XFE/XWP/XNV/DSB branch slice is historical. XAM, XCA, XNV, DSB, and XWP now have explicit `BOUNDED_STATIC` reopen conditions in `queues/UNKNOWN_QUEUE.md`; do not repeat extension, wrapper, or generic consumer scans without those discriminators. XFE remains unresolved but is not a reason to reopen the other bounded branches.

The reconciliation's immediate CMessage target is now superseded: C0108 bounds construction/singleton-local table provenance, and the curator preserves unresolved receiver aliasing. Reopen only under the CMESSAGE_TABLE_PROVENANCE row's actual discriminator, not this historical priority.

Current scheduling is exclusively in RESEARCH_LOOP_STATE.md and its named checkpoint. The curator retained CSHOP_FACTORY_EXTERNAL_OWNERSHIP after a five-candidate comparison, with corrected selector-factory result and named deletion-entry sources. Do not infer factories from constructor addresses, and do not replace the selected action with a bounded CMessage/global/offset census.
