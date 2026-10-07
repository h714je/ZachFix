# D3D9 Device Resource Reset Lifecycle

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired managed-resource loss/recreation lifecycle.

Paired pre-reset helpers Steam `006CC030` / GOG `006CBAD0` emit `On Render Device Lost`, then enumerate four registry families. They release non-null COM resource pointers and clear their slots:

- 2D textures;
- cube textures;
- volume textures;
- vertex buffers.

Paired post-reset helpers Steam `006CBD00` / GOG `006CB7A0` emit `On Render Device Reset`, enumerate the same descriptor registries, and recreate the resource slots through `IDirect3DDevice9::CreateTexture`, `CreateCubeTexture`, `CreateVolumeTexture`, and `CreateVertexBuffer` using stored descriptor fields. Errors are logged but iteration continues.

This establishes renderer device-loss -> release cached resources -> device reset -> recreate cached resources before the outer loop permits frames again. Registry descriptor field semantics and ownership outside the reset lifecycle remain `UNKNOWN`.
