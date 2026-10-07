# World residency and alternate low-detail representation

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Readable research synthesis; follow the cited evidence for build-specific claims.

**Jump to:** [1. This is not a 2D billboard impostor system](#1-this-is-not-a-2d-billboard-impostor-system) · [2. Preloaded table size](#2-preloaded-table-size) · [3. Runtime switching](#3-runtime-switching) · [4. Relationship to other distance systems](#4-relationship-to-other-distance-systems) · [5. 2026-10-04 selected writer/consumer chain](#5-2026-10-04-selected-writerconsumer-chain) · [6. Remaining work](#6-remaining-work)
<!-- END AUTO RESEARCH NAV -->

**Status:** CONFIRMED mechanics; exact semantic inventory of all 75 asset pairs remains PARTIAL
**Primary paired map:** GOG 1.01b. **2026-10-04 writer/consumer addendum:** Steam-only until homology is established.

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

## 5. 2026-10-04 selected writer/consumer chain

The Mega RE Census closes one previously missing Steam writer path for the same CObject.
A selector-`0x46` fallback constructs CObject, writes dword `+0x444 = 1`, and immediately
reaches the representation consumer at Steam `005C8720`. A local descriptor can also copy
its `+0x50` value into object byte `+0x416` when descriptor `+0x58 & 0x100` is set.

The consumer gates desired `+0x444`, current `+0x12`, and descriptor/index state, performs
paired resource access through `006B2C70`, calls same-object `006BE6E0`, then commits the
low byte of desired `+0x444` to current `+0x12`.

This is a verified conditional same-object writer -> consumer -> current-state commit. It
is not yet a general residency policy: upstream context and value semantics, other
writers, all 75 resource pairs, pair identity, ownership, final draw, runtime cadence,
and GOG homology remain open. Do not confuse the Steam interior/function label with the
older GOG `FUN_005C87F0` mapping.

## 6. Remaining work

- identify the upstream policy and full writer set for desired `CObject+0x444` values;
- determine the semantic meaning/range of desired/current representation values;
- type the paired resources and validate the 75-entry table rather than extrapolating from
  one selected pair;
- close ownership/last-use/unload and final rendering behavior;
- establish Steam/GOG homology for the new Phase 4 chain and runtime cadence.
