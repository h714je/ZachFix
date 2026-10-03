# Mega RE Census Phase 4 mechanism integration

**Research snapshot:** 2026-10-04.
**Census state:** `PHASE_4_ACTIVE`, checkpoint seq55.
**Default build scope for new findings:** Steam PC only unless stated otherwise.

This page is the ZachFix-facing synthesis of the latest Mega RE Census. It records what
Phase 4 materially adds beyond the 2026-10-01 architecture snapshot without pretending
that Phase 4, runtime validation, or Steam/GOG homology is complete.

The preserved source slice is in
[`../evidence/mega_re_census_2026-10-04/`](../evidence/mega_re_census_2026-10-04/).

## Outer application frame root

The paired PC application loops now establish the static outer ordering:

```text
Win32 message handling
    -> eligible idle gate
    -> timing update / 60-relative scalar and clamp
    -> FUN_00401A70 once per eligible idle iteration
```

Steam root is `00700650`; GOG is `00700670`. This closes the static relationship between
the Windows idle path and the already known frame/gameplay root. It does not, by itself,
prove live cadence across loading, pause, menus, movies, or every exceptional mode.

## Resource worker and CRdData handoff

Steam now has a typed static `CLoadThread` at `01481130`. The CRdData initializer invokes
its start helper, and the OS thread entry receives the exact same object before dispatching
its worker virtual.

Two request mechanisms are distinct:

```text
nonzero mode -> compact 4-byte queued values
mode 0       -> direct pending slot; caller waits while worker services it
```

The queue does not preserve a full-width identifier at that boundary. The direct path is
not the queue path. A normal return or a pending-slot clear is not equivalent to a
successful resource load.

A selected successful registration path writes an 8-bit control value to descriptor
`+0x2E`, status `2` to `+0x2F`, and increments the word at `+0x2C`. Exact request
serialization, safe shutdown, full resource grammar, GOG homology, and runtime success
remain open.

## PhysX context and record container

Steam has four in-place typed `CPhysicsThread` contexts rooted at `00BDA010`, stride
`0x2C`. Each embeds `CNArray<CPhysicsThread::SCENE>` at `+0x1C`; storage/capacity/count
are the context fields `+0x20/+0x24/+0x28`. Construction allocates eight `0x18` records.

The selected producer performs raw six-dword deduplication and appends to the same
physical vector later consumed by the worker. No separate queued-to-active transfer is
proved. The selected record currently supports these concrete roles:

```text
+0x00  pointer consumed as virtual-call receiver when non-null
+0x04  float scalar used by worker numeric selection
+0x08  opaque forwarded producer value
+0x0C  pointer-slot ordinal from an upstream table
+0x10  context-specific value, not a universal timing field
+0x14  context-specific value, not a universal timing field
```

An available start helper `0040B630` can start all four contexts through the generic
thread machinery and reaches each context's worker virtual, but no actual incoming call
to that helper is currently proved. That is a bounded missing edge, not evidence that
the worker never runs.

Pointer invalidation nulls the first matching record pointer without compacting or
reducing count. Destruction clears the run-like byte, wakes the event, then frees the
vector; the selected body does not prove join/quiescence before the free. These lifecycle
facts are new architecture and do not replace the existing PhysX timing closure.

## Save read, staging, commit, and physical write

Steam now connects the high-level GameRecord model to a typed disk/staging mechanism:

```text
CPreserve / CSaveData / CSysutil
    -> request/read into 00BE5EF0, count 0x7A2620
    -> CPreserve header/state processing
    -> selected commit of record0 to CGame +0x8C568
       and the remaining 27 records to the native tail destination
```

The selected read protocol does not prove exact/full `ReadFile` completion. The raw
reader returns file-size information and does not turn its protocol result into a full
bytes-read validator. The CPreserve numeric-5 read context and numeric-3 commit context are separately
observed; their automatic chronology is not asserted.

The write side is also connected. CPreserve operation 4 calls the builder, which clears
the `00BE5EF0` staging image, copies live `CGame+0x8C568` into record0 at `00BE6010`,
then fills the remaining record images. A later same-context operation-6 path reaches the
physical writer and passes the staging buffer/count to `WriteFile` for `savedata/dp.sav`.

Admission, exact builder-to-write chronology, disk success/bytes-written checking, full
schema validation, ownership, runtime cadence, and GOG correspondence remain open.

## Retail CMenu, CFade, and camera policy

Phase 4 adds stock retail UI mechanisms without changing the preferred ZachFix custom-UI
architecture.

One selected CMenu numeric path produces literal `0x55` / decimal `85`, obtains a CGame
slot, subtracts `999` with 32-bit arithmetic, and clamps a signed-negative result. The
target lies inside the live GameRecord envelope, but containment alone does not supply a
friendly field name, list semantics, or persistence chronology.

A separate typed CMenu event-1 late-state path maps raw states 99/100/101 into a sequence
involving CFade request 3, an independently reacquired fade predicate/state 7, and a later
CCamera mask update that clears bit 1 and sets bit 4. Request-3-causes-7 is not proved,
nor are alias stability, policy meaning, lifetime, cadence, or GOG homology.

