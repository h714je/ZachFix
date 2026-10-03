# Typed model animation state to same-instance submission

**Date:** 2026-10-03. **Batch:** Phase 3 sequence 26, ANIMATION_RENDER_OWNER. **Outcome:** ADVANCE. **New build scope:** Steam. **Claims:** C0144/C0145. **Boundaries:** BND-123/BND-124.

## Contract and reuse

`scratch/phase3_animation_seq26_contract.md` limits this to the missing typed receiver/state/temporal connection. Main owns the update/output genealogy and canonical promotion. A bounded Opus branch investigated the consumer; main read its checker/draft, reran it locally and independently checked the decisive packet, matrix-source, copy and cache-selection instructions against the PE.

C0143 supplies actual registered 0x3B0 CRdObjectModel creation (`006C5930` selector1, constructor00402A30/table0076E704). C0126 separately supplies CPlayer creation (`005E7620` selector1/table007767FC). These factory namespaces are **not interchangeable**. C0117 supplies object-loop-before-pre-render ordering; C0130 supplies packet148/scene63A8 interfaces; C0141 supplies original-object membership/query protocol. Their discovery credit and original limits remain unchanged. No established homology for006C1430/006BD4C0 was found in the canonical lookup; no new GOG mapping is asserted. No generic actor+50, full scene/tree, interpolation or GPU reconstruction.

## Typed update and conditional placement — VERIFIED

Define **M** as the model object's pointer, **R=M160** as its selected resource pointer, and **P** as the packet pointer. Offsets below are **bytes**.

Raw RTTI names `.?AVCRdObjectModel@@` at0076E704 and `.?AVCPlayer@@` at007767FC. Both install:

| Virtual byte slot | Target | Selected role |
|---|---|---|
| 10 | 006BCBE0 | Native event3, then gated preparation/evaluation |
| 28 | 006BD4C0 | Model packet population |
| 44 | 00405E40 | Retained packet capacity/allocation and virtual28 |
| 48 | 004029B0 | Reused original-object membership protocol |

006BCBE0 saves incoming **ECX=M**. After native event3 and numeric resource/control masks, **M20 bit10 clear** allows006BCC88->006C1430 with **ECX=the same M**. Other selected helpers before/after also use M. Native events may alter state; this is not an all-path validity/safety proof.

The acquired scene dispatcher's numeric **object phase4**, written at006C69BA, indexes scene1CAC objects, applies006C7A80 and006C7960(0,100000) guards, then calls **that original object's virtual10(0)** at006C6A6A. This is a class-proven conditional interface/update route, not observation that a particular created instance is selected in a live frame. Phase 4 here is an **object-pass number**, not project Phase 4 authorization.

Selected006BE6E0 setup retains model ECX, publishes resource inputs at160/164, allocates state records at1E4 with observedA0 stride and matrix storage at1E8 with observed40 stride, clears the buffers, and invokes006C1430(0) on the same model. The matrix allocation includes vector-construction/header mechanics; the stride is not an asserted complete allocation size. Reused C0114 establishes CRdMesh/CRdPicture association for the typed **CLevel context**, not the dynamic type of every model160/164 value.

## Actual matrix-output genealogy — VERIFIED selected mechanics

006C1430 first rejects nullM160. Both numeric parameter arms obtain:

- **M1E8:** matrix-output buffer;
- **M1E4:** state-record buffer;
- **&M98:** base-matrix argument;
- **&M200:** additional state argument;
- selected optional **M350 / &M338 / &M35C** plus numeric flags.

At006C1689 and006C181F it calls0070C130 with **ECX=R=M160**, not M. Ordered stack arguments begin `(M1E8, &M98, M1E4, &M200, ...)`. Selected0070C130 nonnull-state branches step input state byA0 and use named D3DX rotation/multiply operations. At0070C315/38E the destination is **M1E8 + 64*i**. Thus M1E8 is not merely a pointer read by two unrelated functions: it is a selected output of the typed update.

The scaled arm temporarily multiplies/divides state-record20/24/28 andC0/C4/C8 around evaluation. Full interpolation, node names, flag meanings, zero-scale safety and all helper branches remain UNKNOWN/unvisited. **STRONG_INFERENCE:** the broader role is model animation/pose evaluation; **VERIFIED:** the described receiver, arguments and selected matrix writes.

## Same-instance buffer consumption and packet storage — VERIFIED

006BD4C0 saves **ECX=M** at[EBP-E8] and its nonnull second stack argument **P** at[EBP-4]. Selected006C3B80(0,0), called with M, reads M1E8 and returns the indexed64-stride pointer or zero; packet population rejects zero.

Selected stores establish **P34=M**, **P84=M160** and **P88=M164**. P80 is **align-up-to-16(P+1D4)**, an address inside packet backing storage—not an alias assignment P80=M1E8.

At006BD990/996 the same saved M supplies **M1E8**;006BD99A/9A0 supplies **P80**;006BD9A7 supplies **ECX=M160**;006BD9AD calls **0070CF90(P80, M1E8, 0)**.

