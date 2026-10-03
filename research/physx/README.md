# Deadly Premonition PC PhysX research — retired production work

**Status:** research retained, runtime patch retired
**Last updated:** 2026-10-04
**Production policy:** ZachFix does not modify PhysX, rigid-body timing, vehicle-physics cadence, or physics solver settings.

This file is the compact canonical record of the PC PhysX/physics-timing investigation. The 2026-10-01 closure pass rechecked the earlier runtime campaign against both Steam/GOG exports, PhysX 2.8.1 semantics, the Xbox scheduler, and the current object/CCT/vehicle maps. Detailed address evidence is retained in [`../evidence/physx_timing/README.md`](../evidence/physx_timing/README.md).

## 1. Confirmed solver contract

### `gameDelta60` is not seconds

The PC main timer produces approximately:

```text
gameDelta60 = realFrameSeconds * 60
```

It is a 60-Hz-relative gameplay scalar. Both builds then use this value on the native ordinary physics path where PhysX expects elapsed seconds.

The helper map was corrected during the closure pass:

| Role | GOG | Steam |
|---|---|---|
| central timing producer | `FUN_00700670` | `FUN_00700650` |
| sign/mode helper | `FUN_0041C290` | `FUN_0041C270` |
| downward-rounding wrapper | `FUN_006C3E20` | `FUN_006C4310` |
| upward-rounding wrapper | `FUN_007010B0` | `FUN_00701150` |

The two scalar transforms are rounding helpers, not hidden gameplay time-scale functions. `gameDelta60/60` is dimensionally seconds, but runtime evidence does not justify treating it as exact downstream wall time in every state.

### Ordinary scene dispatch is common, not Scene-0-only

The ordinary dispatcher fans one incoming elapsed value into a table of up to 20 active scene records.

| Role | GOG | Steam |
|---|---|---|
| top-level submit | `FUN_006EB3D0` | `FUN_006EB3C0` |
| common producer | `FUN_006EAB70` | `FUN_006EAB60` |
| timing/capacity selection | `FUN_006EACF0` | `FUN_006EACE0` |
| game-owned `NxScene::setTiming` configurator | `FUN_006EAE30` | `FUN_006EAE20` |
| queue copy | `FUN_0040BCB0` | `FUN_0040BCE0` |
| bounded-ring enqueue | `FUN_0040BC20` | `FUN_0040BC50` |
| ordinary worker | `FUN_0040BAC0` | `FUN_0040BAF0` |

The GOG producer waits for the previous batch before building the next one. Timing is installed synchronously through scene vtable `+0x148` before enqueue. The worker reads scene plus elapsed, clamps elapsed to about `1/15 s`, then performs:

```text
simulate -> flushStream -> fetchResults
```

The copied queue field historically labelled `task+0x08 maxIter` is bookkeeping, not the live timing authority.

### Native live `maxIter` is capacity

The ordinary native cascade is:

```text
< 1.5 -> 1
< 2.5 -> 2
< 3.5 -> 3
else  -> 4
```

PhysX 2.8.1 documents fixed-step remainder/debt accumulation and `maxIter` as the maximum number of fixed substeps that may be consumed by a `simulate()` call. This directly explains why corrected `1/45 s` elapsed starves with `maxIter=1`: the solver needs an eventual multi-step call to consume accumulated debt.

The still-open SDK edge is whether repeated `NxScene::setTiming()` calls preserve, reset, or transform that accumulated remainder.

### Special timing records are now exact

The special table used by `FUN_006EAE30` / `FUN_006EAE20` decodes as:

```text
record 0      ~= 1/60 s, maxIter 0, fixed method
records 1..19 ~= 1/30 s, maxIter 1, fixed method
```

When the special path selects record 0, a zero incoming `maxIter` is promoted to effective `1`. Therefore “all secondary scenes are always 1/30” is wrong: `1/30,maxIter1` is a special-policy record, not a universal secondary-scene rule.

### Synchronous catch-up is a separate transaction

The catch-up/resync path saves scene timing, installs approximately `0.05 s` with `maxIter=20`, performs synchronous `simulate -> flush -> fetch`, then restores timing.

