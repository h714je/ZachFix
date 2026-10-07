# Typed model buffer/packet retirement versus replacement and scene borrowing

**2026-10-04; STEAM_PC; Phase5 batch8 / sequence80; C0199/C0200.** Contract `scratch/phase5_model_seq0080_contract.md`. C0130/C0143–C0145/C0181 are reused at exactscope; ownslot00402E50/basepacket148 andselected1E4/1E8 cleanup are new interpretedscope. Old00402DA0 localteardown/H0189/CThrowLure delegation are not rediscovered or globallyreopened.

## Result and scope

**VERIFIED local mechanisms:** invoked rebindcleanup conditionally passes currentstate/matrix buffers to a concrete deallocationboundary then clears theirfields; typedmodel availabledestruction reaches a basepacket deallocation/clear before currentvirtual48(0). Rebind/availabledestruction/capacityreplacement/scenevectorerase remain different operations. **STRONG_INFERENCE:** modelretains locallyallocated buffers/packet while scenevector borrows packetpointers. **UNKNOWN:** actualtypeddeletingoccurrence, currentfield/allocation generations, opaque vptr/fieldpreservation, oldpacket invalidation, scenevector detach/finaluse, successfulsafe release. Not an observed use-after-free or wholeownership closure.

## Selected buffer deallocation versus merely clearing resources — VERIFIED

AcceptedC0181 initialM160/164 zero-stores andactual006BE6E0→006BE220 are reused. New selected006BE293–006BE2F4:

- guard samplesM1E4; nonzero freshlyloads thefield, stages itsvalue andcalls0074EE0B at006BE2AF; afternormalreturn storescurrentM1E4=0 at006BE2BA;
- independentlyguard samplesM1E8; nonzero freshlyloads/stages itsvalue andcalls0074EE0B at006BE2E0; afternormalreturn storescurrentM1E8=0 at006BE2EB.

