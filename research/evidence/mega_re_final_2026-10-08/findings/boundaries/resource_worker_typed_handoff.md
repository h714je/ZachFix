# Steam resource worker request-to-record handoff

**2026-10-03; Phase4 selected mechanism reconstruction.** Primary Steam PE/assembly checks, with bounded independent Opus queue and direct-command branches personally reconciled by the main researcher. No GOG homology, runtime success/cadence, full resource grammar or safe retirement claim. Sourcecard addresses were locators, not prior worker proofs; accepted Phase1–3 architecture is reused at its exact scope.

## Typed acquisition and invoked OS boundary — VERIFIED

Static initializer `0076D4E0` passes actual `01481130` to `006B4D70`. A raw `.rdata` pointer at `0076E458` names that initializer. The constructor calls the CThfunc base initializer, writes `008268F4` into the same receiver, and constructs the embedded collection at `+0x24`. Raw COL `0087BF94` / TD `009BBF80` names `CLoadThread`, with `CThfunc` at PMD0. The observed two-pointer interface contains available deleting slot0 `006B4F50` and worker slot4 `006B4DE0`. Root `01481130` lies in the non-file-backed mapped `.data` region; it is an in-place static object, not the CRdData singleton or a stack-local CFlkUtil.

Accepted CRdData initializer `006B2570` actually passes `01481130` to `006B3BA0` at `006B25F3/F8`. If receiver byte `+0x18` is zero, start sets `+0x19=1`, `+0x18=1`, and direct pending `+0x3C=0`, then calls `00712A20` with stack size `0x40000` and name `LoadThread`. This is an **invoked** start API route, unlike the physics helper's unresolved incoming route. The generic helper passes this exact receiver as the `CreateThread` argument, with entry `00712D30`; that adapter recovers the same pointer, invokes its virtual `+4=006B4DE0`, then calls `ExitThread`. Existing generic helper mechanics receive no duplicate discovery credit.

**UNKNOWN:** actual API success/OS execution, startup temporal race freedom, whole lifetime or quiescent cleanup. The start path sets its own started byte before the API and does not establish a successful return. `006B2570` calls the worker start before its later archive metadata initialization and manager mutex creation; static instruction order does not establish live concurrent behavior. Static finalizer registration/available deleting interfaces do not prove safe shutdown.

## Concrete producer routing — VERIFIED, exact mode distinction

Selected public wrapper `0040B330` passes numeric mode0 into acquired CRdData's `006B1310`; sourcecard alternative `0040B350` does the same. Neither is a concrete nonzero-mode queue producer. `006B1310` returns1 even when its negative-ID guard bypasses admission, and this return is not a successful resource registration receipt.

For nonnegative input:

- nonzero mode selects `01481130` / `006B3B40` queue admission;
- mode0 and input at least30000 select separate `006B3C00` directly;
- mode0 below30000 selects archive segment/record resolution; selected descriptor status byte `+0x2F==2` increments its word reference count `+0x2C` under the acquired CRdData mutex, otherwise releases that mutex and calls the same-root direct command. Segment enumeration applies that decision to its returned indices.

The initializer/start proof and this producer share a concrete CLoadThread root. Actual upstream nonzero-mode admission beyond the selected public wrappers remains **UNKNOWN**; a forwarded mode argument is not a caller proving it is nonzero. No all-producer census is claimed.

## Conditional queue retention / mechanism — VERIFIED; roles bounded

With W=the same typed worker and Q=W+0x24, `006B3B40` zeroes a four-byte local, stores identifier **low16** at local0 and control **low8** at local2, then appends through `006B3E10`. Effective representation is `(identifier&0xFFFF) | ((control&0xFF)<<16)`; byte3 is zero. The callee copies the DWORD value into contiguous storage—**not** the address of the producer local. Both normal in-capacity and successful growth paths reach DWORD copy `006B4A16/1B`; the placement helper returns the supplied destination. Growth relocates old four-byte elements and updates begin/end/capacity-end. Shared historical `PointerVector_*` helper names do not make these request values pointers.

**VERIFIED minimum fields:** Q+0x0C/0x10/0x14, equivalently W+0x30/0x34/0x38, are begin/logical-end/capacity-end. Size is `(end-begin) SAR2`. **STRONG_INFERENCE:** vector-family container. **UNKNOWN:** exact source C++ template/type, full allocator/debug machinery or failure/capacity guarantees. No linked request-node ownership is inferred.

