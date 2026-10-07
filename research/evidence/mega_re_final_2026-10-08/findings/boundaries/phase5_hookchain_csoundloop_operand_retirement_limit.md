# Phase5 HookChain inline-loop operand / available-retirement limit

**Date:** 2026-10-05. **Sequence:** 121. **Outcome:** ADVANCE at local interface/operand/order scope only. Phase5 ACTIVE; no Phase6. C0233/C0234, BND-229/BND-230.

## Prospective scope and proof

**VERIFIED:** sequence120 entry matched all61 final-snapshot files and selected HookChain READY / lastADVANCE / counters0-0-0. Existing118/119 semantics were not reopened. Fresh entry structural checks each exited0. Old57 history files/seven append-only prefixes matched. Authority: `scratch/phase5_hookchain_entry_seq0121.json`, `reports/PHASE5_CONTINUATION_VALIDATION_2026-10-05_SEQ0120_ATTEMPT3.txt`.

**VERIFIED:** actual alignment/count staging preceded PE checking/display. Main first two full Steam envelopes: `005F5100..005F5124` 12instructions/36bytes and `005F4E50..005F4EF8` 42/168. After personally passing the exact component-transport prerequisite, one fresh single-assignment Opus branch acquired `0069E710..0069E734` 13/36 and `0046CBC0..0046CBF0` 15/48. Total **4identities /4windows /82instructions /288encodedbytes**, within4/4/112/448; outer54/204 within80/320; downstream28/84 within32/128. Zero new rawdata/RTTI. Coincidence with the earlier metadata288-byte total is not the proof: contiguous actual instruction staging and PE matching are.

**VERIFIED:** main personally replayed the same downstream windows, non-additively, and compared explicit common schema fields: all28 ordered `(va,line,bytes,assembly)` rows, named spans/counts, direct-call targets, build and workspace-relative source/hash keys. Agreement passed before canonical promotion. No whole-object/prose equivalence assumption. No off-list body, sibling, constructor, getter, current-table target, lookup/base helper, allocator, GOG, runtime or XNV payload acquisition.

## C0233 — available outer interface and inline transport

**VERIFIED:** `005F5100` saves incoming receiver `O` in ESI, writes literal0077EAFC, directly calls005F4E50, then tests bit0 of its stack argument. Set arm pushes savedO to accepted deallocation boundary0074E82B; clear arm skips it. It returns savedO bits with RET4. This is an **available** ordinary-before-optional-outer-boundary interface, not a demonstrated current owner-selected invocation or successful destruction/free.

**VERIFIED:** `005F4E50` saves incomingECX, writes literal0077A50C, loads numeric `O+420`, acquires current[00BD7670] as ECX and forwards that loaded value to opaque006C5FD0. Nonzero returnedX yields current-table+30 dispatch after local `X+DC &= ~4` and rewriting loadedX+D8 bits. The current table target and effects are **UNKNOWN**. Local numeric transport and writes do not prove auxiliary destruction or last use.

**VERIFIED:** continuing code forms `L=O+434` in ECX at005F4EB8, stores-1 atO+420, writes literal0077A504 throughL, then directly calls0069E710 at005F4ED3 without intervening ECX modification. Only afterward does it restore savedouterECX and call opaque0045FAF0. The clear/write/call/base-interface ordering is explicit. O+420=-1 is not a free; table writes are not current dispatch proof.

**STRONG_INFERENCE:** selected inline geometry/table/helper transport is compatible with retained CObjectCHookChain / embeddedCSoundLoop434 seeds. Naming/allocation/constructor citations are retained from `findings/boundaries/xnv_hook_tackle_chain_classes.md` and classOBJ-COBJECTCHOOKCHAIN; they have legacy-reference/recovery limits. This batch proves the fixed relative transport, not fresh named/current RTTI, exclusive component ownership or actual constructor/destructor selection. Canonical raw tables0077A504/0077A50C remain classUNKNOWN; no new slot/type extent is assigned. GOG005F5100 is the retained other-build constructor, not this Steam interface homologue.

