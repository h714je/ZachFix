# Camera candidate source geometry residual — sequence 145

**STEAM_PC only; no Camera semantic promotion.** Prior sequence-140/141 selected interval `[00522D50,0052441E)` was reported as 5,836 defined instruction bytes against export `body_bytes=5125`. Sequence 142 stopped on degraded source/control quality after an excluded neighboring ASM exposure. This report preserves that history and narrows only the independent geometry question.

Fresh scout: `scratch/phase6_seq0143_camera_geometry.{md,json}`. Personal geometry replay: `scratch/phase6_camera_geometry_personal_seq0145.json`. Only the old selected interval was checked. Excluded `0052444F..00524502` neighboring ASM was neither acquired nor used here.

## VERIFIED counts and limits

- Export `functions.csv` gives start `00522D50`, inclusive end `0052441D`, body cardinality 5,125; `functions.jsonl` independently repeats size 5,125.
- Half-open min/max envelope width is 5,838 bytes, not 5,836.
- Defined ASM within the selected envelope has 1,665 instructions / 5,836 bytes, exact PE agreement; one two-byte unlisted gap is `[00524141,00524143)`.
- Therefore the shortfall versus defined instruction coverage is 711 bytes; versus the envelope it is 713 bytes.
- The supplied export pack has no exact `Function.getBody()` address-set/range artifact for this target. Min/max bounds and decompile line ranges cannot attribute the 711 instruction bytes to member/nonmember fragments.
- The target remains unnamed/ownership UNKNOWN. Sequence 141 made no Camera semantic promotion and sequence 142 made no canonical promotion. No promoted sequence-141/142 claim is invalidated by this target geometry issue. Independently established Camera table/type facts remain at their own earlier scope.

## STRONG_INFERENCE and first missing edge

The mismatch is consistent with address-set cardinality versus min/max-envelope defined-instruction coverage, potentially a discontiguous recognized body and/or orphan fragments. The implementation explanation and exact missing membership are **UNKNOWN**, not repaired or silently normalized.

Required discriminator: authoritative Ghidra `Function.getBody()` ranges or equivalent exact address-membership export for `STEAM_PC/FUN_00522D50`. This is a source-geometry gap, not an established architecture contradiction, semantic exhaustion, or runtime-only block. The raw vtable cluster remains a selection locator; there is no proved membership in the fixed-word or callback seams.

Do not repeat envelope scans or analyze excluded neighboring ASM without a new explicit bounded source contract. Preserve all inherited failures and Phase 5/88 guards; no Phase 7.
