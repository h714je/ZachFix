# Native mesh LOD

**Status:** CONFIRMED core metric and submesh-group selector  
**Primary build:** GOG 1.01b

## 1. Metric producer

`FUN_006DCF90` computes a per-render-object metric at `object+0x20` for the normal perspective-camera path:

```text
cameraDistance = length(...)

if cameraDistance >= 100000000:
    object+0x20 = 0
else:
    object+0x20 = cameraDistance / (resourceScale * 25.0)
```

If the relevant camera/render mode is not active, the metric is also set to zero.

## 2. Native selector

The renderer derives a native LOD level and matches submeshes by:

```text
submesh.flags & 0x3000
```

against:

```text
level 0 -> 0x0000
level 1 -> 0x1000
level 2 -> 0x2000
level 3 -> 0x3000
```

The same selected submesh then enters the renderer's normal pass-list classification, including shadow/refraction/depth list decisions.

This is a **mesh/submesh LOD mechanism**, not streaming and not object activation.

## 3. ZachFix production scaling

`ObjectLODDistanceScale` modifies the native metric before the game's selector consumes it:

```text
scaledMetric = nativeMetric / configuredScale
```

It deliberately leaves:

- the native submesh flags;
- native resource groups;
- object activation;
- streaming cells;
- frustum classes

unchanged.

## 4. Bridge / multi-stage visual changes

The engine unquestionably contains both:

1. native mesh LOD described above; and
2. separate shadow-frustum/submission distance behavior.

However, the earlier statement that a particular bridge's visible near/mid/far transitions were **proven** to use both mechanisms was too strong. Without isolating that bridge's exact XMD/submesh flags and alternate-representation policy, the model-specific attribution remains **LIKELY / OPEN**.
