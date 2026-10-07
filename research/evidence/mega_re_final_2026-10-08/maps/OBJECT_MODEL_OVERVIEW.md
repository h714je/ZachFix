# Object Model Overview — Phase 2 closeout (historical start preserved)

**Started:** 2026-10-01  
**Evidence policy:** construction/vtable facts are `VERIFIED`; behavior and ownership remain separately labeled.

## Phase 1 construction skeleton

- Selector factories Steam `005E7620` / GOG `005E76F0` are the principal paired object-construction source.
- 170 explicit Steam/GOG constructor homology groups connect matched class roots.
- 344 build-specific class/object records and 1,024 build-specific vtable records are present.
- Multi-vtable splits are retained rather than merged:
  - CPlayer outer object with CMustachesAdmin embedded at `+0x80C`.
  - CNpcKiller outer object with CNpcRecord embedded at `+0xD28`.
  - CTarBomb and chain-effect families with embedded CSoundLoop objects.
- Global `01481358` is a static `CRdHandleUtil` generation-validated object-handle registry with `0xA00` pointer slots and `0x7FFF` generation flags; active-object registration stores allocated packed handles at outer-object `+0x3C`. Registration is not idempotent: CThrowLure proves a repeated call allocates a fresh mapping, overwrites `+0x3C`, and does not release the former mapping.
- Global `00BD7670` roots a lazily allocated `0x6778`-byte active-object-manager candidate; its exact RTTI class name remains unresolved. Reset first broadcasts an active-list virtual `+0x30` dispatcher with selector `0`, then calls generic retirement. The virtual marker is bypassed when manager `+0x18E0 == 2`; generic release consumes only an object’s current `+0x3C` handle.

## Major roots for Phase 2

1. `01481358` CRdHandleUtil: allocator, release, static lifecycle, and the active-object-manager boundary are resolved; recover remaining producer taxonomy and reuse behavior.
2. CPlayer/CMustachesAdmin: outer registration is resolved; CMustachesAdmin has paired helper lifecycle and no direct independent registry evidence across CPlayer, CFishingPerson, and CMenu forms. Pursue only direct virtual consumers or parent cleanup paths.
3. CNpcKiller/CNpcRecord: outer factory-to-manager/registry bridge is verified through transient descriptor byte `+0x30 == 7 -> selector 0x08`; CNpcRecord has vtable-only helpers and no direct independent registration/release path in reviewed lifecycle code. Recover descriptor ownership and indirect CNpcRecord virtual consumers.
4. CObjectDoor sibling family: map base/subclass lifecycle, vtable override slots, and world-resource ownership.
5. CRdData, XMD/CRdMesh, and high-fanout native-task dispatcher: connect identified boundaries to concrete object state and vtable behavior.

## Embedded-object lifecycle rule

`STRONG_INFERENCE`: CPlayer/CMustachesAdmin, CNpcKiller/CNpcRecord, CFishingPerson/CMustachesAdmin, and CMenu/CMustachesAdmin have no direct evidence that the embedded/helper object itself is registered merely from its vtable or allocation. Separate allocation alone does not establish independent game-object ownership. **Verified exception in scope:** Steam CMustachesAdmin is a parent/controller for three separately registered CMustache children, whose packed handles reside in CMustachesAdmin `+0x2C/+0x30/+0x34`; this does not make the CMustachesAdmin helper itself an active object. CFishingPerson teardown’s reviewed broadcast sets a selected object control bit and dispatches event 13; it is not a child release or retirement path.

## Explicit limits

The selector census proves construction topology, not a complete inheritance hierarchy. Vtable slot extent, virtual dispatch semantics, ownership, and render/update relationships require dedicated Phase 2 evidence. CThrowLure proves duplicate registration can leave an overwritten stale mapping. Retirement is an active-list iterator gated by `+0x29 & 0x80`, and its paired direct marker is CThrowLure vtable `+0x30`; the self-link prevents clean source-level cursor progression when the enabled reset marker path reaches the object. Runtime frequency, manager-state gating frequency, and unreviewed indirect cleanup remain unresolved.

## Current Phase 2 portfolio — 2026-10-02 sequence 6

The Phase 1 numbers above remain a historical start snapshot, not current coverage. Current report: reports/PHASE2_EXIT_READINESS.md (NOT_READY; all 21 seeds). VERIFIED conditional CShop construction/registration/callback/update/retirement/deletion is connected; XPC 0x10 typed wrapper has a manager owner/delete interface and concrete CLevel retainer; CRdMovie has a separate named shell/shared graph/session cleanup root. CLevel pointer retention is not independent owning-reference proof; movie session cleanup is not singleton free. See C0111-C0114 and current focused findings.

