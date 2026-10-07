# Phase 7 extended Core-composition falsification — four finite dispositions

## Authority and scope

Parent: validated checkpoint233. New user-authorized finite release: `audit/phase7_core_batch_2026-10-07/EXECUTION_RELEASE.json`; release validation PASS. This does not repeat checkpoint231's reliance reset, reopen C0117/Event, renew Phase6 sufficiency, begin Phase8, or start external asset enrichment.

**VERIFIED source scope:** `PRIMARY_ACCEPTED.json` independently decodes and compares **708 instructions / 2,513 bytes**, with **49 finite checks**: Steam567/1,962 and GOG141/551. Steam has18 distinct contexts in19 exact fragments; GOG has2 separately type-anchored contexts. All708 instructions have `baseline_v7` reachable-singleton structural support. Of these,707 also match original ASM occurrences; Steam0042D3F7's7-byte instruction is PE/reconstructed-only, explicitly not supplied ASM. Structural support is not executed reachability, exact Ghidra FunctionBody membership, current object identity, or complete engine coverage.

Exact primary builds:

- `STEAM_PC`: `inputs/binaries/steam/DP_STEAM.exe`, SHA256 `7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029`; original `inputs/decompiler/steam/DP_full.asm`.
- `GOG_PC`: `inputs/binaries/gog/DP_GOG.exe`; exact SHA256 and original ASM occurrences bound in the accepted receipt. No address-equality inference: GOG getter00408920/initializer0070D0F0 and table0076F2AC/0082762C are independently established, rather than borrowing Steam00408960/0070D150/0076F2BC/008276A4.
- Baseline manifest SHA256 `8b70425655d16b476b8a0d5adf0977c4a6f773041be25e2852876397459c8912`. Both post-repair overlays and checkpoint230 dispositions/aliases remain in force. All88 guards and ten canonical semantic ledgers are conserved; companion rows add scope qualifications, not confidence upgrades.

The read-only finite interpreter in `scripts/phase7_core_bounded_machine.py` executes only selected PE-decoded integer instruction rows and fails closed on unsupported instructions, unknown memory, unmodeled calls, out-of-source transfers or step-cap exhaustion. Its explicit synthetic memory/call models are **not runtime probes**. Instruction consequences are VERIFIED at the stated finite input/model; actual runtime realization remains UNKNOWN. Constructor FPU/SEH and CRT support are not emulated or silently supplied.

## Disposition register

| Attack | Selected surviving proposition | Disposition | Architectural consequence |
|---|---|---|---|
| P7CB01 | C0286/C0287 full available Core initializer premise; left unreviewed by closed A2 | SURVIVES_TYPE_ANCHORED_TWO_BUILD_STRUCTURAL_ATTACK | Retain available physical +4 base/inline-construction relationship; do not transfer helper semantics or active ownership across builds. |
| P7CB02 | C0268/C0269/C0270 nominal publication and intended three cohorts | SURVIVES_WITH_PER_ACQUISITION_AND_CALLBACK_ABI_LIMITS | Retain three intended address cohorts; no single coherent currently initialized Core-owned pool follows. |
| P7CB03 | C0277 lookup subclaim/C0278 conditional publication-consumer coupling | SURVIVES_CONDITIONAL_COMPOSITION_UNCHECKED_BANKS_CONFIRMED | Retain nominal80/32 direct receiver handoffs only with persisted/same-generation cell assumptions; bare accessors provide no bank/type bounds. |
| P7CB04 | C0272/C0275 setter/request subclaims/C0276 conditional configuration | SURVIVES_WITH_ALIAS_STALE_COMPANION_AND_WIDTH_COUNTEREXAMPLES | Retain sequential relative-header/pair/current-member interfaces; retire any immutable-snapshot, unconditional whole-pair replacement or positive-count-validity reliance. |

No selected **already qualified canonical claim** is DISPROVEN. Counterexamples attack stronger propositions explicitly listed below; they are not fabricated contradictions of the narrow historical claims. No semantic credit from reviewers.

## P7CB01 — initializer/base/receiver attack

**VERIFIED:** Steam0070D150's exact106/436 context and GOG0070D0F0's independent106/436 context each save incomingECX in `[EBP-10]`, submit that saved receiver+4 to an opaque base boundary before installing their own literal Core table, submit13 distinct subsequent interiors, make two final same-receiver opaque calls, clear two four-DWORD regions after those calls, restore saved receiver toEAX, and reach the sole plainRET. Within these selected contexts there is no local conditional branch/jump bypass. The proof is only finite normal-call fallthrough; opaque nonreturn, failure, traps, exception/unwind paths and execution are UNKNOWN.

Interior starts in both independently inspected bodies:44,424,528,7B4,A08,A48,A5C,A6C,A7C,A8C,12BC,12C0,12C4. These are13 **call operand offsets**, not13 proved member classes, measured regions or owned objects. Direct explicit-write extent remains a lower bound, notsizeof.

