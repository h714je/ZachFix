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

- Native Gamepad feeds SDL3/XInput state into DP's existing action/binding layer instead of replacing input routing.
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
    -> CInput aggregate state
    -> one-deep pending snapshot
    -> commit + held/rising/repeat/previous derivation
    -> public logical getters
    -> Player / camera / UI / vehicle consumers
```

ZachFix Native Gamepad keeps `configJ.cnf` authoritative without synthesizing a
WinMM joystick. SDL3 or XInput produces canonical `GamepadState`; ZachFix evaluates
DP's binding IDs and invokes the same native controller action helpers to rebuild the
active `0x6C` record. Native filtering, CInput aggregation, pending-snapshot commit,
edge/repeat derivation, and all gameplay consumers remain owned by the game.

The production backend default is `Auto`: SDL3 first with XInput fallback. SDL3 is
statically embedded as a guaranteed fallback, while an optional `ZachFix\SDL3.dll`
can override it through SDL's Dynamic API.

This is also why analog vehicle triggers are implemented at the confirmed vehicle
consumers instead of redefining every LT/RT action globally.

The PC main tick natively commits the pending snapshot before polling the next physical
sample, so the freshly polled state normally becomes live on the following tick. The
current research also confirms that the normal shipped lifecycle has no
concurrent background CInput producer; the preserved 33.333 ms callback worker is
dormant. The original Xbox path derives button edges in the same update as
`XamInputGetState`, without the PC pending-snapshot boundary.

See [input.md](input.md) and
[`../research/evidence/cinput_pipeline/README.md`](../research/evidence/cinput_pipeline/README.md).

## Timing precision boundary

DP's two absolute-QPC conversion helpers are a narrow exception to the ambient
x87 precision state. The long-uptime maintenance fix runs only those helpers in
PC53 and restores the caller's precision-control bits on return. Global PC53 is
not used because the mode-2 aim path has an independently confirmed PC24-sensitive
exact-equality boundary. Cesario67's DeadlyPremonitionFix published the QPC/x87
problem before ZachFix independently reproduced it; ZachFix retains its own
build/signature-gated implementation and runtime evidence.

## Player, camera, and vehicle architecture

The Player gameplay state machine and camera are connected: committed Player states
select native camera modes. Vehicle states use their own player-car and vehicle-camera
paths rather than being ordinary on-foot movement with a different model.

Current production features normally preserve the Player state machine. The opt-in
Combat Strafe restoration restores only the missing Xbox ingress into preserved
states `09/0A`; Quick Turn remains research-only.

The optional `Experimental.AimFpuPrecisionFix` is deliberately narrow: it changes x87
precision only while the native mode-2 aim handler executes, then restores the caller
precision state. Forced PC53 causally reproduces the restricted-aim failure, PC24 removes
that forced case, and the scoped guard is known to correct the affected behavior when it
occurs. What remains unresolved is the trigger/root cause that places a normal session
into the precision-sensitive bad state, so the feature remains experimental rather than
being described as a proven root-cause repair.

See [`../research/player/`](../research/player/README.md),
[`../research/input/`](../research/input/README.md), and the mode-2 aim evidence dossier at
[`../research/evidence/aim_mode2_precision/README.md`](../research/evidence/aim_mode2_precision/README.md).

## Save and persistence architecture

Deadly Premonition PC keeps its current gameplay record in a layout that closely
matches the fixed native save image. `dp.sav` is a `0x120`-byte header followed by 28
fixed `0x45CC0` GameRecord images, and the live current record begins at
`Game+0x8C568`.

The persistence research assigns the major domains: inventory/toolbox and
weapon instances, named NPC state, a 4608-entry keyed world-object registry, doors,
removed/dropped items, lights, first-visit state, vehicle availability schedules,
weather/event-core state, checkpoint/autosave/chapter histories, message history,
playtime and chapter times.

The save/resume research also maps the native control flow. Normal save modes synchronize the
live GameRecord before capture, while special mode-3/mode-4 contexts bypass that refresh.
Phone/manual save is initiated while CPlayer is still in state `0x37`, so the serializer
is not hardcoded to Player idle state or phone coordinates. On load, transient Player
state is rebuilt and persistent resume data is overlaid through a normal position anchor,
a scripted `CADA=0xFF` path, or small explicit one-shot resume adapters.

This matters for future save features: selecting or redirecting a complete vanilla save
image is safer than rebuilding gameplay state field by field because the native loader
owns transient reconstruction and subsystem-specific resume policy. A universal
save-anywhere feature still needs runtime safety rules for in-flight vehicle, transition,
and multi-phase object protocols; it is not a shipped feature merely because arbitrary
position persistence is architecturally possible.

See [`../research/save/`](../research/save/README.md), the full
[`GameRecord evidence dossier`](../research/evidence/game_record/README.md), and the
[`save/resume contract evidence`](../research/evidence/save_resume_contract/README.md).

## Audio restoration boundary

The production surround fix does not replace XACT, XAudio2, wave banks, or the
Windows speaker configuration. It restores three narrow boundaries recovered
from the Xbox 360 executable: the X3DAudio emitter/listener input semantics,
the game-side 3D cue apply call, and the non-3D matrix call.

Stereo stays entirely on the original PC behavior. On 5.1/7.1 the fix keeps
the native XACT final mix, converts positional emitters back to the original
mono/handedness model, applies the complete calculated matrix and XACT3D
variables, and reconstructs the original non-3D six-channel routing. The two
PC callsites and the X3DAudio import are verified before the transaction is
committed; signature/import failures leave audio vanilla.

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

Reverse engineering confirmed a real mismatch between the PC gameplay timing scalar and the
ordinary PhysX scene elapsed contract, but the surrounding vehicle, controller, prop,
solver-capacity, and readback paths form separate timing domains.

After multiple runtime experiments, all PhysX/physics-timing hooks were removed from
the production build. ZachFix currently leaves native PhysX scene timing, vehicle
physics cadence, and wheel torque/brake values untouched.

See [`../research/physx/README.md`](../research/physx/README.md).

## Effect-system timing boundary

The current research also separates ordinary effect timing from one original Xbox
fixed-delta family. Most `CRdObjectEffect` parts consume the normal 60-Hz-relative
`gameDelta60`; selected authored effect families instead force a literal `1.0` per
object update. Cross-version reverse engineering confirms that this exception already
exists in the original Xbox 360 executable and is attached to the same numeric effect
types.

The research concern is therefore not that Director's Cut invented the fixed step, but
that PC can execute the surrounding object update at arbitrary cadence while the
literal `1.0` remains per-call. This is currently a research-only high-refresh timing
candidate. ZachFix does not ship a CEffect timing override yet.

See [`../research/engine/effects.md`](../research/engine/effects.md) and
[`../research/evidence/ceffect_xbox_timing/README.md`](../research/evidence/ceffect_xbox_timing/README.md).

## Research status language

The research archive uses conservative status terms:

- **CONFIRMED** - directly supported by raw/static/runtime evidence;
- **STRONGLY_SUPPORTED / HIGH** - evidence is strong but one semantic edge remains;
- **PARTIAL / OPEN** - mechanics or ownership are incomplete;
- **DISPROVEN** - an earlier interpretation was contradicted and is preserved to
  prevent rediscovery.
