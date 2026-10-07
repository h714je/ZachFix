# Engine Recognition Patterns

**Reviewed:** 2026-10-04  
**Evidence horizon:** Phase 4 through sequence 55  
**Purpose:** candidate-generation signatures extracted from repeated Deadly Premonition engine structures.

These patterns are **recognizers, not semantic proof**. A match ranks a candidate for primary review. All promotions remain governed by `maps/RESEARCH_HEURISTICS.md` and the normal ledgers/findings workflow.

Confidence labels in this map describe the recognition pattern, not every future hit:
- **HIGH:** repeated across independent typed subsystems or directly anchored by generic infrastructure;
- **MEDIUM:** repeated but with narrower scope or unresolved lifecycle/runtime limits;
- **LOCAL:** useful pattern currently established in one bounded family only.

---

## EP01 - CRdObject/native-task adapter signature

**Confidence:** HIGH  
**Scope:** established CRdObject/task family only.

Candidate shape:

`factory/register -> object callback field +0x44 -> synchronous event0 -> scheduler-sensitive virtual +0x0C / event1 -> class-specific work -> retirement marker / event2 -> later manager cleanup`

Established anchors include:
- `findings/boundaries/native_task_callback_lifecycle.md`;
- `findings/boundaries/native_task_connective_architecture.md`;
- `findings/boundaries/cshop_callback_retirement.md`;
- `findings/boundaries/cmenu_organizing_task_use.md`.

Recognition use:
- search callback-setter callsites and literal callback values;
- recover missing raw callback entries;
- decode event maps/jump tables;
- identify the producer/factory selector and registered object family.

Limits:
- event0/event1/event2 meanings are not global engine-wide meanings;
- `+0x44` is not globally a callback field across unrelated classes;
- retirement marker/event2 does not prove delete/free.

---

## EP02 - Scheduled adapter -> separate controller receiver switch

**Confidence:** HIGH  
**Scope:** typed controller/task pairs.

A scheduled CRdObject-derived task may only provide cadence/event delivery while real subsystem state lives in a separate static/root object.

Strong CMenu instance:
- request constructs/registers a CRdObjectModel task;
- callback receives the task;
- event1 preserves/passes the task but switches `ECX` to static `CMenu 01476978` before invoking the real menu handler.

Recognition use:
- in unknown callbacks, look for early save of incoming task followed by absolute/global receiver load into ECX;
- distinguish scheduler-owned adapter fields from controller-owned state.

Limit: do not infer this split merely because a callback touches a global.

---

## EP03 - Active-object field cluster

**Confidence:** MEDIUM-HIGH  
**Scope:** CRdObject-derived active objects.

Useful field constellation from current object-lifecycle evidence:
- `+0x3C`: packed CRdHandleUtil registration handle in established families;
- `+0x44`: generic callback field in native-task family;
- list/link/control fields around `+0x50/+0x54` in established active-object layouts;
- retirement/category control around `+0x29`;
- control/exclusion flags around `+0xD8/+0xDC` in selected families.

Recognition use:
- score constructors/methods by multiple matching behaviors, not one offset;
- cross-check vtable family and registration call before typing.

Limit: no individual offset is globally diagnostic.

---

## EP04 - Embedded helper vs independently registered object

**Confidence:** HIGH

Observed owner/helper pairs include:
- CPlayer + CMustachesAdmin;
- CNpcKiller + CNpcRecord;
- CFishingPerson + CMustachesAdmin;
- CMenu embedded helpers/layout structures;
- selected sound-loop/helper records.

Recognition rule:
1. a vtable, ctor, and dtor prove a real C++ subobject candidate;
2. independent game-object status requires a separate registration/handle/list path or equivalent root lifecycle;
3. owner-driven construction/destruction with no independent registration is evidence for embedded/helper role.

This pattern prevents inflation of the class/object graph.

---

## EP05 - In-place static service/root signature

**Confidence:** HIGH

Typical shape:

`small static initializer -> absolute .data receiver in ECX -> ctor -> at-exit/finalizer registration carrying same receiver`

Current examples include CMap, CMenu, CDemo/CDemoMovie, CLoadThread, and the CPhysicsThread static context array.

Recognition use:
- scan initializer tables and orphan init shims for absolute `.data` receivers;
- pair with RTTI/vtable writes and registered finalizer receiver;
- inspect cross-subsystem readers of the same storage.

Limits:
- zero-filled `.data` roots do not have an on-disk runtime vptr value;
- presence of a deleting destructor does not make the static receiver heap-owned;
- registered finalizer does not prove observed process-exit invocation.

---

## EP06 - Lazy singleton getter signature

**Confidence:** HIGH

Typical candidate shape:

`global/cache read -> null test -> allocation -> ctor with ECX=allocation -> optional table/setup -> publish cache -> return same object in EAX`

