# Application post-dispatch media phase placement

**Date:** 2026-10-03. **Frontier:** MEDIA_MODE_LIFECYCLE. **Outcome:** ADVANCE. C0117 / BND-085. Static control flow only; no live cadence.

## Source contract

Steam `00401440`, its complete direct caller set in `calls.csv` (one caller: `00401A70` at `00401C75`), the caller's repeat-loop/context-selection branches, and concrete selector `0041C270`. The established `00BD7670` CRdSceneDraw identity and H0254 movie-method identity are reused at their accepted scopes. Inspect GOG only to validate the newly foundational phase/selector correspondence and build-qualified targets. Exclude movie construction/source setup, general dispatcher internals, and exhausted generic root/offset searches. Success: explicit caller, mode conditions, receiver and order. Stop at an untyped downstream world edge; do not substitute a generic CMap census.

## Positive static placement — VERIFIED

| Stage | Steam | GOG | Condition / receiver |
|---|---|---|---|
| Repeat-loop selector | `00401B55 -> 0041C270` | `00401B55 -> 0041C290` | Returns bit 31 of dword `008A6074`; named mode meaning UNKNOWN. |
| Selector 1 arm | dispatcher `00401B6E -> 006C5FF0`, then `00401B7E -> 00701350` | dispatcher `006C5AF0`, then `007012B0` | Dispatcher ECX = `[00BD7670]`, receives `014AFFE0`; companion ECX = in-place `00BD9648`, receives **0.0**. |
| Selector 0 arm | companion `00401B8A -> 00701350`, then dispatcher `00401B9F -> 006C5FF0` | companion `007012B0`, then dispatcher `006C5AF0` | Both receive `014AFFE0`; same respective receivers. |
| Repeat decision | `00401BAA -> 006C75F0`; `00401BAF -> 00409DC0` | `006C70F0`; `00409D90` | Repeat to `00401B55` while AL is nonzero. Number of iterations and underlying policy UNKNOWN. |
| Post-loop context selection | `00401C1D -> 0041C270` | `00401C1D -> 0041C290` | ESI = `00BD86B8` if bit 31 is 1, otherwise `00BD9648`. This is a **second read**, not a proven latched mode. |
| Manager pre-media use | `00401C42 -> 006D2B40` | `00401C42 -> 006D2710` | ECX = `[00BD7670]`; selected ESI passed as argument. Full callee semantics not promoted. |
| Media-containing phase | `00401C75 -> 00401440` | Same independently checked edge | Selected ESI passed. Both selector arms converge before this call; it is **after** the dispatcher repeat loop. |
| Movie call | `004016FC -> 00700500` | `004016FC -> 00700520` | ECX = `[00BD9E48]` (or just-published same root), arguments 0 and 1. No selected mode arm skips this call within a returning traversal of the phase. |

`00401440` copies 0x40-byte matrices/scalars from the selected context to staging globals and `[00BD7680]` fields. Its `[00BD7670]+0x6711 == 0` branch controls `006D32E0/006D2EB0`, **not** the later movie call. The `0146E8C0` branch also rejoins before the movie call. The phase is therefore not evidenced as a movie-only mode update.

## Return-value and downstream gate discrimination — VERIFIED

The H0254 method tests receiver `+8`. A null helper returns AL=0. The nonnull-helper path returns AL=1 even if completion polling invokes same-this cleanup and clears the helper. `00401701` tests that return and `00401703` skips a render-state helper block when zero. It is **not a completion-result test**, proof of a fresh sample, or proof that the helper remains allocated after the call. Completion remains a separate internal method result.

Later `[008A7220]` values `0x46..0x49` skip only `004017A7 -> 0044AC10/0044AC40`, after the movie call and manager uses at `00401765` and `00401782`. Their human mode labels are UNKNOWN. These numeric conditions cannot be generalized into a title/gameplay/pause gate for the entire phase.

## World and lifetime limits — UNKNOWN

No explicit `013936F0` CMap receiver was found in the selected phase/frame bodies or the assembly-owned first-level direct callees examined for that exact constant (normalized optional leading zero). This is a narrow search limit, **not** absence of a transitive or indirect world connection. CMap/CLevel update/reset placement remains `WORLD_LEVEL_LIFECYCLE_PLACEMENT`; no acquisition/constructor/attach work is reopened. Context blocks `00BD9648/00BD86B8` are not relabeled CMap or assigned an exclusive owner here.

The accepted C0113 same-root stop/reset and C0116 application manager cleanup remain reused evidence, not new discovery. Singleton free invocation, typed movie helper, full loading/title/gameplay/pause/cutscene policy, repeat-loop policy, live frequency and B0007 remain unresolved. Reopen this **selected placement** only for a conflicting caller/control-flow/raw-boundary observation; use a concrete selector writer/class/transition discriminator for further human mode semantics, not another movie wrapper census.

## Primary anchors and reproducibility

- Steam phase `inputs/decompiler/steam/DP_full.asm:311-529`; frame loop/selection `:778-864`; selector `:32660-32663`; movie helper test and return `:868832-868838`, `:868879-868894`. Supporting decompilation `inputs/decompiler/steam/DP_decompiled.c:263-434`, `:558-679`, `:24304-24314`.
- GOG phase/frame branches occupy the corresponding `inputs/decompiler/gog/DP_full.asm:311-529`, `:778-864`; selector `:30925-30928`; movie return offset +0xBD independently byte-checked. Build-specific `calls.csv` establishes the direct caller source set; it is not an indirect-call census.
- `scripts/inspect_phase3_media_placement.py` corroborates **443 exported instructions / 1,758 bytes per build** against file-backed PE bytes, checks the selected call targets, repeat/context branches and helper-return tail. Derived receipt: `reports/PHASE3_MEDIA_PLACEMENT_RAW_CHECKS_2026-10-03.json`. Binary/exports remain unchanged.
- `00401440` has a discontiguous `RET` at `004011A0` reached by its tail jump. The manifest's `body_start=004011A0` is not an earlier entry; no separate function row is invented. GOG selector is **0041C290**, not same-address `0041C270`.

## Architectural consequence

The inherited startup -> eligible idle -> frame -> object spine now has a positive independent media-use continuation after the object repeat loop, with mode-sensitive context/order and explicit local gates. This meets one useful Phase 3 placement obligation at a selected static scope; it does not complete the world context or all-major-root portfolio. Perform breadth/root review next rather than drifting into adjacent movie/menu implementation.
