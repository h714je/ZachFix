# Final high-signal engine address map

**Build-specific identity index, not complete semantic coverage.** Full selected-node inventory: `audit/phase8_synthesis_2026-10-07/FLOW_NODE_INVENTORY.csv` (232 unique build/address locators:230 structural entries,2 site/unassigned locators). Each row keeps build, VA, RVA, original auto-name, historical canonical name, selected synthesis role, homology IDs, corrected structural status, qualification and source-map line separate. Site locators are not invented functions. A name/index does not certify a whole body or ABI.

Image base00400000 in both independently identified32-bit builds; RVA=VA−base. Build hashes and the complete pair/qualification table are in [Steam/GOG correspondence](../maps/STEAM_GOG_FLOW_CORRESPONDENCE.md). CSV homology IDs are historical identities; current matched companions, especially H0232 renderer correction, govern reliance.

## Major function nodes by build

| Role | Steam VA | GOG VA | Selected scope |
|---|---|---|---|
| Application loop |00700650|00700670|Eligible idle/message/timing/tick paths |
| Tick |00401A70|00401A70|Independent exact fragment/path checks, not equality-based mapping |
| Seconds clock |00701040|00700FA0|QPC/frequency arithmetic |
| Object dispatcher |006C5FF0|006C5AF0|Conditional interfaces/markers; repaired GOG/export scope |
| Context selector |0041C270|0041C290|Bit31 operand, independent reads |
| Pre-render / render consumption |006D2B40 /006D32E0|006D2710 /006D2EB0|Selected context/query/packet/secondary-pass paths |
| Device init |006CC290|006CBD30|PP/CreateDevice/global requests, success unknown |
| Cooperative / loss / recreate |006CCEF0 /006CC030 /006CBD00|006CC990 /006CBAD0 /006CB7A0|Corrected output-cell request protocols |
|2D descriptor accessor |006CCFE0|006CCA80|`[owner+C]+index*18`, not inline |
| COM-value lookup |006CBAC0|006CB560|`[[descriptor+10]]`, first match |
| Reflection builder / clipping |006D9340 /006D93D0|006D8F10 /006D8FA0|Plane/matrix/clip requests |
| Reflection bias caller |006E07D0|006E0760|Two width reads; bug causality unknown |
| Input commit / producer |00708350 /00708B00|00708300 /00708AB0|Distinct live/pending/aggregate/action state |
| Acquisition / filter |00709C40 /00709400|00709BA0 /007093B0|Raw/action/filtered numerical seam |
| Input masks / axes |00708910 /00708A30|007088C0 /007089E0|Readonly current-record operands |
| Resource getter / callback |004051F0 /00408310|004051C0 /004082D0|Current root and typed conditional branch |
| Load worker / dispatcher |006B4DE0 /006B4F20|006B4CC0 /006B4E00|Queue/direct service, not success certificate |
| XPC parser / child accessor |006B5190 /006B5660|006B50E0 /006B56A0|Current-child/metadata address mechanics |
| XMD normalizer / model binding |0070BD20 /006BE6E0|0070BCC0 /006BE1F0|Typed state/early pointer/conditional preparation |
| XAM / XCA setup |006B9020 /006B9E70|006B8F70 /006B9DC0|Qualified pointer/array/rejection protocols |
| Physics submit / timing |006EB3C0 /006EAE20|006EB3D0 /006EAE30|Scalar/timing requests; ABI namesSI |
| Synchronous halves |0040B850 /0040B940|0040B820 /0040B910|Untested result/current population limits |
| Audio Core getter / ctor |0046F9D0 /0072BA00|0046FAD0 /0072B710|Named selected/static construction scope |
| Audio Main token generator |00720890|007205A0|Wrap/reconstruction/full-ID qualification |
| Audio speaker / DSP helpers |0072DA80 /0072DB80 /0072DCB0|UNKNOWN whole-flow counterpart|Steam checked speaker/count/handle/application path |
| Audio sentinel installer/callback |0071FED0 /0071FD50|UNKNOWN selected counterpart|Callback store/reuse before later cleanup |
| NativeUI directory category |0045A590|0045A5C0|Caller-relative directory cell/body distinction |
| CGame restore / player data consumer |0061A830 /00509080|UNKNOWN selected genealogy|No same-address transfer; load episode unknown |

