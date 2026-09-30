# CInput pipeline and latency evidence

**Status:** PC CInput core architecture closed; the normal producer/commit ordering, one-deep staging contract, edge/repeat derivation, dormant async-handoff API, and Xbox contrast are confirmed.
**Compared builds:** Steam 1.01b, GOG 1.01b, original PAL Xbox 360.
**Reconciled:** 2026-09-29.

This note records the evidence behind the CInput closure. It separates the native PC object model from the original Xbox input implementation and from later ZachFix consumer-side controller features.

## 1. PC homolog table

| Role | GOG 1.01b | Steam 1.01b |
| --- | ---: | ---: |
| `CInput` constructor | `0x00707D40` | `0x00707D90` |
| main snapshot commit / edge derivation | `0x00708300` | `0x00708350` |
| physical/evaluator producer | `0x00708AB0` | `0x00708B00` |
| normal main-thread poll wrapper | `0x00709980` | `0x007099D0` |
| async-handoff request (`+0xBC0 = 1`) | `0x007099C0` | `0x00709A10` |
| async-handoff query (`+0xBC0 == 2`) | `0x00709A00` | `0x00709A50` |
| dormant callback worker | `0x007097F0` | `0x00709840` |
| main-tick commit callsite RVA | `DP.exe+0x00001AB0` | `DP.exe+0x00001AB0` |
| main-tick poll callsite RVA | `DP.exe+0x00001AF0` | `DP.exe+0x00001AF0` |

The PC `CInput` allocation is `0xBD0` bytes.

## 2. Reconstructed PC CInput layout

Only fields needed for the proven pipeline are named here.

```text
CInput +0x000  vtable / base state
       +0x008  embedded callback-thread helper
       +0x094  TCallBackClass<CInput>
       +0x0A4  input critical section

       +0x0C8  live slot[0]
                7 slots, stride 0xCC
                +0x04 current/held mask      (object +0xCC)
                +0x08 rising mask            (object +0xD0)
                +0x0C rising|repeat mask     (object +0xD4)
                +0x10 previous mask          (object +0xD8)
                +0x2C/+0x30 accumulated motion-like channels
                +0x4C.. repeat timers

       +0x660  repeat timing anchor, low dword
       +0x664  repeat timing anchor, high dword

       +0x7E4  pending snapshot
                7 records, stride 0x40 = 0x1C0 total
       +0x9A4  pending slot/index state
       +0x9A8  pending count, normal values 0 or 1

       +0x9AC  producer aggregation slot[0]
                7 slots, stride 0x4C
                accumulates while pending snapshot is occupied

       +0xBC0  main-poll / async-handoff state
                0 = normal main poll enabled
                1 = handoff requested; one final main poll allowed
                2 = main poll suppressed
```

The pending area is exactly one seven-slot frame (`7 * 0x40 = 0x1C0`). This is not a multi-entry ring buffer.

## 3. Producer: physical state -> aggregation -> one pending snapshot

GOG `FUN_00708AB0` and Steam `FUN_00708B00` first acquire/evaluate a temporary set of seven `0x6C` records. The helper chain builds the logical digital mask and filtered analog channels, then merges them into the `+0x9AC` aggregation area.

Confirmed producer behavior:

```text
physical acquisition / action evaluation
    -> merge into 7 x 0x4C aggregate records
    -> lock
    -> if pendingCount != 1:
           pendingCount = 1
           copy aggregate -> 7 x 0x40 pending snapshot at +0x7E4
           clear transient aggregate fields
       else:
           keep merging new physical activity into aggregate state
    -> unlock
```

This is more precise than "a one-slot queue drops later input". When the pending snapshot is occupied, later producer calls cannot enqueue a second snapshot, but selected digital/motion state continues accumulating behind it for the next enqueue opportunity.

The PC controller evaluator also confirms the legacy port thresholds and shaping used before staging:

- digital directional bits are synthesized from analog signs at approximately `+/-0x32`;
- stick shaping uses an inner `+/-16` region before normalization;
- trigger-like channels are normalized from the legacy joystick range;
- friendly action names remain consumer-specific and are not inferred from bit position alone.

## 4. Commit: pending snapshot -> live CInput state

GOG `FUN_00708300` and Steam `FUN_00708350` are not simple copies. A commit performs three logically distinct jobs.

First, it preserves the previous digital mask for each of seven live slots:

```text
previous = current
```

Second, if `+0x9A8 != 0`, it consumes the one pending snapshot and copies its channels into the live slot state. Selected motion accumulators are added rather than blindly replaced.

Third, after the copy it derives edge and repeat state:

```text
rising = current & ~previous
selector 0 = current / held
selector 1 = rising
selector 2 = rising | repeat
selector 3 = previous
```

The public digital getter indexes these four masks directly from the live `0xCC`-stride slot.

Repeat bookkeeping is maintained per digital bit. The PC code uses a roughly 33.333 ms repeat-service cadence (`0x8235` microseconds) with hitch reconciliation around twice that interval, and per-bit counters are updated only for held bits.

