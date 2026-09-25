# Six main-frustum visibility classes

**Status:** CONFIRMED core mechanics; specialized consumer semantics remain partially classified
**Primary build:** GOG 1.01b
**Steam homologs:** producer `FUN_006B62E0`; common consumer reported at `FUN_006BD320` and should be treated as cross-build corroboration rather than the naming authority

This document supersedes the earlier interpretation that every object type independently chooses one of six camera frusta inside a unique `vtable+0x24` implementation. That interpretation was too distributed. The PC executable contains a common object-classification layer and a common frustum consumer shared by a large fraction of gameplay classes.

## 1. Producer

GOG `FUN_006B6230` constructs six projection/frustum variants every camera rebuild:

| Index | Native far plane |
|---:|---:|
| 0 | 200000 |
| 1 | 80000 |
| 2 | 20000 |
| 3 | 5000 |
| 4 | 1000 |
| 5 | 500 |

Each variant is stored through `FUN_004022C0` at:

```text
camera + 0x1FC + index * 0xC0
```

The associated absolute plane coefficients begin at:

```text
camera + 0x25C + index * 0xC0
```

The AABB test `FUN_006BB2C0` directly indexes this layout with the selected class index.

## 2. Object classification

Object initialization reaches:

```text
FUN_0040B240(object)
    ├─ FUN_0040ADE0
    ├─ FUN_0040A940
    ├─ FUN_0040ABC0
    └─ FUN_0040AC20
```

`FUN_005E76F0`, the large object factory, writes its factory type byte directly to `object+0x30` after construction. Therefore factory cases can be matched mechanically to the classifier for objects created through this path.

The frustum-class bits live in the low dword at `object+0x138`:

| Bit | Mask | Decoder result | Native far plane |
|---:|---:|---:|---:|
| 18 | `0x00040000` | class 0 | 200000 |
| 19 | `0x00080000` | class 1 | 80000 |
| 20 | `0x00100000` | class 2 | 20000 |
| 21 | `0x00200000` | class 3 | 5000 |
| 22 | `0x00400000` | class 4 | 1000 |
| none | — | class 5 | 500 |

`FUN_006BB440` is the priority decoder. It checks those masks in exactly the order shown above and returns `0..5`.

This priority matters for type `0x05`: one subtype can carry both class-1 and class-3 bits, but class 1 wins because `0x80000` is tested first.

## 3. Explicit type assignments

The following are direct assignments from `FUN_0040AC20` plus the class-3 additions in `FUN_0040ADE0`. Class names come from the same factory where a direct constructor/vtable mapping is available.

### Class 0 — 200000

| Type | Known class | Evidence |
|---:|---|---|
| `0x2B` | not yet named from the main factory | `FUN_0040AC20` sets `0x40000` |

### Class 1 — 80000

| Type | Known class | Condition |
|---:|---|---|
| `0x05` | `CNpcAnimal` | `object+0x621 == 6`; class-3 bit is also set by fallthrough, but class-1 wins decoder priority |

**Correction:** an older working note associated class 1 with York/Player. The main factory instead maps type `0x05` to `CNpcAnimal`; no York-specific class-1 claim should be retained without separate evidence.

### Class 2 — 20000

| Type | Known class | Condition |
|---:|---|---|
| `0x33` | `CHouse` | resource scale `> 450` |
| `0x46` | base `CObject` factory case | resource scale `> 450` |
| `0x62` | `CObjectTarget` | unconditional |

### Class 3 — 5000

| Type | Known class | Notes |
|---:|---|---|
| `0x05` | `CNpcAnimal` | normal branch; `+0x621 != 6` |
| `0x09` | `CNpcCar` | direct |
| `0x33` | `CHouse` | resource scale `<= 450` |
| `0x46` | base `CObject` factory case | resource scale `<= 450` |
| `0x49` | `CObjectShelf` | direct |
| `0x53` | `CObjectCar` | direct |
| `0x5C` | `CObjectSignal` | set earlier by `FUN_0040ADE0`; survives `FUN_0040AC20` |
| `0x64` | `CObjectPutGrass_Far` | direct |
| `0x80` | `CObjectOldTree` | direct |
| `0x95` | `CObjectShopSign` | direct |
| `0xA3` | `CObjectParkingMob` | set by `FUN_0040ADE0` |
| `0xA4` | `CObjectFountain` | set by `FUN_0040ADE0` |
| `0xA8` | `CObjectFieldPhysics_Type1` | direct |
| `0xA9` | `CObjectFieldPhysics_Type2` | direct |
| `0xAA` | `CObjectFieldPhysics_Type3` | direct |
| `0xAB` | `CObjectFieldPhysics_Type4` | direct |
| `0xB1` | `CObjectConcreateBlockType1` | direct |
| `0xB2` | `CObjectConcreateBlockType2` | direct |
| `0xB7` | `CObjectMergeTree` | direct |

