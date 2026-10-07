# World/resource retirement: marks, retained references and missing generation join

**Date:** 2026-10-04. **Phase 5 batch 1.** Steam primary; paired PC raw-table correction only. C0180–C0182. Accepted C0114/C0119/C0120/C0129/C0130 are exact-scope anchors, not reconstructed or awarded duplicate discovery credit.

## Contract and result

Selected `P5_WORLD_RESOURCE_RETIREMENT`: match a specific descriptor/tag/CLevel+164 to world retirement and deferred resource deletion, or expose the first missing join. New sources are the specific state6 retirement call preceding state7, typed CLevel cleanup/deleting/rebinding and packet reference transport. Constructors/parsers/getters, generic offset/global scans and bounded raw-selector incoming scans are excluded.

**VERIFIED positive gain:** world state6 calls a category-filtered retirement-mark broadcast; the typed CLevel interface exposes mark, runtime cleanup and deleting operations; common cleanup clears160/164 rather than releasing their old pointees; resource pointers can also escape into a scene packet. **UNKNOWN/NOT_READY stronger join:** the selected named level instance's word2C/tag/resource generation, actual retirement selection and final packet use are not matched. This is not safe unload, a runtime failure, or static-corpus absence.

## State6 marking versus state7 resource requests — VERIFIED

At `005D44AC/005D44B1`, the selected event1 state6 continuation supplies actual static CMap013936F0 to `005E0F50`, then eventually stores numeric state7 at `005D44DA`. `005E0F50` reacquires application active-manager root00BD7670 for each of eighteen `006C5DF0(2,K,0)` calls, with K in this exact order:

`6,8,9,10,11,12,15,16,17,14,13,23,22,24,25,26,27,28`.

The broadcast passes its current object and all three arguments to `006C7480`. Raw selector2 targets006C74E0: `00402D90(0)` returns object+2C; MOVZX reads that word and compares it with K. This is a concrete **16-bit object category/filter field**, not a resource tag, selector-factory ID, handle generation or proof that a named CLevel instance equals6.

For accepted iterator-selected objects passing that predicate, `006C5DA0` reaches their current virtual+30 only when manager+18E0 !=2; state2 instead performs separate flag calls. The typed Steam CLevel table007798AC has+30=006BAB20. Accepted marker mechanics set29/2A and dispatch local event2 conditionally, not delete/free. Thus **conditional typed-interface compatibility is proved; actual selection of the LEVEL-created generation is not**.

After broadcasting, `005E0F50` switches ECX to CMap013936F0 for005E0D30, then switches to distinct143C078 and tail-jumps005E6E30. No common receiver is inferred from the decompiler's missing ABI.

Accepted state7 subsequently calls CRdData tag-force helpers, clears64 CMap slots, and commits state8. Accepted frame tail calls active-manager retirement006C7070 before CRdData pending release006B2A40. These static orders do **not** join the separate raw world-selector occurrence to that frame or prove all matching actors/packets retired before resource callback deletion.

## Available typed cleanup and deletion — VERIFIED at code/interface scope

Steam raw RTTI names CLevel at007798AC. Observed consecutive code-pointer extent33 is a diagnostic, not a universal interface definition. Material slots:

- +00=005CF4C0: saves this, calls005CCC50, tests deletion flag bit0 and conditionally passes the same allocation to0074E82B; returns the saved this.
- +08=006BCB20: same-this runtime cleanup reaches006BE220.
- +30=006BAB20: retirement marker, not destructor.
- +28=006BD4C0: accepted packet producer can copy this160/164 into packet84/88.

Available deleting chain: `005CF4C0 →005CCC50 →0045FAF0 (tail00402DA0) →006BCB20 →006BE220`. The destructor separately sends this+18C4 to006645C0; it restores the original this for the base teardown. Base teardown changes the current table before calling the common cleanup. Do not describe all subsequent virtual operations as invoked through the original CLevel table.