The outer owner of the level array +0xA3F98 is still UNKNOWN and selected for receiver acquisition/type/root work. The central manager/table/interface receipt remains an explicit closeout gap. Embedded/helper and CThrowLure self-link/current-handle limits remain intact. Steam-first mapped-anchor reuse and GOG raw/export exceptions follow maps/STEAM_GOG_HOMOLOGY_REUSE.md. Phase 3 is not started.

## Static CMap world-root reduction — 2026-10-02 sequence 7

VERIFIED C0115/BND-083: static in-place CMap 013936F0, constructor 005D0680/005D0750, raw RTTI tables 00779AB4/00779AA4, 64 retained CLevel slots +0xA3F98 = 01437688, raw construction/atexit-finalizer registration. This supersedes the preceding world-owner UNKNOWN wording only; exclusive level lifetime, complete update placement and level teardown remain UNKNOWN. Root is distinct from CLevel and 00BD7670. See findings/boundaries/cmap_world_level_owner_root.md. World-root acquisition blocker resolved; selected central active-manager interface still blocks readiness. No Phase 3 started.

## Selected central interface integration — 2026-10-02 sequence 8

VERIFIED C0116/BND-084: application 00BD7670 aliases startup publication of 00BD9E68 CSingleton<CRdSceneDraw>, with CRdSceneDraw/Cross/Scene RTTI lineage. Its one-slot own deleting tables are distinct from the 19-slot CRdObject consumer table. Direct registration/dispatch/retirement and CShop class targets are connected; embedded render members are not independent active objects. Shutdown cleanup/app-root clear is not full destructor/optional allocation free. See findings/boundaries/crdscenedraw_active_manager_interface.md. Historical candidate/free-site wording above is superseded at this scope, not silently erased. Both selected blockers reduced; dedicated closeout judgment pending, no Phase 3 begun.

## Phase4 resource request object — sequence33

**VERIFIED Steam:** CLoadThread is a separate in-place static01481130 object withraw008268F4/CThfuncPMD0,observedsize lowerbound41 andactualinitializer-invokedsame-receiverOSargument/worker-slot4 route. It retainspackedscalarrequests inembedded24 contiguousstorage andseparate direct3C/3E/40 state; it isnotCRdData/CFlkUtil oranelement-pointerowner bysharedhelper naming. Safe retirement/allocation/free andper-requestmulticalleridentity UNKNOWN. See C0152-C0154, findings/boundaries/resource_worker_typed_handoff.md. AcceptedPhase2/3 scopes andhistoricalnotes unchanged; no Phase4closeout/Phase5.

## Phase 2 closeout decision — 2026-10-02 sequence 9

Selected static object/root/interface/factory milestone accepted under the current user closeout-and-stop authorization. Current identity/ownership conclusions are C0111-C0116 and their focused primary-backed findings; preceding chronological start/candidate notes do not override their corrections. `reports/PHASE2_CLOSEOUT_2026-10-02.md` red-teams all eight requirements; readiness/root matrix and the all-row carry-forward review preserve precise missing edges. Actual cached manager allocation free, exclusive CLevel lifetime/unload coordination, complete mode placement, broader subsystem roots and runtime cadence remain unresolved. No Phase 3 work or census-completion claim. Last-active control phase stays PHASE_2_ACTIVE parked at PHASE_TRANSITION_REVIEW pending explicit Phase 3 authorization.

## Phase4 audio retained-value responsibilities — sequence35

**VERIFIED selectedSteam C0155-C0157:** CSound6604/54AC retainparameter/listviews; named54B0[64] retainsinlineP=node20 fromPLSE066.PCM lookup, notanowning payloadpointer. Main128x50 rowsgenerateT/storecoreK at18; core32x24 rowsretainindependentstatusQ/backreferenceA andconditionalforeignoutput14; statusrecordretainsP/name. Same-P raw4/14/2C consumers aremechanical, actualselectedfields/type/population/lifetime/bank/API/creation UNKNOWN. Lookupkeynode18 differsfromnamefieldnode4C. findings/boundaries/audio_descriptor_request_record_chain.md. ExactC0118 reused; no fullaudio/free/cadence/GOG/Phase4closeout/Phase5.

## Phase4 UI/shared-game numeric responsibilities — sequence36

