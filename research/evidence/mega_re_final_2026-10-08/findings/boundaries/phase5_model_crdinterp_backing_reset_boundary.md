# Phase5 — Model/CRdInterp resource-link clear and backing-boundary attempts

Date: 2026-10-05. Sequence118. Build: **STEAM_PC only**.

**VERIFIED:** minimum caller receiver transport and exact local reset/load/call/clear mechanics. **STRONG_INFERENCE:** selected model inline region is CRdInterp-compatible composition, using accepted prior scene/parser naming and this shared reset. **UNKNOWN:** actual current owner/destruction selection, current dynamic type, resource/backing allocation identity, owner/resource/backing generation continuity, call success, final borrower/use, safe retirement and replacement success. No GOG equivalence or runtime behavior is established.

## Source and promotion proof

Prospective contract: `scratch/phase5_model_crdinterp_contract_seq0118.md`.

Three exact half-open windows, staged for alignment/count before checking/display:
- `00402DFA..00402E10`: retained model prefix, 4 instructions / 22 bytes.
- `006B9E70..006B9E81`: retained initializer prefix, 6 / 17.
- `006B9FD0..006BA089`: one cold complete exported helper, 59 / 185.

Total **3 identities / 3 windows / 69 instructions / 224 encoded bytes**, below 3/5/96/384; main prefixes10/39 below16/64 and branch59/185 below80/320. No new raw data, RTTI, table, callee, allocator, GOG or runtime acquisition. Personal helper replay is the same source, non-additive. Existing initializer size/count/array semantics and reset identity receive no new discovery credit.

Primary records:
- `scratch/phase5_model_crdinterp_callers_personal_seq0118.json` — personally PE/ASM-checked two minimum prefixes.
- `scratch/phase5_model_crdinterp_branch_seq0118.json` — fresh independent Opus exact-body PE/ASM check.
- `scratch/phase5_model_crdinterp_personal_seq0118.json` — primary personally checked all59 body instructions, bytes and direct targets.
- `scratch/phase5_model_crdinterp_comparison_explicit_seq0118.json` — exact59 ordered VA/ASM-line/bytes/assembly equality, explicit span/target/source schemas, same-source hashes and caller proof, **before canonical promotion**.

CRdInterp name/layout seed is reused from `findings/boundaries/xca_crdinterp_scene_state.md` and `findings/formats/xca_crdinterp_parser.md`. The accepted11-byte `0074EE0B` adapter tail-forwards its argument to `0074E82B`, the existing deallocation boundary: `findings/boundaries/phase5_model_buffer_packet_retirement.md`. None of those implementations/type metadata is reacquired here. Existing table0076E6AC is canonically an explicit vftable symbol with class unresolved; this batch reads no RTTI and does not promote that symbol to a new named/current-type proof.

## C0231 — two local caller receiver paths

**VERIFIED:** `00402DFA` forms `ECX = ESI + 0x1B4`; `00402E05` writes literal0076E6AC to `[ECX]`; `00402E0B` directly calls006B9FD0. Accepted ordinary-model context identifies originalESI as M, hence the selected region E=M+1B4. The intervening stack-byte write does not push a separate helper argument. This is an available ordinary-model member-reset path, not evidence that an actual current model selected its deleting interface.

**VERIFIED:** initializer006B9E70 saves incomingECX at `[EBP-14]`, reloads it at006B9E79, then directly calls006B9FD0 at006B9E7C. No explicit stack argument is supplied to the reset. The acquired prefix stops at its return address; later resource/header/allocation behavior is old cited scope, not new primary credit or a proved replacement-success/generation join.

**STRONG_INFERENCE:** shared accepted CRdInterp initializer/reset and model inline tablewrite/geometry support CRdInterp-compatible composition. Named/current dynamic type remains unproved by these new windows. Independently allocated sceneE from the old report is not this embedded modelE; no owner or occurrence/generation identity is transferred between them.

## C0232 — exact body storage/resource distinction

**VERIFIED:** entryECX is saved at006B9FD6 to `[EBP-10]`. Subsequent receiver operands are local reloads from that slot; no additional explicit write to the slot occurs in this body. This describes local receiver transport, not protection of the receiver/fields against opaque calls, aliasing or external mutation.

Before every backing guard, the body explicitly writes:
- DWORD E+4 = 0 at006B9FDC;
- floating zero at E+14/18/1C/24 at006B9FE8/FF0/FF8 and006BA00A;
- DWORD E+20 = 0 at006B9FFE.

