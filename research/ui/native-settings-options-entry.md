# ZachFix Settings inside stock Options

**Development snapshot:** 2026-10-04
**Target:** Steam/GOG 1.01b
**Status:** page-2 doorway and highlight behavior runtime-validated; infrastructure pass uses live CLayout row geometry and removes the F9 development path

## Goal

Make stock **Options** the single visible entry point for ZachFix Settings:

```text
Title Main Menu -> Options -> ZachFix Settings
Pause Menu      -> Options -> ZachFix Settings
```

The custom settings page remains a ZachFix-owned selector-0 `CRdObject` task. `COption`
is only the parent/doorway.

## Runtime correction: the visible Options screen is page 2

The first two insertion attempts were based on a wrong static semantic assignment. They
treated COption page `8` (`+0x170 == 8`, handler `00621300`, draw `00621830`) as the
visible main Options screen.

The direct controller hook produced decisive runtime evidence on 2026-10-04:

```text
COption controller observed: event=0x12 ... page=2 state=0x0 selection=0
COption controller observed: event=0x01 ... page=2 state=0x0 selection=0
```

This matched the screen showing Autosave / Aim / Camera / Sound / Gore / Exit. Inspection
of `FUN_00620480` then confirmed page `2` dispatches to the exact routines that own this
screen:

| Role | Steam | GOG |
|---|---:|---:|
| COption callback wrapper | `00621130` | `006210B0` |
| COption controller | `00620480` | `00620400` |
| visible main Options handler | `00624FD0` | `00624F50` |
| visible main Options draw | `00624810` | `00624790` |
| stock styling helper | `006245B0` | `00624530` |

Page `8` remains a real COption subcontroller, but it is **not** the visible root screen
shown in the runtime test.

## Correct COption fields for page 2

```text
COption +0x170  active page selector; visible main Options == 2
COption +0x174  page-2 local state
COption +0x1EC  mode/device-dependent row availability gate
COption +0x1FC  active stock row index
COption +0x200  active row choice while editing
COption +0x204  ten stock row-choice values
COption +0x27C  page fade/alpha
```

Page-2 local states observed statically:

```text
0  initialize row values/layout state
1  normal row navigation / Confirm / Cancel
2  binary left/right editing path
3  four-way/subchoice editing path
```

## Stock row domain

`FUN_00624FD0` navigates a hardcoded **10-index** stock domain (`0..9`) and remaps/skips
several internal rows depending on state. It is not a simple contiguous list.

The stock Exit path is row `9`: Confirm on row `9` executes the page close/apply path.
Row `8` is the final normal settings row before Exit in the visible navigation sequence.

Therefore ZachFix does **not** write a synthetic row index into `+0x1FC`. The corrected
integration uses an external pseudo-row:

```text
... -> stock row 8 -> [ZachFix Settings] -> stock row 9 (Exit)
```

While the custom row is selected, `+0x1FC` is now shadowed at valid stock row `9` (Exit) and
ZachFix keeps `customSelected` externally. Exit is deliberately used as the shadow because
its description/value surface is blank; this prevents the previous stock row description from
bleeding into the custom pseudo-row.

## Input bridge

Only page `2`, local state `1` is intercepted.

```text
row 8 + Down  -> customSelected = true, stock shadow becomes 9
custom + Up   -> row 8
custom + Down -> row 9 (Exit)
row 9 + Up    -> customSelected = true, stock shadow remains 9
custom + Confirm -> create validated selector-0 ZachFix child
custom + Cancel  -> hand control back to stock COption close behavior
```

The stock field never leaves `0..9` and stock page-specific action writers are never
called for the ZachFix pseudo-row.

## Rendering correction

The screenshot also corrected an earlier visual interpretation. The page-2 screen does
not use the page-8 six-row message-text path. `FUN_00624810` first renders CLayout slot
`0` through the retained layout renderer; its visible row artwork/labels are controlled
by the page-2 XLY/XPC layout family and the hardcoded three-byte-per-row element tables.
The function separately draws some headers and dynamic values through message IDs.

Thus the user's observation that the visible settings labels look asset-backed is
consistent with the actual page-2 draw path.

The first page-2 runtime test proved the pseudo-row navigation and Confirm doorway: the
selector-0 child opened and closed repeatedly from the invisible insertion. It also exposed
two visual mistakes in the first implementation:

1. the three-byte styling-table **column 1** was treated as a guaranteed Exit anchor even
   though it can be absent/sentinel for a row;
2. the returned CLayout element was read at `+0x04/+0x08` as if it were the draw-node type
   used elsewhere. Page-2 itself reads the live CLayout position at **`+0x40/+0x44`**.

The next runtime test exposed the remaining infrastructure mistake: the bridge used
`00459080` / `004590B0` as though those functions returned the same elements addressed by
the page-2 styling tables. They do not. Those entries expose a separate `0x10`-byte
geometry-record array used by other menu paths. Page-2 styling resolves its three-byte row
table through `004588C0` Steam / `004588F0` GOG, which returns the `0x50`-byte CLayout
row elements whose live position is at `+0x40/+0x44`.

The infrastructure pass therefore uses the exact retail chain:

```text
CLayout slot 0
  -> page-2 row table column 0
  -> 004588C0 / 004588F0
  -> 0x50-byte row element
  -> X/Y at +0x40/+0x44
```

`ZachFix Settings` is anchored to stock row 8 and its vertical spacing is derived from the
nearest preceding page-2 row. Exit is used as an additional bound. No fixed `(400,500)` or
other resolution-space fallback remains. If real page-2 geometry cannot be resolved, the
bridge fails closed and leaves stock navigation unchanged, preventing an invisible
selectable pseudo-row.

