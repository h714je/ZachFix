# Phase7 A01 — MEDIA_COMPLETION_ATTRIBUTION

2026-10-07; parent validated checkpoint236. Main-read canonical primary only; no runtime observation.

## Inherited proposition and dependency

Named CRdMovie root receives setup/update/session-stop requests; WndProc drains media events and latches event1; update presence-return is not completion.

Canonical identities: C0104, C0113, C0117, C0162. Exact historical rows and source hashes: `audit/phase7_architecture_2026-10-07/A01_INHERITANCE.json`. Both overlays/prior corrections retain precedence.

## Stronger theorem tested

Active byte/update completion certify successful playback of the currently requested session; EC_COMPLETE is the exclusive completion producer and same root joins all generations.

## Checked primary scope

STEAM_PC;GOG_PC, independently movie-update-rooted completion predicates 00736560/00736270 and declared caller-rooted setup/cleanup pairs

`audit/phase7_architecture_2026-10-07/A01_PRIMARY.json`, `audit/phase7_architecture_2026-10-07/A01B_PRIMARY.json`. These contain exact instruction/bytes, original-ASM lines, fragment ownership and edges. Counts are validation/acquisition scope, not discoveries or complete functions/populations.

## Disposition — QUALIFIED_SURVIVAL / VERIFIED

- **VERIFIED selected scope:** Completion predicate has an independent ordered duration/position comparison route (Steam00736615..657/GOG00736325..367), not exclusively EC_COMPLETE. It consumes division-result integer converted to x87 against position; TEST AH41/JP suppresses greater and unordered, permits equal/less under normal x87 completion.

- **VERIFIED selected scope:** The two position/duration virtual-call HRESULTs are not tested before their outputs are consumed. Position starts at zero; duration local is not initialized in the acquired predicate before the request. This is not a validated successful query protocol.

- **VERIFIED selected scope:** Source setup clears readiness014B09E8 and sets it only on its normal success arm, while Movie setup requests fallback once and proceeds to helper setup/active-byte publication without a second readiness gate.

- **VERIFIED selected scope:** Source initialization invokes graph cleanup, whose first edge invokes the event drain; the acquired init/cleanup bodies contain no direct completion-latch reset. Explicit graph-stop resets014B0A74 before its stop request. Do not turn this selected direct-body negative into whole-process producer/reset completeness.


## Counterexamples and limits

- **STATIC_PATH_COUNTERMODEL**: Normal-return original and fallback opens both leave readiness0; helper path returns normally → Movie active-byte publication remains reachable; active does not imply successful source/playback.

- **STATIC_PATH_COUNTERMODEL**: Event path does not latch1, ordered returned duration <= returned position → Completion predicate latches1 through non-event route.

- **CONDITIONAL_MODEL_NOT_OBSERVED_RUNTIME**: Query failure leaves position0 and a readable duration local whose postconversion value is0 → Ordered equality can latch completion despite unchecked failure; actual COM failure behavior/runtime occurrence UNKNOWN.


**DISPROVEN stronger components (not wholesale retirement of canonical claims):** Active implies successful playback; EC_COMPLETE is the sole completion-producing route in the predicate; Predicate return establishes successful query execution.


**Independently preserved:** Movie same-root request/update/stop operand chains; C0117 helper-presence return, not completion; C0104 event1 latch route.


## Dependency propagation

C0104 event route remains reusable only nonexclusively; C0113 setup/stop is a conditional request protocol; C0117 presence-return and C0162 request-not-completion qualifications survive unchanged. No renderer storage reopening: helper/descriptor/COM/backing generations remain distinct under checkpoint236.


Blast radius: Local media completion/success inference; no new invalidation of the accepted renderer dependency contract or unrelated architecture.


## UNKNOWN / deliberately unpromoted

- **UNKNOWN:** Actual playback/query/stop success; Session/generation attribution and transitive/all-producer latch reset completeness; Sample-to-helper backing/COM generation and last use; Runtime cadence.

- **Not promoted:** Observed stale-session failure; Complete global producer census; COM interface successful execution or ownership; Same movie root means same helper/backing generation.

- Reviewer scientific credit: none.


No Phase6 sufficiency renewal, Phase8, asset enrichment or runtime modification. This portfolio record is not a checkpoint/acceptance receipt.