| Role | GOG | Steam |
|---|---|---|
| save/set/simulate | `FUN_0040B820` | `FUN_0040B850` |
| fetch/restore | `FUN_0040B910` | `FUN_0040B940` |

This path must remain excluded from any ordinary elapsed/capacity experiment.

## 2. Runtime results

### QPC elapsed fixes the measured rigid-body timebase

Replacing ordinary Scene-0 elapsed with direct QPC wall seconds made the independent vehicle-yaw invariant converge near one:

```text
abs(poseYawRate) / abs(NxActor.angularVelocity.y) ~= 1
```

This remained the strongest positive solver result across the cap matrix.

### Corrected elapsed with `maxIter=1` starves fractional low FPS

At 45 FPS, real elapsed is about `1/45 s` while the fixed step is `1/60 s`. PhysX needs accumulated remainder and an occasional second substep. A live one-step cap cannot service that pattern and produced approximately the observed `0.75x` solver clock.

### Legacy oversized elapsed plus elevated capacity produces the ~24-FPS collapse

The maxIter-only split retained approximately `0.0666667 s` passed elapsed while live `maxIter=4`, resulting in four fixed steps per submission and a self-sustaining performance loop around 23.6–24 FPS.

The conclusion is narrower than “maxIter 4 is bad”:

> **Elevated capacity is unsafe while the same submission can still carry legacy oversized elapsed.**

### v6-A isolated the solver from the vehicle layer

The clean Scene-0 QPC + eligible live `maxIter=1` test removed the high-refresh collapse and normalized the rigid-body timebase, but player throttle became extremely weak. That proves solver timing and vehicle setter magnitude are separate contracts.

## 3. Vehicle boundary

The normal player-car PhysX-facing wheel path is:

| Role | GOG | Steam |
|---|---|---|
| wheel setters | `FUN_00555C20` | `FUN_00555B50` |
| direct chassis pose/velocity path | `FUN_005578A0` | `FUN_005577D0` |

The wheel path writes persistent `NxWheelShape` motor/brake values multiplied by the central gameplay scalar. Xbox homologs do the analogous multiply, so the multiplication itself is not a PC-only invention. The relevant historical difference is scheduler cadence.

The direct chassis path also performs a conditional linear-velocity read/modify/write through the actor ABI. The local write is skipped under one branch condition; otherwise a component can be threshold-tested and multiplied by `0.3` before being written back. Its exact runtime cadence and ordering relative to submit/fetch remain open and can invalidate a solver-only interpretation if it overwrites freshly fetched state.

Do not globally divide vehicle values by `gameDelta60`. State-87 steering is already delta-aware, visual/readback paths have their own semantics, and persistent motor/brake values are only one boundary class.

## 4. CCT boundary

`NxController::move` is immediate and its hit callbacks occur when the move call is made. CCT caller cadence is therefore separate from scene simulation.

| Role | GOG | Steam |
|---|---|---|
| common CCT wrapper | `FUN_006F9DD0` | `FUN_006F9DC0` |
| common/root-motion path | `FUN_00481D70` | `FUN_00481C60` |

The previous report that GOG `FUN_004E31E0` has 37 direct callers does not survive the current export audit. The function body still contains the fixed-style downward correction, but current call/xref/raw scans do not establish those callers or its live cadence. Treat its reachability as OPEN until the current indirect/vtable path or replacement is identified.

## 5. Event 6 / physical-prop ordering

The old `FUN_0053E8C0 = Event 6 dispatcher` identity is stale for the current exports.

Current GOG `FUN_0055F6A0` explicitly handles event ID 6 as a bounded object state machine: actor setup, phase advance, one force application at phase 2, and release at phase 5. It is not a continuous force stream.

Steam scheduler `FUN_006C5FF0` establishes:

```text
state 14 -> physics submission through FUN_006EB3C0
state 7  -> completion/fetch bridge
state 8  -> eligible object vtable +0x1C receives Event 6
```

Event 6 therefore executes after the physics completion bridge in the scheduler pass, not from a PhysX solver substep. Exact CCT/vehicle object-vtable ordering remains open.

## 6. Xbox comparison

Original Xbox generated PPC logic contains a discrete counter-difference `<2` wait/yield gate before the later gameplay update and central timing-scalar store. PC and Xbox CCT/vehicle code are structurally close, while PhysX ABI offsets are not globally identical.

