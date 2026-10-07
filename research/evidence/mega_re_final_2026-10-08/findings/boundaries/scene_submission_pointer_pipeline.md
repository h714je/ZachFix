# Scene submission pointer pipeline and typed model-packet interfaces

**Date:** 2026-10-03. **Batch:** Phase3 sequence19, RENDER_SCENE_SUBMISSION_ROOT. **Outcome:** ADVANCE. C0129/C0130, BND-102–104. **Build:** Steam only for new semantics.

## Contract / reused scope

`scratch/phase3_scene_seq19_contract.md` declares a new render-context callee/typed producer-consumer discriminator. C0116 supplies acquired CRdSceneDraw; C0117 supplies frame/context ordering, not new callee semantics. H0173/H0174 startup/shutdown identity and C0114 CLevel+160/+164 resource associations are reused only at established scope. No accepted Phase2 root/constructor/registry/+50 census is reopened or credited again. GOG has no new field/packet mapping; Xbox and live cadence remain unproven.

Main analyzed pre-render006D2B40 and collection acquisition/lifetime. One bounded read-only agent analyzed post-render006D32E0 and selected typed model-packet interfaces; main independently checked decisive receiver/table/field/call bytes, including an important +84/+88 resource distinction not settled by the agent's compact extraction.

## Acquired scene receiver / collection lifecycle — VERIFIED

- Reused startup forwards the acquired CRdSceneDraw to006D1610. Its saved ECX flows through a **0x18 allocation**, constructor006DD8E0 and publication at **scene+0x63A8** (006D2209..006D2265). This is a distinct heap-backed collection object, not an inline array of packets or a separately acquired game manager. Exact C++ template/class name is UNKNOWN.
- Count006DD690 computes `(end(+10)-begin(+0C)) >> 2`; index006DD6B0 checks the bound and returns an **entry address**. Callers dereference it for a pointer value. The appender006BC3F0 takes the address of a pointer, copies one four-byte entry into storage and advances end, or uses a growth path.
- Pre-render006D2B40 calls collection erase006DD6E0 before selection. Its erase helper006DDA10 updates end; clearing vector entries is not packet deletion.
- Accepted invoked shutdown cleanup006D2880 now has a newly checked **member** continuation: nonnull scene+63A8 ->006DD510(flag1) ->shared teardown006CB1D0 ->free of the0x18 collection object ->scene+63A8=0. This is positive allocation/free coordination for the collection shell, **not actual CRdSceneDraw cache deletion, model-packet freeing, or full inner-buffer/payload lifetime closure**. The existing manager-cache free bound stays closed.

**STRONG_INFERENCE:** scene holds a submission pointer vector borrowing separately stored model packets. Separate model allocation/retention and pointer-only entries support borrowing; exclusive ownership and safe lifetime across all reset/growth paths remain UNKNOWN.

## Pre-render selection and model packet production — VERIFIED mechanics

Frame00401C42 passes `[00BD7670]` plus the selected context to006D2B40. Original ECX is saved as the scene receiver. The selected query path passes **scene+1CA4** and context+EC to006BB5C0/006BC100; returned node pointers expose object-pointer containers at node+58. Selected object predicates/flags lead to virtual **+24(context)**, then conditional **+44(context)**. The actual query-selected object classes and spatial-query member's construction/type remain UNKNOWN; no generic offset census was used to invent CMap/CPlayer membership.

Raw exact CLevel table007798AC and CPlayer table007767FC each install:

| Installed member | Target | Selected meaning |
|---|---|---|
| +24 | 006BD320 | Selected visibility/eligibility predicate; broader geometry semantics not promoted |
| +28 | 006BD4C0 | Shared model submission packet population |
| +44 | 00405E40 | Packet capacity/allocation and virtual+28 invocation |

These are **class-proven interface properties**, not proof an actual CLevel or CPlayer instance is in this query result. The whole vtable/state/actor update taxonomy is not promoted.

00405E40 retains model this. It derives a capacity from baseline **0x7D4**, adding signed payload count*48 when model+160 exists and a further selected0x17C0 extension. Do not assume the signed count is nonnegative without format evidence. On growth it frees an existing **model+148** pointer, clears it, allocates replacement and republishes there; capacity is **model+144**. It passes that pointer and context to this object's virtual+28. Producer success sets **model+138 bit0x800000**, failure clears it. No all-path allocation-failure safety or teardown claim.

006BD4C0 writes **packet+2C = 1**:006C4920 directly returns AL=1, which is sign-extended before the dword store. It also writes:

- **packet+84 = model+160**;
- **packet+88 = model+164**, not model+160.

For the installed CLevel interface, reused C0114 associates these with **CRdMesh/XMD** and **CRdPicture/XPC**, respectively. The latter is a resource receiver, **not a CLevel/actor pointer**. Actual selected CLevel membership remains UNKNOWN.

006E1150 tests the packet-valid flag and returns **the pointer value stored at model+148**, not an inline record's address. At006D2FFC/006D3001, pre-render obtains that value;006D3014/006D301A appends it to `[scene+63A8]`. A second conditional path uses the same collection. This joins packet storage to scene submission without inventing exclusive transfer of ownership.

## Later consumer / context distinction — VERIFIED

