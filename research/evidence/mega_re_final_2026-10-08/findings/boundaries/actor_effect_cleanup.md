# Actor / Effect State Cleanup Boundary

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for cleanup behavior; exact owner class remains open.

Steam `00718F00` is a high-fan-in lifecycle helper called from many actor/effect paths. It:

- Checks feature flags and object arrays at `this+0x264/+0x268/+0x26C/+0x270/+0x274/+0x278`.
- Releases array members through paired cleanup helpers (`00448670`, `0071BE10`, `00709C30`) and frees the arrays.
- Clears object flags and resets animation node flags at `this+0x1E4 + node*0xA0 + 0x70`.
- Releases auxiliary state at `this+0x350` and resets lifecycle state at `this+0x2FC`.

This is a concrete actor/effect cleanup boundary downstream of the animation/resource object model, but the owning class and relation to individual selector-created effects remain unresolved.
