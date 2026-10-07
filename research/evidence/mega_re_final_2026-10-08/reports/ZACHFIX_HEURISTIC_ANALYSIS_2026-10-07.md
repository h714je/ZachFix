# Mega RE heuristic analysis

Date: 2026-10-07

Scope: heuristic pass over the current Mega RE snapshot after Phase 8 synthesis. This is not a replacement for canonical claims, ledgers, overlays, or validation receipts. New statements below are explicitly separated into:

- **VERIFIED MECHANIC**: already supported by the snapshot's primary/static evidence.
- **COMPOSITE HEURISTIC**: new deduction obtained by joining two or more independently supported mechanics.
- **RUNTIME REQUIRED**: plausible runtime consequence that is not established by the static snapshot.

The pass reviewed the existing 65 heuristic-ledger rules, 554 Markdown findings/reports/maps, selected Phase 7 falsification portfolios, Phase 6 residual architecture, and the final Phase 8 flow maps. A structural text-mining pass was also used to find repeated container, index, sentinel, publication, generation, and lifecycle shapes. Text frequency itself is not treated as executable evidence.

---

## Executive result

The heuristic pass found several useful patterns that are not yet expressed as first-class rules in `HEURISTIC_LEDGER.csv`. Two composite results stand out:

1. **CThrowLure is a much stronger latent lifecycle defect candidate than the individual reports make obvious.** The verified double registration simultaneously creates a stale handle-table entry, a self-linked intrusive-list node, a second current handle, and two manager-count increments. The reviewed retirement path releases only the second handle and decrements the manager count once. The reviewed reset/retirement iterators obtain `next` from the self-link before dispatch/deletion. Separately, the handle allocator has only `0xA00` pointer slots and has no normal full-table exhaustion exit. The runtime consequence is still unobserved, but the static defect chain is unusually coherent and testable.

2. **The reflection W,W bias discrepancy has a distinctive aspect-ratio fingerprint.** The verified caller supplies width for both dimensions to a helper that supports W,H. Relative to the same helper called correctly, the Y translation error is `0.5/W - 0.5/H`. In vertical texels this becomes `0.5*(H/W - 1)`, so at fixed aspect ratio the error is approximately resolution-independent and grows with wider aspect ratios. This gives a clean way to distinguish this hypothesis from many ordinary pixel-resolution-dependent reflection defects.

A third broad result is architectural rather than a single bug: the engine repeatedly separates **publication/admission/completion/lifetime** into different events. A return value, cleared pending flag, reusable row, reset success, or logical removal very often is not a physical-completion or last-use fence.

---

# 1. Proposed new research heuristics

These are proposed additions after the existing RH01-RH46. They are not automatically canonicalized.

## RH47 - Local synchronization is participant-scoped

**Rule:** A mutex, atomic helper, or separately locked field proves exclusion only among participants that actually use the same synchronization protocol. It does not establish a coherent transaction over adjacent state or over a whole table.

**Evidence:**

- Timer fixed-word wrapper can overwrite an intervening atomic increment because it captures/reloads/stores under a different protocol.
- Phase 7 A04 tag traversal has no whole-scan lock; individual counter helpers are separately locked.
- Actuator paired setters are separately locked and can reacquire different root generations.

**Use:** When a code path contains one lock, enumerate all writers before promoting state to globally serialized.

## RH48 - Lazy singleton publication requires publication-time revalidation

**Rule:** `if (!root) { construct; root = p; }` is not evidence of thread-safe one-generation publication unless the publication point rechecks the root, uses CAS/once semantics, or is externally serialized.

**Evidence:**

- Timer lazy mutex creation permits two cold-zero observations and publication of different handles.
- Actuator/root service lazy construction can publish a saved result after leaving/re-entering guard scope without proving the root is still zero.

**Use:** Treat lazy getters as generation producers until publication-time synchronization is proved.

## RH49 - Upstream admission is part of an internal helper's contract

**Rule:** A deep helper's valid-input domain may be enforced entirely by its callers. Lack of local bounds/null/readiness checks is not automatically a native bug, but direct mod calls must reproduce the upstream admission invariant.

