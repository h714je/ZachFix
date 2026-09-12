# DPFix-NG v0.0.41 Texture Dimension Modes

This checkpoint adds an optional exact-dimension path for texture replacements while keeping
original DPFix behavior as the default. Hashing, lookup paths and all render-resource Hot Apply
behavior remain unchanged.

## [Textures] DimensionMode

DimensionMode = DPFix

- Default and compatibility mode.
- Replacement width/height are passed to D3DXCreateTextureFromFileExW as D3DX_DEFAULT.
- D3DX may round each source dimension up to a power of two.
- Existing DPFix texture packs retain their historical loading behavior.

DimensionMode = Preserve

- Replacement width/height are passed as D3DX_DEFAULT_NONPOW2.
- On a device with NPOT texture support, D3DX keeps the exact DDS/PNG file dimensions.
- Intended for exact-ratio HD replacements and bulk AI upscales such as
  2048x1184 -> 4096x2368.
- If D3DX cannot create the Preserve replacement, that override is rejected and the original
  game texture is loaded. DPFix-NG does not silently stretch it back to POT.

The setting can be changed in F10 and takes effect on subsequent override loads. Already-created
textures are not rebuilt by this checkpoint.

## Texture Inspector warnings

The inspector now identifies the active override dimension request and compares the replacement
file against IDirect3DTexture9::GetLevelDesc(0). If D3DX changes the allocation size, the UI shows
a warning and DPFixNG.log records the conversion once per hash.

Examples:

DPFix mode:
    Override file: 4096 x 2368 [NPOT]
    Load request:  DEFAULT x DEFAULT
    Loaded GPU:    4096 x 4096 [POT]
    WARNING: D3DX resized override

Preserve mode on an NPOT-capable device:
    Override file: 4096 x 2368 [NPOT]
    Load request:  DEFAULT_NONPOW2 x DEFAULT_NONPOW2
    Loaded GPU:    4096 x 2368 [NPOT]
    Preserve confirmed

## Compatibility

Unchanged from v0.0.40:

- Original DPFix SuperFastHash behavior and 8-digit names.
- DPFix 0x7FFFFFFF source-size handling.
- Preferred overrides: DPFixNG\textures\override\<hash>.dds / .png
- Legacy fallback: dpfix\tex_override\<hash>.dds / .png
- Texture dumping: DPFixNG\textures\dump\<hash>.tga
- Texture Inspector source/GPU metadata.
- No new D3D9 device hooks.
- No participation in the render-target replacement registry.

All other production features remain unchanged: internal-resolution scaling, shadows,
reflections, DoF, pixel-offset fixes, world-detail extension, Hot Apply, input isolation and
Runtime Resource Audit.
