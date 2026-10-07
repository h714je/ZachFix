# Targeted Phase 1/6 revalidation — dispatcher, CRT roots, changed candidates

**2026-10-06.** Separately authorized one-batch revalidation of immutable `baseline_v7`. Phase 7 remains **PAUSED**; Phase 8 **NOT STARTED / forbidden**. This is a current reliance overlay, not a rewrite of historical closeouts or the ten sealed semantic ledgers. Primary receipts and machine ledgers: `structural/revalidation_2026-10-06/`.

## Decision at the targeted scope

- **VERIFIED:** the repaired GOG dispatcher continuation supports the old generic multi-pass mechanism, explicit virtual-slot set, and previously corrected phase-marker order. The false non-return attribute remains **DISPROVEN**. Historical spans extending through `006C6F17` must not be called a single dispatcher body.
- **VERIFIED:** both CRT arrays have 117 slots, 116 distinct nonzero targets, and 70 targets absent from original function exports. Recovered entries include actual static game-object/member construction, cleanup registration, and save-staging initialization, not merely CRT implementation. Address-range-based exclusion is **DISPROVEN** by these concrete operations.
- **VERIFIED:** changed ranking witnesses include a three-byte no-op and internal forwarding stubs; membership cardinality is not evidence of a new architectural service. Conversely, omitted GOG `00507C70` has a real incoming call and selected active-manager/result-field interfaces. It is not dismissed as score noise.
- **STRONG_INFERENCE:** corrected Phase 6 residual **selection materially changes**. **UNKNOWN / NOT RECONFIRMED:** architectural sufficiency/convergence for the corrected population. Historical exported-PC/Steam-first acceptance remains a historical qualified decision; its exact local positive chains are not blanket retracted. This batch neither proves a new mandatory subsystem nor establishes that new static seams are harmless/subsumed.

## Evidence and ownership contract

`PRIMARY_ACQUISITION.json` retains 118 exact singleton-reachable entries per build: the dispatcher, returning helper, and all 116 initializers. Combined: **6,770 instruction records / 31,678 PE-checked bytes**. `DEPENDENCY_ACQUISITION.json` adds the explicit dependency contract and raw CDemo root/COL/type words. Instruction sets are checked against PE and `baseline_v7` exact membership; this does not recreate original Ghidra FunctionBodyRanges or prove complete semantic ownership. Fresh dispatcher objdump views start at the real prologues, not guessed interior offsets.

The initial acquisition's display-only `indirect_calls=0` field used the wrong enum (`INDIRECT_CALL` instead of `INDIRECT_CALL_UNRESOLVED`). Raw retained instruction rows contained all nine calls. The correction in `DEPENDENCY_ACQUISITION.json` is authoritative: **90 direct / 37 distinct direct targets + nine unresolved indirect calls = 99 physical invocation sites per dispatcher**. Preserve the initial receipt as a diagnosed artifact; do not reuse its zero field. Other tooling corrections/exclusions are in `FAILURES_AND_CORRECTIONS.md`.

## 1. Reconstructed GOG dispatcher

### Returning seam — VERIFIED

GOG `006C5AD0` saves the incoming receiver, forwards its stack word with literal `ECX=01481358` to `006C7280` at `006C5AE0`, and ends with `RET 4` at `006C5AE8`. Thus the export `noreturn` attribute cannot justify discarding the caller fall-through. This is a returning normal static path, not a guarantee that opaque downstream work returns on every runtime invocation.

At `006C5DAF`, the dispatcher calls that helper; `006C5DB4` stores EAX in its local, then clears manager `+3CAC` and `+1CA8`. Subsequent eight-bin loops select object pointers under multiple bit/value tests and append selected results to manager storage beginning `+1CAC`. Physical reachability and gates are verified; universal admission, selected concrete classes and live cohort continuity remain **UNKNOWN**.

Exact reconstructed membership is `[006C5AF0,006C6B6D)`: **897 instructions / 4,221 bytes**, ending at `006C6B6A RET 4`. Original export attribution is 708 bytes. `006C6B70` is a separate recognized companion entry, not a second name for the dispatcher or a license to join the old min/max hull through `006C6F17`. C0005 survives at the exact multi-pass role; C0021 survives with this extent qualification.

