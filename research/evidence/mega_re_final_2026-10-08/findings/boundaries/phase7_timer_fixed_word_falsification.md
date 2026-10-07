# Phase7 P7X02 — Timer gate versus fixed-word authority

## Disposition / exact inherited claim

**Qualified survival.** C0248 names a cached CTimer state shell; C0249 records two fixed014940F8 participants; C0252 a capture/reload/mask/store wrapper; C0253 unchecked mutex API attempts. Their original mechanics survive. **VERIFIED new** Timer updater/gate and direct-setter interfaces narrow previously unresolved policy edges. **DISPROVEN local extrapolations:** one consumed Timer/Core receiver, read-only all-ones observation, global serialization from the bracket, or one bit freezing every Timer field. **UNKNOWN** common pause/readiness authority, actual execution/concurrency, clock units/cadence, successful synchronization and same-instance continuity.

Provenance: the three Phase6 Timer/fixed-word reports, canonical C0248/C0249/C0252/C0253, CPA020, RV/RS exact-positive qualifications. CPA020 already rejects behavioral attribution to the unconsumed Timer/PhysicsCore receiver; this portfolio does not reinstate it. Prior closed Core/Event/root-service/C0117 science is not reopened.

## Primary acquisition and build-qualified roles

`audit/phase7_campaign_2026-10-07/P7X02_PRIMARY_v2.json`: **506 Steam instructions/1512 bytes;503 GOG/1500**. Steam506 and GOG448 match original ASM. GOG55instructions/161bytes are repaired **RAW_ONLY_ANCHORED_DECODE** continuation, not original export/body evidence. Six Steam updater-relay instructions are EXPORT_ATTRIBUTION_ONLY; three GOG relay instructions UNASSIGNED. Their local values/bytes are verified, not parent ownership/reachability/cadence.

| Role | Steam | GOG |
|---|---|---|
| Timer acquire / initializer |00449480 /00470880|004494B0 /00470980|
| CFunc acquire |0040EDE0|0040EDB0|
| separate Core acquire |0040E470|0040E440|
| opposed CFunc tails |0044E61A /0044E6C0|0044E64A /0044E6F0|
| word atomics / predicate |006EAB30 /006EB0F0|006EAB40 /006EB100|
| mask / acquire / release |00712D50 /D80 /DB0|00712A60 /A90 /AC0|
| Timer gate / updater / veto |00470B10 /00470C20 /004705D0|00470C10 /00470D20 /004706D0|
| independent setter |00452170→00470320|004521A0→00470420|
| CFunc countdown |0044E6E0|0044E710|

**VERIFIED independent type/import evidence:** each PE separately supplies CSingleton<CTimer>/CTimer with CEvFlag+4, CSingleton<CFunc>, and CSingleton<CPhysicsCore> table/COL/name graphs. Each PE import directory separately resolves InterlockedIncrement/Exchange and CreateMutexA/WaitForSingleObject/ReleaseMutex slots. Equal addresses of some slots/data do not establish homology; independently decoded call/field/type structures do.

GOG veto004706D0 has17historical exported bytes but178reconstructed bytes. Its primary continuation after004706DC’s call reaches00470781 RET; no Ghidra noreturn truncation is restored. The existing RV001 returning-helper qualification is reused as a dependency, not a new dispatcher review.

## Timer control / update / alternate setter

**VERIFIED:** the opposed CFunc methods have receiver+11 early-return gates. Their selected tails clear/set Timer+4 mask2, then a separate getter overwrites EAX, copies its result to ECX and calls the fixed-word helper with pending stack0/1. The latter saves ECX but never reads the saved value; it uses fixed014940F8. Syntactic acquired-Core transport is not behavioral Timer/Core authority or PhysicsCore+67CC8 alias.