`006B4E70` uses index0, saves its four-byte value **before** `006B5040` erases/compacts the remaining values left and reduces logical end by4, releases the worker mutex, then copies the saved DWORD to its caller's output. Raw ABI is ECX=W, one output-pointer stack argument, `RET4`, EAX=output pointer; the decompiler's missing receiver/extraout argument is not a signature authority. The worker reads signed16 identifier and signed8 control from that same local, then calls `006B4F20`. Selected append-at-end/front-erasure preserves remaining element order; all-admission/global runtime FIFO is not established.

Worker DWORD `+0x20` increments after append returns; pop changes collection logical end but not that counter. Only after `006B4F20` returns does a **separate** worker-mutex interval decrement `+0x20`; the loop's nonzero test is outside acquisition. **STRONG_INFERENCE:** selected queued-plus-in-flight counter, not exact current vector length or a count of successful resource completions. This worker field is unrelated to descriptor `+0x20`/C0044. The queued load occurs after pop unlock and before the later count decrement; direct consumption has a different lock extent.

**VERIFIED empty-path limit:** counter0 skips queued pop but still reaches direct service. Pop itself has no clean empty flag/default: index0 on an empty valid collection reaches an invalid-parameter diagnostic; **if it returns**, indexing and pop dereference continue. No unconditional crash/throw or safe empty behavior is proved. Mutex helpers request `WaitForSingleObject(W+0x14,INFINITE)` and `ReleaseMutex`, not critical-section operations; selected users ignore helper results. Normal successful-container/source ordering is not a concurrency, allocation or safe-lifetime guarantee.

## Separate direct-command mechanism — VERIFIED

`006B3C00` invokes start if necessary, acquires the worker mutex through `00712B90(1)`, writes **word** `+0x3E=low16(identifier)` and **byte** `+0x40=control8`, sets pending byte `+0x3C=1`, releases through `00712BC0`, then repeatedly snapshots pending under that mutex and waits via `006B3CB0` until clear. It is caller-blocking, but resource execution is not a caller-local synchronous call to `006B2780`.

Worker `006B4DE0` conditionally consumes a queued item first, then under the worker mutex invokes direct consumer `006B4EE0`. That consumer reads `+0x3C`, sign-extends `+0x3E` and `+0x40`, calls `006B4F20` on the same worker, then **unconditionally clears pending after normal return**, without inspecting a resource result. Direct pending is not the queued outstanding count `+0x20`; the two storage/control routes must not be collapsed.

Pending clear establishes normal return from the dispatched body, **not** successful extraction, callback construction, readiness of a particular typed family, or a validated result. No request token/result slot or multi-producer serialization guarantee is established by the selected source. Exception/non-return behavior and full shutdown remain unresolved.

## Getter-derived CRdData and descriptor consumer — VERIFIED

`006B4F20` accepts the widened index only under the **signed** test index<30000; it has no local lower-bound test. It zero-extends the control byte again, pushes `(-1, control8, index)` in reverse argument order, calls `00405460→004051F0`, moves returned EAX to ECX, then calls `006B2780`. The worker receiver is not used as CRdData `this` by analogy. Raw `006B2780` pops three stack words (`RET0xC`), although its decompilation does not expose the unused third word.

Within the reused archive/descriptor route, `006B2780` forwards index and control8 to `006B33F0` alongside archive metadata from `006AFC90`. On a nonzero constructor-supplied callback result, `006B33F0` writes that exact control byte into local descriptor `+0x2E`, writes status2 at `+0x2F`, copies twelve dwords / `0x30` bytes to acquired manager `[+0x0C]+index*0x30`, and increments the **word** reference count at `+0x2C`. The accepted callback/type dispatch and selected XMD/XPC portfolio remain reused conditional consumers; this batch does not identify a concrete admitted resource family from a numeric request alone.

`006B4F20` does not test registration return. Group iteration in `006B2780` likewise does not propagate individual `006B33F0` failures. The per-record helper can return `0xFFFFFFFF` for distinct cases, including an already-present reference increment or failed extraction/callback; normal return is not a uniform success status. Control8 identity at descriptor `+0x2E` is proved; its full engine-level tag/group meaning is **UNKNOWN**.

The boundary preserves only low16 of the incoming numeric identifier. Worker widening is signed16, while the dispatch threshold is signed32. Thus full-width identifier preservation and safe handling of every nonnegative public input are not established. This is a static input-domain qualification, not a tested exploit, runtime failure or implemented fix.

## Evidence and personal verification

