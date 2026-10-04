# Native UI / in-game custom windows

**Research snapshot:** 2026-10-04
**Status:** selector-0 lifecycle and stock COption page-2 doorway runtime-validated; infrastructure integration stabilized on Steam 1.01b
**Production status:** no ZachFix native-menu implementation is shipped in a release yet

This branch records the native UI architecture needed to build ZachFix-owned in-game
pages without using ImGui, claiming a global `CLayout` slot, or coercing the retail
`COption` controller into a custom settings shell.

The final static result is that the game already contains a lightweight native task
pattern suitable for this purpose:

```text
stock Title/Pause -> COption path
    -> external ZachFix pseudo-row before page-2 Exit
    -> external WAIT_ZACHFIX integration state
    -> selector-0 CRdObject task
    -> ZachFix callback at object+0x44
       event 0    initialization
       event 1    input/update
       event 0x12 render
    -> native +0x30 removal request
    -> manager cleanup/unlink/free
    -> Pause resumes on a later update tick
```

The important change from the earlier COption-centered approach is that the custom page
does not need a stock Options object or a private global layout slot. The retail engine
already uses base `CRdObject` tasks with arbitrary callbacks for menu-like controllers.

See `../evidence/native_ui/README.md` for the exact addresses, build mappings, evidence,
and runtime contract.

The first implementation of that contract lives in the 0.3.0 development work as
`src/zachfix/ui/native_settings.cpp`. The selector-0 lifecycle, the retired title Main
Menu row-3 experiment, and the final stock COption page-2 doorway have all been
runtime-validated on Steam 1.01b. The visible entry is one externally-owned pseudo-row
inside stock COption page 2, immediately before native Exit. This makes the same entry
reachable through both Title -> Options and Pause -> Options without turning COption
into the custom page shell. The old F9 development path and shared Title/Pause hook have
now been removed entirely. See `native-settings-runtime-poc.md`,
`native-settings-main-menu-entry.md`, and `native-settings-options-entry.md`.

## 1. Retail generic task substrate

Steam `FUN_006C5930` is a small object factory. Selector `0` allocates `0x160` bytes,
runs the base `CRdObject` constructor `FUN_00402650`, and registers the object with the
engine manager through `FUN_006C5AE0`.

The retail reference path `FUN_00635460 -> FUN_006348D0` proves that selector `0` is not
merely a theoretical object type. Retail code uses it as a native menu/task controller:

```text
FUN_00635460
    -> FUN_006C5930(0, 0, 0, 1)
    -> base CRdObject
    -> FUN_006BAB80(FUN_006348D0, 0)

FUN_006348D0
    event 0    -> initialize menu state
    event 1    -> poll native input / update selection
    event 0x12 -> draw native text
    terminal   -> vtable +0x30 removal request
```

The callback ABI is:

```cpp
void __cdecl Callback(CRdObject* object,
                      uint32_t event,
                      uintptr_t payload);
```

`FUN_006BAB80` stores the callback at `object+0x44` and dispatches **event 0
synchronously before returning**. Any ZachFix external state keyed by `CRdObject*` must
therefore exist before calling the setter.

## 2. Native lifecycle

The generic task is manager-owned after registration.

Relevant Steam virtuals on the base `CRdObject` vtable are:

```text
+0x00  FUN_0062D170  final delete-capable virtual
+0x08  FUN_006BA850  cleanup-stage virtual
+0x0C  FUN_006BA8A0  callback event-1 dispatcher
+0x30  FUN_006BAB20  normal removal request
```

GOG equivalents are:

```text
+0x00  FUN_005F4DD0
+0x08  FUN_006BA7A0
+0x0C  FUN_006BA7F0
+0x30  FUN_006BAA70
```

`+0x30` is **not** a scalar deleting destructor. It marks/schedules the object for
removal. Final cleanup is deferred to the manager, which later runs cleanup, unlinks the
record and invokes `+0x00(1)` so the game allocator performs the final free.

The callback must treat a `+0x30` request as terminal for that invocation and must not
free the object itself.

## 3. Native input and sounds

The reference task polls the same native menu input abstraction used elsewhere by the
game. It does not need the COption-only helper wrappers.

Confirmed logical inputs in the investigated path:

```text
Up       0x4001
Down     0x8002
Left     0x10004
Right    0x20008
Confirm  DAT_00A00218
Cancel   DAT_00A0021C
```

