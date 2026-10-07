# Steam-first discovery and scoped Steam↔GOG reuse

**Decision date:** 2026-10-02. **Authority:** the user's mid-shift strategic direction plus the current orchestrator; this is research scheduling/provenance guidance, not executable evidence.

## Decision

Adopt **Steam-first discovery now**, backed by the existing canonical `ledgers/HOMOLOGY_LEDGER.csv` and a conservative derived reuse index. Do not rederive established high-confidence correspondence merely because a new batch starts. Reuse each mapping only within its recorded role, receiver, body/boundary, and evidence scope.

Perform deeper GOG inspection when it can materially change function boundaries, architecture, confidence, build behavior, or an important canonical promotion. A previously documented raw recovery can be reused at its proven anchors; a warning is not an instruction to rediscover the same recovery. A new semantic scope beyond those anchors still needs discriminating GOG evidence.

## Durable tooling

- `python3 scripts/build_homology_reuse_index.py` refreshes **only** `reports/HOMOLOGY_REUSE_INDEX.csv` from the current canonical map, `functions.csv`, and `calls.csv`.
- `python3 scripts/build_homology_reuse_index.py --lookup 005DCA50` or `--lookup H0207` prints declared match scope, confidence, references, and review flags without changing the index.
- `python3 scripts/test_homology_reuse_index.py` runs nine read-only real-corpus assertions, including a known severe export truncation, missing exported entry, and duplicate canonical member.

The index records existing pairs, export sizes, direct-call counts, canonical callee-group neighborhoods, and intersections. It proposes **zero new matches**, transfers zero names/fields, and promotes zero confidence states. Call neighborhoods are diagnostics over the existing canonical groups, not independent proof that every group is correct. A no-flag row is structurally convenient, not automatically semantically verified.

Current projection: **255 groups**, **50** `REUSE_DECLARED_SCOPE_ONLY`, **205** `REVIEW_FLAGGED_SCOPE_BEFORE_FOUNDATIONAL_USE`. Flags overlap: 198 informal-reference scopes; four export-size asymmetries; three GOG and one Steam absent exported entry; four rows per build with a member shared by multiple groups; one unconfirmed canonical match. These are diagnostics, not 205 disproven matches or newly discovered split/merge relationships. Three existing root citations (H0001/H0172/H0173) were normalized to current primary-backed findings without rederiving bodies or promoting confidence.

## Required handling of ambiguity

| Situation | Handling |
|---|---|
| High-confidence declared pair, usable refs, compatible scope | Discover/reason in Steam; cite canonical group and established primary-backed counterpart scope. Do not perform a duplicate paired body census. |
| Informal legacy citation | Resolve the affected citation/scope before foundational reliance. Do not regenerate debt digests, auto-promote, or presume the old pair is wrong. |
| Known orphan/truncated export | Use the already documented raw anchors; preserve supplied export metrics. Deepen raw GOG work only if the new claim depends on unseen continuation. |
| Shared canonical member / potential alias, split, or merge | Retain all alternatives and explicit scope; no automatic one-to-one enforcement. Duplicate membership alone cannot distinguish shared helper, alias, split, merge, or mistaken record. |
| Unconfirmed/missing pair | Report `NO_ESTABLISHED_MAP`; discovery remains Steam-scoped. Obtain independent anchors when cross-build identity matters. Do not equate missing inventory entry with absent executable code. |
| Same-address pair | Same address is not evidence. Retain independent byte/call/string/vtable anchors. |
| Xbox comparison | Remains comparative; this tooling does not create PC↔Xbox mappings. |

## Concrete exceptions established in current evidence

- H0001 generic dispatcher: severe size asymmetry; original legacy citations are now normalized to current paired raw loop/dispatcher findings. They support selected mechanics, not an automatically normalized full CFG equivalence.
- H0241/H0242 CMustache creator/controller: GOG orphan/displaced continuations already have dedicated raw recovery reports. Reuse those scoped anchors rather than repeating direct parent-handle searches.
- H0251 CShop handler: 15,308 Steam recognized bytes versus 587 GOG; raw event/state/retirement anchors support the displayed lifecycle, not every shop operation.
- H0201 GOG registration block and H0227 GOG effect callback: absent exported entries have different documented evidence limits. Do not manufacture function rows.
- H0002/H0007 secondary CCT/vehicle mappings: preserve uncertainty; no same-address shortcut.

## Is the corpus sufficient for a broader structural census?

`VERIFIED` at corpus-availability scope: both PC packs provide full manifests, calls, strings, imports, data xrefs, symbols, disassembly, and binaries. That is sufficient to build **candidate-ranking** features from string/import neighborhoods, constants, normalized instruction shape, data-reader/writer neighborhoods, and verified anchor neighborhoods.

`UNKNOWN`: whether an automated candidate census can safely recover broad coverage at high confidence. The exports contain demonstrable false nonreturn/body-boundary errors even with zero reported decompiler errors. CFG/shape features need boundary/orphan awareness and independent collision tests. Calls and globals are build-qualified; offset similarity and raw-address equality cannot stand in for structural identity.

A future bounded census should output candidates with reciprocal ranking, feature evidence, explicit collisions/split/merge/no-match possibilities, and sample red-team results. It must not rewrite canonical homology until reviewed. **Defer that larger matcher now:** the newly exposed world/level-owner root is a higher-value architectural frontier. The inexpensive existing-map projection and workflow change already reduce repeated paired work without delaying root discovery.

## Per-batch rule

Declare discovery build and reused groups in the source contract. State why any deeper paired inspection is necessary. Preserve uninspected GOG scope rather than forcing equivalence. When a new match is established, update the canonical ledger, regenerate the derived index, and carry raw/export limits forward. No metric or index replaces the Phase 2 closeout rubric.
