# PhysX timing closure evidence

**Date:** 2026-10-01  
**Targets:** Deadly Premonition: The Director's Cut PC, GOG 1.01b and Steam 1.01b  
**Status:** static/runtime reconciliation complete enough to define the remaining blockers; no production patch is approved

This dossier is the compact evidence anchor for the 2026-10-01 PhysX timing closure pass. It supersedes older address-level assumptions where noted below. The production tree still leaves native PhysX timing untouched.

## Source precedence

For PC claims, prefer exact executable bytes/raw assembly, then matching runtime telemetry, then PhysX 2.8.1 headers/source, then current reconciliation notes. For Xbox comparison, prefer original XEX/generated surrounding PPC logic; DPRecomp hooks are landmarks only.

## Central PC timing scalar

Both supported PC builds derive the central gameplay scalar from wall-clock time and then scale it into 60-Hz-relative units:

```text
gameDelta60 ~= realFrameSeconds * 60
```

The value is clamped at `8.0` and can pass through native whole-tick/rounding logic before reaching gameplay consumers. It is not elapsed seconds.

| Role | GOG | Steam |
|---|---|---|
| main timing producer | `FUN_00700670` | `FUN_00700650` |
| sign/mode helper | `FUN_0041C290` | `FUN_0041C270` |
| downward-rounding wrapper | `FUN_006C3E20` | `FUN_006C4310` |
| upward-rounding wrapper | `FUN_007010B0` | `FUN_00701150` |

The two rounding wrappers are not gameplay time-scale functions. Residual runtime differences between `gameDelta60/60` and direct QPC wall seconds therefore remain a state/rounding/baseline question rather than evidence for an unknown scale multiplier.

## Ordinary PhysX scene transaction

The ordinary PC physics path passes one incoming elapsed scalar into a scene table of up to 20 entries. The issue is not Scene-0-only.

| Role | GOG | Steam |
|---|---|---|
| top-level physics submit | `FUN_006EB3D0` | `FUN_006EB3C0` |
| common scene producer | `FUN_006EAB70` | `FUN_006EAB60` |
| timing/maxIter selection | `FUN_006EACF0` | `FUN_006EACE0` |
| game-owned `NxScene::setTiming` configurator | `FUN_006EAE30` | `FUN_006EAE20` |
| queue record copy | `FUN_0040BCB0` | `FUN_0040BCE0` |
| bounded-ring enqueue/reserve | `FUN_0040BC20` | `FUN_0040BC50` |
| ordinary worker | `FUN_0040BAC0` | `FUN_0040BAF0` |

The GOG producer waits on the previous-batch completion handle before processing the next scene batch. The queue path tests scene vtable `+0xFC`, which is an actor-count/`getNbActors`-style gate, not `isWritable`.

The live solver capacity is installed synchronously through scene vtable `+0x148` (`NxScene::setTiming`) before the task is queued. The copied queue field historically labelled `task+0x08 maxIter` is not the live timing authority; the ordinary worker reads the scene and elapsed fields and does not reapply that copied value.

The worker performs the native sequence:

```text
elapsed = min(task.elapsed, ~1/15)
NxScene::simulate(elapsed)
NxScene::flushStream()
NxScene::fetchResults(...)
```

## Native fixed-step records

The ordinary live `maxIter` cascade is selected from the gameplay scalar:

```text
< 1.5 -> 1
< 2.5 -> 2
< 3.5 -> 3
else  -> 4
```

The special timing table is now decoded exactly:

```text
record 0      ~= 1/60 s, maxIter 0, fixed method
records 1..19 ~= 1/30 s, maxIter 1, fixed method
```

In the special path, record 0's zero `maxIter` is promoted to an effective `1`. Therefore it is incorrect to describe every secondary scene as unconditionally 1/30; that policy belongs to the special record selection, not the ordinary common path.

