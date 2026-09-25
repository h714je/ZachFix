# World residency and alternate low-detail representation

**Status:** CONFIRMED mechanics; exact semantic inventory of all 75 asset pairs remains PARTIAL
**Primary build:** GOG 1.01b

## 1. This is not a 2D billboard impostor system

Older research used the shorthand “far impostor” for the representation selected by:

```text
object+0x444  target representation/residency mode
object+0x12   currently active representation
object+0x416  alternate-resource table index
```

That terminology is corrected here.

`FUN_005C87F0` switches between two complete resource pairs and feeds both through the same model/resource binding routine `FUN_006BE1F0`. The alternate resources come from paired tables around:

```text
DAT_008AA6B0
DAT_008AA6B4
```

No billboard-specific render path is involved in the swap.

Use:

> **alternate low-detail 3D representation / far residency package**

not “2D impostor”.

## 2. Preloaded table size

Stage-load function `FUN_005D0C40` walks the paired table as:

```text
ptr = DAT_008AA6B4
repeat:
    preload ptr[-1] with flags 0x11
    preload ptr[0]  with flags 0x11
    ptr += 2 dwords
until ptr == 0x008AA90C
```

The range size is:

```text
(0x008AA90C - 0x008AA6B4) / 8 = 75 pairs
```

Both resources of every pair are therefore requested at stage initialization with the same `0x11` residency/load mode.

## 3. Runtime switching

`FUN_005C87F0` compares:

```text
object+0x444  desired mode
object+0x12   current mode
```

When switching back to the normal/near representation (`+0x444 == 0`), it first calls `FUN_005F08A0` and refuses the transition if the normal resources are not ready. This lets the current alternate representation remain active while the normal resource path catches up.

When switching to the alternate representation, it obtains the paired resources using `object+0x416` and immediately binds them through `FUN_006BE1F0`.

This architecture explains why the far representation can be selected without a fresh disk I/O boundary at the instant of swapping.

## 4. Relationship to other distance systems

This representation swap is independent of:

- the six main-frustum classes;
- the approximately 1000-unit active-list threshold;
- native per-resource mesh LOD (`0x0000/0x1000/0x2000/0x3000` submesh groups);
- high-detail streaming-cell selection.

An object may therefore be resident, active and inside its main frustum while still using an alternate low-detail 3D package.

## 5. Remaining work

- map all 75 pairs to asset names/types from the resource catalog;
- identify the exact writer(s) of `object+0x444` and their distance/cell policy;
- classify which object families use this mechanism;
- cross-check any bridge-specific near/mid/far visual transitions against actual model resource groups before assigning a mechanism.