`006BE220` initially writes zero to same-this160 and164 at006BE22C/006BE239 **before** its helper/auxiliary cleanup calls. It does not read either old resource pointer to submit it to a release helper in this selected body. Later auxiliary frees/reset calls concern separately loaded fields, not the discarded160/164 values. This proves a **local retainer-clear contract**, not ownership relinquishment, descriptor refcount decrement, pointee destruction or safe last use.

Concrete rebinding `006BE6E0` saves this, calls006BE220 on that receiver at006BE720, then writes supplied resource arguments into same-this160/164 at006BE72B/006BE737, before testing the published fields. Available cleanup is therefore **actually called by this setter path**, while invocation of the full CLevel deleting chain on a named LEVEL instance remains UNKNOWN.

The accepted packet-producing interface copies160→packet84 and164→packet88 at006BD78F/006BD7A4. Pointer equality is a transfer, not a new owning reference or packet freshness. Clear/rebind of the actor slots does not clear already published packet slots. Matching last use must include those consumers and their resource generation.

## Specific legacy correction — VERIFIED / DISPROVEN

This Phase5 typed-table check exposed a primary contradiction, not a new Phase4 closeout audit:

- Steam CLevel007798AC: **+4C=005CCDD0**, **+50=006C0620**.
- GOG CLevel0077989C: **+4C=005CCEA0**, **+50=006C0130**. The inherited006C04D0 is actually at+58, not+4C.
- Steam common setter actually loads `[current_vtable+4C]` at006BE978 and invokes it at006BE97B. Conditional on current table still being the named CLevel table, this maps to005CCDD0—not006C0620.

C0047 and the slot-based route asserted byC0086 are **DISPROVEN as written**. BND-026/BND-063 and H0209's same-slot basis must not supply that route. Historical findings remain intact with this explicit superseding qualification. Existing independently described helper-body transformations/H0210 are not disproved by a table-index error; their acceptance is not renewed here, and they no longer establish the false setup-to-attachment virtual join. No new GOG body homology is promoted. Accepted C0114's same-pointer160/164 stores and resource retention remain valid independently of this false locator.

## First missing edges and successor policy

1. Named attach005DCA50 supplies opaque second argument0 to its lookups, not a demonstrated descriptor-tag2E or i+18 admission. Its low-byte local index is not itself tag identity. Need specific typed admission-to-descriptor generation and matching returned wrapper, not another lookup/ctor scan.
2. Need the particular object's category-word writer/current value and actual selector/marker/deleting occurrence. Table compatibility alone does not prove that state6 selects it.
3. Need resource/packet generation and final use versus deferred callback deletion. Independent CMap clear, actor clear and frame-tail order cannot supply that invariant.
4. Need live cadence/concurrency traces for measured unload safety, and separate GOG lifetime evidence beyond raw tables.

Record the missing join and move globally to another strong Phase5 seam rather than exhaust CLevel internals. The world question remains OPEN_PHASE5_CARRY_FORWARD, not solved, blocked after three attempts, or absent from the corpus.

## Primary verification

- Main `scripts/inspect_phase5_world_retirement.py` → `scratch/phase5_world_retirement_primary_20261004.json`:270 instructions/941 bytes checked against Steam PE; selected category table/ABI/root/reset/mark/state7 joins.
- Main `scripts/inspect_phase5_clevel_lifetime.py` → `scratch/phase5_clevel_lifetime_main_20261004.json`:273 instructions/1047 bytes, exact same-this cleanup/setter/packet windows and both typed raw tables. Overlap/reused instructions receive no additive novelty credit.
- Steam ASM: state6 call/state7 store525230–525247; category broadcasts539153–539244; broadcast794665–794701; predicate796074–796106; accessor2198–2200; cleanup785620–785622; setter785953–785966 and786120–786123; deleting519848–519859; destructor516924–516951; runtime cleanup783919–783942.
- Returned bounded Opus branch: `scratch/phase5_world_clevel_branch_20261004.md`; its primary results personally checked at the important joins. Derived agent reports are locators/receipts, not higher-priority evidence than bytes.

No supplied evidence, game binary/asset, production source, hook, patch or runtime state changed.
