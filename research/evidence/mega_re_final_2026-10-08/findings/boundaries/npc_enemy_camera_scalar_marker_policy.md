# CNpcEnemy camera scalar, numeric-bit policy and marker boundary

2026-10-04; sequence52; **STEAM_PC only**. C0173-C0175 / BND-158–160. **VERIFIED** selected conditional original-pointer interfaces, scalar def-use, numeric state transitions and marker/event call endpoint. **STRONG_INFERENCE** camera-relative eligibility/hysteresis interpretation. **UNKNOWN** friendly AI/context/field meanings, live admission, opaque preservation, dynamic-type/lifetime stability, event2 effects/free, runtime cadence and other builds.

## Source, novelty and verification

Independent seven-candidate seq51 review selected NPC_ENEMY_CAMERA_SCALAR_MARKER_POLICY, not warm Player/CItem or CNpcRecord. Contract: scratch/phase4_seq52_npc_contract.md. New discriminator: raw CNpcEnemy class-specific external event adapter and original-pointer scalar/flag/marker policy beyond accepted selector construction. No allNPC/state/command inventory or original bounded-source reopening.

Accepted Steam selector3/H0034 construction, H0202 callback setter, C0132 same-object adapter, C0123/C0124 camera identity and H0195 marker are reused at exact scope; legacy homology references are not new paired proof. Relevant changed canonical references are normalized. No GOG/Xbox correspondence or runtime evidence acquired.

Main personally read decisive assembly, then independently checked **594 instructions/2,092 bytes/21 aligned contiguous windows**, GNU objdump boundaries/bytes, **52 opcode assertions/24 relative calls**, raw type/slots/descriptor/event/dispatch cells, constants, scalar stack arithmetic and x87 status truth cases. `scripts/inspect_phase4_npc_main.py` -> scratch/phase4_seq52_npc_main.json. Fresh one-question producer branch369/1226/14windows and consumer229/870/9windows provide independent checks; counts overlap and are not census coverage. Sources remain untouched; Steam SHA2567a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029.

## 1. Typed creation and actual event/update admission — C0173

**VERIFIED:** selector3 raw005E9590 ->005E76C2 allocates0xE20 and calls0048D5C0 with original nonnull pointer **N**. Constructor saves N, writes00774574 and returns EAX=N. Raw COL00877300/TD008A8BCC names CNpcEnemy; six ancestry PMDs are(0,-1,0): CNpcEnemy, CNpc, CCharacter, CRdObjectModelGame, CRdObjectModel, CRdObject. This is a zero-offset named-class genealogy, not a live immutable-type claim.

Common factory preserves N inESI and selector3 descriptor index through stack+18/EBP restore. Row008BF678 is `{0047F830,0000000C}`; metadata0C is not the emitted event. Actual005E956A calls006BAB80 withECX=N: callback published atN+44, initial event0 seam called. Payload+4C auxiliary is separate. Initialevent0 is not the external event1 producer or recurrence.

Selected own table slots: +0C0047C1F0, +8C00482FE0, +940047D0A0(opaque), +980049EB30, +A4004A1820, +30006BAB20. Own0C wrapper calls accepted006BCB70 without object adjustment; that adapter reloads saved N and calls00402870(N,1,0). The seam's optional0148132C hook runs **before** reloadingN+44. Callback delivery to0047F830 is therefore conditional on the current field, not guaranteed persistence.

Selected callback first calls literal typedCMap013936F0/005D0820; event1 requires full returnedEAX=0. It then obtains N from first stackargument, not assumed entryECX. Current+A4 target004A1820 writes **AL=3 only**, not normalizedEAX3; callback MOVSX-normalizesAL and calls opaque004D51C0(N,event) before event dispatch. Raw map0047F999=1/0047F978=0047F8C6 proves event1 arm; later008A6074 bit31 must clear. Current+8C is reloaded and called withECX=N.

