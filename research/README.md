# ZachFix reverse-engineering research

This directory contains the reconciled reverse-engineering record behind ZachFix.
It is intentionally separate from `docs/`: product documentation describes what the
current production build does, while `research/` records engine architecture,
restoration candidates, disproven interpretations, and unresolved questions.

**Source map:** ZachFix engine map v10, reconciled 2026-09-26.

## Evidence policy

Research claims use the same conservative ordering as the engine map:

```text
raw PC machine code
    > PC decompile
    > Steam/GOG cross-build agreement
    > reconciled notes
    > agent reports
    > Xbox/Xenia clues
    > inference
```

Semantic names are used only when mechanics support them. Attractive old labels that
were later disproven are retained in [disproven.md](disproven.md) instead of being
silently erased.

## Current architecture index

- [engine/overview.md](engine/overview.md) - top-level scheduler, object dispatcher,
  Player spine, physics island, and major engine domains.
- [engine/timing.md](engine/timing.md) - main PC timing domains and known cadence
  boundaries.
- [input/README.md](input/README.md) - physical input -> CInput -> gameplay consumers,
  Native XInput bridge, latency, camera modes, and Xbox-only control findings. Detailed
  maps: [camera-modes.md](input/camera-modes.md) and [xbox-controls.md](input/xbox-controls.md).
- [player/README.md](player/README.md) - CPlayer state families, action packet,
  selector/state-domain separation, CCT bridge, and vehicle handoff. Detailed maps:
  [state-families.md](player/state-families.md), [action-protocol.md](player/action-protocol.md),
  [action_selector.md](player/action_selector.md), [object-action-taxonomy.md](player/object-action-taxonomy.md),
  [vehicle.md](player/vehicle.md), and [cct-bridge.md](player/cct-bridge.md).
- [world/README.md](world/README.md) - streaming, six main-frustum classes,
  activation, mesh LOD, alternate low-detail 3D residency, and shadow distance. Detailed
  maps: [frustum.md](world/frustum.md), [lod.md](world/lod.md), [residency.md](world/residency.md),
  and [shadows.md](world/shadows.md).
- [render/README.md](render/README.md) - renderer/restoration findings and the current
  status of depth, water, terrain, trees, day/night, and interior visibility work. Detailed
  retained branches include [depth.md](render/depth.md), [color.md](render/color.md),
  [water.md](render/water.md), [daynight.md](render/daynight.md), and
  [interior-visibility.md](render/interior-visibility.md).
- [physx/README.md](physx/README.md) - final PhysX timing investigation and why no
  production physics patch is shipped.
- [disproven.md](disproven.md) - corrected interpretations that should not be
  rediscovered.
- [unresolved.md](unresolved.md) - remaining research targets after v10 reconciliation.

## Production boundary

Research findings are not automatically production features. A candidate moves into
ZachFix only after its owning subsystem, call-site scope, failure modes, supported
builds, and runtime behavior are understood well enough to fail closed.

The clearest example is PhysX timing: several real defects were confirmed, but local
repairs repeatedly exposed incompatible assumptions in other timing domains. The
production tree therefore leaves native physics timing untouched.