**VERIFIED direct type data:** Steam008276A4 and GOG0082762C separately nameCEvCore, with CEvCore offset0 andCEvManage mdisp4. Their separate wrapper tables0076F2BC/0076F2AC declareCSingleton<CEvCore> and reuse each build's direct Core type descriptor. Each own getter passes its saved nonzero operand asECX to its actual initializer, then installs its own wrapper table at that same saved operand. Source witnesses include Steam004089AC/004089B1 and GOG0040896C/00408971.

**STRONG_INFERENCE:** selected constructor bodies are structural homologues: every instruction shape/other operand matches after erasing only actual directCALL operands, the installedtable operand and handler-pointer operand. **Not helper equivalence:** Steam base boundary is0040D380 at0070D17C; GOG's is0046F8F0 at0070D11C. No callee body or policy has been opened or transferred. A constructor-shape match alone is insufficient to equate these opaque effects.

Strongest attempted falsifiers: an adjusted receiver/table mismatch, additional finite normal return bypass, or structurally divergent interior layout. None found at this exact scope. C0286 VERIFIED machine mechanics and C0287 STRONG_INFERENCE available inline composition retain their confidence. The cache/current class, initialization success, exception cleanup, actual invocation, ownership and lifetime remain UNKNOWN. Closed A2 typed-cache disposition was a premise, not rerun.

## P7CB02 — cohort/publication/ABI attack

**VERIFIED:** Steam00446890's exact599-byte export-attributed set excludes `[0044693D,00446940)` rather than decoding its602-byte hull. Three normal loops each call00408960, then store computed source address bits relative to that **particular** return. Independently matched request/publication geometry:

| Cohort | Source/stride/count | Getter-result bank | Exact request | Selected callback/table |
|---|---|---|---|---|
| Process | BDBEE0 /54 /32 | +10EC..1168 | 0076AC30..AC54 | 00449420 /007721D4 CEvProcessEx |
| Thread | BDC960 /C4 /80 | +116C..12A8 | 0076AC60..AC87 | 0042D3B0 prefix+tail /007721DC CEvThreadEx |
| Data | BE06A0 /10 /384 | +AEC..10E8 | 0076AC90..ACB7 | 00449290 /007721AC CEvDataEx |

**VERIFIED finite test:** all496 normal iterations compute the exact nominal bank/source pairs. No source-element dereference occurs in these loop paths. An explicit model with alternating normal getter results10000000/10010000 splits bank destinations while preserving all source/stride/count mechanics. This is a countermodel to a guaranteed coherent recipient, **not** an observation that the real cached getter changes during a loop. Current root/generation mutation is UNKNOWN; C0268 already preserves per-acquisition identity.

**VERIFIED callbacks:** each selected callback is locally call/branch-free, leavesESI unchanged, and returns incomingECX inEAX. The ThreadEx available86-byte path comprises71 export bytes plus15 separately contracted tail bytes; baseline_v7 structurally reconstructs that tail. First7 tail bytes at0042D3F7 lack an original ASM occurrence;0042D3FE/0042D405 have original lines51029/51030. Historical export geometry and missing Ghidra membership are not retroactively changed.

**VERIFIED iterator prefix:**0074E836's selected77 bytes loadsESI from `[EBP+8]`, placesESI inECX, calls `[EBP+14]`, then advances the **post-callback ESI**, notEAX, by `[EBP+C]`. The fifth request operand's expected `[EBP+18]` is not read in this extent. Explicit selected callback preservation strengthens the local pointer-advancement compatibility, but the unseen SEH frame-support and out-of-extent0074E883 effects still prevent unconditional CRT ABI/runtime initialization proof.

**STRONG_INFERENCE:** intended32/80/384 typed cohorts remain supported by separately verified request tuples, callback type graphs and publication geometry. No actual static initialization, current element contents, ownership, scheduler or shared format is established. No atexit target/body/lifetime chain opened; baseline-supported orphan identities remain separate from historical recognized exports.

## P7CB03 — bank/loop/direct-receiver attack

**VERIFIED:**007106F0 and00710710 each zero-extend the low16 argument bits, read oneDWORD from savedR+10EC/116C+4*index andRET4. Own27-byte paths have no bound/type check, global read, call or conditional branch.

**VERIFIED local counterexamples to stronger bank-safety/type reliance:**

- Process helper007106F0 with index32 readsR+116C, the first thread-bank cell.
- Thread helper00710710 with index80 readsR+12AC, the descriptor field immediately following its nominal80-cell bank.
- Index10000 hex becomes low16 zero, reading the first cell rather than an independently validated high index.

No actual bad invocation, pointer validity, object type or memory error is asserted. These are definite address calculations at explicit finite inputs; the nominal caller loops use only0..79/0..31.

**VERIFIED bounded coupling:**0070D8C0's183-byte context iterates80 then32 indices, passes the immediate lookup return toECX for opaque00722220 then00729390 respectively, and reloads savedR for later inline requests. A synthetic persistent bank initialized with the stated source addresses reproduces all112 exact handoffs even when each opaque fixed recipe clobbersEAX/ECX/EDX. This demonstrates the conditional local link, not that actual cells currently have those values or targets actually reset typed elements.

