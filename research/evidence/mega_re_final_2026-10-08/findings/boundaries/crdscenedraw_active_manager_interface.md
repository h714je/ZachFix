# CRdSceneDraw active-manager and CRdObject interface integration

**Date:** 2026-10-02. **Frontier:** ACTIVE_MANAGER_INTERFACE_CLOSURE. **Outcome:** ADVANCE. Exact receiver/type/interface integration is new; existing registration/dispatch/retirement mechanics are reused, not rediscovered or counted twice.

## Contract

Universe: Steam `00BD7670` -> acquisition `00406F70`, its concrete constructor/table/RTTI/deletion paths, and selected already mapped registration/frame/retirement/shutdown receiver contexts. Reuse H0172/H0173 (acquisition/startup), H0001 (dispatcher placement, not whole-body equivalence), H0175/H0176 and the CShop positive portfolio at their established scopes. GOG deepening addresses foundational raw type/table/deleting anchors and build boundaries only. No generic dispatcher-caller, virtual-offset, marker, or handle release census. Success: rooted table extent/identity/material targets/use/owner receipt; negative: first missing concrete receiver/interface edge.

## Root and concrete identity — VERIFIED

Acquisition allocates **0x6778** bytes, calls Steam `00405F30` / GOG `00405EF0` with that allocation as ECX, then installs singleton table **0076F048 / 0076F038**, publishing the same pointer to **00BD9E68**. Startup `004018F6 -> 004018FD -> 00401902` acquires it, assigns **00BD7670**, and invokes H0173 initialization with EAX forwarded as ECX. The exact cache/application-root alias at this publication is proved, not merely inferred from matching sizes.

Raw RTTI names the singleton **CSingleton<CRdSceneDraw>** and base **CRdSceneDraw**, with the decoded hierarchy listing CRdSceneCross and CRdScene at zero PMD displacement. Constructor writes the base table **0076F020 / 0076F010** before acquisition overwrites it with the singleton table. **The active-object manager is this scene-draw-layer receiver**, not a separate untyped manager merged by offsets. It is not a CRdObject: its RTTI lineage and one-slot own interface differ from the 19-slot active-object interface it consumes.

Its constructor also installs distinct typed declaration/shader/vertex-buffer/texture subobject tables (first CRdDecl at +0x5D18). These are embedded members, not independent active objects or manager subclasses. This establishes a scene/render resource aggregation boundary but does not identify the final visibility/submission algorithm or prove every member's lifetime.

## Selected interfaces — VERIFIED mechanics

| Interface | Steam / GOG table | Observed extent | Material targets / context |
|---|---|---|---|
| CRdSceneDraw base | 0076F020 / 0076F010 | 1 code slot before next RTTI metadata | 00406980 / 00406940: destructor plus optional free |
| CSingleton<CRdSceneDraw> | 0076F048 / 0076F038 | 1 code slot before next RTTI metadata | 00406E10 / 00406DD0: singleton table rewrite -> 00406650 / 00406610 -> optional free |
| CRdObject consumer interface | 0076E6B4 / 0076E6A4 | 19 code slots before next RTTI metadata | +0x00 typed deleting target; +0x08 cleanup; +0x0C event-one callback adapter; +0x30 retirement marker. Other slots are recorded mechanically, not all named semantically. |

The manager's **own one-slot table is a deleting interface, not the multi-pass update table**. Registration/dispatch/retirement are direct methods on the rooted CRdSceneDraw receiver. The dispatcher calls the separately selected active objects' tables. Conflating those two receiver/table roles would produce a false architecture.

CRdObject constructor `00402650` independently installs its table, clears +0x44 callback and +0x50/+0x54 links, sets +0x3C/+0x40 to -1, and initializes fields through dword +0x158 (**lower bound 0x15C**, not universal allocation size). Raw slot zero resolves to Steam `0062D170`, GOG `005F4DD0`; each rewrites the CRdObject table, calls +0x08's cleanup target `006BA850/006BA7A0`, then conditionally frees this. The distinct shared deleting-entry addresses are not forced into a new universal 1:1 homology or inheritance taxonomy.

### Concrete lifecycle/use contexts

- **Manager registration owner:** H0175 inserts successful factory/CPlayer results into rooted receiver lists and allocates current +0x3C through static CRdHandleUtil. Raw CPlayer site `005FBDEB` loads 00BD7670 as ECX; existing selector/CShop evidence independently supplies the same owner edge.
- **Frame use:** H0001's static placement is `00401A70` root-loaded calls at **00401B6E and 00401B9F**, then root-loaded retirement at **00401C8D**. Prior report prose that calls 00401B68 a CALL is an instruction-location shorthand error: it is the preceding MOV ECX. No runtime frequency is inferred.
- **Class-discriminated active table:** existing CShop 19-slot table has the same concrete +0x0C event-one target `006BA8A0/006BA7F0` and +0x30 marker `006BAB20/006BAA70` as the decoded CRdObject base table. CShop callback/state-3 retirement and class slot-zero deletion establish concrete use/cleanup endpoints; this is stronger than a generic offset scan. See C0112 and `cshop_callback_retirement.md` for raw recovered GOG anchors.
- **Retirement:** H0176 selected list iteration calls the target's +0x08 cleanup, releases its current handle, and invokes that target's slot zero with flag 1. It does not invoke the manager's own deleting slot. CThrowLure self-links, conditional marker gates and non-current mappings remain outside clean-lifetime claims.

