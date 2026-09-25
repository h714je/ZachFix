# Technical architecture notes

This document explains the native Deadly Premonition systems that current ZachFix
features intentionally preserve or extend. It is product-facing: detailed addresses,
state tables, abandoned experiments, and open reverse-engineering questions live in
[`../research/`](../research/README.md).

## Design rule: patch the narrowest proven boundary

ZachFix generally avoids replacing whole engine subsystems. Production fixes are
scoped to the smallest confirmed boundary that explains the problem and are gated by
supported executable profiles plus local instruction/signature checks.

Examples:

- Native XInput bridges into DP's existing action/binding layer instead of replacing
  input routing.
- World-distance controls modify separate native distance mechanisms rather than one
  synthetic global draw-distance value.
- The interior visibility fix bypasses one confirmed bad visibility-volume caller
  while retaining ordinary frustum culling.
- The day/night repair restores the native `HOUSE_LIST.NOD` data contract before the
  original level consumer uses it.
- Experimental PhysX timing work was removed because no narrow patch produced a
  stable whole-system timing contract.

## Input architecture

The native PC path is approximately:

```text
physical keyboard/mouse or WinMM controller
    -> configurable input evaluation
    -> logical action records
    -> staged CInput snapshot
    -> held/rising/repeat derivation
    -> public logical getters
    -> Player / camera / UI / vehicle consumers
```

ZachFix Native XInput feeds a synthetic controller state into this existing layer and
translates the old WinMM axis meanings so `configJ.cnf` remains authoritative.

This is also why analog vehicle triggers are implemented at the confirmed vehicle
consumers instead of redefining every LT/RT action globally.

See [input.md](input.md).

## Player, camera, and vehicle architecture

The Player gameplay state machine and camera are connected: committed Player states
select native camera modes. Vehicle states use their own player-car and vehicle-camera
paths rather than being ordinary on-foot movement with a different model.

Current production features normally preserve the Player state machine. The opt-in
Combat Strafe restoration restores only the missing Xbox ingress into preserved
states `09/0A`; Quick Turn remains research-only.

See [`../research/player/`](../research/player/README.md) and
[`../research/input/`](../research/input/README.md).

## World-distance architecture

Deadly Premonition has several independent distance systems. Current ZachFix controls
map to five different native mechanisms:

| Setting | Native mechanism |
| --- | --- |
| `HighDetailDistanceScale` | high-detail streaming-cell selection |
| `MainFrustumDistanceMode` | selected main-camera frustum far planes |
| `ObjectActivationDistanceScale` | native object active-list threshold |
| `ObjectLODDistanceScale` | native mesh-LOD distance metric |
| `Alternate3DDistanceScale` | alternate low-detail 3D residency range |

Directional shadow relevance is another independent 1000/3000-unit three-frustum
path and is not extended by `MainFrustumDistanceMode`.

See [rendering.md](rendering.md) and
[`../research/world/README.md`](../research/world/README.md).

## Rendering/restoration boundaries

Production rendering/restoration code spans several distinct layers:

```text
resource preprocessing     HOUSE_LIST day/night repair
world visibility           interior volume + distance controls
D3D9 resource creation     resolution / shadow / reflection / depth formats
native shader constants    selected compatibility/restoration paths
ZachFix PostFX             AO / Bloom / DoF / exposure
final output transfer      optional Xbox HDTV/BT.709 mode
```

A visual problem is not assumed to belong to the shader or PostFX layer simply because
it appears on screen. Research first identifies which layer owns the state.

## Physics boundary

The engine map confirmed a real mismatch between the PC gameplay timing scalar and the
ordinary PhysX scene elapsed contract, but the surrounding vehicle, controller, prop,
solver-capacity, and readback paths form separate timing domains.

After multiple runtime experiments, all PhysX/physics-timing hooks were removed from
the production build. ZachFix currently leaves native PhysX scene timing, vehicle
physics cadence, and wheel torque/brake values untouched.

See [`../research/physx/README.md`](../research/physx/README.md).

## Research status language

The research archive uses conservative status terms:

- **CONFIRMED** - directly supported by raw/static/runtime evidence;
- **STRONGLY_SUPPORTED / HIGH** - evidence is strong but one semantic edge remains;
- **PARTIAL / OPEN** - mechanics or ownership are incomplete;
- **DISPROVEN** - an earlier interpretation was contradicted and is preserved to
  prevent rediscovery.
