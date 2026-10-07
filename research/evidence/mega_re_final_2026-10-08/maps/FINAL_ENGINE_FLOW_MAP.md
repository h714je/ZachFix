# Final integrated engine-flow map

**Phase 8 synthesis of validated checkpoint238, with only finite missing-edge checks named in the Phase8 audit.** This is the integrated view of the twelve requested maps, not executable exhaustion or a replacement specification. Detailed component evidence/function tables live in the linked flow maps; uncertainty is part of the architecture.

**Notation:** addresses/`+offset`/stride/mask operands are hexadecimal; byte sizes carry `0x`; record counts, distances and screen dimensions are decimal unless prefixed. `V=VERIFIED selected mechanics`, `SI=STRONG_INFERENCE`, `?=UNKNOWN`; a call/request arrow never implies successful work.

## 1. Whole-engine control-flow spine

```text
Build-specific platform startup
 Steam platform/account checks | GOG non-Steam path
                  │
              004017C0
 acquire/configure roots and install selected native callbacks [V]
                  ▼
Win32 message / idle loop00700650 /00700670
 ├─ messages →window procedure/engine routing
 └─ idle →current D3D cooperative gate
             ├─ loss/error/failedReset →wait request, no tick
             └─ permitted →QPC clock / delta publication
                                 ▼
                         tick00401A70
      commit prior input →conditional next poll [V]
      other pre-dispatch services / local accounting
                                 ▼
      selector-sensitive repeat body [V; eventual exit ?]
       ├─ companion(delta) →SceneDraw object dispatcher(delta)
       └─ SceneDraw dispatcher(delta) →companion(0)
                     │
        conditional original-object passes
         task/native event1 →audio/world/menu/effect branches
         model/state/animation →camera →selected object interfaces
         physics producer /first half →phase7 second half
         →phase8 event6 →other physics/object work →spatial membership
                     └──── repeat decision ────┐
                                 │exit         │back
                                 ▼             │
                  camera/context/frustum staging
                  second independent selector read
                  pre-render query/visibility/packet collection
                  BeginScene-shaped request
                                 ▼
                       00401440(context)
                  conditional SceneDraw render consumption
                  lights/shadows/secondary reflection/material passes
                  movie helper request; later UI/presentation/fade seams
                                 ▼
                   manager retirement request /resource pending sweep
                   separate late WinMM probe
                   EndScene /Present requests
                   numeric frame-index updates
                                 ▼
                  posttick optional extra presentation requests
```

**V local partial order; ? actual cadence/global schedule/coherent generation/API results.** One tick can contain repeated object passes. Audio/world/menu tasks are not totally ordered by a diagram branch. Input is committed outside the repeated dispatcher body; no getter consumes/clears an action edge on read. Render-resource sweep occurs before the selected Present request, not after a proved GPU completion fence.

See [FLOW_FRAME_SCHEDULER](FLOW_FRAME_SCHEDULER.md) for exact markers, conditions, receivers and build scope.

## 2. Whole-engine data/state flow

```text
Physical input / keyboard / mouse / WinMM state
 →raw36 →logical action6C →aggregate4C →pending40 →liveCC
 →player /camera /menu /vehicle control state
              │                         │
              ├→actor state654/658       └→camera control /context projection
              ├→CCT displacement requests           │
              ├→persistent vehicle setters          │
              └→game/UI numeric changes              ▼
                                               view/frusta /visibility
Resource names / numeric IDs /state requests            ▲
 →archive identity /queue-or-direct load                │
 →raw/zlib bytes /typed callback                        │
 →resource state18 +wrapper1C /conditional record commit │
 →world/actor resource retainers160/164                  │
 →animation state1E4 /matrix output1E8 /bounds────────────┘
 →conditional packet148 /packet80 payload /resource84/88
 →SceneDraw63A8 pointer vector →pass lists/shaders/textures
 →D3D request state /targets /draws /Present

Application QPC delta014AFFE0
 ├→objects /animation /camera /fade /Timer /audio control
 └→physics timing policy →scene records →simulate/flush/fetch-shaped requests
                                      └→event6 continuation [not result certificate]

Game/event/audio request +parameter/name resource
 →CSound →Main tokenT/rowA →CoreK/rowE +statusQ
 →bank/cue/spatial requests →query/callback/sentinel/cleanup requests

Persistence: disk request →staging00BE5EF0 →selected Game fields/live record
          UNKNOWN joined load-to-creation episode
             →selected CPlayer reconstruction/vector consumer

Native script/UI/event data producer ?
 →available directory/category /Core member /event organizer request seams
 →selected gameplay/world/resource/audio state operations
 Full admitted schema/execution/order/success remains ?
```