### Virtual phases and ordering — VERIFIED at numeric static scope

Nine indirect sites resolve syntactically through a loaded object vptr:

| GOG call site | Loaded slot | Visible phase/context |
| --- | --- | --- |
| `006C62F4` | `+0C` | after manager phase 2 |
| `006C63C0` | `+30` | selected phase-3 retirement-marker arm |
| `006C656A` | `+10` | phase 4, explicit gates/stack arguments |
| `006C6637` | `+14` | phase 6, explicit gate |
| `006C670E` | `+18` | phase 10, pushed numeric 4 |
| `006C68BE` | `+1C` | phase 8, pushed numeric 6 |
| `006C6B0A/3E/60` | `+48` | phase-13 alternatives, manager `+1CA4` value |

No own dispatcher virtual `+50` call is present. This revalidates C0089's exact dispatcher negative; it does **not** prove absence from all recurring paths, computed destinations, or the separate companion. BND-001/BND-003/BND-066 preserve their positive relationships at that scope. Actor-family grouping remains **STRONG_INFERENCE**, not a recovered universal concrete-target taxonomy.

The full numeric marker-write sequence, including the initial reset, is `0,1,2,3,4,5,6,10,14,7,8,11,12,9,13`. The important recovered seam is `006C6770 phase14 -> 006C67B4 CALL 0040B720 -> 006C67C2 phase7 -> 006C67D4 CALL 0040B750 -> 006C67F4 phase8`. This reinforces the earlier C0122/K0017 correction: do not resurrect “both synchronous halves execute under phase 7.” Steam has the same marker/slot sequence at separately acquired build-local entries. This is static ordering, not scheduling, SDK effects, synchronization or cadence.

### Directly affected four-record boundary — VERIFIED mechanics; type limits retained

Recovered GOG `0040B720` and exported `0040B750` each use `ESI=00BDA010`, stride `2C`, exclusive end `00BDA0C0`: four records. They forward their stack word and call `0040B820` / `0040B910` respectively on each record. Missing initializer `0076A7B0` supplies the same base/stride/count four to the constructor iterator with `0040BA10` and cleanup `0040BB70`, then registers `0076D790`. This connects the formerly missing callable wrapper to an independently initialized static array.

C0149–C0151's **Steam-only** typed worker/vector claims are not automatically extended to GOG. The GOG wrapper/initializer lineage is verified; concrete worker class, record internals, actual thread activation, SDK effect and safe retirement were not re-proved. These available static questions remain live, not “runtime-only.”

## 2. Recovered CRT initializer roots

### Startup invocation protocol — VERIFIED conditional static mechanism

Steam `[0076E320,0076E4F4)` and GOG `[0076E30C,0076E4E0)` are confirmed by raw table words and startup operands, not only sidecar names. GOG `00755AF8` calls `00755A60` after an earlier initializer-result gate. That loop loads each slot, skips zero, indirectly calls nonzero pointers at `00755A70`, advances four bytes and stops at the supplied end. Steam's independent corresponding call site is `00755D60`.

The population is **116 targets/build**, of which **46 are recognized exports and 70 are recovered entries**. All 140 recovered entries receive build-local function-review rows. First-level targets and registrations are recorded; unexamined callback internals, runtime registration success, actual process cleanup and whole-root discovery remain **UNKNOWN**. A CRT callback source makes an entry reviewable; it does not itself prove GAME ownership.

### CDemo/member construction — VERIFIED; existing Steam interpretation survives

Steam `0076AB90` and **GOG `0076A8A0`**, not the same numerical address, initialize two `3C` records at `008A6810` through independently checked iterator helpers `0074E836` / `0074E546`. Constructor/cleanup pointers are Steam `004274B0/004274D0`, GOG `004274D0/004274F0`. Constructors install raw RTTI-backed `CMustachesAdmin` tables and set words `+2C/+30/+34=-1`. Both callbacks clear `008A6074` and supply `0076DB20` / `0076D830` to the `_atexit` entry.

