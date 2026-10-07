# Fishing typed roots, conditional update and new line retainer

**Date:** 2026-10-03. **Batch:** Phase3 sequence21, FISHING_CALLBACK_OWNER. **Outcome:** ADVANCE. C0131–C0133; BND-105–108. **New build scope:** Steam.

## Contract / repair / reuse

Actual declaration: `scratch/phase3_fishing_seq21_contract.md`. Sequence20 corrected my prospective00780354 interface mistake to the already accepted **007802A4 CFishingPerson** table, without game claim/counter/Phase2 changes.00780354 is a floating constant referenced by the constructor, not a proved table. Cancelled planning and original sequence19 action remain history; see `reports/PHASE3_FISHING_SOURCE_REPAIR_SEQ0020.md`.

H0023 supplies accepted selector43/type/construction only. The generic registration/callback seam and object-phase spine are reused, not rediscovered. H0212 supplies the accepted attachment-aware transform helper body at its proven shared-actor scope. No paired field transfer or new GOG callback/root map. CMustache lifecycle and generic actor+50/marker/retirement/callback-seam censuses remain closed. Main-only research; no minigame algorithm walk.

## Typed acquisition and distinct roots — VERIFIED

Reused selector0x43 branch allocates **0x858**, passes that same pointer to00613960 and publishes **CFishingPerson007802A4**. Exact RTTI and selected interface distinguish the actor from its helpers.

New selected constructor continuations:

- **CLayoutMng**: allocate **0x408**, call0060F110 with argument **8**, then publish its returned pointer at **global01470678**. Raw own table **00780224** names `.?AVCLayoutMng@@`; constructor retains its input and writes that vptr. Configured count lives at+404; selected construction creates CLayout members through004592B0. This is a separately allocated layout organizing receiver, **not CFishingPerson this, a CSingleton getter, or a CCar alias**.
- **CFishingLine**: allocate **0xA04**, call00612F00 and retain the returned pointer at **CFishingPerson+0x83C**. Raw own table **00780284** names `.?AVCFishingLine@@`. Constructor initializes selected fields through+A00, including numeric state+9CC.
- **Actor use alias**: installed native callback00619FD0 publishes its **first stack argument** at **01470670**. Its type is proven from the successful selector43 result/callback genealogy. This is a callback receiver alias, not independent allocation, sole owning root or universal uniqueness proof.

**UNKNOWN:** actual helper/global clear/free coordination, repeated-instance publication semantics, exclusive ownership and complete helper algorithms. Own deleting targets are mechanically available (layout0061A1C0/line0061A220), but actual lifetime invocation has not been inspected in this batch. The accepted CFishing selector38 controller remains a distinct seed, not silently merged with CFishingPerson or CLayoutMng.

## Actual installed callback / conditional frame position — VERIFIED mechanics

Factory selector43 uses descriptor **008BF878 = {00619FD0,00000018}**. Common factory continuation installs that callback at the successful actor's+44 through006BAB80, with accepted registration and immediate event0 mechanics. Native callback00619FD0 obtains actual this from its first stack argument; **event1** maps to its default arm0061A0AE, which forwards the same actor as ECX to **00619D40(event1,context)**.

CFishingPerson's exact own **virtual+0x0C** is **006BCB70**. The accepted active-manager object phase2 conditionally selects this slot. New raw class-proven adapter path:

1. same actor -> **006BF550** attachment-aware transform helper, result stored at actor+1FC;
2. same actor ->006C27D0;
3. same actor as ECX and first stack argument ->**00402870(event1,0)** ->installed00619FD0;
4. continuing same-actor post-callback helpers006C3F20/006C36D0.

This is a positive **conditional static phase2 animation/transform-before-fishing-update** connection. Actual selection, once-per-frame rate and all-mode behavior are not proven. It does **not** identify the incoming dispatcher for the separately bounded actor **virtual+50** question: the new source is an independent typed+0C adapter/direct helper edge, not a repeated offset scan or a reopened old generic source universe.

00619D40 requires successful current-player handle lookup and actor+81C==0. Its event1 branch resolves the player again and takes a tested floating comparison branch to **00614F00 with the same CFishingPerson ECX**. The alternative invokes that actor's virtual+30 retirement marker and returns. Do not assign a friendly health/death meaning or silently exclude unordered floating values.

