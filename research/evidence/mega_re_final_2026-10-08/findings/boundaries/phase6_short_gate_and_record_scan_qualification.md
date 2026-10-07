# Short gate and record scan qualification — sequence 143 acquisition

**STEAM_PC only.** Primary anti-loop search found existing 0042D010 gate call locators in native-task reports, but no prior own-body qualification; no focused 00417C40 report. The sequence-143 strategic contract selected own 36 and 84 bytes only. Personal receipt: `scratch/phase6_short_bridges_personal_seq0143.json` (52 instructions / 120 bytes; exact ASM/PE). No caller, callee, GOG or runtime body acquired.

## VERIFIED: 0042D010 local receiver / Demo predicate

The complete 12-instruction body returns full EAX 1 only if receiver DWORD+44 bit 0x40000 is clear, receiver DWORD+34 is zero, and bit31 of fixed 008A6074 is clear. Otherwise it returns full EAX zero. It performs no local writes, calls, allocation, callback publication, wait or dispatch.

008A6074 is already accepted as CDemo+4 context flags, not a new manager. Existing accepted native-task reports associate this gate with an acquired CEvent context; **STRONG_INFERENCE** for CEvent-compatible use is retained separately from the **VERIFIED** raw predicate. Own body alone does not type arbitrary incoming ECX or establish successful task creation, coherent simultaneous flag observations, scheduling policy, generation, ownership or cadence.

**Bounded negative:** the high-fanin bridge is a narrow state predicate, not a newly discovered root or coordination service at this scope. It refines an already mapped Event/Demo-to-task gate rather than adding subsystem topology.

## VERIFIED: 00417C40 bounded stride scan with opaque comparison

The complete 40-instruction body captures B=[incoming ECX+4]. It observes relative DWORD B+68; nonzero produces first candidate B+offset, zero produces null. It tests unsigned WORD B+2A for an empty range. For candidates it passes the original stack DWORD and current candidate pointer to opaque 0040ECB0, with caller stack cleanup. EAX zero returns the current full index; nonzero advances by 0x90 and compares signed index against a freshly read zero-extended WORD count. Exhaustion or initially empty count returns full EAX -1. RET4 consumes the original stack argument.

The local code establishes relative-address/WORD-count/0x90-stride geometry and first-zero-result selection. It does **not** establish a string key, successful match semantics, a typed record format, producer, safe bounds, stable count, payload ownership or effect of 0040ECB0. The zero-offset path can pass a null candidate when count is positive; do not silently repair it. No receiver/global publication, allocation or callback appears locally.

**Bounded negative:** the high-fanin candidate collapses to a local record scan at this scope, not a manager/factory/dispatcher. The opaque comparator and typed producer remain static gaps; following them is not currently justified by architectural information value alone.

## Ranking implications

Both exact identities can be deprioritized as new-root candidates. The frozen original ranking remains untouched; additive dispositions qualify only these own bodies, not all callers, all record users, or the surrounding cluster. All inherited 88 obligations and Phase 5 selected-static acceptance remain unchanged. No Phase 7 or runtime policy claim.
