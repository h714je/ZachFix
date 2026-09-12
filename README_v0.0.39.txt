# DPFix-NG v0.0.39 Texture Compatibility

This checkpoint adds the first production Texture Override implementation on top of the
v0.0.38 production-only baseline.

## DPFix-compatible texture identity

Deadly Premonition loads normal game textures through d3dx9_43.dll. DPFix-NG hooks:

- D3DXCreateTextureFromFileInMemory
- D3DXCreateTextureFromFileInMemoryEx

Texture identity intentionally matches original DPFix:

- Paul Hsieh SuperFastHash, including DPFix's signed-byte tail behavior.
- Hash input is the original encoded D3DX source block, not decoded GPU pixels.
- DPFix's special 0x7FFFFFFF source-size handling is preserved.
- Filenames use 8 lowercase hexadecimal digits, e.g. 12ab34cd.dds.

This makes existing DPFix texture hashes reusable without conversion.

## Override lookup

When [Textures] EnableOverride=true, DPFix-NG checks in this order:

1. DPFixNG\textures\override\<hash>.dds
2. DPFixNG\textures\override\<hash>.png
3. dpfix\tex_override\<hash>.dds
4. dpfix\tex_override\<hash>.png

The final two paths are a compatibility fallback so an existing original-DPFix texture pack
can remain in its old directory.

Overrides affect newly loaded textures. Existing live textures are not rebuilt yet; explicit
"Reload Texture Overrides" support is planned for a later checkpoint.

## Texture dumping

When [Textures] DumpTextures=true, original textures are dumped once per observed hash to:

    DPFixNG\textures\dump\<hash>.tga

The filename hash is compatible with original DPFix. If an override is already active for a
hash, DPFix-NG intentionally does not dump the replacement as if it were the original.

The F10 UI shows observed source loads, unique hashes, override hits, dump counts/failures,
and the most recently observed hash.

## Existing production features

All v0.0.38 production behavior remains: internal-resolution scaling, shadows, reflections,
DoF, pixel-offset fixes, world-detail extension, Hot Apply, input isolation and Runtime
Resource Audit. Texture override does not use the render-target replacement-resource layer.

## Suggested smoke test

1. Start with EnableOverride=true and DumpTextures=false; verify normal gameplay.
2. Enable DumpTextures in F10, Apply, then enter a new area or otherwise load new textures.
3. Confirm DPFixNG\textures\dump receives <hash>.tga files.
4. Pick one dumped hash, place a visibly edited <hash>.dds or <hash>.png in
   DPFixNG\textures\override, then reload the area/restart so the texture is loaded again.
5. Repeat with the file in legacy dpfix\tex_override to verify DPFix-pack compatibility.
6. Exercise InternalScale Hot Apply and confirm Runtime Resource Audit remains balanced.