**VERIFIED local mechanics / STRONG_INFERENCE interpreted correspondence / UNKNOWN stronger joins** are scoped per linked map/CSV row. This compressed table does not override them.

## Important roots and state cells

| Cell / object | Steam established role | GOG qualification |
|---|---|---|
|01480988 /01480984|Direct3D interface /current device |Independently read in selected device initializer, not all globals presumed identical |
|0148BB70|Saved Reset presentation block |Independently selected PP copy/refresh overwrite |
|0148BBD8 /BBF0 /BC08 /BC20|2D/cube/volume/VB descriptor-bank receivers |Paired independent literal/operand checks; names/owners unknown |
|014AFFE0 /014AFFD4 /014AFFF0|Gameplay scalar copies /frequency state |Paired clock/publication mechanics |
|00BD7670 /00BD9E68|Application SceneDraw alias /cached root |Qualified acquired-root/interface scopes, no stable generation |
|00BD9E60 /00BD7794|CRdModel cache/config service /startup alias |Available member/service role, not full renderer authority |
|00BD9E3C /00BD768C|CRdData cache /startup application alias |Independently named/current request roots at mapped getter scopes |
|01481130|Static CLoadThread object |Independent type/ctor/worker/static operand scope |
|00BD9E10|TSiHolder<CInput> zero-offset Steam input receiver |GOG pipeline operands matched; holder/provider/lifetime not wholesale transferred |
|00BE1EA4|Gameplay CCamera root |Selected A06/root reacquisition; not render-context frustum storage |
|00BDA0C8 /01493FA8|PhysicsCore cache /shared SDK cell |Independently selected type/root; full scene/API/ownership topology unknown |
|00BDA010|Four in-place CPhysicsThread contexts |Steam concrete same-vector/available activation; GOG scope not implied |
|0138A6E0 /00BDBCC0 /0138A6E4|CSound /CSdMain /CSdCore caches |Core and Main primitive scope paired; complete coordinator unknown |
|014B0400 /014B01E4|Static Core /shared backend interface |Static Core construction paired; deepest cleanup/useSteam-qualified |
|014B01E8 /014B02F0|X3DAudio handle /DSP settings |Steam exact handle/count/mask distinction; no invented GOG flow |
|00BDA004|Shared CGame root, offered838E30 |Selected restore/player genealogySteam-only |
|013936F0 /01437688|Static CMap /selected64level-retainer range |Selected state/reset/resource use scope not universal paired owner |
|00BE5970 /00BE5EF0|CPreserve /staging image |Selected Steam protocol, not durable/coherent transaction |
|00BD9FF8 /00BD9E0C|TSiHolder<CSaveData> /CSysutil roots |Selected Steam callback/pointer/count protocol |
|00BD9E54 /00BDA0F8|EffectAdmin /ItemManager cached service roots |A16/A17 independently scoped requests; tables4/C installation unknown |

Root caches, aliases, global cells and in-place objects are different identities. Fields and responsibilities are detailed in [object model](DP_OBJECT_MODEL_FINAL.md) and the flow maps.

## Selected vtables and class identities