The diagram deliberately keeps unjoined producer, epoch and result transitions visible. It does not say every resource is a model/texture, every actor is a Player, every message is a command, or every subsystem shares one clock/owner.

## 3. Origin → state → transform → consumer → lifetime register

| Origin | Important state | Concrete transformation / function | Downstream consumer | Lifetime / confidence |
|---|---|---|---|---|
| QPC/frequency | Cachedfrequency014AFFF0;eligible prior-time;014AFFD4/E0 | Seconds clock00701040/00700FA0;application delta×60/clamp/quantization | Tick/object/physics/audio/Timer/effects | **V arithmetic**, exact all-state wall-time/cadence **?** |
| Display configuration |0148BBC0/C4;local PP;stored0148BB70 | Deviceinit006CC290/006CBD30;Create/Reset inputs | Current device01480984 | **V selected fields**, success/current device **?** |
| Render literals/dimensions |01480974/70→SceneDraw65E4/E8→holderC/E | Target creation /viewport0044ADA0 /projection contexts | GPU targets/screen constants/sampling bias | Separate dimensions; not one global resolution |
| Physical inputs |I668 raw records;temporaryactionL |00709C40/00709BA0 |Aggregatehelpers onoriginalI | **V selected receiver/value flow**, device topology/success **?** |
| Logical actions |Aggregate9AC,pending7E4,liveC8;heldCC/risingD0/repeatD4/previousD8 |Producer00708B00/00708AB0;commit00708350/00708300 |Readonly publicgetters;Player/camera/menu/vehicle | One pending snapshot;history persists;cadence **?** |
| Actor numeric state |SelectedPlayer654/658;camera154/158/11C |00528F40,mode overrides andtypedcamera setter |Camera handlers/render context | Selectedtypedgenealogy **V**, allstates/handlers/currentepoch **?** |
| Resource request |Packedlow16/control8 orworker directpending3C |CLoadThread/006B1310/currentmanager dispatch |Archive extraction /callback | Width/remap/result alternatives;global serialization **?** |
| Extracted asset |Localdescriptor30;callbackstate18/object1C |00408310/004082D0 typedbranch;conditional table copy |Current CRdData accessors | Retention/cleanup responsibility,notexclusiveownership |
| XPC2 payload |Picture4/8;currentchildH;temporaryinflatedsource |Parser006B5190/006B50E0→currentarray+index20→D3DXhelper |Texture/surface cellsH14/H18/H10;world/model/UI | Addressseam **V**,decode/API/currentallocation/lastuse **?** |
| XMD/state resource |Meshwrapper/state;model160/164 |006BE6E0;state1E4,matrix1E8;006C1430→0070C130 |Bounds andconditional packet population | Earlypublication;fullgrammar/ready/latestpose **?** |
| Animation output |M1E8,base98,selectedstate200 |0070CF90 multiply/transpose/indexed12DWORD copy |P80 alignedpacketinterior | **V selected data genealogy**,freshness/GPUlastuse **?** |
| World/model bounds |O+F0/+F4 andresource inputs |Bounds prep006C2020;spatialslot48 /COctTree |QueryoriginalO→visibilityslot24 | Non-owningmembership **SI**,uniqueness/currentness **?** |
| Camera/render context |Eye/target/up,FOV54/aspect58;matrices6C/AC/EC;frusta1FC |006B62E0/006B6230;sixfarvariants |CommonAABB/classdecoder/secondaryrescue | Conditionalrebuild;notallcameraobjectsoneclass |
| Object/resource visibility flags |O138classbits;D8/DC suppress/rescue;P20LOD |006BD320/006BCE30;GOG006DCF90/006D5380 |Packets/submeshes/pass lists | **V localmechanics**,notallclass/modelpopulations |
| Directional-light mode/direction |SceneDraw6708,6418;bank5F88;context67C |Shadowmatrix/frustumproducer;main/secondarytests |Shadow-compatiblelist63C4 /rescue | Retainedbank/freshness/point-lightpolicy **?** |
| Surface/material request |Packet1ACmetadata,normal/point;selectedpacket6440 |Reflection006D9340/006D8F10;clip006D93D0/006D8FA0 |Half/quartertargets;laterstage5/6texturebindings | **V operands**,knownoffsetbugcause/finalsample **?** |
| D3D recreation descriptor |B=[owner+C];D=B+i18;S=D10;COM=[S] |Release/clear/Reset/Createrequests;COMlookupdoubleindirect |Holder/currentdevicebind/draw | Bank/cell/COM/device epochs separate;all-success **DISPROVEN** |
| Soundrequest/PRM/namedkey |InlineP;MainT/A;CoreK/E;statusQ |CSoundadapter→Main/Core→bank/cue;X3D calculation |Playback/spatial/status callbacks | Tokensnotepochs;namedcontents/loop/API/lastuse **?** |
| Audio backend mix format |Localmask/count;handle014B01E8;DSP014B02F0 |X3DAudioInitialize/Calculate;initialDSPapply vs later2×2requests |Currentcuevariables/matrix requests | Speaker-awareinputs **V**,effectiveaudiblelayout **?** |
| Disk/persistence request |SaveData40pointer/44count;result5C/busy28;staging00BE5EF0 |ConditionalSysutil callback/I/O/header/livecommit |CGame live/backup andselectedreconstruction | Read/writeprotocol≠fullvalid/durabletransaction |
| CGame live/backup |G+8C568 /G+BE8,copy45CC0;vectors99048/99058 |0061A830 /00647130;selected00509080 |TypedPlayer reconstruction /state40 | Selectedcopy/consumer **V**,joinedloadepisode **?** |
| NativeUI directory |CallerR44base;D=B+4code+C;cachedR48=D;body=B+[D] |0045A590/0045A5C0 category;flagreader |Selectednumeric UI/control consumers | Loadedproducer/count/bodylifetime/typejoin **?** |
| Available Event/Core setup |NamedCEvent root;Core+4base/interiorcohorts/descriptors |Numericorganizer/reset andmember requests |Selected event/control/configuration seams | NotcompleteactiveVM/scheduler/ownedcohort |
| Boss/entity configuration |Selectedouter620;driver4/8/28/2C |Namedconstruction;17DWORD rowcopyselector44 |Conditionalboss/motion candidate ingresses | Actualcurrenttyped/admittedproviderjoin **?**;CNpcEnemyseparate |

