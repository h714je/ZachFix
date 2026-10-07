# Phase 2 Closeout Rubric — Object Model, Vtables, Globals, and Factories

**Purpose:** decide whether Phase 2 can close as an evidence-driven architectural milestone. This is not a percentage target and must not be passed by row counts alone.

## Closeout principle

Phase 2 may close with many unknown functions, fields, vtable slots, and leaf classes. It may not close while a plausible missing manager, central interface, ownership boundary, or factory family blocks the Phase 3 lifecycle/root map.

A closeout review must mark every requirement `MET`, `BOUNDED`, or `NOT_MET`, with primary evidence references and carried-forward queue IDs. `BOUNDED` records a limited method/result, not automatic satisfaction. `NOT_MET` and blocking `BOUNDED` items prevent transition. Leaf or later-phase gaps may be carried forward only with an edge-specific non-blocking justification; that exception cannot waive the positive minimum portfolio below. One blanket 'non-blocking' paragraph is insufficient.

## Required evidence portfolio

### 1. Major roots and global provenance

For each currently established cross-cutting Phase 2 root—at minimum the application/object-dispatch boundary, CRdHandleUtil/active-object path, CRdData/resource path, and CMessage/native-task path—record:

- initialization or acquisition evidence;
- manager/global identity at the proven scope;
- meaningful readers/writers or callers/callees;
- update/use placement where observable;
- teardown/reset or an explicit unresolved limit;
- Steam/GOG status or an explicit build limitation.

The target is a connected root portfolio, not a fixed number of globals. `reports/PHASE2_EXIT_READINESS.md` must include a **Root gap matrix** covering every current SUBSYSTEM_LEDGER seed (including audio, save, player/input/camera, world, event, and renderer) plus any newly discovered major family. For each, identify the organizing-root evidence or the precise first missing object/global/interface edge, its search boundary, queue ID, Phase 3 consequence, and entry strategy. Limiting the review to already established roots cannot satisfy this requirement. This does not require every Phase 3 subsystem root to be solved early; it requires evidence-based visibility into the gaps.

### 2. Representative lifecycle chains

Recover or explicitly bound representative chains across distinct architectural roles:

1. an active object: allocation/construction -> registration/owner -> update or callback -> retirement/teardown;
2. a resource object: archive/lookup -> typed construction -> manager/owner -> first consumer -> release/unload limit;
3. a task/UI/event or actor-family object: creation -> interface/callback/virtual use -> owner/lifetime limit.

Each chain must state the first missing edge rather than bridging it with inference. Reusing the same family for all three does not satisfy this requirement.

**Non-waivable positive minimum:** one active-object chain must positively connect construction, registration/owner, a class-discriminated update/callback use, and retirement/teardown; one distinct resource chain must positively connect archive/lookup, typed construction, manager/owner, and first consumer (release may be explicitly bounded); and a third task/UI/event-or-actor family must positively connect creation, an interface/callback use, and at least one evidenced owner/lifetime edge. Use VERIFIED mechanics or explicitly scoped STRONG_INFERENCE supported by multiple independent primary facts. Missing live cadence and indirect cleanup can remain UNKNOWN. Three reports that merely say 'no owner found' cannot satisfy the portfolio.

### 3. Central interface and vtable closure

For selected high-centrality interfaces—not every table—record enough evidence to place them architecturally:

- table identity and observed slot extent;
- construction/destruction or an explicit absence;
- concrete target(s) for material lifecycle/update/event slots;
- at least one direct or class-discriminated indirect call context;
- ownership/lifecycle consequences and unresolved slots.

The active-object dispatcher and at least one non-dispatcher family must meet this standard. A vtable symbol inventory alone does not.

### 4. Factory and resource-type boundaries

Establish the selected factory/registration families that organize objects and resources:

- selector/factory input -> allocation/constructor -> registration/owner relationship;
- resource callback or equivalent type selection -> typed object/descriptor -> consumer/owner relationship;
- negative limits for branches with no factory/consumer discriminator.

This must explain the meaningful factory seams already found and identify which major families remain unobserved. It does not require every asset extension to have a full parser.

### 5. Ownership and object split discipline

Review high-impact embedded/helper/outer-object splits and manager candidates. Confirm that no class merge, owner, or independent active-object role is retained solely from nearby vtables, allocation, or field-offset similarity. Preserve unresolved alternatives in claims/contradictions.

### 6. Cross-build identity and recoverability

For foundational Phase 2 roots, each Steam/GOG correspondence must be `VERIFIED`, `STRONG_INFERENCE`, or explicitly unavailable with evidence. Known export-boundary problems must be carried as limits. Xbox may remain comparative, but the non-equivalence must be stated.

### 7. Durable-state and frontier closure

Before phase transition:

- `validate_ledgers.py` and `validate_research_state.py` pass;
- `STATUS.md`, `RESEARCH_LOOP_STATE.md`, selected checkpoint, journal, and current queue row agree;
- completed direct censuses are marked bounded/resolved with precise reopen triggers;
- remaining Phase 2 questions are ranked as Phase 3-blocking, non-blocking carry-forward, or runtime-blocked;
- no stale completion sentinel or ambiguous Exact next action remains.

### 8. Transition package

A proposed transition must create, before changing phase:

- `reports/PHASE2_EXIT_READINESS.md` using this rubric, with **Root gap matrix**, **Lifecycle portfolios**, **Red-team review**, and **Carry-forward decision** sections; each conclusion names build-qualified evidence, affected claim/interface/edge, and queue IDs;
- a dedicated Phase 2 closeout checkpoint;
- an updated object-model map/report that separates verified mechanisms from inference;
- a Phase 3 entry target that is a lifecycle/root question, not an arbitrary leftover class or field;
- a red-team review of any foundational claim newly relied on by the Phase 3 entry target, including recovered annotations, unresolved legacy reference debt, missing exported-entry homology limits, and high-impact object splits;
- a dedicated human-authorized transition review; state validators check package structure, not whether the evidence portfolio is semantically sufficient.

## Explicit non-gates

The following are descriptive metrics, not Phase 2 exit gates:

- percentage of functions classified or canonically named;
- count of vtables, class rows, globals, boundaries, or homology groups;
- number of asset extensions listed;
- token budget spent;
- number of autonomous iterations.

## Current status

`CLOSEOUT_ACCEPTED_AT_SELECTED_STATIC_ARCHITECTURAL_SCOPE` as of dedicated sequence-9 review (reports/PHASE2_CLOSEOUT_2026-10-02.md). Non-waivable positive portfolios and selected rooted interfaces are evidenced; remaining edges have specific carry-forward/runtime dispositions rather than a blanket waiver. Stop before Phase 3. Requirements above are unchanged; this is not a metric-based census-completion claim or permission to start the next phase.