- `BIN-STEAM`; `EXPORT-STEAM-DP_full_asm`; `EXPORT-STEAM-DP_decompiled_c`.
- `inputs/decompiler/steam/DP_decompiled.c:427927-427989` producer; `429665-429708` start/direct; `430949-431064` constructor/worker/direct/dispatch; `569817-569829` static initializer.
- `inputs/decompiler/steam/DP_full.asm:772819-772896` same-receiver constructor/worker; `770928-770998` start/direct; `768926-768998` invoked initializer/start; `892937-892983` CreateThread argument; `893271-893279` entry; `772947-772990` direct/dispatch; `769096-769145` registration argument ABI; `770582-770605` control/status/copy/count commit; `993349-993357` static constructor.
- `scripts/inspect_phase4_resource_worker.py` → `scratch/phase4_resource_worker_primary.json`: actual389 instructions/1248bytes,41 anchors/15calls,raw RTTI2bases/2slots andinitializer pointer. Counts include exact scoped helper reuse, not wholly new functions or runtime proof.
- `scripts/inspect_phase4_resource_handoff_joins.py` → `scratch/phase4_resource_handoff_joins.json`: actual191 instructions/545bytes,24 anchors/9calls personally verifies direct-slot writes, signed widening/guard, getter receiver, three-word ABI, callback and control/status/copy/count commit. These scopes overlap other receipts; counts are not additive.
- Accepted scopes: `findings/formats/resource_manager_root.md`; `resource_manager_population.md`; `resource_descriptor_fields.md`; `dpserial_archive_loader.md`; `typed_resource_dispatch.md`; `findings/boundaries/xpc_resource_positive_portfolio.md`; `cmap_state_resource_phase_placement.md`; `physics_context_vector_activation.md` generic thread ABI only.

### Independent queue branch reproduced and personally verified

`scripts/inspect_phase4_resource_queue_branch.py` was read and locally rerun unchanged: **1089 instructions/2804bytes over46 continuous windows,49 calls,68 byte anchors,2 raw imports and9 aligned independent objdump windows passed**. It selects successful growth windows/continuations explicitly; it does not silently treat the exception/minmax span as a complete ordinary function. Deterministic initial receipt `scratch/phase4_resource_queue_branch.json` refuses a changed replay. Main `scripts/inspect_phase4_resource_queue_main_verification.py` → `scratch/phase4_resource_queue_main_verification.json` checked **268 instructions/723bytes,30 anchors/9calls**, personally reconciling pack width, same embedded receiver, DWORD copy leaf, begin/end/capacity commits, saved pre-erase value, compaction/output and late-count/MOVSX joins. No whole-STL reconstruction or new raw exported-entry invention. Queue/helper primary sites: `inputs/decompiler/steam/DP_full.asm:770893-770922`; `771195-771239`; `772902-772941`; `773079-773118`; `772444-772467`; `823843-823875`.

### Independent direct branch reproduced and accepted at its exact scope

`scripts/inspect_phase4_resource_direct_branch.py` was read and locally rerun: **511 defined instructions/1494bytes,28 relative-call/jump targets,3 raw import names and23 instruction assertions passed**. Main191/545 joins personally checks the critical receiver/ABI/control/status/copy/count sites rather than accepting the agent's semantic report blindly. Initial returned JSON is preserved unchanged at `scratch/phase4_resource_direct_branch_return.json`; ordinary checker replay can refresh current diagnostic ledger/hash projections without rewriting that return. Its scope includes selected reused getter/segment/endpoint seams, not a new parser census; overlapping counts are not additive.

**UNKNOWN first temporal edge:** one shared direct publication `006B3C32` is not tied by a per-request token or proved external serialization to consumption `006B4F05`, clear `006B4F0D`, and waiter snapshot `006B3C54` under concurrent producers. The mutex covers individual writes/reads and the whole direct consumer, but the submitting caller releases it between publication and polling. Replacement by a second producer is only a **HYPOTHESIS**, not an observed race. A concrete single-producer/external-serialization contract or correlated multi-producer runtime trace is the needed discriminator; no whole-caller or runtime investigation was performed here.

## First missing edges / deferred work

Actual nonzero-mode caller selection, full input/control domain, concrete identifier-to-family/dependency mapping, multi-producer pending-slot discipline, collection capacity/failure guarantees, result propagation, worker stop/join coordination and manager/payload lifetime are intentionally unresolved. Earlier C0044 `+0x20`, typed-family grammars, CLevel/tag retirement, raw world selector, physics incoming start, save chronology and runtime B0001–B0010 bounds remain unchanged. This selected chain is not full RESOURCE_LOADING or Phase4 completion.