## 4. Important caller → callee → state-change boundaries

| Boundary | Receiver / input | Local state change / output | Downstream meaning ceiling |
|---|---|---|---|
| Eligible application→tick |Current cooperative gate,clockdelta |One tick per eligible idle traversal | Not GPUframe/cadence1:1 |
| Tick→inputcommit/poll |Currentinputroot;pending count/BC0 |Live masks/history thennewpending | Multiple consumers can read sameedge;no consume-on-read |
| SceneDraw marker2→native task |SelectedoriginalO,delta;installedcallback |Event1 callback andprivate state requests | Task order/recurrence/liveness unknown |
| Typed modelslot10→evaluation |SameM pointer;currentresource160/state/buffers |SelectedM1E8 matrixwrites | Notall allocation/success/latest pose |
| Marker14→physics→marker7 |CurrentCore/scenes/contextarrays |Timing/record/firsthalf requests→secondhalfclear | Marker8 ignoresoperation-successreturns |
| Marker8→objectevent6 |EligiblecurrentO,maskgates |Virtual1C(6) | Postrequestcontinuation,not solver-substep delivery |
| Marker13→spatialmembership |O plusquerymember |O14C update andnode58 pointerinsert/erase | Access/membership≠ownership/destruction |
| Pre-render→visibility→packet |QueryselectedO,context;M148cache |Conditionalvirtual24/44 andpointerappend | Coherentcurrentpopulation/freshpacket/GPUlastuseunknown |
| Resourcecallback→record |Currentmanagercallback,descriptor/index |12DWORDcopy/count/control/status | Nonzero callback/localreturnnotfull resource success |
| Reflection→later sampling |Plane/context;selectedtargetholder |Clip/draw/restore;bindsamecolorholders5/6 | Nofreshcontent/finalshaderTEX/resultguarantee |
| Audio completion→callback |CurrentCore488,target/E4 |SentinelstoremaymakeMainrowavailable beforelatercleanup | Reusable rownotcompletedbackenddestruction |
| Save callback→I/O/result |CurrentSaveDatabuffer/count/op |Result5C,busyprotocol,independentlylaterclear | Result0notverifiedreadcompleteness/format/durablewrite |
| Menu→CGame numeric change |SelectedID85/999,guards,currentroot |983C4 signedSUB/clamp selectedpath | Notgeneric “removeallitems” orunboundedmathsaturation |
| Resource pending tail→cleanup |M14,zeroWORDcounter,nonnullpayload |Event1 callback/free/descriptorclear | Afterrenderrequest≠afterfinalCPU/GPUborrower |

