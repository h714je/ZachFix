# Save / GameRecord persistence architecture

**Research snapshot:** 2026-09-30.
**Scope:** Deadly Premonition: The Director's Cut PC, Steam/GOG 1.01b.

This page is the compact architecture view of the PC persistence system. The byte-level
canonical specification and evidence log live in
[`../evidence/game_record/ZachFix_GameRecord_RE_2026-09-30.md`](../evidence/game_record/ZachFix_GameRecord_RE_2026-09-30.md).

## Top-level save image

The native PC save is one fixed-size image:

```text
dp.sav size = 0x7A2620 = 8,005,152 bytes

+0x000000  header, 0x120 bytes
+0x000120  GameRecord[0], 0x45CC0 bytes
             ...
             GameRecord[27], 0x45CC0 bytes
```

The arithmetic is exact:

```text
0x120 + 28 * 0x45CC0 = 0x7A2620
```

The runtime staging image begins at `DAT_00BE5EF0`; record0 begins at
`DAT_00BE6010`. The live current record is embedded in the Game singleton:

```text
Game + 0x8C568 == record + 0x0000
recordOffset = GameRuntimeOffset - 0x8C568
```

This is an important architectural boundary: the save record is not a detached data
model reconstructed field-by-field by a separate serializer. Much of the live gameplay
state is already laid out in a save-compatible record inside `Game`.

## Save/load strategy consequence

For ZachFix multi-save or slot-selection work, the safest design is therefore:

```text
select / redirect complete native dp.sav image
    -> let vanilla loader restore the selected GameRecord
    -> let native post-load reconstruction rebuild transient runtime state
```

Do not manually synthesize a GameRecord from high-level gameplay properties unless a
future feature explicitly needs a controlled migration layer. Whole-image redirection
preserves unknown-but-valid fields, version-local policy, event history, object
registries, weapon instances, and post-load side effects.

## Native snapshot pipeline and save modes

The native serializer does not expose one universal "save only while idle" gate. Instead,
callers establish a resume contract and then converge on the same persistent snapshot
pipeline.

Steam anchors:

```text
FUN_006ACAB0  generic/full save-image builder
FUN_006AE0C0  manual/phone save-image builder
FUN_004524C0  pre-save persistent synchronization dispatcher
FUN_00451450  resume/location/York-transform snapshot
FUN_00470900  CTimer -> GameRecord snapshot
FUN_00430A10  CEvent/CEvCore save-mode snapshot policy
FUN_004B1560  NPC/world persistence snapshot
FUN_00408BD0  physical dp.sav writer
```

For modes other than 3/4, `FUN_004524C0` runs the normal synchronization cluster before
the current `0x45CC0` GameRecord is copied into the staging image:

```text
FUN_00451450(...)
FUN_00449480()          // CTimer singleton/accessor
FUN_00470900()          // timer snapshot
FUN_00405360(mode)
FUN_00430A10(mode)
thunk_FUN_004B76D0()
FUN_004B1560()
```

The recovered save-mode roles are:

| Mode | Native use | Current interpretation |
| ---: | --- | --- |
| `0` | generic/full normal save builder | full persistent snapshot |
| `1` | phone/manual save | full snapshot plus manual-save CEvent resume adapter |
| `2` | CEvent checkpoint capture (`0xC6`) | checkpoint snapshot |
| `3` | `Game+0x8C5EC & 0x00004000` | special top-level state-`0x46` context; normal pre-save cluster skipped |
| `4` | `Game+0x8C5EC & 0x00080000` | load/restore transition active; normal pre-save cluster skipped |
| `5` | historical GameRecord capture | full synchronization before copying to history storage |

The exact friendly name of the top-level state-`0x46` / mode-3 context remains OPEN.
Do not rename it to a title/cutscene mode without further evidence.

### Phone/manual save is not restricted to CPlayer state 0

`CObjectPhone` enters CPlayer state `0x37` through native Event `0x2F`. The phone state
contains its own phase machine and starts the manual save/menu transition while York is
still in state `0x37`; only later does the phone choreography return him to normal state
`0`.

Therefore `CPlayer+0x654 == 0` is **not** a native save-safety invariant. Vanilla itself
serializes from a controlled non-zero Player state.

