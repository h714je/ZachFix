# Scene query member: COctTree lifecycle and object membership protocol

**Date:** 2026-10-03. **Phase 3 sequence24:** SCENE_QUERY_MEMBERSHIP_PRIMARY. **Outcome:** ADVANCE. **Build:** Steam. C0140–C0141; BND-116–118.

## Scope and reuse

Contract: `scratch/phase3_query_seq24_contract.md`. C0116 supplies the acquired CRdSceneDraw and invoked startup/shutdown; C0129/C0130 supply collection/packet/query consumption, not concrete membership. No accepted Phase2 root, packet, registry or generic actor+50 census was reopened. Main analyzed member acquisition/type/lifecycle and phase13 placement; a bounded Opus branch traced object-side insertion. Main personally checked the decisive tables, receivers, guards and append bytes before promotion. GOG was not needed for this Steam-only architectural edge.

## Acquired member and invocation — VERIFIED

- Steam constructor **006BCA40** writes **00826988**. PE RTTI identifies **COctTree**, whose observed own table has one deleting slot **006BCAB0**. The supplied symbol name is independently confirmed by raw RTTI.
- Actual typed startup **006D1706 ->006C7BA0 ->006C5700** preserves the scene receiver. The new member slice allocates **0x70** at006C57EF/1, calls006BCA40 with the allocated receiver, and publishes returned EAX at **scene+0x1CA4** at006C5830. This is a pointer to a separate allocation, not an embedded tree shell or the scene+63A8 packet-pointer vector.
- Initial configuration writes three zero values at tree+4/+8/+C and float values **40000/2000/40000** at+10/+14/+18; the depth argument is5. 006BBC90's selected construction arm stores child count4 and allocates same-type70-byte children. The RTTI name does not justify asserting eight children. Full spatial subdivision/search semantics are intentionally not analyzed.
- The constructor initializes a distinct **0x18 collection at node+0x58**, through0040D8B0. It has begin/end/capacity fields at collection+C/+10/+14, plus an allocated four-byte container-reference cell. The collection is not the packet collection at scene+63A8.
- Actual invoked scene shutdown **006D288C ->006C7CA0 ->006C58B0** checks this same member. It first invokes006BBE40 on it; that recursively invokes child cleanup, child own deleting slot(flag1), and clears the selected child pointers. The root then invokes own slot0(flag1) at006C590D and clears scene+1CA4 at006C591E.
- Own deleting006BCAB0 calls006BCAE0, then conditional outer free0074E82B for flag bit0. 006BCAE0 tears down the node+58 collection through0040D470, which frees its storage and reference cell. These are **positive member/child/buffer deletion invocations**, not CRdSceneDraw cache-free proof or deletion of objects whose pointers were in the collection. No all-path failure/safe-lifetime claim.

## Actual stored value and typed membership interface — VERIFIED mechanics

Exact raw CLevel007798AC and CPlayer007767FC install **virtual+48 =004029B0**. This is distinct from their virtual+44 packet producer and object field+44 native callback.

Let O be incoming ECX and Q the query argument:

1. Q=0 requests detachment from nonzero **O+14C**, then clears that field.
2. Q!=0 and **O+D8 bit31 set** uses Q directly as node N. Otherwise004029E1 invokes006BBEE0 on Q with **&O+E0, &O+F4, literal1**, and uses returned EAX as N. Its complete selection algorithm is outside this batch.
3. N equal to old O+14C is a no-op. If changed, the code requests removal from the old nonzero node, stores N at **004029FF**, and calls **00402A05 ->006BC040** with ECX=N and the **original object O** as stack argument.
4. 006BC040 forwards the address of its argument slot to0040D910 with **ECX=N+58**. The capacity-available path dereferences that pointer-reference and copies the **O pointer value** into the collection end, then advances end by4. The inserted value is **not &O, O+148 packet storage, a packed handle, or a CRdPicture**. Growth invokes the existing container insertion machinery; its allocator/failure internals are not expanded.
5. The old-node removal request006BC060 invokes range/search adapters on node+58 and conditionally invokes006BC360; its selected erase tail shortens end by4 and calls a RET8 no-op element teardown. The checked pointer-search primitive compares four-byte entries with the pointed-to target. **STRONG_INFERENCE:** this removes the matching object entry; iterator-conversion adapters are not fully expanded. **VERIFIED:** the local argument, conditional erase and end-shrink mechanics. Object deletion is not established by collection erase.

