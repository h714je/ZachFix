# CMap numeric state, retainer reset and resource-phase placement

**Date:** 2026-10-03. **Frontier:** WORLD_LEVEL_LIFECYCLE_PLACEMENT. **Outcome:** ADVANCE. C0119/C0120; BND-088–BND-091. Steam-only new scope; accepted C0115 CMap and C0035 CRdData identities reused.

## Contract and new discriminator

New C0118/BND-087 audio input reads **01393700 = CMap013936F0 +0x10**. Universe: exact Steam scalar xrefs/writer branches, concrete installed writer tasks, one class-proven CMap resource/code consumer, and the specific resource operation -> pending consumer exposed by numeric state 7. Exclude accepted constructor/static lifetime/named attach/XPC construction, generic callbacks/offset scans and PhysX internals. A read-only agent gathered exact writer/caller/raw-table facts; the primary researcher independently checked promoted bytes/maps/receivers and interpreted resource callees. Success: positive typed mode/use/reset and object/resource boundary. Stop at missing incoming selector or untyped outer context; numeric values are not human mode names.

## Numeric world-state producers — VERIFIED selected mechanics

Exact `xrefs.csv` universe for 01393700 has 40 rows (19 READ, 20 WRITE, 1 READ_WRITE). Eighteen writer/RMW instructions are in `005D26B0`; three other writes are raw `0063D0FC`, `00643C53` in `00642640`, and raw `0064E737`. These are direct-scalar sources, not an exhaustive receiver-relative field census. CMap+0x10's human meaning remains **UNKNOWN**.

- `005D26B0` is a **three-stack-argument event dispatcher**, not CMap thiscall. Event 1 selects `005D2978`, whose numeric state table `005D4AF0` maps **7 -> 005D44E9**. Full event/state algorithms are not promoted from the monolithic decompilation.
- Raw callback **00637C80**, installed by **0063EA20** through accepted selector-0 registered task/setter mechanics, has an event-1 arm. Private **014735F0 == 200** selects `0063CD43`; if byte **01473674 != 0 AND CMap+0x10 == 3**, it sets CMap+8 bit 0x200, then writes **CMap+0x10 = 4** at `0063D0FC`. This is a concrete conditional **3 -> 4** world-state producer in an installed object-task context. Its installer caller in `00642640` supplies outer context 014722C0; full context type is UNKNOWN.
- Raw callback **0064DB70**, installed by **0064EBF0** on task root **008C5660**, has an event-1 arm and private discriminator **008C5644 == 6 -> 0064E557**. A selected continuing path writes **CMap+0x10 = 3** at `0064E737`. **Six is private callback state, not prior CMap state**: no CMap 6 -> 3 transition is inferred. Outer installer caller raw `0052A556` supplies 008C5640; its fuller type/lifecycle remains unresolved.
- The accepted CRdObject phase-2/event-one interface supplies **conditional static object selection** for these installed tasks. It does not establish every-frame invocation, live selection or named title/gameplay/loading/pause policy.

## State 7: counter requests, pointer clear, numeric successor — VERIFIED

The accepted CRdData singleton getter `004051F0` publishes its returned EAX to **00BD768C** at startup `00401871`, with the identical pointer supplied as ECX to `006B2570`. In numeric state 7, `005D26B0` passes `[00BD768C]` as ECX to **006B29E0** for tags **9, 0x10, 0x11, 0x0C, 0x0B, 0x0E, 0x12..0x16**, followed by **64 tags 0x18..0x57**.

Then `005D45CA..005D45D8` uses zero EAX, count 0x40 and EDI **01437688** to `REP STOSD` all **64 accepted retained CLevel-pointer slots**. Later `005D4677` writes **8** from conserved EBP (initialized at `005D2982`) to the state scalar. Other helpers surround this sequence; their full cleanup effects are unvisited. The pointer-clear block itself neither dereferences nor deletes the old pointers. Equal 64-element cardinality does **not** prove tag i+0x18 names resource/level i.

### Resource operation is deferred, not immediate unload — VERIFIED

Main's independent assembly check establishes:

- **006B29E0:** scan manager count +4 / record table +0x0C, stride 0x30; compare signed byte descriptor **+0x2E** with requested tag; for matches call **006B2830(index,1,0)** using the same manager ECX.
- **006B2830:** wait on manager mutex +0x388; read descriptor **+0x2C with MOVZX** (unsigned 16-bit counter). Nonzero force argument +0x0C sets it to zero rather than decrementing. On transition to zero decrement manager +8 and set **manager +0x14 = 1**; if counter already zero but payload +0x18 is nonnull, also set +0x14. Release mutex. There is **no deletion callback or payload free in this body**; third stack argument is not used here. Decompiler signed-short shape is not authoritative.
- **006B2A40:** when manager +0x14 is nonzero, clear it, acquire mutex, scan records, pass each index to **006B3AA0**, then release mutex.
- **006B3AA0:** only for nonnull record +0x18 and zero unsigned +0x2C, invoke function pointer at manager **+0x10** with stack arguments **event 1, descriptor pointer, index**; then conditionally free payload +0x18, null it and clear the 0x30-byte descriptor. Existing typed XPC event-one deletion interface (C0114) is reused, not reconstructed.

**VERIFIED frame placement, BND-089:** `00401A70`, after its object repeat loop, context/media phase and active-manager retirement call `00401C8D`, loads **[00BD768C]** at `00401C92` and calls **006B2A40 at 00401C98**. Thus the acquired resource root has a positive post-object/media pending-work use position. Actual occurrence/order of the central state-7 handler against that frame tail is **not established** by its separate raw incoming seam. No guarantee the payload callback runs only after the CMap clear, nor before a CLevel's final use, follows.

