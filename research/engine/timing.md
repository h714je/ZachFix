# Timing architecture

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Timing is composed of distinct islands. Do not generalize one delta or clock to every subsystem.

**Jump to:** [Central PC gameplay scalar](#central-pc-gameplay-scalar) · [Long-system-uptime x87 precision failure](#long-system-uptime-x87-precision-failure) · [Confirmed timing domains](#confirmed-timing-domains) · [Engineering rule](#engineering-rule) · [2026-10-04 outer-loop and PhysX-container addendum](#2026-10-04-outer-loop-and-physx-container-addendum) · [MegaRE final timing addendum (2026-10-08)](#megare-final-timing-addendum-2026-10-08)
<!-- END AUTO RESEARCH NAV -->

**Scope:** PC/Xbox CInput and CEffect comparisons plus PhysX timing research; 2026-10-04 Mega RE Census context/lifecycle addendum.

Deadly Premonition PC does not have one universal time domain. The same central
60-Hz-relative gameplay scalar appears in several subsystems, but its correct meaning
depends on the consumer.

## Central PC gameplay scalar

The main PC timer produces approximately:

```text
gameDelta60 = realFrameSeconds * 60
```

Examples:

```text
30 FPS  -> ~2.0
60 FPS  -> ~1.0
120 FPS -> ~0.5
200 FPS -> ~0.3
```

This is a 60-Hz-relative gameplay scalar, not seconds.

## Long-system-uptime x87 precision failure

Two native helpers convert an **absolute** QPC value before timestamp subtraction:
Steam `FUN_00401F50` / `FUN_00701040`, with GOG using the paired
`FUN_00401F50` / `FUN_00700FA0`. DP creates its D3D9 device without
`D3DCREATE_FPU_PRESERVE`, so normal gameplay can remain in x87 PC24. At long
Windows uptime the absolute-seconds value becomes coarse enough to quantize the
main timer even though QPC/QPF themselves remain correct.

Cesario67's `DeadlyPremonitionFix` discovered and published this QPC/x87 issue
before the ZachFix investigation. ZachFix independently reproduced the same
mechanism on 2026-10-06 at roughly 91.7 hours uptime, where the characteristic
31.25 ms PC24 quantum produced raw ~1.875-tick samples that DP's normal mode-0
transform rounded to 2.0, yielding an apparent ~64 final ticks/s from a ~60 Hz
wall cadence.

Production repair is local: only those two QPC conversion helpers execute in
scoped PC53, and only the caller's precision-control bits are restored on exit.
Global PC53 is deliberately avoided because the mode-2 aim path has a separate
PC24-sensitive exact-equality boundary. See
`../evidence/game_time_precision/README.md`.

## Confirmed timing domains

### Gameplay/update cadence

Object and Player logic can execute at arbitrary render/update cadence in the PC
Director's Cut. This is a major structural difference from the original Xbox 360
path, whose top-level gameplay update is gated by a discrete timer/vblank mechanism.

### CInput staging

The PC CInput boundary is closed. The normal main-thread path commits the previously
pending one-slot snapshot and then polls/evaluates the next physical sample. The
ordinary synchronous sample is therefore consumed on the following game tick.

The producer uses one pending `7 * 0x40` snapshot plus a separate `7 * 0x4C` aggregate
area that continues collecting selected activity while the pending slot is occupied.
Commit preserves the old held mask, consumes the pending snapshot, then derives
`held`, `rising`, `rising|repeat`, and `previous` masks.

Runtime census confirmed that normal shipped gameplay uses only the main-thread
producer. The static ~33.333 ms callback worker and `CInput+0xBC0` async-handoff API
exist, but are dormant in the normal lifecycle: both tested steady-state modes recorded
zero background producer calls, and the handoff setter/query have no shipped xrefs in
either PC build.

A same-frame repair is therefore specifically `poll -> commit`, retaining exactly one
native commit per tick. Adding a second commit is not safe because a second
`previous = current` step can erase a newly generated rising edge.

The original Xbox path does not use the PC staging boundary. Xbox `sub_82523238` calls
`XamInputGetState` and derives current/rising/repeat state in the same update. This
makes the PC one-tick staging a port-side architecture difference, not an inherited
Xbox timing requirement. See `../evidence/cinput_pipeline/README.md`.

### Camera

Camera timing is mode-specific. Mode 0 re-anchors its target before the common look
pass; modes 10/11 are incremental X/Y candidates; mode 9 vehicle camera has its own
signed-deadzone/target semantics. Do not apply one global right-stick time rule.

### Vehicle

The live state-87 steering target producer slews `car+0x4E0` with `gameDelta60` and is
already delta-aware. The older fixed-degree recurrence belongs to the alternate
`0x20000` car branch, not the normal `0x8000` player-car path.

Motor/brake wheel setters form a separate persistent-value domain and were central to
the retired PhysX investigation.

### PhysX

The PC ordinary scene path passes the 60-Hz-relative gameplay scalar to PhysX where
elapsed seconds are expected. The 2026-10-01 closure pass confirms that this is a
common active-scene boundary, not a Scene-0-only defect, and that the game installs
`NxScene::setTiming` synchronously before enqueueing the matching elapsed value.

Live `maxIter` is solver capacity. The queue copy historically labelled as carrying
`maxIter` is not the live authority. PhysX 2.8.1 fixed-step semantics carry fractional
remainder/debt across `simulate()` calls, which explains why corrected elapsed with
`maxIter=1` starves around 45 FPS while legacy oversized elapsed paired with elevated
capacity can produce the ~24-FPS feedback collapse.

The native special timing table is also now exact: Scene 0 uses approximately
`1/60,maxIter1` and secondary records use approximately `1/30,maxIter1` in special
mode. A separate synchronous catch-up path temporarily installs approximately
`0.05,maxIter20`, simulates/fetches, and restores timing; it is not part of the
ordinary candidate transaction.

The strongest future solver direction is therefore an atomic pairing of real wall
elapsed with bounded live capacity at the common ordinary boundary while preserving
special/zero/catch-up behavior. It is still research-only because repeated-`setTiming`
debt semantics, discontinuity policy, and object-phase ordering remain open. See
`../physx/README.md` and `../evidence/physx_timing/README.md`.

### Effects / XWP

Inherited `CRdObjectEffect` simulation normally consumes `gameDelta60`, then multiplies by the object's local effect scale before updating runtime parts. Selected effects instead set `+0x1C0 & 2`, forcing the base update to use `delta = 1.0` per object update. Known PC setters include type `0x1E`, types `0x2A..0x2E`, and several `P0MZL`/`W0BUR`/`W1SPL`/`P0HIT` name families.

The original PAL Xbox executable confirms the same contract. `sub_82548180` reads the Xbox gameplay scalar from `0x842AD3B0`, overrides it with `1.0` when `object+0x25C & 2`, and feeds that value into the homologous part-update path. Xbox `sub_82489478` sets the fixed bit for the same numeric types `0x1E` and `0x2A..0x2E`.

This matters because the Xbox gameplay scalar is in 1/60-second units while normal gameplay runs at roughly 30 Hz. Ordinary effects commonly receive about `2` per update; fixed effects receive `1`. On PC, ordinary `gameDelta60` shrinks as update rate rises but the fixed family remains `1.0` per call. Therefore the branch is original design, while its **per-update cadence assumption** is the high-refresh regression candidate.

A conceptual 30-Hz-normalized A/B value is `gameDelta60 * 0.5` for fixed effects only. It is not production-ready until runtime tests cover effect lifetime, emission/animation, collisions/events, one-shot callbacks, low/high FPS, pause/load/reset behavior, and both PC builds.

This is a separate timing island, not evidence that all effects are fixed-per-frame. Type `0x19` contextual fade logic explicitly uses `gameDelta60 * 0.02`, and ordinary effect simulation is already delta-aware. Any experiment must target the fixed-delta families only. See `effects.md` and `../evidence/ceffect_xbox_timing/README.md`.

### CCT and props

Character-controller moves, persistent wheel properties, direct actor writes/forces,
and post-physics object events have different persistence and cadence semantics. A
global "physics delta" would conflate these domains.

`NxController::move` is immediate and its hit callbacks follow caller invocation, so
scene-solver normalization does not normalize CCT cadence. The older claim that GOG
`FUN_004E31E0` has 37 current direct callers is contradicted by the current export;
the body exists, but its live reachability/cadence is still open.

Steam scheduler ordering is now established as physics submission in state 14, the
completion/fetch bridge in state 7, then object Event 6 in state 8. Event 6 therefore
does not run from a PhysX substep. One current GOG Event-6 prop handler is a bounded
phase state machine with a single force application, not a continuous force stream.
Exact CCT and vehicle direct-state-write ordering inside the object phases remains a
runtime-characterization target.

## Engineering rule

When changing timing behavior, classify the consumer first:

```text
continuous persistent property
per-call displacement
one-shot force/impulse
time accumulator
fixed-step scene elapsed
render interpolation/readback
```

A correct scale for one category can be wrong for another.

## 2026-10-04 outer-loop and PhysX-container addendum

The Mega RE Census independently closes the outer static application ordering in both PC
builds: Win32 message handling -> eligible idle gate -> timing update -> `FUN_00401A70`.
This improves placement of the already-known scheduler root but does not prove temporal
behavior in every loading/menu/movie state.

Steam also exposes four typed `CPhysicsThread` contexts at `00BDA010`, each with a
`CNArray<CPhysicsThread::SCENE>` record vector. A selected producer and the worker consume
the same physical vector. Record `+0x04` is a worker-selected float and `+0x0C` is a
pointer-slot ordinal; `+0x10/+0x14` are context-specific values and must not be revived as
universal timing fields. The raw `0.0666666701` comparison constant is likewise not proof
of a live cadence or unit.

An available helper can start those contexts through the generic OS-thread entry, but the
Census found no concrete incoming call to that helper in the bounded scan. Destruction
also does not prove worker join/quiescence before vector free. These are lifecycle and
activation unknowns around the timing island, not a reason to discard the 2026-10-01
paired-build scene-timing results.

## MegaRE final timing addendum (2026-10-08)

MegaRE confirms that Deadly Premonition has multiple timing islands rather than one universal delta. The QPC-derived central scalar is 60-Hz-relative, while input filtering, PhysX debt/timing, vehicle steering, wheel state, CCT displacement, effect families, animation packet reuse, audio recurrence and presentation each have different persistence/cadence contracts.

The corrected selected scheduler order is phase 14 -> physics producer/first half -> phase 7 second half/fetch-shaped continuation -> phase 8 -> event 6. A physics-facing path can clamp a seconds-style value near `0.06666667` (`1/15`), but the static evidence does not establish a universal “60x PhysX” bug.

A targeted bridge found a loading worker that can loop around stage/render/presentation requests with `Sleep(16)`. That wait is loading-worker behavior, not a hidden ordinary-gameplay frame limiter. A nearby `1/30` threshold does not gate the rendering/presentation request path.

See the imported [`FLOW_FRAME_SCHEDULER.md`](../evidence/mega_re_final_2026-10-08/maps/FLOW_FRAME_SCHEDULER.md), [`FLOW_DISPLAY_RESOLUTION_TIMING.md`](../evidence/mega_re_final_2026-10-08/maps/FLOW_DISPLAY_RESOLUTION_TIMING.md) and [`FLOW_PHYSICS_AND_FRAME_DEPENDENCE.md`](../evidence/mega_re_final_2026-10-08/maps/FLOW_PHYSICS_AND_FRAME_DEPENDENCE.md).