**Evidence:**

- Player actor handler table has holes and no local null/bounds gate at the selected dispatch seam.
- NativeUI resolver dereferences its directory before sentinel handling and lacks local table/readiness/code bounds.
- Audio Main resolver indexes by token low byte before any `<128` gate.
- Physics controller creation/release uses a plain reusable index and has weak local generation/null protection.
- Actuator setters perform index-derived stores without a local device-domain bound.

**Use:** For ZachFix hooks, recover the producer/admission path before calling game-private leaves directly.

## RH50 - Diagnostic calls are not control-flow barriers unless raw control flow proves it

**Rule:** Invalid-parameter/error/diagnostic helpers must not be modeled as `noreturn` solely from name, decompiler inference, CRT convention, or intended error semantics.

**Evidence:**

- The foundational GOG false-`noreturn` split hid legitimate fallthrough/callable structure.
- Tracker retirement can continue after a returning diagnostic and perform an indexed load.
- Resource worker empty/error paths have returning diagnostic continuations.

**Use:** High-value structural scan: enumerate calls to diagnostic/assert/invalid-parameter leaves currently treated as terminating, then inspect raw post-call bytes, reachability, and PE-backed fallthrough.

## RH51 - Logical completion is not a physical/backend completion fence

**Rule:** A local success flag, sentinel, pending clear, reusable-row marker, or normal return is only a protocol state unless backend success and last-use are separately joined.

**Evidence across independent systems:**

- Audio Main row can become reusable before backend cleanup is proved complete.
- Resource request pending can clear after no-work/rejection and without request-correlated success.
- D3D reset gate can return eligible while individual `Create*` operations failed.
- Save coordinator protocol result does not certify a full physical transfer.
- Tracker erase/removal does not prove backing/pointee retirement.
- Model packet/resource clears do not certify GPU/borrower last use.

**Use:** Never use a logical flag alone as a safe hook/unload/free boundary.

## RH52 - “Current” is subsystem-local, not a global frame epoch

**Rule:** A value described as current/valid can belong to a different update, generation, or subsystem snapshot. Same-frame coherence must be explicitly proved.

**Evidence:**

- Input pending state is committed on a later update.
- Animation/model packet can be reused while no current pose/resource epoch is checked.
- Shadow/pre-render can read retained banks before a later builder in the same broad frame flow.
- Reflection textures are later bound without proving their contents are from the latest current generation.
- Save staging and execution reacquire roots across steps.

**Use:** Investigate one-frame-delayed or intermittent visual/input bugs as epoch-skew candidates before assuming arithmetic corruption.

## RH53 - Numeric identifier equality is not lifetime identity without an epoch

**Rule:** A token/index may be unique only within a bounded allocation generation. Equality after reuse/wrap/root reconstruction does not prove the same object or backend episode.

**Evidence:**

- CSdMain token sequence wraps and a new Main starts sequence 1; equal full tokens can recur.
- Physics controller uses a plain first-free 0..127 index, allowing ABA-style slot reuse.
- CRdHandleUtil is generation-tagged, but duplicate registration shows that even a generation scheme can be defeated by one object holding multiple table mappings while only the latest handle is retained in-object.

**Use:** Preserve provider/root generation beside cached indices in new integrations.

## RH54 - Rejection/failure may be destructive or partially publishing

**Rule:** Do not infer rollback from a false/negative return. The function may have already cleared the old state, published new pointers, modified list membership, or advanced bookkeeping.

**Evidence:**

- XCA cleanup of previous state can occur before validation of the new payload.
- Resource records can be premarked before typed callback acceptance.
- Active-object registration mutates list/count before handle allocation completion.
- D3D reset pre-clears cells before recreation, and recreation failures are not aggregated into the gate result.
- Save optional setup can occur before admission failure.

**Use:** Hot reload/retry logic must explicitly snapshot or rebuild prior state if rollback matters.

## RH55 - Captured extent plus mutable backing is a separate hazard class

