# XMD Animation to Actor Update

**Date:** 2026-10-01
**Evidence state:** `STRONG_INFERENCE` for animation-to-actor ownership; control/data flow is primary-backed.

## Boundary

Steam `006C1430` is a higher-level consumer of the animation transform representation produced by the XMD helper chain:

- Requires an active owner state at `this+0x160` (accessed through `param_1[0x58]`).
- Uses owner timing/state fields around `+0x1E4` and `+0x1E8` (`param_1[0x79]/[0x7A]`) and invokes an owner virtual slot `+0x6C` when a mode byte from `00402D70` equals `3`.
- Checks owner feature flags and selects optional state blocks around `+0x350`/`+0x358` (`param_1[0xD4]`, `param_1+0xCE`).
- Calls `0070C130` with transform context at `param_1+0x80`, timing/animation state, optional blocks, and feature booleans. The call occurs in both normal and scaled branches; the scaled branch temporarily changes transform scale and restores it after the update.

`0070C130` performs matrix/quaternion composition, interpolation, inverse/multiply operations, and animation-state writes. This places it downstream of the XMD matrix-to-quaternion representation and upstream of owner/actor state updates.

## Limits

The owner class of `006C1430`, the meaning of each feature bit, and the final renderer/actor call after `0070C130` remain unresolved. The boundary is therefore `STRONG_INFERENCE` for subsystem ownership despite verified control flow and math operations.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:438302-438409`
- `inputs/decompiler/steam/calls.csv:74670-74676`
- `inputs/decompiler/steam/DP_decompiled.c:489094-490010` (`0070C130`)
- `findings/boundaries/xmd_animation.md`
- `findings/boundaries/xmd_crdmesh_consumer.md`

## Sequence26 required primary supersession — K0021, 2026-10-03

**VERIFIED selectedSteam correction:** findings/boundaries/animation_model_state_submission.md proves class-anchored006C1430 update onCRdObjectModel/CPlayer,1E4 statebuffer/1E8 matrix-output ratherthan demonstratedtimerfields,base98/additional200/optional350-338-35C andconditionalsame-model1E8-to-packet consume. In006BE6E0, olddecompilerindices58/59/79/7A normalize tobyte160/164/1E4/1E8. CRdObjectModel own4C=00401D10 RET isnotdraw; otherclasses/acceptedCLevel4C stayseparate. Originalseedtext remainshistory; fullalgorithms/latestpose-perframe/free/GOG unknown. No acceptedPhase2 milestone regrade.
