# DPFix-NG v0.0.42 Texture Hot Reload fix1

This checkpoint adds live refresh for texture overrides without a scene reload, D3D9 Reset,
or a new device hook. Existing DPFix-compatible hashing, DimensionMode behavior and render-target
Hot Apply remain unchanged.

## fix1: D3DSPD_IUNKNOWN crash fix

v0.0.42 initially passed the address of a local `IUnknown*` variable to `SetPrivateData`
with `D3DSPD_IUNKNOWN`. D3D9 expects `pData` itself to be the COM interface pointer; it then
calls `AddRef` through that pointer. The old call therefore made the stack address look like a COM
object and could crash on the first tagged texture after pressing Reload Overrides.

fix1 passes the actual `IUnknown` interface pointer and keeps `SizeOfData = sizeof(IUnknown*)`.
The metadata slot and the rest of the lazy generation/reload design are unchanged.

## F10: Reload Overrides

Press "Reload Overrides" after replacing/editing a DDS or PNG in either:

    DPFixNG\textures\override\<hash>.dds / .png
    dpfix\tex_override\<hash>.dds / .png

Every currently loaded texture that originally came from the override path is tagged with D3D9
private data. A reload request increments a generation number. Each tagged texture refreshes lazily
the next time the game submits it through SetTexture, so no scene transition is required.

The reload uses the currently APPLIED texture settings. If EnableOverride or DimensionMode has an
unapplied F10 edit, the UI warns you to press Apply first.

## Lifetime model

No Release hook and no global COM-pointer registry are added.

- DPFix-NG stores a small metadata blob on the logical IDirect3DTexture9 with SetPrivateData.
- A hot-reload replacement is stored in a second private-data slot with D3DSPD_IUNKNOWN.
- D3D9 owns that COM reference and releases it automatically when the logical texture dies or a
  newer replacement takes its place.
- GetPrivateData AddRefs the current replacement for SetTexture; DPFix-NG releases that temporary
  reference immediately after the bind.

This keeps texture-override lifetime independent from the render-target replacement manager and
its Runtime Resource Audit.

## Transactional refresh

Reload is per-texture transactional:

- A new DDS/PNG is created first.
- Only a successfully created texture replaces the previous hot-reload generation.
- If the file is temporarily invalid, missing, or D3DX rejects it, the previous working texture
  remains bound.
- Press Reload Overrides again after fixing/saving the file to retry.

The Texture Inspector updates to the newest successful hot-reload resource, including its file and
actual GPU dimensions. This makes DimensionMode=Preserve especially convenient for iterative AI
upscale work.

## Scope of v0.0.42

Hot reload intentionally targets textures that were already loaded through the override path.
Adding an entirely new override for a texture that was originally loaded without one still requires
that source texture to be loaded again. This keeps the production path small and avoids scanning all
game textures after every reload request.

## Compatibility retained

- Original DPFix SuperFastHash behavior and 8-digit names.
- DPFix 0x7FFFFFFF source-size handling.
- DPFixNG override directory plus legacy dpfix\tex_override fallback.
- DimensionMode=DPFix and DimensionMode=Preserve.
- Texture dump and Texture Inspector.
- No new D3D9 device hooks.
- No participation in the render-target replacement registry.

All other production features remain unchanged: internal-resolution scaling, shadows, reflections,
DoF, pixel-offset fixes, world-detail extension, render-resource Hot Apply, input isolation and
Runtime Resource Audit.