The runtime C-string draw uses the same presentation mode `4` and CMessage text-mode value
`0x10` used by page-2 native message draws. Exact matching to the baked/localized retail
labels remains a later asset/localization task.

Page-2 row styling table:

```text
Steam  007808BC  (3 bytes per row)
GOG    007808AC  (3 bytes per row)
```

Selected/normal color vectors remain:

```text
selected  0147168C
normal    0147169C
```

The first visible-row runtime test showed that manually recoloring only the row-8/row-9
elements was insufficient: stock Exit could remain highlighted at the same time as the
custom label, and the first stock row reached after leaving ZachFix could appear without
its selection highlight until the next navigation step.

The corrected bridge now resynchronizes presentation through the **stock page-2 styling
helper itself** (`006245B0` Steam / `00624530` GOG), but only for its original COption
owner. This does not rehabilitate the helper as a generic UI API. It remains hardcoded to
ten stock rows, slot 0 and retail tables. ZachFix uses it narrowly because an intercepted
boundary move skips the exact helper call retail would normally make.

When ZachFix owns focus, the helper is called with `activeRow = -1`. Raw/decompiled body
inspection shows the row argument is only compared against loop indices `0..9` and is not
used to index a table, so `-1` produces a neutral stock selection without creating an
invalid COption row. When focus returns to row 8 or Exit, ZachFix calls the helper with
that real row and its stock choice value from `+0x204 + row*4`, restoring the same browse
presentation the native navigation path would have produced.

## Runtime validation

The post-ABI-fix Steam test confirmed that the page-2 interception itself is live. The log
showed the real COption controller at page `2`, then multiple successful selector-0 child
open/normal-removal cycles. The user could navigate onto the synthetic row and activate it,
but the label was not visible. Entering from the final setting left that setting's visual
description/value behind; entering from Exit left the corresponding area blank.

That behavior is consistent with ZachFix consuming the navigation edge before stock
`FUN_006245B0` updates the presentation state. The next runtime test made this even more
specific: the custom label became visible, but stock Exit could remain red while ZachFix
was also red, and the first stock row reached after leaving ZachFix lacked its highlight
until another movement caused retail to run the helper again. This is a one-step-late
presentation state, not a selection/lifecycle failure.

The corrected visual bridge therefore treats selection state and presentation state
separately: row `9` remains the safe shadow for stock rendering, the stock helper is called
with no active retail row while ZachFix is selected, and it is explicitly rerun for row 8
or Exit when focus leaves the custom pseudo-row. The custom label remains drawn through the runtime C-string path; the subsequent
infrastructure pass replaces the temporary geometry fallback with the live page-2 CLayout row geometry.

The subsequent highlight-resynchronization build was runtime-tested successfully. The
user confirmed correct focus transfer in both directions across
`row 8 -> ZachFix -> Exit` and `Exit -> ZachFix -> row 8`, with exactly one selected row.
The selector-0 child continued to open and request normal manager removal cleanly.

The same successful run still reported `row8=missing`, `exit=missing` and the temporary
`(400,500)` fallback. Static reinspection then identified the accessor mismatch described
above. The infrastructure pass removes that fallback and the diagnostic geometry log.

The following Steam runtime test confirmed the infrastructure pass itself: the page-2
entry remained stable, the selector-0 child opened and requested normal removal, and the
label was visibly positioned from the real layout rather than the removed fallback. The
remaining issue was presentation-only: the runtime C-string sat slightly low/right relative
to the baked retail labels. The current polish keeps the live row-8/Exit anchors but nudges
the custom text by fractions of the derived native row step (0.75 vertical step from row 8,
up to half a row step left, capped at 32 layout units). No absolute screen coordinate is
reintroduced.

## COption controller ABI

Raw ASM establishes that `FUN_00620480` consumes **two** stack arguments in addition to
`this`. Its entry reads the low byte of `[ESP+4]` as the event, while its exits use
`RET 8`. The wrapper `FUN_00621130` makes the forwarding contract explicit: it pushes
callback argument 3, pushes callback argument 2, restores callback argument 1 into `ECX`,
and calls `00620480`. The evidence-compatible controller ABI is therefore:

```cpp
void __thiscall COptionController(
    COption* this,
    uint32_t event,
    uintptr_t payload);
```

The controller does not need to read `payload` on the observed paths for it to remain part
of the calling convention. Removing it causes `00620480`'s `RET 8` to consume four bytes
more than the caller supplied, corrupting the stack. The 2026-10-04 page-2 runtime test
crashed on Options entry for exactly this reason.

## Runtime test checklist

1. Open stock Options from the title menu and from in-game Pause.
2. Verify `ZachFix Settings` appears without any F9/global-hotkey path.
3. Verify the row is positioned from the native page-2 layout rather than the old fixed
   fallback coordinate.
4. Navigate row 8 -> ZachFix Settings -> Exit and back upward; exactly one row must be
   highlighted at every step.
5. Confirm on ZachFix Settings, then close the selector-0 child with native Back/Cancel.
6. Verify return to the same Options instance with the custom row still selected.
7. Repeat several open/close cycles and inspect only for NativeUI warnings or lifecycle
   failures; the old one-shot controller/geometry diagnostic logs are intentionally gone.

## Deferred polish

The first page-2 test deliberately keeps the label as runtime C-string text. It will not
perfectly match a localized texture-backed retail row. If the page-2 doorway is stable,
the next step is to decide between a tiny ZachFix-owned native label asset and a closer
native font treatment, then connect real ZachFix configuration values.
