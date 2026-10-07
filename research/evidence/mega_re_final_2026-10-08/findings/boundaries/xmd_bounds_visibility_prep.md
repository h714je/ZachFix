# XMD Bounds / Visibility Preparation

**Date:** 2026-10-01
**Evidence state:** `STRONG_INFERENCE` for animation-to-render-preparation ownership; no final draw call is present.

Steam `006C2020` consumes the animation records after the XMD transform chain:

- Reads owner animation array `this+0x1E4` and node flags at record `+0x70` with `0xA0` stride.
- Iterates geometry/attachment records and derives transformed points from animation/context state.
- Uses `D3DXVec4Transform`, bounds accumulation, and visibility/feature checks.
- Writes calculated bounds/position values into owner fields around `+0xF0/+0xF4` and related state.
- It is called from many actor/object paths and from `006BE6E0`, but does not invoke a final mesh draw API in the inspected body.

This establishes an XMD animation -> bounds/visibility preparation boundary, not a final CRdMesh render invocation. Final renderer ownership remains open.