## C0234 — two observations, reacquired CSound and local current-record mapping

Use `q0 = Mem32[L+4]` before the getter and `q1 = Mem32[L+4]` after it. They are distinct observations, not automatically equal or one occurrence/generation.

**VERIFIED:**0069E710 saves callerESI and places incomingL inESI. q0==FFFFFFFF skips getter, operation and explicitL4 store. Otherwise0069E719 calls accepted004183D0, which supplies CSound resultA by cited prior evidence, then0069E71E reloads q1. It pushes literal0 thenq1, setsECX=A, and calls0046CBC0. Thus operation receiverA, firststackoperandq1 and second0 are explicit; the component is not the CSound receiver. CSound0138A6E0 is distinct from Main00BDBCC0/Core0138A6E4/static014B0400. Getter body/root census were not reopened; citation: `findings/boundaries/gameplay_audio_organizing_roots.md`.

**VERIFIED:**0046CBC0 reads the full firststackoperandq1. q1==FFFFFFFF directly RET8 without the deeper operation and leavesEAX=A on this caller path. Otherwise it forms `i=q1 & FF`. Its comparison of maskedi withFFFFFFFF cannot be equal. It compares current `Mem32[A+54*i+34]` against fullq1, forms `B=A+54*i+0C` with flag-preservingLEA, and selectsB on equality or0 on mismatch. It replaces firststackoperand withB/0 and tail-jumps0046B870, retainingECX=A, secondoperand0 and the caller return address0069E72B. The JMP target is personally decoded from the checkedE9 bytes; target body remains opaque.

**VERIFIED:** under ordinary ABI-compatible normal return,0069E72B storesFFFFFFFF atL+4 without checkingEAX, restoresESI andRET. Earlyq0-sentinel path leaves incomingEAX; q1-sentinel path leavesA; tail-operation path leaves opaque targetEAX residue. None is an established success result. q0 admission does not guarantee q1 admission, numeric match does not establish generation continuity, and normal-return store does not establish successful cleanup.

**VERIFIED at geometry only:** admitted q1 can select an interior recordaddressB from the separately acquiredA. Retained CSound constructor evidence describes256records/54stride, but this mapping does not identify a backing allocation, bank/resource, component-owned sound object, live playback request or safe access. No pointer-load/deallocation ofL4 is demonstrated in these four selected bodies. That is a scoped mechanism limit, not a claim of no cleanup anywhere.

## Identity and lifetime separation

| Item | Established scope | Stronger limit |
|---|---|---|
| OuterO and inlineL | VERIFIED saved receiver / L=O+434 geometry | UNKNOWN current type/owner/occurrence/destruction selection |
| L+4, q0, q1 | VERIFIED storage offset / two DWORD observations / -1 sentinel tests | UNKNOWN pointer/handle/resource domain and q0-q1 continuity |
| AcquiredA | VERIFIED getter result transported as operationreceiver; namedCSound by retained citation | UNKNOWN creator/current-root/owner-generation joins and getter side effects |
| RecordB or0 | VERIFIED full-value-match-dependent interioraddress selection | UNKNOWN current record/resource generation, allocation ownership or operation success |
| Literal0 and opaque tail target | VERIFIED secondoperand0 / tail transport to0046B870 | UNKNOWN type/effect/result, borrower release or finaluse |
| L4=-1 | VERIFIED explicit normal-return store only | Not free, lastuse, successfulcleanup, finalreturned invariant through later opaque base effects or globalretirement |
| OuterO boundary | VERIFIED available flag-bit0 pushO→0074E82B after ordinarycall | UNKNOWN actual invocation/success/borrower closure/safe release |

## First stronger edge and global pivot

