# Resource loading worker architecture

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Readable research synthesis; follow the cited evidence for build-specific claims.

**Jump to:** [Typed worker root and activation](#typed-worker-root-and-activation) · [Two request mechanisms](#two-request-mechanisms) · [CRdData descriptor handoff](#crddata-descriptor-handoff) · [Open boundaries](#open-boundaries) · [MegaRE final resource-system addendum (2026-10-08)](#megare-final-resource-system-addendum-2026-10-08)
<!-- END AUTO RESEARCH NAV -->

**Research snapshot:** 2026-10-04.
**Build scope:** Steam PC Phase 4 mechanism; GOG homology not yet established.

The latest Mega RE Census identifies the worker that bridges numeric resource requests to
the already known CRdData descriptor/callback machinery. This page records only the
selected mechanism, not a complete resource-format or lifetime model.

## Typed worker root and activation

Steam `01481130` is an in-place static `CLoadThread`, with `CThfunc` at offset zero. The
CRdData initializer `006B2570` passes that exact object to `006B3BA0`. When not already
started, the start route marks the worker state, clears the direct pending slot, and calls
the generic thread helper with stack size `0x40000` and name `LoadThread`.

The OS entry receives the same CLoadThread pointer and dispatches its virtual slot 4,
`006B4DE0`. This is an actually invoked start route at the static call-graph level. It is
not proof that `CreateThread` succeeds in every run, that startup ordering is race-free,
or that shutdown is quiescent.

## Two request mechanisms

Do not collapse queued and direct requests.

### Nonzero-mode queue

`006B3B40` packs a four-byte request value:

```text
bits  0..15  low16(identifier)
bits 16..23  low8(control)
bits 24..31  zero in the selected producer
```

The worker's embedded collection begins at `+0x24`; its selected begin/end/capacity-end
pointers are worker `+0x30/+0x34/+0x38`. The consumer removes the front four-byte value,
so the selected storage is value storage, not a vector of request pointers.

Only low 16 bits of the identifier cross this queue boundary, and the worker later
sign-extends that 16-bit value. Full-width identifier preservation is therefore not
established.

### Mode-0 direct pending slot

`006B3C00` writes a separate direct command to worker fields:

```text
+0x3C  pending byte
+0x3E  low16(identifier)
+0x40  control byte
```

The submitting caller waits for pending to clear, but execution still happens in the
worker. Clearing pending after a normal consumer return is protocol completion, not proof
that extraction, callback construction, or resource readiness succeeded.

## CRdData descriptor handoff

The worker eventually reaches `006B4F20`, which reacquires CRdData rather than treating
the worker as a CRdData receiver. A selected successful registration path through
`006B2780 -> 006B33F0` commits:

```text
descriptor +0x2E  exact control8
descriptor +0x2F  status 2
descriptor +0x2C  incremented word reference count
```

The registration helper has multiple nonuniform outcomes, including already-present and
failure-like cases. The surrounding worker path does not turn normal return into one
uniform "resource loaded" status.

## Open boundaries

Still unresolved:

- concrete runtime nonzero-mode producers and request cadence;
- direct-request serialization under competing producers;
- thread-start success and startup race behavior;
- exact identifier/control semantics and safe input domain;
- resource success/result propagation;
- worker shutdown/quiescence and resource/node ownership;
- GOG homology and runtime validation.

Exact source report:
`../evidence/mega_re_census_2026-10-04/boundaries/resource_worker_typed_handoff.md`.

## MegaRE final resource-system addendum (2026-10-08)

The final static map broadens this worker-focused note into a whole resource-system spine: named/archive identity -> raw or XZP1/zlib-like path -> CRdData descriptor state -> queue/direct request -> installed typed callback/parser -> family state -> consumer. Publication, successful decode, ownership and last-use remain separate facts.

New producer/binder joins include:

- `MES_ALL.MES`, `GLOBAL.FLG` and `GLOBAL.IDX` -> CMessage installed state used by NativeUI/message consumers;
- `EFF_LIST.PRM` -> EffectAdmin table with 195 x 20-byte records;
- selected SRL normalization/admission -> ItemManager-facing tuples;
- CPut resource payload -> large placement cohorts -> known world/car/model consumers;
- typed audio `CAudio_Data::FILEITEM` production;
- XCA-compatible parsed arrays -> same-model evaluator/blend;
- XAM-compatible binding -> retained allocation/transform/conditional cleanup;
- XPM/model-key paths -> selected triangle-mesh shape preparation.

The direct pending-slot path still does not prove global request serialization or request/completion identity. Stop/join/current-worker ownership also remain unresolved.

See [`FLOW_RESOURCE_SYSTEM.md`](../evidence/mega_re_final_2026-10-08/maps/FLOW_RESOURCE_SYSTEM.md) and the imported resource findings.
