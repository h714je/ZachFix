# Frame and lifecycle pipeline — working Phase 3 map

**Current Phase4 notice (seq30):** Phase3 is independently accepted/closed; historical stop-before-Phase4 wording below belongs to its accepted package, not current authority. Normalhuman-authorized Phase4entry seq28/selectionseq29. CurrentnewSAVE_GAMERECORD roots:CPreserve00BE5970,CSaveData00BD9FF8/CSysutil00BD9E0C andstaging00BE5EF0; selectedmechanisms C0146-C0148 in findings/boundaries/save_disk_staging_record_chain.md. No allschema/free/GOG/runtime orPhase5 claim. Authoritativecarry:reports/PHASE4_ENTRY_CARRY_FORWARD_OBLIGATIONS.csv auditedcolumns pluslivingqueue.

**As of:** 2026-10-03, sequence27. **Scope:** accepted selected conditional-static lifecycle/root milestone; actual validation recorded in the closeout receipt. **Stop before Phase4.**

## Positive spine

1. **VERIFIED, reused:** Steam `00700650` / GOG `00700670` calls startup `004017C0`, processes Win32 messages, and calls `00401A70` on eligible idle iterations after timing arithmetic. C0096 / H0228 and `findings/boundaries/application_idle_frame_loop.md` describe the accepted scope. Full gate policy and runtime cadence are UNKNOWN.
2. **VERIFIED, reused:** `00401A70` loads `[00BD7670]` for the active-object dispatcher Steam `006C5FF0` / GOG `006C5AF0`. C0116 supplies the type/acquisition: application alias of cached `00BD9E68` CSingleton<CRdSceneDraw>. Its own deleting interface is not the object consumer interface.
3. **VERIFIED, new C0117:** selector Steam `0041C270` / GOG `0041C290` reads bit 31 of `008A6074`. Selector 1 orders dispatcher(delta) then in-place `00BD9648` companion(0); selector 0 orders companion(delta) then dispatcher(delta). Selected follow-up/repeat calls re-enter this loop conditionally. No measured count or named mode is established.
4. **VERIFIED, new C0117:** after that loop, a second selector read chooses context `00BD86B8` (1) or `00BD9648` (0). The manager consumes it at `00401C42`; both arms converge on `00401C75 -> 00401440`.
5. **VERIFIED, new C0117/BND-085:** this phase stages context matrices/scalars, conditionally calls another manager method for `+0x6711 == 0`, then calls the same `00BD9E48` CRdMovie root at `004016FC -> 00700500/00700520`. Selected upstream branches do not skip the movie call. Its nonzero result means helper present on entry, not completion; later numeric `008A7220` gates affect only a downstream helper.
6. **VERIFIED, reused:** movie session stop/reset uses the same root (C0113); application shutdown `00401310` invokes manager session/member cleanup and clears `00BD7670` (C0116). These are not actual singleton allocation-free/cache-clear proof.

## Independent gameplay audio continuation

**VERIFIED, new C0118/BND-086:** Steam startup installs raw callback `00454D50` on a selector-0 registered task. Its decoded event-1 arm calls acquired `CSound` at `0138A6E0`; accepted object phase-2/task event-one interfaces provide conditional static frame selection. `CSound 0046F360` can return early for bit31=1 AND `008A670C != 1`; otherwise it acquires `CSdCore 0138A6E4` for maintenance and `CSdMain 00BDBCC0` for scalar-fed record update. These are distinct typed receivers. The core also has static `014B0400`; backend globals are not exclusive singleton fields. Steam `CMap+0x10 == 3` controls auxiliary audio work before the continuing maintenance (BND-087), not all sound. See `findings/boundaries/gameplay_audio_organizing_roots.md`. GOG scope is core acquisition/constructor/static receiver only; B0008 live audio cadence and actual shutdown coordination remain UNKNOWN.

## World and resource contexts

**VERIFIED new C0119/BND-091, Steam:** two concretely installed event-one tasks can produce CMap+0x10 values 3/4 under separate private-state gates. The same accepted object phase-2 interface supplies conditional selection, not order among tasks or live frequency. Central world event handler `005D26B0` numeric state 7 issues CRdData tag-counter force requests, clears CMap's 64 retained level pointers, then writes state 8. Raw selector `00474830` routes input 15 to that handler and input 13 to the `0044ACD0/0044A040 -> CMap005D1B00` resource/code consumer, but its incoming selection remains B0009-bounded. This supplies positive world/use/reset contexts without manufacturing a full recurring world-update nesting.

**VERIFIED new C0120/BND-089, Steam:** after the object repeat loop, context/media phase and manager retirement call, `00401C92` loads `[00BD768C]` (accepted acquired CRdData) and `00401C98` calls `006B2A40`. It consumes pending flag +0x14 and can invoke `006B3AA0` record release: event-one callback, payload free and record clear for zero-counter/nonempty records. The earlier tag helper marks pending work, not immediate unload. Specific tag/CLevel+0x164 retirement coordination and the central world handler's occurrence/order against this frame tail are UNKNOWN. See `findings/boundaries/cmap_state_resource_phase_placement.md`.

## Acquired physics service lifecycle

**VERIFIED new C0121/BND-092, Steam:** startup `004017C0 -> 006CC290 -> 006EAAD0` acquires `00BDA0C8` CSingleton<CPhysicsCore> (0x67EDC) and supplies that exact receiver to SDK initialization. Object dispatcher phase14 acquires the same core for `006EB3C0/006EAB60`; selected phase11/9 methods receive it too. Shutdown `00401310 -> 006CC670 -> 006EAAF0` invokes same-root session/dependency cleanup, not proven singleton deletion. SDK global01493FA8/core+11C, 20 pointer slots+50 and event/heap/controller dependencies remain distinguished.

