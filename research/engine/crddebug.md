# CRdDebug retail debug-menu framework

Status: **runtime-confirmed on PC retail (Steam 1.01b)**, with matching code present in GOG 1.01b and a homologous framework in the original Xbox 360 retail executable.

This note documents the surviving engine-native debug-variable menu. It is distinct from `CToolBox` / `0000_TOOLBOX.FLB`, which belongs to the normal in-game item storage UI and is not developer tooling.

## What survived in PC retail

`CSingleton<CRdDebug>` is constructed during normal startup. Its object contains:

| CRdDebug offset | Role |
|---:|---|
| `+0x04` | registered entry count |
| `+0x08` | current/selected menu entry, null when closed |
| `+0x0C` | menu-entry table |
| `+0x10` | integer debug-value table |

The initializer allocates:

- `0x2A30` bytes for menu entries
- `0x708` bytes for integer values

The entry stride is `0x18`, so both tables have capacity for **450 entries / 450 integer values**.

Each menu entry is effectively:

```text
+0x00  char name[16]       // practical max: 15 chars plus NUL
+0x10  int* value          // null for category rows
+0x14  Entry* parent       // null for root-level rows
```

The retail registration helper copies a text label, resolves an optional parent label, and points the row at one integer slot. The original developer registration calls are absent from PC retail, leaving `entryCount == 1` after reset.

## Native input/editor behavior

The surviving updater implements:

- open/close gesture using input mask `0x20` after a hold threshold
- previous/next navigation among siblings
- enter child category
- return to parent category
- integer edits by `+/-1`, `+/-10`, `+/-100`, and `+/-1000`

The surviving renderer draws categories and values using the literal formats:

```text
 %s
 %s : %+d
```

Runtime testing confirmed that the PC retail renderer and editor work without reimplementation: a ZachFix-owned row rendered on screen and the native input path changed its integer value.

## Supported PC addresses

### Steam 1.01b

| Purpose | VA | RVA from `DP.exe` base |
|---|---:|---:|
| singleton pointer | `0x00BD7688` | `0x007D7688` |
| AddEntry | `0x006FF710` | `0x002FF710` |
| Update | `0x006FF860` | `0x002FF860` |
| Draw | `0x006FFCE0` | `0x002FFCE0` |

### GOG 1.01b

| Purpose | VA | RVA from `DP.exe` base |
|---|---:|---:|
| singleton pointer | `0x00BD7688` | `0x007D7688` |
| AddEntry | `0x006FF730` | `0x002FF730` |
| Update | `0x006FF880` | `0x002FF880` |
| Draw | `0x006FFD00` | `0x002FFD00` |

## Known retail value consumers

The following is a **lower-bound** census. These slots have direct, verified retail consumers reachable from the live `CRdDebug+0x10` value table. It is not proof that other slots are unused: consumers may cache the values pointer or access it indirectly.

