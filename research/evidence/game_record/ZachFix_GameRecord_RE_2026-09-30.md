# ZachFix - DP PC GameRecord / dp.sav reverse-engineering map

Date: 2026-09-30
Scope: Deadly Premonition: The Director's Cut PC, Steam/GOG-target RE
Status: **canonical working specification / consolidated research snapshot**

This document consolidates the 2026-09-30 `dp.sav` / `GameRecord` reverse-engineering work into one current interpretation. It intentionally removes or supersedes earlier coarse names that were disproven by later deep passes. The original incremental research log should be retained separately for provenance.

The important state of the RE is now:

- the top-level `dp.sav` layout is exact;
- the boundaries of every large `GameRecord` region are understood;
- the former 42.4 KiB early "unknown" region is effectively confirmed unused/reserved in shipping PC builds;
- the complete named-NPC persistence cluster is structurally decoded;
- the 4608-entry generic world-object registry is structurally decoded as a keyed variant/union store;
- the tail registries are assigned to doors, world-item removal, scene/resource overrides, dropped items, lights, first-visit tracking and vehicle availability scheduling;
- the systemic tail contains confirmed CEvCore, fishing, achievement, player-status, message-history, timing and weapon-instance persistence;
- remaining uncertainty is mostly **fine semantic naming of individual flags/enums/fields**, not unknown large structures.

## Evidence labels

- **CONFIRMED**: directly established by game code, serializer/restore symmetry, exact layout/math, controlled behavior, or multiple independent evidence anchors.
- **HIGH**: strong code/data agreement; role is reliable but a friendly/original symbolic name or one policy detail remains unresolved.
- **STRONGLY_SUPPORTED**: negative/static/corpus evidence is overwhelming, but a fully computed hidden consumer cannot be mathematically disproven.
- **OPEN**: structure/field exists, but exact semantics or symbolic ownership remain unresolved.

## Evidence base

Primary evidence used by this map:

- Steam PC raw machine code / Ghidra decompile / raw asm;
- GOG PC cross-build checks where relevant;
- retained save corpus: 23 complete `dp.sav` images, 28 `GameRecord`s each = 644 record images;
- `ITEM.PRM`, `SALARY.PRM`, `NPC_LIST.PRM`, `NPC_LIST.SRL`, `MES_ALL.MES` and related game data;
- direct serializer/restore paths and runtime accessors;
- targeted save-corpus field censuses.

The names used below are **safe descriptive names**, not claims that the original source code used those exact identifiers.

## 1. Top-level save layout - CONFIRMED

```text
dp.sav size = 0x7A2620 = 8,005,152 bytes

+0x000000  header, 0x120 bytes
+0x000120  record0, 0x45CC0 bytes
             ...
             record27, 0x45CC0 bytes
```

`0x120 + 28 * 0x45CC0 = 0x7A2620`.

Runtime staging image begins at `DAT_00BE5EF0`; `DAT_00BE5EF0+0x120 == DAT_00BE6010 == record0`.

Runtime live-record base:

```text
Game + 0x8C568 == record + 0x0000
```

Therefore, for live-record fields:

```text
recordOffset = GameRuntimeOffset - 0x8C568
```

The known general 0x48-byte action packet at `Game+0x8C57C` therefore begins at `record+0x14`.

### Header runtime-table correction - CONFIRMED

Header `0x74..0x10F` is copied by the save builder from one contiguous **39-DWORD runtime table**. The earlier conceptual split of 31 gameplay statistics plus 8 sentinel DWORDs is not a serialization boundary.

## 2. Canonical GameRecord offset map

All offsets below are relative to the start of one `0x45CC0`-byte `GameRecord` unless stated otherwise.

| Record offset | Size / count | Current interpretation | Status |
|---|---:|---|---|
| `+0x0014` | `0x48` | serialized image overlaps the general object-action packet in the live record | structural anchor |
| `+0x0060` | `0x08` | `playerStateMaskLo/Hi` | CONFIRMED structure |
| `+0x0068` | `0x0C` | event-conditioned capability/filter masks | HIGH |
| `+0x0074` | `0x04` | facial-hair growth time | CONFIRMED |
| `+0x0078` | `0x04` | service/vehicle unlock flags; low 14 bits mapped | CONFIRMED low14 / OPEN upper bits |
| `+0x007C` | `0x04` | current + pending character-model variant bytes | HIGH/CONFIRMED structure |
| `+0x0080` | `0x02` | door-block interaction message ID | CONFIRMED/HIGH |
| `+0x0084` | `0x04` | global gameplay flags DWORD | CONFIRMED structure |
| `+0x0090` | `0x10` | day/hour/minute/second `CTimer` snapshot | CONFIRMED |
| `+0x00A8` | `0x400` | `repeatDialogueMessageIds[512]` | CONFIRMED structure / HIGH umbrella semantics |
| `+0x04AA` | `0x02` | current/target character-family selector | CONFIRMED role |
| `+0x04AC` | `0x64` | two Side Mission event-byte mirror arrays | CONFIRMED |
| `+0x0574` | `0x31C` | 199 event numeric/value DWORDs | CONFIRMED boundary |
| `+0x0890` | `0x306` | event bitset | CONFIRMED boundary |
| `+0x0B96` | `0xA59A` | unused/reserved serialized PC span | STRONGLY_SUPPORTED / effectively confirmed for reviewed PC builds |
| `+0xB130` | `379 * 4` | toolbox pending/overflow counts | HIGH |
| `+0xB71C` | `379 * 4` | toolbox stored counts | CONFIRMED |
| `+0xBD08` | `379 * 4` | carried inventory counts | CONFIRMED |
| `+0xC2F4` | `379 * 4` | temporary-inventory backup | CONFIRMED for mode `0x2000` |
| `+0xC8E0` | small | selected/current item state | OPEN fine layout |
| `+0xC8E8` | `67 * 4` | unified weapon condition/ammo meter | CONFIRMED |
| `+0xC9F4` | `48` | item acquired/registered bitset for 379 IDs | CONFIRMED structure |
| `+0xCA34` | `28 * 4` | salary/action counters | CONFIRMED |
| `+0xCAA4` | suit block | suit selection + dirtiness/timing state | mixed CONFIRMED/HIGH |
| `+0xCAD8` | `0x38` | pre-save resume/location/player transform snapshot | CONFIRMED structure / mixed fine semantics |
| `+0xCB10` | `50 * 0x78` | `NamedNpcPersistentState[50]` | CONFIRMED owner/architecture; some fine names HIGH |
| `+0xE280` | `50 * 0x40` | `NamedNpcRuntimeSnapshot[50]` | CONFIRMED |
| `+0xEF00` | `256 * 0x24` | NPC world-instance persistence pool | CONFIRMED architecture/layout |
| `+0x11300` | `256 * 4` | NPC spawn-rule persistence overlay | CONFIRMED architecture/layout |
| `+0x11700` | `4608 * 0x28` | generic keyed world-object persistence registry | CONFIRMED architecture/layout |
| `+0x3E700` | `256 * 0x08` | door-family persistent state | CONFIRMED layout / HIGH owner name |
| `+0x3EF00` | `512 * 0x04` | normal world-item removal set | CONFIRMED |
| `+0x3F700` | `64 * 0x04` | special/key/Profiling-Piece removal set | CONFIRMED |
| `+0x3F800` | `256 * 0x08` | `CPutPersistentOverride[256]` placement-state overlay | CONFIRMED owner/restore contract; `+5` reserved HIGH |
| `+0x40000` | `128 * 0x10` | dropped-world-item position + item ID | CONFIRMED |
| `+0x40800` | `128 * 0x08` | `CLight` persistent state | CONFIRMED |
| `+0x40C00` | `128 * 0x04` | first-visit world-placement/location key set | CONFIRMED mechanism / HIGH friendly ID name |
| `+0x40E00` | `128 * 4` | generic type-`0x48` vehicle availability schedules | CONFIRMED |
| `+0x41000` | `13 * 32 * 4` | map-specific vehicle availability schedule banks | CONFIRMED |
| `+0x41680` | `0x180` | reserved/unused candidate | STRONGLY_SUPPORTED |
| `+0x41800` | weather block | `CWeather` persistent state | CONFIRMED owner; fine layout incomplete |
| `+0x4180C` | `float` | stationary/idle vehicle event interval in 60-Hz units, default `300.0f` (5 s) | CONFIRMED role |
| `+0x41810` | `float` | event/interaction distance threshold, default `50.0f` | HIGH |
| `+0x41814` | `4` | CEvCore flag `0x2000` mirror | CONFIRMED |
| `+0x41818` | `0x70` | `CEvCorePersistentSnapshot` | CONFIRMED |
| `+0x41888` | `128 * 4` | CEvent `0xC6` checkpoint-capture routine history | CONFIRMED |
| `+0x41A88` | `128 * 4` | CEvent `0xC2/0` autosave/checkpoint routine history | CONFIRMED |
| `+0x41C88` | `128 * 4` | CEvent `0xCD` chapter-history capture routine history | CONFIRMED |
| `+0x42088` | `2` | saved event-selected audio cue override ID | HIGH |
| `+0x42090` | `128 * 4` | NPC conversation event/routine history | CONFIRMED |
| `+0x42290` | `0x300` | reserved/unused candidate | STRONGLY_SUPPORTED |
| `+0x42590` | `0x24` | `CFishing` persistent state | CONFIRMED owner / partial fine semantics |
| `+0x425B4` | `4` | completion/achievement progress mask | CONFIRMED |
| `+0x4260C` | status arrays | HP/pulse/sleepiness/hunger state | mixed CONFIRMED/HIGH |
| `+0x427E0` | small | active/current suit mirror | CONFIRMED |
| `+0x427E8/+0x427E9` | bytes | small persistent fields | OPEN |
| `+0x427EC/+0x427EE` | `2 + 2` | CEvent resume `eventId` + `routineId` | CONFIRMED |
| `+0x42800` | `8` | playtime seconds | CONFIRMED |
| `+0x42818` | `0x1000` | `messageSeen[32768]` bitset | CONFIRMED |
| `+0x439B4` | `0x10` | mirror copied to save header `+0x110..+0x11F` | CONFIRMED mirror |
| `+0x439F4` | `27 * 4` | chapter clear times in seconds | CONFIRMED |
| `+0x43A60` | `4` | total continues | HIGH / near-CONFIRMED |
| `+0x43B3C` | `67 * 16 * 4` | carried weapon-instance states | CONFIRMED |
| `+0x44BFC` | `67 * 16 * 4` | toolbox weapon-instance states | CONFIRMED |
| `+0x45CBC` | `4` | trailing reserved/padding DWORD | STRONGLY_SUPPORTED |

The map deliberately does not invent names for bytes that are only known structurally. Large-region ownership is essentially closed; the remaining work is fine-grained semantics.

## 3. Early GameRecord state fields: `+0x60..+0x87`

### +0x60/+0x64: paired 64-bit Player state mask - CONFIRMED structure

Runtime `Game+0x8C5C8/+0x8C5CC`. Helpers `FUN_004FD820`, `FUN_004FD840`, and `FUN_004FD860` always accept a `(lowMask, highMask)` pair and set, clear, or test both words together.

Safe structural representation:

```text
+0x60 uint32 playerStateMaskLo
+0x64 uint32 playerStateMaskHi
```

The exact original symbolic meaning of every bit remains OPEN. Many CPlayer action/combat/interaction states manipulate individual bits, so `playerStateMask64` is safer than inventing a narrower name such as input flags.

### +0x68/+0x6C/+0x70: event-conditioned capability/filter masks - HIGH umbrella

Runtime `Game+0x8C5D0/+0x8C5D4/+0x8C5D8`.

`FUN_004FEDE0`, `FUN_004FEE20`, `FUN_004FEE60`, `FUN_004FEEA0`, and `FUN_004FEF00` expose set/clear/test semantics. Several accessors invert their interpretation depending on `FUN_0042D010`, which identifies the normal active-event/gameplay condition. These masks gate CPlayer/context interaction behavior.

All retained record0 samples currently inspected serialize these three words as zero, consistent with transient capability/filter state at save points.

Exact per-word historic names remain OPEN.

### +0x74: facial-hair growth time - CONFIRMED

Runtime `Game+0x8C5DC`.

`CMustache` code (`FUN_00527660` and adjacent methods) reads this DWORD, adds elapsed time-of-day seconds with a `0x15180 = 86400` wrap, compares against growth thresholds, and fades between three facial-hair models. `FUN_00527A60` resets it to zero.

Safe label:

```text
+0x74 uint32 facialHairGrowthTimeSeconds
```

### +0x78: Repair/vehicle unlock bitset - CONFIRMED low 14 bits

Runtime `Game+0x8C5E0`.

The AddItem path has a dedicated branch for item IDs `0x16D..0x17A` (365..378):

```text
record+0x78 |= 1 << (itemId - 365)
```

The ITEM/MESSAGE map identifies these IDs as:

```text
365 Repair
366..378 thirteen named character vehicles
```

Thus bits 0..13 are persistent ownership/unlock state for Repair plus the 13 vehicles. Other code also uses higher bits (`0x8000/0x10000/0x20000`), so the complete DWORD has additional special-state semantics still OPEN.

