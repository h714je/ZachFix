# Engine architecture overview

**Research snapshot:** 2026-09-30.

This is the compact architecture view used by the rest of the research archive.
Addresses below are GOG 1.01b unless a Steam counterpart is stated.

## Master execution spine

```text
Win32 message loop / idle path
    -> native PC timing
    -> FUN_00401A70 top-level frame/gameplay scheduler
    -> FUN_006C5AF0 (GOG) / FUN_006C5FF0 (Steam) generic object dispatcher
    -> depth-ordered active-object construction
    -> multi-pass virtual object framework
    -> common Actor phases
    -> CPlayer event callback
    -> Event 1
    -> CPlayer+0x654 state
    -> 0x008A9758[state]
    -> state-specific gameplay handler
    -> Player post-state / CCT reconciliation
       GOG   FUN_004E3350
       Steam FUN_004E3280
```

The scheduler -> Player-state spine and the Event-1 -> post-state/CCT link are
confirmed.

## Native save / GameRecord persistence spine

The current PC save architecture is now mapped at the subsystem level:

```text
fixed dp.sav image, 0x7A2620
    -> 0x120-byte header
    -> 28 x 0x45CC0 GameRecord images

live current GameRecord
    == Game + 0x8C568
    -> event/player/item state
    -> inventory/toolbox/weapon state
    -> named NPC + NPC world state
    -> generic world-object persistence
    -> specialized door/item/light/vehicle registries
    -> weather / CEvCore / checkpoint-history state
    -> playtime/message/chapter/weapon-instance tail
```

The large `record+0x11700..+0x3E6FF` block is a 4608-entry keyed world-object registry,
not opaque padding. Tail tables beyond it are now assigned to doors, removed world
items, persistent placement overrides, dropped items, lights, first-visit keys and
vehicle availability schedules.

CEvent persistence is also linked back to behavior: three `(eventId,routineId)`
history sets deduplicate checkpoint capture (`0xC6`), autosave/checkpoint transitions
(`0xC2/0`) and chapter-history capture (`0xCD`). Resume identity is retained at
`record+0x427EC/+0x427EE`.

Vehicle availability uses two deliberate resource-index namespaces. Generic type-`0x48`
vehicles use 128 windows at `record+0x40E00`; thirteen maps use dedicated 32-entry
banks at `record+0x41000`. CEvent `0xAC` writes the windows and `CCar` reads them through
the same `FUN_00453A70` router.

The save/resume pass closes the next control-flow layer above that layout. Native callers establish a
resume contract and then converge on the shared persistent synchronization pipeline.
Normal modes `0/1/2/5` refresh the live GameRecord before capture; modes `3/4` are
special top-level/load contexts that bypass the normal refresh cluster. Phone/manual
save is launched while CPlayer is still in state `0x37`, so Player state 0 is not a
native serializer prerequisite.

Load is reconstruction rather than a live-object dump. Normal resume uses the persisted
map/location plus York transform at `record+0xCAD8..+0xCB00`; a consumed scripted-resume
request writes `record+0xCADA = 0xFF`; explicit one-shot adapters can select alternate
reconstruction, including `playerStateMaskHi bit 0x4 -> CPlayer state 0x40` and the
manual-save CEvent `0x1F5` scratch at `record+0x43818/+0x4381C`.

The corrective save-anywhere pass closes the default Player-state side as well: in the
reviewed Steam reconstruction path, high-mask bit `4` selects state `0x40`, while the
corresponding default Player-init branch selects state `0x00`. The prior hypothesis that
a save made in transient state `0x38` reloads directly into `0x38` and dereferences a
stale serialized action pointer is therefore disproven. Pointer-shaped bytes may still
be present in `record+0x14`; exhaustive post-load consumption remains an open census.

For future ZachFix multi-save or save-anywhere work, the architecture therefore favors
reusing the native synchronization/snapshot/load pipeline and retaining resume tokens,
rather than synthesizing gameplay state field by field. A state-`0x00` whitelist is a
conservative mod policy for a first general-purpose path, not a native serializer
invariant. The remaining risk is semantic safety of unproven in-flight protocols, not
hardcoded save-point coordinates.

See `../save/README.md`,
`../evidence/game_record/ZachFix_GameRecord_RE_2026-09-30.md`, and
`../evidence/save_resume_contract/README.md`.

## Physics island inside the object dispatcher

Stable global phase anchors:

```text
State 14 -> PhysX dispatch/simulation boundary
State 7  -> completion/fetch/cleanup bridge
State 8  -> object virtual +0x1C
State 11 -> PhysicsResist processing
```

The asynchronous worker performs:

```text
simulate_dt = min(task.elapsed, 1/15)
simulate -> flushStream -> fetchResults
```

The architecture is retained for reverse-engineering reference, but production
physics-timing patching is retired. See `../physx/README.md`.

## Player / CCT split

A major correction is established:

- `FUN_0048B0A0 -> FUN_004831C0 -> FUN_00481D70` belongs to an NPC/common-character
  inheritance path, not York's missing bridge.
- `FUN_0048B0A0` is an override in the `CNpcDog` virtual table.
- York has a separate Player-specific post-state CCT path in `FUN_004E3350` /
  Steam `FUN_004E3280`.

## Player vehicle handoff

```text
vehicle-entry setup
    -> CPlayer state 0x87
    -> York attached to selected car marker PM_SEAT_ML
    -> selected car has marker M_CENTER
    -> car+0x434 |= 0x00008000
    -> object-scheduler vehicle phases
       high-level 0x005588F0
       wheel/readback 0x005578A0
       PhysX-facing 0x00555C20
```