**Rule:** A loop that captures N/begin/end, then invokes callbacks/helpers capable of mutation, and later continues using the captured extent is not mutation-safe merely because it performs local current-bound checks.

**Evidence:**

- Tracker shutdown captures a count, frees entries, and can later erase current range; finite countermodels admit growth/shrink mismatches.
- Deferred resource callback captures descriptor/index and does not revalidate generation/counter after callback return.

**Use:** Search loops containing callback/free/event calls between captured-bound acquisition and later indexing.

## RH56 - Bounded pool exhaustion is a first-class long-session hypothesis

**Rule:** Fixed arrays with sentinel scans/cursors should be audited for explicit exhaustion behavior, high-water marks, reuse discipline, and generation exhaustion.

**Examples:**

- CRdHandleUtil: `0xA00` pointer slots plus `0x7FFF` generation flags.
- CSound: 256 rows.
- CSdMain: 128 rows.
- CSdCore: 32 rows.
- Physics controller table: 128 entries.
- Physics core: 20 scene-like slots.
- Physics thread contexts: bounded record collections.
- Input finalizer carrier: 100 slots.
- Preserve/save staging: 28 record slots.
- Event/DSB-related bounded populations.

**Use:** Long-session diagnostics should log internal pool high-water marks, not only heap/working-set growth.

## RH57 - Poll/producer cadence is part of semantics when smoothing is per invocation

**Rule:** If a filter changes by a fixed amount per producer invocation rather than by delta-time, moving the producer to another polling cadence changes real-time behavior even when samples are numerically identical.

**Evidence:** Input filtering includes fixed per-invocation approach values while the physical-to-logical pipeline is separately staged.

**Use:** An SDL3 backend should inject at the existing acquisition/producer seam or deliberately reproduce its cadence rather than polling opportunistically from a faster independent loop.

## RH58 - Fail-stop allocators and nullable callers form different contracts

**Rule:** If an allocator's selected implementation loops/diagnoses on OOM and has no normal null return, downstream `if (!p)` branches may be effectively unreachable under the original contract. Replacing that allocator with one that returns null can activate untested caller behavior.

**Evidence:** Shared allocation wrapper `00404390`/GOG counterpart repeatedly diagnoses `Singleton Allocate failed` and has no selected normal-null return.

**Use:** Large-address/custom allocator work must preserve or intentionally repair failure semantics, not just allocation ABI.

## RH59 - Sentinel validation that occurs after dereference is not validation

**Rule:** If code computes/dereferences a record first and only then converts category values into sentinel returns, the sentinel protects semantic interpretation, not memory safety/readiness.

**Evidence:** NativeUI selected resolver and related directory/category path.

**Use:** Callers added by ZachFix must prevalidate root readiness and selector range.

## RH60 - One-cell pending handshakes need correlation, not only locking

**Rule:** A shared pending slot plus “wait until cleared” does not prove one-producer/one-completion semantics if a second producer can overwrite the slot and no request token is carried to completion.

**Evidence:** Phase 7 resource request countermodel with producer A, producer B, one pending cell, and a worker consuming B while both callers later observe clear.

**Use:** Instrument request IDs/sequence numbers before treating pending-clear as completion of the caller's own request.

---

# 2. Proposed new engine signatures

These are candidate EP additions after EP18.

## EP19 - 0x18 custom pointer-vector shell

A recurring engine container shell has:

- reference/auxiliary cell at `+0`;
- begin at `+0x0C`;
- end at `+0x10`;
- capacity at `+0x14`;
- shell size `0x18`;
- four-byte element/value transport in confirmed pointer uses.

Confirmed compatible occurrences include node `+0x58` scene membership and the static tracking root. Related begin/end/capacity idioms also occur in worker/container paths, but layout similarity must not be promoted into type/owner identity.

**Recognition value:** high for locating untyped collections; low for semantics by itself.

## EP20 - Fixed-pool + cursor/sentinel allocator

Signature:

1. fixed number of records/slots;
2. per-slot zero/sentinel state;
3. cursor or first-free scan;
4. publication before/around secondary initialization;
5. weak or special exhaustion behavior.

Seen in handles, audio rows, physics controller slots, input finalizers, and several bounded managers.

**Recognition value:** strong candidate generator for long-session and reuse analysis.

## EP21 - Reusable plain-index ABA service

Signature:

- service returns an integer slot rather than a generation handle;
- release clears that slot;
- new allocation uses first-free slot;
- consumers retain only the integer index.

Confirmed example: selected physics controller table. Any similar unknown table should be treated as generation-sensitive until proven otherwise.

## EP22 - Intrusive registration self-link signature

For intrusive active lists with object-local prev/next fields, repeated registration of the same object without membership testing can transform links into self-links while allocating additional external handles and incrementing counts.

Confirmed example: CThrowLure. Search other registration callers for same-object repeated registrar calls, particularly factory registration followed by explicit registration.

## EP23 - Publication-before-admission state machine

Signature:

- object/field/list state is changed first;
- external callback, parser, allocator, or admission follows;
- failure return does not fully restore prior state.

Seen in XCA, resource records, scene membership, active registration, reset/recreation, and save request paths.

**Recognition value:** useful for finding non-transactional error paths and hot-reload hazards.

---

# 3. Composite finding: CThrowLure lifecycle defect chain

## 3.1 What is already statically verified

The same CThrowLure object is registered twice on one straight-line successful path in both PC builds.

First registration:

- puts object O into handle-table slot S1;
- allocates generation G1;
- stores H1 in `O+0x3C`;
- inserts O into the manager list;
- increments manager count.

Second registration:

- sees no existing-membership/handle deduplication;
- inserts the same O again while it is already head;
- transforms object list links `+0x50/+0x54` into self-links;
- allocates fresh H2/S2/G2;
- overwrites `O+0x3C` with H2;
- increments manager count again.

Generic retirement:

- traverses/unlinks using the object's intrusive links;
- decrements manager count once per selected object iteration;
- releases only the **current** handle from `O+0x3C`, so H2 is releasable but overwritten H1 is not discoverable through the object;
- then deletes the object.

Reviewed post-startup reset/cleanup routes do not directly sweep H1. The CRdHandleUtil static constructor clears the table at initialization; the at-exit table teardown is not a stale-entry sweep.

## 3.2 New composite consequence A: net handle-slot pressure

**COMPOSITE HEURISTIC:** Under the simplest successful create -> one retirement episode, one stale H1 mapping remains after H2 is released. Therefore repeated episodes can monotonically consume handle slots even if ordinary object memory is freed correctly.

The pointer table has `0xA00 = 2560` slots and full-table allocation has no normal exhaustion exit.

Idealized empty-table bound, assuming no other active/stale users and exactly one H2 retirement per episode:

- after k completed episodes: approximately k stale H1 pointer slots remain;
- episode 2559 can still obtain two free slots at creation time and then retire to 2559 stale slots;
- the next episode starts with only one free pointer slot, so first registration can consume it and the second registration can encounter the full-table path.

This is **not** a claim that gameplay performs 2559 lure episodes, nor that the table begins empty. Other active objects reduce headroom, while unreviewed global resets could change it. It is a finite upper-bound model that turns the stale-handle observation into a concrete long-session probe.

## 3.3 New composite consequence B: manager-count drift

Two registrations increment manager count twice. A single ordinary successful retirement decrements once.

**COMPOSITE HEURISTIC:** If execution somehow reaches one clean retirement and does not perform a second logically corresponding retirement, arithmetic leaves a +1 count imbalance for that object episode.

Because the self-link interferes with reset/retirement traversal, actual runtime count evolution is path-dependent. This should be instrumented rather than treated as a confirmed persistent counter leak.

## 3.4 New composite consequence C: reset/retirement cursor pathology

Manager reset broadcasts a virtual operation before generic retirement. Both the broadcast and retirement calculate `next = current+0x54` before dispatch/deletion.