## 5. Why `poll -> commit` is safe but an extra commit is not

The shipped PC main tick calls the same two native functions in this order:

```text
commit previously pending snapshot
poll/evaluate/stage next physical snapshot
```

Therefore a sample acquired during tick N normally becomes live for gameplay on tick N+1.

A same-tick repair needs only to reverse those two calls:

```text
poll/evaluate/stage fresh snapshot
commit fresh snapshot once
```

This preserves exactly one native commit and one native edge/repeat derivation per game tick.

An extra commit is not equivalent. Because commit begins with `previous = current`, a second commit in the same tick can collapse a just-created rising edge: after the first commit, the new held mask is already current; the second commit copies it into previous, so `current & ~previous` becomes zero. This is why the validated repair is a reversible call-order swap, not an additional commit.

## 6. Runtime producer census

The v5 read-only producer probe observed the shared native producer while preserving behavior. A clean Steam 1.01b session produced:

| Mode | main producer calls | background calls | queue empty on entry | queue full on entry | enqueued | full/no enqueue | `+0xBC0 == 0` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Vanilla `commit -> poll` | 2361 | 0 | 2361 | 0 | 2361 | 0 | 2361 |
| Same-frame `poll -> commit` | 3629 | 0 | 3628 | 1 | 3628 | 1 | 3629 |

The single full-queue observation occurred at the hot transition. There was no sustained queue contention and no background producer thread during either steady-state mode.

This closes the concurrency question for the normal shipped gameplay path used by the low-latency reorder: `poll fresh -> enqueue -> commit fresh` is the observed steady-state transaction.

## 7. `CInput+0xBC0` is a dormant async-handoff API

The normal poll wrapper implements the complete `+0xBC0` state transition:

```text
if state != 2:
    PollProducer()
    if state != 0:
        state = 2
```

Separate helpers set state `1` and query whether state is `2`.

Interpretation:

```text
0  normal main-thread polling
1  request handoff; allow one final main-thread poll
2  main-thread polling suppressed
```

Both PC builds contain the setter/query pair, but the shipped executables have no call/data xrefs to those API entry points. The constructor initializes `+0xBC0` to zero, and runtime producer telemetry remained in mode 0 throughout normal gameplay.

The field should therefore not be described as an active "30 Hz input mode". It is a preserved async/background handoff mechanism that the shipped PC lifecycle does not activate.

## 8. The 33.333 ms callback worker exists but is inactive normally

Both PC builds contain a CInput callback worker with a `TTimeCount<33333,0>`-style cadence. The CInput object also contains a callback object and an embedded callback-thread helper.

The important correction is lifecycle ownership:

- the constructor stores the CInput callback function;
- the constructor's callback-vtable call stores callback argument `0`; it does not launch the callback thread;
- the `+0xBC0` handoff API that would suppress normal main polling has no shipped callers;
- runtime producer census found zero background producer calls and zero background producer threads.

Therefore the worker implementation is real but dormant on the normal shipped path. It does not cap gameplay input at 30 Hz and does not contend with the validated same-frame reorder.

## 9. Original Xbox 360 contrast

The original PAL Xbox 360 input implementation uses a materially different arrangement.

Xbox `sub_82523238` synchronously calls the `XamInputGetState` wrapper for up to four controllers and immediately builds each controller record in the same update. The controller record stride is `0xE8`; the core digital masks are written directly as:

```text
record +0x08  current / held mask
record +0x0C  rising mask
record +0x10  rising/repeat composite
```

Per-bit held state and repeat timers live in the same record and are updated during that same `sub_82523238` pass. There is no homologous PC `+0x7E4` one-deep pending-snapshot handoff between polling and edge derivation in this Xbox path.

Cross-version conclusion:

```text
original Xbox:
    XamInputGetState -> normalize -> derive held/rising/repeat in one update

PC Director's Cut:
    poll/evaluate -> aggregate -> pending snapshot
    next main tick commit -> derive held/rising/repeat
```

The one-tick staging boundary is therefore a PC-port architecture difference, not a requirement inherited from the original Xbox input path.

## 10. Closed and still-open boundaries

Closed for the PC CInput core:

- physical/evaluator producer ownership;
- one-deep pending snapshot and aggregate-behind-pending behavior;
- commit semantics;
- held/rising/repeat/previous selector roles;
- normal main-tick `commit -> poll` order;
- reason that the order adds one staging tick;
- safety rationale for one `poll -> commit` reorder;
- `+0xBC0` state semantics and shipped inactivity;
- normal-path background-producer concurrency question;
- Xbox contrast showing same-update input/edge derivation rather than PC-style staging.

Still open elsewhere in the broader input domain, but not CInput-core unknowns:

- friendly names for remaining logical action bits from concrete consumers;
- camera modes 10/11 update-rate behavior;
- remaining original-control restoration policy/validation such as Quick Turn;
- any consumer-specific controller semantics not needed by the core CInput pipeline.