**VERIFIED:** Timer gate requires flags1/2/4/8 together plus external-state/predicate conditions. Clearing2 defeats this gate; setting2 alone does not pass it. Updater zero-gate result branches before its own accumulator stores. Selected updater getter-result relays exist, but their parent attribution and cadence remain deliberately unpromoted.

**VERIFIED:** accepted update reads014AFFE0, stages multiplication by raw doubles0.5/6.0 and float receiver+1C with intermediate float stores, then adds accumulator+18. It processes threshold60.0 and cascades integer comparisons60/60/24 through fields14/10/C, advancing8. This is not an exact all-input algebraic3×source calculation or a proved clock-unit conversion. **STRONG_INFERENCE:** calendar/time-of-day-compatible accumulation. Source origin, units, clock policy and external subsystem success are UNKNOWN.

**VERIFIED counter-boundary:** separately acquired setter00470320/00470420 writes C/10/14 without this flag gate. Thus “bit2 freeze means no Timer mutation” is DISPROVEN at selected paths. CFunc countdown independently consumes014AFFE0 with60000.0 threshold/65535.0 reset; shared scalar use does not make Timer its provider. The acquired external scalar-store fragments are only stores, not complete producer semantics.

## Fixed word / predicate / synchronization attacks

Main `test_phase7_campaign_timer.py`:30checks PASS from exact rows and explicit synthetic models.

- **VERIFIED:** low-byte zero requests Exchange(word,0); nonzero Increment(word). There is no balanced decrement policy or normalized completion status. Argument100h selects zero-byte Exchange, not a distinct increment class.
- **VERIFIED:** predicate returns AL0 for zero input or equality with014940E4; equality with0149406C can return AL1 without reading the word/bracket. Other inputs use the mask wrapper’s old-value result. **DISPROVEN** equivalence of this answer with word-nonzero/Timer readiness; true-with-word0 and false-with-word1 finite inputs exist. FullEAX booleans are not normalized.
- **VERIFIED:** wrapper capturesA, reloadsB, storesB&mask, then returnsA. MaskFFFFFFFF preserves B’s bits only in that local calculation; it is still a store. **DISPROVEN structural global-serialization/read-only entailment:** explicitly interleave the separate atomic increment after B reload but before store; word2 is overwritten by stale1. Even a successful wrapper mutex does not serialize a participant that does not use it. Actual lost updates and external serialization remain UNKNOWN, not runtime defects asserted here.
- **VERIFIED:** acquire lazily publishes unchecked CreateMutexA result; wait/release outcomes are untested; release independently reloads the cached handle. Two explicit cold-zero observations can produce distinct handles, waits and a release of the subsequently cached handle. This is a code-admitted interleaving, not observed initialization races or OS failure.
- **VERIFIED:** alternate users submit fixed0139368C and argument-derived+628 to the same wrapper. Timer-only/fixed-word-only provider attribution is DISPROVEN at that source scope. No ownership meaning of those operands is added.

## Independent review and architectural consequences

Reviewer independently pursued the updater/gate/setter rather than merely confirming the old hooks. Main separately decoded/import/type-anchored those sites, preserved the GOG raw-only continuation and executed finite gate/word/mutex witnesses before credit. Reviewer and main agree on exact control fanout and the authority/synchronization ceiling; no scientific disagreement requiring a premise rewrite was found.

**Architectural blast radius:** CFunc can be a participant in conditional control, but Timer gate, fixed atomics, predicate answers, generic mask plumbing and initialization/currentness are separate dependencies. A mutex-shaped bracket does not discharge all-participant exclusion; “normal return” is not success/admission. These distinctions carry into resource/save/factory portfolios without automatically attributing their storage or mutexes to this word.

**Unpromoted:** runtime pause/resume/readiness, coherent cached generation, complete caller/provider population, scalar clock provenance, calendar schema, ownership/lifetime, global concurrency correctness or Steam/GOG policy parity. Both semantic overlays/all88/DO_NOT_RENEW remain authorities; this evidence is not Phase6 sufficiency or Phase8 readiness.