**DISPROVEN narrow C0122:** both synchronous helpers under phase7. Correct raw order: phase14 -> core producer -> `0040B750` first-half -> write7 -> `0040B780` second-half -> write8/event6. This preserves broad C0006/State5/State11 corrections and accepted Phase2 scope. Ordinary queued versus synchronous virtual transactions have SDK-ABI STRONG_INFERENCE labels; worker activation/list transfer, actual root/SDK free, GOG and runtime cadence remain UNKNOWN. Primary source: `findings/boundaries/cphysicscore_lifecycle_root.md`.

## Input / camera sample and control contexts

**VERIFIED new C0123/BND-093–095, Steam:** startup calls acquired00BD9E10 TSiHolder<CInput> (observed0xBD0, zero-offsetCInput). Frame00401A70 commits pending00708350 at00401AB0, then calls conditionalproducerwrapper007099D0 at00401AF0 before objectdispatch. Actual acquisition/aggregate/pending/live record copies and publicaxis accessor feed rawtyped00BE1EA4/0x1AC CCamera. Managerphase5 calls gated0052E0D0->genericcamera dispatcher, followedby phase6; staticconditional order isnotcadence.

**VERIFIED new C0124:**008A9980 numericstate->camera mode map withoverrides and008A9BC8 17-handler table arecamera control, notsample staging. Dispatcher skipsmode9. CPlayer state-receiver genealogy wasSTRONG_INFERENCE atseq16 andis nowC0126VERIFIED for the selectedcreation/reconstruction/state40chain; C0004 wholeplayerhandlerextent remains secondary. Finalizerimplementation/registration exists, actualexecution/cameracachefree/fullinitializer/GOG UNKNOWN; B0010runtime latency/frequency notpromoted. Primary source: findings/boundaries/input_camera_primary_roots.md.

## Shared CGame record / actor-creation reconstruction

**VERIFIED new C0125/BND-096–097, Steam:**00BDA004 acquired0x838E30 CSingleton<CGame> is shared applicationstate, notCMap orstandaloneSaveManager.0061A830 copies45CC0 backup+BE8->live+8C568 anddecodes signedresume domains; private014736D4 state6/event1 contextcalls00647130 live->backup->restore roundtrip. RestoreprecopyflagOR isoverwrittenbycopy; wrapperpostcopyORispositive. No diskread/header/28-recordschema proven.

**VERIFIED new C0126/BND-098:**typedCPlayer selector1/callbackevent0/currenthandle->00506F70 nativecreationreconstruction; guardedCGame00509080 copiesliveCAE0/CAF0 vectors toactor. SameactorESIfeeds00528F40(40), resolvingselectedplayergenealogy beyondseq16SI. Restore-to-creationoccurrence/order, fullworldreconstruction, disk/staging/free/GOG remainUNKNOWN. This joinsrecorddata/consumer, notan inventedimmediatecallstack. Source: findings/boundaries/cgame_record_reconstruction_root.md.

## Vehicle organizing and actor-interface contexts

**VERIFIED C0127, paired PC:** exactstaticCCar008C29F0 has two embedded398 CLayout members, selected lookup retention and request63/distinctnative-task reset. Steam world-transition liveCGamebit1 guard feeds typedCCar method. Task+30 is notCCar dispatch.

**VERIFIED C0128:** distinctCObjectCar own+A4 event5/bit8000 path reaches Steam00555B50/GOG00555C20 and selected corresponding secondary-interface outputs. H0007 same-address hypothesis corrected; GOG129 recognizedbytes are disconnected export coverage. Actorcallback producer/event-to-frame order, CCar-to-actor ownership/fullfree and B0003cadence UNKNOWN. Source: findings/boundaries/vehicle_organizing_root.md.

## Scene submission pointer pipeline

**VERIFIED C0129/C0130, Steam:** acquiredCRdSceneDraw has18-byte pointervector at63A8, startup allocation/pre-render clear+modelpacketappend/laterstagedconditionalconsume/invokedshutdownshellfree. SharedCLevel/CPlayer installed44/28 interfaces retain/replacepacket148/capacity144, tag1 and84<-160/88<-164 resource associations. Consumer uses pointerentries andpicturepayloadgate, notactor88. Fixedstagedcontext00BD9E70 differsfromselectedsource. C0140/C0141 nowestablishquerymember/type/objectprotocol; actual liveclassmembership/fullGPU/GOG/cadence UNKNOWN; selectedanimationstatejoin nowC0144/C0145. Source: findings/boundaries/scene_submission_pointer_pipeline.md.

## Typed fishing update and helper roots

**VERIFIED C0131-C0133, Steam:** selector43 typedCFishingPerson/installednativecallback00619FD0 publishes01470670 alias; actoracquires408 CLayoutMng01470678 andA04 CFishingLine83C. Exactactor+0C006BCB70 callsH0212transformbeforeevent1 callback->gatedsameactorprivateupdate00614F00. Staticconditionalphase2, notcadence. Line8 retainsacceptedCThrowLurepointer, newlyeligibleownretainer lifetime source; olddirectmatrices/CMustache/+50bounds retained. Helpers/global/controller/free/event18/GOG unknown. Source: findings/boundaries/fishing_primary_owner_update.md.

