# Directional three-cascade shadow visibility path

**Status:** CONFIRMED mechanics; “outdoor/sun mode” is STRONGLY_SUPPORTED semantic naming
**Primary build:** GOG 1.01b

This path is separate from the six object main-frustum classes.

## 1. Mode source

`FUN_00450920` returns exactly:

```text
(gameState+0x8C5EC >> 15) & 1
```

The frame/update path writes:

```text
renderer+0x6708 = (FUN_00450920() != 0)
```

There is also at least one stage-specific path that can force `renderer+0x6708 = 1`.

## 2. Camera linkage

Inside `FUN_006D2710`:

```text
renderer+0x6708 == 0
    -> camera+0x67C = NULL

renderer+0x6708 != 0
    -> camera+0x67C = renderer+0x5F88
```

`FUN_006C4140` reads `camera+0x67C` as three six-plane frusta.

## 3. 1000 vs 3000 construction

The renderer constructs another frustum set with:

```text
renderer+0x6708 == 0 -> far = 1000.0  (0x447A0000)
renderer+0x6708 != 0 -> far = 3000.0  (0x453B8000)
```

A directional-light path builds **three** view/projection matrix pairs and copies the resulting three frusta into `renderer+0x5F88`.

The combination of:

- directional vector fields around `renderer+0x6418`;
- exactly three generated frusta;
- shadow-caster rescue in `FUN_006BCE30`;
- shadow-oriented renderer submission lists

supports the semantic label **directional three-cascade shadow / CSM path**.

## 4. Relationship to common object culling

If an object fails its selected main frustum, `FUN_006BCE30` can still test its AABB against the three `camera+0x67C` frusta. A successful secondary test sets temporary object flag:

```text
object+0xD8 |= 0x10
```

If that also fails, a directional extrusion test (`FUN_006C3EA0`) can perform another rescue against the main frustum family.

Thus a main-camera distance class is not the same thing as directional-shadow relevance.

## 5. Renderer pass-list evidence

The model/submesh classifier writes a dedicated list at `renderer+0x63C4` under shadow-caster masks/fade conditions. Separate lists at `+0x63C8` and `+0x63CC` are used for other renderer roles and should not be conflated with this distance gate.

The exact GPU-pass labeling has been mechanically traced in prior renderer work; this document keeps the distance architecture boundary rather than duplicating the full render pipeline.

## 6. Production implication

`MainFrustumDistanceMode` does **not** extend this 1000/3000 directional-shadow far distance. A future shadow-distance option should therefore target this path separately rather than stretching object main-frustum classes and hoping shadows follow.