00614F00 reads that actor's **CFishingLine+9CC**, uses private actor+618 state0..20, and conditionally marks/releases an opaque handle-dependent auxiliary result. Only entry/gate/receiver boundaries were examined, not its629-callee full algorithm or state taxonomy. CLayoutMng01470678 also has a concrete selected **native event18** use in00619FD0, calling006107E0 on that exact acquired layout receiver under explicit actor/private/player guards. Event18's full producer/frame position remains UNKNOWN.

## New CFishingLine -> CThrowLure retainer discriminator

**VERIFIED C0133:** typed CFishingLine constructor00612F00 receives the selected+83C allocation as this, calls the **already proven CThrowLure creator00627240**, and stores its returned pointer at **line+8**. Reused creator's return00627748 is **EAX=ESI**, the accepted same CThrowLure pointer, **not a packed handle**. Selected constructor then dereferences that result's object fields. No registration/marker/retirement census or cleanup body was repeated.

This is new primary **class-discriminated storage/retainer** evidence. It genuinely satisfies the documented `CTHROWLURE_INDIRECT_CLEANUP` reopen trigger beyond the old direct dispatcher/global/marker/free source matrices. The old matrices and accepted double-registration C0077 mechanics are unchanged. The previously unvisited **CFishingLine+8 / own deleting-to-cleanup** source is now eligible for a future bounded ownership batch; no such cleanup analysis was performed here.

**UNKNOWN:** CFishingLine+8 retirement/null/free/reuse coordination, CFishingPerson+83C teardown, H1 cleanup, self-link repair, actual invocation and B0005/B0003 runtime aftermath. A parent-held pointer does not prove safe lifetime or exclusive ownership. Bounded CMustache parent/child work is unrelated and remains closed.

## Primary evidence / reproduction

`python3 scripts/inspect_phase3_fishing_owner.py` corroborates **582 instructions / 2086 bytes**, three raw types, selected callback/event-table values and13 relative calls. Derived receipt: `scratch/phase3_fishing_owner_primary.json`. Source PE: `inputs/binaries/steam/DP_STEAM.exe`. No supplied evidence or production writes.

- Accepted typed allocation with new selected genealogy: `inputs/decompiler/steam/DP_full.asm:547839-547848`, `:549064-549075`, `:549189-549195`; callback setter/seam `:780947-780963`, `:1854-1879`; constructor vptr `:596485-596489`.
- Separate layout/line allocation/publication: `inputs/decompiler/steam/DP_full.asm:596586-596614`; typed helper construction `:591298-591348`, `:595728-595761`.
- Callback publication/event default/update: `inputs/decompiler/steam/DP_full.asm:603222-603232`, `:603275-603281`, `:603054-603068`, `:603160-603187`; updater selected entry `:597941-597980`.
- Typed actor-phase adapter: `inputs/decompiler/steam/DP_full.asm:783949-783983`; reused phase selection `findings/boundaries/object_virtual_phases.md` and shared transform body `findings/boundaries/shared_actor_attachment_interface_correction.md`, H0212. Original ACTOR_SLOT50_DISPATCH bound is unchanged.
- New line retainer and reused creator return: `inputs/decompiler/steam/DP_full.asm:595755-595761`, `:618605-618615`; accepted creator identity/registration `findings/boundaries/cthrowlure_registration_invariant.md`. Return-edge check only, no old matrix reconstruction.
- Supporting exported source: `inputs/decompiler/steam/DP_decompiled.c:337989-338230`, `:335222-335298`, `:337669-337719`, `:341530-341638`, `:436264-436282`. Raw callback tables/fields override expanded decompiler shape where needed.

## Phase / review consequence

Selected fishing organizing acquisition/use and conditional phase2 callback/transform placement are positive. Carry **FISHING_LIFECYCLE_CONTINUATIONS** for layout/global/line/controller/event18/teardown/GOG/cadence. Reopen only on a new typed caller/helper lifetime source, not generic callback or all-state enumeration.

Parent seq20 counters0/0/3 +this ADVANCE reaches productive4. Required strategic review must assess **all21 families** and compare at least5 architecturally distinct candidates before another RE batch. New line retainer is a genuine viable candidate, not an automatic warm-context continuation. Phase3 still lacks independent effects organizing/use and broader mode/scene/query/event/UI/lifetime edges; no closeout or Phase4.