## Required missing edges

No identifiedPhase3-blocking typedroot/interface remains ontheclaimedselectedconditionalpipeline. C0144/C0145 repairanimationstate-to-sameinstance population; validpacketcanbypass andactualliveclassselection/freshness remainUNKNOWN.

Carrylatermode/retainer-tag/backend/save/worker/actor/effects policy andsafeownership work at exactscopes in reports/PHASE3_CARRY_FORWARD_DECISIONS.csv; noneisdeclaredgloballybounded. OriginalCEvent/CMessage/worldselector/+50/task/cachefree/parsertriggers andB0001-B0010 preserved. Runtimecadence/GPU/fullmechanisms/universalGOG notasserted. Formalcriterion/dependency review reports/PHASE3_CLOSEOUT_2026-10-03.md explainswhy eachmissing side isnot requiredfor theseconditionalclaims.

## Evidence and scope

New primary/control-flow references and reproducible byte checks: `findings/boundaries/application_media_phase_placement.md`, H0263–H0265. Accepted acquisition/setup roots are reused at C0113/C0115/C0116 scopes, not re-opened or counted twice. Xbox unmapped. Current all21-family/sevencandidate breadth review: `reports/PHASE3_ROOT_FRONTIER_REVIEW_2026-10-03_SEQ0021.md`; sequence12 review remains history.

## Sequence22 — selected independent effects organizing/use

**VERIFIED Steam:** typed14 CFadeManager00BDBCC4, actualstartup-installedtaskevent0 init ofthree1D0 CFade retainers4/8/C; typednativecallbackevent1/update andevent12 presentation. Independent00401440 conditionaltail0044AC10 consumeslatch/manager4 data andacquired24C CRdPrim00BD9E5C (startupalias00BD7680). TypedCMap+A3D24/A3D28 weatherretainRain/Haze; Haze event1/event0F geometry/dependencyusepositive, actualevent0F frame-selectionunknown. Retention isnotexclusiveownership, latchclear isnotdrawcompletion. Actualroot/retainerfree/fullmode/GOG/cadence unvisited; XWPgrammar/CScenedemo/rawworldselector boundsunchanged. C0134-C0136/BND-109-BND-112, findings/boundaries/effects_organizing_roots_and_use.md.

Globalfrontiercomparison selects exact008A6074 bit31 producer, notwarmweather/fade depth. No Phase3closeout/Phase4.

## Sequence23 — typed CDemo context activation and selected exit

**VERIFIED Steam:** staticCDemo008A6070+4 is008A6074 selectorfield; distinctCDemoMovie008A7218. Typedrequest route->selector0task/install004213D0/synchronousevent0 setsbit31; selectedevent1state6 clearsbeforeoriginaltask marker andcancreate replacement/synchronousreentry. Alternate00642640 numericreset/CInput-queryguard clearsareprimaryverified, incomingowneruntyped. ReusedC0117 contextsplit nowhasconcreteproducer; separateframe readsnotatomic, no fullhumanmode/cadence/free/GOG claim. CDemo addsscene/eventorganizing/use slice withoutreopeningboundedCEventbufferloader. C0137-C0139/BND-113-BND-115, findings/boundaries/cdemo_context_selector_provenance.md.

Nextindependentglobalfrontier isscene+1CA4 queryownership/actualclassmembership. Phase3stillnotcloseout-ready: broadermode/UI/event-loader/scene/lifetime obligationsremainunvisited, notgloballybounded.

## Sequence24 — query member and object membership protocol

**VERIFIED C0140/C0141 Steam:** separate70-byte rawCOctTree atscene1CA4; typedstartup/configuration, conditionalphase13virtual48 argument, node58 originalobjectpointer insertion andpre-render query-object consumption, invokedshutdown child/root/vector delete+memberclear. CLevel/CPlayer installed48=004029B0 retaincurrentnode atobject14C; unchangednode no-op, not periodicrebuild. Concrete liveclassselection/safeobjectdetach/fulltree/GPU/animation/GOG/cadence UNKNOWN. Source: findings/boundaries/scene_query_member_lifecycle.md. Scene depth stops atPhase3 scope; independentCMenu root/use selected.

## Sequence25 — CMenu organizing task and input/camera use

**VERIFIED C0142/C0143 Steam:** in-placeCMenu01476978/raw007818F4 ctor/finalizerregistration andselectedreset; actualrequest->registered3B0 CRdObjectModel->installedraw00654490. Reusedmodel0C adapter suppliesconditionalphase2 event1; callbackswitchesreceiver fromtask tostaticCMenu, selectednumericstates feedCInputquery/20<-24 andtypedcamera11Cbit1 touch. Initialevent0 notrecurrence; finalizerregistration/event18 endpoint notactualcleanup/free. Fullmode/commit/lifetimes/GOG/cadence unknown. Source: findings/boundaries/cmenu_organizing_task_use.md.

Requiredstrategicreview reports/PHASE3_LIFECYCLE_OBLIGATION_REVIEW_2026-10-03_SEQ0025.md assessesremainingroot/temporal obligations, selectstypedanimationowner-to-packet join, andexplicitlyresetsfamily/productive0 afterproductive4. Stopscenedepth; fullmechanisms/grammar/safety arenotuniversalPhase3minima. Notcloseout-readyatselectedanimationroot/temporaledge, noPhase4.

## Sequence26 — typed animation state to same-instance submission

