# Phase7 A06 — CAMERA_MODE9_NON_GENERIC_SCHEDULING

2026-10-07; parent validated checkpoint236. Main-read canonical primary only; no runtime observation.

## Inherited proposition and dependency

Generic camera dispatcher indexes17 handlers but skips table9; mapping/state overrides are camera control, not sample staging or complete scheduling. Vehicle secondary control is a separate typed boundary.

Canonical identities: C0011, C0124, C0128. Exact historical rows and source hashes: `audit/phase7_architecture_2026-10-07/A06_INHERITANCE.json`. Both overlays/prior corrections retain precedence.

## Stronger theorem tested

Generic skip means the table9 handler cannot execute, generic dispatcher is its exclusive scheduler, or every camera update is selected from current camera154 through that dispatcher.

## Checked primary scope

Paired generic dispatchers005354D0/005355A0; independently PE-read table008A9BC8 slot9 targets00537F10/00537FE0; non-generic callers005577D0/005578A0 with calls00558275/00558345 through physical cell008A9BEC and incoming tail-transfer contexts005502C0/00550390. GOG336 original-ASM-gap instructions remain explicit RAW_ONLY channels

`audit/phase7_architecture_2026-10-07/A06V2_PRIMARY.json`. These contain exact instruction/bytes, original-ASM lines, fragment ownership and edges. Counts are validation/acquisition scope, not discoveries or complete functions/populations.

## Disposition — QUALIFIED_SURVIVAL / VERIFIED

- **VERIFIED selected scope:** Both new caller bodies reach a CALL DWORD PTR[008A9BEC] outside the generic dispatcher, after selected object virtual10/1C loops. Exact PE initializer at that cell is the respective table9 handler; indirect runtime target remains current cell content, not guaranteed immutable initial content.

- **VERIFIED selected scope:** The acquired non-generic callers contain no direct camera154 selection/check around this call and do not invoke the generic signed-index/mode9 filter for that edge. Its route has its own enclosing control dependencies.

- **VERIFIED selected scope:** Mode9 handler's prefix obtains current00BE1EA4, may lazily allocate/write the build-specific camera singleton table, and does not use incoming ECX as its camera receiver. Generic code-pointer ECX and non-generic caller ECX are not ownership/type proofs for the callee.

- **VERIFIED selected scope:** Generic dispatcher also has post-handler 11C:2000 control work; its mode9 skip is a handler-call restriction, not proof that every generic camera operation is skipped in mode9.


## Counterexamples and limits

- **STATIC_ROUTE_COUNTEREXAMPLE**: Non-generic caller reaches00558275/00558345 and physical table9 cell retains its acquired initializer → Table9 handler is called without the generic dispatcher edge; generic-mode9 skip does not establish global exclusion.


**DISPROVEN stronger components (not wholesale retirement of canonical claims):** Generic dispatcher is the only available table9 handler call route; Mode9 skip proves all mode9/camera work is absent.


**Independently preserved:** Generic signed0..16/nonnull/mode9 skip gates; Camera handler table is distinct from actor state and input staging tables; Selected mode9 handler reacquires camera root independently; Vehicle caller receiver/lifetime attribution not transferred from numeric offsets.


## Dependency propagation

C0124 generic skip survives at local scope and C0011 control-table interpretation remains non-scheduling/nonpopulation. Add the actual alternate table-cell edge to future camera/vehicle timing reasoning. C0128 typed vehicle boundary does not prove this caller's live type/phase/ownership. No once-per-frame, current-mode9 or stable-camera-generation theorem.


Blast radius: New scheduler-exclusivity bypass; no change to input staging latency qualifications or accepted camera type/selector mechanics.


## UNKNOWN / deliberately unpromoted

- **UNKNOWN:** Current table-cell target and mutability; Caller live type, object and phase/cadence; Actual current camera mode and allocation generation; All alternate callers/producers and latest-input/frame coherence; Full vehicle handler algorithm.

- **Not promoted:** Observed mode9 update in gameplay; Non-generic caller always executes or owns camera; All handlers/callers censused; Generic and alternate route share same live camera generation.

- Reviewer scientific credit: none.


No Phase6 sufficiency renewal, Phase8, asset enrichment or runtime modification. This portfolio record is not a checkpoint/acceptance receipt.