00482FE0 retains N and requires byte+38=0/+61C=3. CurrentAL3 bypasses optional+1BC; opaque own+94 executes before byte+1C&6 and byte+6A4&1 clear gates.00449070(N;0,10000000h) must return **fullEAX0**; only then is current own+98 reloaded and called withECX=N. Raw0077460C gives0049EB30 under the checked class table. Gates are separately timed reads, not one immutable snapshot. Pointer forwarding is proven; transitive callback/vptr/lifetime preservation is not.

## 2. Actual camera-address scalar and local state mechanism — C0174 / BND-159

**VERIFIED selected entry:**0049EB30 retainsECX asESI=N; opaque0047D490 must return nonzero, and byteN+1C&4 must clear. Separate acquiredCGame results **R1/R2** supply raw fields+834FE0=9 and+834FE4=1. They are not established as one stable context/generation.

Selected camera **C=[00BE1EA4]** is accepted zero-offset CSingleton<CCamera>/CCamera. Local lazy allocation may publish0x1AC pointer/null and a vptr; it does not initialize the queried member.0049EB96 **ADD EAX,0C** andPUSH EAX pass address **C+0C**, not a pointer loaded from[C+0C]. Nullallocation still passes numeric0C: no local successful-allocation/safety assertion.

00479DD0 receives originalN and literal3. Raw mode3 cell00479F00=00479E8F selects float32N+5C minus float32[P+4], i.e. **C+10**. Subtraction is stored/reloaded asfloat32 **beforeFABS**, then furtherfloat32 stores/reloads andRET8. Do not replace this with an unrounded real-number distance or infer a unit/axis/camera value writer.

Caller stores returned scalar **S** at itsESP+0C, independently of threshold **T=80** atESP+10. Actual jump0049EBAB ->0049EC8E bypasses alternate scalar producers; no call intervenes through threshold commits. Rawfloat0077229C=80,double007722C8=10,double00771518=100. At first comparisonST0=T,ST1=S.

**VERIFIED normal x87 comparison/status behavior** (not proof that unmasked exceptions cannot interrupt):

| Previously sampled N+69C bit2000 | Exact branch | Ordered outcome | Unordered status outcome |
|---|---|---|---|
|1|80 versusS; FCOMP ST1; FNSTSW/TEST AH,01/JNZ skips|S<=80 clears, including equality; S>80 retains1|retains1|
|0|80+10 versusS; FCOMP ST1; FNSTSW/TEST AH,41/JP skips greater/unordered|S>=90 sets, including equality; S<90 retains0|retains0|

Thus ordered80<S<90 preserves prior bit. IntegerPF is computed by **TEST** on maskedC0/C3; it is not PF copied directly from x87. Both transition commits write modified69C snapshot and earlier unchanged698 snapshot; skipped transitions write neither. A strictS>90 shorthand is **DISPROVEN** at this selected opcode scope. This is a sourcecard/model correction, not regrading any accepted old claim.

The threshold comparison pops T, leavingS.0049ECE3 comparesS versusdouble100 and popsS: orderedS<=100 clears **D8** bits1000/2000; greater/unordered sets them. D8:2000,69C:2000 and laterDC:2000 are distinct fields. No direct scalar-local reread occurs in the checked continuation; laterESP+10 is overwritten with a different bridge value. The marker consumes memory state, notS.

## 3. Subsequent bit consumer and conditional marker endpoint — C0175 / BND-160

**VERIFIED local transport / UNKNOWN preservation:** opaque bridge0049ED32–0049EE15 performs two orfour separate006C5FD0 lookups, repeated manager/handle loads and distinct return-pointer dereferences. Local code does not explicitly addressN+69C/698 or reassignESI/EBX, but that is **not** a transitive no-mutation/type/lifetime guarantee. First post-writer opaque edge0049ED3F prevents asserting that the later sampled bit equals the committed/retained writer value. Expected nonvolatile-register/stack discipline is also a dependency.

Correct aligned read starts **0049EE15**, not0049EE19. Its freshly read69C:2000 selects:

- **Bit0:** clearsDC2000, setsDC04000000/08000000, clearsDC01000000; continues to unreviewed common tail. It does not take the selected0049EE8B call; no whole-function marker absence follows.
- **Bit1:** separate laterCGame results **R3/R4** test raw+834FE0=0/+834FE4=0. If both samples pass, jump directly to0049EE84, bypassing DC masks/698:800. Otherwise setDC2000/01000000, clearDC04000000/08000000 and require freshly read698:800 before that call. Earlier9/1 does not imply later0/0 or one stable phase.

0049EE84 reloads **current**N vptr; +30 target called withECX=N at0049EE8B. RawEnemy007745A4=006BAB20 establishes the class-specific endpoint **conditional on the current table/slot remaining that target**. Same numeric pointer does not prove immutable dynamic class across opaque calls.

**VERIFIED reused marker mechanics:**006BAB20 retains originalECX, writes backD8 and ORs sameN+DC with60 before full-EAX marker predicate00427320. If unmarked, byte+29=80/+2A=0 then actual00402870(N,2,0) call. Alreadymarked skips new byte stores/event2 butDCOR60 already happened. Stop here: event2 delivery/body, marker persistence, later deletion/free/traversal/safety remain **UNKNOWN**.

## Object/data responsibilities and lifecycle

Selected CNpcEnemy is an actor registered by the common factory, not a newly proven exclusive NPC manager. Observed allocation0xE20; constructor max selected+E10 dword yields lowerbound0xE14. Own deleting target004A1860 remains an inherited candidate, not an invoked free edge. Callback+44/type-specific+0C/+8C/+98 connect conditional object work to numeric policy; no once-per-frame/population/order claim. Important fields:5C queriedfloat;38/61C/1C/6A4 admissionbytes;698 auxiliary mask/snapshot;69C persistent bit;D8/DC separate controlwords;29/2A markerbytes. Friendly meanings/lifetimes unknown.

NPC_ACTOR is a new scoped organizing component, not a transfer into PLAYER_ACTOR or fullAI taxonomy. CMap worldpredicate, CDemo bit31, CGame raw context, CCamera address-valued input and OBJECT_DISPATCH marker are concrete cross-subsystem dependencies. No resourceformat/player/CCT/GOG proof is inherited.

## Primary evidence index / reproduction

All1-indexed physical refs below are to untouched inputs/decompiler/steam/DP_full.asm:
- Selector3/type/return:547370–547390,547411–547420;164103–164137; commondescriptor/installation549057–549205.
- Setter/seam/externaladapter:780947–780963;1854–1879;143392–143396;783949–783970.
- Callback/admission/ALleaf:147618–147650;147667–147675;151596–151631;187071–187072.
- Selectedcontext/camera/scalar:183916–183951;140792–140797;140815–140817;140853–140865.
- Exactthreshold/localcontinuation/consumer:184016–184055;184056–184113;184114–184154.
- Markerpredicate/endpoint:43932–43934;780913–780941.
Rawtables/COL/basePMDs/descriptor/map/constants are recorded in main andbranchreceipts; GNU decoder and directSteamPE supply independent checks. Supporting C:96198–96220,98376–98388,119420–119547,92309–92322,434783–434796,312097–312102. Decompiler roles do not override checked assembly.

## Exact residual / reopen condition

NPC_CAMERA_VALUE_CLASS_AND_MARKER_CONTINUITY: require a new typedC+10/N+5C value producer, opaquehook/prehandler/bridge memory-vptr-lifetime preservation contract, concrete laterclass/bit genealogy, directly connected event2 effect, or correlated runtime trace. No moreselectedscalar/threshold/maps/marker/constructors, allNPCstate/handler, generic+50/callback orPlayer/CItem offset scans. CNPCR_INDIRECT_BEHAVIOR and every earlier bounded/runtime trigger remain unchanged. Phase4 active; global reassessment after this substantial gain; no Phase5.