| Slot | Offset | Steam consumer / evidence | Current interpretation |
|---:|---:|---|---|
| 6 | `+0x018` | `FUN_004213D0` | value `2` suppresses a per-state time/timer advance; exact dev label unknown |
| 7 | `+0x01C` | `FUN_00470B10` | multi-condition engine gate; exact role unknown |
| 23 | `+0x05C` | code at `0x004630E7` | branch gate in an object/system update path |
| 34 | `+0x088` | code at `0x00674AD3` | callback/dispatch gate |
| 39 | `+0x09C` | `0x00476DF3`, `0x00476F1B` | multi-mode object/debug path; value `2` selects an extra action |
| 40 | `+0x0A0` | `0x00476B8C` | 1..4 mode switch in object diagnostics |
| 41 | `+0x0A4` | `0x004769D8`, `0x00476A3E` | 1..3 mode switch; feeds diagnostic formatting/transform branches |
| 42 | `+0x0A8` | `0x00476829` | object-position diagnostic gate |
| 43 | `+0x0AC` | `0x004764AB`, `0x0047650C` | signed mode used by transform/debug positioning path |
| 47 | `+0x0BC` | `FUN_005C92C0` | serial/debug overlay control branch; value `2` reaches a `Serial:%d` path. 2026-10-04: the same function is also proved as an installed CObjectCar callback, so it is not debug-only. |
| 48 | `+0x0C0` | `0x004763C9` | object/debug display branch |
| 49 | `+0x0C4` | `0x0047634B` | object/debug display branch |
| 50 | `+0x0C8` | `0x00476B3B` | debug helper/object allocation branch |
| 51 | `+0x0CC` | `0x00476D60` | object/serial diagnostic branch |
| 52 | `+0x0D0` | `FUN_005CAC60` | iterates object IDs and invokes an engine helper; likely diagnostic control |
| 87 | `+0x15C` | `FUN_0060F310` | **high confidence: input/action gate**; normal evaluation requires menu closed and slot zero |
| 93 | `+0x174` | `FUN_00496BE0` | enables an extra call in a character/event path |
| 209 | `+0x344` | `FUN_0046B7D0` | numeric override: nonzero value becomes a percentage-like scalar |
| 217 | `+0x364` | `FUN_004B1C30` | character/object ID filter |
| 218 | `+0x368` | `FUN_004B1C30` | character/object ID filter |
| 219 | `+0x36C` | `FUN_004B1C30` | character/object ID filter |
| 226 | `+0x388` | `FUN_004B1C30` | upper filter/selector in the same path |
| 243 | `+0x3CC` | `FUN_004DEE70`, `FUN_005BE540` | copied into runtime object state and gates an alternate branch |
| 255 | `+0x3FC` | `FUN_00537270` | **high confidence: hard-coded camera/view preset A** |
| 256 | `+0x400` | `FUN_00537270` | **high confidence: hard-coded camera/view preset B** |
| 257 | `+0x404` | `FUN_00537270` | **high confidence: hard-coded camera/view preset C** |
| 258 | `+0x408` | `FUN_00537270` | **high confidence: hard-coded camera/view preset D** |
| 261 | `+0x414` | `FUN_004213D0` | state-transition/reset gate |
| 262 | `+0x418` | `FUN_004213D0` | explicitly cleared by a state/reset path |
| 277 | `+0x454` | `FUN_004213D0` | toggles an engine singleton byte at `+0x24C` |
| 320 | `+0x500` | `FUN_005FFE80` | extra HUD/debug display branch |
| 379 | `+0x5EC` | code at `0x00476F31` | object/world diagnostic branch |
| 383 | `+0x5FC` | `FUN_004533C0` | one-shot-like branch; code clears the slot back to zero after use |

The dense `+0x09C..+0x0D0` cluster is particularly promising. Its surrounding code references diagnostic strings such as `LEVEL`, `FIELD`, `ACT[%d:%03d]`, `LISTNO:%d`, `ACTIVE:%d`, `DELETE:%d`, hexadecimal IDs and object/resource names. This strongly suggests that a substantial chunk of the original menu controlled object/world inspection overlays.

## Reconstructed ZachFix research menu

The research bridge deliberately does **not** bind the native editor directly to unknown retail values. Instead it reconstructs a hierarchy using ZachFix-owned integers and mirrors retail values after each native update.

Current tree:

```text
ZachFixDbg
├─ Safe Demo
│  ├─ Demo Value
│  ├─ Demo Toggle
│  └─ Demo Scale
├─ Raw Browser
│  ├─ Slot Index          editable, clamped to 0..449
│  ├─ Byte Offset         read-only mirror
│  ├─ Live Value          read-only mirror
│  ├─ Prev Value          read-only mirror
│  ├─ Change Count        read-only mirror
│  ├─ Known Refs          lower-bound static consumer count
│  ├─ Min Seen
│  └─ Max Seen
├─ Activity
│  ├─ Nonzero Slots
│  ├─ Changed Slots
│  ├─ Change Events
│  ├─ Frame Changes
│  ├─ Last Slot
│  ├─ Last Old
│  └─ Last New
├─ Known 000-199          verified consumer slots, read-only
├─ Known 200-449          verified consumer slots, read-only
├─ Inferred               conservative semantic aliases, read-only
├─ Actions
│  ├─ Dump Nonzero
│  ├─ Dump Changed
│  ├─ Dump All
│  └─ Reset Stats
├─ Unsafe Write
│  ├─ Write Enabled       read-only INI state
│  ├─ Write Arm           must be exactly +1
│  ├─ Write Value
│  └─ Apply Write         one-shot token
├─ Menu State
│  ├─ Entry Count
│  ├─ Cur Entry
│  ├─ Slot Count
│  └─ Known Count
└─ Input Probe
   ├─ Pad Index
   ├─ Held Mask
   ├─ Press Mask
   ├─ Repeat Mask
   ├─ Mask20 Held
   └─ Mask20 Frames
```

### Unsafe write safety

Raw writes are off by default. A write requires **both**:

1. `NativeDebugMenuAllowWrites = true` in `ZachFix.ini`
2. `Write Arm` set to exactly `+1` in the native menu
3. changing `Apply Write`

Every raw write is logged and immediately disarms the writer. Changing `Slot Index` also disarms it.

Unknown slots can still crash, soft-lock, trigger scene transitions, alter state unexpectedly, or call developer-only paths with missing assets. Use snapshots and read-only activity tracking first.

## Original native controls and open gesture

The editor controls can be mapped back to already recovered PC logical-action masks:

| Native debug-menu operation | Logical mask | Existing PC action mapping |
|---|---:|---|
| previous sibling | `0x4001` composite | Up |
| next sibling | `0x8002` composite | Down |
| return to parent | `0x10004` composite | Left |
| enter child | `0x20008` composite | Right |
| `+1` | `0x001000` | INTERACT |
| `-1` | `0x000800` | RELOAD |
| `+10` | `0x002000` | LIGHTONOFF |
| `-10` | `0x000400` | OBSERVE |
| `+100` | `0x000040` | AIM |
| `-100` | `0x000080` | ATTACK |
| `+1000` | `0x400000` | RUN |
| `-1000` | `0x800000` | HOLDBREATH |

The retail updater contains its own open/close gesture around **held logical mask `0x20`**. `0x20` is not among the normal remappable action masks recovered from the PC config loader, making it a strong candidate for a reserved developer/raw action.

The research bridge calls the native updater even while the menu is closed and exposes the same CInput state through `Input Probe`: active pad index, held mask, rising/press mask, repeat mask, whether bit `0x20` is currently held, and the number of consecutive observed frames. Transitions of bit `0x20` are also logged with the full masks. This should let runtime testing identify the original developer gesture without guessing. F9 remains a deterministic emergency/research toggle.

## Related startup artifacts

The executable also contains:

```text
UPDATA/_FLINK/0001_DEBUGTITLEMOVE.FLB
UPDATA/_FLINK/0000_DEMOSTAGE1.FLB
UPDATA/_FLINK/0000_DEMOSTAGE2.FLB
...
UPDATA/_FLINK/0000_DEMOSTAGE49.FLB
```

These are not yet proven to be part of `CRdDebug`; keep them as a separate developer-startup/stage-launcher research branch until the flow graph connects them.

## 2026-10-04 CObjectCar callback correction

Steam `005C92C0`, previously encountered through the debug/serial-value surface, is now
proved to be an actually installed CObjectCar callback in one Phase 4 producer chain.
Its diagnostic/debug-value reads remain real, but the function must not be treated as a
"debug-only" routine. On selected event 1 it participates in a CObjectCar control path
and later selector handling.

This does not imply that every callsite or branch is vehicle-only. Preserve both facts:
`005C92C0` contains the recovered debug/serial surface, and at least one concrete CObjectCar
producer installs it as a live object callback.