**VERIFIED selectedSteam C0144/C0145:** rawCRdObjectModel/CPlayer10=006BCBE0 ->sameM006C1430 ->resource160 evaluation outputM1E8; sameM packet006BD4C0 consumes1E8 through0070CF90 transformed/indexed48-byte copies intoP80. Typed44->28/M148/scene63A8 joinobject-before-pre-render conditionalspine. Existingvalidpacketcanbypasspopulation, so latestpose/everyframe isnotproven. Actualclass/queryselection,refreshpolicy/fullmechanisms/free/GOG/cadence remainUNKNOWN. findings/boundaries/animation_model_state_submission.md. Noimmediatelynecessary follow-upmechanismbatch; requiredcriterion-basedreadiness reviewnext, notPhase4.

## Phase3 closeout decision and carry-forward

Selected-static Phase3 root/temporal milestone is accepted after semantic readiness/dependency/redteam review and actual procedural validation. No Phase4. Historical sequence notes retain their original scopes/readiness conclusions; the current decision is reports/PHASE3_CLOSEOUT_2026-10-03.md. All21 lifecycle axes and per-edge carry decisions: reports/PHASE3_ROOT_LIFECYCLE_MATRIX.csv and reports/PHASE3_CARRY_FORWARD_DECISIONS.csv.

Startup/frame/context/world-resource/independentUI-media-event andmandatoryaudio-save-input-physics-vehicle roots arepositive; C0144/C0145 repairselectedanimationstate-to-sameinstance packetjoin. Othermode policies/liveclassselection/latestposefreshness/actualfree/matchedretirement/fullalgorithms/GOG/cadence remainindividualUNKNOWN/bounded/runtime dispositions, not implied solved. No furtherPhase3 familyexpansion justified now; humanbarrier beforePhase4.

## Phase4 sequence54 — explicit actor/base-state evaluation request

**VERIFIED selectedstaticorder:** CObjectCar005ED987 ->00541760 copies ->006BABF0(1,0) selectedbase98production ->opaqueafter-multiplytail ->006C1430(0) currentstate/acceptedconditionaloutput. This constructor-local order isnot placementinrecurringframe orunchangedvaluepreservation. C0144/C0145 object/pre-render/packet-bypass scope remainsunchanged; no latestpose/invalidation/population/runtime chronology. findings/boundaries/cobjectcar_model_base_cache_production.md.

## Phase4 sequence56 — Available audio finalizer versus live ordering

**VERIFIED Steam C0178/C0179/BND-164-166:** literalstaticCSdCore014B0400 ->0076DF60/0072C2E0 local sharedbackend attempts andabsoluteA12/B13/Vstorage cleanup. A CloseHandleattempts lack localclear; B opaque0074EE0B calls followedbydwordclear; Vunmapfollowedbyclear/CoUninitialize. Theseare notreceiverfields orlazy0138A6E4 free. Actualdispatch/order/userquiescence/lastuse/helpersuccess/destruction/COMbalance/runtime/GOG remainUNKNOWN. findings/boundaries/audio_static_core_backend_cleanup.md. Prior acquired/task/use scopes reusedonlyexactly; noPhase4closeout/Phase5.

## Phase5 local lifetime order versus occurrence — through batch5, 2026-10-04

Phase4 accepted/closed; currentC0180-C0192 routes in maps/CROSS_SUBSYSTEM_DATAFLOW.md distinguishlocalinstructions fromruntimeordering. VERIFselectedorders: worldstate6 categoryword marking thenstate7 resourceforce/retainerclear areseparatefromframe-tail selectedretirement/release; appresourcecleanup descriptor/table release precedesexplicitloadworkerstop (mutexnotjoin); CMenutaskevent2 localFadeManager8maskclear notimmediatedelete; fishingevent2 currentline-delete/clear precedes81C1 admission gate; physicsinvalidatesrecords beforeSDKinterfacecall/slotclear, andpurgesW0 beforeW1reload/namedmanagerrelease/wrapperdeallocation.

UNKNOWNjoins: rawworldselector-to-currentframe occurrence, sameobject/resource/task/slot/generation acrossopaquecalls, alreadyloadedworkerlastuse, actualfinalizerdispatch/shared-lastuser/SDKfree andall livecadence. Invokedcall/deallocationboundary isnotexternalsuccess orsafequiescence. Sources/claims: findings/subsystems/PHASE5_BOUNDARY_PORTFOLIO.md. No fullframe/ownership/Phase5closeout orPhase6 claim.

## Phase5 batch6 — deferred application root callbacks

VERIFIED selectedapp0040141D->wrapperJMP006E19A0->CSiHoldercurrent-codeCALL/AL1recordclear-repeat mechanism; afteremptypasscurrentcarrierdeleting/rootclear. SaveData70guard/idle28/dependencydecrement precedescallbackvptr/lock/storage boundary; Sysutil220guard precedeslock/callback/embeddedthread memberteardown. Registryunlocks/rootreloads andconstruct-cleanup-finalU observations areexplicitgenerationbreaks. Progresshelper00408AF0(1000,0) unexpanded, notassertedSleep/activation/join. Currentinputcallback/cleanup successor usesnewcarrier invocationdiscriminator, notmode9/sample/repeatedgetters. Enginecarrier isnotproof ofseparateCRTatexit dispatch. C0193-C0195 report/map/seq77handoff govern; noPhase5closeout/Phase6.

## Phase5 Input conditional finalizer/member retirement