Safe label:

```text
+0x78 uint32 serviceVehicleUnlockFlags
```

with low14 CONFIRMED and upper bits OPEN.

### +0x7C..+0x7F and +0x4AA/+0x4AB: character-family/model rebuild state

The resource builders reveal the character-family enum directly:

```text
0 = NM = 02_NM / CNM = male NPC
1 = NF = 03_NF / CNF = female NPC
2 = NC = 04_NC / CNC = child NPC
3 = PL = 01_PL / CPL = player
4 = EN = 05_EN / CEN = enemy
```

`record+0x4AA` is the current family selector and `record+0x4AB` is the target/pending family used during model rebuild. `FUN_0044F360` clamps the requested family to 0..4, stores it in +0x4AB, and triggers `FUN_0044E730` when a rebuild is needed.

This corrects the earlier uncertain ordering of the five status-array families. The default HP values `100,85,100,1000,100` therefore correspond to NM/NF/NC/PL/EN, respectively.

`+0x7C/+0x7D` are the current character-model resource variant pair consumed by `FUN_0044E730`. `+0x7E/+0x7F` are the pending pair: deferred model-change code stores them and the main Player update later calls `FUN_0044E730(+0x7E,+0x7F)` when the deferred rebuild flag is serviced.

Safe names:

```text
+0x7C uint8 characterModelVariantA
+0x7D uint8 characterModelVariantB
+0x7E uint8 pendingCharacterModelVariantA
+0x7F uint8 pendingCharacterModelVariantB
```

Exact friendly meaning of variant A vs B (model/costume/texture sub-index) remains OPEN.

### +0x80: door-block interaction message ID - CONFIRMED/HIGH

Runtime `Game+0x8C5E8`, uint16.

Door script command `FUN_004376B0` broadcasts Door Event `0x19` and stores the supplied message ID here. When the script provides no message, the game substitutes one of the stock message IDs:

```text
0x3C4C -> "CAN'T LEAVE NOW."
0x425D -> "I HAVE THE FEELING THAT I FORGOT SOMETHING, ZACH."
0x425E -> "NO, NOT NOW."
```

Safe label:

```text
+0x80 uint16 doorBlockedMessageId
```

`+0x82..+0x83` currently have no direct consumer and remain padding/reserved candidate.

### +0x84..+0x87: one global gameplay flags DWORD - CONFIRMED structural correction

Do not split +0x86/+0x87 into separate fields. They are the upper bytes of the same DWORD at runtime `Game+0x8C5EC`.

Examples:

- `FUN_004508D0` reads bit1;
- `FUN_004508E0` reads bit2;
- `FUN_004508F0` reads bit15;
- `FUN_00450900` reads bit16 by viewing the upper uint16 at +0x86;
- other code tests bit24 through byte +0x87.

Safe representation:

```text
+0x84 uint32 globalGameplayFlags
```

Individual bit taxonomy remains a separate RE task.

## 4. Timer, repeat-dialogue and Side Mission persistence

### +0x0090..+0x009F: CTimer snapshot - CONFIRMED

Save/restore code copies four DWORD fields between `record+0x90` and `CSingleton<CTimer>+0x08`. `CTimer` update code independently establishes their exact meanings:

```text
record+0x90  uint32 day
record+0x94  uint32 hour
record+0x98  uint32 minute
record+0x9C  uint32 second
```

`FUN_00470320` derives hour/minute/second from elapsed seconds and increments the day counter on day rollover. The constructor default is day 0, 07:00:00. Retained saves match the expected in-game clock progression. The day field is also the source used for the Results-screen `TOTAL NUMBER OF DAYS` value.

### +0x00A8..+0x04A7: repeat-dialogue message-ID history - CONFIRMED structure / HIGH umbrella semantics

The constructor initializes this region to `0xFF`. Semantically it is:

```text
int16 repeatDialogueMessageIds[512]
```

with `0xFFFF` as unused.

- `FUN_0044F6C0` finds the first negative/FFFF short and appends a message ID.
- `FUN_0044F6F0` searches all 512 shorts for membership.
- `FUN_00470950` clears the list back to FFFF.
- dialogue/event selection code checks this list to choose first-time versus repeat/alternate message branches and appends newly encountered IDs.

Saved IDs resolve directly through `MES_ALL.MES`; observed examples include ordinary NPC/York dialogue messages. This is distinct from the global `seenMessages[0x8000 bits]` registry at `record+0x42818`.

Safe name: **`repeatDialogueMessageIds[512]`**. Exact broader inclusion rules beyond dialogue/event-message history remain HIGH rather than fully named.

### +0x04AC..+0x050F: Side Mission event-byte mirrors - CONFIRMED owner

Two adjacent 50-byte arrays:

```text
record+0x4AC  uint8 sideMissionPhaseAMirror[50]
record+0x4DE  uint8 sideMissionCompletionMirror[50]
```

`FUN_0044F9B0` increments the selected byte; `FUN_0044F9E0` reads it. During initialization/load, `FUN_0043CA90` scans Side Mission event type `0x28` and rebuilds these arrays:

```text
event IDs  0..49  -> first array
event IDs 50..99  -> second array
```

These are separate from the immediately following chapter-stamp arrays at `+0x510/+0x542`. Because the implementation increments bytes, safest field semantics are occurrence/count mirrors rather than booleans.

## 5. Event-state boundary and the `+0x0B96..+0x0B12F` reserved PC span

### Exact boundaries

The left boundary is established by the native event-state copy path in both Steam and GOG PC builds.

The event numeric/value table begins at:

```text
record+0x0574
199 DWORDs = 0x31C bytes
ends at record+0x0890
```

The event bitset then begins at:

```text
record+0x0890
size = 0x306 bytes
ends exactly at record+0x0B96
```

Steam `FUN_00433E40` and GOG `FUN_00433EC0` independently perform the same copies:

```text
199 DWORDs -> Game+0x8CADC == record+0x0574
0x306 bytes -> Game+0x8CDF8 == record+0x0890
```

The next confirmed persistent subsystem begins at:

```text
record+0x0B130
Game+0x97698
uint32 toolboxPending[379]
```

Therefore the exact gap is:

```text
record+0x0B96 .. record+0x0B12F
size = 0x0B130 - 0x0B96
     = 0xA59A
     = 42,394 bytes
```

### Save corpus result

The range was scanned across:

```text
23 complete dp.sav images
28 GameRecords per image
= 644 GameRecord images
```

Result:

```text
nonzero records: 0 / 644
nonzero bytes anywhere in the exact gap: 0
```

Every byte in `record+0x0B96..+0x0B12F` was zero in every inspected live/historical/empty record.

### Static PC evidence

The full Steam and GOG Ghidra `xrefs.csv` exports contain no direct data references into the corresponding absolute Game-object range:

```text
Game+0x8D0FE .. Game+0x97697
```

The apparent `0x8F1D4/0x8F274/0x8F294/0x8F2B4` numerical hits found in an earlier broad text scan are NOT GameRecord accesses. They are offsets from a different manager pointer (`*(manager+0x1C)`) used by localized menu texture setup.

Exact-boundary text/xref review also finds:

```text
record+0x0B96  no PC accessor found
record+0x0B130 explicit consumers in both Steam and GOG
```

Chapter Replay merge (`FUN_0044FE00` Steam and PC-equivalent GOG path) does not copy anything from this gap. Its item/toolbox carry-over begins explicitly at `record+0x0B130`.

GameRecord construction zeroes the entire `0x45CC0` record. No reviewed PC path subsequently populates this gap; whole-record save/load `memcpy` naturally carries the zero region along because it is physically inside the record image.

### Safe conclusion

For the reviewed PC Steam/GOG 1.01b builds:

```text
record+0x0B96..+0x0B12F
uint8 reservedUnusedGap[0xA59A];
```

Status: **HIGH / effectively CONFIRMED for the reviewed PC builds**.

The only reason not to use the bare label `padding` as an absolute historical claim is that static PC evidence cannot prove whether the original source layout once reserved the space for a removed/console-era structure. What IS established is that the shipping PC builds keep the bytes zero and no reviewed PC gameplay/save consumer uses them.

### Architectural consequence

The early-to-item portion of GameRecord now has a clean boundary:

```text
+0x0574 eventValue[199]
+0x0890 eventBits[0x306]
+0x0B96 reservedUnusedGap[0xA59A]
+0xB130 toolboxPending[379]
+0xB71C toolboxStored[379]
+0xBD08 inventory[379]
+0xC2F4 inventoryBackup[379]
...
```

This removes roughly 42 KB from the unresolved GameRecord surface in one step.

## 6. Inventory, Toolbox and item persistence

### Item / Toolbox substructure

Four consecutive 379-DWORD arrays occupy `record+0xB130..+0xC8DF`:

```text
+0xB130  uint32 toolboxPendingOrOverflow[379]  HIGH
+0xB71C  uint32 toolboxStored[379]             CONFIRMED
+0xBD08  uint32 inventory[379]                 CONFIRMED
+0xC2F4  uint32 inventoryBackup[379]           CONFIRMED in temp-inventory mode
+0xC8E0  current/selected item state           OPEN exact subfields
+0xC8E8  float weaponConditionOrAmmo[67]       CONFIRMED
+0xC9F4  uint8 itemAcquiredBits[48]            CONFIRMED structure/indexing
```

Every 379-DWORD table is exactly `0x5EC` bytes, with no gap.

### `+0xB71C`: Toolbox stored counts - CONFIRMED

Directly consumed by `CToolBox`. Transfers subtract from carry inventory and add to this table.

### `+0xB130`: pending/overflow counts - HIGH

Normal AddItem can route carry-limit excess here. Toolbox logic later merges this table into `+0xB71C` and clears it.

### `+0xC2F4`: temporary inventory backup - CONFIRMED for mode 0x2000

The temporary-inventory path copies normal `+0xBD08` to `+0xC2F4`, runs with alternate active inventory, then restores the backup and re-adds temporary items through normal AddItem.

### `+0xC8E8`: unified weapon meter - CONFIRMED

67 floats for item IDs 0..66.

- melee branch initializes to `100.0`: durability/condition percentage;
- firearm branch initializes from `ITEM.PRM+0x3E`: ammo capacity.

Known capacities match the displayed weapon descriptions (16 handgun, 180 SMG, 60 rifle, 6 Magnum, 7 shotgun, 100 flamethrower, 1 RPG, 3 Dart Gun).

### `+0xC9F4`: item acquired/seen bitset - CONFIRMED

Game sets:

```c
bits[itemId >> 3] |= 1 << (itemId & 7);
```

`ceil(379/8) = 48` bytes. Exact semantic distinction "acquired" vs "seen/registered" remains to be fully audited, but structure/indexing are exact.

### ITEM.PRM -> MESSAGE bridge - CONFIRMED

`ITEM.PRM` contains exactly 379 rows, stride `0xDC`, one-to-one with save item IDs 0..378.

Known fields:

```text
+0x30  storage/toolbox-related limit
+0x38  carry limit used by normal AddItem
+0x3E  firearm ammo-capacity field
+0x5E  uint16 ITM_LIST row index
+0x68  uint16 nameMessageId
+0x6A  uint16 descriptionMessageId
+0x6C  weapon condition/ammo branch discriminator
+0x6D  weapon class byte
```

`+0x68/+0x6A` resolve directly into `MESSAGE/OUTPUT/MES_ALL.MES`. The fan `_FR` override preserves message IDs while replacing text, independently confirming the mapping.

Native item classifier:

```text
1..73     cat0 - weapon inventory (HIGH)
74..121   cat1 - ordinary/consumable inventory (HIGH)
122..161  cat2 - story/key/progression item family (HIGH; friendly category label OPEN)
162..201  cat3 - OPEN
202..208  outside classifier
209..235  cat4 - Profiling Keyflags/Profile Pieces (CONFIRMED)
236..300  cat5 - Trading Cards #1..65 (CONFIRMED)
301..310  cat6 - York Suits/Outfits (CONFIRMED)
311..378  outside ordinary inventory classifier; service/food/vehicle pseudo-items etc.
```

### Chapter Replay inventory merge - CONFIRMED

`FUN_0044FE00(dst,src)` copies item ranges:

```text
0..208
(skip 209..235 Profiling Pieces)
236..378
```

and performs the same exclusion across the relevant paired item tables. This is direct code proof of the replay carry-over policy.

## 7. Salary, suit and pre-save resume snapshot

### `+0xCA34..+0xCAA3`: salary/action counters - CONFIRMED

```text
uint32 salaryActionCounters[28]
```

Runtime: `Game+0x98F9C`.

`SALARY.PRM` has exactly 28 rows, and `FUN_00450460` updates these counters and dispatches salary/Agent-Honor feedback through `FUN_005FF400`.

### Suit persistence

```text
+0xCAA4  uint8 currentSuitIndex       CONFIRMED
+0xCAA5  uint8 previousSuitIndex      HIGH
+0xCAA8  float suitDirtiness[10]      CONFIRMED/HIGH
+0xCAD0  dirty-suit timing marker     HIGH
+0xCAD4  dirty-suit effect/fly count  HIGH
```