`FUN_006AE0C0` also performs manual-save-specific preparation before selecting mode 1.
When the active CEvent/context permits it, it clears global gameplay bits `0x00800000`
and `0x01000000`. If mode-3 or mode-4 flags are present it selects those modes instead
of mode 1.

### CEvent autosave/checkpoint callers have their own gates

The CEvent save callers confirm that safety policy lives partly above the serializer:

```text
CEvent 0xC2 autosave/checkpoint transition
    -> gate through FUN_00723820() and an additional context bit test
    -> normalize resume/transition flags
    -> enter native save transition

CEvent 0xC6 checkpoint capture
    -> gate through FUN_004FEAB0()
    -> establish checkpoint-specific runtime flags
    -> FUN_004524C0(mode 2)
    -> copy current GameRecord to Game+0xBE8 checkpoint backup
```

`0xC2` explicitly clears global bits `0x00800000`, `0x01000000`, and `0x04000000`
before establishing its own resume path. This is direct evidence that a native save
caller may normalize transient resume state before snapshotting.

## Resume model: persistent world plus reconstruction, not a live-runtime dump

The normal load path reconstructs York and transient runtime state rather than restoring
the complete live CPlayer object. The common positional-resume branch applies saved
transform through Steam `FUN_00509080` after Player initialization has reset numerous
runtime handles, phases and action fields.

The core normal-resume block is:

```text
record+0xCAD8..+0xCADB  map/place/load tuple
record+0xCADC           uint16 load/location selector
record+0xCAE0..+0xCAEF  York position/resume vector
record+0xCAF0..+0xCAFF  York orientation/resume vector
record+0xCB00           resume flags snapshot
```

`FUN_00451450` snapshots current York position/orientation when no explicit vectors are
provided. A one-shot scripted-resume request is represented by
`Game+0x8C5EC & 0x04000000`: the snapshot routine consumes that bit, writes
`record+0xCADA = 0xFF`, and clears the bit. On load `CADA == 0xFF` selects the predefined
scripted load-point path rather than normal positional restore.

The native load code also contains a small post-load positional nudge with map/location
exceptions. Its purpose is consistent with avoiding bad respawn overlap near
interactables, but a phone-specific historical name is not proven.

## Special resume adapters

The save/load architecture is not purely "always return to state 0". Specific gameplay
protocols can opt into one-shot resume adapters encoded in persistent GameRecord state.

### `playerStateMaskHi bit 0x00000004` -> CPlayer state `0x40`

This is the clearest recovered example.

The only confirmed producer found in the reviewed path is world-resource subtype
`0x75C`. Before entering the corresponding interaction the engine writes a target
transform into the early live GameRecord/object-action storage and sets:

```text
record+0x64 / Game+0x8C5CC bit 0x00000004
```

On load, if that bit is present, Player initialization selects CPlayer state `0x40`
instead of the common normal-state path. State `0x40` reconstructs the relevant
world/location context, uses the persisted transform/context data, and later calls the
mask-clear helper with `(low=0, high=4)`. The marker is therefore a confirmed one-shot
resume token.

The transform side of this contract overlaps the generic object-action packet:

```text
record+0x14             runtime action-object field (may be a live object reference)
record+0x18..+0x24      packet transform/vector fields; resume-significant for subtype 0x75C
record+0x38             packet heading/yaw field; resume-significant for subtype 0x75C
```

This means the `0x48` action packet cannot be treated either as wholly disposable or as
a fully serializable runtime object. Some fields are deliberately reused by native
resume protocols, while pointer-like runtime fields still require reconstruction rather
than literal reuse after process/map rebuild.

### Manual-save CEvent `0x1F5` adapter

Mode 1 contains one explicit CEvent special case. If the active CEvent runtime mode at
`CEvent+0x3012` equals `0x1F5`, `FUN_00430A10` sets persistent
`globalGameplayFlags` bit `0x1000` and stores two event-runtime DWORDs in:

```text
record+0x43818  <- CEvent+0x3000
record+0x4381C  <- CEvent+0x3004
```