| Class / table role | Steam | GOG | Scope |
|---|---|---|---|
| CRdTexture constructed table |0076E68C|Qualified separate construction; full type not freshly generalized|Named selected0x20-byte recipe, no runtime ownership |
| CRdMesh /CRdPicture |0076F2AC /0076F2B4|0076F29C /0076F2A4|Typed callback wrapper interfaces |
| CRdObjectModel /CPlayer |0076E704 /007767FC|Unknown full selected packet/type genealogy|Installed10/24/28/44/48 relationships at Steam scope |
| CInput base /holder |00827604 /0076F5DC|No whole holder transfer|Named zero-offset Steam type/available callback scope |
| CPhysicsCore /singleton |0076FE04 /0076FE3C|Independently named in matched companion scope|One-slot acquisition/available cleanup distinct from SDK interfaces |
| CLoadThread |008268F4|008268E4|Slot4 build-local worker; CThfunc base0 |
| CSdCore base /singleton |00828548 /00773154|00828078 /00773144|Named Core construction; not backend COM vptr |
| CBossBase /George2 /George3 |00782834 /00782ABC /00783094|00782824 /00782AAC /00783084|Checkpoint238 selected construction/class companions |
| CMotionDriver |00783404|007833F4|Offered11C construction/backpointer/configuration, ownership unknown |
| CNpcEnemy |00774574|00774564|Separate named table170 method association |

Vtable declarations/slot availability are not proof of actual current dynamic receivers. All canonical CLASS/VTABLE/HOMOLOGY ledgers remain unchanged; companion overrides and selected-source pointers preserve historical corrections.

## Post-synthesis current supplement

Phase8 remains COMPLETE;[post-Phase8 white/gray closure](../maps/POST_PHASE8_GAP_CLOSURE_INDEX.md) refines six selectedstaticbridges (conditionalSteamshader/audioinput ceiling) andnarrowsoneActuatorABI gap. Originalcheckpoint243 report/package/validation remains preserved. Newclaim/boundary/function/site/class/global/vtable/homology/resource/subsystem companions live separately under`audit/post_phase8_closure_2026-10-07/`; tenhistoricalsemanticledgers unchanged.62entrylocators/48sitewindows/13declaredtables/17localcorrespondencegroups are scopedindexes, not62newly understoodfunctions orwholeenginecoverage. All88/checkpoint238 guards/renderer236/Phase6 DO_NOT_RENEW retained; currentepisode/success/ownership/runtime/bugcause limits notwaived.


## Targeted bridge campaign — current supplement

**VERIFIED selected static scope; stronger guarantees explicitly retained:** New scoped address/site indexes contain41 function locators,75 context windows,7 selected type/table anchors and16 independent local pairs. Raw GOG callback and exact dispatcher prefixes remain sites, not invented recovered function identities. Historical canonical rows/addresses remain sealed. See [the targeted bridge index](../maps/POST_PHASE8_TARGETED_BRIDGE_INDEX.md), `audit/targeted_bridges_2026-10-07/BRIDGE_REGISTER.csv`, reconciliation/frontier/handoff and manifest-bound validation. Protected checkpoint245 copies preserve the previous report. Ten canonical semantic ledgers/all88/overlays/renderer236/Phase6 DO_NOT_RENEW unchanged; no runtime or implementation work.


## Final static campaign amendment — FSC001 / checkpoint252

**VERIFIED selected mechanics; STRONG_INFERENCE conditional composition.** New current companions connect busy-gated `00401050` key/payload insertion and comparator `00401020` to the accepted `00401080` drain. Selected Steam event0x12 paths (including only the established CHelp-installed path into00633B80) enqueue key0 then exit on admission; available independently named CObjectTarget slot98 methods005B19A0/GOG005B1A70 offer a wrapping460+0xA key then exit or request localfallback. Own-build producer/comparator/drain/type-method correspondence is qualified, not numeric-address transfer. Comparator equalkeys return-1/nozero: no stable/total/FIFO order. Hook mutation/current callback generations, arbitrary payload type/key domain/capacity, actualdraw/cadence/globalserialization/ownership/lastuse remainUNKNOWN. Details/current companion identity: `findings/boundaries/final_static_optional_record_admission.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical Phase8/through250 provenance remains intact.

Accounting qualification: original20,252/15,072 UNKNOWN labels and corrected21,871 entries remain unchanged. Seven selected role entries and three site-prefix locators are scoped evidence, not complete-function understanding; semantic fraction remains UNKNOWN.


## Final static campaign amendment — checkpoint253

**VERIFIED selected mechanics / STRONG_INFERENCE family:** shared child attachment setter0071EDE0 retains a parent-related token/index/localvectors;0071EEF0 requests indexedparent matrix006C3B80 and local-left/parent-right multiplication, storeschildposition thenoptionalorientation. Descriptor00461100 and004A1960/006A9F10 supplycreatedchildECX, notoriginalparent. Setup/earlyexits are nontransactional;210mask1 skipsorientation only;0xD48 isofferedlookupdiscriminator and comparedhelperRESULTs; reset uses signedcurrentcount/separatelyreloadedbacking. Dynamicclass/indexadmission/ownership/currentpose/packetfreshness/success/schedule/GOG remainUNKNOWN. Thisconnectsdescriptor/gameplaycreation toanimation/modelmatrix/world-transformuse; it doesnot establish streaming-subsystem ownership.

Evidence/qualification: `findings/boundaries/final_static_parent_matrix_attachment.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint254