**VERIFIED selectedSTEAM C0196-C0198:** code-onlyregistered00408A30/currentI BC8zero/table0flag1/holdercleanup/deallocationboundary/currentrootclear; U220INC/DEC ondistincttypedprovider observations; I94callbackretainsI/code, vptrreset notdetach; typedT=I8 ordinarymembercleanup; no-op668 andcallbackT25observation withremainingTuse. **UNKNOWN:** successfulregistration/carrier-slot/currentI/currentU generations, balanceddependency/admissionclose/lastuse/stopjoin/operation-success. C0123/C0193 reusedonlyexactly; mode9/sample/commit/API/registryclosed. All70+18guards/K0022-K0023histories retained. findings/boundaries/phase5_input_registered_finalizer_lifetime.md

## Phase5 model availabledestruction versusinvokedrebind

**VERIFIED STEAM C0199/C0200:** actualrebind commoncleanup passesstate/matrix pointers todeallocationboundary thenfieldclears/selectedreplacement; rawmodelownslot→ordinary/membercleanup→basevptr→packet148deallocation/clear BEFOREcurrentvirtual48(0). D8/DC notpacket138; querydetach/scenevectorerase notborrowedpacketlastuse. **UNKNOWN:** actualselectedM/generation/currentvptr, oldpacket invalidation, vector/drawlastuse/success/safety. Thin11-byteadapter narrowsaudioopaquehelper only; noaudio-loopreaudit/GOG/runtime. findings/boundaries/phase5_model_buffer_packet_retirement.md

## Phase5 requestedSleep interval versuslocalcompletion

**VERIFIED STEAM C0201/C0202:** unsignedpair/1000 quotientlow32→importSleep; carrier1000/0→DWORD1 request, localguardprotocol/noLeave anduncheckedreturn. CompleteCCallBackThreadwrapper neveruses/dereferencesthis; latecallbackpointertransport notlateTmemoryaccess, callbackstillbeforecleanup/RET. **UNKNOWN:** sourceclock/actualelapsedtime/runtimebinding/currentparticipantgeneration/admissionclose/detach/lastaccess/join/quiescence. Sleep/progress notnewowner orsafe-retirementfence. findings/boundaries/phase5_shared_progress_delay_retirement_limit.md

## Current Phase5 handoff — sequence83

Ninepositiveconditional-static portfolios throughseq82; currentfirstedges/corrections andquantitativecoverage in reports/PHASE5_PRIMARY_HANDOFF_2026-10-04_SEQ0083.md. Formerthroughbatch5/6/seq77phrases abovearehistorical, notcurrentstop/authorization. SelectedcoldCFishingPerson7F0 continuation isREADYforboundedprimaryqualification, notCMustacheidentity/destructionpromotion oroldgenericchildreopening. Primarycontextoverloadfreshhandoff/ordinarycontinuationauthorized/READY/counters0-0-0, noPhase5closeout/Phase6.

## Cold event2 shell lifecycle interface

**VERIFIED STEAM C0203/C0204:** exact3C allocation/value/CMustachesAdmin type retainedP7F0; cold event2 direct00526BA0 thenindependentlyreloadedcurrent-slot0flag1/fieldclear. Helperthreehandles/twoseparatelookups/currentvirtual30, namedshellavailabledeallocation notdescendantdestruction. **UNKNOWN:** construction/currentA/X generations, currenttable/typedchild/lastuse/free safety. Originalordinaryactor/controller/CMustache/CThrow/handlebounds andallseq83corrections retained. findings/boundaries/phase5_fishing_cold_7f0_shell_retirement.md

## Provider registered-code lifecycle limits

**VERIFIED STEAM C0205-C0207:** exactconstructorproviderresult INC versuscleanupnewcurrentroot DEC, provideroffsets1C50/60 notInputpointerfields; typedholders registercode004035E0/0070BA50, availablezero-gate/unlock/currenttable0flag1/relock/currentrootclear/AL1. Namedholdermember/backinginterfaces availableconditionally. **SI:** dependencyaccounting interpretation. **UNKNOWN:** gen/balance/actualcarrier/currenttable/admissionclose/lastuser/success. OriginalInput/allseq83guards unchanged. findings/boundaries/phase5_input_provider_accounting_finalizers.md

## Light cleanup helperentries andfirst currentdispatch break

**VERIFIED STEAM C0208/C0209:** lighttyped48zero-query N/O removalrequest thencurrentO14Cclear; availableowncleanup thenconditionalouterboundary. Ordinarylighttable/helperentry, opaqueO194cleanup, basevptr/helperentry; latercurrenttargets conditional. **SI:** matchingnode-pointerremoval/C0141 unchanged. **UNKNOWN:** actualcurrentdispatch/ownslotinvocation/O-Ngen/aliaseslastuse/success/GOG. findings/boundaries/phase5_spatial_light_detach_cleanup.md

## Current Phase5 fresh-primary handoff — sequence93

Threeadditional substantialportfolios seq84cold7F0shell/86provider/88typedlight; compactaudioavailability90/moviecandidate limit92. Twelvepositiveportfoliosoverall, allstrongergeneration/admission/lastusefirstedges conserved in reports/PHASE5_PRIMARY_HANDOFF_2026-10-04_SEQ0093.md. All70+18guards/seq83corrections andclericalhistories retained. Selectedunpromotedcoldweather-retainer qualification, broadworldresourcefamily, READY/ordinarycontinuationauthorized; primarycontextoverloadfreshhandoff notPhase5closeout/Phase6. No newweathertype/ownership/free semantics promotedhere.

## Batch13 — CMap Rain request/store andRain/Grid cleanupavailability

