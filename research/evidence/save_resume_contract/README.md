# Native save / resume contract evidence

**Research snapshot:** 2026-09-30
**Scope:** Deadly Premonition: The Director's Cut PC, Steam-target control-flow pass using the established GameRecord byte layout.

This page records the focused RE performed to answer one question: whether native
save/load is fundamentally tied to phone/save-point coordinates, or whether arbitrary
position saving is architecturally possible when runtime state is resumable.

## Bottom line

The save format is **not** save-point-coordinate-bound. Native persistence consists of a
large persistent GameRecord, a resume anchor, native runtime-to-persistent synchronization,
and a small set of explicit special-resume adapters.

The unresolved problem for a universal quicksave is therefore semantic safety of
arbitrary in-flight runtime protocols, not the ability to serialize York's current
position.

## Confirmed control-flow anchors

```text
Steam FUN_006AE0C0  manual/phone save builder
Steam FUN_006ACAB0  generic/full save builder
Steam FUN_004524C0  common pre-save synchronization dispatcher
Steam FUN_00451450  resume/location/York transform snapshot
Steam FUN_00430A10  CEvent/CEvCore snapshot policy
Steam FUN_00506F70  load/reconstruction branch
Steam FUN_00509080  normal York transform restore
```

## Save modes

```text
0  generic normal
1  manual/phone
2  checkpoint
3  special top-level state-0x46 context, normal refresh skipped
4  load/restore transition, normal refresh skipped
5  historical record capture
```

Modes 0/1/2/5 share the full persistent synchronization cluster. Mode 1 has one explicit
extra CEvent resume adapter for runtime mode `0x1F5`. Modes 3/4 bypass the normal refresh
cluster.

## Phone save

`CObjectPhone -> Event 0x2F -> CPlayer state 0x37` is confirmed. The save transition is
started from inside state `0x37`; York is returned to state `0` only later. Therefore
"Player must be idle/state 0" is not a native save invariant.

## Autosave/checkpoint gates

CEvent `0xC2` and `0xC6` do not snapshot blindly. They have separate caller-side guards
and prepare different transition/resume flags before converging on native persistence.
This is the strongest direct evidence for a caller-specific safe-save contract.

## Special resume token A: high Player-state mask bit 4

World-resource subtype `0x75C` stores a transform in the early action-packet overlap and
sets `record+0x64 bit 0x4`. On load this marker selects CPlayer state `0x40`, which
reconstructs the relevant world/location context and later clears the bit. The marker is
therefore one-shot persistent resume state, not a generic transient capability bit.

## Special resume token B: manual CEvent 0x1F5

Manual save mode 1 detects `CEvent+0x3012 == 0x1F5`, sets `record+0x84 bit 0x1000`, and
stores two DWORDs at `record+0x43818/+0x4381C`. Load code recreates the `0x1F5`
controller, restores the saved value and clears the marker.

## Action packet interpretation update

`record+0x14..+0x5B` is still the live generic object-action packet, including a runtime
object field, but selected transform/heading fields are intentionally reused by at least
one native resume adapter. Treat this region as mixed live/persistent protocol storage,
not as wholly disposable padding and not as a self-contained serializable object graph.

## Post-load reconstruction

The loader rebuilds transient CPlayer/runtime state and then overlays persistent resume
state. Normal positional resume uses `record+0xCAE0/+0xCAF0`; scripted resume can force
`record+0xCADA = 0xFF`; explicit resume tokens can select alternate Player reconstruction
such as state `0x40`.

The 2026-10-01 corrective pass closes an important default/special branch in the reviewed
Steam path. `FUN_00506F70` selects state `0x40` when `playerStateMaskHi bit 0x4` is
present, while the later Player initialization stage `FUN_00507BA0` selects state `0x00`
when that token is absent. The pre-save transient value of `CPlayer+0x654` is not itself
serialized in GameRecord.

Consequently the earlier hypothesis:

```text
save while state 0x38
    -> load restores state 0x38
    -> stale record+0x14 pointer reaches FUN_004DDDB0
```

is **DISPROVEN**. The pointer-shaped value at `record+0x14` can still be physically
captured, but the claimed state-`0x38` post-load reachability edge does not exist in the
normal/default path. A complete census of every other possible post-load consumer of
`record+0x14+0x00` remains OPEN.

See [SaveAnywhere_Corrective_Verification_RE.md](SaveAnywhere_Corrective_Verification_RE.md)
for the disputed-claim audit and raw control-flow anchors.

## Safe future quicksave envelope

Research-only conservative boundary:

```text
reject mode-3 / mode-4 contexts
reject a pending 0x04000000 scripted-resume command rather than consuming it accidentally
reject active fade/world transition (FUN_004492C0 != 0)
require a valid York object
retain native special-resume tokens
invoke native persistent synchronization rather than hand-serializing fields
```

For a first general-purpose ZachFix experiment, state `0x00` is a conservative whitelist,
not a native invariant. Vanilla phone save still proves that controlled non-zero states
can be safe when the caller establishes the matching resume contract.

Vehicle (`0x38/0x87/0x88`) and other multi-phase object-action cases remain outside that
conservative whitelist until their caller-specific resume semantics are proven. This is
no longer justified by the disproven universal stale-pointer-crash hypothesis.
