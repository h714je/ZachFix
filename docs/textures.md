# Texture overrides

## DPFix-compatible identity

ZachFix uses the same 8-digit lower-case SuperFastHash naming convention as original DPFix texture replacement.

Preferred override paths:

```text
ZachFix\textures\override\<hash>.dds
ZachFix\textures\override\<hash>.png
```

Legacy DPFix-compatible override locations are also discovered automatically.

## Basic settings

```ini
[Textures]
EnableOverride = true
DeveloperMode = false
DimensionMode = DPFix
DumpTextures = false
```

### EnableOverride

Enables normal texture replacement.



### DimensionMode

`DPFix` reproduces original DPFix/D3DX sizing behavior. NPOT replacement dimensions can be rounded upward.

`Preserve` requests the replacement file's exact dimensions and is recommended for true-size AI/upscaled texture packs when the device supports NPOT textures.

The selected mode applies to new override loads. In Developer Mode, hot reload uses the currently applied mode.

## Developer Mode

`DeveloperMode = true` changes texture ownership so the game's original logical texture is retained as the baseline and replacement textures are substituted at bind time.

This enables a texture-authoring workflow where overrides can be added, edited, removed, and rescanned while the game is running.

Changing Developer Mode on or off requires a restart.

With Developer Mode active, F10 provides **Reload Overrides** and a Texture Inspector with source/GPU dimensions and current override information.

## Texture dumping

With Developer Mode enabled:

```ini
DumpTextures = true
```

Original textures are dumped as:

```text
ZachFix\textures\dump\<hash>.tga
```

Dumping applies to newly observed texture loads during the session.

## Glyph interaction

When Dynamic Glyph Atlas is active, the game's keyboard/gamepad glyph atlases are isolated from the generic texture-override path so glyph themes and texture packs do not compete for ownership of the same textures.
