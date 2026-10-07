# Cross-subsystem dataflow and lifetime obligations

**Phase:** 5 active. **Updated:** 2026-10-04, through batch6/sequence76; fresh-primary successor sequence77. This is an interacting-system map, not a safe-lifetime or runtime-cadence proof. Phase4 remains closed at its accepted conditional-static scope; `audit/PHASE4_CLOSEOUT_AUDIT_2026-10-04.md` governs all inherited reliance.

## Reading convention

**VERIFIED** applies to the stated local instructions/interfaces, **STRONG_INFERENCE** to supported architectural interpretation, **UNKNOWN** to the explicitly missing joins. An “interface break” below means the route must not be read as a joined occurrence/generation. Retained pointers are not automatically owning references. Cleanup, clear, marker, available destructor and actual external release remain distinct milestones.

## World/resource/actor/scene seam — batch1

**VERIFIED local mechanisms:**

- Archive/typed callback publishes a CRdPicture in CRdData descriptor1C; the manager exposes event1 deletion. Named LEVEL lookup supplies a pointer retained by a CLevel164 setup interface (C0114).
- World event1 state6 calls active-manager category-filtered marking using word[object2C]. CLevel table30 is a compatible marker, not a deleting operation (C0180).
- CLevel-compatible model cleanup clears160/164; the setter actually calls that local cleanup before publishing replacement arguments. Available deleting/base teardown also reaches this local clear (C0181).
- The CLevel-compatible packet interface can copy resource pointers160/164 into packet84/88; actor clear does not clear previously copied packet values (C0181, accepted C0130/C0145).
- Independent world state7 forces resource tags, clears64 CMap slots, and commits8; frame tail calls active retirement before deferred resource release (C0119/C0120).

**Interface breaks / UNKNOWN invariants:** named LEVEL descriptor-tag generation ↔ CMap slot; concrete object word2C and selected marker/deleting occurrence; actor and packet final use ↔ descriptor event1 deletion; raw world-selector occurrence ↔ frame-tail retirement/release. Cardinality, pointer clear and static order do not establish any of these joins.

**Reference-kind result:**160/164 and packet84/88 are verified retained/copied pointer values. Actor local cleanup supplies no old-pointer release instruction in the selected body. Exclusive ownership, reference acquisition, borrowing contract and safe release remain UNKNOWN.

**Correction:** C0047/C0086 setup-to-attachment slot route and H0209's same-slot basis are disproven as written. Steam CLevel4C=005CCDD0/50=006C0620; GOG4C=005CCEA0/50=006C0130 (old006C04D0 is58). Historical helper-body/H0210 evidence is retained at its separate scope. Report: `findings/boundaries/phase5_world_resource_retirement.md`.

## Worker/resource/platform seam — batch2

**VERIFIED accepted anchors:** typed static CLoadThread actual initializer-called start; packed value queue and separate shared direct slot; same-root worker dispatch to CRdData descriptor publication (C0152–C0154).

**VERIFIED new local order:** application conditionally invokes CRdData session cleanup and later clears only its root. Cleanup force-requests/releases qualifying descriptors and frees/zeros the table **before** invoking same static CLoadThread stop. Stop clears loop/logical queue/count, waits on worker mutex14 (not thread handle8), discards the wait-status AL, and sets period1. Separate registered finalizer's available destructor releases queue backing storage before a vptr-only CThfunc base teardown. C0183–C0185; `findings/boundaries/phase5_resource_worker_shutdown_coordination.md`.

**Interface break / UNKNOWN invariant:** invoked upstream admission closure and same-thread/request/table-generation completion **before the first descriptor/payload release**. Mutex acquisition, queue clear and available finalizer do not supply it; queued execution can occur outside that mutex. A runtime race or absence of all external coordination is not established. Separate application-root clear versus singleton free and registered-body versus actual exit dispatch remain explicit.

## Menu/task/effects seam — batch3

**VERIFIED local mechanisms:** the world's actual CMenu reset call switches receiver to inline CLayout U+6EE8; that helper resets fields and makes no release/deallocation call. The available ordinary CLayout destructor invokes the same reset. Separately, CMenu's registered selector1 model task has category-word8 and an installed callback whose event2 arm acquires CFadeManager, loads retainer+8 and clears bits1/4 at that CFade168. C0186/C0187; `findings/boundaries/phase5_cmenu_task_layout_retirement.md`.

