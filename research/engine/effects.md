# CEffect / XWP architecture

**Source:** dedicated PC CEffect reverse-engineering pass plus PAL Xbox 360 cross-version timing comparison, 2026-09-26..27; presentation/effects root addendum from Mega RE Census, 2026-10-04.
**Builds:** Steam 1.01b, GOG 1.01b, and original PAL Xbox 360 cross-checked for the CEffect/XWP core; new CFade/CMap presentation-root addendum is Steam-only until homology is established.
**Status:** core class/resource/lifecycle structure and the original fixed-delta contract are confirmed; presentation organizing roots are conditionally mapped; runtime high-FPS consequences, presentation lifetime, and several friendly scenario labels remain open.

This document records the current effect-subsystem findings. The key architectural correction is that `CEffect` is not the class that owns the main particle simulation, render-packet, or spatial-tree virtuals. On PC those methods are inherited unchanged from `CRdObjectEffect`; the Xbox comparison additionally shows that the simulation/fixed-delta core predates the Director's Cut port.

## Architecture at a glance

```text
CEffectAdmin singleton
  |- 195-entry effect type catalog
  |    `- name[16] + XWP resource id
  |- XWP load/resolve mutex
  |- 500-slot live CEffect registry
  `- CreateByType / CreateByName / InitExistingByName
       |
       v
CEffect (0x348 bytes)
  |- CEffect-specific tail state
  |- name-derived behavior class
  |- game-side event callback
  |- optional ordinary-object bridge
  `- contextual world/UI logic
       |
       v
CRdObjectEffect
  |- XWP -> runtime part conversion
  |- per-part simulation
  |- ordinary gameDelta60 timing
  |- selected fixed-delta=1.0 exception
  |- visibility/render packet
  `- inherited spatial ownership
       |
       +-> renderer intrusive registry
       `-> world spatial tree / active-list
```

Keep the stages distinct:

```text
CEffect registry insertion
    != name classification
    != spatial-tree payload activation
    != render-packet generation
```

## 1. Class layering

Confirmed inheritance:

```text
CRdObject
  -> CRdObjectEffect
       -> CEffect
