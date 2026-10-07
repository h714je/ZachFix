# Audio request and retained-record architecture

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Selected audio paths are mapped, not an exhaustive behavioral or surround-sound runtime proof.

**Jump to:** [Selected resource/name chain](#selected-resourcename-chain) · [Selected request record](#selected-request-record) · [Architectural cautions](#architectural-cautions) · [Current frontier](#current-frontier) · [MegaRE final audio addendum (2026-10-08)](#megare-final-audio-addendum-2026-10-08)
<!-- END AUTO RESEARCH NAV -->

**Research snapshot:** 2026-10-04.
**Build scope:** Steam PC selected static mechanism; GOG/runtime correspondence open.

This page records the first typed resource-to-audio request chain recovered by the Mega
RE Census. It is not a complete sound engine map and does not prove successful playback.

## Selected resource/name chain

A class-proven CSound writer joins two supplied PRM resources:

```text
UPDATA/PRM/SND_SE.PRM   resource 0x3A0B
    -> selected entry 0x4E
    -> first short = 64

UPDATA/PRM/SE_LIST.PRM  resource 0x3A05
    -> row 64 = "plse066"
    -> formatted lookup key "PLSE066.PCM"
```

A successful named lookup returns an inline address `P = node + 0x20`. CSound retains
that inline address. It is not a dereferenced owning payload pointer, and matching the
lookup key does not prove that every downstream name field contains the same string.

## Selected request record

The selected CSound adapter builds a request that carries row 64 and the retained `P`
into CSdMain. The resulting main/core path has at least three distinct numeric domains:

```text
T  generated CSdMain token
K  CSdCore slot/output index
Q  independently allocated status-record index
```

Do not use one as a friendly alias for another.

The same `P` is retained into selected status/record associations. Concrete consumers
read `P+0x04`, `P+0x14`, and `P+0x2C`; those are verified offsets, not yet friendly field
names. A conditional later path reaches an opaque bank/interface table, but the bank
identity, API contract, successful voice creation, and playback status are not proved.

## Architectural cautions

- a normal outer return is not proof of successful playback;
- the selected bank operation can be skipped while a nonnegative core slot remains;
- node/name/bank/interface lifetimes are not closed;
- selected request/control blocks include bytes not initialized by the reviewed adapter,
  so do not invent a fully initialized public struct;
- CSound, CSdMain, CSdCore, shared bank storage, and backend/static objects are distinct
  roots even when they participate in one request.

## Current frontier

The latest Census checkpoint seq55 selected a static CSdCore cleanup branch as the next
audio target. That branch is not yet evidence for actual shutdown chronology or lazy-free
policy. Audio therefore remains active research.

Exact source report:
`../evidence/mega_re_census_2026-10-04/boundaries/audio_descriptor_request_record_chain.md`.

## MegaRE final audio addendum (2026-10-08)

The selected audio architecture is now separated into distinct identity/storage domains:

```text
resource / PRM / name
    -> CSound row and coordination ID q
    -> stored Main operand / Main token T
    -> Core index/token K
    -> status Q
    -> cue/bank/backend requests
```

`q`, `T`, `K` and status are not one universal sound handle. Main tokens can wrap/reuse, so equal numeric values at different times do not establish the same playback episode. Selected organizing capacities are about 256 CSound rows, 128 Main rows and 32 Core rows.

Post-Phase8 work connects a typed `CAudio_Data::FILEITEM` writer to selected bank/control/name inputs, and a GOG installed callback path to event/state arm -> typed CSound -> Core/Main request flow. Current recurrence and live-episode identity are still runtime questions.

Speaker layout enters through the backend mix-format speaker mask passed into X3DAudio initialization; the X3DAudio handle is separate. A later selected path applies an explicit 2x2 diagonal matrix, giving the 5.1/7.1 brake-loop investigation a concrete static seam without proving causality.

Logical row reuse/completion can precede final backend stop/destruction, so “available row” and “backend voice gone” are different lifetime claims.

See [`FLOW_AUDIO.md`](../evidence/mega_re_final_2026-10-08/maps/FLOW_AUDIO.md).
