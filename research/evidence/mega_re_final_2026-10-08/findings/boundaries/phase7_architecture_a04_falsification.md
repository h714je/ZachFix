# Phase7 A04 — DEFERRED_RESOURCE_CALLBACK_RETIREMENT

2026-10-07; parent validated checkpoint236. Main-read canonical primary only; no runtime observation.

## Inherited proposition and dependency

World tag requests zero counters and mark pending work; frame-tail scanner conditionally invokes event1 callback and frees/clears selected record storage. World pointer clear and mark requests do not establish resource/actor/packet last-use closure.

Canonical identities: C0114, C0119, C0120, C0180, C0181, C0182. Exact historical rows and source hashes: `audit/phase7_architecture_2026-10-07/A04_INHERITANCE.json`. Both overlays/prior corrections retain precedence.

## Stronger theorem tested

Pending scan and event1 callback form generation-coherent successful destruction, serialize publication globally and revalidate eligibility after callbacks; AL1 proves completed destruction.

## Checked primary scope

Paired independently frame/caller-rooted006B29E0/006B2830/006B2A40/006B3AA0 at same numerical VAs; exact operands/guards/calls checked in each build, not identity assumed from numbers

`audit/phase7_architecture_2026-10-07/A04_PRIMARY.json`. These contain exact instruction/bytes, original-ASM lines, fragment ownership and edges. Counts are validation/acquisition scope, not discoveries or complete functions/populations.

## Disposition — QUALIFIED_SURVIVAL / VERIFIED

- **VERIFIED selected scope:** Pending14 is cleared before the scanner waits on the mutex; scan's loop rechecks current count4, while each release leaf computes and saves descriptor D=[M+C]+index*30 once.

- **VERIFIED selected scope:** Event1 callback receives D and index. After it returns, the leaf reloads payload18 through the saved D, not a new lookup through current M+C. It does not recheck counter2C, record identity, index validity or backing generation before its free request and30-byte zeroing.

- **VERIFIED selected scope:** Callback return is discarded; payload cleanup request return is untested; AL1 follows normal-return zeroing. This is a local executed-path marker, not aggregate callback/destruction/safe-retirement success.

- **VERIFIED selected scope:** Paired tag helper scans current table/count without a surrounding lock of its own and calls the separately locked counter helper for each match. Per-counter locking does not prove coherent whole-tag population, frame scheduling or global publication serialization.


## Counterexamples and limits

- **CONDITIONAL_MODEL_NOT_OBSERVED_RUNTIME**: Indirect event1 callback changes D counter2C from0 to1 and returns normally → Leaf proceeds to free/clear without an eligibility recheck. Actual installed callback doing this is UNKNOWN.

- **CONDITIONAL_MODEL_NOT_OBSERVED_RUNTIME**: Callback replaces table backing or record instance while M receiver/index remain numerically unchanged → Saved D, current M+C and current record generation cannot be equated; actual alias/mutation occurrence UNKNOWN.

- **STATIC_PATH_COUNTERMODEL**: Callback or payload cleanup operation returns a non-success result normally → No result-based branch prevents AL1; leaf return is not proof of success.


**DISPROVEN stronger components (not wholesale retirement of canonical claims):** Release leaf rechecks counter/generation after callback; AL1 certifies callback/free success; Whole tag scan is one locally locked transaction.


**Independently preserved:** Unsigned counter and signed-byte tag mechanics; Force/pending versus deferred callback distinction; Callback/free/clear instruction order; World/actor/resource/packet last-use join remains UNKNOWN.


## Dependency propagation

Keep C0120 and C0114 as conditional callback/free/clear protocols; do not join them to ownership, successful unload or world last-use. C0119/C0180-182 retain marks/slot-clear/order facts without a matched resource epoch. Replaceable-bank caution is independently evidenced here and is not transferred from renderer layout.


Blast radius: Local resource callback/publication scheduling qualification; previously acknowledged last-use and generation UNKNOWNs retained, no new cross-phase foundational contradiction.


## UNKNOWN / deliberately unpromoted

- **UNKNOWN:** Installed callback reentrancy/mutation and complete producer population; Mutex acquisition success/thread scheduling; Descriptor/backing/payload epoch continuity; Actual destruction/free and borrowed actor/packet last use; World-selector occurrence against frame-tail scan.

- **Not promoted:** Observed use-after-free, race or resurrected record; Ownership from a payload free request; Callback completeness; Same numerical paired address as standalone homology proof; Whole manager population closure.

- Reviewer scientific credit: none.


No Phase6 sufficiency renewal, Phase8, asset enrichment or runtime modification. This portfolio record is not a checkpoint/acceptance receipt.
