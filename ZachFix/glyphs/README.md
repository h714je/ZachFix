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

## Bundled gamepad themes

ZachFix currently bundles four clean controller themes built from the CC0 Kenney Input Prompts asset set:

- `xbox.png`
- `playstation.png`
- `steamdeck.png`
- `switch.png`

The generated atlases are distributed with ZachFix. The full upstream Kenney source-asset pack is intentionally not stored in this repository. See `THIRD_PARTY.md` and `licenses/Kenney-Input-Prompts.txt` for provenance and license information.

## Gamepad atlas authoring layout

The gamepad atlas is `1024x1024` and uses a base grid of `128x128` cells. Rows and columns below are zero-based and written as `R<row>C<column>`.

Rows `0` through `6` use ordinary `128x128` cells. Row `7` contains four logical `256x128` regions spanning column pairs `0-1`, `2-3`, `4-5`, and `6-7`.

For custom themes, preserve these UV regions. A useful visual target for ordinary cells is about `120x120` visible content, centered with roughly 4 px of transparent padding on each side. Preserve aspect ratio when fitting non-square artwork.

Known controller regions:

| Region | Meaning |
| --- | --- |
| `R0C0` | A / Cross semantic action |
| `R0C1` | B / Circle semantic action |
| `R0C2` | X / Square semantic action |
| `R0C3` | Y / Triangle semantic action |
| `R0C4` | D-pad |
| `R0C5` | D-pad Up |
| `R0C6` | D-pad Down |
| `R0C7` | D-pad Left |
| `R1C0` | D-pad Right |
| `R1C1` | Left Stick |
| `R1C2` | Right Stick |
| `R1C3..R1C6` | Four-frame loading animation |
| `R1C7` | Start / Menu / Options / Plus |
| `R2C0` | Select / View / Create / Minus |
| `R2C1` | LB / L1 / L |
| `R2C2` | RB / R1 / R |
| `R2C3` | LT / L2 / ZL |
| `R2C4` | RT / R2 / ZR |
| `R2C5` | L3 / Left Stick Press |
| `R2C6` | R3 / Right Stick Press |
| `R2C7` | Up/Down combined direction |
| `R3C0` | Left/Right combined direction |
| `R3C1` | Continue / scroll-down indicator |
| `R3C2` | `TM` legacy/unobserved glyph |
| `R3C3` | circled `R` legacy/unobserved glyph |
| `R7C0-1` | Left-stick left motion prompt |
| `R7C2-3` | Left-stick right motion prompt |
| `R7C4-5` | Right-stick left motion prompt |
| `R7C6-7` | Right-stick right motion prompt |

Rows containing old motion-controller artwork are intentionally not treated as required authoring targets unless runtime testing proves they are used by the PC build.

Nintendo face-button labels use Nintendo's physical layout while preserving DP's semantic action slots: the A/B and X/Y labels are swapped relative to the Xbox-labelled atlas.

## Keyboard/mouse atlas

The supported PC builds use a `1024x1408` keyboard/mouse atlas. Custom keyboard themes must preserve the original relative UV/layout structure.

## Runtime behavior

With `HotReload = true`, editing or replacing the active theme file is detected at runtime. Theme selection from F10 is also live. Enabling or disabling `DynamicAtlas` itself requires a restart.

While `DynamicAtlas = true`, the managed keyboard/gamepad atlas textures are excluded from the generic texture-override pipeline so the two systems cannot compete for the same textures.