`+0xCAA8` has ten 0..100 floats indexed by the current suit; suit menu logic uses 25/50/75 thresholds, and gameplay increments the active suit meter.

### `+0xCAD8..+0xCB0F`: resume/location snapshot

Directly written by pre-save snapshot logic (`FUN_00451450` called before save/history capture).

```text
+0xCAD8..+0xCADB  four-byte map/place/load tuple    CONFIRMED structure, OPEN symbolic names
+0xCADC            uint16 load/location component   HIGH
+0xCAE0..+0xCAEF  4-component player transform A   CONFIRMED
+0xCAF0..+0xCAFF  4-component player transform B   CONFIRMED
+0xCB00            flags snapshot                   OPEN
+0xCB04            current vehicle/object resource ID candidate HIGH
+0xCB06            temporary audio/BGM override ID candidate    HIGH/OPEN
+0xCB08            load-position/world-node index candidate     OPEN
+0xCB0C            timing/mode flag candidate                   OPEN
```

## 8. Named-NPC persistent state: `+0x0CB10`

### Identity/index mapping - CONFIRMED

The table is indexed by `CNpc+0xD54`:

```text
Game + 0x99078 + (CNpc+0xD54) * 0x78
record + 0x0CB10 + npcId * 0x78
```

`NPC_LIST.PRM` contains exactly 50 rows and maps those IDs directly:

```text
00 none             10 isaach           20 richard          30 brian
01 anna             11 jim              21 polly            31 young
02 becky            12 keith            22 xander           32 zeta
03 diane            13 lily/lilly       23 valentine        33 george_mam
04 carroll/carol     14 harry            24 fiona            34 isaiah
05 george           15 michael          25 gina             35 child_young
06 emily            16 nick             26 jack             36 child_zeta
07 thomas           17 olivia           27 wesley           37 child_george
08 usher/ushah      18 sallie           28 general          38 old_isaach
09 forest/kaysen    19 quint            29 sigourney        39 old_isaiah
40 youth_harry      41 willy            42 blank            43 boss_killer
44 boss_thomas      45 boss_george      46 boss_george      47 boss_forest
48 boss_forest      49 boss_forest
```

Therefore these are persistent records for the game's fixed named-NPC identity domain, not a generic runtime actor pool.

### Static partner: NPC_LIST.SRL - CONFIRMED structural match

`NPC_LIST.SRL` is exactly `0x5140 = 20800` bytes and decomposes exactly as the `CNpcMain` code indexes it:

```text
region A: 50 NPC * 12 entries * 0x10 = 0x2580
region B: 50 NPC * 12 entries * 0x10 = 0x2580
trailer : 50 NPC * 0x20              = 0x0640
                                           ------
                                           0x5140
```

`CNpcMain` loads resource ID `0x39F0` and `FUN_004C6230` searches one of the two 12-entry regions for the current NPC. Each `0x10` static row begins with two selector bytes and then contains three resolved resource IDs/handles.

This proves a static/dynamic split:

```text
NPC_LIST.SRL  = static per-NPC visual/resource variants
GameRecord    = selected variant + dynamic route/travel progress
```

### Refined 0x78 layout

Safe current representation:

```cpp
struct NpcTravelPhase {                 // 0x14
    float routeDistance;                // +00 HIGH
    float distanceForSpeedCalc;         // +04 HIGH
    float totalDuration;                // +08 CONFIRMED
    float elapsedTime;                  // +0C CONFIRMED
    uint32_t flags;                     // +10; bit0 = phase valid/active
};

struct NamedNpcPersistentState {        // 0x78
    uint8_t visualGroup;                // +00 CONFIRMED role
    uint8_t visualVariant;              // +01 CONFIRMED role
    uint8_t flags;                      // +02 bitfield, partial taxonomy below
    int8_t activeTravelPhase;           // +03, -1 none, 0..4 active CONFIRMED

    uint8_t pendingVisualGroup;         // +04 HIGH/CONFIRMED deferred role
    uint8_t pendingVisualVariant;       // +05 HIGH/CONFIRMED deferred role
    uint8_t forcedRouteProfile;         // +06 HIGH
    uint8_t reserved07;                 // +07 OPEN, zero in retained sample corpus

    uint8_t routeFromA;                 // +08 CONFIRMED route endpoint key component
    uint8_t routeFromB;                 // +09 CONFIRMED route endpoint key component
    uint8_t reserved0A;                 // +0A currently zero/unused candidate
    uint8_t reserved0B;                 // +0B currently zero/unused candidate

    uint8_t routeToA;                   // +0C CONFIRMED route endpoint key component
    uint8_t routeToB;                   // +0D CONFIRMED route endpoint key component
    uint8_t reserved0E;                 // +0E currently zero/unused candidate
    uint8_t reserved0F;                 // +0F currently zero/unused candidate

    NpcTravelPhase travel[5];           // +10..+73 CONFIRMED five-phase structure

    float routeStateTimeOfDaySeconds;   // +74 HIGH, writer CONFIRMED
};
```

The names `visualGroup/visualVariant` and route-key A/B are descriptive names, not recovered original C++ identifiers.

### +00/+01 current visual/resource variant - CONFIRMED role

`FUN_004C6230` reads `entry+00/+01` and uses them to select one of the 12 static `NPC_LIST.SRL` rows for this NPC. Consumers then use the static row's resource IDs to load the NPC resources.

`FUN_004C9EA0` changes both selectors and immediately triggers NPC resource/model refresh paths.

The default is `1,1` for all 50 entries, with NPC-specific exceptions such as Carol during reset. This is a visual/resource selector pair, not route phase state.

### +04/+05 deferred visual/resource variant - HIGH / structurally confirmed

`FUN_0044EE90` normally changes +00/+01 immediately. When the NPC is in a state where immediate rebuild is unsafe, it stores the requested pair in `+04/+05` and sets a deferred-rebuild runtime flag.

A later update path reads `+04/+05` and calls `FUN_004C9EA0`, promoting them into current +00/+01 and rebuilding resources.

Thus these two bytes are the pending/deferred counterpart of the current visual/resource selector.

### +06 forced route-profile selector - HIGH

`FUN_004C0C70` treats values greater than 3 as an explicit override. `FUN_004C75A0` then selects:

```text
CNpc + 0xD64 + entry[6] * 0x0C
```

instead of the normally computed route profile from `FUN_004C0930`.

Script/event command `0x277D` stores values produced by `FUN_004AB620`:

```text
input 1 -> 4
input 2 -> 6
input 3 -> 8
input 4 -> 10
```

Zero disables the override. In the retained 22-save corpus this byte is zero in all sampled 30,800 NPC-record instances, showing that it is an explicit/special override path rather than ordinary scheduling state.

Safe name: `forcedRouteProfile`.

### +08/+09 -> +0C/+0D route endpoints - CONFIRMED

Raw code in `FUN_004C1FE0` writes the source route node to +08/+09 and destination route node to +0C/+0D. One key byte is derived from the node's `+0x4C` float converted to integer and the other from node byte `+1`.

`CNpcRoute::FUN_004D8020(fromA,fromB,toA,toB)` searches a route table using exactly these four key bytes and returns the route metric used to build travel phases.

Therefore these fields are persistent route endpoint keys even though the original symbolic subfield names remain unknown.

### Five 0x14 travel phases - CONFIRMED structure

`FUN_004C7600` builds five identical local phase records and copies them to `entry+0x10..+0x73`.

`FUN_004C10D0` advances `activeTravelPhase` through indices `0..4`, incrementing each active phase's `elapsedTime`. Inactive phases are skipped. `-1` means no current phase.

`FUN_004C7180` computes:

```text
progress = elapsedTime / totalDuration
clamped to 0..1
```

so phase `+08 = totalDuration` and `+0C = elapsedTime` are CONFIRMED.

`FUN_004C0FF0` computes movement scale from:

```text
phase+04 / (phase.totalDuration - phase.elapsedTime)
```

and clamps the result to `0.1..2.5`. During phase creation +00 and +04 are initialized from the route metric. This establishes both as route-distance/cost fields, although the precise distinction between the two remains HIGH rather than fully named.

`FUN_004C7200` constructs the time budget. Normal phases use approximately:

```text
1800 + distance / (npcSpeed * 0.1)
```

while phase index 2 has a special:

```text
3900 + distance / 3.7037039
```

The five phase durations are summed and converted to minutes. This is direct evidence for off-screen NPC route/schedule timing.

### +02 flags - partial taxonomy

Known semantics:

```text
bit0 0x01  route/travel schedule state valid/active       HIGH
bit1 0x02  runtime-presence/unload persistence marker     HIGH, exact friendly name OPEN
bit2 0x04  pending schedule-time gate                     HIGH
bit3 0x08  special linked-NPC interaction state           HIGH, Michael-specific preservation path
```

Evidence for bit2: `FUN_004C6EC0` compares current time-of-day with scheduled source/destination times. While bit2 is set, the state is held before the scheduled threshold and the bit is cleared when that threshold is crossed. Script/event handling can explicitly set the bit.

Evidence for bit3: `FUN_004CA710` toggles it while attaching/detaching a linked NPC, and unload/reset code has special preservation logic for NPC ID 15 (Michael) based on linked actor state.

Bit1 is cleared/recomputed during NPC unload (`FUN_004C8C10`) from `FUN_004C8320` and live NPC mode. It clearly persists a runtime-presence/reconstruction decision, but the exact original semantic name is not yet recovered.

### +74 route-state timestamp - writer CONFIRMED, consumer not found

The field is written after route-state activation/reconfiguration by:

```text
FUN_00452160 -> FUN_00470300
```

`FUN_00470300` computes exactly:

```text
hour*3600 + minute*60 + second
```

and the result is converted to float and stored at `entry+0x74`.

Safe current label:

```text
float routeStateTimeOfDaySeconds
```

No direct reader of this saved field has been found in the reviewed PC decompile, and all retained save samples inspected so far serialize it as zero. It may be a legacy/write-only timestamp or consumed through a path not decompiled into an obvious direct read. Do not claim a stronger role yet.

### Concrete retained-save route examples

Observed populated entries include:

```text
Becky  ID02: route 03,02 -> 03,03; phase0 distance ~300; duration ~3300
George ID05: route 03,08 -> 03,09; phase1 distance ~300; duration ~2953.846
Emily  ID06: route 03,09 -> 03,10; phase1 distance ~300; duration ~2871.429
Isaach ID10: route 03,05 -> 03,06; phase0 distance ~149.39; duration ~2397.57
Sallie ID18: route 03,05 -> 03,06; phase0 distance ~227.07; duration ~2673.35
Isaiah ID34: route 03,05 -> 03,06; phase0 distance ~76.88; duration ~2107.50
```

These independently agree with the off-screen travel/schedule interpretation.

### Revised safe name

The old umbrella name `NamedNpcScheduleState` was too narrow because the record also owns visual/resource selection and deferred visual changes.

Use:

```text
NamedNpcPersistentState namedNpc[50]
```

with a nested `NpcTravelPhase travel[5]` substructure.

Status: **CONFIRMED owner/indexing/static partner/five-phase timing; HIGH fine names for some flag and distance fields**.

## 9. Named-NPC runtime reconstruction: `+0x0E280`

### Table identity - CONFIRMED

```text
record +0x0E280
Game   +0x09A7E8
50 entries * 0x40 = 0x0C80
index = CNpc+0xD54
```

The constructor clears exactly `0xC80` bytes. `FUN_004C08D0` indexes this table with the same named-NPC ID used by the `+0xCB10` table. `FUN_004C7300` snapshots live `CNpc` state into it; `FUN_004D23D0` event `0x23` restores the entry into a reconstructed NPC.

Safe owner/name:

```cpp
NamedNpcRuntimeSnapshot npcRuntime[50];
```

### Recovered layout

```cpp
struct NamedNpcRuntimeSnapshot {               // 0x40
    float position[4];                         // +0x00..+0x0F
    float rotation[4];                         // +0x10..+0x1F
    uint8_t facialExpressionIndex;             // +0x20
    uint8_t reserved21[3];                     // +0x21..+0x23
    uint32_t restoreFlags;                     // +0x24
    uint32_t currentXamAnimationResourceId;    // +0x28
    uint8_t reserved2C[0x14];                  // +0x2C..+0x3F
};
```

### +0x00..+0x1F transform - CONFIRMED

When live `CNpc+0x6A0 bit0x40000` is active, `FUN_004C7300` copies:

```text
CNpc+0x58..+0x67 -> snapshot+0x00..+0x0F
CNpc+0x68..+0x77 -> snapshot+0x10..+0x1F
```

Restore copies them back and mirrors the rotation block into the secondary/current transform fields as well.

Real historical snapshots contain ordinary world coordinates and Euler-like rotations such as yaw `0.4363323`, `-2.216955`, `-2.424465`, strongly confirming position + rotation rather than opaque data or a quaternion-only representation.

### +0x20 facialExpressionIndex - CONFIRMED

Writer condition:

```text
CNpc/CCharacter +0x5D0 bit0x20
```

When active, the writer stores `CCharacter+0x508` into snapshot `+0x20` and sets restore bit `0x02`.

