# Native UI evidence record

**Date:** 2026-10-01
**Primary build:** Steam 1.01b
**Cross-check:** GOG 1.01b

This is the compact evidence ledger for the native UI/custom-window investigation.
It records the addresses and claim corrections needed to reproduce the architecture
without carrying forward the superseded COption-centered interpretation.

## Exact build identities

```text
Steam DP_STEAM.exe
SHA-256 7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029

GOG DP_GOG.exe
SHA-256 c954c2e3b205d444b0fc3649adf4bd8462a7dfe73d89599e24a7e17a129415c2
```

Both exports were matched to these exact PE identities before the final address map was
accepted.

## Core address map

| Role | Steam | GOG | Status |
|---|---:|---:|---|
| top-level menu dispatcher | `00452780` | `004527B0` | confirmed |
| shared Title/Pause menu controller callback | `00642640` | `00642590` | confirmed static + runtime |
| COption factory | `005E7620` | `005E76F0` | confirmed |
| COption creation wrapper | `005EA8D0` | `005EA9A0` | confirmed return in EAX |
| COption ctor | `00620050` | `0061FB90` | confirmed |
| COption callback/controller | `00621130` / `00620480` | `006210B0` / `00620400` | confirmed |
| COption page-8 handler | `00621300` | `00621280` | confirmed homolog; not the runtime-visible root Options screen |
| COption page-8 draw | `00621830` | `006217B0` | confirmed homolog; not the runtime-visible root Options screen |
| COption visible main handler | `00624FD0` | `00624F50` | confirmed + runtime page-2 identification |
| COption visible main draw | `00624810` | `00624790` | confirmed + runtime page-2 identification |
| generic selector-0 factory | `006C5930` | `006C5430` | confirmed |
| manager registration | `006C5AE0` | `006C55E0` | confirmed |
| callback setter | `006BAB80` | `006BAAD0` | confirmed |
| callback dispatcher | `00402870` | `00402860` | confirmed |
| manager update | `006C5FF0` | `006C5AF0` | confirmed homolog |
| render callback broadcast | `006C7420` | `006C6F20` | confirmed homolog |
| manager cleanup | `006C7070` | `006C6B70` | confirmed homolog |
| base final delete virtual | `0062D170` | `005F4DD0` | confirmed PE vtable |
| base cleanup virtual | `006BA850` | `006BA7A0` | confirmed PE vtable |
| base event-1 virtual | `006BA8A0` | `006BA7F0` | confirmed PE vtable |
| base removal virtual | `006BAB20` | `006BAA70` | confirmed PE vtable |
| existing-message draw | `0045C9F0` | `0045CA20` | confirmed homolog |
| formatted C-string draw | `0045C680` | `0045C6B0` | confirmed homolog |
| scene/menu context getter | `00427780` | `004277A0` | confirmed homolog |
| Main Menu `0x10` geometry-record accessor | `00459080` | `004590B0` | confirmed homolog; not the page-2 row-element accessor |
| Main Menu layout object | `01473E74` | `01473E74` | confirmed direct receiver |
| shared menu state | `014736D4` | `014736D4` | confirmed |
| title row selection | `014736DC` | `014736DC` | confirmed |
| COption page-8 row-pair table | `00780804` | `007807F4` | confirmed direct data use for page 8 |
| COption page-2 row styling table | `007808BC` | `007808AC` | confirmed direct data use; 3 bytes per stock row |
| COption selected RGBA | `0147168C` | `0147168C` | confirmed direct data use |
| COption normal RGBA | `0147169C` | `0147169C` | confirmed direct data use |
| CLayout accessor | `004587A0` | `004587D0` | confirmed |
| CLayout `0x50` row-element accessor | `004588C0` | `004588F0` | confirmed; used by COption page-2 styling/geometry |
| CLayout binder | `00459A90` | `00459AC0` | confirmed build-specific entry |

The GOG generic factory and manager/draw mappings close the final v5 address-map OPEN
entries. They are exact homologous function entries in the GOG export, not address
shifts guessed from Steam.

## Generic task creation ABI

The Steam retail reference uses:

```cpp
CRdObject* CreateTask(
    Manager* manager,          // ECX
    uint8_t selector,          // 0
    int bucket,                // 0
    uint8_t dispatchClass,     // 0
    uint8_t alternateList);    // 1
```

