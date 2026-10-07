# Vehicle organizing roots and selected PC correspondence

**Date:** 2026-10-03. **Batch:** Phase 3 sequence 18, VEHICLE_ROOT_PRIMARY. **Outcome:** ADVANCE. C0127/C0128; BND-099–101.

## Source contract and credit

`scratch/phase3_vehicle_seq18_contract.md` declares Steam exact RTTI/vptr/acquisition and one typed use/lifecycle boundary, then independently located material GOG correspondence. H0007 was HYPOTHESIS, not a reusable address map. H0063 selector-0x53 construction is reused at its existing allocation/type scope; new event/use/type distinctions receive credit, not the generic factory or handle dispatcher again. Accepted Phase 2 remains closed. No runtime, Xbox, production-source or supplied-evidence modifications.

A bounded read-only agent independently located GOG types and boundaries. Main checked all decisive paired type/receiver/call/table and selected raw continuation facts before canonical integration. Middleware internals, full vehicle algorithms and B0003 cadence are excluded.

## Exact CCar organizing acquisition — VERIFIED

| Fact | Steam | GOG |
|---|---|---|
| Static root | 008C29F0, PE-initialized vptr | 008C29F0, independently checked PE-initialized vptr |
| Exact CCar own table | 0077F6FC -> deleting slot 005F5D80 | 0077F6EC -> deleting slot 005F5E80 |
| Raw COL / descriptor | 0087ABA8 / 008C29DC | 0087A6E0 / 008C29DC |
| Static initializer / registered finalizer | raw 0076B770 / 0076DC90 | raw 0076B480 / 0076D9A0 |
| Embedded CLayout constructor / table | 004592B0 / 0077294C | 004592E0 / 0077293C |
| Selected retained-result method | 005F5DD0 | 005F5ED0 |
| Request writer | 005F5DC0 | 005F5EC0 |
| Distinct native-task launcher / callback | 005F8400 / raw 005F5DF0 | 005F8500 / raw 005F5EF0 |

Exact RTTI `.?AVCCar@@` has only self in its hierarchy. The static root already contains its vptr in the PE; do not invent a heap getter or constructor vptr publication. Initializers construct **two embedded CLayout objects**, stride **0x398**, beginning at root+0xC and +0x3A4, then register the paired static finalizer. Initializer pointers are present at Steam0076E3A0/GOG0076E38C. These raw initializers are not exported function entries; no function inventory rows are manufactured.

Both static finalizers rewrite the CCar vptr and destroy those members. Own slot zero additionally has conditional allocation-free mechanics. **UNKNOWN:** actual execution of the finalizer registration, use of deleting slot zero on this static root, and full lifetime coordination. A conditional deleting implementation does not prove the static object is heap-freed. Selected dword+0x73C establishes size lower bound **0x740**, not a heap allocation size or full sizeof.

**STRONG_INFERENCE:** CCar is an organizing layout/resource/native-task aggregate. This interpretation is supported by embedded CLayout construction, selected resource requests and task-mediated reset; its exact raw type and mechanics above are VERIFIED. It is not yet proven to own physical vehicle actors.

## Concrete same-root use / task dependency — VERIFIED mechanics

Steam world-transition context `005D3591..005D35AB` acquires the previously proven CGame, tests its live-record bit 1 through `004508D0`, then sets ECX=008C29F0 and calls `005F5DD0`. That method retains this in ESI, calls CGame acquisition and selected lookup `00452270(0)`, then stores EAX at **CCar+0x73C**. The result's detailed type/ownership remains UNKNOWN. GOG independent caller `0043893A/00438942` supplies the same typed static root to `005F5ED0`, which likewise retains this and stores the selected lookup result at +0x73C. No GOG CGame type transfer is inferred merely from analogous getter calls.

Paired request callers Steam005DF1ED/GOG005DF2BD pass this root to a helper that writes absolute **root+4 = 0x63**, independently of incoming ECX. The raw callback's event-one arm tests that value, calls acquired resource-manager tag request **0x61**, marks its own native-task receiver through virtual +0x30, and clears root+0x73C. Event zero resets the two layouts and initializes selected root fields.

Crucial receiver distinction: `005F8400/005F8500` overwrite incoming ECX with the accepted CRdSceneDraw application root, acquire a **distinct selector-0 task**, set task+0x2C=0x11 and install the raw callback. Thus the callback's +0x30 target is the task, **not CCar**. This positive request/task/retained-result reset is not actor deletion, descriptor unload safety, proof of the callback's live recurrence, or a direct CCar-to-CObjectCar owning edge.

## Distinct typed vehicle actor boundary — VERIFIED selected scope

The actor RTTI is **CObjectCar**, not CCar. Paired tables **00773F74/00773F64** have 42 observed consecutive executable pointers; raw hierarchy includes CObjectCar, CObject, CRdObjectModelGame, CRdObjectModel and CRdObject at zero displacement. Reused selector0x53 allocation is0x2008; paired constructors **00541AA0/00541B70** publish these actor tables after CObject base construction. No new CAutomobile class is fabricated. Exact CAutomobile naming remains UNKNOWN; lack of an exact raw name is not proof the engine has no such conceptual family.

The actor own **slot+0xA4** points to **00544AB0/00544B80**. Its raw byte/pointer selector maps **event5** to **00546B44/00546C14**. Each retains the actor this in ESI, tests +0x1FD4==0, forwards the same actor to **0054AAE0/0054ABB0**, and, under +0x12F0!=0 and **+0x434 bit0x8000**, tails to **Steam00555B50 / GOG00555C20**.