**VERIFIED STEAM C0212/C0213:** currentRainretainer/currentvirtual30 thennormalreturnfieldstore; separateRainowncleanup/GridR2424/backingE3Cboundary/clear beforeoptionalouterRboundary. **UNKNOWN:** currenttable/actualsamegen selecteddeleting/postcallfield/backingidentity/lastuse/success. PriorbranchstrictdisplaycapNOTPASS retained; noHaze/render/oldmatrix widening. findings/boundaries/phase5_weather_retainer_grid_retirement.md

## Batch14 — Playernumericoperand tophysicsretainer request/clear

**VERIFIED STEAM C0214/C0215:** ordinaryPlayer94C/950 guards→independentcurrentroot/postgetter numericreload→fullDWORDindexedQ/currentW bridge→normalreturncoreslotzero; availableownordinarybeforeoptionalPouterboundary. **SI:** selectedgamephysics-support role. **UNKNOWN:** actualP/indexdomain/C-Q-W-M/table/gen/lastuse/success; notSDKquiescence/childdestruction. findings/boundaries/phase5_player_physics_dependency_teardown.md

## Batch15 — worldreset andseparateLensflare shellpublication

**VERIFIED STEAM C0216/C0217:** invokedCMaprelativezeros/Rain-Haze retainerclears/two64DWORD ranges; separatecold454L zero-fill→opaqueVadapter→namedLensflarewrappertable→fixedcachepublication/commonL8zero→independenttypedactiveD6744zero. Genericadapter passes&argumentslot; retainedcapacityarmcopiesslotvalue. **UNKNOWN:** type/value/stackcontinuity/generation/registeredcleanup/ownership/lastuse/success; notlevelvector oroldN58. findings/boundaries/phase5_cmap_reset_lensflare_cache_publication.md

## Batch16 — save/Gameinlinevalues andworldslot-fold integration

**VERIFIED STEAM C0218/C0219:** K3/4skip; otherpreparationGentry→runtimebyte/optional-or-currenthandle4DWORDcopies into99048/99058;64worldslots nonzero samples→one32bitrotatingfold at99068. BuilderlaterG2reacquire beforecopy. **UNKNOWN:** sourcevalidity/currentR1-R2/world/G1-G2gen/coherentstaging/owner/lastuse/success; not64independentbits/synchronization. findings/boundaries/phase5_save_preparation_world_player_values.md

## Compactqualification — CCarretainsGameinterioraddress

**VERIFIED STEAM C0220/C0221:** producerselectedn0..127 returnssavedGame+CD368+4n/fail0, acceptedCarretainsaddress73C. Secondleafincomingthisunused/globalbyteequals1; firstleafopaquequeryQthenQ0secondreturn. **UNKNOWN:** cellcontents/currentG-U-bytegeneration/typedactor/owner/lastuse/success. Notnewphysicalvehicleowner orfullsafe-lifetimeportfolio. findings/boundaries/phase5_ccar_retained_game_interior_address.md

## Currentfresh-primaryPhase5handoff — sequence103

Fourresumedsubstantialmechanisms94weather/96Playerphysics/98worldLensflare/100saveinline-worldvalues pluscompactCCar102Gameinterioraddress, allprecisestrongerfirstedges conserved. Sixteenpositiveportfoliosoverall plusqualifiedsupplements, notsafeowner/free/lastuse closure. Primarycontext/control-order reliabilitydegraded, coldsharedGamecompositionqualificationselected; ordinarycontinuationauthorized. All70+18/seq83–102corrections/failedreceipts/capNOTPASS preserved; noPhase5closeout/Phase6. reports/PHASE5_PRIMARY_HANDOFF_2026-10-04_SEQ0103.md

## Sequence104 — shared Game inline composition qualification

**VERIFIED Steam C0222/C0223:** availableGameowninterfaces callordinarybeforeoptionalstackbit0 originalGouterboundary; ordinarysuppliesLEA G+838A50 thenG+8386B8 toacceptedCLayouttablewrite/reset, thenG835014 exacttablevalue. **STRONG_INFERENCE:** selectedinlineCLayout-compatiblecomposition. **UNKNOWN:** currentrootinvocation/currentG-table/membergeneration/ownedfieldtypes/Carborrowerlastuse/safe release/cadence. Failedfirstpersonalreset-targetassertion/rejectedbranchcommand retained; correctedpersonal+explicit-schemaPASS beforepromotion. findings/boundaries/phase5_cgame_inline_layout_composition.md

## Sequence106 — selective audio client operand andrecord mapping

**VERIFIED Steam C0224/C0225:** postgetterclienth/unusedthiswrapper -> typedcurrentMain; fullh/low8/50stride/currentIDmatch -> interiorRor0; selectedq0 R18/exactAL1/T-M/U interfaces, no directrecordclear. **UNKNOWN:** h_guard-h_reload/creator-currentA-R-M generation, i<128 admission, opaqueeffects/stack/finalarg/type/lastuse/safe release. This isselectiveoperandqualification, notfullretirementportfolio. findings/boundaries/phase5_audio_client_selected_record_operation.md

## Sequence107 — disconnect recovery and fresh-primary handoff

