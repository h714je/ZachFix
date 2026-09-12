# DPFix-NG v0.0.44 Texture Developer Mode

This checkpoint separates normal gameplay texture overrides from the live texture-maker workflow.

## Production mode (default)

    [Textures]
    EnableOverride = true
    DeveloperMode = false

Production mode keeps the DPFix-compatible override path only:

- original DPFix SuperFastHash / 8-digit names;
- DPFixNG\textures\override first, legacy dpfix\tex_override fallback;
- DDS before PNG;
- DimensionMode=DPFix or Preserve.

It intentionally does NOT attach texture-maker state:

- no per-texture hot-reload metadata;
- no D3DSPD_IUNKNOWN replacement slot;
- no retained original baseline for live rollback;
- no generation/rescan work on SetTexture;
- no Texture Inspector tracking;
- no texture dumping.

When EnableOverride=false and DeveloperMode=false, the extended D3DX hook immediately passes through to the original loader without hashing. The non-Ex D3DX path is also a direct passthrough.

## Texture Developer Mode

    [Textures]
    DeveloperMode = true

Developer Mode is restart-only because it changes texture ownership for the whole session.

For D3DXCreateTextureFromFileInMemoryEx loads, Deadly Premonition always receives the ORIGINAL game texture as its logical IDirect3DTexture9. DPFix-NG stores an override separately and substitutes it only when the logical texture is bound through SetTexture.

This makes the live cycle fully symmetric:

    original
      -> add <hash>.png/.dds -> Reload Overrides -> override active
      -> edit file           -> Reload Overrides -> override refreshed
      -> delete file         -> Reload Overrides -> original restored

An invalid or half-written replacement does not destroy the last working replacement.

## Dump / Inspector

DumpTextures and Texture Inspector are Developer Mode tools. Dumping always uses the real original game texture, even if an override was already present when the texture loaded.

Preferred workflow:

    play / find texture
      -> enable Dump Textures
      -> obtain DPFixNG\textures\dump\<hash>.tga
      -> edit / AI upscale
      -> save DPFixNG\textures\override\<hash>.png or .dds
      -> Reload Overrides
      -> inspect result immediately in game

## Runtime safety

DeveloperMode is captured once when the D3DX hooks initialize. Changing the checkbox/INI during a running game only configures the next launch; the current session does not switch ownership models mid-flight.

No new D3D9 device hooks were added. Texture live reload continues to share the existing SetTexture hook while remaining separate from the render-target Hot Apply resource manager.
