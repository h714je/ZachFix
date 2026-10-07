# Phase7 A17 — ITEM_MANAGER_SELECTOR_ABI_RESOURCE_TUPLE_AND_READINESS

2026-10-07; parent validated checkpoint236. Main-read canonical primary only; no runtime observation.

## Inherited proposition and dependency

Selected World-compatible caller publishes named CSingleton<CItemManager> and supplies signedWORD input/currentEAX as ECX to opaque00456FB0. Base/table producer, ownership, current receiver and request success were not proved.

Canonical identities: C0303, C0304, C0037, C0114. Exact historical rows and source hashes: `audit/phase7_architecture_2026-10-07/A17_INHERITANCE.json`. Both overlays/prior corrections retain precedence.

## Stronger theorem tested

Incoming cached manager is the actual callee this/current instance, selector return is a uniform success status, named allocation initializes the usable table, and the resource tuple proves item/resource ownership or completed publication.

## Checked primary scope

Paired wrappers00456FB0/00456FE0 and their actual EAX-consuming/current-cache helpers00456C70/00456CA0; raw singleton declarations007702C4/007702B4. No table-loader/type/subsystem or external item-asset census

`audit/phase7_architecture_2026-10-07/A17_PRIMARY.json`. These contain exact instruction/bytes, original-ASM lines, fragment ownership and edges. Counts are validation/acquisition scope, not discoveries or complete functions/populations.

## Disposition — QUALIFIED_SURVIVAL / VERIFIED

- **VERIFIED selected scope:** Wrapper loads fullDWORD selector toEAX, overwrites incomingECX with selector-1 and unsigned-checks<=305. Accepted numeric1..306 calls the helper; rejected input reaches RET4 with originalEAX untouched. Incoming manager ECX is not consumed as a this pointer by this wrapper.

- **VERIFIED selected scope:** Helper saves selector fromEAX, independently reacquires current00BDA0F8, may allocate590/install the singleton table/publish0-or-pointer, then loads its C pointer without a local root/table-readiness guard. No table initializer is invoked on this acquired allocation path.

- **VERIFIED selected scope:** Helper indexes the pointed table by12*original selector, saves threeDWORD tuple values, and tests the first two as signed nonnegative. They separately feed current CRdData field1C reads and conditional existing Game-compatible resource requests; third value feeds field18 read/request on selected path. Tuple values are not the wrapper's cached-manager pointer or a proved owning resource identity.

- **VERIFIED selected scope:** Current-manager/CRdData acquisitions and read/request/retry paths are separate samples. Helper contains no aggregate execution/completion/result acknowledgement; wrapper's preserved EAX on rejected inputs alone prevents treating every nonzero return as a successful request.


## Counterexamples and limits

- **STATIC_INPUT_COUNTERMODEL**: Input selector307hex → Range rejects helper but returns original nonzeroEAX307; nonzero cannot be a uniform request-success certificate.

- **CONDITIONAL_MODEL_NOT_OBSERVED_RUNTIME**: Current00BDA0F8 changes after caller's publication/sample → Helper uses its independently reacquired M, not the incoming pointer overwritten by wrapper; actual epoch change UNKNOWN.

- **STATIC_INPUT_COUNTERMODEL**: Allocation/cached path reaches helper with unusable rootC table → No local readiness guard prevents the12-byte row reads; actual initialization/admission ordering UNKNOWN.


**DISPROVEN stronger components (not wholesale retirement of canonical claims):** Wrapper formally consumes incoming manager ECX as this; Every nonzero wrapper return certifies request execution; Selected helper has a rootC table-readiness gate.


**Independently preserved:** Named singleton declaration and selected caller/cache publication; Selected signedWORD operand transport, not item-owner semantics; Existing CRdData field1C/18 accessor protocol without universal resource subtype attribution; C0304 positive identity/service frontier is not convergence.


## Dependency propagation

C0303 emitted receiver transport remains a syntactic caller fact, not actual receiver identity/continuity. The named cache now has one positive selector/resource-use seam, but its table producer/schema/admission is a prerequisite for stronger item/resource identity, ownership and successful publication. C0037/C0114 selected typed resource facts cannot be transferred to all tuple IDs merely because the same accessor is used.


Blast radius: Manager ABI/current-instance/readiness and resource-domain qualification; no full item manager or resource-type proof.


## UNKNOWN / deliberately unpromoted

- **UNKNOWN:** RootC table producer, schema/count/selector admission and generation; Base CItemManager offset/full construction and current manager type/epoch; Actual tuple values/resource types/owning references; Resource request execution/completion/last use; All callers/items/service populations and runtime schedule.

- **Not promoted:** Observed null/OOB or wrong-resource fault; Original selector-1 is the actual row index; Every field1C result is C0114's picture; Named manager owns item/resources; Successful class/table initialization from vptr install; Uniform boolean return ABI.

- Reviewer scientific credit: none.


No Phase6 sufficiency renewal, Phase8, asset enrichment or runtime modification. This portfolio record is not a checkpoint/acceptance receipt.