```

On PC, `CEffect` and `CRdObjectEffect` share vtable entries 1..18. Only the destructor slot differs. Therefore the main simulation, visibility, render-packet, generic event, and spatial-tree methods belong to `CRdObjectEffect`.

The recovered Xbox `CRdObjectEffect`/`CEffect` virtual shape has 17 slots. Its simulation core maps cleanly to PC, while the two extra PC tail virtuals correspond to the already-mapped render-packet scratch/build and spatial/tree integration. This supports a useful porting split: the effect simulation/content contract is inherited from Xbox, while parts of renderer/world integration were extended or reorganized for PC/DC.

### Steam 1.01b vtables

- `CRdObjectEffect::vftable = 0x00782754`
- `CEffect::vftable = 0x007827B4`

| Slot | Address | Role |
|---:|---:|---|
| 1 | `0071BE70` | reset/init runtime effect state |
| 2 | `0071C010` | free per-part runtime buffers/resources |
| 3 | `0071C170` | dispatch event 1 |
| 4 | `0071C190` | main effect simulation/update |
| 5 | `0071C250` | dispatch event 9 |
| 9 | `0071C270` | renderability/visibility predicate |
| 10 | `0071C320` | build render packet/context |
| 11 | `0071C560` | dispatch event `0x13` |
| 12 | `006BAB20` | finish/remove + event 2 |
| 17 | `00679120` | render-packet scratch wrapper |
| 18 | `004029B0` | spatial owner/tree update |

### GOG 1.01b homologs

- `CRdObjectEffect::vftable = 0x00782744`
- `CEffect::vftable = 0x007827A4`

Relevant homologs are `0071BB80`, `0071BD20`, `0071BE80`, `0071BEA0`, `0071BF60`, `0071BF80`, `0071C030`, `0071C270`, `006BAA70`, `00679070`, and `004029A0`.

## 2. CEffect object layout

Allocation size is `0x348` bytes in both PC builds.

Constructors:

- Steam `CEffect::ctor`: `006750C0`
- GOG `CEffect::ctor`: `00675010`
- Steam `CRdObjectEffect::ctor`: `00679080`
- GOG `CRdObjectEffect::ctor`: `00678FD0`

CEffect-specific tail:

| Offset | Working meaning | Status |
|---:|---|---|
| `+0x218..+0x317` | 16 x 16-byte XWP dependency/resource-name slots | CONFIRMED |
| `+0x318` | CEffectAdmin live-registry slot index | CONFIRMED |
| `+0x31C` | name-derived behavior mode | CONFIRMED |
| `+0x320` | runtime field | init confirmed, semantics open |
| `+0x324` | signed 16-bit effect/type ID | CONFIRMED |
| `+0x326` | secondary 16-bit field | init confirmed, semantics open |
| `+0x328` | state byte | init confirmed, semantics open |
| `+0x330` | auxiliary ordinary-object handle | HIGH |
| `+0x334` | condition/token ID A | HIGH |
| `+0x338` | condition/token ID B | HIGH |
| `+0x33C` | runtime field | init confirmed, semantics open |
| `+0x340` | type-`0x19` fade/blend accumulator | CONFIRMED |
| `+0x344` | source/owner/context handle | HIGH |

`+0x32C` is not explicitly initialized by the CEffect constructor and should remain unnamed.

## 3. CEffectAdmin

Approximate object size is `0x7DC`:

```text
+0x000 vtable
+0x004 effect-type table pointer
+0x008 CEffect* slots[500]
+0x7D8 mutex / mutex wrapper
```

Constructors/destructors/getters:

| Role | Steam | GOG |
|---|---:|---:|
| ctor | `00403360` | `004033B0` |
| dtor | `004034D0` | `00403060` |
| singleton getter | `004052E0` | `004052B0` |
| allocate live slot | `00679230` | `00679180` |

The slot allocator scans 500 pointers and stores the first free `CEffect*`. Callback event 0 writes the returned index to `CEffect+0x318`; the destructor clears that slot.

### Effect type catalog

Startup loads resource `0x39C1`, installs the table at admin `+0x4`, then walks `0xF3C / 0x14 = 195` records and appends `.XWP` to their names.

High-confidence record model:

```cpp
struct EffectTypeEntry {
    char name[16];
    int resourceId;
};
```

Type-table init:

- Steam `00675390`
- GOG `006752E0`

Type -> resource resolver:

- Steam `006753E0`
- GOG `00675330`

The resolver contains a separate special-variant remap for selected W0/W1 BUR/SPL families. Keep the selector itself unnamed until its owning game mode is proven.

## 4. XWP resource architecture

XWP version string: `ver1.5`.

Name path family:

```text
UPDATA/EFFECT/XWP/<basename>/<filename>.XWP
```

Load helpers:

| Role | Steam | GOG |
|---|---:|---:|
| ensure/load by resource ID | `00676030` | `00675F80` |
| ensure/load by name | `00676070` | `00675FC0` |
| preload/validation helper | `006761F0` | `00676140` |

During parsing:

```text
DAT_0147FFDC = XWP header/blob
DAT_0147FFE0 = blob + 0x20 = first part
```

Reconstructed file layout:

```text
header +0x00 : part count
header +0x04 : version string
header +0x20 : part array
part stride  : 0x118
```

Useful part fields:

| Offset | Role |
|---:|---|
| `+0xC8` | optional numeric dependency/resource ID |
| `+0xF8` | count/capacity-like runtime field |
| `+0x100` | part flags |
| `+0x104` | resolved dependency/resource handle |
| `+0x108` | dependency/resource name string |

If `+0xC8 != 0`, the dependency resolves by numeric ID; otherwise by the name at `+0x108`. Relevant names are copied into the per-instance `CEffect+0x218..+0x317` cache.

### XWP -> CRdObjectEffect runtime parts

`FUN_0071C580` builds the runtime representation:

- `object+0x19C` = part count;
- runtime part array at `object+0x1A8`;
- runtime part stride `0x20`;
- runtime entry `+0x14` points to the part definition;
- per-part particle/runtime buffers are allocated;
- selected units are converted once during parsing, including degrees -> radians and some `/10` coordinate scaling.

This parser belongs to the base effect layer, not the CEffect-specific tail.

## 5. Creation APIs and lifecycle

Important function cluster:

| Role | Steam | GOG |
|---|---:|---:|
| find type by name | `00674FB0` | `00674F00` |
| CEffect ctor | `006750C0` | `00675010` |
| auxiliary-object toggle | `00675190` | `006750E0` |
| CEffect event callback | `00676520` | `00676470` raw entry |
| CreateByTypeWithContext | `00677960` | `006778B0` |
| CreateByName | `00677CF0` | `00677C40` |
| InitExistingByName | `00678600` | `00678550` |
| CreateByType | `00678BA0` | `00678AF0` |
| ClassifyName | `006791C0` | `00679110` |
| admin mutex unlock | `00679210` | `00679160` |
| scalar deleting dtor | `00679260` | `006791B0` |

Common creation flow:

```text
resolve/load XWP + dependencies
 -> allocate 0x348 CEffect or initialize existing object
 -> CEffect / CRdObjectEffect ctor
 -> install transform
 -> base reset/init
 -> XWP -> runtime part conversion
 -> assign effect type at +0x324 when type-based
 -> install CEffect event callback
 -> immediate event 0
 -> renderer intrusive registry insertion
 -> CEffect::ClassifyName(name)
 -> unlock CEffectAdmin mutex
