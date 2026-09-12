# DPFix-NG v0.0.38 Production Prune

This checkpoint is the production-only baseline before Texture Override work.
Research-only SSAO, shader/pass tracing, streaming lifecycle probes and their D3D9 hooks
are intentionally absent from this production tree.

ReShade DisplayDepth has been verified to receive a normal geometric depth buffer through
DXVK, so ambient occlusion and other generic depth post-processing belong in ReShade rather
than DPFix-NG.

## Production features

- Display Width/Height = 0 uses the native monitor resolution.
- Borderless mode.
- InternalWidth/InternalHeight or InternalScale supersampling.
- Shadow scaling with viewport and pixel-shader constant corrections.
- Reflection scaling.
- Improved DoF resolution.
- Pixel-offset corrections.
- HighDetailDistanceScale 1/2 for the original 2x2 or extended 4x4 streaming grid.
- F10 Dear ImGui settings UI with game input isolation.
- Hot Apply without IDirect3DDevice9::Reset.
- Transactional replacement backing resources for scalable render resources.
- Runtime Resource Audit: created/released/outstanding/active replacements, estimated memory and generation.

## Production D3D9 hook surface

Only hooks required by shipped functionality remain:

- Present
- CreateTexture
- CreateRenderTarget
- CreateDepthStencilSurface
- StretchRect
- SetRenderTarget
- SetDepthStencilSurface
- SetViewport
- SetTexture
- SetVertexShaderConstantF
- SetPixelShaderConstantF

Research-only hooks such as Draw*, BeginScene/EndScene, Clear, SetRenderState,
SetVertexShader, SetPixelShader and CreateCubeTexture are not installed.

## Research preservation

Keep the v0.0.36 research tree in a dedicated Git branch/tag. It contains useful
reverse-engineering results, but none of that machinery is required at runtime by this
production baseline.

## Runtime audit invariant

After a successful Apply in a steady state:

    Created - Released == Active replacements

Returning settings to the original backing dimensions should allow outstanding replacement
resources to return to zero when no replacements are needed.