**Conditional interface breaks:** current task table30 must still supply the marker; optional global callback must preserve task lifetime and installed callback44 before local event2 delivery. Same-generation task selection/last-use/manager deleting occurrence is UNKNOWN. Fade+8 storage is distinct from the accepted request/predicate+4 storage; pointer equality/inequality, shared generation and ownership are not proved. Marker/event2/inline reset do not establish task, Fade or controller deletion/free.

## Fishing parent/line/auxiliary seam — batch4

**VERIFIED local mechanisms:** installed CFishingPerson event2, admitted under current81C=0, calls a parent cleanup before81C=1. Cleanup invokes currentP83C slot0 with flag1 and clearsP83C. Named CFishingLine slot0 supplies ordinary0060DB50 plus conditional same-allocation deallocation; ordinary teardown marks/clears line4/C through currentvirtual30, not a direct line8 operation. C0188/C0189; `findings/boundaries/phase5_fishing_parent_line_aux_retirement.md`.

**Interface breaks / UNKNOWN invariants:** original line/CThrowLure generation↔currentP83C/currentline8; separately marked P7E0↔line8 alias; current childvptr/transitive marker effects; actual parent/line/manager last use and actor-global alias retirement. Absence of a direct line8 operation is not global absence of lure retirement, a leak or H1 cleanup. A later7F0 decompiler candidate is separately unqualified; no generic CMustache/CThrow source reopening.

## Physics service/context/external dependency seam — batch5

**VERIFIED local interfaces:** currentC50 slot value is passed to typedcontext record nulling, then reloaded for currentG.virtual14, then the same physicalslot is cleared without post-call generation/status check. Earlier reachedprovider can publish an unvalidated currentG.virtual10 return into that same family. Controller support uses distinctW/M/A/Q domains: purgeW0, reloadW1, actually invoke NxReleaseControllerManager(W1[0]), reloadallocatorA1 for conditionaldeleting, then invoke a deallocation boundary on savedW1 and clearcoresupport. Later SDK operations still read independentC11C; no explicitsharedSDKclear/self-release is established here. C0190–C0192; `findings/boundaries/phase5_physics_scene_controller_release.md`.

**Interface breaks / UNKNOWN invariants:** scene entry-generation/provider/currentselector identity; SDK-call-to-cleared-occupant equality; workeralreadyloadedreceiver completion; W0↔W1, M0↔M2 and A0↔A1; currentallocatorvptr; G↔R and sharedSDKlastuser. Named external release and local deallocation are positive invocation, not successful safe retirement or exclusive ownership. These game-service/context/SDK lifecycle entries are classified as internal_lifecycle within the existingPHYSICS_PHYSX taxonomy rather than fabricating a new subsystem or duplicate application-root seam.

## Application/deferred roots/save/Sysutil callback seam — batch6

**VERIFIED local mechanism:** application calls a dispatcher on typedCSiHolder0148CA40 (observed194-byte carrier/100 code slots), distinct from templatedobject holders and the wrapper's touchedCRdMovie cache. Registeredcallbacks capture onlycode, notconstructedobjectreceiver. Dispatcher unlocks/reloadscurrentcarrier+slot forcall, consumesAL1 forcurrentrecordclear, and makesrepeatpasses; rootfinalizers gate currentSaveData70/Sysutil220, then selectedholdercleanup/deallocationboundary/rootclear. Savecleanup polls28, decrements newlyacquiredU220, resetscallbackvptr/deleteslock; Sysutilmembercleanup deleteslocks/callback/embeddedthread beforeholderstorage boundary. C0193–C0195; `findings/boundaries/phase5_save_sysutil_deferred_root_lifetime.md`.

**Interface breaks / UNKNOWN invariants:** successfulregistration/carrier-record-currentroot generation; Uconstruct↔Ucleanup↔Ufinal balancedcounter responsibility; currentA28idle↔sameU+C8 detached/noinflight callback; sameT=U8 admissionclose/lastuse/join beforelock/storage retirement; postcallbackroot/record clear equality. AL1 islocalprogress, notoperation/quiescence success. Enginecarrier application invocation doesnotprove separateCRT `_atexit` finalizer dispatch. The exactprogresshelper00408AF0 remainsunexpanded, notassumed Sleep/activation/join.

## Other globally retained seams