The load path later checks the `0x1000` marker, recreates the `0x1F5` controller through
`FUN_0042D080`, restores the saved runtime value, dispatches the associated `0x25F/0x1C`
path, and clears the marker. These two DWORDs are therefore no longer an anonymous gap
in the GameRecord tail; they are a manual-save special-resume scratch pair.

No equivalent mode-specific branch was found for modes 0, 2 or 5 in this CEvent snapshot
routine. The rest of the persistent synchronization cluster is shared.

## Safe-save contract: current architecture conclusion

The evidence supports the following model:

```text
live runtime
    -> caller-specific safety/resume policy
    -> runtime-to-persistent synchronization
    -> GameRecord snapshot
    -> dp.sav / checkpoint/history copy

load
    -> restore persistent GameRecord
    -> reconstruct transient world/Player runtime
    -> apply normal positional resume
       OR a small explicit special-resume adapter
```

Accordingly, the absence of a universal F5-style save is **not** evidence that save
locations are fundamentally hardcoded. Native phone saves, CEvent autosaves and
checkpoints all use the same persistent machinery but establish different controlled
resume contracts.

For a future save-anywhere experiment, the conservative research boundary is:

```text
DENY while mode-3/mode-4 contexts are active
DENY while the one-shot scripted-resume bit 0x04000000 is pending
DENY while world/fade transition FUN_004492C0() reports active
require a valid live York object
preserve native special-resume markers rather than forcibly zeroing them
use the native full synchronization/snapshot path rather than hand-building GameRecord
```

This is an engineering consequence, not yet a claim that every arbitrary mid-action
state is safe. Vehicle states (`0x38/0x87/0x88`) and multi-phase object protocols remain
high-value runtime validation targets because their reconstruction ordering may involve
additional live objects.

## Canonical GameRecord domains

The `0x45CC0` record now has no large unexplained gameplay region. The major domains are:

| Record range / offset | Role | Status |
| --- | --- | --- |
| `+0x0014` | live image overlaps the generic `0x48` object-action packet | structural anchor |
| `+0x0090` | `CTimer` day/hour/minute/second snapshot | CONFIRMED |
| `+0x00A8` | repeated-dialogue message ID table | CONFIRMED structure |
| `+0x04AC` | Side Mission event-byte mirrors | CONFIRMED |
| `+0x0574` / `+0x0890` | event value table + event bitset | CONFIRMED |
| `+0x0B96..+0x0B12F` | serialized PC reserved span | STRONGLY_SUPPORTED |
| `+0xB130..+0xC9F3` | toolbox / inventory / weapon condition / item-acquired state | CONFIRMED/HIGH |
| `+0xCA34..+0xCB0F` | salary, suit and resume/location state | CONFIRMED/HIGH |
| `+0xCB10..+0x116FF` | named-NPC, NPC-instance and spawn-rule persistence | CONFIRMED architecture |
| `+0x11700..+0x3E6FF` | generic world-object persistence registry | CONFIRMED architecture |
| `+0x3E700..+0x417FF` | door, item-removal, placement override, dropped item, light, first-visit and vehicle schedule registries | CONFIRMED/HIGH |
| `+0x41800..+0x427FF` | weather, CEvCore, event-history, conversation, fishing, achievements, status and resume state | CONFIRMED/HIGH |
| `+0x42800..+0x43A63` | playtime, message history, header mirror, chapter times, continue count | CONFIRMED/HIGH |
| `+0x43B3C..+0x45CBB` | carried/toolbox weapon-instance matrices | CONFIRMED |
| `+0x45CBC` | trailing reserved DWORD | STRONGLY_SUPPORTED |

Remaining uncertainty is mainly friendly field naming and class-local flag semantics,
not ownership or large-region boundaries.

## Inventory, item and weapon persistence

The item domain is indexed by the 379 ITEM records used by the PC game:

```text
+0xB130  toolbox pending/overflow counts[379]   HIGH
+0xB71C  toolbox stored counts[379]             CONFIRMED
+0xBD08  carried inventory counts[379]          CONFIRMED
+0xC2F4  temporary inventory backup[379]        CONFIRMED
+0xC8E8  weapon condition/ammo meter[67]        CONFIRMED
+0xC9F4  item acquired/registered bitset        CONFIRMED
```

Weapon-instance state is stored separately from count/condition state:

