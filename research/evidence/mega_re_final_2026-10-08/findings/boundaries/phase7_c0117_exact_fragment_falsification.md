# C0117 / BND-085 — exact-fragment Phase 7 falsification

**Date:** 2026-10-07. **Parent:** validated checkpoint231. **Disposition:** `VERIFIED` conditional-static placement survives, with explicit progress and identity qualifications. Not an unconditional movie-update guarantee. The reliance reset is reused, not repeated; Phase6 remains `DO_NOT_RENEW`.

## Question and competing explanations

Test the selected frame's returning post-dispatch convergence onto the same-root movie method, not historical whole-engine convergence. Candidate falsifiers: export truncation or a shared fragment changes the selected CFG; an order/context arm bypasses `00401C75`; a pre-media local gate returns before `004016FC`; the tested AL means completion; or a purported unconditional continuation is prevented by repetition/allocation. No external assets, runtime modification, dispatcher-internal recensus, or Phase8 work.

Primary examination uses the Steam and GOG PE independently, fresh anchored GNU objdump decoding, original ASM bytes, and baseline-v7 accepted instruction/fragment rows. Baseline reachability/attribution is not semantic ownership. Reused CRdMovie identity and cleanup interpretation remain at their inherited C0113/H0254 scopes; this batch freshly checks placement, operands, helper test and return mechanics, not the entire helper implementation.

## Exact admitted fragments — VERIFIED structural facts

| Selected entry | Steam fragments `[start,end)` | GOG fragments `[start,end)` |
|---|---|---|
| Frame | `00401A70–00401D01` | independently checked, same interval |
| Media-containing phase | `00401440–004017B7`; separate `004011A0–004011A1` RET | independently checked, same intervals |
| Selector | `0041C270–0041C27C` | `0041C290–0041C29C` |
| Movie method | `00700500–007005C9` | `00700520–007005E9` |
| Repeat getter | `00409DC0–00409DC6` | `00409D90–00409D96` |
| Immediately relevant allocator | `00404390–004043A7`; `004043B0–004043DF` | `00404370–00404387`; `00404390–004043BF` |

All selected entries have zero shared bytes/conflicts in baseline-v7, with reconstructed fragments matching export attribution. Fresh decoding corroborates every selected instruction against PE and ASM, without treating gaps/padding as callable code. The phase tail `004017B2 -> 004011A0` reaches a RET, not a different semantic function. No recovered-fragment contradiction was found.

## Falsifier results

1. **Order-arm bypass — DISPROVEN at selected finite local scope.** `00401B66` selects the dispatcher/companion order; the nonzero arm's `00401B83` jumps to `00401BA4`, while the zero arm falls there after its dispatcher call. Steam dispatcher/companion are `006C5FF0/00701350`; GOG are `006C5AF0/007012B0`. The nonzero arm gives the companion 0.0; the zero arm uses the selected time global. Both returning arms reach the repeat decision.
2. **Context-arm bypass — DISPROVEN at selected finite local scope.** After loop exit, the second selector call at `00401C1D` selects ESI=`00BD86B8` or `00BD9648`, joins at `00401C30`, and pushes ESI at `00401C74` before `00401C75 -> 00401440`. The getter reads bit31 of `008A6074` on each call. **VERIFIED:** two distinct reads. **UNKNOWN:** equal values across loop iterations or across the post-loop read; no latched human mode.
3. **Pre-media local-gate bypass — DISPROVEN at selected finite local scope.** Both edges of each phase branch before the movie call (`00401450`, `0040162C`, `0040166B`, `00401689`, `004016D7`, `004016E5`) rejoin before `004016FC`, assuming intervening calls return normally. This includes the manager `+6711` gate, optional state clearing, `0146E8C0` optional work, and lazy-root paths. Both finite entry-to-return graphs have no path avoiding the respective phase/movie call. This is a safety/order fact about finite normal-return paths, not total postdomination over infinite executions or interprocedural exception handling.
4. **Movie completion-result test — DISPROVEN.** Movie entry checks receiver `+8`; helper-null returns AL=0. On the nonnull-helper arm, even a returning same-this cleanup at Steam `007005B8` / GOG `007005D8` passes the AL=1 tail at method `+BD`. `00401701/00401703` then gate only the subsequent helper block. AL is not proof of completion, fresh data, retained helper, or actual playback.
5. **Pre-movie numeric mode exit — DISPROVEN in the selected phase.** The `008A7220 == 46..49` branches occur at `00401791..004017A0`, after the movie call, and skip a later local helper. They do not bypass the media-containing phase or movie method.

## Material qualifications / concrete progress counterexamples

### Repeat loop — VERIFIED edge; eventual exit UNKNOWN

