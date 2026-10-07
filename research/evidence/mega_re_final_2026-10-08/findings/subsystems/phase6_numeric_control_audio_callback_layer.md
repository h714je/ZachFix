# Receiver-local numeric control, audio and callback interfaces

## Primary scope

Steam0046DEB0[0046DEB0,0046E2DD),287 owned instructions/1069 bytes, metadata/interval/PEASM geometry MATCH. Main source: scratch/phase6_main_control1069_personal_seq0174.json; exact direct selector/float literals: scratch/phase6_main_control1069_data_seq0174.json. No caller/callee/virtual target or gap source; selector table at0046E2E0 is directly referenced data, not the intervening three-byte gap.

**VERIFIED C0299:** saved incoming ECX=R; first stack DWORD N; unsigned N-1<=10 selects11 exact raw targets. Otherwise normalRET18. Targets for N1..11: DECC/E2D7/DF6C/DFA5/DFDA/E094/E154/E1EC/E265/E292/E2A8 within this body. N2 is the no-op exit. No allocation, root publication, own table installation or new manager construction occurs in this selected body.

Selected mechanics:
- N1 compares incoming value against R6630 whenR6628=1, otherwiseR665C; inequality writes receiver6628..6640 state/arguments. Numeric -1 invokes opaque0046BF10; other values may use R6664 to request accepted CSdMain/007201F0 interface before R665C=-1. Actual audio effect/record generation and field policy remain UNKNOWN.
- N3/4 and10/11 copy current receiver floats into neighboring fields, write literal0/1 and flags. Direct floats007722F4=3.0,00770370=120.0,00772A48=60.0,00773174=15.0 are PE-backed numeric constants, not proof of seconds/duration/mode names.
- N5/6 perform selected float/byte writes and opaque same-receiver requests0046C240/0046DD70, then set originalR+6770 as a two-DWORD traversal. Each iteration uses current00BD7670 as ECX and the current DWORD as the006C5FD0 request operand. Nonzero returned O first conditionally goes to current[0148132C] with pushed(O,76,&localDWORD), then current[O+44] is independently reloaded and conditionally invoked with that tuple. LocalDWORD is1 for N5,0 for N6. ESI is replaced by O; the traversal's originalR-relative address remains in EDI. Neither callback target/effect nor object class/generation/ownership is proved.
- N7/8 conditionally request accepted CSdMain result as ECX for007201F0, then write R665C/R6664=-1. N7 additionally invokes opaque same-R0046D3A0 and writes separately acquired Game-compatibleG1+CE5F0 WORD0, then separately acquiredG2+8C5EC OR20. N8 separately acquires G and ANDsG+8C5EC withFFFFFFDF. Both traverse six DWORDs startingoriginalR+6884: non--1 values participate in the audio request interface before that traversed slot becomes-1; R689C=0 is written per iteration. No success/last-use/free implication.
- N9 invokes opaque same-R0046A720 and two accepted CSdMain-result00720330 requests. The pushed operands and callee ABI/effects are not upgraded to named record semantics.

Accepted context reused, not reopened: CGame_acquire0040A320; CSdMain_acquire_singleton00427700; selected007201F0 operand/interface qualification from Phase5; existing006C5FD0 and typed00BD7670 root; optional0148132C transport from findings/boundaries/phase6_optional_callback_record_delivery.md. Stack values adjacent to getter/method calls do not make the getter consume them; normal returns do not prove success.

## Architectural disposition

**STRONG_INFERENCE role:** receiver-local numeric control layer with known audio/Game/object-callback interfaces. This qualifies frozen rank49 independently (B/C convergence) and deprioritizes a new shared-root/manager interpretation for this body. Incoming R class, caller-wide role, selector names, callback76 effects and transition timing remain UNKNOWN. The minimum own-body organizing/control discriminator is resolved; no new typed producer boundary or subsystem/homology is promoted.

First static stopping edges are opaque same-R helpers, current callback targets, selector producer/incoming typedR, and the unqualified00720330 operation. None is automatically a material readiness blocker absent a map dependency. Independent rank47/48/55/other sources remain eligible;168NOT_READY is not overruled by this collapse. No Phase7/BND-244/runtime/ownership/gen/safe-retirement claim.