For duplicate-registered CThrowLure, `+0x54` points to itself.

Static consequences already supported by the constituent reports:

- in the marker-enabled reset path, next cursor remains the same object and source-level traversal cannot be shown to advance;
- if the object is already marked and generic retirement runs, the first iteration saves the self pointer, deletes the object, and a later iteration can reuse that saved address.

**RUNTIME REQUIRED:** actual hang, repeated callback, use-after-free, or crash.

## 3.5 Best runtime experiment

A low-noise diagnostic build can log only transitions:

- occupied count in CRdHandleUtil pointer slots;
- occupied generation flags;
- CThrowLure `+0x3C`, `+0x50`, `+0x54` after factory registration, after second registration, and at retirement;
- manager object count before/after those operations;
- H1/H2 slot values;
- manager reset entry while a lure is live.

If occupancy rises by one across complete lure create/retire episodes, the static stale-mapping finding becomes a demonstrated runtime leak. If reset repeats the same object cursor, the self-link consequence becomes directly confirmed.

**Priority:** P0 for targeted runtime validation. No production patch should be made before the trace confirms the actual episode/cadence.

---

# 4. Composite finding: reflection bug has an aspect-ratio signature

## 4.1 Verified local arithmetic

The bias helper supports:

- X translation `0.5 + 0.5/W`;
- Y translation `0.5 + 0.5/H`.

The selected caller invokes the width getter twice and therefore supplies W,W.

Local Y difference versus calling the same helper with W,H:

`deltaY = 0.5/W - 0.5/H`

This discrepancy is verified locally. The final shader/TEX causality remains only strongly inferred/unknown, so it is not yet a proved visual bug cause.

## 4.2 New derived fingerprint

Express the same difference in vertical texels by multiplying by H:

`deltaY_tex = H * (0.5/W - 0.5/H)`

`deltaY_tex = 0.5 * (H/W - 1)`

Therefore, if this matrix directly drives the relevant sampled texture coordinates:

| Aspect | Predicted Y discrepancy, vertical texels |
|---|---:|
| 1:1 | 0.00000 |
| 5:4 | -0.10000 |
| 4:3 | -0.12500 |
| 16:10 | -0.18750 |
| 16:9 | -0.21875 |
| 21:9 | about -0.28571 |
| 32:9 | -0.35938 |

The key signature is not the exact sign alone. It is that **at a fixed aspect ratio the offset in texels should stay approximately constant across resolutions**, while changing systematically with aspect ratio and vanishing at square dimensions.

Examples under this hypothesis:

- 1280x720 and 3840x2160 should have roughly the same texel-space contribution from this specific W/W mistake;
- 1280x1024 should show a smaller contribution;
- a square target should remove this particular discrepancy.

This is a much cleaner falsification test than simply comparing 720p versus 4K.

## 4.3 Best runtime experiment

1. Log W/H supplied to the bias helper and the four registers beginning at `0xEF`.
2. Capture the same reflection scene at multiple resolutions with the **same aspect ratio**.
3. Repeat at several aspect ratios.
4. In an experimental build only, replace the second width query with the established height query and compare the image.

A symptom that tracks aspect ratio but not same-aspect absolute resolution would strongly raise this path's probability. A symptom that scales mainly with absolute pixel dimensions would argue for a different source.

**Priority:** P0/P1 targeted graphics experiment.

---

# 5. Structural finding: another false-noreturn/fallthrough scan is justified

The foundational repair proved that a false `noreturn` can hide real callable structure. Phase 7 later found additional diagnostic/invalid-parameter paths that return and continue execution.

This creates a new, precise structural scan recipe:

1. enumerate every call to known diagnostic/assert/invalid-parameter/error wrappers;
2. locate callsites whose current recovered CFG terminates at the call or whose enclosing function boundary ends suspiciously there;
3. inspect raw bytes immediately after the call;
4. test whether those bytes form PE-backed reachable instructions and whether inbound/outbound edges support a continuation;
5. rebuild only those local function/fragment boundaries;
6. compare both builds independently.