These seams are high-information architecture edges because they show exactly which value/receiver changes domains. They do not assert all callers or complete producer populations.

## 5. Resource/state lifetime is a graph, not one owner tree

```text
archive stream /temporary extracted bytes
      →resource descriptor state andtypedwrapper
      →actor/world/model retainedresource values
      →evaluationbuffers /submissionpacket
      →SceneDraw borrowedpacketpointer
      →D3D outputcell /COMobject /GPUcommanduse

separate: audioPRM/nameP →MainT/row →CoreK/statusQ →bank/cue/interface
separate: SaveDataretainedbuffer →staging →Gameinlinecopy →actorreconstruction
separate: node58 objectpointer ↔object14C node membership
```

**SI borrowing/non-owning roles** where pointer-only storage, independent cleanup and distinct producers reinforce them. **? exclusive owners/aliases/currentepochs/final borrowers** elsewhere. A backpointer or stable receiver is not ownership or a stable allocation generation. Named construction does not prove a loaded table or admitted schema.

Established local cleanup orders must remain visible: release callback before payloadfree/recordclear; model ordinary cleanup before optionalouterfree; packet deallocation/clear before currentvirtual48(0); audio callback/sentinel beforelatercuecleanup; worker/localwake/storagefree without demonstratedjoin. None proves a safe global retirement transaction.

## 6. Independent state domains and timing islands

| Domain | Persistence / time input | Why it is not interchangeable |
|---|---|---|
| Gameplay scalar014AFFE0 |QPC-derived×60 withclamp/rounding |NotPhysXseconds oruniversal exactwalltime |
| Inputlive/history |Commit-based masks,per-bit repeats;producerfixedfilterstep |Different sampling/history lifecycle fromrenderframe |
| Physics scene timing |Configured timestep/capacity plusqueuedelapsed |SceneAPI/debt/scheduling notcompletedby markeradvance |
| CCT |Per-call displacement/immediate-move-compatible request |Independentofscene substep cadence |
| Vehicle wheel values |Persistent motor/brake values;selecteddeltamultiply |Notone-shot impulse oruniversallyscaled displacement |
| Fixed-effect subset |1.0 perselectedupdate×localscale |Ordinaryeffects usegameplaydelta;notall effects fixed |
| Animation/packet |Resource/state evaluation andconditionalcache reuse |Drawfrequency≠latestposefrequency |
| Timer/CFunc |Localaccumulator/countdown/gates;alternativewriters |NotQPCprovider oroneglobalpause authority |
| Audio |Request/token/status/spatial anddeltafedparameter recurrence |Backendcompletion androwreuse are separate |
| Save/resource workers |Pending/admission/result/borrowedbuffer episodes |Localclear/wait doesnotjoin everyproducer/borrower |
| Renderer |Requeststate/retained targets/frusta/COMcells |Present request≠GPUcompletion/finaluse |

## 7. Event, UI, world and persistence joins that remain open

