# Deadly Premonition PC PhysX research — retired production work

**Status:** research retained, runtime patch retired
**Last updated:** 2026-09-26
**Production policy:** ZachFix does not modify PhysX, rigid-body timing, vehicle-physics cadence, or physics solver settings.

This file is the compact final record of the PhysX/physics-timing investigation. The experimental runtime hooks, A/B switches, probes, scheduler candidates, and validation-source copies that accumulated during the investigation have been removed from the production tree. The conclusions below are kept so the same dead ends do not need to be rediscovered.

## 1. What was confirmed

### PC scene-time contract is dimensionally wrong

The PC port's central gameplay scalar is approximately:

```text
gameDelta60 = realFrameSeconds * 60
```

It is a 60 Hz-relative gameplay scalar, not seconds. The ordinary PC physics path nevertheless forwards that value to the asynchronous PhysX scene worker as elapsed time. The worker clamps it to about `1/15 s` and calls `NxScene::simulate(elapsed)`.

For Scene 0 the normal fixed timing is approximately:

```text
maxTimestep = 1/60 s
method      = NX_TIMESTEP_FIXED
```

The live native `maxIter` is selected from the gameplay scalar:

```text
< 1.5 -> 1
< 2.5 -> 2
< 3.5 -> 3
else  -> 4
```

The queued task's copied `maxIter` field is not the live controlling value. The controlling value is the synchronous `NxScene::setTiming()` state installed before the task is queued.

### The common scene dispatcher is broader than Scene 0

The common dispatcher fans the same elapsed scalar into Scene 0 and active secondary scenes. Secondary scenes can use a native fixed `1/30 s`, `maxIter=1` policy in one mode. A separate catch-up/resync path temporarily uses a much larger fixed-step capacity and must not be conflated with ordinary simulation.

### Scene-0 QPC elapsed fixes the measured rigid-body timebase

Replacing the ordinary Scene-0 elapsed with real QPC wall seconds made the independent vehicle yaw invariant converge near one:

```text
abs(poseYawRate) / abs(NxActor.angularVelocity.y) ~= 1
```

This was reproduced across multiple caps in earlier experiments and was again visible in the final v6-A run at roughly 162–163 FPS.

### High-refresh collapse is tied to live solver-capacity escalation

At high refresh, a transient hitch can raise native `gameDelta60`, which raises live Scene-0 `maxIter`. In repeated experiments this produced a self-sustaining performance collapse around 22–24 FPS:

```text
hitch
  -> gameDelta60 ~= 2..3
  -> live maxIter = 2..3
  -> more solver work per submission
  -> slower frame
  -> gameDelta60 stays high
  -> maxIter stays high
```

The decisive v6-A experiment forced eligible Scene-0 live `maxIter=1` while keeping QPC elapsed. The 163 FPS collapse disappeared. This strongly isolates Scene-0 capacity escalation as one trigger for the high-refresh feedback loop.

### Vehicle motor/brake setters are a separate timing domain

The normal player-car path writes `NxWheelShape::setMotorTorque` and `setBrakeTorque` values pre-multiplied by `gameDelta60`. These setters are persistent continuous values, not one-frame impulses. Once Scene 0 is normalized to approximately 60 real fixed substeps per second, the retained per-render `gameDelta60` multiplier makes the continuous drive/brake magnitude FPS-dependent.

Observed consequence in the stable v6-A experiment:

```text
~163 FPS
solver clock corrected
Scene-0 maxIter = 1
FPS stable
vehicle throttle becomes extremely weak
```

That symptom is consistent with the player-car setter receiving only roughly `60 / renderHz` of the 60-FPS baseline continuous torque.

### Xbox and PC both multiply vehicle torque/brake by their central timing scalar

Static Xbox/PC comparison showed that the multiply itself is inherited behavior. The important historical context is scheduling: the original Xbox gameplay path runs behind a discrete roughly 30 Hz tick/vblank gate, whereas the PC Director's Cut can execute retained gameplay/object logic at arbitrary render cadence.

This means simply declaring the scalar multiply a "PC-only bug" is incorrect.

## 2. What was tried and what happened

### Elapsed-only / QPC Scene-0 experiments

These corrected the measured rigid-body timebase at high FPS, but they exposed the separate live-`maxIter` and vehicle-drive problems. At low FPS, `maxIter=1` also lacks enough fixed-step capacity to maintain a true 60 Hz solver.

### Common-boundary / all-scene elapsed correction

Correcting fixed-1/60 and fixed-1/30 ordinary submissions closed the known elapsed-unit holes. Runtime telemetry could reach `nativeBoundaryHz=0`, yet requested high refresh still collapsed to roughly 24 FPS. Therefore "an uncorrected secondary elapsed boundary" was not the sole cause.

### Fixed-60 dispatcher scheduler

A whole-tick scheduler was tested to avoid frequent sub-timestep submissions above 60 FPS. The experiment did not eliminate the collapse. One early version also bypassed too much of the native common dispatcher on skipped frames, making it unsuitable as a production architecture even before the runtime result.

### v6-A: Scene-0 QPC + `maxIter=1`

This was the cleanest successful high-refresh isolation:

```text
render ~= 162–163 FPS
Scene-0 QPC elapsed ~= 0.00616 s
live maxIter = 1
no 24-FPS collapse
VehicleTurn timebase ratio ~= 1
```

But the vehicle throttle became extremely weak, proving that solver-time repair alone is not a whole-system repair.

### v7: Xbox-like 30 Hz player-car cadence

The active player-car phase was gated to roughly 30 Hz while skipped `gameDelta60` values were accumulated. Initially the telemetry looked exactly as intended:

```text
inputHz ~= 163
logicHz ~= 30
passedDelta60 ~= 2.0
Scene-0 maxIter = 1
```

After several seconds a heavier vehicle update produced a hitch and the game fell back to roughly 22–24 FPS even though Scene-0 `maxIter` remained `1`. The cadence reconstruction therefore introduced a second independent feedback/performance failure and was rejected.

It also highlighted an important semantic issue: setting approximately `2x` persistent motor/brake torque at 30 Hz while the corrected PC Scene 0 still solves at 60 Hz is not automatically equivalent to the original Xbox whole-system contract.

### v8: player motor/brake normalization

A narrower candidate removed the per-frame `gameDelta60` factor only at the normal-player `NxWheelShape` motor/brake boundary while retaining native vehicle cadence. This was designed to restore the 60-FPS continuous-force baseline without gating the whole vehicle dispatcher.

The project was retired before accepting this as a production fix. Even if this local correction improves throttle/brake magnitude, the overall vehicle and physics stack still contains other timing domains: vehicle readback/corrections, alternate car branches, CCT helpers, props, low-FPS solver capacity, and lifecycle transitions. Shipping another local fix would continue the same whack-a-mole pattern.

## 3. Other confirmed timing-sensitive boundaries

The investigation also mapped several non-solver assumptions:

- the established live `car+0x434 & 0x8000` player steering target is produced in Player state `87` with a `gameDelta60`-scaled slew and is therefore already delta-aware; the older fixed-degree `FUN_00551640` recurrence belongs to the alternate `0x20000` car branch and must not be generalized to the live player path;
- normal player motor/brake values are persistent `NxWheelShape` properties and are scaled by the central gameplay scalar;
- visual wheel rotation, odometer accumulation, and ordinary player CCT displacement are delta-integrated and should not have their timing scalar blindly removed;
- a shared GroundSnap/CCT helper uses a fixed downward displacement per invocation and has many callers;
- physical props, controllers, wheel shapes, scene simulation, and game-side vehicle state form separate timing domains and cannot be safely repaired with one global multiplier.

## 4. Why no production physics fix is shipped

The investigation established several real defects, but not a stable replacement contract for the whole PC engine/PhysX boundary.

Every apparently local repair changed assumptions made by another layer:

```text
fix Scene elapsed
  -> expose live maxIter feedback

clamp maxIter
  -> expose weak persistent vehicle torque/brake

reconstruct Xbox vehicle cadence
  -> introduce a new 24-FPS feedback path

normalize drive setters
  -> still leave other vehicle/CCT/prop timing domains
```

The original Xbox was a coupled system: gameplay cadence, vehicle setters, solver cadence, and fixed-step capacity evolved together. Reconstructing one piece in isolation on the PC port is not enough.

The production decision is therefore conservative:

> **Do not patch PhysX or physics timing in ZachFix until a single end-to-end timing contract can be demonstrated across solver, vehicles, CCTs, props, low/high FPS, hitches, pause/load transitions, and both supported PC builds without performance collapse or gameplay drift.**

> **Maintainer's note:** Screw this. Writing **OpenDP** from scratch would literally cost less sanity than trying to surgically extract Deadly Premonition's physics from its FPS counter. Every local patch just uncovers another layer of hardcoded timing hacks. The foundation is rotten. **BURN THIS TRASH TO ASH AND REBUILD IT PROPERLY**

## 5. Do not repeat these dead ends

- Do not pass `gameDelta60` directly as PhysX seconds.
- Do not treat queued task `maxIter` as the live solver-capacity control.
- Do not raise live Scene-0 `maxIter` broadly at high refresh.
- Do not assume secondary-scene elapsed correction alone solves the 24-FPS collapse.
- Do not gate only the player-car dispatcher to 30 Hz and call that an Xbox reconstruction.
- Do not globally divide all vehicle values by `gameDelta60`; several game-side integrations legitimately use it.
- Do not combine solver, vehicle, CCT, and prop timing into one global "physics delta" patch.

## 6. Production cleanup

The following runtime experiments have been removed from the current production tree:

- fixed-60 gameplay/tick scheduler;
- Scene-0/common PhysX QPC elapsed hooks;
- live timing/maxIter mutation;
- solver and VehicleTurn telemetry;
- Xbox-like 30 Hz player-car cadence hook;
- player motor/brake normalization hook;
- associated `[Physics]` INI switches;
- research-only physics build-profile RVAs;
- archived compilable probe/experiment sources.

The remaining ZachFix code does not intentionally change Deadly Premonition's PhysX scene timing, physics solver settings, vehicle-physics cadence, or physics torque/brake values.
