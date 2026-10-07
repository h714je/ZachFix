# Audio singleton cleanup before optional receiver deallocation

**2026-10-04; Phase5 bounded supplement; STEAM_PC.** C0210. Not a substantial shared-last-user portfolio or lifetime closure. Reuse C0118/C0178/C0179/H0266–H0268 at their accepted scopes only.

## New finite relationship — VERIFIED availability

Raw`00773154`→COL`00876D00`→TD`008A8330` names`CSingleton<CSdCore>` and slot0=`0046F8F0`. Complete36-byte interface (ASM128912–128923):

- saves incoming ECX as ESI and writes singleton table;
- `0046F8F9` directly calls accepted shared cleanup`0072C2E0` with the unchanged incoming receiver;
- **only after cleanup returns**, `0046F8FE` tests original argumentbit0;
- if set, passes saved ESI to accepted outer-deallocation boundary`0074E82B` at`0046F906`;
- returns saved receiver/RET4.

Flag0 still enters cleanup. Argumentbit0 is an optional receiver-deallocation selector, **not an observed shared-user count/gate**. The complete local interface has no direct read/clear of cache`0138A6E4` or root-user count. This local fact does not constrain indirect callee effects or prove global absence. Return of a pointer value after a boundary call is not proof of continued live storage or operation success.

## First missing edge — UNKNOWN / NOT_READY

No finite source here supplies the currently acquired`0138A6E4` generation to an actually invoked`0046F8F0`/cleanup. Typed getter/table publication proves interface availability, not invocation. Distinct literal-static`014B0400` cleanup registration is not lazy-cache finalization, shared-instance equality or shared last use. Stop at root→actual selected interface, before any shared-last-user/completion analysis. Reopen only with a new exact typed provider/finalizer/callsite and pointer/generation transport or correlated trace; no staticbackend/CRT/registry/free/handle/thread/global audio census.

## Primary verification and retained clerical history

Independent scratch branch:`scratch/phase5_audio_root_branch_20261004.md`/`.json`: three slices82instructions/281bytes, of which only12instructions/36bytes are the new interface; getter checks reused. Main`scripts/inspect_phase5_audio_interface_main.py` → `scratch/phase5_audio_interface_main_confirmed_seq0090.json` and distinct`..._confirmed_replay_seq0090.json`: each12/36 plus raw type/slot/call operands. Importantjoins personallyverified after validatedseq89 READY. Counts overlap/notadditive. No callee cleanup or deallocator body reopened.

Seq89 recorderSyntaxError occurredbeforecontrol/canonicalwrite, but the initial compound shell command continued one early personalcheck while gate still STRATEGIC_REVIEW_REQUIRED. Preserved initial`..._20261004.json` and`..._replay_seq0090.json` are same-run duplicate outputs, **not independent replay and not used for canonicalcredit**. Minimalparenthesis repair, actualledger/state/heuristicPASS and separate NEWconfirmed/replay afterREADY restore the intended ordering without altering validators or hiding history. No initial canonicalpromotion. Future commands failclosed with`&&`.

All70+18 guards and sequence83 corrections remain binding; no input/production/runtime modification, actualsafe/free/lastuse/cadence/GOG claim, Phase4 audit, Phase5 closeout or Phase6.