PhysX 2.8.1 documents that `NX_TIMESTEP_FIXED` carries fractional remainder/debt across `simulate()` calls and that `maxIter` is a capacity ceiling. This explains the observed 45-FPS starvation with corrected elapsed and `maxIter=1`: a `1/45 s` submission needs an eventual `1,1,2`-style substep pattern, which a one-step cap cannot service without accumulating debt.

What remains unknown is whether repeated `NxScene::setTiming()` calls preserve, reset, or transform the internal accumulated remainder. The supplied SDK material does not expose the desktop rigid-body implementation strongly enough to close this statically.

## Synchronous catch-up path

The catch-up/resync path is separate from the ordinary queue transaction and must not be modified by an ordinary Design-A experiment.

| Role | GOG | Steam |
|---|---|---|
| save timing / set catch-up / simulate | `FUN_0040B820` | `FUN_0040B850` |
| fetch / restore saved timing | `FUN_0040B910` | `FUN_0040B940` |

The path saves current timing, installs approximately `0.05 s` with `maxIter=20`, performs synchronous `simulate -> flush -> fetch`, and restores the previous timing state.

## Runtime evidence and failure modes

The raw-log reanalysis reproduced the earlier report medians instead of trusting the
summary prose. Representative steady-state medians were:

| Case | Queue Hz | live `maxIter` | median substeps | simulate elapsed |
|---|---:|---:|---:|---:|
| QPC 30 FPS | 30.0 | 4 | 2 | ~0.03333 s |
| QPC 45 FPS | 45.0 | 4 | 1 | ~0.02222 s |
| QPC 60 FPS | 60.0 | 4 | 1 | ~0.01667 s |
| QPC 120 FPS | 120.0 | 4 | 0 | ~0.00833 s |
| QPC ~200 FPS | ~189-194.5 | 4 | 0 | ~0.00514-0.00528 s |
| native 120 control | 120.0 | 1 | 1 | ~0.06667 s worker clamp |
| elapsed-only split | 120.0 | 1 | 1 | ~0.01253 s |
| maxIter-only split | ~24.0 | 4 | 4 | ~0.06667 s |

The QPC high-refresh rows show why `maxIter=4` is capacity rather than demanded work:
substep medians fall to zero when elapsed is below the fixed step. The maxIter-only
control shows the opposite failure mode when legacy oversized elapsed remains active.

The runtime matrix independently reproduced these behaviors:

- replacing ordinary Scene-0 elapsed with direct QPC wall seconds normalizes the rigid-body timebase invariant across high refresh rates;
- corrected elapsed with insufficient live capacity starves low/fractional FPS, most visibly around 45 FPS;
- leaving legacy oversized elapsed while raising live capacity to 4 produces repeated four-substep submissions and the observed self-sustaining ~24-FPS collapse;
- Scene-0 QPC plus eligible live `maxIter=1` removes that high-refresh collapse, but exposes weak player-car propulsion because vehicle wheel setters remain a separate timing domain.

Therefore the solver candidate must pair **correct elapsed and bounded live capacity atomically**. Neither half is safe by itself.

## CCT is a separate immediate domain

| Role | GOG | Steam |
|---|---|---|
| common controller wrapper | `FUN_006F9DD0` | `FUN_006F9DC0` |
| root-motion/common CCT path | `FUN_00481D70` | `FUN_00481C60` |

`NxController::move` is immediate and hit callbacks occur when `move` is called. Scene solver normalization cannot normalize CCT cadence automatically.

The older claim that GOG `FUN_004E31E0` has 37 direct callers is contradicted by the current export: the function body exists, but the current calls/xrefs/raw scan finds no direct or data references to that address. Treat its internal fixed downward correction as real but its live reachability/cadence as OPEN until the current indirect/vtable path or replacement is identified.

## Vehicle boundaries

The normal player-car wheel path is:

| Role | GOG | Steam |
|---|---|---|
| normal wheel/PhysX path | `FUN_00555C20` | `FUN_00555B50` |
| direct chassis pose/velocity path | `FUN_005578A0` | `FUN_005577D0` |