`00401BB6` branches back to `00401B55` while AL is nonzero. Its direct getter is Steam `00409DC0`, GOG `00409D90`, reading byte `00886FF8`. The local graph contains a cycle through the dispatcher arms and repeat decision. If that byte remains nonzero at each decision, the graph admits continued repetition without reaching `00401C75`. This is a conditional static witness, **not** an observed infinite run or a proof that arbitrary branch sequences are feasible. No eventual loop-exit invariant was established; the inherited repeat-count/policy UNKNOWN remains.

### Null root / allocation failure — VERIFIED conditional non-continuation

The phase loads ECX from `00BD9E48` at `004016CF`. If nonzero, that receiver reaches the movie call. If zero, `004016DB` invokes **Steam `00404390`, GOG `00404370`**, requesting 0xC bytes. Fresh allocator decoding shows:

- allocation result is saved in ESI and tested;
- zero takes the diagnostic loop at Steam `004043B0..004043BF`, GOG `00404390..0040439F`;
- the back jump has no normal CFG edge to the success path or RET, and no allocation retry occurs in that loop;
- nonzero follows zero-fill and opaque tracking calls, then returns saved ESI.

Thus the wrapper's local `EAX==0` arm at `004016E5` is **not evidence of a normally returning zero allocation**. In this decoded allocator, failure does not return through that arm. If failure occurs with a null movie root, the selected phase does not normally advance to the movie call; a diagnostic callee's termination/nonlocal transfer would also fall outside normal-return convergence. Actual allocation failure occurrence, diagnostics' runtime behavior, and whole-process progress remain UNKNOWN. This rejects an unconditional-update extrapolation, not the inherited returning-traversal proposition. No null-this crash is asserted from an infeasible normal-return zero-allocation path.

### Same root is not same instance — VERIFIED operand; temporal identity UNKNOWN

Existing-root ECX is loaded immediately at `004016CF`. The lazy path uses the returned pointer, writes the vtable locally at `004016E7`, copies it into ECX and publishes it to `00BD9E48` at `004016F3`. `004016F9/004016FB` push 1 and zero before the build-specific movie call. This proves the selected root operand/publication connection, **not** equality with an earlier traversal's object, root stability during callbacks, helper allocation retention, a fresh sample, or measured frequency.

## Reproducibility and evidence scope

`scripts/inspect_phase7_c0117_20261007.py` emits `audit/phase7_c0117_2026-10-07/PRIMARY_CHECKS_ACCEPTED.json`: **473 instructions / 1,834 bytes / 50 checks per build**, 946 instructions / 3,668 bytes / 100 checks total. Calls are modeled as *possible normal-return fallthrough*, never as proven returning. Raw PE opcode displacement decoding supplies graph transfers; fresh objdump targets and accepted substrate transfers must agree. Finite bypass search and conditional cycle witnesses are separate tests. A deterministic repeat is checked during handoff validation.

Primary ASM anchors: both builds frame `:726–881`, phase `:311–529`, discontiguous RET `:118`; Steam allocator `:4001–4028`, GOG allocator `:3987–4014`; Steam repeat getter `:10790–10791`, GOG `:10750–10751`. Per-instruction ASM line references and binary hashes are in the receipt; selector/movie anchors are likewise build-qualified there. Steam PE SHA256 `7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029`; GOG `c954c2e3b205d444b0fc3649adf4bd8462a7dfe73d89599e24a7e17a129415c2`. Baseline-v7 manifest SHA256 `8b70425655d16b476b8a0d5adf0977c4a6f773041be25e2852876397459c8912`.

Two checker-development attempts failed on a schema vocabulary mismatch (`FALLTHROUGH` versus baseline `ORDINARY`); the schema was corrected, failure recorded, and checks rerun. The earlier successful 44-check receipt is superseded by the 50-check accepted receipt, not silently overwritten. No subagents were launched for this primary continuation and no new reviewer/API failure is an evidence boundary. The historical pre-release review is governance only and supplies no semantic proof.

## Disposition and stop boundary

**VERIFIED:** C0117/BND-085 survive as the selected finite, normally returning frame/phase order and root-operand connection. **VERIFIED:** the selected allocator contains a failure diagnostic loop, and the frame contains the repeat back-edge. **UNKNOWN:** eventual progress, real cadence, complete mode/world/lifetime policy, same-instance/generation continuity, successful playback and nonlocal/exceptional behavior of opaque callees. Do not expand into unrelated callee internals or asset enrichment to manufacture completeness. No inherited evidence state is promoted, no historical report is erased, and no Phase6 sufficiency is renewed. The finite falsification has an evidence-based qualified-survival disposition; stop at the validated checkpoint232 handoff.
