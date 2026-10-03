# Current Research Status

## Current phase

`PHASE_4_ACTIVE`

## Phase scope

Phase4 subsystemdeepreconstruction under currenthuman authorization; Phase3 closedatindependentlyacceptedconditional-static lifecycle/root scope. Reuseexactprior scopes, reduceprimaryproducer→mechanism/state→consumer gaps, periodicallyreviewglobalfrontier. NoPhase5 untilnormalPhase4closeout+independentfresh-context acceptance.

## Evidence available
- Steam PC binary and complete zero-error Ghidra export pack.
- GOG PC binary and complete zero-error Ghidra export pack.
- Xbox 360 PAL XEX plus a comparative recompilation/static corpus.
- 14,333 DPserial assets across multiple resource families.
- 81 prior-research files, including explicit corrections and unresolved queues.
- No raw runtime traces in `inputs/runtime/`.

## Build baseline
- `STEAM_PC`: `DP_STEAM.exe`, SHA-256 `7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029`, 10,322 exported functions.
- `GOG_PC`: `DP_GOG.exe`, SHA-256 `c954c2e3b205d444b0fc3649adf4bd8462a7dfe73d89599e24a7e17a129415c2`, 9,930 exported functions.
- `XBOX360_PAL`: `default.xex`, SHA-256 `a46bfbfbb1783728de497f9f5425ab3177e9d28bedd40560f4b275a8ff704a66`, comparative only.

## Current checkpoint

`checkpoints/2026-10-04_seq0055_phase4_post-car-model-global-review.md`

## Coverage snapshot

Functions: 20,252; classified 4,989 (24.6%); ownership UNKNOWN 15,263; named 3,274.
Function states: {'UNKNOWN': 15263, 'VERIFIED': 3903, 'STRONG_INFERENCE': 1086}.
Claims: 177; states {'VERIFIED': 150, 'STRONG_INFERENCE': 17, 'DISPROVEN': 8, 'UNKNOWN': 2}.
global 192; vtable 1024; class 391; homology 272; boundary 163; subsystem 22; resource_type 34. Legacy reference cells: 5,542. Metrics are not semantic closure.

## Strong current architecture slices

- Phase4 C0176/C0177: originalCObjectCar/firststackargument>=0/localA-B ->58/68/78 copies ->selectedbase98 destination/rotation/directfloat/multiply ->opaque-preservationboundary ->explicitcurrentstateevaluation/acceptedconditional1E8 interface. Packetvalidity/repopulation/latestpose/actualframe remainUNKNOWN. findings/boundaries/cobjectcar_model_base_cache_production.md

- Phase4 C0173-C0175: typedCNpcEnemy class-specific event1/current8C-98 ->separate raw9/1/typedcameraC+0C address ->roundedscalar/inclusive80-90/unordered69C2000 ->laterfreshbit/context/aux/current30 marker/event2. Opaquebit-vptr-lifetime/value/cadence/GOG limits explicit. findings/boundaries/npc_enemy_camera_scalar_marker_policy.md

- Phase4 C0171-C0172: independenttypedCItem selector0x14/payloadP0/descriptor4C/native0->4A8; subsequentfieldloads/DC-row/formatbuffers->distincttypedCRdData actualreturns->sameobject160/164 before nullgates. N0equality/row/name/type/success/owner/lifetime/GOG/runtime andPlayerownership UNKNOWN. findings/boundaries/citem_factory_value_resource_binding.md

- Phase4 C0169-C0170: typed CPreserve operation4/live CGame staging -> same-context gated operation6 -> retained pointer/count WriteFile boundary; builderstate1/write-dispatchstate2, admission/generation/success/schema/lifetime/GOG remain unknown. findings/boundaries/save_writer_staging_write_policy.md

- Phase4 C0167-C0168: typed selector46 CObject same-object +444 desired state -> local descriptor+416 -> paired resource accessors/same-object006BE6E0 -> current+12 commit. Context/value/range/pair/ownership/last-use/setup-success/GOG/runtime remain unknown. findings/boundaries/world_object_representation_policy.md

- Phase4 C0165-C0166: typed CMenu U+14 raw99/100/101 -> CFade request3 immediate state4/data -> independently reacquired predicate7 -> conditional101 common-tail/later CCamera mask1-clear/root-reload/mask4-set. Alias/producer/child/free/cadence/GOG limits remain. findings/boundaries/cmenu_fade_predicate_camera_policy.md

- Phase4 C0162-C0164: typed CDemoMovie request0x40 -> distinct callbacktask -> conditional private-state0/3/R-byte gates -> CFade NULL/request7 state/data and receiverless00BE1EAC latch -> acceptedconsume/clear/drawguard. F_A/F_B/F_C identity/lastwriter, +198snapshot versus+178poststep, AL-only/untestedreturn, eventdelivery/free/GOG/runtime limits explicit. findings/boundaries/presentation_movie_fade_latch_chain.md

