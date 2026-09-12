# DPFix-NG v0.0.40 Texture Inspector

This checkpoint keeps the v0.0.39 DPFix-compatible texture override behavior unchanged and
adds observational texture-dimension diagnostics for HD texture-pack work.

## Texture Inspector

The F10 UI now exposes a Texture Inspector with two snapshots:

- Last observed: the most recently completed D3DX texture load.
- Last override hit: the most recent texture that was successfully replaced.

For each record DPFix-NG reports:

- DPFix-compatible 8-digit hash.
- D3DX entry point (InMemory / InMemoryEx).
- Original source image width, height, mip count, format and image-file type.
- Width/height/mip/format requested by D3DXCreateTextureFromFileInMemoryEx.
- Whether an override was used and whether it came from DPFixNG\textures\override or the
  legacy dpfix\tex_override path.
- Override DDS/PNG source dimensions and metadata.
- Actual level-0 width/height, mip count and D3D format of the IDirect3DTexture9 returned to DP.
- POT/NPOT status.
- Override/original source scale and file-to-GPU dimension scale when both sides are known.

Source image metadata is read using D3DXGetImageInfoFromFileInMemory / D3DXGetImageInfoFromFileW.
Actual GPU allocation dimensions are read from IDirect3DTexture9::GetLevelDesc(0). The inspector
never changes dimensions, formats, mip levels, filtering or texture data.

Unique texture dimensions are also logged once per hash to DPFixNG.log. A successful override
gets an additional one-time log line describing the override file and resulting GPU dimensions.

## Compatibility behavior remains unchanged

Texture identity and override lookup are still the v0.0.39 behavior:

- Paul Hsieh SuperFastHash with original DPFix signed-byte tail behavior.
- Hash input is the original encoded D3DX source block.
- DPFix 0x7FFFFFFF source-size handling is preserved.
- Preferred lookup:
    DPFixNG\textures\override\<hash>.dds
    DPFixNG\textures\override\<hash>.png
- Legacy fallback:
    dpfix\tex_override\<hash>.dds
    dpfix\tex_override\<hash>.png
- Replacement loading still uses D3DX_DEFAULT width/height exactly as v0.0.39 did.

No NPOT/POT policy is changed in this checkpoint. The inspector exists specifically so the
actual behavior can be measured before introducing any optional modern dimension mode.

## Suggested atlas test

1. Load an unmodified texture with override disabled and inspect Last observed.
2. Note Original source and Loaded GPU dimensions.
3. Enable the replacement and reload the texture/area.
4. Inspect Last override hit.
5. Compare Original source, Override file and Loaded GPU dimensions.
6. For an atlas such as 2048x1184 -> 4096x2048, check the reported source scaling and whether
   D3DX changes the replacement allocation at all.

All existing production features remain unchanged: internal-resolution scaling, shadows,
reflections, DoF, pixel-offset fixes, world-detail extension, Hot Apply, input isolation,
Runtime Resource Audit, DPFix-compatible texture dumping and texture override.