The direct blocking writer0070CF90 has a bounded448-byte body. Selected source addresses are **M1E8+64*i**. It calls PE-import-proven **D3DXMatrixMultiply**, then **D3DXMatrixTranspose** on temporary matrices. A later resource-selected byte index chooses a temporary;0070D12E/133/136 copies **12 dwords=48 bytes** into destination P80 and advances destination30 bytes. This proves selected source-derived values enter submission storage, not only forwarding of an opaque pointer. Payload selectors1/11, count/index validity, friendly bone/skinning/shader meanings and full packet grammar remain untyped.

## Temporal join and essential cache qualification — VERIFIED conditional mechanics

Under the reused C0117 ordering, eligible object updates precede pre-render. Reused C0141 supplies original-object query entries. For a selected object **M**, pre-render preserves that same pointer for virtual24, virtual44, packet getter and scene append. Raw model virtual44->00405E40 forwards **M148**, context and **ECX=M** to the same table's virtual28=006BD4C0. Later006E1150 returns that object's valid **M148 pointer value**, which is appended to scene63A8.

**Population is conditional:**006D2F35/52 forces virtual44 when mask8000 is nonzero; otherwise006D2F5A/61 permits an already-valid packet to **bypass repopulation**. Therefore the positive connection is:

`typed M update -> selected M1E8 writes -> conditional same-M packet population -> same-M148 pointer submission`.

It is **not** “every animation update submits the newest pose every frame.” Exact invalidation/forced-refresh policy, concrete live M selection/query membership and cadence remain UNKNOWN. Existing packet/cache behavior is part of the recorded boundary, not a reason to reconstruct the full renderer here. Allocation failure is also not proven safe: null P can return1 as a probe; producer return alone is not population evidence.

For exact CRdObjectModel table0076E704, **virtual4C=00401D10 is a one-byte RET**. It does not serve as the renderer endpoint in this selected route. Other classes'4C targets, including accepted CLevel attachment work, remain separate; no universal slot semantics are inferred.

## K0021: required seed/provenance correction

The old seed `xmd_actor_animation.md` treats1E4/1E8 as timing/state and gives optional offsets350/358. Primary evidence proves selected **buffer roles**, optional350/**338**, base98/additional200 and35C. `xmd_object_render_handoff.md` gives unscaled decompiler indices58/59/79/7A as offsets; actual byte offsets are **160/164/1E4/1E8**. These old paragraphs are preserved with explicit supersession. This is a required current foundational correction, not a reopening/regrading of accepted Phase2 acquisition or a new GOG claim.

The ANIMATION subsystem's inherited shutdown field listing006C1430 is corrected: it is the proved update consumer, **not cleanup invocation**. Actual animation-buffer/model teardown remains UNKNOWN. Broader old SI claims are not automatically promoted.

## Reproduction and evidence references

- Main producer: `python scripts/inspect_phase3_animation_owner.py` — **754 instructions/2614 bytes**,2 raw types,8 calls. Receipt `scratch/phase3_animation_owner_primary.json`.
- Bounded consumer branch, locally rerun by main: `python scratch/seq26_submission_agent_check.py` — **344/1324**,14 relative calls,86 exact opcodes, raw RTTI and2 PE import names. `scratch/seq26_submission_agent_receipt.json`/draft. Supplementary891/3333 packet literal scan is overlapping byte verification, **not full semantics or transitive absence**.
- Main independent decisive consumer: `python scratch/verify_seq26_submission_main.py` — **169/681**. Receipt `scratch/seq26_submission_main_verification.json`. Counts overlap and are not additive. Initial shell assertion expected the wrong source register at006BD990; inspection showedEAX, assertion corrected and passed **before promotion**. No semantic reversal or source alteration.
- Producer/update: `inputs/decompiler/steam/DP_full.asm:783989-784057`, `:785933-786067`, `:789071-789397`, `:795420-795463`, `:884316-884475`.
- Consumer/copy/conditional interfaces: `inputs/decompiler/steam/DP_full.asm:784687-784736`, `:784809-784811`, `:784850-784857`, `:784956-784986`, `:791927-791953`, `:885254-885361`, `:5815-5836`, `:812144-812183`, `:812209-812216`. Exact per-instruction lines/import/table bytes in receipts take precedence over broad navigation ranges.
- Supplied PE `inputs/binaries/steam/DP_STEAM.exe`; all supplied inputs/binaries/assets/production source unchanged.

## First missing edges and Phase3 consequence

**Selected Phase3 architectural obligation is positive:** a class-proven update caller, concrete state output and same-instance packet consumer are joined at conditional static lifecycle scope. No unidentified interface is assumed on this selected route. Main-reviewed readiness assessment follows; this finding alone is not phase acceptance.

**UNKNOWN/unvisited carry-forward:** actual typed instance/query selection; latest-pose freshness/invalidation; state/packet/resource retirement coordination; full interpolation/format/pass/GPU; new GOG mapping and runtime cadence. Stable `ANIMATION_LIFECYCLE_CONTINUATIONS` requires a new typed selection/refresh/deleting/retainer source or reproducible trace. `ACTOR_SLOT50_DISPATCH` and accepted scene/root/registry source bounds remain closed under original triggers. No further mechanism batch is immediately necessary for this Phase3 question.
