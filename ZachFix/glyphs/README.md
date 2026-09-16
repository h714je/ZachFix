# Glyph themes

This directory contains optional ZachFix keyboard and gamepad glyph themes.

Theme files use a simple filename-based layout:

```text
ZachFix/
`-- glyphs/
    |-- keyboard/
    |   `-- <theme>.dds/.png/.tga
    `-- gamepad/
        `-- <theme>.dds/.png/.tga
```

Select the active themes in `ZachFix.ini` or from the F10 menu:

```ini
[Glyphs]
DynamicAtlas = true
HotReload = true
KeyboardSet = Native
GamepadSet = xbox
```

The theme name is the filename without its extension. ZachFix searches DDS first, then PNG, then TGA.

`Native` uses the original Deadly Premonition atlas captured during the current run when that atlas exists. If DP did not instantiate that input family's native atlas, an optional `native.dds`, `native.png`, or `native.tga` in the matching directory can provide it. Gamepad themes can also fall back to `xbox.*` when the requested set is unavailable.

The release package intentionally does not redistribute Deadly Premonition's original texture assets. You can provide your own themes or compatible atlas files in these folders.

Custom themes must preserve the atlas layout expected by the game. The supported Steam build uses a 1024x1408 keyboard/mouse atlas and a 1024x1024 gamepad atlas.

With `HotReload = true`, editing or replacing the active theme file is detected at runtime. Theme selection from F10 is also live. Enabling or disabling `DynamicAtlas` itself requires a restart.