**VERIFIED selected mechanics / STRONG_INFERENCE typedfamily:** CPasswordselectedthreebytes641 andCChesssixsymbolorder124035 haveclass-provenreset/callbackpaths, lowbyte0setup/1update/2cleanuprequests/0x12drawrequests andselectedphase/data transitions. Steamidentifierlookup0070E9A0 scans32currentCEvCore-relatedslots butre-fetchesup to3times; finalpointer+38 isconditional, notstablematchedbank/null-safe/observedgameoutcome. Passwordcancel2phase1 canoverridesubmit;1/0phase2/3,Chesscancelcount<=0/deletelastpositive andfeedback-gated1/0. Phase4predicate-gatedvirtual30TAIL isnotcompletedretirement.16own-buildrolepairsSI doNOT transferexternalcallees/globals orGOGlookupalgorithm. Fourlarge methods/buildselectedEXPORT_ATTRIBUTION/PEtablepaths,notreconstructedwholebodies.

Evidence/qualification: `findings/subsystems/final_static_password_chess_puzzle_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint255

**VERIFIED selected mechanics / STRONG_INFERENCE family:** World progress/prefetch feeds multiple XPM forms and model-key NxStream imports;97initialSteamXPM/XMD ID pairs plusTREEPHYterminal are executable catalog witnesses. Modelreader sends cachedpointer todescriptorB8kind4 and physicallynamedNxTriangleMeshShapeDesc6C; separate region/part cachekind5 converges ontriangle-shapepreparation. Writerboolword isnotbatchID, FULL32batchkeys differfromsignedLOW16readerkeys, cursorlengthnotstreamlimit, stagezeroSETZnotSDKsuccess, pre-callflag/separatecurrentSDKloads notcompletedvalidmesh. Unsignedguard/physicalcasearms doNOTrepairbaselineCFG.13SIpairedmechanisms doNOTextendGOGshape/corpus/runtime/lifetime ownership.

Evidence/qualification: `findings/subsystems/final_static_xpm_model_physics_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint256

**VERIFIED selected mechanics / STRONG_INFERENCE environment/thunder association:** CMapcloudcallbacks/time-compatibleweights/pulse connectsharedbanks toconcrete84-byte zero/seed packetproducer/nativeevent0E; nativeO44 andreceiver66FCcallbackchannels separate. OptionalchannelmathincludesHALF-luma0.5 andindependentpulse/fadeslotreloads; packet-tagcopy andflagmutationdomains differ. One66FCnonnulltest doesnotprotectlaterreload, noalias/weight/index/currentepoch proof. N_THUNDER1..9 andnumericCSound requestchain strengthenpulseassociationwithoutactual/synchronizedlightning/audio orsuccessfulrendering. Cleanup/null-basepathsconditionalstatic,notobservedfault/safe retirement. NoGOG/completepacket/schema/ownertheorem.