PE `008A6070` names **CDemo** through separate build-local vptr/COL/type-descriptor chains. Hence `008A6074=CDemo+4`, and array base is `CDemo+7A0`; observed member span gives lower bound `818`, not an allocation size. Available finalizers restore the CDemo table and call the destructor iterator for the same two-member array. Registration/instructions are verified; registration success, actual finalizer execution, root destruction/free and last use are **UNKNOWN**.

C0137's selected Steam root/member/init interpretation survives. This batch newly provides a narrowly matched GOG static-init/root/finalizer seam; it does not extend C0138/C0139's activation/tasks/exit policy or establish full GOG parity. Numeric-address equality is specifically unsafe: **GOG `0076AB90` is a 12-byte cleanup-registration-only callback**, not Steam's CDemo initializer. The homology overlay records this explicitly.

### Save-staging construction thunk — VERIFIED transport/default mechanics

Steam `0076AD10` sets `ECX=00BE5EF0` and **jumps** to `00469050`; GOG `0076AA20` independently sets the same static receiver and jumps to `00469080`. Their reconstructed address sets are disjoint: ten-byte startup thunks plus 117-byte target regions. Calling the target a contiguous initializer body, or the E9 a CALL, would reintroduce repaired geometry/edge defects.

The target preserves that receiver, invokes helpers on members `+40/+74/+134`, and initializes 27 stride-`45CC0` groups: counter `1A`, body before signed `JNS`, with helper requests for `32` records of `40` bytes and `100` records of `24` bytes. Original reviewed bounds are recorded in primary instructions. Selected `00448DC0` / `00448E10` copies four separately loaded global words twice, writes supplied word0, float zero, numeric zero/-1 fields, and `WORD +44=1000`.

This verifies a startup-fed construction/default source for the known Steam staging address and a narrowly corresponding GOG initializer mechanism. It does **not** prove a full save image, disk-to-live chronology, opaque constructor effects or buffer ownership. `00BE5EF0` is in mapped non-file-backed `.data`; no initial PE word, vptr or implicit zero-value claim is made. The separate 30-record/stride-`120` default initializer (`0076AD20` / `0076AA30`) is recorded as numeric initialization only; its concrete policy/type/consumers remain **UNKNOWN**.

Other recovered initializers include exact receiver/call/registration sequences for the known CMap, CMenu and layout storage. They are included in the first-level root register, not reopened as complete Phase 2–5 subsystems. Large numeric/static-table initializers are not semantically decoded merely because their bytes were acquired.

## 3. Architecture-sensitive candidate triage

### Shared membership is not a new service — VERIFIED witnesses

- Steam `004029B0`: 125-byte receiver/node detach/rebind path, conditional `+14C` replacement, not a newly discovered manager. Existing selected spatial boundary remains intact. No new ownership/quiescence claim. Corrected bounded vtable bonus **5**, versus historical-weight comparator **955**; primary vtable-xref table count **0**. Many slot memberships are not many independent type/constructor proofs.
- Steam `0040D420`: exactly `C2 08 00` (`RET 8`). Bonus **5**, versus comparator **700**. Its coarse review admission is appropriate but supplies no hidden service semantics.
- Steam `00489FD0` / GOG `0048A0B0`: five-byte E9 forwarding to `005C60A0` / `005C63E0`, not an IAT/Win32 import transfer. Bonus **5**, versus comparator **300**. “PLATFORM_WIN32 because thunk” is contradicted; full destination ownership/type remains **UNKNOWN** here.

These compare **same corrected facts under different weight channels**, not before/after semantic discoveries. Final scores already use bounded membership; do not subtract comparator bonuses from current scores or reinterpret old rank values as current ranks.

### Genuine newly reviewable game-facing seam — VERIFIED local interface; larger role UNKNOWN

GOG `00507C70`, NEW_ENTRY rank 1 in the final baseline, has **1,335 instructions / 5,307 reachable bytes, 252 direct calls, two unresolved indirect calls and one indexed indirect jump**. A real incoming `E8` at `005D4FDF` belongs to reconstructed source `005D4EB0`, not a guessed vtable member. Preceding `005D4FCC..DD` loads the current `008A9BA4` word and active-manager root `00BD7670`, calls returning `006C5AD0`, and forwards EAX as ECX into the new entry. The source arm is conditional, not universally executed.