This is **not evidence that more than 21,871 structural entries definitely exist**. It is a high-value search because the exact failure mode has already occurred once in a foundationally important dispatcher and later returning-diagnostic examples prove the assumption is not isolated.

**Priority:** P1 static scan.

---

# 6. Fixed pools: a likely source of “long session” failures that heap monitoring misses

The snapshot contains many bounded, old-engine-style pools. Several use cursor/sentinel allocation and not all have robust exhaustion semantics.

A generic diagnostic facility could periodically emit only `(used, high_water, capacity)` for known pools:

- CRdHandleUtil 2560 pointer slots;
- CSound 256 rows;
- CSdMain 128 rows;
- CSdCore 32 rows;
- physics controller table 128;
- 20 PhysicsCore scene-like slots;
- bounded physics thread record queues/contexts;
- selected input finalizer table 100;
- selected save/preserve record arrays;
- other discovered fixed managers once their availability counters are known.

This provides a new explanation class for bugs that appear “after hours”: the heap may remain stable while a fixed internal pool quietly approaches exhaustion.

**Priority:** P1 instrumentation framework, especially useful for ZachFix long-session audit mode.

---

# 7. Resource request handshake: locking is not request correlation

Phase 7 constructed a finite model where:

- producer A publishes request 0x120;
- producer B overwrites the same pending slot with 30000;
- worker consumes B's no-work request and clears pending;
- both callers later see the same cleared pending state and return.

All modeled mutex acquisitions may succeed. The missing element is a request sequence/token that ties completion to the submitting producer.

**COMPOSITE HEURISTIC:** If actual runtime permits overlapping producers, this is a lost/cross-completion hazard even without a traditional unlocked data race.

The most informative trace is therefore not “did the mutex succeed?” but:

`producer_seq -> published packed ID -> worker consumed ID -> callback result -> clear/counter -> producer return`

If every observed producer is externally serialized, the hazard is falsified in practice. If overlapping submissions exist, the one-cell handshake deserves direct repair consideration.

**Priority:** P1/P2 runtime trace depending on observed resource-loading symptoms.

---

# 8. Renderer reset: output-cell aliasing can create a COM leak, but alias existence is still unknown

The corrected reset descriptor formula is:

`D = [owner+0x0C] + index*0x18`

and descriptor state leads to an **output-cell address** S, with COM pointer P stored indirectly at `[S]`.

Pre-reset cleanup releases/clears through S. Post-reset creation writes newly created COM pointers through S. Individual failed creates do not make the outer gate fail.

If two live descriptors share the same output-cell address S:

1. pre-reset first descriptor releases P and clears S;
2. second descriptor sees S already null and does not release another object;
3. post-reset first descriptor writes new P0 to S;
4. post-reset second descriptor writes P1 to the same S;
5. no intervening `Release(P0)` is inherent in that write sequence.

That is a valid static alias countermodel and would leak P0's reference if the alias exists.

**UNKNOWN:** whether the actual current descriptor population ever contains duplicate S values.

A very cheap runtime/static-enumeration probe is to dump all descriptor output-cell addresses before Reset and detect duplicates, then log per-descriptor Create HRESULT and final cell value.

**Priority:** P1 if device-reset bugs remain relevant; otherwise P2.

---

# 9. NativeUI and actor-state dispatch expose hidden admission contracts

## NativeUI

Selected resolver behavior constructs/reads a directory record before its later category-to-sentinel interpretation. Constructor state can have table pointer `+0x44 = 0`, and the selected resolver has no local readiness or code/count bound before reads.

**Engineering consequence:** ZachFix NativeUI integration should never use the low-level resolver merely because a CMessage-like object has been constructed. It should gate on proven table publication/readiness and a valid selector domain.

## Player actor state

The 137-word selected handler table contains:

- 119 nonzero cells;
- 18 zero cells;
- only 82 distinct nonzero targets;
- null indices `0x2D` and `0x69..0x79`.

The selected dispatch seam uses state `+0x654` as a full DWORD index and calls the table cell without a local null/bounds check.