```

Renderer registry insertion RVAs:

- Steam `DP.exe+0x002C5AE0`
- GOG `DP.exe+0x002C55E0`

Classification runs **after** registry insertion. A registry hook may therefore see a valid type at `+0x324` while `+0x31C` is still zero.

## 6. Name-derived behavior modes

`CEffect::ClassifyName` is Steam `006791C0`, GOG `00679110`.

Confirmed mapping:

```text
S*        -> mode 1
F1FIR005  -> mode 2
other F*  -> mode 3
other     -> mode 0 unless another path changes it
```

For `S*` and `F*`, inherited `+0x1B4` also receives bit `8`.

### Important correction: F1FIR005

An older probe note said `F1FIR005 -> mode 3`. Raw Steam x86 proves the opposite:

```asm
call strstr
neg eax
sbb al, al
add al, 3
```

Truth table:

```text
match found     -> AL = 2
match not found -> AL = 3
```

Therefore `F1FIR005` is mode 2 and other `F*` names are mode 3.

## 7. Mode 2 / mode 3 gameplay interaction

Steam `00675540`, GOG `00675490` runs only for `CEffect+0x31C == 2 || 3`.

This is gameplay behavior, not just rendering classification. The path:

- validates current map/player state;
- uses a broad player proximity gate of roughly 40 world units;
- iterates active runtime effect parts;
- transforms part/particle positions through the effect matrix;
- applies distinct mode-2 and mode-3 shape/proximity tests;
- dispatches event-`0x1C`-style gameplay packets with branch-specific payloads.

Treat `+0x31C` as a **behavior class**.

## 8. CEffect event callback

Steam `00676520`, GOG raw entry `00676470`.

| Event | CEffect-specific behavior |
|---:|---|
| `0x00` | live-instance registration and initialization |
| `0x01` | main game-side policy and type-specific behavior |
| `0x02..0x0B` | no CEffect-specific body here |
| `0x0C` | set related-object flag family `0x0A000000` |
| `0x0D` | clear related-object bit `0x08000000` |
| `0x0E..0x11` | no CEffect-specific body here |
| `0x12` | type-`0x19` contextual world/UI feedback path |
| other | return |

### Event 0

Confirmed behavior:

- inherited `+0x30 = 0x19`;
- allocate one CEffectAdmin live slot and store it at `+0x318`;
- allocation failure can mark the effect finished;
- selected effect/source types trigger special startup handling;
- source/index `+0x14` can seed current context at `+0x18` and participate in object-state checks;
- then generic effect initialization continues.

### Event 1

This is the main CEffect game-policy branch. Confirmed subdomains:

- types `0x19/0x1A` adjust inherited byte `+0x160` from game/player state;
- around 2000 world units, eligible effects toggle inherited `+0x1B4 & 4`, suppressing or enabling additional effect work;
- behavior modes 2/3 call the F-player interaction helper;
- source types `0x817/0x818` consult stage/object state and toggle inherited `+0xD8 & 0x40000000`;
- effect types `0x0D/0x11` can react to eligible class-3 actors within roughly 15 units;
- `+0x334/+0x338` are condition/token IDs queried through separate player-state predicates, not ordinary pointers;
- a separate related/global-object query can affect the same visibility/disable state;
- auxiliary gameplay objects are created or destroyed through `+0x330`.

### Event 0x12

Strictly associated with effect type `0x19` in the reviewed branch. It consumes source/context `+0x344`, computes its own distance conditions, and updates `+0x340` as:

```text
fade += gameDelta60 * 0.02
or
fade -= gameDelta60 * 0.02
clamp to [0, 1]
```

It then drives contextual 2D/HUD feedback. One branch formats `No.%03d`. Keep the human-facing feature label open; architecturally this is a **world-effect -> contextual HUD/feedback bridge**.

## 9. Auxiliary ordinary-object bridge

`CEffect+0x330` can own an ordinary gameplay object created for selected effect types.

Known map:

| Effect type | Ordinary object ID |
|---:|---:|
| `0x10` | `0x275` |
| `0x11` | `0x276` |
| `0x12` | `0x275` |
| `0x13` | `0x275` |
| `0x14` | `0x275` |
| `0x32` | `0x272` |
| `0x33` | `0x273` |
| `0xA3` | `0x24A` |

On disable/destruction the object is deleted and `+0x330` returns to `-1`.

## 10. CRdObjectEffect simulation timing

Inherited main update:

- Steam `0071C190`
- GOG `0071BEA0`

Core behavior:

```cpp
float delta = gameDelta60;
if (object->flags1C0 & 2)
    delta = 1.0f;

