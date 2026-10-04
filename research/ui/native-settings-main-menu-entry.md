# ZachFix Settings Main Menu entry

**Development snapshot:** 2026-10-04
**Target:** Steam/GOG 1.01b
**Status:** runtime-validated on Steam 1.01b; retired as the primary UX in favor of the stock Options/COption entry

## Why row 3

The shared Title/Pause controller (`00642640` Steam, `00642590` GOG) owns the title Main
Menu when controller state `DAT_014736D4 == 2` and the current scene/menu mode at
`sceneContext+0x164` is `2`. The title selection is `DAT_014736DC`.

Retail navigation explicitly excludes selection value `3`:

```text
Down from 2 increments to 3, then immediately increments again to 4.
Up from 4 decrements to 3, then immediately rewrites 3 to 2.
```

Retail Confirm dispatch likewise has no `case 3`; its stock branches are `0`, `1`, `2`,
`4`, and `5`. The event-`0x12` title renderer draws message IDs `0x3A49`, `0x3A4A`,
`0x3A4B`, then `0x3A4D`, leaving the corresponding visual/message-number gap.

This makes row 3 a narrower insertion point than replacing Options, Exit, or another
retail item. ZachFix does not enlarge a retail array or claim a CLayout slot.


## Runtime result and retirement

The Steam 1.01b test on 2026-10-04 succeeded: the row rendered in the title Main Menu,
keyboard/controller navigation reached it, Confirm opened the selector-0 child, and two
open -> deferred-removal cycles completed normally. This closes the Main Menu row as a
useful runtime proof.

The visible row is intentionally retired in the next development step. The better UX is
to enter ZachFix through stock **Options**, because the same COption controller is
reachable from both the title Main Menu and the in-game Pause Menu. The temporary F9
fallback survived only until that COption doorway was runtime-validated; the later
infrastructure pass removes the F9/shared-menu hook entirely.

## Validated integration behavior (retired)

`src/zachfix/ui/native_settings.cpp` used this runtime-validated experiment to:

1. identifies Main Menu with both controller state `2` and scene mode `2`;
2. turns only the stock `2 -> 4` navigation gap into `2 -> 3 -> 4`;
3. draws `ZachFix Settings` through `0045C680` / `0045C6B0`;
4. anchors the label between existing native layout elements 6 and 5 from layout
   object `01473E74`;
5. intercepts Confirm only while selection is exactly `3`;
6. opens the already-validated selector-0 ZachFix child;
7. suppresses parent input/render while the child is active;
8. resumes the same title controller after deferred native removal.

When moving away from custom row 3, navigation is handed back to the original callback
before it evaluates rows 4+, preserving stock availability checks. Upward navigation
from lower stock rows is post-adjusted only when retail's own skip lands directly on
Options (`2`).

## Build map

| Surface | Steam | GOG |
|---|---:|---:|
| shared menu controller | `00642640` | `00642590` |
| scene context getter | `00427780` | `004277A0` |
| layout element accessor | `00459080` | `004590B0` |
| formatted runtime text | `0045C680` | `0045C6B0` |
| Main Menu layout | `01473E74` | `01473E74` |
| controller state | `014736D4` | `014736D4` |
| title selection | `014736DC` | `014736DC` |

## Runtime test

The validated title-menu order for that development build was conceptually:

```text
...
Options
ZachFix Settings
<next stock item>
...
```

Test keyboard and controller navigation in both directions, including configurations
where a lower stock row is unavailable. Confirm on `ZachFix Settings` must open the
native child; Back/Cancel must return to the title menu with row 3 still selected.
Repeated open/close cycles in this historical experiment logged normal manager removal.
The later COption integration supersedes both this visible row and its temporary F9
fallback.

The custom row does not yet reproduce the stock title-menu pulse/scale animation or its
menu-navigation sound on the one transition that enters row 3. Those are presentation
follow-ups, not prerequisites for validating ownership and dispatch.