`FUN_00555C20` discovers `NX_SHAPE_WHEEL` shapes and writes persistent motor/brake values through wheel vtable slots `+0xD8/+0xDC`. Those values are multiplied by the central gameplay scalar. Steering/tire state is another persistent boundary and must not be globally rescaled with motor/brake.

`FUN_005578A0` / `FUN_005577D0` also performs a direct chassis linear-velocity read/conditional-modify/write through actor vtable `+0xE8/+0xE0`. The write block is skipped when the local condition represented by `[ESP+0x1C] != 0`; otherwise a velocity component is threshold-tested and can be scaled by `0.3` before being written back.

The exact object-phase cadence and ordinary-driving frequency of this direct write remain open. A solver A/B must therefore log its ordering relative to submit/fetch before concluding that solver normalization alone is behaviorally complete.

## Event 6 / physical-prop ordering

The old `FUN_0053E8C0 = Event 6 dispatcher` identity does not match the current Steam/GOG exports and is retired.

Current GOG `FUN_0055F6A0` explicitly handles event ID 6 as a bounded phase state machine: it creates/owns an actor, advances phase, applies one force in phase 2, and releases the actor in phase 5. It is not evidence for a continuous force stream and must not be globally scaled.

Steam scheduler `FUN_006C5FF0` establishes the ordering:

```text
state 14 -> physics submission (`FUN_006EB3C0`)
state 7  -> completion/fetch bridge
state 8  -> eligible object vtable +0x1C receives Event 6
```

Event 6 therefore runs after the physics completion bridge in the scheduler pass, not from an internal PhysX solver substep. Exact object-vtable ordering for the CCT and vehicle direct-RMW functions remains open.

## Xbox comparison

Original Xbox generated PPC logic contains a discrete counter-difference `< 2` wait/yield gate before the later gameplay update and central scalar store. PC and Xbox wheel/CCT logic remain structurally similar, including central-scalar multiplication for motor/brake, but ABI offsets are not byte-identical.

This supports the scheduler/cadence difference as historical context. It does **not** justify a global PC 30-Hz gate: the earlier isolated player-car 30-Hz experiment reached its target cadence and still produced a separate ~22–24-FPS feedback collapse.

## Current solver design status

The strongest future solver direction remains a common ordinary-scene transaction:

```text
previous-batch barrier
-> acquire valid real wall elapsed
-> preserve native ordinary/special record selection
-> install fixed 1/60 timing with bounded live capacity for the same submission
-> enqueue the matching elapsed
-> native worker simulate / flush / fetch
```

This is a research direction, not an approved ZachFix patch. It must exclude zero/reset and synchronous catch-up paths and fail closed on uncertain build/signature/timing state.

Three implementation-blocking facts remain before a precise runtime A/B specification is justified:

1. **Repeated `setTiming` debt semantics:** whether the fixed-step accumulator remainder is preserved or reset.
2. **Discontinuity policy:** how QPC baseline/debt should be reseeded or bounded across pause, loading, alt-tab, debugger gaps, startup, and hitches.
3. **Object-phase ordering:** exact ordering/cadence of live CCT and direct vehicle actor-state writes relative to ordinary submit/fetch.

The next useful work is therefore a set of small runtime characterization probes, not another broad static PhysX census.

## Retired interpretations

Do not repeat these as current facts:

- `gameDelta60` is elapsed seconds;
- `gameDelta60 / 60` is always exact downstream wall time;
- Scene 0 is the only ordinary elapsed boundary;
- every secondary scene is always fixed 1/30;
- queued `task+0x08` controls live `maxIter`;
- scene vtable `+0xFC` is `isWritable`;
- `maxIter=4` is inherently unsafe regardless of elapsed;
- `FUN_004E31E0` has 37 current direct callers;
- `FUN_0053E8C0` is the verified current Event 6 dispatcher;
- all vehicle values should be divided by `gameDelta60`;
- all `NxController::move` or force/torque calls should share one global physics scalar.
