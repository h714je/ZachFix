# Event incoming transport and fixed-word mask wrapper — sequence141

**Steam bounded primary.** Main checked exact selected Event caller prefix[00447B50,00447B93) (16 instructions/67 bytes) and helper00712D50[00712D50,00712D7A) (17 instructions/42 bytes); receipts/scope in scratch/phase6_event_atomic_personal_seq0141.json and scratch/phase6_event_atomic_main_adjudication_seq0141.md. No full caller/callee/producer/worker/GOG/runtime expansion.

## VERIFIED Event incoming transport

00447B51 copies entryECX to ESI; no instruction in the permitted prefix changes ESI. At00447B8C, ECX is reloaded from ESI immediately before00447B8E calls00441470. A stack-derived pointer at original entry+4 supplies a byte selector through an indirect local table, but does not flow to selected ECX. Thus the Event coordinator receives the untyped entry receiver of00447B50. This confirms the exact control seam but does not distinguish accepted CEvent root from script/context receiver, recover caller ABI/class, or establish ownership/generation.

First missing edge is the producer/typed identity of00447B50 entryECX. Do not open the full 1131-byte caller merely to repeat this prefix; only a new discriminator that can resolve that identity justifies scope expansion.

## VERIFIED fixed-word mask wrapper

00712D50 calls opaque00712D80, loads P from first stack argument, captures old A=[P], reloads B=[P], computes B AND second stack argument, stores the result to[P], calls opaque00712DB0, and returns captured A. Complete body is17 instructions/42 bytes. In retained006EB0F0 use, P=014940F8 and mask=-1, so the visible middle operation preserves B; the returned value is A captured before the store. No atomic instruction, wait, decrement, scheduler call or explicit readiness operation occurs in this body. Hook effects remain opaque.

This resolves the earlier helper edge only to a local mask/update wrapper. It does not establish synchronization/admission versus shared accounting, word ownership, Core+67CC8 alias, or concurrency correctness. The fixed word's participation in006EAB30 remains separately verified. Exact next edges00712D80 and00712DB0 are unvisited but should be selected only if their architecture value outranks independent residuals; generic wrapper collapse is a valid stop.

## Camera candidate negative and preserved mismatch

Sequence140 Camera own interval checked 1665 instructions/5836 bytes and repeatedly acquires/publishes CSingleton<CCamera> table007721BC through00BE1EA4; first opaque jump table is00523A15. However functions.csv reports body_bytes=5125 for the same declared interval. This source-contract/count failure means no canonical promotion of00522D50 or its full role is made. The branch’s exact local repeated-camera-cache facts are retained as a bounded negative: high fanout may be inflated by auxiliary cache construction, but incoming owner and cross-subsystem root remain UNKNOWN. The mismatch is a metadata/extent discriminator, not evidence of a new architecture.

All Phase5 acceptance, 88 guards, source/runtime/build limits and Phase7 prohibition remain unchanged.
