# Final object model — construction, fields, retainers and lifetime limits

**Phase8 evidence-qualified view.** Tables record selected named construction/interface/field facts, not complete C++ layouts or a universal ownership tree. Hex sizes are explicit; observed allocation requests, access/write extents and `sizeof` are different. The sealed CLASS/VTABLE/GLOBAL ledgers remain historical identities; current matched companions govern stronger reliance.

## 1. Application, renderer and input objects

| Object / root | Size and construction scope | Important fields/interfaces | Consumers / lifetime limit |
|---|---|---|---|
| CSingleton<CRdSceneDraw>,cache00BD9E68/application00BD7670 |Historical selected acquisition0x6778;Steamgetter00406F70/GOG00406F30;named wrapper construction |18E0passmarker;1CA4querymember;1CA8/count/1CAC selectedobject pointers;63A8submission collection;targetholders/shadow/frustum/control tail |Selected dispatcher/query/render/shutdown interfaces;cache/current generations/all ownership/free unknown |
| CRdModel cache00BD9E60/startup00BD7794 |Fixed aggregate initializer00405690Steam/00405650GOG;0x31C observedwriteextent,notexactsize |14shader regions40..214 stride24;pairedshader-array interfaces;tailstate280/light-array2EC/texture2FC;selectedconfiguration fields |Renderer-associated available service,notwhole active renderer authority orGPUresource owner |
| COctTree pointed member |Separate0x70 offered root/child allocations;ctor006BCA40Steam |Node58 pointercollection;selected4child construction;configuration40000/2000/40000/depth5 |Originalobject membership/query;non-owningroleSI;duplicates/coherentcurrent tree/safe deletion unknown |
| Model/actor packet carrier M |CRdObjectModel selected0x3B0 creation;typedPlayer separate0xD50 offered creator |Resources160/164;state1E4;matrices1E8;packet148/capacity144;node14C;flags138/13C |Conditional evaluation/population/cache/query use;resource/object/packet identities separate |
| Packet P |Capacity derived byproducer;P80 alignedinterior,notmatrix-pointer alias |Tag2C;object34;resources84/88;LOD20;selectedflags30/materialmetadata1AC |BorrowedpointervectorSI;freshness/outputvalidity/finalCPU/GPUborrower unknown |
| CRdTexture holder H |Named0x20 recipe00402220Steam;selected embedded/child/static candidates |Role8;WORDdimensionsC/E;surface10;2D14/cube18/volume1C;registrymarker4 |Create/import/bind/cleanup requests;typed constructor≠usableCOMobject/exclusive owner |
| Gameplay CCamera root00BE1EA4 |Steamoffered0x1AC;namedzero-offsetsingleton/base |Mode154/previous158;flags11C;selectedinputfed6C/70 |Generic andalternate handlers;fullinitializer/cadence/currentgeneration/free unknown |
| Render-camera context |Separate in-place/argument state,not0x1AC gameplay singleton |Eye4,target14,up24,FOV54,aspect58,size5C/60,near64,matrices6C/AC/EC/inverses;frusta1FC,secondarypointer67C |Selected matrix/frustum producer andrender staging;namedC++type/completeownership unknown |
| TSiHolder<CInput>,cache00BD9E10 |Observed0xBD0 holderrequest;CInputzero-offsetbase;corefieldBC0 gives0xBC4 access/write lowerbound,notstandalone size |Callbackhelper8,callback94,lockA4;liveC8/strideCC;repeatanchor660/664;raw668/stride36;pending7E4;aggregate9AC;index9A4/count9A8;BC0protocol;holderBC8guard |Selectedmainpoll/commit/accessors;registeredcleanup notproducerquiescence oractualfinalizer/free |
| Named CInput_Actuator available state |PGC005 independently pairedconstruction andtwo independently sampled formalsetter routes |Sevenstride8 pairs28..60;availablepairedread/address/lock ABI;holderaccounting60 separate |Invokedbackend/physicaldevice/units/paircoherence/currentprovider/lifetime unknown |

## 2. Resource, world and audio objects