The reference generic menu directly uses `FUN_00404400` plus `FUN_00708910` for
polling. Native menu sounds include:

```text
0x2C9 navigation
0x2CA confirm
0x2CB cancel
```

This keeps controller/keyboard routing inside the game's own input layer.

## 4. Native rendering and arbitrary ZachFix text

Two independent native text paths are confirmed in the generic callback environment:

- `FUN_0045C9F0` draws existing native message IDs;
- `FUN_0045C680` formats a runtime C string and forwards it to the game's text
  primitive.

The second path means a first ZachFix page does **not** need new message-table entries,
XPC label textures, or a custom XLY resource. For safety, format strings must be trusted
and bounded; user-controlled data must never become a format string.

A development page can therefore start with something as small as:

```text
ZachFix Settings

> Test Option      ON
  Another Value    123
  Close
```

## 5. Why COption is not the custom-page shell

The stock `COption` path was investigated first, but it is too tightly coupled to
retail Options semantics:

- class ID `0x39`, allocation size `0x768`;
- shared global `CLayout` pool;
- hardcoded top-level category state at `this+0x1FC`;
- stock value/subselection fields beginning at `this+0x200`;
- hardcoded ten-row highlight tables and slot-0 accesses;
- confirm paths that write retail player settings;
- Pause-specific tracking through `DAT_01474CE8`;
- terminal behavior that clears stock tracking state.

The object manager can structurally hold more than one COption, but that is not evidence
that two such controllers are semantically safe.

The old idea of loading a ZachFix layout into slot 15 is also rejected. The global
layout pool contains exactly 16 slots of `0x398` bytes, slot 13 has direct retail users,
there are many dynamically indexed accesses, and COption cleanup can reset all 16
slots. No slot is a proven mod-owned allocation target.

## 6. Shared Title/Pause controller and current COption doorway

The original parent/child analogue was Pause -> stock Options. Runtime validation later
showed that the same controller also owns the title flow; the current visible ZachFix
entry is inside stock COption itself so Title and Pause share one doorway.

Steam shared Title/Pause controller callback:

```text
FUN_00642640
```

GOG homolog:

```text
FUN_00642590
```

Important shared menu globals:

```text
DAT_014736D4  active Pause controller state
DAT_014736D8  paired/return state
DAT_014736DC  selected Pause row
DAT_01474CE8  stock COption child pointer only
```

Stock selection value `2` opens Options. The retail path preserves the menu selection,
suppresses parent interaction through parent state/context gating, and uses the COption
pointer only as a stock child liveness signal.

ZachFix must **not** reuse `DAT_01474CE8` or COption's `+0x160` liveness byte. The
validated F9 lifecycle probe used ZachFix-owned shared-menu parent state:

```text
Pause state 2
    + development-only trigger
    -> create ZachFix child
    -> set PauseWaitZachFix
    -> stop the remaining Pause navigation dispatch for that update

while PauseWaitZachFix:
    -> parent keeps pause/world/backdrop ownership
    -> parent normal menu input is skipped
    -> child handles its own input/rendering

child closes:
    -> external completion generation is set
    -> native +0x30 removal is requested
    -> parent resumes on a later update tick
    -> DAT_014736DC remains unchanged
```

When entered from Pause, the shared parent remains responsible for game pause state,
HUD/backdrop, camera/world context and other scene ownership. The child must not
independently pause or unpause the game.

For the current visible integration, runtime observation identifies the root Options
screen as COption page `2`, not page `8`. The page-2 controller/draw pair is Steam
`00624FD0` / `00624810` (GOG `00624F50` / `00624790`). Its stock selector `+0x1FC`
remains inside the native `0..9` domain; ZachFix holds an external pseudo-selection
between stock row 8 and stock row 9 Exit, then opens the same selector-0 child. See
`native-settings-options-entry.md`.

## 7. Frame order and closing-state lifetime

The relevant Steam frame order is confirmed as:

```text
FUN_006C5FF0  manager update / event 1
    -> render preparation
    -> FUN_006C7420(event 0x12)
    -> FUN_006C7070 manager cleanup
```

Therefore a child that calls `+0x30` during event 1 can still receive render event
`0x12` in the same frame before physical cleanup.

External ZachFix state must survive the removal request as a closing/tombstone record:

```text
event 1 / Back
    -> state.closing = true
    -> state.completedGeneration = currentGeneration
    -> request +0x30
    -> return immediately

same-frame event 0x12
    -> callback finds closing state
    -> no-op

next parent update
    -> consume completion from an earlier frame
    -> clear WAIT_ZACHFIX
    -> resume parent input

one additional frame
    -> erase tombstone state
```

Do not dereference the retail object after the removal request to prove whether it has
already been freed.

## 8. Creation-order requirement

Because callback assignment synchronously emits event 0, the PoC creation order must be:

```text
1. Create selector-0 CRdObject.
2. Insert external state keyed by returned CRdObject*.
3. Set PauseWaitZachFix / generation state.
4. Call FUN_006BAB80 with the ZachFix callback.
   -> event 0 happens inside this call.
5. Return from the Pause integration seam.
```

Creating the map entry after `FUN_006BAB80` would lose the first initialization event.

The manager builds its event-1 update snapshot before it begins dispatching that pass,
so a child created from a parent callback during the current update does not join that
already-built event-1 snapshot. It can render in the same frame, but its first normal
input/update occurs on the next manager update. This also prevents the opening trigger
from immediately activating the first child item through the same update dispatch.

## 9. Runtime evolution and current contract

The first selector-0/F9 lifecycle probe was intentionally small and is now historical.
A visible title Main Menu row was then runtime-validated and retired. The current
COption-based contract is:

- exact-build profile gate;
- one active ZachFix task maximum;
- visible entry inside stock COption, reachable from both Title and Pause;
- external COption page-2 pseudo-selection before stock Exit;
- stock `COption+0x1FC` never outside the page-2 stock `0..9` domain;
- one externally tracked selector-0 child and its owning COption instance;
- `__cdecl` callback with event 0 init, event 1 update and event `0x12` render;
- native formatted text and native `+0x30` removal;
- no ZachFix-owned global CLayout slot;
- no XLY injection;
- no retail vtable writes;
- no manual CRdObject allocation/free;
- stock page-2 styling explicitly resynchronized at pseudo-row boundaries;
- custom-row position derived only from live page-2 CLayout geometry;
- no hardcoded `(400,500)` fallback and no global hotkey/F9 opening path.

The COption insertion is still a runtime-test implementation, not yet a production-safe
contract. See `native-settings-options-entry.md` for exact bounds and test criteria.

## 10. 2026-10-04 retail CMenu / CFade / camera policy addendum

The Mega RE Census adds two selected Steam retail chains. They improve understanding of
stock policy but do not change the ZachFix-owned task design above.

A typed CMenu numeric path can produce literal `0x55` / decimal 85, obtain a selected
CGame-backed value, subtract `999` with 32-bit arithmetic, and clamp a signed-negative
result. The target lies inside the live GameRecord envelope. That byte containment does
not prove a friendly setting/item name, list cardinality, refresh policy, or save
chronology.

A separate CMenu event-1 late-state path maps raw states 99/100/101 to CFade and camera
work. State 99 issues a selected CFade request 3 and immediately establishes state/data;
state 100 independently reacquires the fade manager and tests a `+0x164 == 7` predicate;
state 101 later reaches a CCamera mask update that clears bit 1 and sets bit 4. The static
chain does not prove request 3 causes state 7, that all reacquired pointers are the same
instance, or the human meaning of those state values.

This extra coupling is another reason not to treat stock CMenu/COption as a generic
ZachFix page shell. The selector-0 CRdObject task remains the least entangled retail-
proven substrate.

Exact reports:

- `../evidence/mega_re_census_2026-10-04/boundaries/cmenu_numeric_game_slot_commit.md`
- `../evidence/mega_re_census_2026-10-04/boundaries/cmenu_fade_predicate_camera_policy.md`

## 11. Current boundary

Static RE is sufficient for a development-only integrated PoC. Remaining work is:

1. runtime-validate repeated create/update/render/close/reopen cycles;
2. validate the exact Steam and GOG runtime address profiles and expected-byte guards;
3. separately design the permanent visible Pause row after the generic child is stable.

Main/Title integration remains lower priority. Its dispatcher creates selector-0 tasks,
but the bounded trace did not expose a cleaner parent-owned wait/return contract than
Pause and is entangled with broader title/world/resource state.
