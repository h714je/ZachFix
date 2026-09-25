# Engine architecture overview

**Source:** reconciled engine map v10, 2026-09-26.

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
    -> one-slot staged CInput snapshot
    -> held/rising/repeat derivation
    -> public CInput getters
    -> Player / camera / UI / vehicle consumers
```

Native XInput in ZachFix intentionally bridges into this native action layer rather
than replacing it.

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

The reconciled renderer map separates native resource/scene problems from ZachFix
PostFX work. Production-closed findings include the `HOUSE_LIST.NOD` day/night repair,
the narrow interior visibility-volume bypass, the Xbox ENV tone/color restoration
path, and the world-distance controls described above.

Depth producer precision, richer Xbox water shading, and several asset-specific tree
questions remain research-only.