- **UNKNOWN script/input producer edge:** loaded/native command data →actualadmitted Core/Event instruction/schema. Available member requests/cohort banks do not prove one current script scheduler orall initialized elements.
- **VERIFIED selected NativeUI static producer:** resource2400 field18→typed binderR44→selected typed category-use, refined inPGC001. **UNKNOWN** actualadmittedbodycount/code-domain/currentstorage/lifetime; cacheR48 remains directorycell,notbody pointer.
- **UNKNOWN world-selector incoming edge:** rawselectedselector15→worldhandler and13→resource/codeconsumer are known local routes; their actual top-level frame occurrence is not supplied by arbitrary callback analogy.
- **UNKNOWN load-to-reconstruction episode:** diskstaging→selectedlivecopy andCPlayerdata consumer are individually concrete, butone successfuldurable coherentload→immediateactorcreation/reconstruction transaction is not established.
- **UNKNOWN live type/schema joins:** actual-currentbossR/driver/admittedrow;EffectAdminroot4/ItemManagerrootC currentpayload/readiness/epoch;Actuatorcurrentexecutor/backend/units. Available Effect/Item producer/admission is now statically connected by the post-synthesis amendments below; it is not current-instance or success proof.
- **UNKNOWN last-use/success joins:** resource/node/packet/cue/COM ownership andeverycurrentgeneration/schedule/API outcome. Staticlocks/clears/normalreturns do not fill them.

## 8. Steam/GOG and completeness boundary

Read [STEAM_GOG_FLOW_CORRESPONDENCE](STEAM_GOG_FLOW_CORRESPONDENCE.md) for build-qualified nodes/pairs/uncertainties. Steam has deeper selected actor/audio/save/render genealogy; GOG has independently checked majorcontrol/type/primitive/flow nodes andexplicitraw-only/export asymmetry. Equaladdresses andone addressdelta are nothomology. Xbox is comparative only whereindividually matched.

The corrected executable population is21,871 structural entries, while20,252 historical function-ledger rows and15,072 historicalUNKNOWN records describe a different accounting universe. Neither names norVERIFIED rowcounts compute whole-function semantic understanding. Phase7 scientificcloseout and this Phase8 requested synthesis are notexecutable exhaustion. Phase6 corrected sufficiency remainsDO_NOT_RENEW; all88 inheritedguards and186 closeoutresidual records remain retained at their exactscopes.

## 9. Reading routes and evidence

1. Control/time: [scheduler](FLOW_FRAME_SCHEDULER.md) →[display/timing](FLOW_DISPLAY_RESOLUTION_TIMING.md) →[physics/FPS dependence](FLOW_PHYSICS_AND_FRAME_DEPENDENCE.md).
2. Visibility/output: [visibility/LOD](FLOW_VISIBILITY_FRUSTUM_LOD.md) →[reflection](FLOW_REFLECTION_RENDERING.md) →[D3D lifecycle](FLOW_D3D9_DEVICE_RESOURCES.md).
3. Input/game state: [controller](FLOW_CONTROLLER_INPUT.md), withselected camera/menu/player boundaries.
4. Resources: [textures](FLOW_RESOURCES_TEXTURES.md) →[whole resource system](FLOW_RESOURCE_SYSTEM.md) →[audio](FLOW_AUDIO.md).
5. Build identity: [correspondence](STEAM_GOG_FLOW_CORRESPONDENCE.md); finaladdress/object/coverage/correction/unknown reports under`reports/`.

Primary/claim authority remains `maps/PHASE7_CLOSEOUT_RELIANCE.md`, both semantic overlays, renderer236 contract, C0117/Core/Event/A01–A17/P0 companions and individually cited original PE/ASM/runtime-qualified sources. Integratedsave/state seams additionally consume `findings/boundaries/save_disk_staging_record_chain.md`, `cgame_record_reconstruction_root.md`, `cmenu_numeric_game_slot_commit.md` under their later matched save/epoch/result companions. This map supplies no implementation advice or runtime-modification authorization.

## 10. Post-synthesis connected/narrowed static bridges

Phase8 remains COMPLETE. These evidence-qualified amendments improve continuity without a numbered phase, broad portfolio renewal, runtime success or ownership tree. Exact original checkpoint243 maps/reports remain in the campaign parent snapshot.

| Gap | New atlas connection | Explicit unjoined edge |
|---|---|---|
| PGC004 **VERIFIED / SI** |Resource payload→typed CPut cohort8/18/4 binding→known world-progress/car-model provider |Admitted rows/keys/current resource and retirement |
| PGC002 **VERIFIED** |EFF_LIST.PRM interior→typed EffectAdmin4/195×20 descriptors→effect request |Current override/readiness/success/ownership |
| PGC003 **VERIFIED** |Named ITM_LIST.SRL791×12 endian normalization→ItemManagerC→selected model/picture selectors |Actual admitted current generation/product;774wrapper domain retained |
| PGC001 **VERIFIED** |Resource2400→typed CMessage binder44→selected typed category/numeric use |Loadedcode/count/body lifetime/universal dispatcher/finaldraw |
| PGC005 **VERIFIED available ABI** |Seven stride8 Actuator pairs with matching read/address/lock interfaces; independent GOG ancestry |Invoked rooted accessor/executor/backend still UNKNOWN; no executed arrow inferred |
| PGC006 **VERIFIED conditional Steam contract** |DiffuseV constant239..242→selected VS32 TEXCOORD5→selected Rain PS8 projected samplers5/6 |Actual live permutation/current targets/device/GPU and reflection-offset cause |

