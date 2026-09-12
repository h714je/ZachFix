# DPFix-NG v0.0.45 Smart Texture Filtering

This checkpoint restores the texture-filtering feature from original DPFix, but uses a deliberately safer sampler policy instead of globally forcing anisotropic filtering onto every texture stage.

## Configuration

    [Filtering]
    Mode = Original
    MaxAnisotropy = 16

Modes:

- Original
  - no filtering override;
  - sampler states requested by Deadly Premonition pass through unchanged;
  - texture-bind filtering classification is skipped.

- Bilinear
  - compatibility mode inspired by original DPFix filteringOverride=1;
  - on ordinary 2D textures, POINT/NONE MINFILTER becomes LINEAR;
  - POINT/NONE MIPFILTER becomes LINEAR;
  - render targets, depth resources and dynamic textures are excluded.

- Anisotropic
  - smart AF mode;
  - only ordinary 2D textures with more than one mip level are candidates;
  - render targets, depth resources and dynamic textures are excluded;
  - intentional POINT/NONE minification is preserved;
  - when the game already requests LINEAR or ANISOTROPIC minification, MINFILTER is upgraded to ANISOTROPIC;
  - MAGFILTER is never forced;
  - the game's MIPFILTER policy is preserved;
  - MAXANISOTROPY is set to the requested value, clamped to the D3D9 device capability.

MaxAnisotropy accepts 2..16. It only affects Anisotropic mode.

## Why this differs from original DPFix

Original DPFix filteringOverride=2 aggressively forced anisotropic MIN/MAG filtering and a high anisotropy value across sampler stages. That was useful, but it could also touch UI, post-processing and other special-purpose sampling.

DPFix-NG instead classifies the texture currently bound to each pixel sampler. Render targets, depth resources and dynamic textures are left alone. Smart AF additionally requires mipmaps and refuses to replace intentional point sampling. This keeps the useful world-texture improvement while reducing the chance of changing UI or post-process behavior.

This is intentionally not a byte-for-byte recreation of filteringOverride=2.

## Live Apply

Filtering Mode and Max Anisotropy can be changed from the F10 UI and applied without restarting the game.

DPFix-NG tracks the game-requested sampler state while filtering is active. Switching back to Original restores those game values. When enabling filtering from Original, the current game-visible sampler and texture state is captured before the override is applied.

The SetSamplerState hook ignores calls that do not originate from DP.exe, so Dear ImGui rendering does not become part of the game's filtering state cache.

## Hook surface

v0.0.45 adds one D3D9 device hook:

- IDirect3DDevice9::SetSamplerState

The production device-hook count therefore changes from 11 to 12.

The existing SetTexture hook is reused to classify the effective bound texture. No COM ownership hook and no new texture lifetime registry are introduced.

## Texture Developer Mode

The v0.0.44 Texture Developer Mode behavior remains unchanged:

- DPFix-compatible hashing and legacy pack lookup;
- DPFix / Preserve dimension modes;
- original-texture dumping;
- live add/edit/remove override workflow;
- hot reload and rollback to the original texture.

Filtering is independent from Texture Developer Mode and works in normal production gameplay.

## Suggested test

1. Start with:

       [Filtering]
       Mode = Original

2. Find a road or ground surface viewed at a shallow angle.
3. Open F10, select Anisotropic (smart), leave Max Anisotropy at 16x and press Apply.
4. Compare distant/oblique texture detail.
5. Toggle back to Original and press Apply. The visual state should revert without restarting.
6. Check UI, menus, DoF, reflections and shadows for regressions.

On unsupported hardware the requested anisotropy is clamped to the device capability; if anisotropic minification is not supported, smart AF remains conservative instead of forcing an invalid state.