The previous E+4 value is **never explicitly loaded or passed** by this body. Prior initialization identifies that storage as a retained resource link, but zeroing it is not a resource release/free operation. Distinct E+4 and E+8/C/10 storage offsets do not prove distinct pointees/allocations or exclude resource/backing aliasing. No final-return E+4-zero invariant is asserted across opaque calls.

| Field | Guard | Fresh argument load | Spill/PUSH | Deallocation-boundary CALL | Post-normal-return clear |
|---|---|---|---|---|---|
| E+8 | 006BA010 | 006BA019 | EBP-4 / 006BA022 | 006BA023→0074EE0B | 006BA02E |
| E+C | 006BA038 | 006BA041 | EBP-8 / 006BA04A | 006BA04B→0074EE0B | 006BA056 |
| E+10 | 006BA060 | 006BA069 | EBP-C / 006BA072 | 006BA073→0074EE0B | 006BA07E |

**VERIFIED:** taken blocks occur in +8, +C, +10 order. Each guard is followed by a separate current field load, stack-local spill/reload, then PUSH of that loaded pointer value. Guard sample is not a body-entry/argument snapshot or a proved allocation-generation guard. Later +C/+10 observations follow any taken earlier call. Each returning call is followed by ADD ESP,4, a receiver-slot reload and the explicit corresponding field clear. Zero guard skips both that call and its clear. There is no call-result success test gating a returning call's clear.

Through the accepted adapter, these are concrete **deallocation-boundary attempts** on the loaded values—not pointer clearing alone. They do not prove pointee destruction, successful cleanup/free, distinct or owned arrays, preserved resource identity, last use, or safe retirement. Scalar reset roles and backing element meanings remain unnamed. Highest explicit field extent is E+24..27, giving an observed reset-region lower bound0x28, **not sizeofCRdInterp**; prior scene allocation0x30 remains a separate accepted observation.

## First genuinely unproved stronger edges — stop and pivot

1. Actual current model/scene owner-generation reaches this available member reset/initializer and selects a destruction/retirement occurrence. A static direct call inside available ordinary cleanup is not actual cached/root dispatch.
2. Current resourceE4 and guard/reload/backing arguments correspond to a specific created/owned allocation generation. Shared fields/address/class do not join generations; distinct storage offsets do not prove allocation identities.
3. The attempted deallocation succeeds and follows the last scene/renderer/other borrower use. Local call/clear order is not global last use, admission closure or safe retirement.
4. Initializer reset corresponds to successful replacement on that same generation. The acquired prefix and old allocation map do not establish that join.

These are **UNKNOWN**, not a three-attempt block or whole-corpus exhaustion. The new local mechanism is an **ADVANCE**, while stronger lifetime closure remains NOT_READY. Do not descend into0074EE0B/CRT, other helper bodies, Model packet/Motion/interpolation, scene/cache, XCA semantic consumers, factories, handles/freecaller or GOG/runtime lanes. Globally reassess the independent Phase5 frontier after synchronization/validation.

## Failures, corrections and conservation

Entry117 exact30 hashes/control and four current structural checks passed without semantic re-auditing108–116; last persisted research outcome was BOUNDED_NEGATIVE, ModelREADY/counters0/0/0. Entry/tool/agent setup failures and cancelled-task source-read uncertainty are retained in `scratch/phase5_model_crdinterp_setup_failures_seq0118.json`; branch's four empty-pages Read rejections are in its immutable receipt. Failed worktree launches created no worktree; a stopped local task yielded no evidence result, with pre-cancellation primary reads UNKNOWN rather than asserted absent.

The first comparison's ordered59 instruction rows passed but its metadata-span equality assertion failed. Original helper/failure survive in `scratch/phase5_model_crdinterp_failed_comparison_helper_seq0118.py` and `scratch/phase5_model_crdinterp_comparison_failure_seq0118.json`. Shell continued into hash/report reads only, with **no canonical/control promotion**. Explicit named mappings for span objects, call-target records and absolute/relative source paths then passed before promotion; no immutable proof was rewritten or new primary acquired.

All prior70+18 obligations, required world/cache/worker/provider/Game/audio joins, original Movie NOTPASS/prospective amendments, seq106 disconnect/107 failed-check chronology and frozen source evidence remain unwaived. Structural validation is required but is not semantic acceptance or a phase transition. Phase5 remains ACTIVE; Phase6 has not begun.