The corresponding detailed resource/controller/reflection maps and focused post-synthesis reports supply exact entry/site/build/confidence scopes. [Campaign register/index](POST_PHASE8_GAP_CLOSURE_INDEX.md) names outcomes and remaining discriminators; no original closeout obligation is waived.

PGC007 **VERIFIED producer / SI request-role link:** selected metadata→typed CAudio_Data FILEITEM population→inline bank/control/name fields→existing sound request. InitialP2C/stored-key correspondence isqualified; duplicate/secondary histories andactualPLSE066 metadata/currentbackend/loopmeaning remain UNKNOWN. See[Audio producer](../findings/boundaries/post_phase8_audio_fileitem_population.md).


## Targeted post-Phase8 bridge additions — TBC001/TBC002

**VERIFIED static flow:** CMap polling bracket -> selected CDrawLoadingThread member start/clear requests -> byte00886FF8 -> known conditional frame repeat. Separately saved numeric S state -> paired selector producer ->00BD77BC -> known post-tick extra-presentation gate/clear. These make loading/repeat and transition/presentation regions contiguous without certifying thread activation, polling progress, all producer policy, class S, actual request success or live cadence. Counter2/3 is a gate, not loopcount; both local loops count3 independently. Details and primary: `findings/boundaries/targeted_bridge_loading_repeat_bracket.md`, `targeted_bridge_extra_presentation_producer.md`; campaign register `audit/targeted_bridges_2026-10-07/BRIDGE_REGISTER.csv`.


## Targeted bridge connections — TBC003/TBC004/TBC005/TBC006

**VERIFIED selected static scope** (semantic/epoch ceilings retained): Four selected static relations now lengthen established regions: typedCHelpregistration -> storedcallbackadapter -> known textureimport receiver; XCAretainedarrays -> same-model scalar evaluation/blend; animationmatrix -> packetplane record -> reflection plane consumer; CEffectnumericdistance flag -> inheritedpartupdate gate. Friendlyasset/actionmeaning, wholeclass/schema/type domains, currentepisodes, runtime/API/cadence/ownership/borrower/bugcause claims are NOT promoted. Exact per-bridge evidence/ceilings live in `audit/targeted_bridges_2026-10-07/BRIDGE_REGISTER.csv` and six focused targeted reports.


## Targeted bridge connections — TBC007/TBC009/TBC010/TBC011

**VERIFIED selected static scope:** Fouradditionalboundedconnections: XAMcompatibleS14 -> allocation/decode-shaped retainedblockproducer -> knownconditionalcleanup; CSoundqcreation -> fullIDrow -> distinctstoredMainoperand; inputconstructor -> initialselectedrow0flag -> rawslotconsumer; GOGcallbackinstallation -> ownnumeric event1selectedarm -> typedCSound -> knownCore/Maincontinuation. These lengthen existing resource/audio/input/task flows withoutpromoting fullformat/schema, permanentidentity, freshMainT, unchangedlocals/Sfields/currentepochs, actualactivation/cadence/API/decode/ownership/lastborrower guarantees. Explicitqualifiedreports/register: `audit/targeted_bridges_2026-10-07/BRIDGE_REGISTER.csv`.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** Freshsinglevslotdiscriminator TBC012 extendsTBC001: knownworld/loadingflag bracket -> initialCDrawLoadingThread virtualtarget -> sameW18-control loop -> knownstage/clear/presentation requests -> Sleep16 request/backedge. Threshold1/30 onlyresetslocalaccumulator,notrendergating. Thisisavailableconditionalworker control,notactualactivation/completealternative scheduler/currentdevice/API/cadence/safeconcurrency/quiescence proof. Priorinitialstopreviewpreservedassupersededpre-callbacksnapshot. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.