- **Model/resource rebind and packet retirement:** accepted CItem/resource publication, CObject representation policy and car/cache/evaluation interfaces. UNKNOWN old-generation reference release, cache invalidation and final packet use; evaluation is not freshness.
- **Handle/object generation:** accepted current packed-handle validation/release and selected manager retirement. UNKNOWN non-current mappings, repeat registration, stale generation, traversal and deleting safety.
- **Physics/SDK:** accepted same-vector producer/consumer/null/count mechanism. UNKNOWN actual physics-thread activation and invoked wait-before-vector/SDK release/capacity contract.
- **Save/runtime:** separate read/staging/live-record, build/write and Player-creation interfaces are positive. UNKNOWN admission/last-writer/generation/creation chronology and actual I/O success.
- **Presentation/UI/effects:** accepted task/controller/Fade/latch/camera mechanisms. UNKNOWN task/child/free occurrence, repeated Fade identity and generation, lastwriter and event2 destruction.
- **Audio:** accepted descriptor/request records and static-core finite local cleanup. UNKNOWN shared-last-user/worker completion/actual finalizer dispatch/external success/destruction.

These are Phase5 obligations, not rejected Phase4 minima or proof that every static frontier is exhausted. Exact queue rows and 70+18 carry package retain all source/build/runtime limits. No Phase6 transition is authorized.

## Application/Input/Sysutil/callback member seam — batch7

**VERIFIED selectedSTEAM C0196-C0198:** code-onlyregistered00408A30/currentI BC8zero/table0flag1/holdercleanup/deallocationboundary/currentrootclear; U220INC/DEC ondistincttypedprovider observations; I94callbackretainsI/code, vptrreset notdetach; typedT=I8 ordinarymembercleanup; no-op668 andcallbackT25observation withremainingTuse. **UNKNOWN:** successfulregistration/carrier-slot/currentI/currentU generations, balanceddependency/admissionclose/lastuse/stopjoin/operation-success. C0123/C0193 reusedonlyexactly; mode9/sample/commit/API/registryclosed. All70+18guards/K0022-K0023histories retained. findings/boundaries/phase5_input_registered_finalizer_lifetime.md

## Model state/packet/query/scene borrowing seam — batch8

**VERIFIED STEAM C0199/C0200:** actualrebind commoncleanup passesstate/matrix pointers todeallocationboundary thenfieldclears/selectedreplacement; rawmodelownslot→ordinary/membercleanup→basevptr→packet148deallocation/clear BEFOREcurrentvirtual48(0). D8/DC notpacket138; querydetach/scenevectorerase notborrowedpacketlastuse. **UNKNOWN:** actualselectedM/generation/currentvptr, oldpacket invalidation, vector/drawlastuse/success/safety. Thin11-byteadapter narrowsaudioopaquehelper only; noaudio-loopreaudit/GOG/runtime. findings/boundaries/phase5_model_buffer_packet_retirement.md

## Shared app/input delay versus retirement completion — batch9

**VERIFIED STEAM C0201/C0202:** unsignedpair/1000 quotientlow32→importSleep; carrier1000/0→DWORD1 request, localguardprotocol/noLeave anduncheckedreturn. CompleteCCallBackThreadwrapper neveruses/dereferencesthis; latecallbackpointertransport notlateTmemoryaccess, callbackstillbeforecleanup/RET. **UNKNOWN:** sourceclock/actualelapsedtime/runtimebinding/currentparticipantgeneration/admissionclose/detach/lastaccess/join/quiescence. Sleep/progress notnewowner orsafe-retirementfence. findings/boundaries/phase5_shared_progress_delay_retirement_limit.md

## Current Phase5 handoff — sequence83

Ninepositiveconditional-static portfolios throughseq82; currentfirstedges/corrections andquantitativecoverage in reports/PHASE5_PRIMARY_HANDOFF_2026-10-04_SEQ0083.md. Formerthroughbatch5/6/seq77phrases abovearehistorical, notcurrentstop/authorization. SelectedcoldCFishingPerson7F0 continuation isREADYforboundedprimaryqualification, notCMustacheidentity/destructionpromotion oroldgenericchildreopening. Primarycontextoverloadfreshhandoff/ordinarycontinuationauthorized/READY/counters0-0-0, noPhase5closeout/Phase6.

## Typed fishing shell and unresolved descendants — batch10

**VERIFIED STEAM C0203/C0204:** exact3C allocation/value/CMustachesAdmin type retainedP7F0; cold event2 direct00526BA0 thenindependentlyreloadedcurrent-slot0flag1/fieldclear. Helperthreehandles/twoseparatelookups/currentvirtual30, namedshellavailabledeallocation notdescendantdestruction. **UNKNOWN:** construction/currentA/X generations, currenttable/typedchild/lastuse/free safety. Originalordinaryactor/controller/CMustache/CThrow/handlebounds andallseq83corrections retained. findings/boundaries/phase5_fishing_cold_7f0_shell_retirement.md