Evidence/qualification: `findings/subsystems/final_static_environment_packet_thunder_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint257

**VERIFIED mechanics / STRONG_INFERENCE family:** Numericresource/latch/variant/Gameflag producers feedavailabletypedCShotpayload/callback andscenequery mode0withSEPARATEflags9. TypedCNxRaycast signedcount/28-byte response fields canrefineengineface/triangle geometry thenconditionalnative1Cpacket targetdelivery; D3DX/platformimports aredependenciesnotrender-only/TLSsemantics. Rawactionnonentries/CShotcandidatequalificationpreserved. Upper-only/reloadedcount,truncatedSIGNEDface,independentrecordreads,conditionalpacket44overwrite,unclampedwrappingR90/unsigned20arrayguard,9keyedCAS+sharedfallback,currentcallback/nullablepayload/lifetime limits remainUNKNOWN. Noall-pathnearest/safeperthread/actualhit/FPSfault theorem.

Evidence/qualification: `findings/boundaries/final_static_player_shot_hybrid_query.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint258

**VERIFIED numericalmechanics / STRONG_INFERENCE typedfamily:** ThreeDartsreferences/pool→launchfollowtarget/nativepacket; namedboardevent1C usespacket-minusposition/inverseY/radius20sectorclassification withBOTHbull50×1. Independentcurrentminigamescore addsmultiplier×number thenmultipliesWHOLEaccumulator10×or5×perpositiveprefixmatch. Boardinteger470..478 anddartfloatoffsetsseparate,board470laterfeedbackindex. PackethelpercanmakeNOevent beforelaterindependentscore,currentretainer110canbeold,so nolivehit/classifier/scorecoherence. DrivergetterOUTPARAM andmatchingborrowedslotclear/camera5 areaccessrequests,notowner/free.16SIownpairedroles keep544GOGRAW_ONLY/nooldctors/externalcallee transfers explicit.

Evidence/qualification: `findings/subsystems/final_static_darts_board_driver_score.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint259

**VERIFIED mechanics / STRONG_INFERENCE typedtext/savepresentationconnection:** EightprivateFISHINGXLYchildren coupletoCLayoutMessage32display/64transitionbanks and1000scratchramp/temporarydrawstateoverride. Restore reloadscurrentstate/slots/backing,notpostcallequality/transaction. Shared4Crecord current28/desired48 selectedpresentationselector4359origin iscontext3C slot3 desired168→nine-recordcurrent148; notfilemagic/checksum orsave-success/content proof. Constructorpartiallyinitializes/readsoldcurrentvectorbeforestatecheck; arbitrarycounts/index/scalars/currentepochs unadmitted.9SIownrolepairs include6ADCC0GOGdisplacementwithoutallcallee/privateXLY/saveorigin transfer.

Evidence/qualification: `findings/boundaries/final_static_layout_text_save_presentation.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.


## Final static campaign amendment — checkpoint260

**VERIFIED mechanics / STRONG_INFERENCE controlpresentationfamily:** ActualCGame-relative8DWORDmaskbank mapscapturedInputqueries andcustomESIWORDtoken/EDIcode/EBXflagsmessageformatting. Recognizedmasksdropunrelatedbits, composite/unmatchedresultsleavepartialtokenpolicy, earlyspecial/04000000returnsbypassfinal3C53. Stageddirty/default/copy producersarefunction/mode-scoped,sparse52bytedefaultsnotfullreset/schema,204/208maymaskorbyte. RepeatedGame/capturedInput/opaqueapply/glyphrefsnotcoherentsnapshot/API/persist/drawsuccess.3SIownGOGlocalroles/datamaps doNOTextendproducercallees ormapnumeric55970. FSC002newown23byteOR helper issupport-onlyeffectqualification,notnewfamily/activationproof.

Evidence/qualification: `findings/boundaries/final_static_control_bank_message_tokens.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.

Accounting: original20,252/15,072 UNKNOWN and corrected21,871 entries remain historically intact; new scoped roles/families are companions, not a whole-function semantic fraction.