## Final static campaign amendment — FSC001 / checkpoint252

**VERIFIED selected mechanics; STRONG_INFERENCE conditional composition.** New current companions connect busy-gated `00401050` key/payload insertion and comparator `00401020` to the accepted `00401080` drain. Selected Steam event0x12 paths (including only the established CHelp-installed path into00633B80) enqueue key0 then exit on admission; available independently named CObjectTarget slot98 methods005B19A0/GOG005B1A70 offer a wrapping460+0xA key then exit or request localfallback. Own-build producer/comparator/drain/type-method correspondence is qualified, not numeric-address transfer. Comparator equalkeys return-1/nozero: no stable/total/FIFO order. Hook mutation/current callback generations, arbitrary payload type/key domain/capacity, actualdraw/cadence/globalserialization/ownership/lastuse remainUNKNOWN. Details/current companion identity: `findings/boundaries/final_static_optional_record_admission.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical Phase8/through250 provenance remains intact.


## Final static campaign amendment — checkpoint253

**VERIFIED selected mechanics / STRONG_INFERENCE family:** shared child attachment setter0071EDE0 retains a parent-related token/index/localvectors;0071EEF0 requests indexedparent matrix006C3B80 and local-left/parent-right multiplication, storeschildposition thenoptionalorientation. Descriptor00461100 and004A1960/006A9F10 supplycreatedchildECX, notoriginalparent. Setup/earlyexits are nontransactional;210mask1 skipsorientation only;0xD48 isofferedlookupdiscriminator and comparedhelperRESULTs; reset uses signedcurrentcount/separatelyreloadedbacking. Dynamicclass/indexadmission/ownership/currentpose/packetfreshness/success/schedule/GOG remainUNKNOWN. Thisconnectsdescriptor/gameplaycreation toanimation/modelmatrix/world-transformuse; it doesnot establish streaming-subsystem ownership.

Evidence/qualification: `findings/boundaries/final_static_parent_matrix_attachment.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint254

**VERIFIED selected mechanics / STRONG_INFERENCE typedfamily:** CPasswordselectedthreebytes641 andCChesssixsymbolorder124035 haveclass-provenreset/callbackpaths, lowbyte0setup/1update/2cleanuprequests/0x12drawrequests andselectedphase/data transitions. Steamidentifierlookup0070E9A0 scans32currentCEvCore-relatedslots butre-fetchesup to3times; finalpointer+38 isconditional, notstablematchedbank/null-safe/observedgameoutcome. Passwordcancel2phase1 canoverridesubmit;1/0phase2/3,Chesscancelcount<=0/deletelastpositive andfeedback-gated1/0. Phase4predicate-gatedvirtual30TAIL isnotcompletedretirement.16own-buildrolepairsSI doNOT transferexternalcallees/globals orGOGlookupalgorithm. Fourlarge methods/buildselectedEXPORT_ATTRIBUTION/PEtablepaths,notreconstructedwholebodies.

Evidence/qualification: `findings/subsystems/final_static_password_chess_puzzle_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint255

**VERIFIED selected mechanics / STRONG_INFERENCE family:** World progress/prefetch feeds multiple XPM forms and model-key NxStream imports;97initialSteamXPM/XMD ID pairs plusTREEPHYterminal are executable catalog witnesses. Modelreader sends cachedpointer todescriptorB8kind4 and physicallynamedNxTriangleMeshShapeDesc6C; separate region/part cachekind5 converges ontriangle-shapepreparation. Writerboolword isnotbatchID, FULL32batchkeys differfromsignedLOW16readerkeys, cursorlengthnotstreamlimit, stagezeroSETZnotSDKsuccess, pre-callflag/separatecurrentSDKloads notcompletedvalidmesh. Unsignedguard/physicalcasearms doNOTrepairbaselineCFG.13SIpairedmechanisms doNOTextendGOGshape/corpus/runtime/lifetime ownership.

Evidence/qualification: `findings/subsystems/final_static_xpm_model_physics_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint256

