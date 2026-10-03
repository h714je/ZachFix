# Animation/model-state submission architecture

**Research snapshot:** 2026-10-04.
**Build scope:** Steam PC selected static mechanism; no new GOG mapping claimed.

The Mega RE Census connects a typed CRdObjectModel update to matrix-state production and
a conditional same-instance submission packet. This closes a useful producer/consumer
join without claiming that the newest pose is rebuilt every frame.

## Typed CRdObjectModel interfaces

For the selected Steam CRdObjectModel table:

```text
virtual +0x10 -> 006BCBE0  event/preparation/evaluation path
virtual +0x28 -> 006BD4C0  packet population
virtual +0x44 -> 00405E40  packet capacity/allocation then +0x28
virtual +0x48 -> 004029B0  object membership protocol
```

A selected scene object phase invokes the original object's `+0x10` path conditionally.
Within the model, `006BCBE0` can reach `006C1430` on the same receiver after its native
event and resource/control gates.

## Matrix/state output

Selected setup `006BE6E0` stores resource inputs at model `+0x160/+0x164`, allocates
state records at `+0x1E4` and matrix output storage at `+0x1E8`, clears them, then invokes
`006C1430(0)`.

The evaluated matrix destination is concretely `model+0x1E8 + 0x40*i`. The paired state
records advance by observed `0xA0` stride. This corrects older shorthand that treated
`+0x1E4/+0x1E8` as generic timing fields. `006C1430` is an update/evaluation consumer,
not a shutdown routine.

## Same-instance packet population

`006BD4C0` retains the same model and a packet pointer. Selected stores connect the packet
back to that model/resource state, and `0070CF90` transforms/copies source matrices from
`model+0x1E8` into packet-owned aligned storage. The selected path includes named
D3DX matrix multiply/transpose calls and 48-byte matrix-derived copies.

Later pre-render work can submit the same model's retained packet pointer into the scene
query/submission structure.

The essential qualification is cache behavior: an already-valid packet can bypass
repopulation unless a force/invalidation mask requires the model's packet-population
virtual. Therefore the supported chain is:

```text
typed model update
    -> selected +0x1E8 matrix writes
    -> conditional same-model packet population
    -> retained packet submission
```

not:

```text
every animation update -> guaranteed newest pose packet in the same frame
```

## Open boundaries

- actual live instance/query selection and cadence;
- packet invalidation and freshness policy;
- state/resource/packet lifetime and teardown coordination;
- complete interpolation, packet grammar, skinning/render semantics, and GPU endpoint;
- GOG homology.

Exact source report:
`../evidence/mega_re_census_2026-10-04/boundaries/animation_model_state_submission.md`.
