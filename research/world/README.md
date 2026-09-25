# World distance, LOD, residency, and shadow architecture

**Source:** engine map v10 world reconciliation, 2026-09-26.

"Draw distance" is not one variable in Deadly Premonition PC.

## Canonical distance stack

| Mechanism | Native scale / threshold | Role |
| --- | ---: | --- |
| High-detail streaming cells | topology/cell based | selects high-detail streamed cell content |
| Main frustum classes | `200000 / 80000 / 20000 / 5000 / 1000 / 500` | object main-visibility class |
| Object active-list gate | radius ~`1000` | object activation in native active-list path |
| Native mesh LOD | `distance / (resourceScale * 25)` | selects native submesh LOD group |
| Alternate low-detail 3D package | policy driven | swaps to one of 75 preloaded resource pairs |
| Directional shadow frusta | far `1000` or `3000` | three-cascade directional-shadow relevance |
| Character secondary-update gate | `500` | bounds a separate NPC/character work path |

## Six native main-frustum classes

Native far planes:

```text
class 0 = 200000
class 1 =  80000
class 2 =  20000
class 3 =   5000
class 4 =   1000
class 5 =    500
```

Classification flows through object flags at `+0x138`, `FUN_006BB440`, and the common
AABB consumer `FUN_006BCE30` for most named gameplay vtables.

A concrete result is closed:

```text
factory 0x9A = CObjectHut
factory 0x9C = CObjectVendor
```

Both retain default class 5 = 500 units. Their observed pop-in is therefore separate
from the ~1000-unit active-list gate.

## Object active-list distance

Native baseline:

```text
radius      ~1000
thresholdSq 1,000,000
```

The production `ObjectActivationDistanceScale` patch changes this comparison only. It
does not change frustum class, streaming topology, mesh LOD, or shadow distance.

## Native mesh LOD

The native selector uses approximately:

```text
metric = cameraDistance / (resourceScale * 25)
```

Submesh LOD flags are `0x0000 / 0x1000 / 0x2000 / 0x3000`.

Production `ObjectLODDistanceScale` scales this existing metric rather than replacing
the resource groups or selector.

## Alternate low-detail 3D residency

The old "2D impostor" description is disproven.

Runtime switching uses:

```text
object+0x444 desired representation/residency mode
object+0x12  current representation
object+0x416 alternate-resource table index
```

Stage setup pre-requests exactly 75 paired resources from tables around
`DAT_008AA6B0/DAT_008AA6B4`, both members of each pair using load/residency mode
`0x11`.

`FUN_005C87F0` swaps between complete resource pairs through the normal model/resource
binding path. The correct term is:

> alternate low-detail 3D representation / far residency package

Production `Alternate3DDistanceScale` extends the near/full range feeding this native
residency path; it is not a main-frustum, active-list, or mesh-LOD control.

## Directional three-cascade shadow visibility

A separate renderer mode chooses far construction at either 1000 or 3000 units and,
in the long-distance mode, links `camera+0x67C` to a three-frustum set at
`renderer+0x5F88`.

The common object-culling path can rescue an object for directional-shadow relevance
even after it fails its selected main camera frustum.

Therefore `MainFrustumDistanceMode` does not directly extend shadow-caster distance.

## Corrected character distance claim

The old 1995-unit skeletal cutoff is disproven. The relevant caller gate is 500 units.
The adjacent `1995.0` constant belongs to unrelated environmental-range logic.

## Production controls and their exact domains

```text
HighDetailDistanceScale       -> streaming-cell detail selection
MainFrustumDistanceMode       -> selected main-frustum far planes
ObjectActivationDistanceScale -> native active-list threshold
ObjectLODDistanceScale        -> native mesh-LOD metric
Alternate3DDistanceScale      -> alternate low-detail 3D residency range
```

No current production control rewrites the separate 1000/3000 directional-shadow
frusta.

## Remaining work

- identify explicit frustum-classifier types `0x2B` and `0x4F`;
- classify specialized `vtable+0x24` consumers that do not use `FUN_006BCE30`;
- map all 75 alternate resource pairs to friendly asset names;
- identify the exact writers/policy for `object+0x444`;
- decide whether a bounded production shadow-distance control is desirable;
- resolve bridge-specific near/mid/far behavior only after its own resource package is
  identified.