00401440 stages the selected frame context. At0040161D the rooted scene is loaded; **scene+6711==0** permits00401633 ->006D32E0. Its actual immediate argument at0040162E is **fixed staging address00BD9E70**, not directly the earlier selected00BD9648/00BD86B8 pointer. C0117's staging/order remains valid; source and derived context must remain distinct.

006D32E0 reads the **same scene+63A8** collection, counts/indexes/dereferences pointer entries and compares **integer packet+2C tags1/2/3**. The decompiler's comparisons with tiny float constants are a representation artifact, not a floating tag format.

One selected tag1 branch tests packet+30 flags, then sends **packet+88** as ECX to005D7CB0. That helper reads `word[*(receiver+4)+8]` through006B56F0. With the checked typed CLevel packet producer, this is compatible with the retained **CRdPicture/XPC** resource association, not evidence the helper is a CLevel actor method. No friendly count/format meaning is assigned to that word.

Accepted tag1 pointers enter a **stack-local pointer collection** at006D3C95. Its later indexed pointer flows at006D40D1 to006D57B0 with the original scene this. That consumer reads **packet+84** and invokes a method on the model resource. This is an exact collect -> classify/filter -> resource-aware consume continuation. Full draw-call algorithm, all tag meanings/pass policies and final GPU submission ownership remain UNKNOWN.

## Primary evidence / reproduction

`python3 scripts/inspect_phase3_scene_submission.py` checks **1,824 instructions / 6,884 bytes**, two raw typed interfaces and22 relative call edges, and asserts packet tag/resource assignments, pointer getter, allocation size and fixed staging argument. Derived receipt: `scratch/phase3_scene_submission_primary.json`. Counts are verification scope, not completeness. Source PE: `inputs/binaries/steam/DP_STEAM.exe`; supplied sources are unchanged.

- Allocation / member shutdown: `inputs/decompiler/steam/DP_full.asm:810577-810590`, `:811329-811346`, `:811902-811918`, `:823696-823712`.
- Pre-render query/object/packet handoff: `inputs/decompiler/steam/DP_full.asm:811927-812083`, `:812146-812216`; supporting `inputs/decompiler/steam/DP_decompiled.c:450123-450319`.
- Packet allocator and producer: `inputs/decompiler/steam/DP_full.asm:5772-5836`, `:784687-784857`, `:793151-793158`, `:829841-829860`; supporting `inputs/decompiler/steam/DP_decompiled.c:5046-5090`, `:436562-436846`.
- Pointer count/index/append/erase: `inputs/decompiler/steam/DP_full.asm:823843-823875`, `:783177-783221`, `:823881-823915`; `inputs/decompiler/steam/DP_decompiled.c:455513-455542`.
- Post-render caller/tag/resource/local-list/consume: `inputs/decompiler/steam/DP_full.asm:415-421`, `:812630-812650`, `:812707-812713`, `:812951-812957`, `:813205-813211`, `:814788-814825`; resource helper `:529313-529315`, `inputs/decompiler/steam/DP_decompiled.c:431472-431482`.
- Reused typed resource lineage: `findings/boundaries/xpc_resource_positive_portfolio.md`, `findings/boundaries/crdpicture_clevel_resource_attachment.md`. Reused roots/order: `findings/boundaries/crdscenedraw_active_manager_interface.md`, `findings/boundaries/application_media_phase_placement.md`.

## Late handoff consistency check

A targeted primary check confirms that006E1150's mask helper00417E00 reads the same **model+138/+13C** fields written by the packet producer and returns their masked EAX/EDX pair. Five instructions/23bytes matched the PE; `inputs/decompiler/steam/DP_full.asm:27519-27523`, derived `scratch/phase3_scene_packet_flag_consistency.json`. This strengthens the existing C0130 flag/getter join without a new claim, helper-classification or productivity credit. The original sequence19 checker/count remains unchanged. Also keep **object field+44 native callback** distinct from **vtable slot+44 packet allocation/population**.

## First missing edges / continuation

**UNKNOWN / unvisited OPEN:** scene+1CA4 query-member acquisition/type and one concrete class-discriminated object insertion/membership ->selected render record; full draw-call/pass algorithm, model-packet lifetime after capacity replacement, inner-container teardown, animation-state-to-packet genealogy, GOG and cadence. No global absence or exact-source exhaustion is claimed.

Selected root/collection/packet/caller mechanics are positive. Carry **SCENE_QUERY_MEMBERSHIP_PRIMARY** for a new typed query-member or insertion/retainer discriminator, not repetition of collection helpers or generic actor+50 offsets. RENDER_SCENE_SUBMISSION_ROOT is **PARTIALLY_RESOLVED_PHASE3**, not a waived all-family closeout requirement. Independent fishing owner/update is the next breadth candidate; productive counter becomes3, so the next useful batch requires the whole-frontier strategic meta-review at4.

## Sequence24/25 continuation at new scope

C0140/C0141 nowestablish acquiredCOctTree1CA4/memberlifecycle/originalobjectnode58 protocol/phase13-to-query join; see findings/boundaries/scene_query_member_lifecycle.md. C0142/C0143 supplyanactualregisteredCRdObjectModel taskcreation source withshared48 interface, notlivequeryselection. Earlierquerytype/protocolunvisitedwording ishistoricalsequence19scope, notcurrentabsence. ActualCLevel/CPlayer/liveclassmembership/fullGPU/animationgenealogy/GOG remainunknown. Originalpacket/collectionmechanicsandconfidenceunchanged; no wholesalesource reopening.
