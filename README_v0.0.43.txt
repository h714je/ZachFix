# DPFix-NG v0.0.43 Texture Live Workflow

This checkpoint turns Texture Hot Reload into a texture-maker workflow rather than an
"already-overridden textures only" refresh.

## Live workflow

A regular D3DXCreateTextureFromFileInMemoryEx texture is now tracked whether or not an
override existed when it first loaded. The DPFix-compatible hash and the original D3DX
load parameters are attached to the texture as D3D9 private data.

This enables the intended loop without leaving Deadly Premonition:

    play / find texture
        -> Dump Textures
        -> edit or AI-upscale <hash>.tga
        -> save <hash>.dds or <hash>.png into DPFixNG\textures\override
        -> Reload Overrides
        -> see the result when that texture is next bound (normally the same/next frame)
        -> edit again and repeat

No scene transition, D3D9 Reset, or game restart is required.

## Reload semantics

Reload Overrides increments a generation. On the next SetTexture for each tracked texture:

- no override file: use the baseline texture;
- newly added override: create it and activate it;
- changed override: create the new resource first, then atomically replace the previous one;
- removed override: clear the hot replacement and return to the baseline texture;
- invalid/half-written override: keep the previous working texture and report a failure.

"Baseline" means the IDirect3DTexture9 originally returned to the game. In the normal texture-maker
workflow where the game was started without that override, baseline is the original game texture.
If an override was already present at initial load, baseline is that initial override resource.

Missing overrides are a normal rescan result and are not counted as failures or logged per texture.
This keeps a reload generation from flooding DPFixNG.log while it checks ordinary textures.

## Lifetime

No new D3D9 device hook, Release hook, or global COM-pointer registry is added.

- Metadata is copied into the baseline resource with SetPrivateData.
- The current hot replacement is stored in a second private-data slot using D3DSPD_IUNKNOWN.
- D3D9 owns that COM reference and releases it when replaced, cleared, or when the baseline dies.
- SetTexture obtains a temporary AddRef'd pointer and releases it immediately after the bind.

This remains separate from the render-target Hot Apply replacement manager and Runtime Resource Audit.

## Compatibility

Unchanged from the previous production checkpoints:

- original DPFix SuperFastHash and 8-digit filenames;
- DPFix 0x7FFFFFFF source-size behavior;
- DPFixNG\textures\override first, legacy dpfix\tex_override fallback;
- DDS before PNG, matching original DPFix preference;
- DimensionMode=DPFix and DimensionMode=Preserve;
- Texture Inspector and dump support;
- 11 existing D3D9 device hooks only;
- no participation in render-target replacement resources.

The rescan currently tracks the D3DXCreateTextureFromFileInMemoryEx path, which is the same path
used by DPFix-NG's compatible override loader and by the Deadly Premonition textures observed during
current testing.