Restore bit `0x02` calls `FUN_004101A0(savedIndex, 0)`. `FUN_004101A0` maintains current/previous channel indices and blend weights; `FUN_00410210` applies the resulting weights to the channel array and `FUN_00410480` advances the blend.

The decisive semantic proof is CEvent opcode `0x42`, whose handler `FUN_0042EA50` calls this same setter with a signed selector and transition duration. In shipped developer event `EVENT/91/0_0455.DSB`, all five opcode-`0x42` commands are inside the routine literally named:

```text
フェイシャル  = Facial
```

The commands select facial indices `29`, `33`, `-1`, `28`, and `39`, each with transition time `30`.

Therefore:

```text
snapshot+0x20 = facialExpressionIndex
```

Status: CONFIRMED.

### +0x28 currentXamAnimationResourceId - CONFIRMED

The writer snapshots the live resource at `CNpc/CCharacter+0x18C`:

```text
live resource object
 -> FUN_006B2E30: find its resource-table entry
 -> resource-ID lookup
 -> snapshot+0x28
```

Restore performs the inverse:

```text
saved ID
 -> FUN_006B2BE0(id,1): resolve resource object
 -> CNpc vtable slot +0x54
 -> FUN_004174C0(resourceObject,...)
```

Direct vtable inspection of the Steam executable gives:

```text
CNpc::vtable +0x50 -> FUN_00417650  // numeric-ID variant
CNpc::vtable +0x54 -> FUN_004174C0  // already-resolved resource-object variant
```

`FUN_00478880` is a thin wrapper around slot `+0x50` and is used by ordinary NPC animation/motion selection. Both variants converge on the same character-animation resource replacement machinery (`FUN_006C0480` / `FUN_006C0620`).

Asset-level proof comes from `NPC_LIST.PRM` + `NPC_LIST.SRL`:

- `NPC_LIST.PRM` stores up to eight animation basenames at row offsets `+0x58, +0x62, +0x6C, +0x76, +0x80, +0x8A, +0x94, +0x9E`;
- the code appends `.XAM` to those names;
- the final `50 * 0x20` block of `NPC_LIST.SRL` contains exactly eight resource IDs per named NPC for the same slots.

Example, George (`npcId 5`):

```text
nc03g001.XAM -> 0x2631
nc03g002.XAM -> 0x2632
nc01g001.XAM -> 0x2570
nc01g002.XAM -> 0x2571
nc01g003.XAM -> 0x2572
nc01g004.XAM -> 0x2573
nc01g005.XAM -> 0x2574
```

A saved George runtime snapshot contains `+0x28 = 0x2574`, matching `nc01g005.XAM` exactly.

This disproves the intermediate `model/body resource` interpretation. The safe final label is:

```text
snapshot+0x28 = currentXamAnimationResourceId
```

Status: CONFIRMED.

### +0x24 restoreFlags - bit mapping CONFIRMED, friendly semantics partially OPEN

Writer clears only `+0x24`; it does **not** clear the whole `0x40` record. Payload bytes may therefore remain stale while their validity bits are clear.

Known bits:

```text
0x001 transform snapshot valid; gates +00..+1F and nested optional fields
0x002 +0x20 facialExpressionIndex valid
0x004 +0x28 currentXamAnimationResourceId valid
0x008 mirrors CNpc+0x6A0 bit0; restore calls FUN_004C6BD0(1)
0x010 mirrors CNpc+0x1C bit0x01000000
0x020 mirrors CNpc+0x698 bit0x40000000
0x040 mirrors CNpc+0x1C bit0x00040000
0x080 mirrors CNpc+0x1C bit0x00080000
0x100 mirrors CNpc+0x1C bit0x00020000
```

The mapping itself is direct-code CONFIRMED. Friendly gameplay names for bits `0x008..0x100` remain OPEN until their live actor consumers are fully classified.

### +0x2C..+0x3F reserved - HIGH / effectively confirmed for reviewed PC path

Across 23 save images * 28 GameRecords * 50 NPC entries = 32,200 inspected entries, no nonzero byte was observed in `+0x2C..+0x3F`. The reviewed PC writer never writes the range and the restore path never reads it.

Safe PC layout:

```text
+0x2C..+0x3F reserved[0x14]
```

### Observed save populations

Across the 32,200 inspected entries, restore flags were overwhelmingly zero. Nonzero values observed were mainly:

```text
0x05 = transform + XAM resource
0x15 = transform + XAM resource + actor flag0x10
0x20 = standalone mirrored actor flag
```

This reinforces the selective-snapshot model: stale payload is expected, and `restoreFlags` is authoritative.

### Architectural result

The two adjacent named-NPC tables now separate cleanly:

```text
record+0xCB10  NamedNpcPersistentState[50]
               appearance selection + off-screen route/schedule progress

record+0xE280  NamedNpcRuntimeSnapshot[50]
               reconstruction-only live transform + facial expression + current XAM animation + actor flags
```

Together they provide both halves of named-NPC persistence: simulation while unloaded and reconstruction state when a live actor is unloaded/recreated.

## 10. NPC world-instance persistence pool: `+0x0EF00`

### Table identity and initialization - CONFIRMED

```text
record +0x0EF00
Game   +0x09B468
256 entries * 0x24 = 0x2400
```

`FUN_00450910` and the GameRecord constructor clear the full `0x2400` bytes and then initialize every entry as:

```text
flags = 0
placementKey = 0xFFFF
```

`FUN_004B0F80(CNpc*)` obtains the NPC's current world-table row and derives a 16-bit identity from the row's `+0x2A` field (row stride `0x3C`). It searches all 256 records for that identity and allocates the first `placementKey == FFFF` entry when absent.

Safe table name:

```cpp
NpcWorldInstanceState npcWorld[256];
```

### Recovered entry layout

```cpp
struct NpcWorldInstanceState {            // 0x24
    uint8_t  persistFlags;                // +0x00
    uint8_t  resourceVariantIndex;        // +0x01  HIGH; CNpcEnemy+0xD2E
    uint8_t  spawnClassCode;              // +0x02  CONFIRMED
    uint8_t  behaviorSubtype;              // +0x03  HIGH; CNpc+0x621
    uint8_t  groupSlotId;                  // +0x04  CONFIRMED role
    uint8_t  weaponItemId;                 // +0x05  CONFIRMED for enemy path
    uint16_t placementKey;                 // +0x06  CONFIRMED; FFFF=unused
    uint32_t mapPresenceMask;              // +0x08  CONFIRMED
    uint16_t routeLocationNodeId;          // +0x0C  CONFIRMED
    int16_t  currentHp;                    // +0x0E  CONFIRMED
    float    position[4];                  // +0x10..+0x1F CONFIRMED live transform block
    float    headingYaw;                   // +0x20  CONFIRMED role / CNpc+0x6C
};
```

### +0x02 spawnClassCode - CONFIRMED with class mapping

The respawn path reconstructs a `0x40` create descriptor from the save entry and passes `entry+0x02` to `FUN_004AB200`. That function maps codes `1..7` into object-factory class IDs; `FUN_005E7620` exposes the concrete constructors:

```text
saved +02 = 1 -> factory 2 -> CNpcNormal
saved +02 = 2 -> factory 4 -> CNpcMob
saved +02 = 3 -> factory 3 -> CNpcEnemy
saved +02 = 4 -> factory 6 -> CNpcDog
saved +02 = 5 -> factory 7 -> CNpcBird
saved +02 = 6 -> factory 5 -> CNpcAnimal
saved +02 = 7 -> factory 8 -> CNpcKiller
```

This is a true spawn/factory class selector, not a generic state byte.

### +0x01 resourceVariantIndex - HIGH

The dynamic writer copies `CNpcEnemy+0xD2E` into `entry+0x01`; restore returns the byte through the spawn descriptor. `FUN_00491730` derives `CNpcEnemy+0xD2E` by comparing the enemy's loaded resource against six resource-pairs and assigns values `1..6` (`FF` when not classified).

Therefore the safe semantic label is `resourceVariantIndex` / `enemyResourceVariantIndex`. Exact friendly names for variants 1..6 remain OPEN.

### +0x03 behaviorSubtype - HIGH

The writer copies `CNpc+0x621`; the base NPC descriptor restore places the corresponding byte back into the same field. This byte has substantial AI/gameplay branching with values in the `0..6` range and is initialized from a world/NPC definition field. For enemy actors it selects behavior/animation/combat families; a special `CEN0811.XMD` path forces value `5`.

Safe name: `behaviorSubtype`. Do not assign human-facing subtype names until the subtype table is fully classified.

### +0x04 groupSlotId - CONFIRMED role

The writer copies `CNpc+0x61A`. The field is used as an index into a 64-entry (`0x40`) CNpcMain group-state/timer table; event code compares actors by this byte and applies linked state changes to all NPCs sharing the same value.

Safe name: `groupSlotId` / `npcGroupId`.

### +0x05 weaponItemId - CONFIRMED for CNpcEnemy

The dynamic writer copies `CNpcEnemy+0xD28`. Enemy initialization first reads a compact weapon selector and then `FUN_0048D7D0` normalizes it to real item IDs:

```text
0 -> FF (none)
1 -> 0x1B
2 -> 0x03
3 -> 0x0F
4 -> 0x09
5 -> 0x0A
6 -> 0x0B
7 -> 0x0D
8 -> 0x1F
9 -> 0x08
```

These are IDs in the normal weapon portion of `ITEM.PRM`, and later enemy combat code switches directly on the normalized values. Therefore `entry+0x05` is safely `weaponItemId` for the enemy respawn path.

### +0x06 placementKey - CONFIRMED

`FUN_004B0F80` derives this key from the active world/NPC placement-table row (`row+0x2A`, stride `0x3C`), searches the 256-entry pool by exact equality, and treats `FFFF` as free/unused.

This is not the named-NPC ID (`0..49`); it identifies the world placement/table record.

### +0x08 mapPresenceMask - CONFIRMED

The dynamic writer copies `CNpc+0x6A8`. That field is initialized from the world row's `+0x2C` DWORD. `FUN_0047D490` tests it against the current map bit:

```text
mask == FFFFFFFF -> always allowed
otherwise        -> (1 << currentMapBit) & mask
```

Safe name: `mapPresenceMask` / `mapAvailabilityMask`.

### +0x0C routeLocationNodeId - CONFIRMED

The writer copies `CNpc+0x630`. Route helpers `FUN_004D8980` / `FUN_004D89C0` resolve this 16-bit ID to world position and heading, and CNpc paths prefer it over ordinary table placement when present.

Safe name: `routeLocationNodeId`.

### +0x0E currentHp - CONFIRMED

The writer copies `CNpc+0x66E`; damage/healing paths mutate the same signed 16-bit field, default ordinary NPC initialization writes `100`, and restore Event `0x23` copies a positive saved value back into `CNpc+0x66E`.

Safe name: `currentHp`.

### +0x10..+0x1F position[4] and +0x20 headingYaw - CONFIRMED

The writer copies:

```text
CNpc+0x58,+0x5C,+0x60,+0x64 -> entry+0x10..+0x1F
CNpc+0x6C                   -> entry+0x20
```

The dynamic restore reconstructs the create descriptor with the same values. The four-DWORD block is the base world-position vector; `CNpc+0x6C` is the heading/yaw component used by the orientation path. Retained save entries contain ordinary world coordinates and radian-like heading values.

### persistFlags +0x00 - confirmed mechanics, partial friendly semantics

Directly established bits:

```text
0x04  persists inherited CObject event-mask bit0; restore re-applies generic Event 0x0C mask bit1
0x20  mirrors live CNpc+0x69C bit0x10
0x40  marks a complete runtime respawn/reconstruction entry
0x60  runtime respawn entry with the mirrored 0x20 actor bit also set
```

Other bits (`0x01`, `0x02`, `0x10`, `0x80`) are genuine persistent NPC behavior flags manipulated through `FUN_004B1060/1080/10A0`. Their control-flow roles are confirmed but exact human-friendly labels remain OPEN; do not invent names from one state-machine branch.

### Logical pool partition

`FUN_004B1560` explicitly clears entries `100..127` (`0xE10..0x11FF` within the pool) before building runtime respawn records, then writes eligible live actors starting at entry 100. Restore scans all 256 entries and recreates actors whose `persistFlags & 0x40` is set.

The keyed allocator `FUN_004B0F80` itself scans all 256 entries, so the safest description is:

```text
0..99      normal intended keyed-persistence region
100..127   explicitly recycled runtime-respawn scratch/persistence region
128..255   spare capacity; generic allocator can theoretically reach it
```

Do not call `128..255` hard padding: the allocator's search range includes them.

### Save-corpus census

Across 23 complete saves * 28 records = 644 GameRecord images:

- 328 meaningful `npcWorld` entries were found;
- meaningful entries occupied only indices `0..74`;
- no retained sample used indices `>=75`;
- no retained sample contained a `0x40/0x60` runtime-respawn entry;
- observed persisted flags were `0x00` and `0x04` only;
- `+0x01/+0x02/+0x04/+0x05` were zero in the retained keyed entries, as expected because those bytes primarily matter to reconstruction descriptors;
- one current/live `record0` sample (`chapter23.sav`) contained 22 keyed entries; most other record0 snapshots had the pool empty while historical snapshots preserved earlier populated states.

