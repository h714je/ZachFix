# Renderer and restoration research

**Research snapshot:** 2026-10-06.

This document records the architecture findings that sit behind current rendering
features and the branches that remain research-only.

## Production-closed or sufficiently grounded

### HOUSE_LIST day/night

The Director's Cut day/night regression is a structured-resource preprocessing bug,
not a shader-time workaround problem. ZachFix restores the table to the native
semantics before the original level consumer uses it.

### Interior visibility volume

The disappearing-prop regression near interior walls/mirrors is isolated to a narrow
visibility-volume callsite. The production fix bypasses only that confirmed path;
normal main-frustum culling remains native.

### Xbox ENV tone/color path

The Xbox-oriented color work is separated into scene grading and the optional HDTV
output transfer. It does not redefine the underlying world/visibility architecture.

### World-distance controls

Streaming detail, main-frustum class, active-list distance, mesh LOD, and alternate
low-detail 3D residency are separate controls. See `../world/README.md`.

## Native final presentation

The dormant PC 4:3 path is now mapped end to end: DP can render the complete logical
`1280x720` frame into a native `CRdTexture`, restore the physical backbuffer, and draw the
finished frame through one native `CRdPrim` textured quad immediately before `EndScene` /
`Present`. A separate 0.3.x PoC generalizes only the exact-16:9 destination rectangle while
keeping DP ownership of the `CRdTexture` and lost-device lifecycle. See
[native-final-presentation.md](native-final-presentation.md).

## Long-session performance degradation

The 0.3.x research branch has a session-only A/B recorder for the historical long-session
framerate-degradation reports. It correlates frame cadence, real `Present` time, ZachFix limiter
waits, DP native frame delta, process/system pressure and D3D resource lifetime/churn. See
[long-session-performance-audit.md](long-session-performance-audit.md).

## Depth

The PC renderer has both packed-depth behavior and a high-precision sampleable INTZ
path available through ZachFix. The remaining RE question is the native packed-depth
producer and exact packing formula, not merely the decoder.

The fixed AO band observed in prior experiments should not be attributed to a generic
"low depth resolution" without closing that producer path.

## Water

One Xbox/PC constant-ordering problem around the water path is known, but the richer
Xbox river shading regression is not reduced to a single confirmed patch. Remaining
work includes direct shader/interpolator comparison, coordinate-space checks, and UV
frequency.

## Trees / foliage

Tree work remains asset- and material-specific. Open tasks include:

- compare Xbox/PC `T18_00_00.XMD` submeshes and flags;
- resolve `MAGE_B_NSR_3` material 6 and texture bindings;
- compare foliage alpha/mips and Xbox `RE_LEAVES` against the PC counterpart.

Do not label the observed behavior as alpha threshold or billboard LOD until those
assets are closed.

## Terrain

The terrain branch is considered closed for now. Reopen it only with exact same-place
Xbox hardware evidence or a specific geometry/state mismatch.

## Shadows

Directional shadow relevance uses a separate three-frustum path with 1000/3000-unit
far construction and should not be conflated with the six main-camera frustum classes.

## Research policy

Renderer regressions should be classified at the lowest proven layer:

```text
resource preprocessing
object classification / culling
resource residency / LOD
shader constants
shader program/interpolators
D3D9 resource format/precision
post-process/output transfer
```

A visual symptom alone is not enough to choose the layer.