For selector `0`, `FUN_006C5930` allocates `0x160`, calls `FUN_00402650`, then forwards
the object to `FUN_006C5AE0` for registration.

GOG `FUN_006C5430` has the same selector-0 branch and calls GOG manager registration
`FUN_006C55E0`.

## Callback contract

`FUN_006BAB80` / `FUN_006BAAD0` stores the callback at `object+0x44` and immediately
sends event 0. The evidence-compatible callback ABI is:

```cpp
void __cdecl Callback(CRdObject* object,
                      uint32_t event,
                      uintptr_t payload);
```

Observed generic-task phases:

```text
event 0     synchronous initialization from callback setter
event 1     regular manager update through vtable +0x0C
event 0x12  render broadcast
```

The callback may point directly into a resident 32-bit ASI if the function uses the
correct ABI, the module stays loaded for the task lifetime, nonvolatile registers are
preserved normally, and no C++ exception is allowed to unwind through retail code.

## Retail menu reference

Steam `FUN_00635460` creates selector-0 task `FUN_006348D0` using the exact tuple
`(0,0,0,1)`. This callback is the key proof that a base CRdObject can behave as a
menu-like controller without COption or CLayout ownership.

It handles:

- event 0 initialization;
- event 1 native Up/Down/Confirm/Cancel polling;
- event `0x12` native text rendering;
- native menu sounds;
- normal `+0x30` removal.

Additional retail selector-0 + callback compositions exist, confirming this is a normal
engine pattern rather than a one-off accident.

## COption corrections

The following older interpretations are superseded:

- `FUN_006B2BE0` is not the XLY parser/loader; it is an existing-resource accessor;
- `FUN_004587A0` returns one of 16 global `CLayout` slots and does not allocate/register
  a private layout;
- `FUN_004588C0` is a bounds-checked `0x50`-stride element-index accessor, not a semantic
  control-ID lookup;
- `COption` top-level selection is `this+0x1FC`, not `this+0x200`;
- `FUN_006245B0` is a stock ten-row/slot-0 highlighter, not a generic focus helper;
- `FUN_00624E40` writes retail settings and is not a generic action dispatcher;
- `FUN_0061F660` is a stock category-specific controller and is not a universal Confirm
  seam;
- vtable `+0x30` is the deferred removal request, not the scalar deleting destructor;
- `DAT_01474CE8` is a stock Pause/COption tracking pointer, not a general owner or a
  ZachFix child slot.

## CLayout global-pool result

The pool is exactly 16 entries of `0x398` bytes.

A whole-program Steam call census of `FUN_004587A0` found hundreds of accesses,
including direct use of slot 13 and many dynamically supplied indices. COption teardown
can reset all 16 slots. Therefore slots 10-15 are not reserved/private space for a mod.

The final architecture deliberately avoids claiming the pool.

## Shared Title/Pause controller and Main Menu row gap

Runtime validation on 2026-10-04 corrected the earlier Pause-only label. Steam
`FUN_00642640` / GOG `FUN_00642590` is a shared Title/Pause menu controller. Its event-0
path loads `UPDATA/TITLE`, while event 1 and event `0x12` dispatch behavior according to
shared state globals:

```text
DAT_014736D4  active menu/controller state
DAT_014736D8  paired/return state
DAT_014736DC  title/menu row selection
DAT_01474CE8  stock COption child only
```

For the title Main Menu, state `2` plus scene mode `2` is the narrow runtime context.
The retail row selector deliberately skips selection value `3` in both directions:

```text
Down: 2 -> 4
Up:   4 -> 2
```

The Confirm switch has cases `0`, `1`, `2`, `4`, and `5`, but no case `3`. Event `0x12`
draws message IDs `0x3A49`, `0x3A4A`, `0x3A4B`, then `0x3A4D`, leaving the same numeric
hole. This makes row `3` a much narrower integration seam than replacing a retail row.

The Main Menu draw path uses layout object `01473E74`; `00459080` / `004590B0` returns
its indexed `0x10`-byte geometry records, with coordinates at `+4/+8`. ZachFix can use
those existing coordinates while still avoiding ownership or mutation of a global
CLayout slot.