## Input provider accounting versuscurrentroot retirement — batch11

**VERIFIED STEAM C0205-C0207:** exactconstructorproviderresult INC versuscleanupnewcurrentroot DEC, provideroffsets1C50/60 notInputpointerfields; typedholders registercode004035E0/0070BA50, availablezero-gate/unlock/currenttable0flag1/relock/currentrootclear/AL1. Namedholdermember/backinginterfaces availableconditionally. **SI:** dependencyaccounting interpretation. **UNKNOWN:** gen/balance/actualcarrier/currenttable/admissionclose/lastuser/success. OriginalInput/allseq83guards unchanged. findings/boundaries/phase5_input_provider_accounting_finalizers.md

## Typed light spatialdetach versusretirement — batch12

**VERIFIED STEAM C0208/C0209:** lighttyped48zero-query N/O removalrequest thencurrentO14Cclear; availableowncleanup thenconditionalouterboundary. Ordinarylighttable/helperentry, opaqueO194cleanup, basevptr/helperentry; latercurrenttargets conditional. **SI:** matchingnode-pointerremoval/C0141 unchanged. **UNKNOWN:** actualcurrentdispatch/ownslotinvocation/O-Ngen/aliaseslastuse/success/GOG. findings/boundaries/phase5_spatial_light_detach_cleanup.md

## Compact audio own-interface availability — sequence90

**VERIFIED STEAM C0210:** typedavailableowninterface cleanupbeforeargbit0optionalreceiverboundary; argumentflagnotsharedusercount. **UNKNOWN:** current0138A6E4root→actualinvocation/sharedlastuser/completion/success. Compactavailability supplementnotnewsubstantialshared-lifetimeportfolio. findings/boundaries/phase5_audio_singleton_cleanup_interface_limit.md

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

## Sequence105 — post-Game cold integration selection

Globalfive-familyreviewselectsactualselectiveaudio-client h->typedcurrentMain operation qualification, notanotheravailableGame/rootdestructor. ModelCRdInterp backing isstrongalternate. Allfirststrongergen/lastuse edges/corrections retained; no new executablecredit. reports/PHASE5_POST_CGAME_GLOBAL_REVIEW_2026-10-04_SEQ0105.md

## Sequence106 — selective audio client operand andrecord mapping

**VERIFIED Steam C0224/C0225:** postgetterclienth/unusedthiswrapper -> typedcurrentMain; fullh/low8/50stride/currentIDmatch -> interiorRor0; selectedq0 R18/exactAL1/T-M/U interfaces, no directrecordclear. **UNKNOWN:** h_guard-h_reload/creator-currentA-R-M generation, i<128 admission, opaqueeffects/stack/finalarg/type/lastuse/safe release. This isselectiveoperandqualification, notfullretirementportfolio. findings/boundaries/phase5_audio_client_selected_record_operation.md

## Sequence107 — disconnect recovery and fresh-primary handoff

Governancehandoffafteruserrequestedcontextend/connectionloss: seq106research/promotion andfouractualchecks completed; pending-proof/STATUS/journal/freshhandoff gaps preserved/correctedclerically. All11ledgers/debt unchanged. GameC0222/C0223 andAudioC0224/C0225 scopes/firstedges conserved; no newprimaryclaim/frontier. Currentpendingglobalreview/STRATEGIC_REVIEW_REQUIRED/lastADVANCE/counters0-0-1 remain. ColdModelCRdInterp candidate documentedonlyfornextreview. reports/PHASE5_PRIMARY_HANDOFF_2026-10-04_SEQ0107.md

## Sequence108 — post-audio global reassessment

Postaudio globalreview credits retainedpositive scopes and individually requiredworld/cache dependencies; chooseNPCevent2 routing-first missinginvocation/effect overadditional predictable Model backingmechanics. Movie strongestbreadthalternate; Model strongertypedlowambiguity alternate; saveevent conditionalreserve. No newprimary/semanticcredit; allfirststrongergen/lastuse edges preserved. reports/PHASE5_POST_AUDIO_GLOBAL_REVIEW_2026-10-04_SEQ0108.md

## Sequence109 — NPC event2 nominated-arm routing limit