## Shutdown/reset versus allocation free

**VERIFIED:** `00401310` passes 00BD7670 to **006D2880 / 006D2450**, then clears only the application root. The destructor **00406650 / 00406610** also calls that method, then performs additional embedded member/vector/base teardown. Base/singleton slot-zero targets conditionally free the allocation only after that fuller destructor.

**UNKNOWN:** a concrete invocation of either manager deleting slot on the cached allocation, or cache-root clear/free coordination. The reviewed cache xrefs are only acquisition READ/WRITE (two per build); direct deleting-target calls are absent from the supplied direct-call manifest. This is an exact-source free-path limit, not proof that indirect deletion never happens or that the process leaks. **Application cleanup/root clear is not allocation free.** Original OBJ-ACTIVE-OBJECT-MANAGER free-site wording and `active_object_manager_destroy` naming must be narrowed to shutdown cleanup; the positive destructor/deleting implementation is recorded separately.

## Primary evidence / reproduction

- Raw table/COL/type/hierarchy decoding for both builds: `python3 scripts/inspect_phase2_active_manager.py`; derived `reports/PHASE2_ACTIVE_MANAGER_RAW_CHECKS_2026-10-02.json`; primary binaries `inputs/binaries/steam/DP_STEAM.exe` and `inputs/binaries/gog/DP_GOG.exe`. Next words are distinct RTTI metadata, not an assumed continuation of the manager/object tables.
- Steam acquisition/construction: `inputs/decompiler/steam/DP_full.asm:6805-6836`, `:5842-5863`; GOG corresponding raw `inputs/decompiler/gog/DP_full.asm:6775-6806`, `:5812-5833`. Supporting decompiler Steam `inputs/decompiler/steam/DP_decompiled.c:5091-5358`, `:5868-5900`; GOG `inputs/decompiler/gog/DP_decompiled.c:5045-5312`, `:5822-5854`.
- Root publication: Steam `inputs/decompiler/steam/DP_full.asm:619-622`; independently paired startup context uses the same instruction VAs and GOG H0172/H0173 targets. Frame/retirement contexts: Steam `inputs/decompiler/steam/DP_full.asm:784-852`, corresponding GOG raw at the same selected call VAs; reused `application_object_dispatch.md` / `object_virtual_phases.md` preserve GOG false-noreturn continuation limits.
- Constructor/destructor/deleting contexts: Steam `inputs/decompiler/steam/DP_decompiled.c:5359-5484`, `:5770-5785`; GOG `inputs/decompiler/gog/DP_decompiled.c:5313-5438`, `:5724-5739`. Raw singleton deleting targets checked via objdump at Steam 00406E10..00406E34 / GOG 00406DD0..00406DF4; body passes the same allocation to optional free.
- Shutdown cleanup: Steam `inputs/decompiler/steam/DP_full.asm:265-269`, decompiler `inputs/decompiler/steam/DP_decompiled.c:450063-450122`; reused H0174 root context and corresponding GOG raw source. Cache xrefs and absence of direct deleting callers: each build's `xrefs.csv` and `calls.csv`, limited to 00BD9E68 and the decoded deleting targets.
- Object interface construction: Steam `inputs/decompiler/steam/DP_decompiled.c:1428-1526`, raw table write `inputs/decompiler/steam/DP_full.asm:1735-1745`; GOG constructor `inputs/decompiler/gog/DP_decompiled.c:1430-1528`. Base deleting implementations: Steam `inputs/decompiler/steam/DP_decompiled.c:352322-352337`, GOG `inputs/decompiler/gog/DP_decompiled.c:250313-250328`. CPlayer registration anchor: Steam `inputs/decompiler/steam/DP_full.asm:569845-569855`, GOG `inputs/decompiler/gog/DP_full.asm:430328-430338`. CShop and handle mechanics reuse their focused primary-backed findings.

## Closeout consequence and limits

C0116/BND-084 integrate rooted identity, table extent/material targets, construction, direct method/use contexts, concrete active-object consumer targets, and cleanup/free split. The selected central interface requirement is satisfied at that static architectural scope; it is not a promise of full scene-render semantics or all virtual slots.

Carry `ACTIVE_MANAGER_ALLOCATION_FREE` for cache/deleting invocation under a new concrete caller/cache-clear/typed indirect edge, and retain all existing runtime/handle/parser bounds. Full game-owned visibility/submission remains `RENDER_SCENE_SUBMISSION_ROOT`; root identity/member aggregation reduces its acquisition gap, not its final submit algorithm. H0172 mechanical acquisition identity can now be VERIFIED from exact paired anchors; H0173/H0001 stay scoped STRONG_INFERENCE where already recorded. No Xbox mapping, no forced orphan/full-body correspondence. Phase 2 readiness must still receive a dedicated all-requirement closeout/red-team review before any Phase 3 entry.