Governancehandoffafteruserrequestedcontextend/connectionloss: seq106research/promotion andfouractualchecks completed; pending-proof/STATUS/journal/freshhandoff gaps preserved/correctedclerically. All11ledgers/debt unchanged. GameC0222/C0223 andAudioC0224/C0225 scopes/firstedges conserved; no newprimaryclaim/frontier. Currentpendingglobalreview/STRATEGIC_REVIEW_REQUIRED/lastADVANCE/counters0-0-1 remain. ColdModelCRdInterp candidate documentedonlyfornextreview. reports/PHASE5_PRIMARY_HANDOFF_2026-10-04_SEQ0107.md

## Sequence109 — NPC event2 nominated-arm routing limit

**VERIFIED Steam C0226:** event2map02/selected0047F8E2 invokescurrentNtable+90 thenJMPcommon-tail; nominateddirect0047F92C->0047F770 notinthatarm. **UNKNOWN:** currenttarget/effect/transitive/tail/Ngen/lastuse. BOUNDED_NEGATIVE candidateonly; coldcleanupbranch notlaunched, nootherarm. findings/boundaries/phase5_npc_event2_selected_virtual90_routing_limit.md

## Sequence113 — Movie constructed texture and descriptor interfaces

**VERIFIED Steam C0227-C0229:** Moviealloc20/namedCRdTexture ctor->M8pub/currentreload/sevenargs; Hcleanupbefore s6guard; localdescriptoraddress->literal0148BBD8/opaque006CD010; two01480984observations/currenttable5C; commonnumeric8/C/E stores andpartialAX EAXresidue. **UNKNOWN:** currentH/M8/table/resourcegen/type/API/effect/heapretirement/lastuse. OriginalsourceNOTPASS retained; prospective128/448+88metadata fitPASS beforepromotion. findings/boundaries/phase5_crdmovie_constructed_texture_setup_descriptor.md

## Sequence115 — composite consumer capture limit

**VERIFIED Steam C0230:** currentunsignedtwo-armDpointerrequests, conditionalR10=opaqueEAX, two-wordcopyfromopaqueP(notDalias), RET4unqualifiedbits. No localDfields/Hcapture. Capture/retention/currentR-D-P-H-gen/lastuse **UNKNOWN/NOT_READY**;45/115onebody only, no supportcallee/oldregistrychain rescue. findings/boundaries/phase5_texture_descriptor_consumer_request_limit.md

## Sequence117 — complete fresh-primary Phase5 handoff

Fresh-primarycontextquality handoff afterNPCrouting/Movieconstruction-descriptor/consumercapturelimit andglobal116review. Currentcanonicalstatevalid butreceiver/gen/control-exception historiescrowdnewacquisitiondiscipline; selectedModelREADY0-0-0/lastBOUNDNEGATIVE nextfreshonly, nobodystarted. Alloldstrongerfirstedges/70+18/failurechronology preserved, noPhase5closeout/Phase6. reports/PHASE5_PRIMARY_HANDOFF_2026-10-05_SEQ0117.md

## Sequence118 — qualified Model/CRdInterp reset boundary

VERIFIED Steam C0231/C0232: ModelE=M1B4 literaltablewrite/directreset andinitializer savedECXreset; coldhelper E4clearwithoutoldload before scalarwrites and E8/C/10 guard/reload/argument boundaryattempts then normal-returnfieldclear. SI inlineCRdInterp-compatiblecomposition; actualowner/currenttype/gen/success/lastuse UNKNOWN. Personal69/224 and explicit59row+metadataagreement beforepromotion; firstcomparisonfailure retained. findings/boundaries/phase5_model_crdinterp_backing_reset_boundary.md

Resource link, three backing storage fields, allocation/resource/owner identity and generations remain separate; no successfulfree/global-lastuse portfolio closure.

## Sequence119 — post-Model global selection and API recovery

Governance119 completedpendingnine-familyreview afterAPI/agenttermination. Seq118 exactpersistedstate andfreshAttempt3fourEXIT0 verifiedwithoutModelrepetition. Two119partialMarkdown retainedunfinished; noscoutfinalresult/JSON inferred. Maincheckedselectedmetadata/class-gettercitations andselectsnewHookChain component/currentCSoundoperand localquestion, gatedfirsttwotransport beforedownstreambodies; strongerowner/type/gen/effect/success/lastuse NOT_READY. No newsemanticcredit/Phase6. reports/PHASE5_POST_MODEL_GLOBAL_REVIEW_2026-10-05_SEQ0119.md

## Sequence120 — terminal fresh-primary handoff

Terminalfresh-primaryhandoff aftercompletedvalidated118ModelADVANCE and119globalselection. Canonicalstatevalid; repeatedsetup/schema-shellorderfailures plusinterruptedpartial-resultprovenance nowdegrade nextacquisition/control reliability. StopbeforeHookChainbody, nofurtherresearch/globalreview. SelectedHookChainREADY/lastADVANCE/0-0-0 retained; 118notrepeated/failedscoutnotinferredorrelaunched. All70+18/priorNOTPASS/schema/disconnecthistory preserved. reports/PHASE5_PRIMARY_HANDOFF_2026-10-05_SEQ0120.md

## Sequence121 — HookChain inline audio operand boundary

VERIFIED Steam C0233/C0234: availableouter ordinarybeforeoptionalOboundary; O434/tableliteral/componentcall beforeopaquebase; q0guard beforeCSoundgetter/q1reload; currentA fullqmatch selectsinteriorB-or0/opaque-tail before uncheckednormalreturnL4=-1. SI CSoundLoop-compatibleinlineregion; no backing/resource/ownership/actualdispatch/gen/success/finaluse proof. Freshbranch/mainPE/ASM and explicit28row-schemaPASS, exact82/288. findings/boundaries/phase5_hookchain_csoundloop_operand_retirement_limit.md