**UNKNOWN / NOT_READY:** q domain and0046B870 effect onA withB/0 and literal0. Also unresolved: actualcurrentouterdispatch, O/L/A/q/B/resource/backing occurrence-generation joins, cleanup success, lastborrower/lastuse and safe retirement. No load/replacement source was selected; no replacement-success claim follows. Available outer cleanup and localcall/store are not globalaudio shutdown or currentHookChain retirement. Embedded-object identity, backingpointer, resource, ownership and retirement remain separate.

Stop this frontier here. Reopen only on independent actualselectedowner/type/generation evidence or a discriminating operand/effect/resource/finalborrower source—not because an opaque support helper is available. No006C5FD0/0045FAF0/0046B870/registry/handle/free/allocator/getter/sibling/GOG/runtime/XNV descent. Required world/CLevel/64slot, cachedrenderer, worker/provider/Input, Game/Car and Main/Core/sharedaudio retirement joins remain individually unwaived. Global Phase5 comparison is next, not Phase5 closeout or whole-corpus exhaustion.

## Failure / correction preservation

**VERIFIED:** main five initial empty-page Read rejections and one guessed missing helper path acquired no evidence from those calls; corrected entry reads/helper lookup succeeded. First Agent worktree launch failed in the non-git workspace; that failed invocation created no branch/worktree. A subsequent one fresh single-assignment branch completed. Branch records three empty-page Read rejections corrected before acquisition; no acquisition/count/PE/target failure. Original setup history/branch receipt remain unchanged.

**VERIFIED provenance clarification:** branch final receipt `scope.callee_bodies_acquired=0` denotes **off-list expansion only**, not all callee bodies: the expressly authorized0046CBC0 envelope was acquired and is counted. Branch Markdown and this report preserve that clarification without rewriting the sealed receipt. Main observed two authorized downstream bodies, zero off-list bodies. No failed119scout result is inferred; its original partial files/absentJSON/APIhistory and all older70+18/NOTPASS/corrections survive.

## Primary / derived references

- `scratch/phase5_hookchain_outer_stage_seq0121.json` and `scratch/phase5_hookchain_outer_personal_seq0121.json`: full exact outer instructionVAs/ASM lines/bytes/directtargets/sourcehashes.
- `scratch/phase5_hookchain_transport_gate_seq0121.json`: main personally checked prerequisite before downstream launch.
- `scratch/phase5_hookchain_downstream_stage_seq0121.json`, `scratch/phase5_hookchain_downstream_branch_seq0121.json`, `scratch/phase5_hookchain_downstream_branch_seq0121.md`: bounded independent source/counts and mechanical interpretation.
- `scratch/phase5_hookchain_downstream_personal_seq0121.json`, `scratch/phase5_hookchain_comparison_seq0121.json`: main same-source replay and explicit28-row/schema agreement; non-additive.
- `scripts/inspect_phase5_hookchain_outer_seq0121.py`, `scratch/phase5_hookchain_downstream_acquire_seq0121.py`, `scripts/inspect_phase5_hookchain_downstream_personal_seq0121.py`: reproducible read-only helpers.
- `scratch/phase5_hookchain_setup_history_seq0121.json`: initialsetup/entryfailures; immutable earlier preservation remains authoritative at its scope.

## Actual postpromotion validator failure and correction

**VERIFIED:** Initial postpromotion121 validator FAILED: CLASS40 changed free-site/notes with frozen informal evidence_refs; stale debt. Original failed receipt/promotion card and preamendment report hash preserved in scratch/phase5_hookchain_legacy_scope_failure_seq0121.json. Exact one-cell normalization now cites retained XNV class seed plus fresh bounded report; one debt row retired5520->5519, all other row fields/remaining debt rows exact. No new RTTI/type/constructor/confidence evidence; validators unchanged; no research/globalreview during correction. Attempt2 fourEXIT0/fullconservationPASS, followed by preservation/final recheck. scratch/phase5_hookchain_legacy_scope_correction_seq0121.json
