# CPlayer → Character Controller bridge

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Readable research synthesis; follow the cited evidence for build-specific claims.

**Jump to:** [Corrected architecture](#corrected-architecture) · [Player path](#player-path) · [Controller handles](#controller-handles) · [Vehicle suppression](#vehicle-suppression) · [Event 9 / GroundSnap nuance](#event-9-groundsnap-nuance)
<!-- END AUTO RESEARCH NAV -->

## Corrected architecture

The previously assumed bridge through `vtable ~0x007741F0 → FUN_0048B0A0 → FUN_004831C0 → FUN_00481D70` was a class-ownership error.

`FUN_0048B0A0` is an override in the `CNpcDog` virtual-table region and calls a common/base NPC character implementation. That chain remains useful for NPC CCT architecture, but it is **not York's missing state→CCT bridge**.

## Player path

Steam preserves the crucial Event-1 code:

```text
CPlayer Event 1
    ↓
0x008A9758[player->state_654]()
    ↓
Player post-state processing
    ↓
FUN_004E3280 (Steam)
FUN_004E3350 (GOG homolog)
    ↓
state-aware Player character-controller reconciliation
```

This puts the Player CCT boundary in the same Event-1 update after the gameplay state handler.

## Controller handles

Player has two controller handles at:

```text
CPlayer +0x94C
CPlayer +0x950
```

The PhysX controller manager stores live controller objects in an indexed table and exposes position/set-position/move-like wrappers around them. Descriptor creation distinguishes controller type 0 vs type 1; the accumulated static evidence supports the interpretation that Player owns a box/capsule pair, with the capsule being the primary locomotion controller in the ordinary move path.

Keep the exact shape-to-field assignment marked **STRONGLY_SUPPORTED** unless a future raw constructor trace is added to this document.

## Vehicle suppression

Inside the Player CCT routine, states:

```text
0x03
0x7D
0x87
```

enter a special branch that performs controller-state cleanup/synchronization and returns before ordinary on-foot reconciliation.

For `0x87`, this is the structural point where York's normal on-foot controller movement is suppressed while the selected car object becomes authoritative for movement.

## Event 9 / GroundSnap nuance

CPlayer Event 9 remains a later Actor/controller-synchronization phase, but the older
address-level GroundSnap story must be kept separate from that architectural fact.
The current GOG export still contains the fixed-style correction body at
`FUN_004E31E0`, yet current call/xref/raw scans do **not** reproduce the inherited
claim that it has 37 direct callers. Its live reachability may be indirect, shifted,
or superseded and is therefore OPEN.

Do not use the stale caller census to infer a render-cadence GroundSnap fix. The
decisive Player state-handler→CCT bridge remains the Event-1 post-state
`FUN_004E3280/004E3350` call. `NxController::move` itself is immediate, so any fixed
per-call correction must be classified by its actual live caller cadence rather than
by the scene solver.