| Object / storage | Construction / state | Consumer / lifetime boundary |
|---|---|---|
| CRdData cachedroot00BD9E3C/application00BD768C |Selected0x38C acquisition;count4/tablepointerC/callback10/pending14;support/platform384/mutex388 |Typed descriptor/reference/request/lookup/sweep role;noexclusiveall-family owner orsafe retirement |
| CRdData descriptor |0x30 table element:state18/wrapper1C/aux20;WORDcount2C;control2E/status2F |Callback-produced values;type/count/status/admitted population qualified |
| CLoadThread static01481130 |NamedCThfuncbase0;constructor/worker slots;vectorbegin30/end34/capacity38;count20;directpending3C/ID3E/control40 |InvokedCreateThread route;OSsuccess/globaladmission/serializedproducer/stopjoin unknown |
| CRdMesh |Callbackoffered0x54;Steam004087E0/GOG004087A0;normalizedstate4 |Recordretention→model/animation consumers;availablecleanup,notexclusive actor/resource ownership |
| CRdPicture |Callbackoffered0x10;namedwrappervptr;normalizedmetadata4/childarray8/identifierC |Currentindexed0x20 child holders;availabledeleting interface/payloadcleanup separate |
| CMap static013936F0 |Selected0xA40C0 observedextent/declaration;Steam005D0680/GOG005D0750 construction scopes |State10;Rain/HazeretainersA3D24/28;64levelretainersA3F98 (absolute01437688);resourcetags/state/reset. Fullmode/retirement/owner topology unknown |
| CLevel |Selectedfactory28 typedclass;resource160/164;packet/node interfaces |Retains returned Mesh/Picture values;managerunload coordination/finalborrower notproved |
| CSound cache0138A6E0 |Offered0x68A0;256rows atC stride54;descriptor/name/controlarrays |Game sound requests/retained clients;notMain/Core/backendalias |
| CSdMain cache00BDBCC0 |Offered0x2838;128rows30/stride50;cursor28/sequence2C;scalars2830/2834 |TokenT/control/rowreuse;fullID≠generation,token≠resourceowner |
| CSdCore cache0138A6E4 andstatic014B0400 |Offered0x50C;32rows4/stride24;cursor484/callback488/countbank48C |Separate instances shareabsolute backend/storage;actualstatic/lazycoordination unknown |
| Audioforeignbank/cue interfaces |Sharedbackend014B01E4;bankcells014B01FC...;coreE14 cueoutput |APIsSI;currenttype/cue/loop/success/references/cleanup completion unknown |
| X3Daudio/spatial state |Handle014B01E8;listener0320/emitter0354/DSP02F0;matrix03B8 (all014Bprefix) |Speaker mask comesfromlocal backendmixformat,nothandlecell;sharedscratch/output lifetime separate |

## 3. Game, event, save and service objects

| Object / candidate | Selected fields / construction | Scope |
|---|---|---|
| CGame cache00BDA004 |Offered0x838E30;getter0040A320/ctor00451D90;live8C568/backupBE8 size45CC0;flags8C5EC;vectors99048/99058 |Sharedapplicationstate,notCMap orstandaloneSaveManager;selectedcopy/reconstruction proven,fullschema/lifetime/coherentepisode unknown |
| CPlayer |Generalgamefactoryselector1;offered0xD50;namedtable007767FCSteam;state654/658 andselectedresource/model interfaces |Typedcreation→callback/reconstruction→state40 genealogy;allstates/currenthandler populations notderived |
| CCar vsCObjectCar |StaticCCar008C29F0 withtwoembeddedCLayout members;separatetypedactorCObjectCaroffered0x2008 |CCar-to-actorownership unknown;retainedGameinteriorcelladdress73C isnotowning actor/resource pointer |
| CEvent availableorganizer root00BD9E58 |Offered0x3B9C;constructor0042D5B0;wrapperdeclaresCEventoffset0 |Selectedunadjustedreceiver/resetmask use;noactivecontroller/allcaller/currentcohort/schedule theorem |
| CEvCore compatibleinitializer |Ownwrittenendpoint12BC,notfullsizeof;physicalbase+4;13interiorrequeststarts;staticcohort geometry |Availableconstruction/configuration/request seams;not13ownedmembers/onecurrentVM orcoherent496-resultcohort |
| NamedCMessage cache00BDA000 |Selectedconstructor0040A270/0040A240;observeddeclared0x124D0;constructorclears44 |PGC001 selectedtyped nonconstructorresource2400→R44 installation/category use;R48directorycell/currentgeneration/count/lifetime remainqualified;constructor notloaded-table proof |
| CMenu static01476978 |Native/modeltask installation andselectedinput/numericstate interfaces |Taskreceiver switches tostaticmenu;fullmode/UIownership/cadence/free unknown |
| CRdMovie cache00BD9E48 |Selected0x0C root;helper8 andbyte/control state;movie texture candidate separately0x20 |Helperpresence/query/cleanup requests,notsession/backing/callback completeness |
| CPreserve static00BE5970 |Selectedctor00468F70,CAutoSave member28C;observedbyte57C lowerbound0x57D |Read/staging/header/context/commit requests;notcompleteimage validation ortransaction |
| TSiHolder<CSaveData> cache00BD9FF8 |Observed0x74 holder;callback2C retainsmanager/code;operation3C/buffer40/count44/result5C/busy28/flags64/65;holder70guard |Borrowedsuppliedbuffer;admission/result/cleanup epochs distinct |
| TSiHolder<CSysutil> cache00BD9E0C |Observed0x228 holder;callbackconfiguration94/currentC8;dependencyguard220 |Conditionalcallbackexecution;actualthreadactivation/stopjoin/admissionclosure unknown |
| EffectAdmin root00BD9E54 |Observed7D8DWORD endpoint7DC;pointeddescriptor-table4;500zeroedDWORDbank;mutexchild7D8 |PGC002 pairedEFF_LISTview installation/195×20 descriptors connected;currentadmission/override/readiness/outputtype/owner unknown;knowncleanup not500-objectwalk |
| ItemManager root00BDA0F8 |Offered0x590;pointedtupletableC;helperreacquiresroot afterselectorwrapper |IncomingECX overwritten;helperindependentlyreacquiresroot;PGC003 namedSRL791×12 normalization/selectedtuple source connected;774domain/currentreadiness/success qualified |