```text
+0x43B3C  carried weapon instances  67 * 16 floats
+0x44BFC  toolbox weapon instances  67 * 16 floats
```

The instance values are ordered, with the most-worn active condition occupying the
front of the relevant slot family in observed saves.

## NPC persistence cluster

Three different NPC layers are deliberately separate:

```text
+0xCB10  NamedNpcPersistentState[50]      50 * 0x78
+0xE280  NamedNpcRuntimeSnapshot[50]      50 * 0x40
+0xEF00  NpcWorldInstanceState[256]       256 * 0x24
+0x11300 NpcSpawnRulePersistent[256]      256 * 0x04
```

The first two are the named-character layer, the third is a world-instance pool, and
`+0x11300` is only a compact persistent overlay over the richer live spawn-rule table.
Do not collapse these into one generic NPC save structure.

## Generic world-object registry

The largest persistent gameplay structure is:

```cpp
struct WorldObjectPersistentState { // 0x28
    int8_t   mapId;
    int8_t   areaId;
    uint16_t placementKey;
    uint32_t stateFlags;
    uint8_t  payload[0x20];
};

WorldObjectPersistentState worldState[0x1200]; // 4608 entries
```

Location:

```text
record +0x11700
Game   +0x09DC68
size   0x2D000
end    record+0x3E700
```

`FUN_00453D80` allocates/finds by `(mapId, areaId, placementKey)`; the placement key is
derived from the active world-placement row at `row+0x2A`, stride `0x3C`.

The `0x20`-byte payload is a class-local union. A common overlay stores position and
rotation when `stateFlags & 0x04000000`, but many object classes use the same bytes for
other persistent values. The registry must therefore remain typed as a union/opaque
payload at the global map level.

Common cross-class flag anchors:

```text
0x02000000  live/resident-instance marker       HIGH
0x04000000  transform payload valid             CONFIRMED
```

Lower bits remain object-class-local unless a concrete consumer proves otherwise.

## Specialized world registries

The tail after the generic registry is now structurally mapped:

```text
+0x3E700  DoorPersistentState[256]              256 * 0x08
+0x3EF00  NormalWorldItemRemoved[512]           512 * 0x04
+0x3F700  SpecialWorldItemRemoved[64]            64 * 0x04
+0x3F800  CPutPersistentOverride[256]            256 * 0x08
+0x40000  DroppedWorldItem[128]                  128 * 0x10
+0x40800  LightPersistentState[128]              128 * 0x08
+0x40C00  VisitedWorldPlacementEntry[128]        128 * 0x04
```

The normal and special item-removal sets are intentionally separate. ITEM categories
`2` and `4` use the special set; category `4` includes Profiling Pieces.

`CPutPersistentOverride` is mirrored by CEvent/script control into live placement
state and survives save/load. Field `+0x05` is not written by the reviewed PC path and
remains reserved/HIGH rather than semantically named.

## Vehicle availability persistence

The old "live table plus fallback table" interpretation is superseded. Both regions
store the same schedule:

```cpp
struct VehicleAvailabilityWindow { // 0x04
    uint8_t startHour;
    uint8_t startMinute;
    uint8_t endHour;
    uint8_t endMinute;
};
```

Backing namespaces:

```text
+0x40E00  genericVehicleAvailability[128]
+0x41000  mapSpecificVehicleAvailability[13][32]
```

`FUN_00453A70(index)` selects the namespace from the current map:

```text
map  1 -> bank  0      map  8 -> bank  6      map 40 -> bank 10
map  2 -> bank  1      map  9 -> bank  7      map 41 -> bank 11
map  3 -> bank  2      map 10 -> bank  8      map 42 -> bank 12
map  4 -> bank  3      map 11 -> bank  9
map  5 -> bank  4
map  6 -> bank  5
all other maps -> generic 128-entry table
```

The split is architectural, not padding. `CCar` uses the same thirteen map values to
select map-specific vehicle resource families (`0x54..0x5F/0x64`) instead of generic
resource type `0x48`. The schedule writer is CEvent opcode `0xAC`, handler
`FUN_00437F90`; `CCar` reads the same table through `FUN_005B1B30`.

The physical size closes exactly:

