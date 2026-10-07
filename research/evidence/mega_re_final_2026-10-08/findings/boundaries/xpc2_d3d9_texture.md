# XPC2 to D3D9 Texture Boundary

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for the helper's D3D9 API boundary; child-field semantics remain unresolved.

## Boundary

Steam XPC2 parser `006B5190` reaches helper `006B58D0` while processing child payloads. `006B58D0` first resets/releases prior texture state through `006B59A0`, inspects the in-memory payload through `00732C20`, and then calls one of:

- `D3DXCreateTextureFromFileInMemoryEx`, storing the resulting texture at helper/object `+0x14`;
- `D3DXCreateCubeTextureFromFileInMemoryEx`, storing the resulting cube texture at helper/object `+0x18`.

It writes decoded width/height-like values to `+0x0C/+0x0E` and clears a state field at `+0x08`. Cleanup releases texture slots `+0x10/+0x14/+0x18/+0x1C` through their vtable release methods. The helper therefore owns concrete D3D9 texture resources after XPC2 payload normalization.

## Interpretation limits

This verifies an XPC2 child-payload -> D3D9 texture boundary, but does not prove which XPC2 child-record fields select 2D versus cube texture, nor whether `006B58D0`'s first argument is the child record itself or a texture-holder object reached during parsing. The `0x20` child stride and row/column accessor semantics remain separate from the texture-holder layout.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:431227-431327` (`006B5190` parser call path)
- `inputs/decompiler/steam/DP_decompiled.c:431614-431685` (`006B58D0/006B59A0` D3DX texture creation and cleanup)
- `inputs/decompiler/steam/calls.csv:73760-73775`
- `findings/formats/xpc2_child_accessors.md`
- `findings/formats/typed_resource_dispatch.md`