There is at least one later resource-specific override path (`FUN_0057C650`) that can force class-3 behavior for selected resource IDs. Therefore the table above describes the normal type-classification layer, not an assertion that class bits are forever immutable for every specialized object.

### Class 4 — 1000

| Type | Known class |
|---:|---|
| `0x4D` | `CObjectTreeshaphand` |
| `0x4E` | `CObjectTreeshaphand_Museam` |
| `0x4F` | not yet named from main factory |
| `0x51` | `CObjectMannequin` |
| `0x52` | `CObjectHideBox` |
| `0x58` | `CObjectWristDoor` |
| `0x59` | `CObjectHandDoor` |
| `0x69` | `CObjectWirenet` |
| `0x6C` | `CObjectSlideDoor` |
| `0x6D` | `CObjectShutter` |
| `0x76` | `CObjectItemEffect` |
| `0x77` | `CObjectPushDoor` |
| `0x7A` | `CObjectIvyLock` |
| `0x83` | `CObjectMuDeadTree` |
| `0x87` | `CObjectWesternDoor` |
| `0x88` | `CObjectToruso` |
| `0x96` | `CObjectWinebox` |
| `0x98` | `CObjectPianoHarry` |
| `0xAD` | `CMissile` |

### Class 5 — 500 default

If none of bits 18–22 is set, `FUN_006BB440` returns class 5.

Two especially important direct factory mappings are:

```text
0x9A -> FUN_00589FF0 -> CObjectHut
0x9C -> FUN_005BD830 -> CObjectVendor
```

`FUN_005E76F0` then writes `0x9A` / `0x9C` to `object+0x30`. Neither type is promoted by `FUN_0040AC20` or `FUN_0040ADE0`, so both use the default class-5 / 500-unit main frustum.

This provides a direct PC explanation for the observed sleep-hut / vending-machine disappearance near 500 units. It is **not** the native 1000-unit active-list threshold.

## 4. Common consumer, not one unique culler per object type

Direct GOG PE/vtable inspection gives:

```text
309 named vtables with a code pointer at slot +0x24
83 unique +0x24 targets
158 named vtables point +0x24 to FUN_006BCE30
```

Examples sharing `FUN_006BCE30` include:

```text
CRdObjectModel
CCharacter
CRdObjectModelGame
CNpc
CNpcAnimal
CNpcBird
CObject
CObjectCar
CNpcCar
CNpcDog
CNpcEnemy
CNpcMob
CNpcNormal
CPlayer
CObjectHut
CObjectVendor
CHouse
CObjectDoor family
CObjectItem / effects
CRiver / CLake
many world props
```

Therefore the earlier statement “each object type implements its own culling method and chooses a frustum independently” is **DISPROVEN as a general architecture description**.

Specialized `+0x24` implementations do exist, but the dominant gameplay path is centralized.

## 5. `FUN_006BCE30` mechanics

For the common path:

1. clear temporary object flag `+0xD8 bit 0x10`;
2. reject explicit hidden/suppressed object flags;
3. require model/bounds pointers at `+0x160/+0x164`;
4. call `FUN_006BB440` to obtain the main-frustum class;
5. test the object AABB via `FUN_006BB290` against that exact camera frustum;
6. if the main test fails, test up to three secondary frusta through `FUN_006C4140(camera+0x67C)`;
7. if needed, test an extrusion along `camera+0x44..0x4C` through `FUN_006C3EA0` against the six main frusta;
8. secondary acceptance sets object flag `+0xD8 |= 0x10`.

This is not merely “draw or do not draw”. It combines main-camera visibility with a secondary directional/cascade rescue path used by later renderer submission.

## 6. `camera+0x67C` secondary three-frustum set

`FUN_006C4140` treats `camera+0x67C` as an optional array of **three** frusta, each `0x60` bytes of six planes.

`FUN_006D2710` writes:

```text
renderer+0x6708 == 0 -> camera+0x67C = NULL
renderer+0x6708 != 0 -> camera+0x67C = renderer+0x5F88
```

The renderer builds/copies three matrices/frusta into `renderer+0x5F88` in the directional-light path. See `world/shadows.md`.

## 7. ZachFix control boundary

`MainFrustumDistanceMode` changes selected native short-range **far planes**, but does not change the class bits or classifier:

```text
Original      class 3/4/5 = 5000 / 1000 / 500
Extended      class 3/4/5 = 5000 / 1000 / 1000
Extended Plus class 3/4/5 = 5000 / 5000 / 5000
Extreme       class 3/4/5 = 20000 / 20000 / 20000
```

Classes 0/1/2 remain `200000 / 80000 / 20000` in production.

This separation is intentional: ZachFix extends visibility distance without silently reclassifying object types.
