# CRdData shutdown versus CLoadThread retirement

**2026-10-04; Phase5 batch2; STEAM_PC.** C0183–C0185. Accepted C0035/C0120/C0152–C0154 provide typed roots and request/release interfaces; no duplicate start/queue/format reconstruction credit.

## Contract and outcome

Selected `P5_RESOURCE_WORKER_RETIREMENT`: actual application→resource-manager cleanup and typed worker stop/storage teardown, with API/result and release order. Fresh bounded branch returned a concrete caller; primary read the load-bearing assembly, replayed its checker into a separate receipt and independently checked the main joins.

**VERIFIED:** conditional application cleanup invokes CRdData session cleanup; descriptor/payload/table release locally precedes explicit stop of static01481130; stop clears loop/logical queue/count and sets period1, not a thread join; a separate registered finalizer's available destruction body releases queue storage before a vptr-only CThfunc base teardown.

**UNKNOWN:** admission closure, same-thread/request/manager generation, upstream last-use enforcement and actual quiescence. The selected local sequence does not prove a live race, leak, use-after-free, unsuccessful shutdown or absence of external coordination.

## Invoked cleanup versus available full deletion — VERIFIED

`00401310` zeroes ESI at00401322. At004013F3 it loads Dshutdown=[00BD768C] into ECX, conditionally calls006B2670 at004013FD, and after normal return clears only the application root at00401402. Accepted startup publication supplies typed CRdData provenance; equal global addresses do not prove equality to an earlier object generation.

Raw RTTI identifies base0076E684 asCRdData and singleton0076E820 asCSingleton<CRdData>. Available ordinary destructor004021A0 calls006B2670 and separately tears down its member; available deleting slots00402200/00403420 additionally free conditionally on flagbit0. The application invokes **session cleanup**, not these full/deleting entries. No singleton-cache clear or manager-allocation free follows from00BD768C=0.

## Selected session cleanup order — VERIFIED, normal-return scope

`006B2670` conserves its incoming Dshutdown:

1. If D+0C table exists, iterate signed index0..<D+4 and call006B2830(index,1,0), reusing force-zero unsigned descriptor2C/pending marking.
2. Call006B2A40. Accepted pending scan, if its flag is set, locks D+388 and selects descriptors with payload18 nonnull and word2C zero. Callback atD+10 receives(event1,descriptor,index); after normal return, payload18 is reacquired for conditional deallocation and the descriptor is cleared. Callback return is not a success test or worker acknowledgment.
3. Recheck table pointer, call deallocation at006B26D5 and zero D+0C at006B26E0. The scan has released its mutex before this separate table teardown.
4. **Only then** load ECX=01481130 at006B26E7 and call006B3D70 at006B26EC.
5. Call archive-member cleanup onD+18 at006B26F4/F7. This is a separately typed member/call boundary, not full archive grammar or successful external release.
6. If D+388 is nonzero, callCloseHandle at006B2712, then set that field0 at006B271B without testing the API result.
7. Return; caller clears its application root.

Null table skips1–3, not4–6. This order disproves a proposed **local stop/join-before-table-release** interpretation; it does not disprove quiescence established by an unexamined upstream caller.

## Stop request and logical queue clear — VERIFIED, not join

Complete `006B3D70(W=01481130)`:

- byteW+19=0 at006B3D7A, before mutex acquisition;
- `00712B90(W,1)` requestsWaitForSingleObject on **W+14**, with timeoutINFINITE;
- same embedded Q=W+24 reaches full-range erase; normal valid-range path makes logical end=begin while retaining begin/capacity allocation;
- DWORDW+20=0 at006B3D96;
- ReleaseMutex(W+14), status ignored;
- another mutex interval in006B3C80(W,1) stores DWORDW+1C=1;
- return.

The complete selected stop contains no thread-handle wait, termination call or completion receipt. It does not resetW+18, clear direct pendingW+3C, destroy queue backing storage or close worker handles. Clearing logical count/end is not completing or canceling an already dispatched request.