## Concrete CMap resource/code consumer — VERIFIED; presentation role STRONG_INFERENCE

`0044A040`'s **event 0x12** path has explicit early-return gates, then a composite continuation condition including **CMap+0x10 == 1 or 2**, receiver bytes +0x1CC/+0x1CD, or selected other globals. On that selected branch, raw **0044A7B0 sets ECX=013936F0**, and **0044A7B5 calls 005D1B00**.

`005D1B00` saves the actual CMap receiver, uses root predicate `005D12B0` to choose index field **+0xA40B8 or +0xA40BC**, obtains an indexed resource through the accepted CRdData getter/result ECX -> **006B2C70**, and feeds the result to **006B5590**. It also calls renderer-side helpers and the accepted narrow code dispatcher **0045C9F0**, updates CMap timer **+0xA40B4** using 014AFFE0, and rotates the selected index on expiry. This is a concrete **world-root -> resource/code consumer** context; “transition/loading presentation” is **STRONG_INFERENCE**, not a proved user-facing mode or final draw submission. Resource type/individual code semantics and CMessage receiver alias are not transferred from matching offsets or prior names.

Raw wrapper `00474938` forwards arguments to **0044ACD0**, which forwards the event/argument pair to `0044A040`. A selected raw selector connects this path and the central world handler, but not a known application caller.

## Raw selector/export limit — VERIFIED routing, UNKNOWN incoming placement

Raw **00474830**, absent from `functions.csv` and omitted entirely from supplied `DP_full.asm`, reads a signed word selector at stack +4, subtracts 3, unsigned-bounds it to 0x3F, and jumps through **00474E18**:

- input **15 (0xF)** -> table word **00474E48 = 00474980** -> wrapper call **0047498F -> 005D26B0**;
- input **13 (0xD)** -> **00474E40 = 00474938** -> **00474947 -> 0044ACD0**.

The selector word is not forwarded; wrappers forward original +8/+0x0C/+0x10 as the three callee arguments. **BOUNDED_STATIC source limit:** exact absolute pointer search across file-backed PE sections and relative E8/E9 target scan across .text expose no incoming encoding to 00474830. Computed/indirect selection, other encodings and runtime invocation remain **UNKNOWN**; no dead-code claim. Reopen `WORLD_SELECTOR_INCOMING_PLACEMENT` only for a concrete incoming computed/indirect/other-encoding or structurally new raw boundary, or a trace naming this receiver/selector path—not a repeated pointer scan.

This newly recovered indirect table satisfies the old native-task reset finding's higher-table discriminator **at wrapper-routing scope only**. Accepted `native_task_availability_reset_boundary.md` export-level negative/history remains intact; native 00647730 availability/lifetime and its bounded generic seams are not reopened or resolved. No raw callback/selector is fabricated as an exported function-ledger row.

## Verification and references

- Accepted static roots/type/retention: C0115 / `findings/boundaries/cmap_world_level_owner_root.md`; C0035 / `findings/formats/resource_manager_root.md`. No construction/attach repetition or new GOG/Xbox mapping.
- Scalar/event/state7 and request/clear: `inputs/decompiler/steam/DP_full.asm:523366-523378`, `:523532-523569`, `:525248-525340`; startup alias `:581-586`; supporting `DP_decompiled.c:297757-297808`.
- Main resource operation bodies: `inputs/decompiler/steam/DP_full.asm:769168-769226`, `:769329-769411`, `:770824-770875`; supporting `DP_decompiled.c:428654-428688`, `:428741-428783`, `:429608-429634`. Frame call/root: `DP_full.asm:849-858` (exact verified site 00401C92/00401C98).
- CMap consumer: `inputs/decompiler/steam/DP_full.asm:85583-85589`, `:522651-522981`; supporting `DP_decompiled.c:56929-57109`, `:296245-296436`; raw selector wrappers `DP_full.asm:135052-135087`. The omitted entry is referenced by raw executable VA/bytes, not an invented line.
- Installed writer-task primary anchors: `inputs/decompiler/steam/DP_full.asm:636060-636076`, `:638320-638356`, `:640854-640861`, `:641058-641069`, `:642512-642533`; second installer/writer `:659437-659526`, `:659797-659817`. Omitted 0064DB70/0064E1F5 prologues require raw bytes/objdump.
- `scripts/inspect_phase3_world_placement.py` independently rechecks all **630 returned instructions / 2,668 bytes**, omitted raw blocks, exact table branches, root/slot stores and source limits; main scope separately checks **1,330 instructions / 5,191 bytes**. Derived output: `reports/PHASE3_WORLD_PLACEMENT_RAW_CHECKS_2026-10-03.json`; received source-selection receipt `reports/PHASE3_CMAP_STATE_REVIEW_RECEIPT_2026-10-03.json` is a checked derived artifact, not primary evidence; its scratch original is retained.

## First missing edges / consequences

Positive installed-task world-state input, retainer reset request/clear, acquired resource frame-tail use and a concrete CMap resource/code consumer now exist. This is not complete world scheduling/lifetime closure. Central incoming raw-selector selection, specific descriptor-tag -> CLevel+0x164 dependency/retirement matching, surrounding cleanup helpers, full human mode taxonomy, GOG counterparts and live cadence remain unresolved. `XPC_CLEVEL_UNLOAD_COORDINATION` is narrowed by the deferred request/clear/consumer facts, not closed. Preserve accepted Phase 2 and runtime B0001–B0008 limits. Mandatory current-primary PhysX/save/input/vehicle root checks remain higher-value breadth work than adjacent selector depth.