Offsets/access/declaration facts are **VERIFIED at named report scopes**; compatible roles/composition are **STRONG_INFERENCE** where indicated. None proves a currently selected live class solely from a vtable write before an opaque operation.

## 4. Checkpoint238 independent named candidates

| Candidate | Steam/GOG ctor | Size scope / important fields |
|---|---|---|
| CBossBase |0067A250 /0067A1A0|0x694 selectedwrite lowerbound,not0x698sizeof;Mutexfuncbase618;constructeddriverresult620includingnull |
| CBossGeorge2 |00681C00 /00681B50|Offered0x818;separatebasetype/source-table configuration |
| CBossGeorge3 |00687A30 /00687980|Offered0x818;sibling,notidenticalprofile/schema |
| CMotionDriver |0069ED40 /0069EC90|Offered0x11C;backpointer4/table8/selector28/17DWORDcopy2C;independentlyinstalled1C/20/24 tables |
| CNpcEnemy |0048D5C0 /0048D6A0|Sizeunknownatselectedprefix;independentNPC/Characterancestry/table170method |

**VERIFIED selected static type/construction mechanics; STRONG_INFERENCE bounded correspondence.** Full actual-currentR/driver/admittedrow/schema/provider/ownership join remains **UNKNOWN**. Do not convert these into one resource-owned ANIMATION class or force CNpcEnemy into boss/driver composition.

## 5. Lifetime/ownership model

The strongest general architecture is **separate lifetime domains with explicit local protocols**, not a complete owner hierarchy:

- cachepointer publication,applicationalias andstaticobject identity;
- descriptorbacking/table entry/statepointer/typedwrapper;
- actor/model retainedresource/state/matrix/packet;
- spatialnode membership/objectpointercollection;
- holder/outputcell/COMobject/device/GPUuse;
- audio namednode/token/Main/Core/status/bank/cue;
- save retainedbuffer/staging/Gameinlinecopy/actorreconstruction;
- callback/thread/provider/root/admission epochs.

Available destructor, invoked cleanup request, deallocation boundary, root clear and proven final borrower are different states. Backpointer/access/retention do not establish exclusive ownership. Local locks/counts/normalreturns do not establish global quiescence orcompletedAPIeffects. The original88 lifetime/success/provider/type obligations remain unwaived.

## Sources

Detailedflow maps/qualifiedaddressinventory;canonical CLASS/VTABLE/GLOBAL identity rows;`maps/PHASE7_CLOSEOUT_RELIANCE.md`;P0 CLASS/HOMOLOGY companions;renderer236;Phase7 A02/A03/A04/A05/A06/A09/A10/A11/A14/A15/A16/A17;Event233/Core234;bothsemantic overlays. Specificobjectsource dossiers are linked in the corresponding flow maps. No class/field is asserted understood solely from decompiler structure or plausible naming.

## Post-synthesis current supplement

Phase8 remains COMPLETE;[post-Phase8 white/gray closure](../maps/POST_PHASE8_GAP_CLOSURE_INDEX.md) refines six selectedstaticbridges (conditionalSteamshader/audioinput ceiling) andnarrowsoneActuatorABI gap. Originalcheckpoint243 report/package/validation remains preserved. Newclaim/boundary/function/site/class/global/vtable/homology/resource/subsystem companions live separately under`audit/post_phase8_closure_2026-10-07/`; tenhistoricalsemanticledgers unchanged.62entrylocators/48sitewindows/13declaredtables/17localcorrespondencegroups are scopedindexes, not62newly understoodfunctions orwholeenginecoverage. All88/checkpoint238 guards/renderer236/Phase6 DO_NOT_RENEW retained; currentepisode/success/ownership/runtime/bugcause limits notwaived.


## Targeted bridge campaign — current supplement

**VERIFIED selected static scope; stronger guarantees explicitly retained:** Selected CHelp installed-path typing, initial CDrawLoadingThread type/field/callback and own-GOG CSound constructor/getter extend existing object relations. Current vptr, complete sizes, ownership and invoked retirement remain UNKNOWN; adjacency does not make00886FFC an owned member. See [the targeted bridge index](../maps/POST_PHASE8_TARGETED_BRIDGE_INDEX.md), `audit/targeted_bridges_2026-10-07/BRIDGE_REGISTER.csv`, reconciliation/frontier/handoff and manifest-bound validation. Protected checkpoint245 copies preserve the previous report. Ten canonical semantic ledgers/all88/overlays/renderer236/Phase6 DO_NOT_RENEW unchanged; no runtime or implementation work.


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