**VERIFIED selectedSteam C0158/C0159:** CMenu40policy/28substate/floatprogress/80latchedvalue controlsconditionalrequest85,999; CGamehelpercanproduceglobal008A9514literal85 pointer, separatefromordinaryindexedlist836270. ActualgetterG983C4 uses32bitSUB999/signednegativeclamp, under98E48inequality avoidinguntypedactor sidearm. Lookup/globalpointer returnisnotownership/itemallocation; incomingauxR andgetterG staydistinct. Actualmode/list/count/slotdomain/sidearm/refresh/lifetimes unknown. Existingrecord0containment isreuse, notdisk-save transaction. findings/boundaries/cmenu_numeric_game_slot_commit.md.

## Phase4 sequence50 additive CItem actor/resource slice

findings/boundaries/citem_factory_value_resource_binding.md; C0171-C0172/BND-154-157. SelectedoriginalCItem allocation/native0/value4A8 and name-buffer/typedCRdData actualreturn/sameobject160/164 mechanism. This is an additivePhase4class/data slice, not a regrade ofacceptedPhase1-3 organizingroots or a Player-owned/wholeactor/loader/format/success/lifetime claim. Root/value generations, resourcepointees andretirement remainunknown.

## Phase4 sequence54 — distinct model state and render packet

**VERIFIED selectedSteam C0176/C0177:** CObjectCar originalpointer ownsselectedembeddedinputstate58/68/78/base98 andusescurrent160/1E4/1E8 modelinterface underacceptedguards. Sameactorinputcopy/preparation/explicitcurrentevaluation linked, but multiply-result preservationacrossopaquetail unknown. Rawinstalled28/44 conditionalpacketconsumer isnot actualqueryselection/freshness. M98 embeddedmatrix/M1E8 outputbuffer/M148 packetpointer/P80 packetbacking remainseparate. findings/boundaries/cobjectcar_model_base_cache_production.md. No newexclusiveowner/free/GOG/frame proof.

## Phase4 sequence56 — Static receiver versus absolute cleanup storage

**VERIFIED Steam C0178/C0179/BND-164-166:** literalstaticCSdCore014B0400 ->0076DF60/0072C2E0 local sharedbackend attempts andabsoluteA12/B13/Vstorage cleanup. A CloseHandleattempts lack localclear; B opaque0074EE0B calls followedbydwordclear; Vunmapfollowedbyclear/CoUninitialize. Theseare notreceiverfields orlazy0138A6E4 free. Actualdispatch/order/userquiescence/lastuse/helpersuccess/destruction/COMbalance/runtime/GOG remainUNKNOWN. findings/boundaries/audio_static_core_backend_cleanup.md. Prior acquired/task/use scopes reusedonlyexactly; noPhase4closeout/Phase5.

## Phase5 current ownership/lifetime qualification — through batch5, 2026-10-04

Phase4 is accepted/closed at conditional-static subsystem architecture, under audit/PHASE4_CLOSEOUT_AUDIT_2026-10-04.md; earlier noPhase5/candidate wording is historical. Current interacting-system authority: maps/CROSS_SUBSYSTEM_DATAFLOW.md and findings/subsystems/PHASE5_BOUNDARY_PORTFOLIO.md. All70+18 obligations survive. Inherited owns/owner means at most evidenced creation/retention/local cleanup responsibility, not exclusive or safe lifetime. DSB is16-bit slot withFFFF sentinel.

VERIFIED C0180-C0192 at listedlocalinterfaces: CLevel/model160/164 retainers clearbefore-rebind and canescape to packet84/88; CLoadThread logicalstop andseparatebackingstorage destructor differ; CMenu contains selectedinlineCLayout6EE8 distinctfromregisteredtask/Fade+8 retainer; CFishingPerson event2currentline83Cslot0flag1/clear differsfromline4/C marking andunmatchedline8lure; CPhysicsCore retainsW/M/A/S/G/R domains withactualnamedMrelease/localdeallocation-boundary invocation butnogeneration/lastuse/SDKownershipproof. WnominalclassUNKNOWN; allocatorControllerManagerAllocator separatelyrawtyped. Theseareobject/reference responsibilities, notwhole-classalgorithms orsame-occurrence safeownership.

DISPROVEN C0047/C0086 setup+4C locator/H0209same-slotbasis: pairedrawCLevel tables correctedbyC0182/K0022. Originalfindings/helperbody/H0210 scope retainedseparately. K0023selectedphysicsglue role supersedesFID/import-onlytaxonomy atselecteduse, notcompiled-origin/sharedfoldingproof. Allunprovedgeneration/deleting/finalizerdispatch/cadence axes remainUNKNOWN.