**Engineering consequence:** any ZachFix experiment that writes player state directly must recover the legitimate producer/admission domain first. The contiguous 0x69..0x79 hole is also a good static-search target: scan all state writers/comparisons for these values to determine whether this is a reserved/removed state family, but no semantic label is justified yet.

**Priority:** P1 for NativeUI integration safety; P2/P3 for state taxonomy mining.

---

# 10. Identity reuse: audio and physics have ABA-shaped domains

## CSdMain

The 128-row Main token uses approximately `(sequence << 8) | row`, with sequence cycling through `1..0x7FFFFE` and new Main instances restarting at sequence 1. Resolver rejects `0xFFFFFFFF` but indexes by low byte before a local `<128` gate.

Consequences:

- token equality is not eternal sound-instance identity;
- a malformed/non-producer token with low byte 0x80..0xFF is not locally range-protected;
- ZachFix should validate token domains before injecting/caching arbitrary values.

Wrap intuition only, not runtime cadence evidence: the sequence space contains 8,388,606 normal values. At one allocation per second this is about 97 days; at 10/s about 9.7 days; at 60/s about 38.8 hours; at 100/s about 23.3 hours. A newly reconstructed Main can collide at sequence 1 much sooner. Actual allocation rate and stale-token retention are unknown.

## Physics controller

The 128-entry controller table returns a plain first-free index with no generation tag. Release clears that slot, and a later creation can reuse the same number for a different provider object.

**Engineering consequence:** never retain a controller index across a provider/root generation change in new code.

**Priority:** P2 unless runtime symptoms point here.

---

# 11. Allocation tracker: duplicate admission plus raw-free shutdown deserves a separate invariant check

The shared allocation wrapper mallocs, zeroes, and appends the pointer to an in-place tracker compatible with the 0x18 vector shell. Application shutdown has a separate indexed retirement path that requests raw free for each tracked entry.

Phase 7 proved the append path does not suppress equal pointer values. A finite model with duplicate P entries therefore requests `free(P)` twice during stable retirement.

This does **not** prove that the normal allocator pipeline actually inserts the same real non-null pointer twice. It means uniqueness is an upstream invariant and should be checked explicitly rather than assumed.

A useful low-cost debug assertion in a diagnostic build would count duplicate non-null pointer values in the tracker before shutdown. If duplicates never occur, the static double-free-shaped countermodel is practically discharged for ordinary sessions. If they do, provenance should be logged immediately.

**Priority:** P2 diagnostic assertion.

---

# 12. Cross-subsystem architectural fingerprint: four states should be tracked separately

Many confusing findings become clearer if each asynchronous/resource-like path is modeled with four independent axes:

1. **Published** - a pointer/token/flag is visible to other code.
2. **Admitted** - downstream logic accepted it as a valid request/state.
3. **Completed** - requested work/backend operation finished successfully.
4. **Retired** - no borrower/backend/GPU/worker can still use it.

Deadly Premonition repeatedly allows transitions such as:

`Published -> Rejected`

`Published -> Admitted -> Logical done, backend unknown`

`Old retired logically -> old physical use unknown`

`New published -> old generation still borrowed`

This four-axis model is more accurate for ZachFix hooks than a binary `valid/invalid` state and should be used in future RE notes and instrumentation.

---

# 13. Practical priority list

| Priority | Target | Why |
|---|---|---|
| **P0** | CThrowLure create/retire/reset trace | Strongest new composite defect chain; finite counters and exact fields available |
| **P0/P1** | Reflection W,W A/B test | Verified local discrepancy plus unique aspect-ratio fingerprint |
| **P1** | Returning-diagnostic / false-noreturn structural scan | Exact historical failure mode, could recover more callable structure |
| **P1** | Internal pool high-water telemetry | Broad payoff for long-session bugs without heavy logging |
| **P1** | NativeUI readiness/admission guard mapping | Directly relevant to ZachFix 0.3.x UI work |
| **P1** | D3D reset descriptor output-cell duplicate scan | Cheap way to confirm/falsify COM alias leak hypothesis |
| **P1/P2** | Resource pending request correlation trace | Could reveal lost/cross-completed loads if producers overlap |
| **P2** | Audio token/provider generation logging | ABA and malformed-token domain, runtime impact unknown |
| **P2** | Physics controller generation wrapper | Plain reusable index can target new occupant if cached stale |
| **P2** | Allocation-tracker duplicate-value assertion | Can cheaply falsify a shutdown double-free-shaped model |
| **P2/P3** | Player state hole producer scan | May expose reserved/removed state family; no current bug proof |