- Phase4C0160/C0161: customtypedCObjectCar005ED660 result/selector0x53(decimal83)->actual+44 callback005C92C0->conditionalevent1/postcallselector->sameactorstate1/1FD8=2 numericcontrolwrites. Opaquehelper/activation/own-event1finaleffects/owner/free/GOG/cadenceUNKNOWN. findings/boundaries/cobjectcar_callback_control_chain.md

- Phase4C0158/C0159: newtypedCMenu40/28/floatgate/ID80→special85 pointer→actualCGame983C4 SUB999/signedclamp, underexplicitpopulation/98E48inequality guards. Laterdispatch/liveavailability/fullinventory/aux/refresh/lifetimes/GOG UNKNOWN. findings/boundaries/cmenu_numeric_game_slot_commit.md.

- Phase4C0155-C0157: typedCSound/assetSND_SE4E→SE_LIST64/keyPLSE066→inlineP=node20→actualmain/core/statusnumericrecordchain andconditionalopaque bankconsumer. T/K/Q distinct; nodefields/name/bank/API/success/lifetime/GOG remainUNKNOWN. findings/boundaries/audio_descriptor_request_record_chain.md.

- Phase4 C0152-C0154: typedstaticCLoadThread01481130 actualinitializer-invokedOSargument/slot4 route, distinctpackedqueue/directslot andgetter-derivedCRdData/control8-to-descriptor2E/status2/word2C commit. Publicmode0 isdirectnotqueue; normalreturn/pendingclear isnotloadsuccess; fullID/safeconcurrency/free/GOG/runtimeUNKNOWN. findings/boundaries/resource_worker_typed_handoff.md.

- Phase4 C0149-C0151: typed4 CPhysicsThread contexts/CNArray samevector producer/6-worddedup/worker/null protocol andavailableomitted0040B630 start→sameOSargument→worker. Actualincomingstart/quiescence/SDK/capacity/GOG/cadence remainunresolved; findings/boundaries/physics_context_vector_activation.md.

- Phase4 C0146-C0148: selectedSteam CSaveData/Sysutil queuedcallback protocol →file-size-basedread/staging00BE5EF0 →typedCPreserve header andseparatedirectlive27+1recordcommit→reusedconditionalCPlayerblock consumer. No fullReadFile validation/automaticcreationchronology/GOG/free; findings/boundaries/save_disk_staging_record_chain.md.

- XPC has a positive typed resource portfolio: archive/lookup -> callback-created 0x10 CRdPicture -> CRdData record +0x1C owner -> named LEVEL.XPC/CLevel +0x164 retainer; callback event 1 supplies a typed deleting interface. CLevel unload coordination remains unproven (C0114).
- `00BD9E48` is a class-proven 0x0C CRdMovie singleton shell; paired title/control callers connect movie/source/fallback setup, sample/surface update, and explicit same-root graph/helper cleanup. This serves MEDIA_PLAYBACK, not the general AUDIO root; singleton free caller and full mode placement remain unknown.
- CShop selector-0x34 results have verified setup/packed-handle retention in paired command and native-task paths; state 11 polls existence before retiring the task receiver. Raw 19-slot CShop tables and conditional deleting mechanics are mapped; a concrete installed callback, event-one update, conditional CShop retirement arm, and generic-manager class deleting endpoint are now positively connected (C0112). Live selection/state reachability remains unknown.
- The paired outer application loops establish static Win32 message -> eligible idle gate -> timing -> `00401A70` frame-root ordering. Complete loading/pause/menu/cutscene temporal behavior remains unresolved.
- `01481358` is a static `CRdHandleUtil`; direct registration, current-handle lookup/release, and selected active-object retirement mechanics are mapped. The active-manager identity is now raw-type-proven CSingleton<CRdSceneDraw> (C0116); actual cached-free invocation and non-current handle/reuse behavior remain unresolved.
- DPserial archive stream/extraction and CRdData callback registration mechanics are primary-backed. XMD/XPC/XPC2 have selected construction/parser/consumer anchors; full format factories, ownership, and consumers remain incomplete.
- Paired `0045C9F0/0045CA20` are verified narrow CMessage code dispatchers: they lazily root `00BDA000`, use the `+0x44` record resolver, and execute code-specific control flow. Paired CMessage constructors clear `+0x44`; direct constructor/singleton-local table-writer provenance is bounded. Raw resolver callsites forward ECX/EBX rather than the singleton result in EAX; receiver-to-singleton alias, the later loader, code semantics, broader native-UI ownership, and cadence remain unresolved.
- CScenedemoPostEffect is a paired lazy `0x1224` singleton rooted at `00BDBCC8`; its cleanup dispatches and clears generic active-object pointers from a 0x32-record collection plus `+0x2C8/+0x2CC`. Paired vtables expose a vector deleting destructor, but direct root writers only publish allocation/failure and no direct static deletion caller is exposed. This direct caller matrix does not identify a CThrowLure receiver.
- C0115: in-place static CMap 013936F0 is the named-level attach receiver in all five direct sites per PC build; +0xA3F98 is 64 retained pointers at 01437688. Raw constructor/RTTI and registered embedded cleanup establish acquisition/type without claiming full world update/level teardown.
- C0116: 00BD7670 is the application use root of cached CSingleton<CRdSceneDraw>; own one-slot deleting tables differ from CRdObject 19-slot consumer tables. Shutdown cleanup/app-root clear is not fuller destructor/optional allocation free; actual cached-free invocation UNKNOWN.
- C0117: both numeric bit-31 context/order arms converge after the object repeat loop on 00401440 and the same-root movie call at 004016FC; helper-present return is not completion. Full world/mode/cadence/lifetime semantics remain UNKNOWN.
- C0118: independent CSound0138A6E0 / CSdCore0138A6E4 / CSdMain00BDBCC0 audio receivers have typed acquisition and conditional startup-installed task use; CMap+0x10 supplies an auxiliary-work guard. Core backend globals also have distinct static014B0400 constructor receiver; exclusive ownership/free/cadence UNKNOWN.
- C0119/C0120: Steam installed-task event1 paths produce CMap numeric state3/4; state7 CRdData tag-counter requests precede64 retained-slot clear/state8. Acquired CRdData pending release is called at00401C98 after object/media work; requesthelper is not immediate unload. Rawselector00474830 routes centralworld/presentation consumers but incoming selection remains bounded/UNKNOWN.
- C0121: Steam00BDA0C8 acquired0x67EDC CSingleton<CPhysicsCore> has rawtype/startupSDK alias/object-phase use/invoked sessioncleanup. Actual cachedfree/SDKfree/worker activation remain unknown. C0122 corrects synchronous first-half beforephase7, second-half after; broad14->7->8 order retained.