This demonstrates that the normal disk-save population is dominated by keyed world-placement persistence; the richer respawn descriptor mode is real engine functionality but comparatively situational in the retained corpus.

### Revised status

`record+0x0EF00` is now structurally **CONFIRMED** and field-level semantics are largely closed. Remaining uncertainty is limited to friendly names for a few low-byte behavior flags and the exact labels of the `resourceVariantIndex` / `behaviorSubtype` enums, not the purpose or layout of the table itself.

## 11. NPC spawn-rule persistent overlay: `+0x11300`

### Table identity - CONFIRMED

```text
record +0x11300
Game   +0x09D868
size   0x400 = 256 * 4
```

`FUN_004B0F60(index)` returns `Game+0x9D868 + index*4`. The table is consumed by `CNpcMain` while rebuilding its live 0x14-byte spawn-rule entries at `CNpcMain+0xF88` in `FUN_004B4F00`.

The live spawn-rule table is data-driven and controls whether/when NPCs are spawned. The persistent 4-byte record stores only the subset of live rule state that survives rebuild/load.

Safe name:

```cpp
NpcSpawnRulePersistentState npcSpawnState[256];
```

### Recovered entry layout

```cpp
struct NpcSpawnRulePersistentState {   // 0x04
    uint8_t flags;                     // +0x00
    uint8_t defeatCount;               // +0x01
    uint16_t reserved;                 // +0x02..+0x03
};
```

### flags bit0 -> live rule flag 0x08: forced/range-bypass spawn - HIGH

`FUN_004B38D0(ruleKey, 8)` sets live rule flag `0x08` and, on the normal gameplay path, persists save bit0. During table rebuild, saved bit0 restores live flag `0x08`.

In `FUN_004B59C0`, live flag `0x08` bypasses the ordinary distance/vertical gating before attempting the spawn. The producer `FUN_004B3AB0` applies it to selected data-driven world entries.

Safe semantic label: `forceSpawn` / `ignoreNormalDistanceGate`.

### flags bit1 -> live rule flag 0x02: spawn suppression - CONFIRMED role

`FUN_004B39A0(ruleKey,on)` sets/clears both live flag `0x02` and saved bit1. Rebuild maps saved bit1 back to live flag `0x02`.

`FUN_004B59C0` only executes the ordinary spawn attempt when live bit `0x02` is clear:

```text
if ((ruleFlags & 0x02) == 0) { ... attempt spawn ... }
```

Therefore this persistent bit is safely a spawn-disabled/suppressed state. An object-event path toggles it on events `0x0C/0x0D`.

### flags bit2: linked spawner/controller removed - HIGH / direct role confirmed

Saved bit2 is controlled by `FUN_004B3940`. On rebuild, if it is set, `FUN_004B4F00` calls `FUN_004B46B0(ruleKey,1)`.

`FUN_004B46B0` finds the live world object of type `0x99` with matching key at object `+0x458`, calls its removal/destruction virtual path, and persists bit2 again.

Safe semantic label: `linkedSpawnerRemoved` / `spawnControllerConsumed`. Exact original class name for object type `0x99` remains OPEN.

### +0x01 defeatCount - CONFIRMED source, HIGH full policy

`FUN_004925C0`, reached from the `CNpcEnemy` death/finalization path, looks up the live spawn rule by `CNpc+0x630`, increments the live entry byte at `+0x0D` up to 100, then mirrors the result to saved entry `+0x01` through `FUN_004B0F60`.

The live rule's byte `+0x0C` is a threshold (data-driven; default path uses 5). When `defeatCount >= threshold`, the linked type-`0x99` spawn controller is removed through `FUN_004B46B0`.

Therefore the persistent byte is safely a per-rule enemy defeat/kill count. The exact design label of the threshold remains OPEN.

### +0x02..+0x03 reserved - HIGH / effectively confirmed for shipping PC

No Steam or GOG PC code path found writes or reads bytes 2 or 3 of the persistent 4-byte record. The only accessor returns the 4-byte entry base; all reviewed users address byte0 or byte1 only.

Across the retained save corpus:

```text
23 dp.sav images * 28 GameRecords = 644 records
256 entries per record
164,864 inspected entries
```

all four bytes of every `+0x11300` entry were zero. Thus the table is a real but rarely exercised persistent mechanism, not padding; bytes `+02..+03` are nevertheless unused/reserved in the reviewed shipping PC paths.

### Architectural relation to the live table

The live `CNpcMain` rule entries are 0x14 bytes and contain substantially more transient data, including rule key, map mask, threshold/counter, category/group bytes and runtime flags. `+0x11300` is intentionally only a compact persistence overlay:

```text
persistent flags  -> selected live spawn-rule flags
persistent count  -> live defeat/progress count
reserved          -> no observed PC use
```

The live table storage is over-provisioned (up to 0x1000 0x14-byte entries), while the save overlay reserves 256 four-byte entries. In the reviewed corpus no persistent entry is nonzero, so no evidence exists that shipping content exceeds the persistent overlay capacity.

Status: table owner/role/layout CONFIRMED; exact original symbolic names for bit0 and bit2 remain HIGH/OPEN.

## 12. Generic world-object persistent registry: `+0x11700..+0x3E6FF`

### Table identity - CONFIRMED

```text
record +0x11700
Game   +0x09DC68
0x1200 / 4608 entries * 0x28 = 0x2D000 bytes
end = record+0x3E700
```

Steam and GOG PC builds use the same Game-relative offsets and entry size.

Constructor initialization for every entry is:

```text
+00 = FF
+01 = FF
+02 = FFFF
+04..+27 = 0
```

`FUN_00453D80` is the allocate/find helper. It searches by the triple stored at `+00/+01/+02`; if no match exists it allocates the first entry whose `+02 == FFFF` and writes the current map-context pair plus the supplied placement key. `FUN_00453E40` performs the corresponding lookup from a live world object by deriving the same placement key from the active world row (`row+0x2A`, stride 0x3C). `FUN_00455A80` clears a complete entry back to the canonical unused representation.

Safe key interpretation:

```cpp
int8_t   mapId;          // +00, FF unused
int8_t   areaId;         // +01, FF unused
uint16_t placementKey;   // +02, FFFF unused
```

The exact original C++ symbolic names for the first two map-context bytes are not recovered, but they are the same current map/area selectors used broadly by PC world/event code. `placementKey` is data-driven and comes from the current world-placement row rather than from a runtime object pointer.

### Recovered structural layout

The important correction is that `+08..+27` is a **generic class-specific payload union**, not permanently a transform structure.

```cpp
struct WorldObjectPersistentState {          // 0x28
    int8_t   mapId;                          // +00
    int8_t   areaId;                         // +01
    uint16_t placementKey;                   // +02
    uint32_t stateFlags;                     // +04

    union {                                  // +08..+27, 0x20 bytes
        uint32_t raw[8];

        struct {
            float position[4];               // +08..+17
            float rotation[4];               // +18..+27
        } transform;
    } payload;
};
```

Status: key, flags field, union size and transform overlay are CONFIRMED. Individual class-specific payload interpretations remain intentionally local to their owning object class.

### Common transform overlay - CONFIRMED

Flag `0x04000000` is the common transform-payload valid marker.

Many unrelated world-object classes use exactly the same save path:

```text
live object +0x58..+0x67 -> entry +0x08..+0x17
live object +0x78..+0x87 -> entry +0x18..+0x27
entry stateFlags |= 0x04000000
```

and the corresponding construction/load path copies these fields back when the bit is present.

The first block is the standard world-position vector. Retained saves overwhelmingly show homogeneous values such as:

```text
X, Y, Z, 1.0
```

The second is the object's orientation/rotation vector. Retained saves show Euler-like radians and its fourth component is zero in the reviewed corpus, e.g.:

```text
0, 1.5708, 0, 0
```

Some object classes save/restore only the position half while using the same generic registry machinery, so the payload must still be treated as a union rather than a mandatory full transform.

### The payload is genuinely class-specific - CONFIRMED

Low state bits do not imply transform semantics. Concrete examples exist where `stateFlags == 1` and only `payload+0x00` is meaningful. One shipped object path stores a `uint16` angle at entry `+0x08` (commonly `90`) while toggling flag bit 0.

Other object classes use bits `1/2/4/8/...` as independent persistent latches and read no transform data at all. Therefore do not globally name `+08` as position unless `0x04000000` or the owning class's save contract proves transform usage.

### stateFlags: common infrastructure vs class-local state

Two bits have cross-class infrastructure semantics strong enough for global labels:

```text
0x02000000  live/resident-instance marker - HIGH
0x04000000  transform payload valid        - CONFIRMED
```

`0x02000000` is set by many unrelated world-object initialization paths once a keyed persistent entry is associated with the live object. Event/callback case `0x16` clears it, and map-reset cleanup code specifically recognizes entries carrying the bit. Safe label: `liveInstanceMarker` / `residentMarker`. The exact original engine name remains unknown.

`0x04000000` directly gates transform restoration and is set immediately after writers copy object transform fields to the payload.

The rest of `stateFlags` is intentionally **not** assigned one universal gameplay meaning. Different object families use the same DWORD as their persistent state word. Confirmed patterns include:

```text
low bits 0x1 / 0x2 / 0x4 / 0x8 / 0x10
    class-specific multi-state latches

0x00100000 .. 0x01000000
0x08000000
0x10000000 / 0x20000000 / 0x40000000 / 0x80000000
    additional class-specific persistent modes/flags
```

For example one object family uses bits `0x10000000`, `0x20000000`, and `0x40000000` as three mutually exclusive visual/state modes; another family uses low bits as independent switches. Treat these as `classStateFlags`, not as global `destroyed/opened/...` bits.

### Retained-save census

Dataset:

```text
23 dp.sav images
* 28 GameRecords
= 644 GameRecord images
```

Across these records:

```text
non-empty WorldObjectPersistentState entries: 99,701
unique key triples across corpus:              705
maximum populated entries in one GameRecord:   513 / 4608
highest slot index observed:                   519
records with duplicate key triples:            0
```

Thus the table behaves exactly as the code suggests: a unique keyed registry with very large spare capacity, not a fixed semantic array of 4608 predetermined object types.

Most common exact flag values in the corpus:

```text
0x04000000  45,468   transform-valid entries
0x00000000  27,846
0x08000000  11,558
0x00000002   4,258
0x00000001   2,542
0x0A000000   2,181
0x00200000   1,730
0x02000000   1,602
0x00100000     579
0x04000002     401
```

Transform-bearing entries commonly contain real world coordinates and Euler-like orientation values. `entry+0x24`, the fourth rotation component, remained zero throughout the retained corpus, consistent with the engine's 4-float storage convention rather than a separately meaningful scalar.

A handful of keyed entries retain nonzero payload with no current validity/state bit. This is consistent with the broader engine pattern of stale payload bytes being left in place while flags determine whether a field is authoritative. Consumers must therefore obey `stateFlags` / class contract rather than infer validity from nonzero payload alone.

### Allocation / clear behavior

- unused entry: `mapId=FF, areaId=FF, placementKey=FFFF, stateFlags/payload=0`;
- allocation uniqueness is `{mapId, areaId, placementKey}`;
- free-slot test is `placementKey == FFFF`;
- whole-entry clear resets the complete `0x28` bytes to the canonical unused form;
- location reset can clear entries belonging to a selected map/area pair;
- the table is serialized as part of the raw `0x45CC0` GameRecord, so all class-local state survives ordinary save/load and historical snapshots automatically.

### Architectural interpretation

This registry is the generic persistence substrate beneath many world-object classes:

```text
world placement row
    -> {mapId, areaId, placementKey}
    -> WorldObjectPersistentState
       + classStateFlags
       + optional standard transform snapshot
       + class-specific 0x20-byte payload
```

It should not be modeled as one giant typed object array. The correct model is a **keyed variant/union registry** shared by multiple gameplay object families.

Status: **CONFIRMED architecture and layout; common transform contract CONFIRMED; live-marker HIGH; individual class-specific flag/payload meanings remain local to their object classes and are not required to decode the top-level dp.sav structure.**

## 13. World/tail registries after `worldState`

The old anonymous registry list is now mostly resolved. These tables all live directly after the 4608-entry generic world-object registry and use compact keyed records rather than the generic `0x28` payload format.

### `+0x3E700`: door-family persistence - HIGH owner / CONFIRMED layout

Runtime base: `Game+0xCAC68`.

256 entries x `0x08`:

```text
+0  int8/uint8 mapA
+1  int8/uint8 mapB
+2  uint16 local object/door ID, 0xFFFF unused
+4  int16 door-local state / angle-like value
+6  uint16 persistent flags
```

`FUN_00453FC0` searches/allocates this registry by current location tuple + local ID. `FUN_004540B0/D0/F0` test/set/clear the `+6` flags. Direct consumers belong to the `N_SDOOR`/door choreography family; one consumer converts `+4` from degrees with the standard `0.017453292` factor.

Safe semantic name: `DoorPersistentState[256]`.

### `+0x3EF00`: normal CObjectItem removal set - CONFIRMED

Runtime base: `Game+0xCB468`.

512 records x `0x04`:

```text
+0 mapA
+1 mapB
+2 uint16 world-item spawn ID
```

