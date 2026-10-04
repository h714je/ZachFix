# ZachFix Settings native runtime PoC

**Development snapshot:** 2026-10-04
**Scope:** first runtime lifecycle proof for the 0.3.0 Native UI work
**Status:** selector-0 lifecycle runtime-validated on Steam 1.01b; superseded as the primary opening path by the COption integration

This proof deliberately stops before permanent menu integration. It exists to validate
one narrow question first: can ZachFix create, drive, render, and remove its own
manager-owned native UI task without borrowing `COption`, claiming a `CLayout` slot, or
owning the game's Pause lifecycle?

## Runtime validation result

The first Steam 1.01b test on 2026-10-04 succeeded. The supplied log recorded three
consecutive `Opened ZachFix Settings selector-0 native task` -> `child requested normal
manager removal` cycles with no NativeUI warning between them. The test was performed
from the title Main Menu, which exposed an important semantic correction: the hooked
`00642640` / `00642590` controller is shared by Title and Pause rather than being a
Pause-only callback.

That result validates the custom callback residency, event-0/update/render contract,
native input path, formatted native text, deferred `+0x30` removal, and repeated
open/close lifetime. A later visible Main Menu row was also runtime-validated, then
retired in favor of using stock COption as the single Title/Pause doorway. The F9 path
was kept only during bring-up and was removed after the COption doorway/highlight cycle
was runtime-validated.

## Runtime contract

The implementation lives in:

```text
src/zachfix/ui/native_settings.cpp
src/zachfix/ui/native_settings.h
```

Build-specific entry points are recorded in `NativeUiBuildProfile` in
`src/zachfix/core/main_exe.h` / `.cpp`.

The original prototype installed a build/signature-gated hook on the shared Title/Pause controller callback:

```text
Steam  FUN_00642640
GOG    FUN_00642590
```

No native child exists at startup. In the original lifecycle probe, pressing **F9**
while the shared menu controller was in its interactive state performed the temporary
development-only open path:

```text
shared-menu event 1
    -> F9 edge
    -> selector-0 factory
    -> external ZachFix state published
    -> callback setter
       -> synchronous event 0
    -> parent update returns
```

The child uses only proven native surfaces:

```text
selector-0 CRdObject task
object+0x44 callback
manager event 1     input/update
manager event 0x12  rendering
vtable +0x30        deferred removal request
```

## Test page

The first page is intentionally tiny:

```text
ZachFix Settings

> Test Option        ON
  Another Value      123
  Close
```

Native input behavior:

- Up / Down moves the selection using the game's menu input path.
- Confirm on `Test Option` toggles ON/OFF.
- `Another Value` is intentionally inert in this lifecycle proof.
- Confirm on `Close`, or native Cancel/Back, requests normal manager removal.

Normal page navigation uses only DP's native input abstraction. During the original
lifecycle proof F9 served as a parent-side trigger and emergency escape hatch. That was a
test-only safety measure and is no longer present in the current COption integration.

## Parent/child ownership

While the child is active, its current parent callback remains installed and its object
remains manager-owned, but ZachFix suppresses that parent's event-1 navigation and
event-0x12 rendering. In the current implementation the only visible parent is COption; the shared
Title/Pause controller is no longer hooked by the Native Settings bridge.

When the child requests `+0x30` removal, its external state becomes a closing tombstone.
Any same-frame event `0x12` becomes a no-op. The first later parent update is consumed so
the Cancel/Confirm edge cannot leak through; the following parent update clears the
external record and resumes the retail callback.

## Safety gates

The bridge is enabled only for the existing exact Steam/GOG 1.01b build profiles and
additionally validates native prologues for:

- COption callback for the current visible entry;
- selector-0 task factory;
- callback setter;
- input prepare helper;
- input poll helper;
- formatted C-string renderer.

The removal call is made through the task's `vtable+0x30` only if that virtual resolves
to the build-specific proven CRdObject removal request function. A mismatch is treated
as a hard stop for closing rather than calling an unknown virtual.

## Historical runtime validation checklist (superseded)

The first in-game lifecycle proof used this temporary procedure; it is retained only as
a record of what was validated. The current COption integration has no F9 hook.


1. Start a normal game and open the stock Pause Menu.
2. Press F9 once.
3. Confirm the stock Pause rows disappear and `ZachFix Settings` appears.
4. Test Up and Down on keyboard and controller.
5. Toggle `Test Option` with Confirm.
6. Close with native Back/Cancel.
7. Confirm the stock Pause Menu returns with its previous selected row intact.
8. Repeat open/close several times.
9. Repeat using the `Close` row rather than Back.
10. Open it once more and verify F9 also closes it as the development escape hatch.
11. Inspect `ZachFix.log` for `[NativeUI]` warnings.

Also test an F9 press outside Pause. It must do nothing.

## Not part of this PoC

The following are intentionally deferred until lifecycle runtime validation passes:

- permanent `ZachFix Settings` row in Pause or stock Options;
- real ZachFix config bindings;
- left/right value editing;
- native navigation/confirm/cancel sounds;
- localization;
- custom XLY/XPC resources;
- removal of the existing F10 ImGui settings UI.

The preferred eventual UX remains a permanent `ZachFix Settings` entry that opens a
ZachFix-owned native child page. The stock `COption` controller remains retail-owned.