## Phase5 batch6 — code carrier versus current object holders

VERIFIED C0193-C0195: namedCSiHolder0148CA40/raw00826D8C isobserved194-byte/100-code-slot carrier, nottemplateTSiHolder<CSaveData>/<CSysutil>/<CInput> objectcache. Itretainscodeonly; current-root finalizers loadnewA/U observations, gate70/220, invokeslot0flag1/cleanup/deallocationboundary andclearroots. SaveDataidle28/depU220/callbackvptr andSysutilguard/callback/embeddedCCallBackThreadU8 cleanup arelocalmechanisms, notC8detach/workerjoin/bufferownership orsamegeneration. DistinctMoviecache00BD9E48 touchedbywrapper isnotcarrier. Newroot/memberclasses returntocanonicalmapwithoutinventingnewmajorfamily orfree-success. findings/boundaries/phase5_save_sysutil_deferred_root_lifetime.md.

Currentseq77fresh-primary successor isCInputregistered00408A30/BC8/004098C0->00708280 lifetime. Allsixportfolios andfirstedges in findings/subsystems/PHASE5_BOUNDARY_PORTFOLIO.md. Contextoverloadhandoff, notPhase5closeout; noPhase6.

## Phase5 sequence94 — Rain/Grid retainer andcleanup distinction

**VERIFIED selectedSteam interfaces:** staticCMap+A3D24 currentpointer →currentvirtual30→normalreturnfixedfieldstore. NamedRain slot0 callsordinarycleanup beforeargbit0optionalouterallocationboundary; ordinary supplies E=Rain+2424 toGridhelper, which conditionallypasses P=E3C (Rain2460) toacceptedopaqueadapter thenfieldzero. **STRONG_INFERENCE:** embeddedGridcleanup/storage interpretation. **UNKNOWN:** currenttable/generation, selecteddeletingoccurrence, Ptype/ownership/finaluser, successfulcomplete destruction. Do notjoinconstruct/current/postcallfield generations fromequaladdresses. findings/boundaries/phase5_weather_retainer_grid_retirement.md

## Batch14 — Playernumericoperand tophysicsretainer request/clear

**VERIFIED STEAM C0214/C0215:** ordinaryPlayer94C/950 guards→independentcurrentroot/postgetter numericreload→fullDWORDindexedQ/currentW bridge→normalreturncoreslotzero; availableownordinarybeforeoptionalPouterboundary. **SI:** selectedgamephysics-support role. **UNKNOWN:** actualP/indexdomain/C-Q-W-M/table/gen/lastuse/success; notSDKquiescence/childdestruction. findings/boundaries/phase5_player_physics_dependency_teardown.md

## Batch15 — worldreset andseparateLensflare shellpublication

**VERIFIED STEAM C0216/C0217:** invokedCMaprelativezeros/Rain-Haze retainerclears/two64DWORD ranges; separatecold454L zero-fill→opaqueVadapter→namedLensflarewrappertable→fixedcachepublication/commonL8zero→independenttypedactiveD6744zero. Genericadapter passes&argumentslot; retainedcapacityarmcopiesslotvalue. **UNKNOWN:** type/value/stackcontinuity/generation/registeredcleanup/ownership/lastuse/success; notlevelvector oroldN58. findings/boundaries/phase5_cmap_reset_lensflare_cache_publication.md

## Batch16 — save/Gameinlinevalues andworldslot-fold integration

**VERIFIED STEAM C0218/C0219:** K3/4skip; otherpreparationGentry→runtimebyte/optional-or-currenthandle4DWORDcopies into99048/99058;64worldslots nonzero samples→one32bitrotatingfold at99068. BuilderlaterG2reacquire beforecopy. **UNKNOWN:** sourcevalidity/currentR1-R2/world/G1-G2gen/coherentstaging/owner/lastuse/success; not64independentbits/synchronization. findings/boundaries/phase5_save_preparation_world_player_values.md

## Compactqualification — CCarretainsGameinterioraddress

**VERIFIED STEAM C0220/C0221:** producerselectedn0..127 returnssavedGame+CD368+4n/fail0, acceptedCarretainsaddress73C. Secondleafincomingthisunused/globalbyteequals1; firstleafopaquequeryQthenQ0secondreturn. **UNKNOWN:** cellcontents/currentG-U-bytegeneration/typedactor/owner/lastuse/success. Notnewphysicalvehicleowner orfullsafe-lifetimeportfolio. findings/boundaries/phase5_ccar_retained_game_interior_address.md

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