The stock Options branch at row `2` still owns `DAT_01474CE8`. ZachFix does not reuse
that pointer and continues to track its selector-0 child externally.

## COption page-2 insertion seam

Runtime observation on 2026-10-04 corrected the integration target. The visible root
Options screen executes with `COption+0x170 == 2`, initially `+0x174 == 0`, and dispatches
to `00624FD0` / `00624810` (Steam). Page 8 is a different COption subcontroller.

The page-2 controller owns a hardcoded 10-index row domain (`+0x1FC`, values `0..9`) with
stock remapping/skips. Row 9 is the Exit path; row 8 is the last normal settings row in
the visible sequence. The ZachFix doorway therefore keeps the retail selector valid and
uses external state between rows 8 and 9.

The page-2 render uses global CLayout slot 0 and the three-byte-per-row tables at
`007808BC` Steam / `007808AC` GOG. Those row-table indices must be resolved with the
`0x50` element accessor `004588C0` Steam / `004588F0` GOG. The superficially nearby
`00459080` / `004590B0` entries address a different `0x10` geometry-record array and are
not interchangeable. The infrastructure pass derives the custom row from the live stock
row geometry and does not alter the retained XLY.

The direct COption controller ABI is `this + event + payload`. Raw Steam ASM shows
`00620480` reading the event from `[ESP+4]` and returning with `RET 8`; wrapper
`00621130` pushes callback arguments 3 and 2 before restoring callback argument 1 into
`ECX`. The payload is therefore ABI-significant even where the controller does not
otherwise read it. A development build that removed this second stack argument crashed
on Options entry and was corrected immediately.

## Frame-order proof

Steam top-level frame execution establishes:

```text
manager update       FUN_006C5FF0
render               FUN_00401440
render broadcast     FUN_006C7420(event 0x12)
manager cleanup      FUN_006C7070
```

Important raw anchors:

```text
00401B6E / 00401B9F  manager update
00401C75             render entry
00401765             event-0x12 broadcast
00401C8D             manager cleanup
```

A `+0x30` request made in event 1 therefore does not prevent a same-frame event `0x12`.
The custom callback needs a closing/tombstone state that turns such later events into a
no-op.

The manager update builds the current object snapshot before it begins event-1 dispatch.
A child created by Pause during that pass is not added to the already-built update
snapshot. It can receive event `0x12` in the same frame, but its first normal event-1
input/update occurs on the next manager update.

## Correct PoC initialization order

The callback setter's synchronous event 0 changes the creation order required by a
ZachFix implementation:

```text
CreateTask
    -> create external state map entry
    -> set WAIT/generation integration state
    -> SetCallback
       -> event 0 fires immediately
    -> return from Pause hook
```

If the state entry is created after `SetCallback`, the initialization event can be lost.

## Runtime PoC status

The selector-0 lifecycle PoC was runtime-validated on Steam 1.01b on 2026-10-04. Three
consecutive open/normal-removal cycles completed cleanly in the first supplied log. A
second runtime test validated the visible title Main Menu row-3 experiment and repeated
normal child removal. That title row is now retired as the primary UX. The current
runtime-test step inserts the external pseudo-row inside stock COption, while retaining
the same lifecycle guards:

- exact executable SHA-256 before enabling any profile;
- expected-byte/prologue validation for every retail entrypoint used;
- one active ZachFix native task maximum;
- one externally tracked selector-0 child and its owning COption instance;
- external state created before callback installation;
- explicit `closing` state and harmless post-close callbacks;
- parent resume no earlier than the next update tick;
- optional tombstone retained one additional frame;
- no object dereference after `+0x30` in the same callback;
- no manual game-object free;
- no retail vtable modification;
- no COption tracking globals or ZachFix-owned CLayout slots;
- stock COption page-2 `+0x1FC` kept inside its native `0..9` domain;
- COption page-2 styling resynchronized through its stock helper at pseudo-row boundaries;
- custom-row geometry resolved through `004588C0` / `004588F0` and live `+0x40/+0x44` positions;
- bounded trusted C-string formats only.

The visible entry is now inside stock Options so the same doorway can be reached from
both Title -> Options and Pause -> Options. The earlier F9/shared-menu development hook
has been removed; COption is the only opening path.
