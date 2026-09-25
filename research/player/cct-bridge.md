# CPlayer → Character Controller bridge

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

## Event 9 nuance

CPlayer Event 9 also reaches lower-level controller synchronization (`FUN_004E31E0`-family). This is a real later Actor phase, but it is **not** the missing state-handler→CCT bridge. The decisive bridge is the Event-1 post-state `FUN_004E3280/004E3350` call.