State `0x87` is the active player-car state. State `0x88` is an exit prelude that
returns to state `0x38`; state `0x38` is the entry/exit animation hub and can commit
back to `0x87` or continue cleanup toward `0x00`.

## Input spine

```text
Win32 keyboard/mouse + WinMM joystick
    -> physical acquisition
    -> 0x6C logical action records
    -> PC filtering
    -> 7 x 0x4C CInput aggregate state
    -> one-deep 7 x 0x40 pending snapshot
    -> commit + held/rising/repeat/previous derivation
    -> public CInput getters
    -> Player / camera / UI / vehicle consumers
```

ZachFix Native Gamepad now enters at the controller-action boundary: SDL3/XInput
produces canonical `GamepadState`, DP's existing binding IDs/action helpers rebuild the
active `0x6C` controller record, and all downstream CInput staging/edge/repeat logic
remains native. The earlier synthetic WinMM/JOYINFOEX bridge has been retired from the
native path.

The normal PC main tick is `commit -> poll`, so a newly polled sample normally waits one
tick in the pending slot. Runtime census closed the old concurrency concern: the
normal shipped lifecycle produced no background CInput producer calls, while the
33.333 ms callback worker and `+0xBC0` async-handoff API remained dormant. A single
`poll -> commit` reorder preserves native edge/repeat semantics and removes the staging
tick; an extra commit does not. The original Xbox input update derives button edges in
the same `XamInputGetState` pass and does not expose the PC pending-snapshot boundary.

## Camera architecture

`0x008A9980[state]` maps committed CPlayer state to `CCamera+0x154` mode. The normal
camera dispatcher uses `0x008A9BC8[mode]`.

Confirmed semantic anchors:

```text
mode 2  -> aim/combat
mode 9  -> player vehicle camera (states 87/88)
mode 10/11 -> context-target acquisition/handoff
mode 16 -> Telescope state 65
```

The ordinary mode-0 free-look path re-anchors its target before the common look pass;
the local `lookX * 2 degrees` instruction alone does not prove FPS-scaled angular
velocity.

Mode `2` has now been mapped substantially deeper. PC live aim uses a bounded
reticle/target accumulator and transfers motion into actual camera yaw/pitch when the
accumulator reaches its edge. The PC x87 implementation compares a spilled float32
accumulator against a retained x87 limit using exact equality. Forcing x87 PC=53
reproduces the historical "reticle reaches edge but camera stops following" failure;
PC=24 restores the handoff locally. This is a confirmed precision-sensitive mechanism and the only locally reproducible
look-alike found for the reported bug. The original spontaneous bug itself remains
unreproduced, so the scoped PC24 guard is kept as an experimental best-effort workaround.

A separate cross-version divergence exists in the camera-mode setter. Xbox performs
fresh mode-2 anchor/transient initialization on every explicit SetCameraMode(2); PC
skips two initialization gates when the previous camera mode is already 2. Ordinary
stock weapon ingress is normally mode0 -> mode2 and therefore safe, while restored
Xbox combat-strafe states 09/0A can intentionally exercise a mode2 -> mode2 re-entry.
The previously suspected CEvent A6/4 producer was content-censused across all shipped
DSB scripts and occurs only in an internal developer "Shooting Test" event, so it is
not a normal-gameplay root cause.

See `../input/camera-modes.md` and
`../evidence/aim_mode2_precision/README.md`.

## Effect / XWP subsystem

The effect stack is now separated into resource/admin, game-side CEffect policy, and inherited base simulation/rendering:

```text
CEffectAdmin
  -> 195-entry type catalog + XWP resource resolution
  -> 500-slot live CEffect registry
  -> CEffect creation
       -> CEffect callback / behavior state
       -> CRdObjectEffect XWP runtime parts
            -> simulation + render packet + spatial ownership
```

`CEffect` is a thin derived layer over `CRdObjectEffect`: vtable slots 1..18 are inherited unchanged. Name-derived `CEffect+0x31C` is a gameplay behavior class, not merely a renderer tag. Correct mapping is `S* -> 1`, `F1FIR005 -> 2`, other `F* -> 3`.

Effects also contain an explicit timing exception: ordinary runtime parts use `gameDelta60`, while selected `+0x1C0 & 2` families force `delta = 1.0` per update. The original PAL Xbox executable contains the same branch (`+0x25C & 2`) and the same numeric fixed-effect types, proving that the fixed step is inherited design rather than a PC invention. Its native wall-time behavior, however, depended on the Xbox ~30 Hz gameplay cadence; arbitrary PC update cadence makes this a narrow high-refresh regression candidate. Do not globally rescale CEffect.

The recovered Xbox effect virtual shape has 17 slots versus 19 on PC. The simulation core maps across versions; the two extra PC tail virtuals align with render-packet/spatial integration, separating inherited effect simulation from later PC/DC renderer/world plumbing.

See `effects.md` and `../evidence/ceffect_xbox_timing/README.md`.

## World-distance architecture

There is no single draw-distance scalar. At minimum the PC engine separates:

1. high-detail streaming-cell selection;
2. six main-frustum visibility classes;
3. object active-list distance;
4. native per-resource mesh LOD;
5. alternate low-detail 3D residency packages;
6. directional three-cascade shadow visibility;
7. a separate 500-unit character secondary-update gate.

See `../world/README.md`.

## Rendering/restoration domains

The renderer research separates native resource/scene problems from ZachFix
PostFX work. Production-closed findings include the `HOUSE_LIST.NOD` day/night repair,
the narrow interior visibility-volume bypass, the Xbox ENV tone/color restoration
path, and the world-distance controls described above.

Depth producer precision, richer Xbox water shading, and several asset-specific tree
questions remain research-only.
