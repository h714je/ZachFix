# XMD Transform Blending Consumer

**Date:** 2026-10-01
**Evidence state:** `STRONG_INFERENCE` for animation/object ownership; math and data flow are primary-backed.

Steam `006C1850` consumes the animation state after the XMD/actor update path:

- Reads owner animation/context fields and invokes owner virtual slot `+0x68` to obtain/update a 64-element transform/control array.
- Resolves node records from `this+0x7C` into the `0xA0`-stride animation array rooted at `this+0x79`.
- Copies base transform records, then blends subsequent transforms with `D3DXQuaternionSlerp` and `D3DXQuaternionMultiply`, interpolating translation fields at record offsets `+0x20/+0x24/+0x28`.
- Writes blended transforms back into the animation/object state and processes attachment/root records.

This is the next consumer beyond `006BE460`, `006C1430`, and `0070C130`, but it still does not expose the final D3D9 draw call. The owner class, slot `+0x68` semantics, and final render handoff remain open.
