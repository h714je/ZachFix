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

## Stable heuristic ID cross-reference

The numbered guidance above remains unchanged. These headings link all current research-method and anti-heuristic IDs to the ledger; they are scheduling and verification guidance, never claim evidence. Added 2026-10-04 during sequence-56 preflight to repair the map/ledger cross-reference check.

### RH01 - Keep build-local identity explicit

Carry build,address,auto-name,canonical-name,homology-group separately.

### RH02 - Raw control flow outranks broken decompiler boundaries

Compare raw bytes/branches/returns against decompiler body before semantic naming.

### RH03 - Historical reports are locators, not fresh proof

Use old material to locate addresses, then recheck primary evidence for promotion.

### RH04 - Preserve disproven interpretations

Record negative constraint and superseding evidence instead of deleting history.

### RH05 - Track the real receiver

Track ECX/this or exact stack-local address across each load-bearing edge.

### RH06 - Callback/table targets survive export omissions

Seed raw target recovery from pointer literals even when functions.csv has no row.

### RH07 - Opaque calls are continuity barriers

Split symbols at opaque calls and prove identity/value continuity before rejoining.

### RH08 - Reacquisition creates a new logical generation

Name repeated results R1,R2... until identity continuity is established.

### RH09 - Maintain explicit stack-coordinate bases

Record entry-ESP/current-ESP transforms and recompute argument/local offsets.

### RH10 - Reconstruct raw ABI before assigning semantics

Classify EAX/AL/AX/out-param/address/ignored return shapes.

### RH11 - Preserve width and signedness literally

Track truncation, sign/zero extension, ordered/unordered comparisons and threshold inclusivity.

### RH12 - Restore split/truncated functions before semantics

Recover continuation with raw branches, callers/callees and neighboring blocks.

### RH13 - Keep numeric domains separate until equality edge

Assign symbolic domain names before friendly semantics.

### RH14 - Normal return does not prove semantic success

Locate the first validated status/result consumer before using success language.

### RH15 - Classify reference kind before ownership reasoning

Use resolver/release/sentinel/arithmetic/dereference behavior to classify.

### RH16 - Same offset across unrelated classes is not shared semantics

Type receiver first, then compare offset use within the family.

### RH17 - Same numeric state across subsystems is not shared meaning

Use typed receiver+field+writer+value+consumer as semantic key.

### RH18 - Bound a gate to the exact controlled region

Mark branch start/end and list operations skipped vs still executed.

### RH19 - Equal stride or size is not type identity

Compare initialization, fields and consumers before merging equal-size records.

### RH20 - Separate finalizer registration, cleanup availability, invocation and free

Record each lifecycle milestone independently.

### RH21 - Retention is not ownership

Track creator,retainer,releaser,freer separately.

### RH22 - Clear/reset/unlink is not free

Search allocator/deallocator and pointee destructor separately from reset.

### RH23 - Retirement marker/event is not deletion

Separate marker/event2, manager unlink, destructor and free.

### RH24 - Staging or cache publication is not freshness

Track producer generation, invalidation, bypass paths and consumer generation.

### RH25 - Static call order is not runtime concurrency proof

Separate instruction-order facts from OS scheduling/cadence/quiescence claims.

### RH26 - Repeated registration is not automatically idempotent

Inspect duplicate registration behavior and old-handle release before calling ensure.

### RH27 - Start typed-resource work at a verified seam

Follow exact typed branch from callback/factory to first discriminating consumer.

### RH28 - Collect resource identity tuple before naming type

Collect extension,helpers,allocation,vtable,parser magic,handoff,consumer.

### RH29 - Keep archive,manager,child,cache,utility records distinct

Track allocation source, owner, stride, lifecycle and consumer separately.

### RH30 - Reconstruct managers as lifecycle clusters

Inspect ctor args,table,callback,locks,population,root,update,cleanup.

### RH31 - Reconstruct child records through discriminating consumers

Start at initialization and enumerate all helpers/consumers of child base.

### RH32 - Homology requires structural anchors

Compare role,constants,fields,vtable,calls,strings,CFG.

### RH33 - Regional address deltas are candidate priors only

Rank regional delta candidates then structurally verify.

### RH34 - Prefer paired narrow slices when cross-build confidence matters

Use matched bounded slices where homology/artifact detection is the immediate goal.

### RH35 - Coarsely classify obvious external/runtime code and move on

Assign coarse ownership once adequate evidence exists.

### RH36 - Promote game-owned roots only from concrete anchors

Require initializer/static receiver/lifecycle/vtable/table or repeated typed cluster.

### RH37 - Use global clusters as joins not labels

Aggregate readers/writers then verify initializer,receiver and consumers.

### RH38 - Prefer cross-subsystem edges over isolated leaf helpers

Rank producer->mechanism->consumer boundaries across subsystem lines.

### RH39 - Centrality ranks targets but does not name them

Use centrality for scheduling only.

### RH40 - Sourcecards and handoffs are locators not promotion artifacts

Re-derive mechanism from primary evidence in ordinary batch.

### RH41 - Reuse accepted claims only at exact scope

Quote/reuse only the precise typed/root/boundary fact already accepted.

### RH42 - Distinguish unvisited,not-found,bounded-static,disproven

Record exact search grammar and reopen discriminator.

### RH43 - Metrics are not semantic closure

Keep semantic exit criteria independent from raw count/byte/function percentages.

### RH44 - Reassess globally after coherent mechanism gain

Compare independent families after each coherent gain before following warm locators.

### RH45 - Three non-discriminating revisits trigger BLOCKED

After repeated equivalent searches, state exact missing evidence and block.

### RH46 - Keep checkpoints concise but complete

Record universe,targets,changes,scope,limits,counters,validation,next action.

