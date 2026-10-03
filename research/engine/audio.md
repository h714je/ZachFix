# Audio request and retained-record architecture

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