**VERIFIED Steam C0226:** event2map02/selected0047F8E2 invokescurrentNtable+90 thenJMPcommon-tail; nominateddirect0047F92C->0047F770 notinthatarm. **UNKNOWN:** currenttarget/effect/transitive/tail/Ngen/lastuse. BOUNDED_NEGATIVE candidateonly; coldcleanupbranch notlaunched, nootherarm. findings/boundaries/phase5_npc_event2_selected_virtual90_routing_limit.md

## Sequence110 — post-NPC media/helper producer selection

NPCevent2candidate-localnegative recorded/validated; fivefamilycomparison choosesactualCRdMovie helperproduction/setup-to-cleanup rendereroperand seam overpredictableModelbacking orwarmAudio/requiredjoins withnonewtypedsource. Allcurrentfirststrongergen/lastuse dependencies andfailuresretained; no governancesemanticcredit. reports/PHASE5_POST_NPC_EVENT2_GLOBAL_REVIEW_2026-10-04_SEQ0110.md

## Sequence111 — prospective media finite-contract correction

MaincutoffassertionbeforeactualM8publication failedbeforePEreceipt/canonicalpromotion; diagnostic8bytesbeforeminimumallocation sourcepreservedasoriginalscopeNOTPASS. Prospective samefourbodies/fivewindows128instructions448bytes, main48/176branch80/272 beforector/setupbodyacquisition. No semanticcredit/cleanhistoricalpass. reports/PHASE5_MOVIE_FINITE_CONTRACT_AMENDMENT_2026-10-04_SEQ0111.md

## Sequence112 — exact constructor-table type qualifier

Constructor-written0076E68C classUNKNOWN motivatesprospectiveexactCOL/name metadata(max88bytes), samecodecontract/no otherRTTI/slot/body. Main41/168priorreceiptunpromoted; freshsetupbranchscopeunchanged. Nosemantic/countercredit; originalsourceNOTPASS/failedchecks retained. reports/PHASE5_MOVIE_EXACT_TYPE_DISCRIMINATOR_2026-10-05_SEQ0112.md

## Sequence113 — Movie constructed texture and descriptor interfaces

**VERIFIED Steam C0227-C0229:** Moviealloc20/namedCRdTexture ctor->M8pub/currentreload/sevenargs; Hcleanupbefore s6guard; localdescriptoraddress->literal0148BBD8/opaque006CD010; two01480984observations/currenttable5C; commonnumeric8/C/E stores andpartialAX EAXresidue. **UNKNOWN:** currentH/M8/table/resourcegen/type/API/effect/heapretirement/lastuse. OriginalsourceNOTPASS retained; prospective128/448+88metadata fitPASS beforepromotion. findings/boundaries/phase5_crdmovie_constructed_texture_setup_descriptor.md

## Sequence114 — post-Movie exact composite consumer selection

Independentseven-card reviewselectsnewtypedcompositedescriptor exact006CD010 localconsumer, notname/size-basedregistryidentity oropaque-callee momentum. Modelstrongalternate; strongerrequiredworld/cache/worker/provider/Game/audiojoins unwaived. Samebroadmedia-rendererfamily, explicitreviewreset only; no semanticcredit. reports/PHASE5_POST_MOVIE_GLOBAL_REVIEW_2026-10-05_SEQ0114.md

## Sequence115 — composite consumer capture limit

**VERIFIED Steam C0230:** currentunsignedtwo-armDpointerrequests, conditionalR10=opaqueEAX, two-wordcopyfromopaqueP(notDalias), RET4unqualifiedbits. No localDfields/Hcapture. Capture/retention/currentR-D-P-H-gen/lastuse **UNKNOWN/NOT_READY**;45/115onebody only, no supportcallee/oldregistrychain rescue. findings/boundaries/phase5_texture_descriptor_consumer_request_limit.md

## Sequence116 — globalreview andfresh Model backing successor

AfterNPCroutingnegative/Moviepositive/compositecaptureNOT_READY, fullseven-familycomparison selectscoldModelEbacking-versus-resource helper fornextfreshprimaryonly; notautomaticpriorranking. Currentcanonicalstatevalid,butreceiver/gen/amendmenthistory crowding threatensadditionalacquisitiondiscipline. No newModelbody/evidence, no semanticcredit/closeout/Phase6. reports/PHASE5_POST_COMPOSITE_GLOBAL_REVIEW_2026-10-05_SEQ0116.md

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