**VERIFIED selected mechanics / STRONG_INFERENCE environment/thunder association:** CMapcloudcallbacks/time-compatibleweights/pulse connectsharedbanks toconcrete84-byte zero/seed packetproducer/nativeevent0E; nativeO44 andreceiver66FCcallbackchannels separate. OptionalchannelmathincludesHALF-luma0.5 andindependentpulse/fadeslotreloads; packet-tagcopy andflagmutationdomains differ. One66FCnonnulltest doesnotprotectlaterreload, noalias/weight/index/currentepoch proof. N_THUNDER1..9 andnumericCSound requestchain strengthenpulseassociationwithoutactual/synchronizedlightning/audio orsuccessfulrendering. Cleanup/null-basepathsconditionalstatic,notobservedfault/safe retirement. NoGOG/completepacket/schema/ownertheorem.

Evidence/qualification: `findings/subsystems/final_static_environment_packet_thunder_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint257

**VERIFIED mechanics / STRONG_INFERENCE family:** Numericresource/latch/variant/Gameflag producers feedavailabletypedCShotpayload/callback andscenequery mode0withSEPARATEflags9. TypedCNxRaycast signedcount/28-byte response fields canrefineengineface/triangle geometry thenconditionalnative1Cpacket targetdelivery; D3DX/platformimports aredependenciesnotrender-only/TLSsemantics. Rawactionnonentries/CShotcandidatequalificationpreserved. Upper-only/reloadedcount,truncatedSIGNEDface,independentrecordreads,conditionalpacket44overwrite,unclampedwrappingR90/unsigned20arrayguard,9keyedCAS+sharedfallback,currentcallback/nullablepayload/lifetime limits remainUNKNOWN. Noall-pathnearest/safeperthread/actualhit/FPSfault theorem.

Evidence/qualification: `findings/boundaries/final_static_player_shot_hybrid_query.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint258

**VERIFIED numericalmechanics / STRONG_INFERENCE typedfamily:** ThreeDartsreferences/pool→launchfollowtarget/nativepacket; namedboardevent1C usespacket-minusposition/inverseY/radius20sectorclassification withBOTHbull50×1. Independentcurrentminigamescore addsmultiplier×number thenmultipliesWHOLEaccumulator10×or5×perpositiveprefixmatch. Boardinteger470..478 anddartfloatoffsetsseparate,board470laterfeedbackindex. PackethelpercanmakeNOevent beforelaterindependentscore,currentretainer110canbeold,so nolivehit/classifier/scorecoherence. DrivergetterOUTPARAM andmatchingborrowedslotclear/camera5 areaccessrequests,notowner/free.16SIownpairedroles keep544GOGRAW_ONLY/nooldctors/externalcallee transfers explicit.

Evidence/qualification: `findings/subsystems/final_static_darts_board_driver_score.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint259

**VERIFIED mechanics / STRONG_INFERENCE typedtext/savepresentationconnection:** EightprivateFISHINGXLYchildren coupletoCLayoutMessage32display/64transitionbanks and1000scratchramp/temporarydrawstateoverride. Restore reloadscurrentstate/slots/backing,notpostcallequality/transaction. Shared4Crecord current28/desired48 selectedpresentationselector4359origin iscontext3C slot3 desired168→nine-recordcurrent148; notfilemagic/checksum orsave-success/content proof. Constructorpartiallyinitializes/readsoldcurrentvectorbeforestatecheck; arbitrarycounts/index/scalars/currentepochs unadmitted.9SIownrolepairs include6ADCC0GOGdisplacementwithoutallcallee/privateXLY/saveorigin transfer.

Evidence/qualification: `findings/boundaries/final_static_layout_text_save_presentation.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint260

**VERIFIED mechanics / STRONG_INFERENCE controlpresentationfamily:** ActualCGame-relative8DWORDmaskbank mapscapturedInputqueries andcustomESIWORDtoken/EDIcode/EBXflagsmessageformatting. Recognizedmasksdropunrelatedbits, composite/unmatchedresultsleavepartialtokenpolicy, earlyspecial/04000000returnsbypassfinal3C53. Stageddirty/default/copy producersarefunction/mode-scoped,sparse52bytedefaultsnotfullreset/schema,204/208maymaskorbyte. RepeatedGame/capturedInput/opaqueapply/glyphrefsnotcoherentsnapshot/API/persist/drawsuccess.3SIownGOGlocalroles/datamaps doNOTextendproducercallees ormapnumeric55970. FSC002newown23byteOR helper issupport-onlyeffectqualification,notnewfamily/activationproof.

Evidence/qualification: `findings/boundaries/final_static_control_bank_message_tokens.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