## Sequence122 — global comparison and finite save/event selection

Nine-family globalcomparison aftervalidated121localHookChainADVANCE; firstqdomain/effect/currentgen/lastuse edge retained. Completedfresh122METADATA_ONLYbranch plusmain2function/2call/12hash verification support separatefinite selectedsave/eventcurrentoperand question, notcodefit/ownedstream/retirement. reports/PHASE5_POST_HOOKCHAIN_GLOBAL_REVIEW_2026-10-05_SEQ0122.md

## Sequence123 — terminal fresh-primary handoff

Terminalfresh-primaryhandoff aftervalidated121HookChainADVANCE and122globalfiniteSaveEventselection. Currentcontrols/canonicalvalid; accumulatedlegacy-scope/journalreceipt/review-headerfailures andcorrectionprovenance reduce nextacquisition/control reliability. SelectedSaveEventREADY/lastADVANCE/0-0-0 retained; no00430A10/0072A470body ornewresearch/reassessmentafterterminaldecision. All70+18/oldandnewfailedoriginals/corrections preserved. reports/PHASE5_PRIMARY_HANDOFF_2026-10-05_SEQ0123.md

## Sequence124 — Save/Event untyped current-member value bridge

VERIFIED Steam C0235/C0236: opaque G0/E0 getter results form P=G0+CDD80 and R=E0+7B4 at selected call; leaf eight interleaved DWORD copies plus20 REP DWORD moves, then normal stack return. Separate later getter/flag/x87/256-byte conditional copy order; no selected pointee cleanup/reference maintenance observed. CEvCore/Game association SI; types/owned or borrowed references/generation/success/lastuse UNKNOWN. Personal87/303 and independent41-row exact schema agreement. findings/boundaries/phase5_save_event_core_member_value_transport.md

No retained/owned reference, selected cleanup invocation, common getter generation or safe-retirement claim. Stronger edges remain individually unwaived; postbatch global comparison/readiness-signal review is required.

## Sequence125 — dedicated Phase5 dependency/readiness review selected

Sixteen-card finite-source global reassessment after validated124 recommends dedicated Phase5 dependency/readiness REVIEW; genuine local Model/HookChain/Movie/Save discoveries retained, no newly substantiated independent stronger join source in this universe. Main12source/153snapshot/10proof/18queue verification; metadata/primary/semantic rows0. Review selection not acceptance/exhaustion/newhuman-runtime barrier; all70+18/22family visibility and individuallyrequired joins unwaived. reports/PHASE5_POST_SAVE_EVENT_GLOBAL_REVIEW_2026-10-05_SEQ0125.md

This is selection/governance only; no fullrequirementverdict/waiver/acceptance/Phase6. Original124 preservationfailure andallolderhistory remain conserved.

## Sequence126 — actual Phase5 dependency/readiness review

Actual preparing-primary dependency/readiness review:12edge profiles separately credit local MET and stronger NOT_MET; all88original/current individual triggers conserved; all22families have explicit stronger/unserved consequences. Normal Phase5 closeout readiness NOT_ESTABLISHED, no normal-exit exception/waiver/independentacceptance/newprimary/semanticrows. Select independent fresh-primary dependency-scope adjudication, not Phase6. reports/PHASE5_DEPENDENCY_READINESS_REVIEW_2026-10-05_SEQ0126.md

Normalcloseout eligibility unestablished; source bounds are not exhaustive and unvisited is not BOUNDED. Original124preservationfailure/125branchfailures andallolder corrections conserved.

## Sequence128 — dependency scope adjudicated; normal closeout review ready

STRONG_INFERENCE scope adjudication selects PHASE5_CLOSEOUT_REVIEW_READY at unchanged selected conditional-static Phase5 minimum. All12profiles/49components/22families/88carries individually reviewed; finite requiredstatic blockers0; no targetedresearchsuccessor. Required positives and exact local limits credited; stronger joins remain NOT_MET/UNKNOWN, unvisited not bounded, allfailedoriginals/NOT_PASS preserved. Normal closeout candidate prepared for later independent fresh-context review, not acceptance or Phase6; no primary/semanticledger changes.

reports/PHASE5_DEPENDENCY_SCOPE_ADJUDICATION_2026-10-05_SEQ0128.md;reports/PHASE5_CLOSEOUT_CANDIDATE_2026-10-05_SEQ0128.md. The maps continue to break every missing generation/effect/actual-invocation/final-use edge rather than assuming it. No source reopened or lifetime claim waived.


## Final static campaign amendment — FSC001 / checkpoint252

**VERIFIED selected mechanics; STRONG_INFERENCE conditional composition.** New current companions connect busy-gated `00401050` key/payload insertion and comparator `00401020` to the accepted `00401080` drain. Selected Steam event0x12 paths (including only the established CHelp-installed path into00633B80) enqueue key0 then exit on admission; available independently named CObjectTarget slot98 methods005B19A0/GOG005B1A70 offer a wrapping460+0xA key then exit or request localfallback. Own-build producer/comparator/drain/type-method correspondence is qualified, not numeric-address transfer. Comparator equalkeys return-1/nozero: no stable/total/FIFO order. Hook mutation/current callback generations, arbitrary payload type/key domain/capacity, actualdraw/cadence/globalserialization/ownership/lastuse remainUNKNOWN. Details/current companion identity: `findings/boundaries/final_static_optional_record_admission.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical Phase8/through250 provenance remains intact.