Attempted loop-skip bypass **DISPROVEN:** the syntactic edges to0070D8CD/0070D901 follow `XOR EAX,EAX; JE`;ZF is set immediately and theJE is taken. The bypassJMP is infeasible in this local normal instruction sequence. Baseline conservative CFG branches are not path-feasibility proofs. No baseline graph rewrite or coverage promotion follows.

**STRONG_INFERENCE:** publication-layout/current-cell/direct-receiver composition survives **if** cell values persist in the same receiver generation. Fixed-recipe semantics remain reused at their prior qualified scope; their bodies were not reopened. Scheduling, current typed objects, bounds for other callers and ownership remain UNKNOWN.

## P7CB04 — descriptor alias, stale companion and width attack

**VERIFIED:**0070D3E0 independently reloads header/currentP for+12AC/+12B0/+12B4, obtains the first pair fromP+4/+8 and calls0070D470, then reloadsP+C/+10 and calls0070D4A0. Setters always replaceR+3C/+34 but replaceR+40/+38 only for signed-positive second operands. OwnRET8 and local signedJLE paths corroborate the actual ABI. Nonpositive inputs preserve old companions, not clear them.

**VERIFIED bounded alias counterexample:** take synthetic valid receiverR, descriptorP=R+2C and initial words `{H,200,306,300,C7}`. H's+14/+10 words are40/80. The first setter's0070D47D stores200 atR+3C, which isP+10. The subsequent0070D442 reload therefore observes200 rather than originalC7; the second setter storesR+38=200. This needs no opaque callee effect or concurrency. Disjoint/stable descriptor memory instead produces the nominal `{R3C=200,R40=306,R34=300,R38=C7}`. Thus the actual sequence is not an immutable five-word snapshot.

The alias is **not** established for actual00446890's stack tuple or actual cached root: their relationship/current identity remains UNKNOWN. The already-qualified C0272 reload formulation and C0276 persistence conditions survive; the stronger generic snapshot hypothesis is DISPROVEN at this finite input.

**VERIFIED stale-companion cases:** second arguments0,FFFFFFFF and80000000 replace each pointer word but preserve prior companion20. Large positive40000000 and7FFFFFFF are admitted and stored without capacity validation. These mechanisms are not unconditional whole-pair/atomic configuration replacement.

**VERIFIED current requests:**0070D980 checks current00BE1EB8; nonzero returns without either request. Zero requests `(currentR3C,0,currentR40)` at0070D9A0, then after normal return independently reloadsR34/R38 and requests `(currentR34,0,(R38<<2) mod2^32)` at0070D9BB. Target0074E760 remains an opaque symbol-named interface here, not successful buffer clearing. Counts306/C7 yield requested306/31C only under explicit persistence/generation/guard assumptions.

**VERIFIED width counterexample:** signed-positive40000000, accepted by the setter, shifts to zero. Positive input alone therefore does not prove a valid/nonzero second request extent. No actual count, storage validity, target effect or runtime defect inferred. Header/schema/relative-offset semantics, coherent generations, request success and safety remain UNKNOWN.

## Independent reviewers and operational failures

Two scientific reviewers were launched concurrently through exact-model `claude --model claude-sonnet-5`, directly in the canonical workspace, restricted toRead/Grep/Glob, no worktree/commits/writes/nested agents. Questions: independent initializer/other-build attack, and independent bank/configuration counterexamples. Both returned proxy refusal `ERR_PROXY_TUNNEL` before any model usage/tool review; zero completed reviewers, zero findings adopted, zero scientific credit. No retry, alternate model, credential search or provider-cooldown evasion. Prompts/results retained under the audit directory.

One acquisition checker mismatch was corrected: independently decoded `INDIRECT_CALL` must compare with baseline `INDIRECT_CALL_UNRESOLVED`, not be relabeled resolved. `ACQUISITION_1.stderr` retains the failure. It supplied no counterexample. Later acquisition and49-check replay passed. Initial coordinator tool-schema errors supplied no evidence and are not scientific dispositions.

## Consolidation point and remaining Phase 7 frontier

Four distinct architecture-bearing propositions have dispositions; no structural contradiction requires reconciliation. Available constructor, intended static cohorts, nominal bank-to-recipe and relative configuration interfaces remain coherent **at their retained qualified scopes**. This is a natural consolidation, not engine convergence or unknown-set exhaustion. Another step in these internals would require selecting a new opaque-helper/current-generation/lifecycle question; no automatic descent is justified or authorized by this handoff.

Remaining prospective, **unselected** Phase7 frontier: still-unexecuted pointer-tracker/root/callback authority question C0240; still-unexecuted fixed-word/Timer consumption authority challenge C0248/C0249/C0252/C0253 at a new finite discriminator; factory-successor/source claims only with fresh independent contracts and no collided-output credit. Closed prior root-service, C0117 and Event portfolios remain closed. This batch's own stronger UNKNOWNs (opaque support, actual current cells/identity, header semantics, scheduling/ownership/safety) are retained, not expanded into research or generic absence theorems.

One consolidated final validation is required. Its PASS concerns exact replay, companion scopes, protected sources/overlays/controls, append-only state and all88 conservation—not semantic completeness or Phase6 renewal. Checkpoint234 is the single final STOP/handoff; no next execution frontier is selected.
