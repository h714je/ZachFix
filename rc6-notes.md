# ZachFix v0.1.0-rc6

RC6 focuses on controller support, automatic input switching, and a new runtime glyph-theme system.

## Highlights

- Added a native XInput backend. XiDi is no longer required for modern controller support.
- Added automatic Keyboard/Mouse ↔ Controller switching using the game's own `USEJOY` mode.
- Added proper separate LT/RT handling and corrected right-stick mapping.
- Preserved vanilla controller behavior when `NativeXInput = false`.
- Added dynamic keyboard/gamepad glyph atlas switching.
- Added custom glyph themes for both keyboard and gamepad.
- Added live glyph-theme selection in the F10 menu.
- Added glyph-theme hot reload for theme authors.
- Added safe fallback behavior for missing or unavailable glyph themes.
- Isolated managed glyph atlases from the regular texture override pipeline while Dynamic Glyph Atlas is enabled.
- Documented controller rebinding through the game's existing `configJ.cnf`.

## Native XInput

Enable the native backend in `ZachFix.ini`:

```ini
[Gamepad]
NativeXInput = true
```

ZachFix reads XInput directly and feeds Deadly Premonition the controller layout it expects internally.

When `NativeXInput = false`, ZachFix does not install the native XInput controller hooks or remap controller bindings, so the game's original WinMM controller path remains intact.

Automatic input-mode switching is configured separately:

```ini
[Input]
AutoSwitch = true
```

This allows the game to switch between keyboard/mouse and controller input at runtime while keeping its native action and prompt logic.

## Controller rebinding

Controller actions still use Deadly Premonition's existing `configJ.cnf` binding system.

With `NativeXInput = true`, the useful controller binding values are:

```text
0   A
1   B
2   X
3   Y
4   LB
5   RB
6   Back / View
7   Start / Menu
8   LS Click
9   RS Click

41  D-Pad Up
42  D-Pad Down
43  D-Pad Left
44  D-Pad Right

45  Left Stick Left
46  Left Stick Right
47  Left Stick Up
48  Left Stick Down

49  RT
50  LT

51  Right Stick Left
52  Right Stick Right
55  Right Stick Up
56  Right Stick Down
```

Manual changes to `configJ.cnf` may be overwritten by DPLauncher.

## Glyph themes

RC6 adds a separate glyph-theme system for keyboard and gamepad prompts.

Example structure:

```text
ZachFix/
└─ glyphs/
   ├─ keyboard/
   │  ├─ redseed.png
   │  └─ minimal.dds
   └─ gamepad/
      ├─ xbox.tga
      ├─ playstation.png
      └─ retroxbox.dds
```

Configure the active themes in `ZachFix.ini`:

```ini
[Glyphs]
DynamicAtlas = true
HotReload = true
KeyboardSet = Native
GamepadSet = xbox
```

Theme names are simply filenames without extensions.

Supported theme file extensions are searched in this order:

```text
.dds
.png
.tga
```

`Native` uses the game's captured original atlas when available. The gamepad path can also fall back to `gamepad\native.*`, then to `gamepad\xbox.*` when needed.

Glyph themes can also be changed live from the F10 menu.

## Glyph hot reload

With:

```ini
[Glyphs]
HotReload = true
```

theme files can be edited and saved while the game is running. ZachFix detects changes and reloads the active glyph texture without requiring a restart.

If a file is temporarily unavailable or invalid while being saved, ZachFix keeps the last working atlas instead of replacing it with a broken texture.

## Texture override compatibility

When `DynamicAtlas = true`, the keyboard and gamepad glyph atlases are managed by the glyph-theme system and are excluded from the regular texture override path.

When `DynamicAtlas = false`, the traditional DPFix-compatible texture override behavior remains available.

## Notes

- `DynamicAtlas` enable/disable is restart-required.
- Glyph theme selection and glyph hot reload operate at runtime.
- Automatic input switching remains independent from the native XInput backend.