if (!(object->flags1B4 & 2))
    CheckAllPartsFinishedAndPossiblyRemove();

UpdateEffectParts(delta);
```

Per-part simulation then receives approximately:

```text
partDelta = delta * object+0x1BC
```

Ordinary effects therefore use the engine's 60-Hz-relative `gameDelta60` and are delta-aware.

### Fixed-delta exception and Xbox 360 origin

PC `+0x1C0 & 2` forces `delta = 1.0` per object update. The PAL Xbox 360 executable contains the same semantic branch in `sub_82548180`:

```text
Xbox:
delta = *(float*)0x842AD3B0
if (object+0x25C & 2)
    delta = 1.0
if (!(object+0x250 & 2))
    completionCheck()
UpdateEffectParts(object, delta)

PC:
delta = gameDelta60
if (object+0x1C0 & 2)
    delta = 1.0
if (!(object+0x1B4 & 2))
    completionCheck()
UpdateEffectParts(object, delta)
```

The fixed-delta branch is therefore **original Xbox engine behavior**, not a Director's Cut invention.

Confirmed type-based setters also match across Xbox and PC:

- type `0x1E`;
- types `0x2A..0x2E`;
- type `0x2C` additionally receives the separate bit-1 behavior.

The Xbox name-create path has the same six-check fixed-bit structure as the PC path. The corresponding PC literals are:

- `P0MZL001`, `P0MZL003`, `P0MZL004`, `P0MZL005`, `P0MZL007`, `P0MZL009`;
- broader init-existing families `P0MZL00*`, `W0BUR*`, `P0TYA001`, `W1SPL*`, and `P0HIT*`.

Cross-version structure strongly supports the same authored family contract. Friendly expansions such as "muzzle", "burst", "splash", or "hit" remain inference from names and are not promoted as facts.

### Cadence consequence

The original Xbox gameplay scalar is measured in 1/60-second units and normally advances by about `2` at the native ~30 Hz gameplay cadence. The fixed family deliberately advances by only `1` on each such update:

```text
Xbox ~30 Hz:
  ordinary: 30 * 2.0 ~= 60 delta-units/s
  fixed:    30 * 1.0  = 30 delta-units/s

PC 60 Hz:
  ordinary: 60 * 1.0  = 60 delta-units/s
  fixed:    60 * 1.0  = 60 delta-units/s

PC 120 Hz:
  ordinary: 120 * 0.5 = 60 delta-units/s
  fixed:    120 * 1.0 = 120 delta-units/s
