# Timing architecture

**Source:** engine map v12 timing, PC/Xbox CEffect comparison, and PhysX reconciliation.

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

## Confirmed timing domains

### Gameplay/update cadence

Object and Player logic can execute at arbitrary render/update cadence in the PC
Director's Cut. This is a major structural difference from the original Xbox 360
path, whose top-level gameplay update is gated by a discrete timer/vblank mechanism.

### CInput staging

The normal main-thread input path commits the previously pending one-slot snapshot and
then polls the next sample. The ordinary synchronous sample is therefore normally
consumed on the following game tick.

A separate ~33.333 ms sampler exists, but normal gameplay input is not proven to be
30 Hz because the main thread also polls each normal game tick.

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

The PC physics worker expects elapsed seconds but receives the 60-Hz-relative scalar
on the native ordinary path. The runtime campaign proved that repairing this boundary
alone is not sufficient for a stable whole-system fix. See `../physx/README.md`.

### Effects / XWP

Inherited `CRdObjectEffect` simulation normally consumes `gameDelta60`, then multiplies by the object's local effect scale before updating runtime parts. Selected effects instead set `+0x1C0 & 2`, forcing the base update to use `delta = 1.0` per object update. Known PC setters include type `0x1E`, types `0x2A..0x2E`, and several `P0MZL`/`W0BUR`/`W1SPL`/`P0HIT` name families.

The original PAL Xbox executable confirms the same contract. `sub_82548180` reads the Xbox gameplay scalar from `0x842AD3B0`, overrides it with `1.0` when `object+0x25C & 2`, and feeds that value into the homologous part-update path. Xbox `sub_82489478` sets the fixed bit for the same numeric types `0x1E` and `0x2A..0x2E`.

This matters because the Xbox gameplay scalar is in 1/60-second units while normal gameplay runs at roughly 30 Hz. Ordinary effects commonly receive about `2` per update; fixed effects receive `1`. On PC, ordinary `gameDelta60` shrinks as update rate rises but the fixed family remains `1.0` per call. Therefore the branch is original design, while its **per-update cadence assumption** is the high-refresh regression candidate.

A conceptual 30-Hz-normalized A/B value is `gameDelta60 * 0.5` for fixed effects only. It is not production-ready until runtime tests cover effect lifetime, emission/animation, collisions/events, one-shot callbacks, low/high FPS, pause/load/reset behavior, and both PC builds.

This is a separate timing island, not evidence that all effects are fixed-per-frame. Type `0x19` contextual fade logic explicitly uses `gameDelta60 * 0.02`, and ordinary effect simulation is already delta-aware. Any experiment must target the fixed-delta families only. See `effects.md` and `../evidence/ceffect_xbox_timing/README.md`.

### CCT and props

Character-controller moves, GroundSnap-style immediate moves, persistent wheel
properties, direct actor forces, and post-physics readback have different persistence
and cadence semantics. A global "physics delta" would conflate these domains.

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
