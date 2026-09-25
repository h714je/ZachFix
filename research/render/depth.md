# Depth / PostFX depth sources

## Scope

This document separates three concepts that were repeatedly conflated during AO/depth work:

1. the game's native scene depth-stencil;
2. DP's packed depth-like render target used by legacy/post-processing paths;
3. ZachFix's PostFX choice between packed fallback and sampleable native D24/INTZ.

## Status

| Claim | Status |
|---|---|
| DP exposes a packed multi-channel depth representation consumed by ZachFix PostFX | CONFIRMED |
| ZachFix packed decoder uses `dot(rgb, {65535,255,1})` before any legacy clamp | CONFIRMED |
| Packed depth has materially lower/quantized precision than native D24/INTZ for AO | STRONGLY_SUPPORTED by runtime A/B |
| Native D24/INTZ is the preferred high-precision ZachFix PostFX depth source when available | CONFIRMED production architecture |
| A visible stationary AO/depth band means the game's geometry depth buffer itself is broken | DISPROVEN / too broad |

## Packed depth decode

The ZachFix research decoder for the game's packed source is:

```hlsl
float DecodePackedDepth(float3 packed)
{
    return dot(packed, float3(65535.0, 255.0, 1.0));
}
```

An older ZachFix implementation then clamped this through an effective `0..255` domain. That clamp was useful legacy behavior but must not be confused with the actual packed value range.

The important architectural fact is that this is a **packed fixed-point representation**, not a native hardware depth sample and not a simple single-channel linear view-Z texture.

## Native D24 / INTZ path

Research A/B added a sampleable INTZ replacement for the main D24S8 depth-stencil and reconstructed view depth with the current projection `q/qn` values.

Conceptually:

```text
scene geometry
    ↓
native D24/INTZ hardware depth
    ↓
projection q/qn reconstruction
    ↓
linear/view-depth for AO / DoF / composite
```

This is separate from:

```text
scene geometry
    ↓
DP packed RT
    ↓
packed fixed-point decode
    ↓
legacy PostFX depth domain
```

## Important INTZ binding bug found during audit

A ZachFix audit found that the native INTZ texture could be bound as a shader texture **before being detached as the active depth-stencil**. D3D9 does not allow the same resource to be simultaneously used for depth writing and texture sampling.

The hardened order is:

```text
if selected INTZ == active depth-stencil:
    detach depth-stencil

bind INTZ as sampler

run PostFX pass

unbind sampler
restore depth-stencil
```

This matters because a broken bind could make the code appear to have native depth available while the actual effect silently fell back or sampled stale/wrong state.

## Packed fallback rules

When INTZ is unavailable, ZachFix retains a packed fallback. The production-safe fallback keeps packed-only semantics separate from native depth:

- legacy packed range/clamp behavior where required;
- packed far/sky sentinel handling;
- packed textures treated as data (`sRGB sampling disabled`);
- native INTZ is not subjected to the packed far/sky `~255` sentinel.

## Stationary AO band

Runtime A/B isolated the visible stationary band to the packed-depth route: switching the same AO logic to native D24/INTZ removed the fixed depth-step signature.

Therefore the current conservative interpretation is:

```text
fixed AO band
    = packed depth quantization / representation artifact at the PostFX input boundary

not proven:
    = broken native scene Z-buffer
```

## Evidence

Primary retained artifacts:

- `ZachFix-depth-precision-AB-test.md`
- `ZachFix-depth-packed-decode-fix-A.patch`
- `ZachFix-depth-native-INTZ-B-on-top-of-A.patch`
- `ZachFix-high-precision-postfx-depth-G*.patch`
- `ZachFix-code-audit.md`

## Remaining questions

- Preserve a compact final runtime A/B log/screenshot pair in the engine-map evidence archive so the packed-vs-INTZ visual conclusion is not dependent on conversation history.
- Document exactly which native DP render target contains the packed depth producer and its write shader/packing formula from the game side, not only ZachFix's decode side.
- Keep packed fallback behavior as compatibility infrastructure, not as the preferred basis for new high-precision effects.
