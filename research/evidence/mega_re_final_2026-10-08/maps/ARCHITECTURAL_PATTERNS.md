# Architectural Patterns

## Scope and evidence posture

This map consolidates patterns already established in the ledgers, findings, checkpoints, and journal. It is not a new executable scan. Statements use the project confidence vocabulary; address identity remains build-specific unless a homology group is explicitly recorded.

> **Historical-scope notice — approved governance reconciliation, 2026-10-02:** the numbered sections below preserve earlier pattern slices, not a current root/owner census. Their resource-manager no-vtable/candidate wording and broad typed-family UNKNOWN statements are superseded at the specific scopes in `findings/formats/resource_manager_root.md`, C0114 / `findings/boundaries/xpc_resource_positive_portfolio.md`, and C0116 / `findings/boundaries/crdscenedraw_active_manager_interface.md`. C0115 / `findings/boundaries/cmap_world_level_owner_root.md` supplies the current static world receiver. Use current scoped claims/ledgers/findings rather than those old unknown labels as new premises; no full parser, scheduling, ownership or runtime closure follows. This notice adds no executable evidence and preserves the historical text.

## 1. Resource loading is a layered pipeline

**VERIFIED** from the archive, manager, callback, and typed-resource findings:

`DPSERIAL.001-.003 stream -> archive record/header extraction -> raw or XZP1/zlib payload normalization -> outer resource-manager descriptor/record path -> extension callback -> typed parser/constructor -> subsystem consumer`

The archive stream and extraction layers are separate from the outer manager table. The manager owns a callback pointer at `+0x10`, a record table rooted at `+0x0C`, and a mutex at `+0x388`; callback invocation is performed under that mutex. `CFlkUtil` is a distinct stack-local archive utility and must not be merged with the outer manager merely because their lifecycle code is adjacent.

**STRONG_INFERENCE:** the callback is the principal typed-resource seam because startup supplies `00408310` (Steam) / `004082D0` (GOG) to the manager initializer and that callback dispatches by extracted record extension/name.

## 2. Extension dispatch is a family boundary, not proof of a complete class model

**VERIFIED:** the callback has direct XMD, XPC, and DSB-related branches plus explicit checks for the other recorded extension families. XMD allocates/constructs CRdMesh; XPC allocates CRdPicture and enters the XPC2 parser; XPC2 validates magic `0x32435058`, derives child records from dimensions, inflates subpayloads when needed, and copies normalized state.

**UNKNOWN:** XAM, XCA, XFE, XWP, XNV, DSB, and the remaining branch helpers' concrete object classes, ownership, and downstream consumers. A literal extension check establishes an executable boundary, not a parser or class identity.

## 3. Constructor/destructor/vtable evidence scales class identification

**STRONG_INFERENCE:** reliable class promotion requires at least two of: concrete allocation/constructor behavior, destructor or cleanup behavior, vtable write/reference, and a typed callback or consumer edge. This standard produced separate build-specific candidates for CRdMesh, CRdPicture, CFlkUtil, and the outer resource manager layout.

**VERIFIED:** CRdMesh has build-specific constructors and vtables; CRdPicture has allocation/destructor/vtable evidence; CFlkUtil has concrete constructor/destructor/vtable evidence and stack-local provenance. The outer manager has a strong field/layout and constructor/cleanup path but no proven vtable, so it remains a candidate rather than a named engine class.

## 4. Scheduler-to-dispatcher boundaries expose lifecycle phases

**VERIFIED:** `00401A70` is an application-to-object-dispatch boundary in both PC builds. The Steam dispatcher writes phase state at `this+0x18E0`, submits through the PhysX path at state `0x0E`, reaches completion/fetch-side bridges at state `0x07`, delivers virtual Event 6 at state `0x08`, and enters another worker path at state `0x0B`.

The GOG dispatcher has the same structural role, but its visible decompiler boundary is wrong because `006C5AD0` is marked `noreturn` despite a raw return and continuation. Raw assembly outranks the truncated decompiler shape.

## 5. Managers commonly combine tables, callbacks, synchronization, and lifecycle markers

**STRONG_INFERENCE:** the outer resource manager pattern is a reusable search template for other subsystems: constructor receives count/configuration and a callback, allocates a fixed-stride table, initializes support state and synchronization, then registration/population functions create descriptors and invoke the callback. This is a research heuristic, not proof that every manager uses the same layout.

The resource manager's table stride (`0x30`) must remain distinct from archive candidate records (`0x20`) and XPC child records (`0x20` observed in the parser). Equal strides do not imply equal formats or ownership.

## 6. Globals are bridge candidates, not semantic labels

**UNKNOWN/STRONG_INFERENCE:** high-reference writable globals such as `008A9BA4`, `008A9758`, `008A9980`, `008A9BC8`, `00BD7670`, and `014AFFE0` are useful cluster bridges. Their names in `GLOBAL_LEDGER.csv` are candidate labels derived from access patterns and prior maps; they do not establish ownership or exact type without initialization and consumer evidence.

Use global read/write aggregation to join clusters, then verify the owning constructor/root and lifecycle before promoting a manager interpretation.

## 7. Cross-build homology needs structural anchors

**VERIFIED:** Steam and GOG contain paired scheduler, resource, archive, and typed-object structures with build-specific addresses. **STRONG_INFERENCE** homology is justified when call role, constants, field offsets, vtable relationships, and surrounding control flow agree. Address proximity alone is insufficient. Xbox remains comparative because its corpus lacks a matching PC-style function/call/xref manifest.

## 8. Negative evidence is architectural information

**VERIFIED correction:** `0040A2F0` GOG / `0040A320` Steam are CSingleton<CGame> initialization, not generic resource request helpers. `006B2C30` / `006B2BA0` are indexed manager record/cache accessors. **VERIFIED correction:** CFlkUtil is not the outer resource manager. These rejected interpretations constrain future searches and should not be silently reintroduced.

## 9. Current architecture map

```text
application/frame root
    -> object dispatcher
        -> actor virtual phases / PhysX-facing phases
        -> native task/object registration

resource startup
    -> outer resource manager (+0x0C table, +0x10 callback, +0x388 mutex)
        -> DPserial segment/archive stream
            -> record lookup and XZP1/zlib extraction
                -> descriptor registration/callback
                    -> extension-specific parser/constructor
                        -> CRdMesh / CRdPicture / unresolved typed families
                            -> render, actor, animation, effects, navigation, script consumers
```

The lower resource layers and XMD/XPC/XPC2 boundaries are primary-backed. The right side after typed construction remains intentionally incomplete.

## 10. Derived-state cautions

`maps/WHOLE_PROGRAM_OVERVIEW.md`, `reports/PHASE1_STRUCTURAL_CENSUS.md`, and some older status wording predate the archive and typed-resource discoveries. Their historical statements remain useful as snapshots but must not override current ledger entries. Duplicate prose in selected derived ledger notes is a presentation-quality issue only; no raw evidence was changed and the blocked normalization script was not executed.