```text
+0x40E00..+0x40FFF = 128 * 4
+0x41000..+0x4167F = 13 * 32 * 4
+0x41680            = next reserved span
```

## Weather, event core and CEvent persistence

The systemic tail ties save data back into several live subsystems:

```text
+0x41800           CWeather persistent block
+0x4180C float     stationary/idle vehicle event interval, default 300.0 = 5 s
+0x41810 float     interaction/event distance threshold, default 50.0 HIGH
+0x41814           CEvCore bit-0x2000 mirror
+0x41818..+0x41887 CEvCorePersistentSnapshot, 0x70 bytes
```

The three routine-history registries are keyed by `(eventId, routineId)`:

```text
+0x41888  CEvent 0xC6 checkpoint-capture history[128]
+0x41A88  CEvent 0xC2/0 autosave/checkpoint history[128]
+0x41C88  CEvent 0xCD chapter-history capture history[128]
```

Their ownership is now semantic rather than merely structural:

- `0xC6` deduplicates full current-record checkpoint capture;
- `0xC2/0` deduplicates autosave/checkpoint transition entry;
- `0xCD` deduplicates chapter-history capture into the 27-record historical archive.

Resume identity is retained at:

```text
+0x427EC uint16 eventId
+0x427EE uint16 routineId
```

The historical archive used by `0xCD` is separate runtime storage at:

```text
Game + 0xD2228 + historySlot * 0x45CC0
```

with slot 0 reserved for the episode-0/prologue case and later slots derived from the
global chapter index.

`+0x42088` is a saved event-selected audio-cue override ID (HIGH). `+0x42090` is
another 128-entry `(eventId,routineId)` set, owned by NPC conversation history rather
than the checkpoint trio. `+0x42290..+0x4258F` is the following strongly-supported
reserved PC span.

## Player/system tail

Additional persistent owners established in the tail include:

```text
+0x42590  CFishing persistent state, 0x24
+0x425B4  completion/achievement progress mask
+0x4260C  HP/pulse/sleepiness/hunger family
+0x427E0  active/current suit mirror
+0x42800  uint64 playtime seconds
+0x42818  messageSeen[32768] bitset, 0x1000 bytes
+0x43818  manual-save CEvent 0x1F5 resume scratch DWORD A
+0x4381C  manual-save CEvent 0x1F5 resume scratch DWORD B
+0x439B4  16-byte mirror copied to save header +0x110..+0x11F
+0x439F4  chapterTimeSeconds[27]
+0x43A60  total continues HIGH / near-CONFIRMED
```

## Reserved PC spans

The following regions remain intentionally labeled `STRONGLY_SUPPORTED` rather than
historical-source-code `CONFIRMED`:

```text
+0x0B96..+0x0B12F   0xA59A bytes
+0x41680..+0x417FF   0x0180 bytes
+0x42290..+0x4258F   0x0300 bytes
+0x45CBC              0x0004 bytes
```

The early `0xA59A` span was zero across the retained corpus of 644 GameRecord images
and has no reviewed PC consumer beyond whole-record initialization/copy behavior.

## Remaining fine-grained work

Large-region ownership is considered closed for the reviewed PC builds. The save/resume pass also
closes the major save/resume control-flow architecture, while arbitrary mid-action runtime
validation remains intentionally separate. Remaining targets are intentionally narrow:

- friendly/original names for several world-placement keys and class-local flags;
- byte-perfect `CWeather` field names;
- `CPutPersistentOverride +0x05` historical purpose;
- exact friendly semantics of `record+0x41810`;
- rare CEvent `0xC2/1` forced/bypass policy name;
- fine `CFishing` counter/result/tutorial field names;
- exact names for some HP/pulse/sleep/hunger auxiliary fields and `+0x427E8/+0x427E9`;
- final promotion or correction of `+0x43A60 totalContinues` after one more direct owner proof;
- complete census of any additional one-shot resume tokens analogous to `playerStateMaskHi bit 0x4`;
- runtime validation of vehicle and multi-phase object-action saves before any save-anywhere feature.

These are symbolic-detail questions, not evidence of another unknown serialized
subsystem.

See also [`../evidence/save_resume_contract/README.md`](../evidence/save_resume_contract/README.md) for the focused save/resume evidence summary.