Selected targets retain that same actor and gate bit0x800000 / nonnull +0x26C. Their secondary receiver is `[[actor+0x26C]]`: virtual +0x4C supplies a count and +0x50 supplies the list subsequently indexed. A list-element virtual +0x5C result4 gates selected float-argument calls through +0xD8/+0xDC; the arguments use scalar014AFFE0. A separate selected block reads actor+0x4E0 and sends the bounded value through list-element +0xE0 for indices2..3.

**VERIFIED:** numeric selector/gates/receiver/call mechanics and selected correspondence. **STRONG_INFERENCE:** physics wheel/drive-facing interpretation, supported by the selected interface structure and prior reconciled physics seeds. Exact middleware receiver identity/API labels were not newly proven here. **UNKNOWN:** mutable actor+0x44 callback installation, actual event-five delivery/frame placement, full wheel algorithm and live frequency/solver interaction. Native event routing in an installed own table is not by itself a once-per-frame claim. Historical player-car bit semantics and dismount transfer remain at their earlier secondary/runtime limits.

## H0007 correction / export boundary

**DISPROVEN historical same-address assertion:** Steam00555C20 is not the entry corresponding to GOG00555C20. It lies within the Steam00555B50 selected target. H0007 now records **00555B50 / 00555C20** at VERIFIED **selected boundary** scope, using independent CObjectCar RTTI/vptr/own-slot/event routing, same-receiver gates, list interfaces and distant selected output/steering blocks. Not full-body CFG equivalence or address-offset inference.

GOG `functions.csv` lists **body_start00555C20, body_end005575E6, body_bytes129**. These describe disconnected recognized coverage, **not a contiguous 129-byte function**. The decompiler stops at falsely nonreturning handle lookup006C5AD0; its raw RET at006C5AE8 and fallthrough00555C87 establish the omitted continuation. Some later supplied assembly is orphaned. The raw bounding interval[00555C20,005575E7) is6599bytes, not a unique instruction-byte count or universal function-boundary proof. Supplied exports remain unchanged.

H0063 construction identity is reused and its citation normalized without confidence inflation. H0006 chassis/readback cadence is not promoted by this batch. No forced full-function, shared-member, Xbox or same-address identity transfers.

## Primary evidence and reproduction

- `python3 scripts/inspect_phase3_vehicle_root.py` writes derived `scratch/phase3_vehicle_primary.json`. Main receipt: Steam **363 instructions/1327 bytes**, GOG **209 supplied-assembly instructions/795 bytes**, plus **136 raw-disassembly instructions** over exact omitted/selected spans. These counts are check scope, not semantic completeness. RTTI/hierarchy/vptr/event cells and relative calls are asserted directly against the PEs.
- Primary binaries: `inputs/binaries/steam/DP_STEAM.exe`, `inputs/binaries/gog/DP_GOG.exe`. Raw absent-listing blocks are decoded from these bytes using objdump; derived receipts remain separate from supplied evidence.
- Steam static/member/use: `inputs/decompiler/steam/DP_full.asm:992096-992105`, `:994173-994179`, `:563784-563820`, `:103120-103130`, `:524305-524311`, `:537069-537070`, `:566104-566118`, `:563826-563888`.
- GOG corresponding static/member/use: `inputs/decompiler/gog/DP_full.asm:813909-813918`, `:815986-815992`, `:425736-425773`, `:95502-95505`, `:62330-62332`, `:400771-400772`, `:426907-426921`, `:425788-425840`.
- Steam actor/selected event/writes: `inputs/decompiler/steam/DP_full.asm:547979-547988`, `:362453-362479`, `:365669-365685`, `:367962-367966`, `:372678-372690`, `:384535-384614`, `:385580-385607`, `:386223-386254`.
- GOG constructor/event/helper/export epilogue: `inputs/decompiler/gog/DP_full.asm:260087-260099`, `:262921-262930`, `:264400-264404`, `:268672-268684`, `:278259-278284`, `:279090-279148`; missing raw00555C87..00555D23 and selected556B0E..556B6D are in the checker receipt. `inputs/decompiler/gog/functions.csv` and `inputs/decompiler/gog/DP_decompiled.c:164411-164433` preserve the disconnected/false-noreturn limitation.

An initial checker assertion used incorrect GOG support-target assumptions; raw relative calls resolved them to0074E546/0074F23B and0040A2F0/004522A0. No canonical promotion preceded passing checks. This mechanical correction is not semantic correspondence by guessed deltas.

## Continuations and phase consequence

Selected mandatory vehicle acquisition/use and material independent PC boundary now have positive primary evidence. Carry **VEHICLE_LIFECYCLE_COORDINATION** for concrete CCar-to-actor ownership, actor+0x44 producer, actor teardown/retainer coordination, selected event-to-frame placement and fuller protocol/build scope. These are **unvisited OPEN**, not globally bounded. Reopen accepted selected type/event correspondence only for contradictory primary bytes/receiver/type or a structurally new boundary. B0003 still requires runtime traces.

This batch does not close Phase3. Independent scene/world/actor submission, animation ownership, fishing/effects, broader UI/event roots, named mode policy and shared lifetime coordination remain viable frontiers. Next candidate: RENDER_SCENE_SUBMISSION_ROOT, using the already acquired CRdSceneDraw/frame-context interface as an access point to new typed submission bodies, not repeating root construction, texture-registry wrappers or generic actor+0x50 scans.