**STRONG_INFERENCE:** COctTree retains non-owning spatial/query membership of model objects, with per-object current-node state at+14C. Pointer-only entries, node-specific removal, unchanged-node no-op, separate packet storage and collection-only deletion reinforce this interpretation.

**UNKNOWN:** a particular CLevel/CPlayer instance actually taking this path in a live frame. Installed typed interfaces plus conditional dispatch prove a concrete class-compatible insertion protocol, not observed runtime class membership. The changed-node arm does not independently check resolver EAX for null before insertion; no all-path success/safety assumption. Do not claim a periodic whole-tree rebuild.

## Cross-system temporal connection — VERIFIED static placement

The already established scene/object dispatcher006C6090 has a newly checked **numeric phase13** arm:006C6EE3 writes scene+18E0=0D; a conditional loop obtains O from **scene+1CAC[index]**, loads **[scene+1CA4]** as argument, and invokes O's **virtual+48** at006C700A/006C703E/006C7060. Existing predicate/mask and selected temporary O+F0 overrides remain conditions, not friendly policy names.

Reused frame spine subsequently calls006D2B40. Its006BB5C0/006BC100 seam uses the **same acquired query member**;006BC100 appends node pointers only under selected tests and recursion/fallback. Returned nodes flow through **006BC2C0 ->node+58** to object pointer lookup, then the already verified predicate/virtual24/virtual44/packet148/scene63A8 submission continuation.

This joins **conditional object-phase membership assignment -> query-node object collection -> pre-render packet collection**, rather than inferring membership from a packet-compatible vtable alone. The first untyped edge is the particular class/object selected from scene+1CAC and its live eligibility; no new class census or temporal-frequency assertion is made.

## Primary reproduction and limits

`python3 scripts/inspect_phase3_scene_query_membership.py`: **948 instructions /2976 bytes**, COctTree plus two typed object interfaces,23 relative calls, exact publication/current-node/object-value/erase-noop assertions. Receipt: `scratch/phase3_scene_query_member_primary.json`. Main insertion-only personal check: **227/588**, `scratch/phase3_scene_query_insertion_main.json`; overlapping counts are not additive coverage. Bounded Opus handback is preserved at `scratch/seq24_query_insertion_agent_draft.md`; its selected insertion166/428 and detachment229/567 (395/995 total,12 calls) were locally reproduced successfully by `scratch/seq24_query_insertion_agent_check.py`. Counts overlap the main checks and are not additive. Pointer-removal role stays STRONG_INFERENCE at the unexpanded iterator-adapter scope. An initial checker assertion used address006BC2C9 instead of actual006BC2CA; it failed, was corrected from supplied assembly and rerun successfully before canonical use.

Primary Steam `DP_full.asm` references:

- member acquisition/configuration/reset invocation: `inputs/decompiler/steam/DP_full.asm:794177-794320`, `:796739-796821`, `:810625-810628`, `:811769-811771`;
- COctTree construction/deleting/destruction: `inputs/decompiler/steam/DP_full.asm:783816-783892`; collection teardown: `inputs/decompiler/steam/DP_decompiled.c:12576-12593`, `:12870-12934`;
- child cleanup and query-node collection: `inputs/decompiler/steam/DP_full.asm:782622-782675`, `:782887-782965`, `:783055-783062`;
- exact object/receiver transfer: `inputs/decompiler/steam/DP_full.asm:1960-2001`, `:782804-782881`, `:15527-15536`, `:783117-783148`, `:783700-783721`;
- phase13 and selected query consumer: `inputs/decompiler/steam/DP_full.asm:795689-795772`, `:812000-812065`.

Only the selected primary scope is promoted. Retainer exclusivity, object detachment versus free coordination, all masks/modes, actual class selection, safe reset, final GPU, animation-to-packet genealogy, GOG and cadence remain UNKNOWN. Stop scene depth here: Phase3 has the organizing member, acquisition/use/shutdown and actual object-value protocol; further algorithmic exploration is not justified by this batch. B0001–B0010 and all original accepted/bounded reopen triggers remain unchanged.