`FUN_004544E0` records a removed/picked `CObjectItem` spawn here when its item category is not one of the special category-2/category-4 groups. `FUN_00454410` checks this set during object reconstruction to suppress respawn.

Safe name: `NormalWorldItemRemoved[512]`.

### `+0x3F700`: special/key CObjectItem removal set - CONFIRMED

Runtime base: `Game+0xCBC68`.

64 records x `0x04`, same key layout as above. Chosen when the removed item's `ITEM.PRM +0x2C` category is `2` or `4`.

Current category mapping makes this specifically the story/key-item category plus Profiling Pieces, rather than a generic quest-item guess.

Safe name: `SpecialWorldItemRemoved[64]`.

### `+0x3F800`: `CPut` placement persistent overlay - CONFIRMED owner/restore contract

Runtime base: `Game+0xCBD68`.

256 records x `0x08`:

```cpp
struct CPutPersistentOverride { // 0x08
    uint8_t  mapId;             // +0x00
    uint8_t  areaId;            // +0x01
    uint16_t placementId;       // +0x02, 0xFFFF unused
    uint8_t  type3State;        // +0x04
    uint8_t  reserved05;        // +0x05, no reviewed PC writer/restore consumer
    uint16_t placementFlags;    // +0x06, mirror of CPut placement-row word +0x28
};
```

`FUN_00454370` searches/allocates the record by the current map/area tuple plus `placementId`. `FUN_00455720` writes the `+0x04` state byte and `FUN_00455740` writes the `+0x06` word.

The owner is now direct: CEvent opcode `0x9D` resolves a `CPut` placement and mirrors the same live change into this registry. For placement type `3`, the saved `+0x04` byte mirrors the type-3 payload state. For other placements, `+0x06` stores the resulting complete `CPut` row word at `+0x28`.

`FUN_00454110` is the restore consumer. After a `CPut` table is freshly loaded/built, it walks all 256 saved records, resolves the matching placement, and reapplies either the type-3 byte or the saved `+0x28` word. `FUN_005E4A20` invokes this restore overlay at the end of CPut-table construction.

Safe name: `CPutPersistentOverride[256]`. The `+0x05` byte remains `HIGH: reserved/unused in reviewed PC paths`; the rest of the ownership and restore contract is CONFIRMED.

### `+0x40000`: persistent dropped-world-item registry - CONFIRMED

Runtime base: `Game+0xCC568`.

128 records x `0x10`:

```text
+0x00 float x
+0x04 float y
+0x08 float z
+0x0C int32 item/resource ID, -1 unused
```

`FUN_00450C70` allocates/registers entries. `FUN_005EBEE0` iterates the complete table after restore and calls CItemManager to recreate each dropped item at the stored world position. `FUN_00450CE0` clears a slot.

Safe name: `DroppedWorldItem[128]`.

### `+0x40800`: CLight persistence - CONFIRMED

Runtime base: `Game+0xCCD68`.

128 records x `0x08`:

```text
+0 mapA
+1 mapB
+2 uint16 light/resource ID, 0xFFFF unused
+4 uint8 enabled/state, default 1
+5 uint8 reserved/secondary state, default 0
+6 uint16 intensity/parameter-like value, default 100
```

`CLight` constructor is `FUN_005F2EF0`. Its event callback uses the registry directly: events `0x6A/0x6B` persist on/off through `+4`, while event `700` persists the `+6` light parameter.

Safe name: `LightPersistentState[128]`.

### `+0x40C00`: first-visit world-placement registry - CONFIRMED mechanism / HIGH friendly key name

Runtime base: `Game+0xCD168`.

128 records x `0x04`:

```text
+0 mapA
+1 mapB
+2 uint16 scene/location-entry ID, 0xFFFF unused
```

`FUN_004542C0` searches and inserts these keys. `CPlayer` initialization calls it with the current scene/map entry ID; only on a previously absent key does the code increment the related statistic and salary action. Thus this is a persistent first-visit set.

The caller obtains the 16-bit key from the active world-placement row at `row+0x2A` (stride `0x3C`) and records it only on the first unseen visit. On first insertion, the Player initialization path increments the associated statistic/salary action.

Safe current name: `VisitedWorldPlacementEntry[128]` / `VisitedSceneEntry[128]`. The mechanism and data source are CONFIRMED; only the original friendly name of the row key remains OPEN.

### `+0x40E00` and `+0x41000`: CCar availability namespaces - CONFIRMED

Both regions store the same four-byte schedule structure:

```cpp
struct VehicleAvailabilityWindow { // 0x04
    uint8_t startHour;
    uint8_t startMinute;
    uint8_t endHour;
    uint8_t endMinute;
};
```

`CCar::availability` path `FUN_005B1B30` passes `CCar+0x494` to `FUN_00453A70`, treats an all-zero window as always available, and otherwise converts the two HH:MM pairs to seconds and checks the current game time through `FUN_00470500`.

The important correction is that the two backing regions are **two vehicle resource-index namespaces**, not a live table plus a generic fallback:

```cpp
// record+0x40E00, runtime Game+0xCD368
VehicleAvailabilityWindow genericVehicleAvailability[128];

// record+0x41000, runtime Game+0xCD568
VehicleAvailabilityWindow mapSpecificVehicleAvailability[13][32];
```

`FUN_00453A70(index)` routes by the current map selector (`Game+0x834FE0`):

```text
map  1 -> bank  0
map  2 -> bank  1
map  3 -> bank  2
map  4 -> bank  3
map  5 -> bank  4
map  6 -> bank  5
map  8 -> bank  6
map  9 -> bank  7
map 10 -> bank  8
map 11 -> bank  9
map 40 -> bank 10
map 41 -> bank 11
map 42 -> bank 12
all other maps -> generic table
```

Thus the accessor is exactly:

```text
special-map set -> +0x41000 + (bank * 32 + index) * 4
all other maps  -> +0x40E00 + index * 4
```

The size boundary independently matches the routing exactly:

```text
+0x40E00..+0x40FFF = 0x200 = 128 * 4
+0x41000..+0x4167F = 0x680 = 13 * 32 * 4
+0x41680           = start of the separately established reserved span
```

The reason for the split is confirmed by the independent CCar resource-presence path `FUN_005B1A00`. It uses the **same 13 map values** to select dedicated vehicle resource families:

```text
map  1 -> resource type 0x54
map  2 -> 0x55
map  3 -> 0x56
map  4 -> 0x57
map  5 -> 0x58
map  6 -> 0x59
map  8 -> 0x5A
map  9 -> 0x5B
map 10 -> 0x5C
map 11 -> 0x64
map 40 -> 0x5D
map 41 -> 0x5E
map 42 -> 0x5F
all other maps -> generic resource type 0x48
```

Steam and GOG implement the same mapping. On the special-map resource lookup, nonzero `Game+0x834FE4` also adds `0x10` to the resource index; that is a resource-addressing detail and should not be projected onto the schedule table itself, because `FUN_00453A70` indexes the 32-entry schedule bank directly with the supplied index.

`FUN_00452270` closes the generic side independently: it scans indices `0..127`, queries resource type `0x48`, validates the corresponding vehicle object, and returns `Game+0xCD368 + index*4`. Therefore `+0x40E00` is specifically the shared/generic type-`0x48` vehicle namespace.

The script writer is CEvent opcode `0xAC` (`FUN_00437F90`, dispatch case `-0x54`). It parses a vehicle index and four time bytes and writes those bytes through `FUN_00453A70`, so the same map-dependent namespace selection is used by both the script writer and the CCar reader.

A direct census of the retained `dp.sav` confirms that both physical regions are populated in real saves; map-bank entries and generic entries can coexist in one record.

Safe descriptive names:

```text
+0x40E00 genericVehicleAvailability[128]
+0x41000 mapSpecificVehicleAvailability[13][32]
CCar+0x494 vehicleResourceIndex / vehicleScheduleIndex
```

The original source-level member names remain unknown, but the structure, routing rule, map-bank set, resource-namespace reason for the split, writer and CCar consumer are CONFIRMED.

## 14. Weather, CEvCore and script/event persistence

### `+0x41680..+0x417FF`: reserved/unused candidate - STRONGLY_SUPPORTED

Size: `0x180` bytes.

Evidence:

- zero in record0 for every retained PC chapter save from chapter6 through chapter26 and Finished, plus the standalone `dp.sav` sample;
- no direct displacement/literal reference found in Steam decompile or raw asm for runtime range `Game+0xCDBE8..+0xCDD67`;
- constructor only initializes it through the whole-record zero-fill and has no explicit initializer.

This is retained as `STRONGLY_SUPPORTED` rather than historical-source-code `CONFIRMED` only because a fully computed-pointer consumer cannot be formally disproven.

### `+0x41800`: CWeather persistent state - CONFIRMED owner

The block beginning at `record+0x41800` is consumed by `CWeather`. `FUN_004740A0` restores the saved bytes/words/floats into weather runtime fields and uses them to reconstruct the active precipitation/environment configuration. Direct fields include weather type/state bytes, an enable/state flag, a float/parameter copied into the weather setup, and additional coefficients.

Owner and persistence role are CONFIRMED; a byte-perfect field-by-field structure is still in progress.

### `+0x4180C` / `+0x41810`: independent script-tunable floats

These are not part of the direct `CWeather` state block.

- `+0x4180C`: default `300.0f`; CEvent command `0x8F`, subcommand `0x1A`, writes it. The active-player-car state-87 path compares a per-car stationary timer (`car+0x8D4`) against this value while `abs(car+0x13E4)` is near zero; `car+0x13E4` is the vehicle speed/average axle-speed field and the timer advances by `gameDelta60`. Reaching the threshold creates the periodic vehicle event/object and resets the timer. Therefore this field is a **stationary/idle vehicle event interval in 60-Hz units**. Default `300.0` is 5 seconds; shipped EVENT content also sets `1800.0` (30 s) and `3600.0` (60 s). **CONFIRMED role**.
- `+0x41810`: default `50.0f`; script command case `0x33` writes a positive float; event/interaction code uses it as a distance threshold and falls back to `50.0f` when the value is `<= 1`. **HIGH: event interaction distance threshold**.

### `+0x41814`: CEvCore bit-`0x2000` mirror - CONFIRMED

Save checks `CEvCore+0x04 & 0x2000` and stores nonzero at `record+0x41814`; load restores the bit from this DWORD.

### `+0x41818..+0x41887`: CEvCore persistent snapshot - CONFIRMED

Exactly `0x70 = 28 DWORD` bytes.

`FUN_00430A10` calls `FUN_0072A470` with this record address while ECX is the CEvCore singleton, copying:

- `CEvCore+0x08..+0x24` = 8 DWORD;
- `CEvCore+0xA0..` = 20 DWORD.

`FUN_004309B0` uses `FUN_0072A4E0` in the reverse direction during restore.

Safe name:

```text
CEvCorePersistentSnapshot[0x70]
```

`LD_STATE.PRM` participates in event/load-state initialization but is not this snapshot. Its loader (`FUN_005DFB40`) constructs 64 runtime flag DWORDs from 13 data-driven 12-byte records based on map/area/state selectors.

### `+0x41888`, `+0x41A88`, `+0x41C88`: CEvent routine-history registries - CONFIRMED

`FUN_00453270` manages three statically reached tables, each containing 128 keys:

```cpp
struct EventRoutineKey { // 0x04
    uint16_t eventId;    // numeric DSB/event ID
    uint16_t routineId;  // routine-local key from the DSB routine descriptor
};
```

The key interpretation is direct: values match shipped `EVENT/**/*.DSB` identities, including the helper's special selector-0 case `(0x013E,0)`, which resolves to event `0_0318.DSB` routine `[PolD167]`.

#### `+0x41888`: CEvent `0xC6` checkpoint-capture history - CONFIRMED

`FUN_00439560` calls `FUN_00453270(selector=0,eventId,routineId)`. On first occurrence it prepares the native GameRecord snapshot path (`FUN_004524C0(2,...)`) and copies the complete live `0x45CC0` record to the dedicated runtime backup at `Game+0x0BE8`. It also retains the originating event/routine in CEvent state. Repeated keys are suppressed. The special `(0x013E,0)` key is intentionally made repeatable by the helper under the normal flag condition.

Safe name: `checkpointCaptureRoutineHistory[128]`.

#### `+0x41A88`: CEvent `0xC2/0` autosave/checkpoint history - CONFIRMED

`FUN_00439160` handles opcode `0xC2`. Operand `0` deduplicates through selector `1`; a repeated event/routine returns immediately. On first occurrence the handler records the current event/routine at `record+0x427EC/+0x427EE`, sets the resume/transition flag (`globalGameplayFlags & 0x01000000`), and enters the native save/transition state-machine. Shipped content contains routines literally named `オートセーブ` (Auto Save), plus several `保存` save-marked development/story routines. Operand `1` is a rare special/forced variant that bypasses this history table and sets a CEvent-local flag instead.

Safe name: `autoSaveRoutineHistory[128]`.

#### `+0x41C88`: CEvent `0xCD` chapter-history capture history - CONFIRMED