- C0123/C0124: typedSteamTSiHolder<CInput>/CCamera acquisition, actualaggregate/pending/live transfers, committedaxis camera use andconditionalobjectphase5; controltablesarenotsamplebuffers. CPlayeraliasSI; free/initializer/mode9/GOG/cadenceUNKNOWN.

- C0125/C0126: acquiredsharedCGame00BDA004/0x838E30 ownsselectedbackup/live45CC0 copies; private6roundtrip andtypedCPlayercreation/event0/record-vector/state40 consumer. PrecopyORoverwritten; fulldiskimage/temporal/lifetime/GOG unproven.

- C0127/C0128: pairedstaticCCar008C29F0/two398CLayout/request/task use anddistinctCObjectCar own-event5 selectedboundary. H0007 corrected00555B50/00555C20; GOG129exportfragment/actorcallback/frame/owner/free limitsretained.

- C0129/C0130: Steamscene63A8/18-byte pointervector lifecycle; modelpacket148/typedCLevel-CPlayer44/28 producer and84/88 resourceconsumer. Actualquerymembership/finalGPU/GOG/cadence unresolved.

- C0131-C0133: SteamtypedCFishingPerson/layoutglobal/line member andconditionalphase2 transform/event1 use; newtypedline8 CThrowLurepointerretainer. Helper/controller/free/event18/GOG/cadence unvisited; oldsourcebounds retained.

- C0134-C0136: typedCFadeManager threeCFade retainers/actualstartupinit andconditionaltailCRdPrim use; concreteCMapRain/Haze retention/callbacks. Exclusiveownership/free/weatherdrawselection/fullmodes/GOG unknown.

- C0137-C0139: staticCDemo008A6070+4 contextselector, separateCDemoMovie; actualrequest/task/event0 producer andselectedstate6 clear-before-marker/conditionalreentry; alternateCInput-guarded numericclears. Fullhumanmode/UI/loader/sceneowner/GOG/cadence unknown.

- C0140/C0141: SteamCOctTree1CA4 member acquisition/invokeddeletion andobject14C-node58 originalpointerprotocol, conditionalphase13-to-pre-renderqueryjoin. Concrete liveclass/algorithm/safety/GOG/cadence unknown.

- C0142/C0143: staticCMenu01476978, actualselector1 CRdObjectModel task/installedcallback/conditionalphase2 event1/rootstate/input20<-24/camera11Cbit1 use. Fullmode/commit/lifetimes/GOG/cadence unknown.

- C0144/C0145: typedCRdObjectModel/CPlayer update/M1E8 matrixoutput-to-sameinstancepacket80/148 conditionalsubmission. Cachebypass/actualclass/freshness/free/fullalgorithm/GOG/cadence UNKNOWN.