At entry, that same manager/lookup interface is used again; raw query-result words are written through separately reacquired results at `+694/+698`. This is an actual current-result boundary, not a literal-only or no-op candidate. Original entry receiver, concrete returned class, alias/generation continuity, meaning/success of opaque requests and full numeric branch policy remain **UNKNOWN**. Subsystem contacts/score do not type the routine as a universal vehicle/world mode controller. This batch acquires its exact instruction set but deliberately does not recursively analyze 252 callees.

## 4. Old conclusions: surviving versus qualified/contradicted/unresolved

Machine decisions are in `DISPOSITION_LEDGER.csv`, keyed to original claims/boundaries and blast-radius trace IDs. They distinguish:

1. **SURVIVES:** C0005 multi-pass dispatcher; C0089 exact own-slot negative; earlier C0122/K0017 correction; selected Steam CDemo/member-init interpretation; existing scoped positive portfolios and 88 stronger guards.
2. **REQUIRES_QUALIFICATION:** C0021's old broad continuation locator; dispatch negatives beyond exact acquired entries; GOG extensions from Steam worker/activation/save conclusions; old game-facing import/range labels versus independent ownership; graph ranks versus semantic importance.
3. **CONTRADICTED:** false `noreturn`; treating 20,252 exports as complete callable universe; exclusion of recovered startup game construction as runtime-only; forwarding form as proof of Win32 ownership; resurrected both-halves-under-7 claim.
4. **NEWLY_UNRESOLVED:** complete role/receiver/policy of genuine new GOG bridge; residual omitted CRT root/default-table consumers; corrected-population architectural sufficiency. These are available finite static frontiers, not falsely declared blocked by missing runtime.

No unaffected Phase 2–5 work was repeated. Local acquisition/verification of directly changed dependencies does not regrade every inherited claim or all 55 blast-radius assertions. Untested negative/hull/confidence seams keep their original pending discriminators.

## 5. Phase 6 selection and sufficiency

**VERIFIED structural population:** final baseline contains 21,871 corrected entry records, 1,619 NEW_ENTRY review rows (supported entries plus provisional candidates), 15,072 UNKNOWN, 4,427 PARTIAL and 753 CONTEXT_ONLY. Those are structural/review counts, not understood functions or a complete executable universe. Final F03 review admission adds 4,109 coarse uncertain PARTIAL contexts without canonical ownership promotion. Relevance-weight, population-eligibility and frozen Phase 6 drift channels stay separate.

**STRONG_INFERENCE — materially changed selection:** dispatcher-related newly callable code, CRT startup entries and internal game-facing forwarding contexts require candidate review independent of historical exported-only rankings. This batch supplies an actual new incoming/result bridge and game-initialization counterexamples, not just abstract population drift. Leaf/no-op/shared membership witnesses show why score magnitude alone is a poor selection reason.

**UNKNOWN / NOT RECONFIRMED — corrected-population sufficiency:** original Phase 6 closeout remains historically accepted at its declared exported-PC/Steam-first minimum. Exact positive chains and limits survive. However, its residual convergence/exclusion premise cannot certify the corrected entry/partial population. This batch does not satisfy a whole corrected-residual necessity review and does not assert a concrete new mandatory subsystem blocker solely from size/call count. Conversely, new roots/bridge cannot be declared subsumed or harmless without discriminating evidence. Broad discovery sufficiency remains unsupported; do not automatically restore the old `STRONG_INFERENCE` milestone as current corrected-model sufficiency.

Smallest next available static discriminators, if separately authorized: connect `005D4EB0`'s conditional selection and `00507C70`'s original/lookup receivers to a raw typed object and one producer/consumer policy arm; qualify the omitted CRT root/default-table consumers by exact users; triage remaining supported NEW_ENTRY/coarse PARTIAL architecture seams before any fresh sufficiency decision. No need to redo unaffected local portfolios, no Phase 7/8 authorization, no premature final synthesis.