**Exact result domain:**00712B90 consumes its argument's lowbyte, selects timeout0/FFFFFFFF, readsW+14, and returns **AL=1 only if waitEAX==0**, otherwiseAL=0; raw ABI RET4. Stop and period setters discardAL and continue.00712BC0 callsReleaseMutex onW+14 with no local result branch. Manager wait/release andCloseHandle results are also not consumed as shutdown-success gates.

**Coordination limit:** accepted worker loop testsW+19 at006B4DEC/F0/F2, but an already-entered queued iteration calls the resource body outside the worker mutex, then later decrementsW+20 under it. The primary verified that selected test/late-decrement ordering without reconstructing the queue again. Successful acquisition of the mutex is therefore not a thread join or proof that all in-flight work is complete. A particular interleaving or failure remains HYPOTHESIS until supplied occurrence evidence.

## Separate storage destructor/finalizer — VERIFIED availability, UNKNOWN dispatch

Initializer registers0076DE60 at0076D4ED/F2; registration result is not tested. The finalizer body supplies actualW=01481130 to006B4F80. Registration/body is positive evidence, not actual exit dispatch or order relative to application cleanup.

Ordinary typed destruction route:

`006B4F80(W) →006B4FE0(Q=W+24) →006B50B0(Q)`.

On nonnull begin, storage teardown invokes scalar destruction and sends the saved begin/count to006E5300→accepted deallocation endpoint0074E82B; then zeros Q+0C/+10/+14 (begin/end/capacity). An opaque container-support endpoint follows backing-storage release; it cannot be assumed to establish a prior join. Finally006B4F80 calls00409E80(W), whose entire raw body is **MOV[ECX],0076F638;RET**, namedCThfunc byRTTI. There is no explicit stop/join/handle-close edge in either complete ordinary destructor or this base body.

Available worker deleting slot006B4F50 conditionally frees a supplied receiver; the registered finalizer calls ordinary006B4F80, not that slot. Do not infer heap deletion of the in-place static object. Decompiled CancellationToken/CAtlWinModule FID matches are false naming authorities for this typed receiver chain.

## First missing edge and global return

Before the **first manager force-release/callback/payload teardown**, establish an invoked admission-closure and same-worker/thread/request/table-generation last-use/completion edge. The explicitW+19 clear is later, after table free/zero, and no same-thread completion identity is consumed in this route.

Reopen stronger coordination only for a **concrete typed upstream pre-release shutdown/admission/last-use edge** or a correlated trace naming thread handle, requests and table generations. Do not widen into generic CThfunc/CRT/global/free scans to fill the absent edge. Move to another strong independent Phase5 family after this substantial local-order result.

GOG/Xbox parity, actual finalizer dispatch, opaque transitive effects, exception paths and live cadence remain UNKNOWN. Phase4 stays closed; no Phase6.

## Verification and primary references

- Returned checker reviewed and replayed unchanged except **output destination** into new `scratch/phase5_resource_worker_retirement_main_replay_20261004.json`; original return preserved.34 aligned windows/699 instructions/1994 bytes,86 anchors/52 relative calls,4 typed tables/3 IAT names: PE, suppliedASM and independentobjdump agree.21 canonical/control before/after hashes unchanged during replay.
- Independent main `scripts/inspect_phase5_worker_shutdown_main.py` → `scratch/phase5_worker_shutdown_main_20261004.json`:204 instructions/618 bytes and12 receiver/order/width anchors; source/loop/lock/storage joins personally interpreted. Overlapping/reused counts are not additive novelty.
- SteamASM application290–294 (zero-register236); cleanup769004–769060; stop771103–771124; period771004–771018; loop772852–772896; destructor/storage772996–773159; base10861–10862; mutexABI893079–893114; registration993353–993357/finalizer994365–994370.
- Exact check spans/bytes/type/IAT/source hashes: retained original `scratch/phase5_resource_worker_retirement_primary_20261004.json`, main replay and main check. Scratch branch report is a derived evidence receipt, not a substitute for primary bytes.

No game/input/production/runtime modifications or live API execution tests.