Recognition use:
- recover getters decompiled as `void` when raw EAX is returned;
- connect the cache global to ctor/vtable and consumers;
- compare against static in-place roots before calling something a singleton.

Limit: singleton-capable class does not imply every instance is the singleton.

---

## EP07 - Singleton-capable class can also have independent instances

**Confidence:** MEDIUM-HIGH

CSdCore provides a strong counterexample to a simplistic "class has singleton getter -> only one instance" assumption. A class/constructor family may support both a lazy global singleton and separate static or embedded instances.

Recognition use:
- when typing a receiver by constructor/vtable, track allocation/root provenance independently from class identity.

---

## EP08 - CThfunc OS-worker signature

**Confidence:** HIGH

Established generic path:

`CThfunc-derived receiver -> generic start helper 00712A20 -> CreateThread(receiver as LPVOID, entry 00712D30) -> entry restores receiver as ECX -> virtual slot +4`

CPhysicsThread provides a concrete typed instance; CLoadThread supplies related worker evidence.

Recognition use:
- enumerate callsites of the generic start helper;
- recover receiver provenance, RTTI/vptr, slot+4 target, static/lazy root, and cleanup;
- separately search incoming sources of omitted/raw start shims.

Limit: an available start path with no live caller does not prove actual runtime activation.

---

## EP09 - Fixed-stride manager/record-array signature

**Confidence:** HIGH as a structural recognizer, LOW for concrete type naming.

Candidate cues:
- constructor receives or establishes count/capacity;
- `count * stride` allocation or static array iteration;
- per-record sentinel/default initialization;
- index/ordinal -> `base + stride * index` consumers;
- cleanup loop over the same stride;
- optional callback, lock, cursor, generation, or root publication.

Examples span resource descriptors, physics records/contexts, audio record arrays, event handles, and other manager tables.

Limit: equal count/stride does not imply equal type or ownership.

---

## EP10 - Requested/pending vs current/committed state pair

**Confidence:** HIGH

Repeated shape:

`requested/deferred value -> compare against current/committed value -> conditional side effect -> commit/copy/update current`

Examples:
- native-task current/deferred selector state;
- world CObject desired `+0x444` vs current `+0x12`;
- resource worker queue/direct pending/control state;
- UI/Fade request state vs separately reacquired predicate state;
- save build/write states as separate phases.

Recognition use:
- search same-domain fields with compare -> effect -> copy/update sequence;
- prefer neutral names such as `requested/current` until consumer semantics are typed.

---

## EP11 - Packed handle signature

**Confidence:** HIGH for established CRdHandleUtil family.

Candidate cues:
- sentinel such as `0xFFFFFFFF`;
- resolver call before object dereference;
- explicit release/unregister path;
- packed index/generation behavior;
- object field retaining the packed value rather than a directly dereferenced pointer.

Recognition use:
- distinguish object handles from raw pointers, indices, and inline addresses;
- flag overwrite of an existing non-sentinel mapping as a potential stale-handle/lifetime obligation.

---

## EP12 - Scheduler-sensitive vtable fingerprint

**Confidence:** MEDIUM

Rather than treating all slots equally, cluster object families by overrides at scheduler/lifecycle-sensitive slots already touched by the census, including `+0x0C`, `+0x1C`, `+0x30`, `+0x48`, and neighboring proven family-specific slots.

Recognition use:
- compare unknown vtables against typed CRdObject/CRdObjectModel/actor families;
- combine shared target slots, override targets, factory selector, and registration behavior;
- use the result to rank class-family candidates.

Limit: slot meaning is family- and phase-specific. A shared slot target is not enough to name a class.

---

## EP13 - Callback event-map/jump-table signature

**Confidence:** HIGH as a boundary recognizer.

Raw callback blocks may:
- consume only the low byte of an event;
- remap through a compact byte table;
- jump through a target table;
- have no exported function entry in the decompiler manifest.

CMenu is a clear instance. Similar callback-driven paths exist elsewhere.

Recognition use:
- literal callback pointer -> raw block -> event byte map -> target table -> typed receiver transitions.

Limit: event numbers remain local until producer/consumer semantics are established.

---

## EP14 - Model/resource binding signature

**Confidence:** LOCAL-MEDIUM

Current CItem path:

`numeric/object value -> CGame DC-stride row -> formatted "%s.XMD" / "%s.XPC" names -> CRdData typed lookups -> same CItem resource slots +160/+164`

Recognition use:
- search sibling actor/object factories for the same row/name/lookup/publication shape;
- use factory type context to rank candidate resource-binding mechanisms.

Limits:
- suffix does not prove exact returned concrete class;
- successful lookup/load and ownership remain separate.

---

## EP15 - Model/cache production pipeline

**Confidence:** MEDIUM

Selected animation/vehicle evidence supports a recurring shape:

`input/local blocks -> object state copies -> base/model transform preparation -> explicit evaluation -> cached/output state -> conditional packet population -> scene/render handoff`

Recognition use:
- locate sibling model classes through shared evaluation and packet helpers;
- distinguish producer/evaluator, cache, packet population, and final consumer.

Limit: valid existing cache/packet can bypass population, so consumer use does not prove same-frame freshness.

---

## EP16 - Persistent-bit hysteresis recognizer

**Confidence:** LOCAL

CNpcEnemy currently shows a classic two-threshold state policy:
- prior bit set: clear at/below the lower threshold;
- prior bit clear: set at/above the upper threshold;
- between thresholds: preserve prior state;
- unordered x87 case also preserves prior state.

Recognition use:
- when an unknown policy reads a prior bit, compares one scalar to two ordered thresholds, and conditionally preserves the old bit, test a hysteresis hypothesis before inventing multiple modes.

Limit: threshold units and gameplay meaning remain local until independently typed.

---

## EP17 - Latch producer/test/clear handshake

**Confidence:** LOCAL-MEDIUM

Presentation/effects evidence shows a useful cross-root pattern:

`producer sets latch -> later subsystem tests latch -> conditional action -> latch clear/reset -> presentation/draw guard`

Recognition use:
- cluster global byte/dword writers and test/clear consumers across subsystem boundaries;
- look for a one-shot handshake before calling a global a permanent mode flag.

Limit: exact latch meaning, lifetime, and last-writer freshness remain local.

---

## EP18 - Publication before validation/null-gate

**Confidence:** MEDIUM

Several mechanisms publish or stage a returned value before later validation or optional use. The existence of a stored pointer/value therefore may prove only retention of the raw result, not successful readiness.

Recognition use:
- order stores, null/status tests, and first dereference explicitly;
- distinguish `published`, `validated`, and `consumed` milestones.

CItem resource slots and selected cache/resource paths are useful examples.

---

# Cross-build candidate prior

## CB01 - Regional Steam -> GOG address-delta ranking

**Confidence:** HIGH as a ranking prior, ZERO as standalone homology proof.

Current `ledgers/HOMOLOGY_LEDGER.csv` through seq55:
- 272 paired rows;
- 260 `VERIFIED` paired rows;
- top ten exact deltas cover 229/260 verified pairs, about 88.1%;
- most common deltas are `+0x100` (90), `+0xD0` (67), `-0xB0` (18), `+0x20` (14), `-0x500` (10), `0` (7), `+0xE0` (6), `-0x4F0` (6), `-0x2F0` (6), `-0x40` (5);
- Steam `0x5Fxxxx`: 127 verified pairs, 89 at `+0x100`, 37 at `+0xD0`, one at `+0xB0`;
- Steam `0x6Cxxxx`: 18 verified pairs, 10 at `-0x500`, 5 at `-0x560`, 3 at `-0x4F0`.

Candidate workflow:

`Steam address -> regional delta candidates -> raw boundary/constant/call/field/vtable/string comparison -> canonical homology review`

Never write a homology row from address delta alone.

---

# Suggested read-only scanner backlog

Scanner output should be candidate-only and should never mutate canonical ledgers automatically.

| ID | Scanner | Seed | Primary payoff |
|---|---|---|---|
| S01 | Native-task callback census | callback setter callers + callback literals | unknown UI/event/gameplay controllers and adapter tasks |
| S02 | Phase-slot vtable fingerprint | typed and unknown vtables | actor/object family clustering |
| S03 | CThfunc worker census | callers of generic thread-start helper | remaining game-owned OS workers |
| S04 | Static-root ctor/finalizer census | initializer tables + absolute `.data` receivers | EVENT_SCRIPT / PLATFORM / service roots |
| S05 | Lazy-singleton getter census | cache/null/alloc/ctor/publish/EAX shape | typed roots and decompiler-void getter recovery |
| S06 | Fixed-stride manager census | stride arithmetic + count/capacity + cleanup | record managers and container families |
| S07 | Continuity/freshness lint | producer -> opaque call -> later same-field read | report validation and false temporal joins |
| S08 | Regional homology candidate generator | verified homology delta distribution | faster GOG counterpart search |
| S09 | Orphan callback/boundary scan | callback/vtable/jump/initializer pointers absent from function inventory | hidden raw entry recovery |
| S10 | Model/resource-binding sibling scan | `.XMD`/`.XPC`, CRdData, publication slots | sibling actor resource mechanisms |

# Promotion discipline for scanner hits

A scanner hit should enter the workflow as one of:
- `candidate` in scratch/report output;
- a bounded `UNKNOWN_QUEUE` item with an exact discriminator;
- a targeted primary inspection request.

Only reviewed primary evidence can update `FUNCTION_LEDGER`, `CLASS_LEDGER`, `CLAIM_LEDGER`, `HOMOLOGY_LEDGER`, or subsystem completion state.