This makes scheduler/cadence a strong explanation for inherited fixed-per-call behavior on PC, but it does not justify a universal PC 30-Hz gate. The v7 player-car-only 30-Hz experiment reached its intended cadence and still produced a separate ~22–24-FPS feedback failure.

## 7. Best-supported future solver direction

The strongest research direction remains an atomic common ordinary-scene transaction:

```text
previous-batch barrier
-> acquire valid real wall elapsed
-> preserve native ordinary/special timing policy
-> choose bounded live fixed-step capacity for the same submission
-> native setTiming
-> enqueue matching elapsed
-> native worker simulate / flush / fetch
```

This is not a production specification yet. It must exclude zero/reset and synchronous catch-up paths and fail closed whenever build identity, elapsed baseline, or native timing classification is uncertain.

Three implementation-blocking facts remain:

1. repeated-`setTiming` behavior of the PhysX fixed-step accumulator/debt;
2. explicit QPC/reset/debt policy across startup, pause, loading, alt-tab, debugger gaps, and hitches;
3. exact object-phase ordering/cadence of live CCT and direct vehicle actor-state writes relative to ordinary submit/fetch.

The next useful work is a small runtime characterization set for those three questions. Another broad static census is not required before that.

## 8. Retired experiments and dead ends

The following experiments were useful diagnostically but are not production designs:

- Scene-0-only QPC elapsed;
- common/all-scene elapsed correction without a coherent capacity transaction;
- broad live `maxIter=4` escalation;
- fixed-60 dispatcher scheduling;
- Xbox-like 30-Hz player-car-only cadence;
- standalone player motor/brake normalization.

Do not repeat these interpretations:

- `gameDelta60` is PhysX seconds;
- `gameDelta60/60` is always exact downstream wall time;
- Scene 0 is the only ordinary elapsed boundary;
- queued `task+0x08` controls live `maxIter`;
- scene vtable `+0xFC` is `isWritable`;
- all secondary scenes are unconditionally fixed 1/30;
- `maxIter=4` is inherently unsafe regardless of elapsed;
- `FUN_004E31E0` has 37 verified current callers;
- `FUN_0053E8C0` is the verified current Event 6 dispatcher;
- all vehicle or CCT values should share one global physics delta.

## 9. Production cleanup

All earlier runtime experiments, telemetry-only physics hooks, scheduler candidates, `[Physics]` INI switches, research-only RVAs, and compilable probe sources remain removed from the production tree. The current ZachFix code does not intentionally change Deadly Premonition's PhysX scene timing, solver settings, vehicle-physics cadence, or motor/brake values.

## 10. 2026-10-04 Mega Census context/vector addendum

The timing transaction above remains the canonical paired-build result, but the latest
Steam Phase 4 Census adds a more precise container/lifecycle model around the ordinary
worker.

Four in-place `CPhysicsThread` contexts begin at `00BDA010` with stride `0x2C`. Each
contains a typed `CNArray<CPhysicsThread::SCENE>` at `+0x1C`; the actual storage pointer,
capacity, and count are context `+0x20/+0x24/+0x28`. Construction reserves eight
`0x18`-byte records.

The selected producer deduplicates all six dwords of a record and appends directly to the
same physical vector later read by the worker. There is no proved queued-to-active vector
swap. Selected record roles are now bounded as pointer `+0x00`, float `+0x04`, opaque
forwarded value `+0x08`, pointer-slot ordinal `+0x0C`, and context-specific `+0x10/+0x14`.
The latter pair must not be relabeled as generic timing fields.

`0040B630` is an available same-context activation helper: it can route the four objects
through the generic CreateThread path and each object's slot-4 worker. The bounded source
scan did not find an incoming call to `0040B630`; actual activation remains UNKNOWN rather
than disproven. Pointer invalidation nulls the first matching record pointer without
compaction/count change, and the selected destructor wakes then frees the vector without
a proved join. Capacity growth/saturation behavior and safe quiescence therefore remain
architectural unknowns.

These facts do not create a new production physics candidate. For exact proof scope see
`../evidence/mega_re_census_2026-10-04/boundaries/physics_context_vector_activation.md`.