`FUN_00430660` handles opcode `0xCD` and deduplicates through selector `2`. On first occurrence it stores the same resume `eventId/routineId` at `record+0x427EC/+0x427EE`, sets the resume/transition flag, and calls `FUN_00452720`.

`FUN_00452720` performs the full native snapshot preparation and then copies the complete live GameRecord into the 27-entry historical archive at:

```text
Game+0xD2228 + historySlot * 0x45CC0
```

The slot rule is:

```text
record+0xA0 (episode) == 0  -> historySlot 0   // prologue/initial special case
otherwise                  -> historySlot = record+0xA1 (globalChapterIndex) + 1
```

The archive is exactly `27 * 0x45CC0 = 0x75C840` bytes and is serialized/restored as the historical `GameRecord` images behind live `record0`. This directly explains the retained-save observation that a previous chapter's `record0` later appears, with only small transition adjustments, in the corresponding historical record.

Shipped content uses `0xCD` at destination changes, chapter/scene startup points and related progression boundaries; all 28 reviewed `CD` commands carry the normal zero operand.

Safe name: `chapterHistoryCaptureRoutineHistory[128]`.

The three registries are therefore separate one-shot domains, not interchangeable generic pair sets:

```text
+0x41888  C6  runtime checkpoint capture history
+0x41A88  C2  autosave/checkpoint transition history
+0x41C88  CD  chapter-history archive capture history
```

### `+0x42088`: saved event-selected audio cue override - HIGH

Runtime address `Game+0xCE5F0`; the physical field is a `uint16`, not a DWORD.

`FUN_0046D3A0` resolves a numbered audio cue, applies per-cue volume scaling and creates playback through the audio backend. Event/script code around `FUN_00438xxx` passes its cue parameter to `FUN_0046DEB0` and stores the same short at `+0x42088`.

During sound-manager reinitialization, if global gameplay flag bit `0x20` is active, the saved short is replayed at full gain through `FUN_0046D3A0`; otherwise normal location-derived audio is selected through `FUN_0046B580` and started at half gain. Mode 7 of `FUN_0046DEB0` clears the saved short and sets bit `0x20`; mode 8 clears the bit.

Safe current label:

```text
+0x42088 uint16 savedEventAudioCueId
```

A narrower `BGM` label remains intentionally withheld until the cue category/table is directly identified.

### `+0x42090`: NPC conversation event/routine history - CONFIRMED

This region contains 128 persistent event/routine keys:

```cpp
struct EventRoutineKey { // 0x04
    uint16_t eventId;
    uint16_t routineId;
};

EventRoutineKey npcConversationRoutineHistory[128];
```

`FUN_0044F720` appends a pair into the first all-zero slot; `FUN_0044F760` tests membership. Static xrefs leave one real writer path and one real reader path.

The writer is inside `FUN_004471A0`, the shared data-driven interaction/event resolver. When an interaction descriptor matches and the incoming interaction/action code is `0x42`, the resolver records descriptor fields `+0x12/+0x14` through `FUN_0044F720`. The same descriptor pair is passed as `eventId/routineId` to `FUN_0043C140` on the direct special-event path, establishing the pair domain.

The reader path is `FUN_0047E5F0`. It calls `FUN_00439B40` to resolve the relevant descriptor for the candidate NPC, obtains the same two `uint16` values, tests them with `FUN_0044F760`, and changes the interaction availability/classification when the pair has already been seen. `FUN_004AFCC0` reaches this reader after scanning the live NPC set and selecting the nearest eligible NPC, tying the registry to NPC interaction rather than generic world-object persistence.

A direct census of the retained `dp.sav` sample found the following unique nonzero keys in this table:

```text
(152,12) -> EVENT/35/0_0152.DSB  :ConD01: branch 0, in front of Isaac
(220,6)  -> EVENT/06/0_0220.DSB  :PolD16: pre-conversation check
(372,12) -> EVENT/10/0_0372.DSB  [EX conversation] Emily / Hospital / condition
(363,27) -> EVENT/00/0_0363.DSB  [EX conversation] George / Hospital [P]
(363,28) -> EVENT/00/0_0363.DSB  [EX conversation] Emily / Hospital [P]
```

The `eventId` is the numeric DSB ID and `routineId` is the routine-local key stored in the routine descriptor, matching the interpretation already recovered for the adjacent CEvent history registries. The real saved keys are all conversation-related, including explicit `EX会話` (EX conversation) routines.

Safe name: `npcConversationRoutineHistory[128]`. The original source-level label for interaction/action code `0x42` remains unknown, but the persistence domain and runtime role no longer do.

### `+0x42290..+0x4258F`: reserved/unused candidate - STRONGLY_SUPPORTED

Size: `0x300` bytes.

Evidence:

- all bytes are zero in all retained PC chapter6..chapter26/Finished record0 samples and the standalone `dp.sav`;
- no direct static references were found;
- construction reaches the region only through whole-record zero-fill.

## 15. Fishing, achievement progression and player status

### `+0x42590..+0x425B3`: CFishing persistent block - CONFIRMED owner

The leading counters are exact:

```text
+0x42590 uint16 fishingCounterA  // getter FUN_0060FB80; increment FUN_0060FB90
+0x42592 uint16 fishingCounterB  // increment FUN_0060FBD0
+0x42594 uint16 fishingCounterC  // increment FUN_0060FC00
+0x42596 uint16 fishingCounterD  // increment FUN_0060FF10; also salary action 0x13
+0x42598 uint8 fishingStateA     // mirror CFishing+0x634
+0x42599 uint8 fishingStateB     // mirror CFishing+0x638
+0x4259A uint8 fishingStateC     // mirror CFishing+0x640
```

`FUN_0060FC60` snapshots the three bytes from the live `CFishing` object and `FUN_00614E10` restores them. The four counters are incremented by distinct fishing-state outcomes.

The fourth counter has been narrowed further: `FUN_0060FF10` is reached only after the caught item/reward path is accepted and immediately dispatches salary action `0x13` through `FUN_00450460`.

Safe current refinement:

```text
+0x42596 uint16 fishingSuccessfulRewardCount   HIGH / near-CONFIRMED
```

The first three counters remain outcome-specific but not fully named:

```text
+0x42590 fishingCounterA  // also gates the first-time/tutorial branch
+0x42592 fishingCounterB  // distinct failure/outcome branch
+0x42594 fishingCounterC  // result-selection branches 0/1
```

The remainder of the `0x24`-byte fishing block stores persistent progression/result/tutorial/message state; owner and subsystem boundary are confirmed, but several individual friendly field names remain OPEN.

### `+0x425B4`: completion / Steam-achievement progress mask - CONFIRMED

Runtime address: `Game+0xCEB1C`.

`FUN_00628F40(param)` sets `1 << param` in this DWORD and directly dispatches Steam achievement unlocks through `FUN_00710970` / `SteamUserStats::SetAchievement` / `StoreStats`.

Mapped bits:

```text
bit 0  Prologue cleared
bit 1  Episode 1 cleared
bit 2  Episode 2 part 1 cleared
bit 3  Episode 2 part 2 cleared
bit 4  Episode 3 cleared
bit 5  Episode 4 cleared
bit 6  Episode 5 cleared
bit 7  Episode 6 cleared
bit 8  Episode 7 cleared on Easy
bit 9  Episode 7 cleared on Normal
bit 10 Episode 7 cleared on Hard
bit 11 all 65 Trading Cards collected/present
```

Bits 8..10 are directly selected from the difficulty value in the final-clear path. All three map to the same public Steam `EPISODE_7_CLEARED` achievement, while the save preserves per-difficulty completion. Bit 11 is set only when all primary Trading Card IDs `236..300` are present.

The platform/meta-achievement check requires `(mask & 0x1FF) == 0x1FF` plus all 50 Side Missions complete before unlocking `PLATINUM_TROPHY`.

Safe name: `completionAchievementMask` / `achievementProgressMask`.

### `+0x4260C` onward: player status block - refined

The actor-family selector at `record+0x4AA` indexes five-element float arrays.

```text
+0x4260C float maxHP[5]                   CONFIRMED
+0x42620 float currentHP[5]               CONFIRMED
+0x42634 float displayedOrPreviousHP[5]   HIGH / near-CONFIRMED

+0x42648 float pulseAuxScalar             OPEN exact semantic
+0x4264C float pulseMax                   CONFIRMED / HIGH
+0x42650 float pulseCurrent               CONFIRMED
+0x42658 float pulseCooldownTimer         HIGH
+0x4265C float pulseRatePercent[5]        CONFIRMED
+0x42670 float pulseRateTimer[5]          CONFIRMED

+0x42684 float sleepinessCurrent[5]       CONFIRMED
+0x42698 float sleepExhaustionReserve[5]  OPEN exact friendly semantic
+0x426AC float sleepinessRatePercent[5]   CONFIRMED
+0x426C0 float sleepinessRateTimer[5]     CONFIRMED

+0x426D4 float hungerCurrent[5]           CONFIRMED
+0x426E8 float starvationReserve[5]       OPEN exact friendly semantic
+0x426FC float hungerRatePercent[5]       CONFIRMED
+0x42710 float hungerRateTimer[5]         CONFIRMED
```

`pulseMax` is directly compared against `pulseCurrent`; HUD/gameplay code divides or compares current vs max. Stabilizer/Sedative modify the pulse state and temporary rate fields. Suit data scales both maximum HP and pulse maximum.

Hunger and Sleepiness are independently confirmed by update consumers and threshold messages from `MES_ALL.MES`. At `60/80/100` the code emits escalating hunger/sleep messages; the `100`-state paths consume the adjacent long-duration reserve/penalty arrays.

### Additional confirmed systemic fields

```text
+0x427E0  current/active suit mirror                 CONFIRMED
+0x427E8  small persistent byte                      OPEN
+0x427E9  small persistent byte                      OPEN
+0x427EC  uint16 cEventResumeEventId                 CONFIRMED
+0x427EE  uint16 cEventResumeRoutineId               CONFIRMED
+0x42800  uint64 playtimeSeconds                     CONFIRMED
+0x439B4  uint8 headerMirror[0x10]                   CONFIRMED mirror to save header +0x110..+0x11F
```

The bytes at `+0x427E8/+0x427E9` remain explicitly OPEN rather than being folded into the adjacent suit/status structures without a direct owner proof.

## 16. Message history and chapter/result statistics

### Message-seen registry - CONFIRMED

```text
record +0x42818
size 0x1000 bytes = 32768 bits
```

The game indexes this block directly by `MES_ALL.MES` message ID and sets the bit after a message is shown. It is therefore a persistent `messageSeen[32768]` bitset. The size comfortably covers the ~20k message slots in the PC `MES_ALL.MES` database.

### Chapter result timing/statistics - CONFIRMED/HIGH

```text
record +0x439F4  chapterTimeSeconds[27]   CONFIRMED
record +0x43A60  totalContinues           HIGH / near-CONFIRMED
```

The current chapter timer is accumulated in 60 Hz units, divided by 60 on chapter completion, and stored into `chapterTimeSeconds[chapter]`. The results screen sums these values and formats the result as `HH:MM:SS` under `TOTAL CLEARED TIME`.

The same results screen presents `TOTAL NUMBER OF CONTINUES`, `ENEMIES DEFEATED`, and `TOTAL NUMBER OF DAYS`. The latter two can be independently tied to their known sources, leaving `+0x43A60` as the persistent total-continues value; save progression samples are consistent with this interpretation.

## 17. Per-instance weapon persistence and record tail

### End-of-record weapon-instance persistence - CONFIRMED

The final `0x2180` bytes before the last 4 bytes of the `0x45CC0` record are two arrays with identical layout:

```text
+0x43B3C  float inventoryWeaponInstanceState[67][16]; // 67 * 0x40 = 0x10C0
+0x44BFC  float toolboxWeaponInstanceState[67][16];   // 67 * 0x40 = 0x10C0
+0x45CBC  final 4 bytes / padding or unresolved tail
+0x45CC0  end of GameRecord
```

Runtime addresses with `GameRecord` base `Game+0x8C568`:

```text
record +0x43B3C = Game+0xD00A4
record +0x44BFC = Game+0xD1164
```

### Ownership pairing

`FUN_00451690` selects the count source and matching per-instance array together:

```text
inventory mode:
  count = record.inventory[itemId]          // Game+0x98270
  state = record+0x43B3C + itemId*0x40

toolbox mode:
  count = record.toolboxStored[itemId]      // Game+0x97C84
  state = record+0x44BFC + itemId*0x40
```

This directly binds array A to carried weapon instances and array B to Toolbox weapon instances.

### Per-instance semantics

Each weapon item ID `0..66` owns 16 float slots. The code:

- inserts a new value in the first free `0.0` slot (`FUN_00455590`),
- sorts the 16 slots after count changes (`FUN_00451690`),
- shifts slots left when an instance is removed (`FUN_00451740`),
- copies individual instance values when moving weapons between carry inventory and Toolbox,
- decrements the selected carried weapon's current slot during weapon wear/use (`FUN_004FE190`), zeroing the slot when exhausted,
- uses the selected slot directly for the on-screen percent/durability gauge.

Therefore these arrays are not generic weapon-type tuning data. They preserve state for separate copies of the same weapon type.

Safe semantic name:

```text
per-instance weapon condition/ammo state, 16 slots per item ID
```

For melee/durability-style items the UI consumes the value as a percentage (`value / 100.0`). For weapon types whose main current meter is ammo, related weapon state is synchronized with the current selected state; exact division of per-instance durability vs ammo semantics for every weapon subtype remains a smaller follow-up question.

### Serialized mirrors of live arrays

The game also maintains live working copies outside the serialized record:

```text
Game+0x8A3E4  live carried-instance array, size 0x10C0
Game+0x8B4A4  live Toolbox-instance array, size 0x10C0
```

Snapshot/save/replay paths copy:

```text
live carried  -> record +0x43B3C
live toolbox  -> record +0x44BFC
```

and restore paths copy them back in the opposite direction. This makes the persistence role CONFIRMED from the serializer/restore path itself.

### Per-instance weapon array ordering

The two final arrays are:

```text
+0x43B3C float carriedWeaponInstanceState[67][16]
+0x44BFC float toolboxWeaponInstanceState[67][16]
```

Empirical census over retained save records shows meaningful nonzero values sorted **ascending**, with no descending counterexample among multi-instance rows. Examples include `31.09994, 100` and `79.19992, 100, 100`.

Therefore slot 0 is the lowest / most worn remaining instance state, and equipped/current weapon logic consumes slot 0. This supersedes the earlier tentative statement that sorting was descending.

### `+0x45CBC`: final trailing DWORD - STRONGLY_SUPPORTED reserved/padding

Runtime address `Game+0xD2224`; next GameRecord begins at `Game+0xD2228`.

Evidence:

- no direct static references to `Game+0xD2224` were found;
- checked 23 retained PC save files x 28 records = **644 GameRecord images**;
- every `record+0x45CBC` DWORD is exactly zero.

Safe label: `uint32 trailingReservedZero;` / tail padding. Not promoted to absolute CONFIRMED only because a computed-pointer consumer cannot be disproven formally.

## 18. Current remaining OPEN/HIGH items

There is no longer a large anonymous `GameRecord` subsystem. Remaining uncertainty is concentrated in relatively small semantic details:

### Early/global state

- exact bit taxonomy of `playerStateMaskLo/Hi` at `+0x60/+0x64`;
- exact historic names/policies for the three capability/filter masks at `+0x68/+0x6C/+0x70`;
- upper bits of `serviceVehicleUnlockFlags` at `+0x78`;
- exact friendly distinction between character model variant A/B at `+0x7C..+0x7F`;
- bit-level taxonomy of `globalGameplayFlags` at `+0x84`;
- exact subfields at `+0xC8E0` and whether `itemAcquiredBits` means strictly acquired vs broader registered/seen state;
- friendly name for ITEM classifier category 3 (`162..201`).

### Suit/resume state

- exact semantics of `previousSuitIndex`, dirty-suit timing marker and fly/effect counter;
- friendly names for the map/place/load tuple and several flags/IDs in `+0xCAD8..+0xCB0F`.

### Named NPCs

- fine names of some `NamedNpcPersistentState.flags` bits and the distinction between its two route-distance/cost floats;
- whether `routeStateTimeOfDaySeconds` has a surviving PC reader;
- friendly names for `NamedNpcRuntimeSnapshot.restoreFlags` bits `0x008..0x100`;
- exact enum labels for NPC world `resourceVariantIndex` / `behaviorSubtype` and several low persistent behavior flags;
- original names for spawn-rule bit0/bit2, though their control-flow roles are already understood.

### Generic world/tail systems

- per-class meanings of most `WorldObjectPersistentState.stateFlags` and payload union variants; these are intentionally class-local rather than a top-level save-format gap;
- exact friendly name for the world-placement key used by `VisitedWorldPlacementEntry`;
- `CPutPersistentOverride +0x05` is unused by the reviewed PC writer/restore contract; historical/original purpose remains unnamed;
- exact friendly policy name of the rare CEvent `0xC2/1` forced/bypass variant;
- byte-perfect names for the `CWeather` block;
- exact names of fishing counters A/B/C and the remaining fishing result/tutorial fields;
- exact friendly semantics of the `pulseAuxScalar`, sleep/hunger reserve arrays and bytes `+0x427E8/+0x427E9`;
- `+0x43A60 totalContinues` is near-confirmed rather than promoted to absolute CONFIRMED.

### Reserved/unused regions

The following are intentionally retained as `STRONGLY_SUPPORTED` rather than historical-source-code `CONFIRMED`:

```text
+0x0B96..+0x0B12F  0xA59A-byte unused/reserved PC span
+0x41680..+0x417FF  0x180-byte reserved/unused candidate
+0x42290..+0x4258F  0x300-byte reserved/unused candidate
+0x45CBC             trailing zero DWORD
```

For the reviewed Steam/GOG PC builds and retained save corpus, there is no evidence that gameplay consumes these regions.

## 19. Design consequence for a native ZachFix load selector

The record contains large, interdependent native world-state registries. A ZachFix multi-save selector should not manually reconstruct gameplay state. Preferred architecture remains:

```text
select complete vanilla 0x7A2620 save image
    -> feed/redirect it to the vanilla save staging/load path
    -> let DP restore record0
    -> let vanilla post-load reconstruction run normally
```

## 20. Canonical status summary

At this point the `GameRecord` RE is **structurally near-complete for the reviewed PC builds**.

The important distinction is:

```text
unknown structure/ownership     -> now rare
known structure, unknown name   -> still present in several small fields
known shared registry, class-local payload semantics -> expected by design
```

For future work, do not reopen a region merely because an original C++ field name is unknown. A block should be considered structurally closed when its boundaries, indexing/keying, serializer/reset behavior and runtime owner/consumer contract are established.

The remaining useful RE targets are therefore the small `OPEN/HIGH` items listed above, not another broad sweep for large anonymous save regions.


## 19. Addendum: native save/resume contract and special-resume tokens

This addendum records the focused save-anywhere feasibility pass performed after the
original GameRecord census. It changes the architectural interpretation of several
early/tail fields without invalidating the major region map above.

### 19.1 Native save modes and shared pre-save synchronization - CONFIRMED

Steam `FUN_004524C0` is the common persistent-synchronization dispatcher. Modes other
than `3` and `4` run the full pre-save refresh cluster before GameRecord is copied:

```text
FUN_00451450      resume/location/York transform snapshot
FUN_00449480      CTimer accessor
FUN_00470900      CTimer -> record+0x90..+0x9C
FUN_00405360      subsystem synchronization
FUN_00430A10      CEvent/CEvCore snapshot with mode argument
thunk_FUN_004B76D0
FUN_004B1560      NPC/world persistence snapshot
```

Recovered mode roles:

```text
0  generic/full normal snapshot
1  phone/manual snapshot
2  CEvent 0xC6 checkpoint snapshot
3  top-level/system state-0x46 context; normal refresh skipped
4  load/restore transition; normal refresh skipped
5  historical GameRecord capture
```

Manual mode 1 is selected by Steam `FUN_006AE0C0` when neither mode-3 nor mode-4 global
flag is active. The generic full builder `FUN_006ACAB0` uses mode 0 in the corresponding
normal case.

Mode 3 is mechanically tied to numeric state `0x46`: Steam `0x006427D5` compares the
active top-level/system state with `0x46`, `0x006427E0` sets `Game+0x8C5EC bit 0x4000`
on equality, and `0x006427F1` clears it otherwise. The friendly/original semantic name
of state `0x46` remains OPEN.

### 19.2 Player state 0 is not a save prerequisite - CONFIRMED

The native phone flow enters CPlayer state `0x37` and launches the manual save/menu
transition from inside that state's phase machine. York returns to state `0` only after
later phone phases complete. Therefore a blanket `CPlayer+0x654 == 0` rule would be
stricter than vanilla behavior.

### 19.3 Caller-specific save gates and resume normalization - CONFIRMED architecture

CEvent autosave/checkpoint callers perform their own preconditions before the common
serializer:

- CEvent `0xC2` gates through `FUN_00723820()` plus a context-bit condition, then clears
  global bits `0x00800000`, `0x01000000`, and `0x04000000` before establishing its save
  transition;
- CEvent `0xC6` gates through `FUN_004FEAB0()`, establishes checkpoint-specific runtime
  flags, snapshots with mode 2, then copies the current `0x45CC0` record to `Game+0xBE8`.

This is direct evidence that native save safety is partly a caller/resume-contract
property rather than one hidden universal serializer predicate.

### 19.4 Resume block behavior - CONFIRMED

`FUN_00451450` snapshots normal positional resume data at `+0xCAD8..+0xCB00`. When
`Game+0x8C5EC & 0x04000000` is set, the routine consumes that one-shot scripted-resume
request by writing `record+0xCADA = 0xFF` and clearing the bit. The load path treats
`CADA == 0xFF` as the predefined/scripted load-point branch; otherwise normal resume
uses saved York transform through Steam `FUN_00509080`.

### 19.5 Early object-action packet is partially resume-significant - REFINED

The live `record+0x14..+0x5B` overlap remains the generic 0x48-byte object-action packet,
but the save/resume pass proves that it cannot be classified as merely incidental serialized runtime
storage.

For world-resource subtype `0x75C`, the interaction setup writes a target transform into:

```text
record+0x18..+0x24   packet transform/vector
record+0x38          packet heading/yaw
```

and sets `playerStateMaskHi` bit `0x00000004` at `record+0x64`.

On load this bit selects CPlayer state `0x40` rather than the common state-0 path. State
`0x40` reconstructs the world/location context using the persisted data and later clears
the high-mask bit through the `(low=0, high=4)` mask-clear helper. This is a confirmed
one-shot special-resume token.

The packet's `record+0x14` runtime action-object field may still hold a live object
reference during gameplay. The current interpretation is mixed: selected packet
fields are deliberate resume storage, while pointer-like/runtime phases must be
reconstructed rather than assumed valid after reload.

### 19.6 `+0x43818/+0x4381C`: manual-save CEvent 0x1F5 resume scratch - CONFIRMED

The previous map ended `messageSeen` at `+0x43817` and left the following tail only
coarsely assigned. Manual save mode 1 gives the first eight bytes a concrete owner.

Inside the CEvent snapshot routine, when:

```text
saveMode == 1
CEvent+0x3012 == 0x1F5
```

the engine sets persistent `globalGameplayFlags` bit `0x1000` at `record+0x84` and
writes:

```text
record+0x43818 = CEvent+0x3000
record+0x4381C = CEvent+0x3004
```

The load path later tests the `0x1000` marker, recreates the `0x1F5` controller through
`FUN_0042D080`, restores the saved value, dispatches the associated `0x25F/0x1C` event
path, and clears the marker. Safe field names:

```text
+0x43818 uint32 manualSaveEvent1F5ResumeA
+0x4381C uint32 manualSaveEvent1F5ResumeB
```

The exact historical names of the two CEvent runtime values remain OPEN.

### 19.7 Post-load Player reconstruction - CONFIRMED architecture

The normal loader does not restore the complete live CPlayer FSM/object graph from the
save image. `CPlayer+0x654` is transient Player state rather than a serialized GameRecord
field. Player initialization clears/rebuilds numerous handles and phases, then applies
persistent world state and resume policy.

The reviewed Steam path now has both sides of one explicit state-selection split:

```text
FUN_00506F70:
    FUN_004FD860(0, 4) != 0
        -> FUN_00528F40(0x40)

default Player initialization in FUN_00507BA0:
    FUN_004FD860(0, 4) == 0
        -> FUN_00528F40(0x00)
```

Thus normal/default reconstruction selects state `0x00` when the known high-mask resume
adapter is absent; the persistent bit-4 adapter selects state `0x40`. No reviewed load
selector restores state `0x38` merely because it was the pre-save transient state.

This establishes the current model:

```text
persistent GameRecord + resume anchor + small explicit resume adapters
    -> native world/Player reconstruction
```

rather than a byte-for-byte live-runtime snapshot.

Whole-record buffer restore is also separate from reconstruction: `FUN_0061A830` owns
the `Game+0xBE8 -> Game+0x8C568` `0x45CC0` memcpy; `FUN_00506F70` owns the subsequent
Player/world reconstruction and resume work.

### 19.8 Save-anywhere research consequence - STRONGLY_SUPPORTED, not production proof

The PC format and serializer do not impose hardcoded save coordinates. Phone saves,
autosaves and checkpoints share the same persistent machinery but establish different
controlled resume contracts. A future quicksave experiment should therefore reuse the
native synchronization/snapshot/load pipeline and respect special resume tokens rather
than synthesize GameRecord fields manually.

A rejected intermediate hypothesis claimed that the serialized action-object field at
`record+0x14` would make a state-`0x38` arbitrary save deterministically crash after
reload. That specific chain is **DISPROVEN**: the pre-save transient Player state is not
restored directly, so normal/default load does not re-enter `0x38` on that basis.

Still-unproven cases include other post-load consumers of `record+0x14+0x00` and the
caller-specific semantics of vehicle/multi-phase object protocols. These remain
validation targets, not reasons to relabel the native format as save-point-bound. A
state-`0x00` whitelist is therefore a conservative future ZachFix policy, not a statement
that vanilla can only save from state 0.