The practical ZachFix consequence remains the same: stock CMenu/COption logic owns dense
retail policy. The generic selector-0 CRdObject task remains the safer custom-page shell.

## World representation state

A selected Steam chain now proves one concrete writer and consumer for the same CObject's
alternate-representation state:

```text
selector 0x46 CObject creation
    -> same object +0x444 = 1
    -> 005C8720 representation consumer
    -> optional descriptor-driven object byte +0x416
    -> paired resource accessors
    -> same-object 006BE6E0 handoff
    -> low byte of desired +0x444 committed to current +0x12
```

This closes the previous question of whether any writer for `+0x444` could be found. It
does not close the upstream policy, value semantics, other writers, pair-resource identity,
ownership, final draw, all 75 table entries, or cross-build mapping.

## CObjectCar callback and model-state production

A typed Steam CObjectCar producer now supplies a stronger vehicle control chain. It
constructs a `0x2008` object, writes words `+0x2C=0x0E`, `+0x2E=7`, and
`+0x30=0x53` (decimal 83), then installs callback `005C92C0` at the actor callback slot.
This corrects the historical `0x35` / decimal-53 transcription.

On selected callback event 1, a post-helper selector reload accepts only `0x53` and enters
`00543160`. A selected activation arm requires byte `+0x1FD4 != 0` and state
`+0x1FD8 == 1`, then commits `+0x1FD8 = 2` and several numeric control fields including
`+0x11D8 = -1000.0f`. Actual activation, opaque helper effects, later event-1 mutations,
ownership, deletion, cadence, and GOG remain unknown.

A separate model/cache-production chain follows the original CObjectCar and selected
local blocks into same-actor copied state around `+0x98`, matrix/float preparation, then
an explicit current-state evaluation through `006C1430(0)` under resource guards. This
connects actor inputs to animation/model base state, but not to a guaranteed freshest pose
on every frame.

## Animation submission

The Phase 3 model-vtable map is now connected to a selected Phase 4 state/submission
chain: object phase work reaches the model update consumer, resource evaluation updates
model state, and the same model's transform/copy path can populate a packet that later
enters a selected scene query/eligibility path.

An existing packet can bypass population, so the evidence does not justify a blanket
"fresh pose is rebuilt every frame" rule. Buffer freshness, invalidation, ownership,
render/GPU completion, teardown, runtime cadence, and GOG remain open.

## Effects, presentation, and weather retainers

`CFadeManager` is now a typed lazy singleton root at `00BDBCC4`. It is a small retainer
for three CFade pointers at `+4/+8/+0x0C`; this is not proof of exclusive ownership. A
typed CFade callback covers event 1 numeric-state work and event `0x12` presentation data
feeding CRdPrim.

The application tail can consume a receiverless latch at `00BE1EAC`, clear it, obtain
manager `+4`, and feed CFade `+0x18C..+0x198` data to CRdPrim. A separate movie/task
mechanism can issue CFade request 7 and set that same latch, giving a concrete
movie/fade/presentation chain. Exact event delivery, last-writer/freshness, final GPU
meaning, and GOG remain unknown.

The apparent weather globals are also refined: CEffectRain and CEffectHaze pointers at
`01437414/01437418` are fields inside the static CMap object rooted at `013936F0`, not
independent manager singletons. CMap is therefore a verified retainer at this scope, not
a proven exclusive owner.

## Audio request-record chain

The selected Steam audio path joins resource and sound domains more tightly:

```text
SND_SE.PRM table
    -> CSound entry 0x4E
    -> SE_LIST row 64
    -> key PLSE066.PCM
    -> retained inline named-node address
    -> CSdMain / CSdCore request and status-record consumers
```

The generated request token, core slot, and status value are distinct numeric domains.
The same retained node pointer reaches multiple selected consumers, including a conditional
bank-facing path. Playback success, API semantics, ownership/lifetime, field names,
GOG homology, and the newly selected static CSdCore cleanup branch are still open. Seq55
explicitly chose that cleanup branch as the next research frontier, so audio is not closed.

## Other actor mechanisms

Phase 4 also adds two bounded actor slices useful for future work:

- selector `0x14` constructs a typed CItem, event 0 stores its payload-derived value at
  `+0x4A8`, and later name formatting plus typed CRdData returns are joined to same-object
  resource slots `+0x160/+0x164`; this is not proof of Player ownership or resource success;
- selector `3` constructs typed CNpcEnemy, and a selected update path computes a camera-
  related scalar with inclusive 80/90 thresholds around bit `0x2000`, then later reaches a
  marker/event-2 boundary; opaque calls prevent promoting full AI meaning or state
  preservation.

These are architecture footholds, not complete item or NPC subsystems.

## Promotion limits

The durable rule for all of the above is:

```text
verified conditional mechanism
    != live cadence
    != successful operation
    != exclusive ownership
    != safe destruction
    != cross-build homology
    != production-ready patch
```

Use the domain pages for older paired-build evidence and the preserved Census reports for
exact Phase 4 proof scopes.