Complete **0074EE0B is11 bytes**, a prologue/restored-EBP/**tailJMP0074E82B** at0074EE11. It preserves the incomingstackargument; accepted0074E82B is the existingdeallocationboundary. This is concrete invocation of thatboundary on the selectedloadedbufferpointer, **not merely pointerclear**, pointee destructor dispatch, successfulfree orsafe last use. No CRT/free internals expanded. Separate guard/argument/currentfield-clear observations are not automatically oneallocationgeneration.

The selected ordinarycleanup then calls shortmaskhelpers:004D7C60 readsM20 bit02000000 asboolean;004028B0 returns maskedD8/DC pair. Later00417DE0 with(0D,0) clears bits0/2/3 in **M+D8**, leavingDC mask0 unchanged. **These are not packetvalidM138/M13C.** Localmaskreset doesnotestablish packetinvalidating/detaching. Remainingcurrentvirtual64 calls(0..2) and00718F00 effects are opaque here; no transitive no-invalidation claim is made.

## Actually invoked rebind/replacement — VERIFIED selected genealogy

006BE6E0 preservessavedM; call006BE720→006BE220 happensbeforeacceptednew160/164 publication. Underbothnonnullresourceguards, selected repeatedresourcegetter calls separatelysupply quantities:

- A0-stride allocationrequest→0073A8DC→tailallocationboundary0074EEEB→resultstoredM1E4 at006BE7AE;
- independentlyacquiredquantity/40-stride allocationrequest→sameadapter; conditional00401D90 vectorconstruction usesoriginalallocationpointer, thenselectedpointerpublicationM1E8 at006BE827;
- latercaller supplies M1E4/M1E8 andbytecounts to0074E760 at006BE843/85E, thenacceptedselectedevaluation.

The11-byte allocationadapter andspecificpublication/callargument transport are checked; allocator/constructor/memset internals andall-pathsuccess are not reconstructed. Overflow/request/count/null anddifferentgetterresults remainbounds; thisdoesnotproveequalcounts, safezerocount/allocationfailure orcurrentfreshpose. Rebinding invokes oldbuffer deallocation/fieldreset and thenselectedreplacement **without needing objectdestruction**.

The explicit006BE220 body has no directM148 store/deallocation, and itsshortmaskhelper isD8/DC. This finite fact doesnotprove thepacket survivesopaquevirtual/tailcalls orthat cachevalidity ispreserved. Firstoldpacket→newresources/cacheinvalidation edge remainsUNKNOWN.

## Available typed destruction and packet-before-query order — VERIFIED

RawCRdObjectModel0076E704 slot0=00402E50. It savessameM, invokes00402DA0 at00402E53 andforflagbit0 passes savedM to0074E82B at00402E60. This is available ownslot implementation, not anactualselectedCMenu/task/model lifetime retired bymanager.

00402DA0 installsmodelvptr, conditionallydeallocates/clears separateM27C, invokes006BCB20 onoriginalM (acceptedcommoncleanup006BE220 continuation), andhandlesinline1B4/178. It then rewritesoriginalM vptr to **CRdObject0076E6B4** at00402E30 andcalls006BA850 at00402E36. Do notmap latercalls through theoriginalmodel orCLeveltable.

Complete006BA850:

1. guardsM148, reloads/stagesit andcalls0074EE0B at006BA875, nowprovedtailtoaccepteddeallocationboundary;
2. afternormalreturn clearcurrentM148 at006BA880;
3. bothnullskip andnormalreturn convergeon currentvptr/slot48 load and **virtual48(0)** at006BA897.

RawCRdObjecttable48=004029B0, theacceptedoriginal-object querymember attach/detach interface. Conditionalon thepost-deallocationcurrentvptr stillbeingthatbasetable, itszeroarm conditionallycalls006BC060(oldnode,M) andclearsM14C. This is concrete detach-request/fieldclear transport; actualremoval remainsSTRONG_INFERENCE atacceptediterator-adapter scope. **Packetdeallocation/clear precedes thatcall**, notafter. Currentvptr reload isafteropaque deallocation; unchangedtable/lifetime andactualnode/generation requireseparateevidence. Detachingquerymembership isnotdetachingalreadyborrowed scenevector entries orjoiningdraw consumers.

## Borrowing, replacement and first missing edges

ReusedC0130 capacitygrowth00405E40 frees/replacesM148, thenconditionalpacketpopulation. ReusedC0129 scene63A8 storescopiedpacketpointervalues, prerendererasesentries, andshutdown deletescollectionshell. Those are neitherpacket-owningreference transfer nor a provedgeneration fence. Availablebaseteardownnowadds a distinct packetdeallocationroute.

**UNKNOWN firstedges:** actualsameM deletingoccurrence; buffer/packet guard-argument-clear generationmatching; currenttable preservation; oldpacket validity/invalidation acrossrebind/virtual64/00718F00; submittedpacket/currentresource generations andfinalscene/draw use beforepacket/resource retirement. No clearing M148 orquerydetachcall removes an alreadycopiedvectorpointer byitself. NotREADYfor safelastusewithoutnewtypedinvalidation/detach/completion source orcorrelatedtrace; no indefinitewidening.

## Narrow cross-family adapter qualification, not audio re-audit

New typedmodel buffer/packetarguments justifyexpanding onlythe11-byte0074EE0B adapter. OriginalC0179 finiteaudioB13 loop/caller scope ispreserved. Thehelper's formerlyopaque firstedge isnow a concrete tailforward to0074E82B, so its alreadyacquired argumentcalls arecompatible with deallocation-boundary attempts. **B13 pointee type/allocationgeneration/destructor/success/lastuser remainUNKNOWN**; noaudio finalizerdispatch/CRTatexit/sharedquiescence/GOG re-audit orlooprediscoverycredit. Originaltext/history retainedwithqualifiedaddendum.

## Primary verification and limits

- `scripts/inspect_phase5_model_buffers_main.py` → `scratch/phase5_model_buffers_main_20261004.json`: **163 instructions/551 bytes**, selectedbufferfree/reset/rebind andfivefixedsmalladapter/maskbodies totaling88bytes, rawmodeltable.
- `scripts/inspect_phase5_model_packet_main.py` → `scratch/phase5_model_packet_main_20261004.json`: **80/287**, typedownslot/ordinary/basepacket andsameMcommoncleanupjoin, model/basetables.
- Freshsingle-assignmentOpus packetbranch report/checker/originalreceipt preserved under `scratch/phase5_model_packet_branch_seq0080_*`; replayedtoNEW `scratch/phase5_model_packet_main_replay_seq0080.json`: **222/771**,16contiguousspans/72anchors/21rel32targets/4rawtypes. Mainimportantreturnedjoins personallyverifiedabove. Flag0 stillinvokesordinarybuffer/packetcleanup; flagbit0 onlygatesouterMdeallocation. Bytecounts overlap/reuse andarenotadditivecoverage orsemanticclosure.

MainASM: buffers785637–785669; rebind785953–786050; adapter949420–949424/allocation942378–942382; D8/DC helpers1885–1889/27507–27513; availablemodelordinary2206–2247/ownslot2253–2263; basepacket780627–780651; commoncaller783919–783942. ExactVA/line/bytes/sourcehashes inreceipts governshorthand.

NoestablishedownslotGOGhomology; H0189 ordinary mappinghaslegacy-reference/scope flagsandisnotnewdeepGOGproof. Runtimeabsent/Xboxcomparative. All70+18guards/K0022-K0023history retained; noPhase4 re-audit, Phase5closeout orPhase6. Suppliedbinaries/evidence/assets/production/runtimeunchanged.