---

# 14. Suggested runtime diagnostic counters for ZachFix

A minimal, low-overhead “archaeology diagnostics” mode could log only state changes/high-water records rather than every frame:

### Handles / active objects

- CRdHandleUtil occupied pointer slots / peak
- occupied generation flags / peak
- allocation cursor wrap/exhaustion attempts
- CThrowLure H1/H2, `+3C/+50/+54`
- active manager count delta per register/retire

### Renderer

- reset descriptor count
- duplicate output-cell S addresses
- failed recreation HRESULT count
- cells left null after outer reset gate returns eligible
- W/H passed to reflection bias helper

### Resources

- monotonically assigned debug request sequence external to the game protocol
- published packed ID
- worker-consumed ID
- callback return/commit
- pending-clear owner sequence
- outstanding-counter high water

### Fixed pools

- only used/peak/capacity for known bounded tables

### Generation/freshness

- optional debug generation counters beside SDL input snapshot, D3D reset generation, and selected resource bank generation; these are ZachFix-only diagnostics, not claims about native data layout.

Such telemetry is more likely to uncover rare architectural failures than another broad per-frame function log.

---

# 15. Implications for SDL3 input integration

The existing input architecture already provides a good replacement seam at physical acquisition/raw state before the game's logical-action machinery. The new heuristic result adds one important constraint:

**Do not change the producer cadence accidentally.**

Because selected smoothing/approach behavior is per producer invocation rather than uniformly delta-scaled, polling SDL3 from a much faster independent thread and pushing every sample through the native producer can change real-time response. Better options are:

- poll/cache SDL3 independently but inject one snapshot only when the native acquisition seam runs; or
- intentionally emulate the original producer cadence if the native seam is bypassed.

This preserves the useful native logical/action/pending/live behavior while avoiding a hidden “same values, different feel” regression.

---

# 16. What this pass does not claim

The following remain deliberately unclaimed:

- a runtime CThrowLure hang/UAF/table exhaustion has occurred;
- the reflection W,W discrepancy is the visual reflection-offset bug;
- a real descriptor output-cell alias currently exists;
- resource producers overlap in a live session;
- audio token wrap/stale clients are observed;
- physics stale-index clients exist;
- NativeUI invalid selectors occur naturally;
- further false-noreturn structural entries definitely exist;
- every fixed pool has an exhaustion bug.

The point of the heuristic pass is to turn broad UNKNOWNs into **small falsifiable experiments** and reusable RE rules.

---

# Bottom line

The Mega RE snapshot has reached a stage where the most valuable discoveries no longer come only from naming another function. The higher-yield method is now to combine already verified local mechanics and ask whether they form repeated architectural invariants or finite countermodels.

The strongest new direction is CThrowLure because several independent systems line up into one exact defect chain: duplicate intrusive registration, stale first handle, self-link, count arithmetic, single-current-handle retirement, bounded handle table, and non-progressing reset cursor geometry. It is exceptionally suitable for a tiny runtime probe.

The cleanest graphics direction is the reflection bias path because its W,W discrepancy predicts a specific aspect-ratio-dependent, same-aspect-resolution-independent texel signature. That makes it unusually easy to falsify.

For the broader project, the most useful new meta-rule is to model **publication, admission, completion, and retirement as four different states**. Deadly Premonition repeatedly separates them, and many of its strangest lifetime, reset, load, audio, and rendering behaviors become much less mysterious once those states are not collapsed into one “success” flag.