```

The branch itself is intentional; the **cadence assumption around it** is the high-refresh regression candidate. If PC `CRdObjectEffect` runs once per normal gameplay/object update, its literal per-update `1.0` no longer preserves the original Xbox wall-time ratio as update frequency rises.

A narrow A/B repair candidate is conceptually `fixedEffectiveDelta ~= gameDelta60 * 0.5`, which preserves the native Xbox 1:2 fixed/ordinary ratio. This is **not yet a production conclusion**. Runtime validation must cover lifetime, emission/animation speed, collisions/events, one-shot callbacks, low FPS, high FPS, and both PC builds before any patch is shipped.

Do not globally normalize the effect subsystem because ordinary effect simulation and the type-`0x19` fade path already consume `gameDelta60` correctly.

`+0x1C0 & 1` is a separate render/particle-filter flag used with XWP part flags and must not be confused with the fixed-delta bit.

See [`../evidence/ceffect_xbox_timing/README.md`](../evidence/ceffect_xbox_timing/README.md) for the compact cross-version evidence table.

## 11. Render/world integration

Inherited render-packet wrapper:

- Steam `00679120`
- GOG `00679070`

It ensures scratch space of at least `0x144`, calls the vslot-10 packet builder, and updates inherited success state.

Inherited spatial owner/tree update:

- Steam `004029B0`
- GOG `004029A0`

This path owns inherited spatial field `+0x14C` and adds/removes the effect from the spatial tree when ownership changes.

Runtime probing previously confirmed CEffect world effects entering the renderer registry, then spatial payload activation, and resolving to render-kind 3.

## 12. Separate CEffect distance domains

Do not collapse these into one `EffectDrawDistance` or one extension of `ObjectActivationDistanceScale`:

1. **World active-list distance** inherited from the general object/spatial system.
2. **~2000-unit internal effect-work gate** in CEffect event 1, toggling inherited `+0x1B4 & 4`.
3. **~40-unit broad F-mode player proximity gate** before per-part gameplay tests.
4. **Per-part F-mode geometry/radius tests** after the broad gate.
5. **Type-`0x19` event-`0x12` contextual HUD distance logic**, with separate data-driven thresholds and fade.

Only item 1 belongs to the existing global object-activation distance mechanism.

## 13. Destructor / ownership

Destructor bodies:

- Steam `006754C0`
- GOG `00675410`

Scalar deleting destructors:

- Steam `00679260`
- GOG `006791B0`

Confirmed teardown includes clearing the CEffectAdmin slot recorded at `+0x318` before generic base cleanup and optional storage free.

## 14. 2026-10-04 CFade, presentation, and CMap weather roots

The Mega RE Census adds a presentation-effects layer that is adjacent to, but distinct
from, the CEffect/XWP hierarchy above.

Steam `00BDBCC4` is a typed lazy `CFadeManager` singleton root. The manager is `0x14`
bytes and retains three CFade pointers at `+4/+8/+0x0C`, populated through selector
`0x3B` creation paths. This proves a three-object retainer at the selected scope, not
exclusive ownership or complete deletion coordination.

Factory-installed CFade callback `0044ACD0` has a selected event-1 numeric/state path and
an event-`0x12` path that supplies CFade `+0x18C..+0x198` data to acquired CRdPrim.
Separately, the application tail can consume latch `00BE1EAC`, clear it, obtain manager
`+4`, and feed that same quartet to CRdPrim. A typed movie/task path can issue CFade
request 7 and set the latch, establishing a concrete movie -> fade -> presentation
boundary without proving final GPU semantics or freshness.

The weather roots are also clearer. The apparent CEffectRain/CEffectHaze globals at
`01437414/01437418` are actually `CMap` fields at static root `013936F0 + 0xA3D24/+0xA3D28`.
CMap is therefore a verified retainer for the selected weather objects. It is not proven
to be their exclusive owner or the only deletion authority.

These findings do not alter the XWP fixed-delta timing conclusion. They add organizing
roots and presentation ownership boundaries around a different effect family.

## 15. Remaining open questions

- exact semantics of `CEffect+0x320`, `+0x326`, `+0x328`, and `+0x33C`;
- producer-friendly names for condition/token IDs `+0x334/+0x338`;
- exact scenario names for every event-1 type-specific branch;
- precise user-facing identity of the type-`0x19` event-`0x12` HUD/feedback path;
- runtime high-FPS behavior of each fixed-delta family, including lifetime, emission/animation, collisions/events, and one-shot callbacks;
- whether a 30-Hz-normalized fixed delta reproduces Xbox wall-time behavior without introducing low-FPS or pause/load/reset regressions;
- complete symbolic naming of every XWP part field.

## 16. Do-not-carry-forward corrections

- **DISPROVEN:** `F1FIR005 -> mode 3`. Correct mapping is `F1FIR005 -> mode 2`, other `F* -> mode 3`.
- **DISPROVEN framing:** CEffect owns its own main render/update virtuals. Those are inherited unchanged from `CRdObjectEffect`.
- **DO NOT assume:** renderer-registry insertion means name classification is already complete. Classification follows insertion.
- **DISPROVEN:** the fixed `delta = 1.0` branch is a PC/DC invention. The original Xbox code contains the same branch and matching numeric setters.
- **DO NOT globally normalize CEffect timing.** Ordinary effects and type-`0x19` fade are already delta-aware; only the fixed-delta family is a cadence-sensitive candidate domain.