## Bounded and blocked frontiers
- CMessage constructor/singleton-local table-base provenance, CThrowLure direct dispatcher-caller provenance, CScenedemoPostEffect direct singleton free-path provenance, CMustache direct parent-child retirement, CNpcRecord direct lifecycle, DPserial descriptor `+0x20` direct accessor work, XAM/XCA/XNV/DSB/XWP generic parser/consumer routes, conventional Win32 helper routes, CEvent direct stream-writer searches, shared actor `+0x50` generic offset scans, native-task generic callback-seam searches, and D3D9 registry wrapper chaining are `BOUNDED_STATIC`. Reopen only on their documented discriminators.
- Runtime characterization remains blocked: timing/PhysX/CCT cadence, native-task order, CThrowLure aftermath, and DirectShow media cadence require reproducible traces.
- Xbox remains comparative until a matching function/call/xref inventory and structurally anchored homology method exist.
- Secondary fullplayerhandlerextent/actionfamilies, fullsavefileimage/finefields,effects,fullvehicleprotocol andremainingGOG/runtime seeds requireprimaryrechecks; selectedSteam input/camera C0123/C0124 andCGame/CPlayer reconstruction C0125/C0126 nowpositive. `C0004` is explicitly retained as `STRONG_INFERENCE` pending such a check.

## Reconciliation state

The earlier 2026-10-02 reconciliation repaired legacy metadata/report/taxonomy/CMessage scope, but did not complete every audit recommendation. The independent curator review corrected ingestion-as-review-date provenance, an inherited H0008 Steam/GOG reversal, explicit exported-entry limits, missing architectural progress dimensions, validator/control gaps, and the constructor-as-factory next-action wording. No new RE batch, executable claim, or confidence promotion occurred. Original audit and historical checkpoints/journal remain preserved. See `reports/PHASE2_CURATOR_REVIEW_2026-10-02.md`, current checkpoint, K0010–K0014, and the frozen legacy-reference debt index; foundational legacy/recovered annotations still need primary rechecks.

Approved sequence-10 governance applies the independent Phase 3–8 recommendations, shared prospective closeout contract and phase-appropriate transition/resume/template rules. Existing C0115/C0116 scopes are projected into current subsystem/H0207 notes; broad confidence, members, claims and research triggers are unchanged. Historical closeout/audit/checkpoint/journal evidence is preserved. Runtime B0001–B0003/B0005–B0007 remain trace-blocked; B0004 retains static family discriminators. Frozen reference-debt digests are unchanged; no primary analysis or confidence promotion. See reports/PHASE_PLAN_GOVERNANCE_RECONCILIATION_2026-10-02.md.

## Ordinary research safeguards

- Phase3 accepted/closed at conditional-static scope; noPhase5.
- Seq54 C0176/C0177 exacttypedinput/baseproduction/currentevaluation scope accepted; opaquecomposition/preservation/concreteoutputsuccess notinflated.
- Production/evaluation do notprove cacheinvalidation/packetrepopulation/query/latestpose oractualframechronology.
- Seq55 seven-familyglobalreview selects independenttypedstaticaudio localcleanup; sourceavailability12/33 isnotcleanupsemanticcredit.
- All175priorclaims/20252identities/70carry/suppliedevidence andoriginalbounded/runtime/build/provenance conditions conserved.
- SelectedREADY authorizesonlyfreshaudio267-bytelocalcontract; actualdispatch/order/quiescence/lazyfree/PLSE066 unknown, noautomaticloop/Phase4closeout/Phase5.

## Exact next action

Phase 4: in a fresh primary context after ordinary preflight, reconstruct only Steam0076DF60 literal static CSdCore014B0400 ->0072C2E0 267-byte local cleanup branch/value/store/effect contract, including the previously unreconstructed middle and separately sampled014B01E4 operations plus014B0298 unmap/clear/COM boundary; reuse C0118/H0268 only at proven acquisition/registration scope, stop first opaque owner/type/free edge, keep actual registered dispatch/order/quiescence/lazy0138A6E4 retirement andPLSE066 values independently UNKNOWN, exclude car/math/vehicle/genericCRT/atexit/root/bank/request/GOG/runtime/free scans, and do not beginPhase5.

## Session handoff

Completehandoff reports/PHASE4_RESEARCH_HANDOFF_2026-10-04_SEQ0055.md; checkpoint `checkpoints/2026-10-04_seq0055_phase4_post-car-model-global-review.md`; selected `AUDIO_STATIC_CORE_BACKEND_CLEANUP`/READY_FOR_BOUNDED_RESEARCH. Resumefreshboundedstaticaudio cleanup question; carcontent/algebra/preservation/packetfreshness/chronology andallotherdiscriminators remainopen. No automaticloop/Phase4closeout/Phase5.
