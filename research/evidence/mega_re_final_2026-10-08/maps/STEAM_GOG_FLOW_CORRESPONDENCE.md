# Steam / GOG flow correspondence

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint238 and explicitly bounded flow-edge companions.** Addresses are build-specific. A row maps only its stated role/operands/type/fragment; it never grants whole-body behavioral equivalence, identical live values, ownership, epochs, API outcomes or cadence. `V` below means **VERIFIED selected local mechanics**; `SI` means **STRONG_INFERENCE bounded structural/semantic correspondence**; `?` means **UNKNOWN**.

## 1. Build identity and address rules

| Build | Executable SHA256 | Image base / pointer width |
|---|---|---|
| STEAM_PC |`7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029`|00400000 /4bytes|
| GOG_PC |`c954c2e3b205d444b0fc3649adf4bd8462a7dfe73d89599e24a7e17a129415c2`|00400000 /4bytes|

RVA=VA−the build's image base. Original export auto-name, address/RVA, selected synthesis role and homology identity are separate fields; the final address report/derived node inventory preserve them. Equal VA or equal string data is not a correspondence discriminator.

Authority: corrected structural `baseline_v7`, both semantic overlays, identity-matched230–238 companions, canonical HOMOLOGY ledger at its corrected scopes and Phase8 finite primary receipts. H0232's copied old inline formula is overridden by renderer236. GOG missing/orphan/export-attribution channels are not silently promoted to complete Ghidra bodies.

## 2. Application, display and device nodes

| Flow role | Steam | GOG | Correspondence / limit |
|---|---|---|---|
| Application outer loop |00700650|00700670|**V selected idle/message/timing/tick order**, Steam platform/account startup differs |
| Startup / tick |004017C0 /00401A70|004017C0 /00401A70|Independently checked exact paths, not identity from equality |
| Window-message boundary |00700C90|00700BF0|H0234 **V selected routing**; full input/game semantics unknown |
| Seconds clock |00701040|00700FA0|Phase8 **V QPC/frequency arithmetic**; discontinuities/cadence unknown |
| Context/order selector |0041C270|0041C290|**V bit31 operand**; separate reads, no latched mode theorem |
| Repeat getter |00409DC0|00409D90|**V selected byte read**; eventual loop exit unknown |
| Context companion |00701350|007012B0|C0117 selected receiver/order/delta-or0; whole semantics not promoted |
| Active-object dispatcher |006C5FF0|006C5AF0|H0001/qualified corrected CFG and exact phase windows; object/interface/marker mechanics, not full admitted population or live schedule |
| Movie method in post-dispatch phase |00700500|00700520|C0117/A01 selected helper-presence/query/cleanup request protocol; no playback/completion success or stable session |
| Device initializer |006CC290|006CBD30|Phase8 **V selected PP/CreateDevice/global/dimension requests**, success unknown |
| Cooperative/reset gate |006CCEF0|006CC990|H0229/request-level paired paths; recreation success not eligibility prerequisite |
| Loss / post-reset requests |006CC030 /006CBD00|006CBAD0 /006CB7A0|H0230/31 with renderer236 pointed-bank/output-cell qualification |
|2D count/accessor |006CCFC0 /006CCFE0|006CCA60 /006CCA80|**V stride/check/pointed bank**, not owning class or guaranteed rejection |
| COM-value reverse lookup |006CBAC0|006CB560|**V `[[descriptor+10]]` first match**, not descriptor-address match |
| Normal presentation helper |006CC8D0|006CC370|Both selected EndScene/Present request bodies checked; GOG frame callsite00401CB7 independently identifies counterpart; API results/cadence unknown |
| SceneDraw initialize |006D1610|006D11E0|H0173 **SI original startup/interface scope**; Steam target layout deeply checked, not universal paired lifecycle |
| Pre-render / render consumption |006D2B40 /006D32E0|006D2710 /006D2EB0|Selected C0117/packet/context edges; complete render/per-pass parity unknown |

**Independent same-numbered cells:** both device initializers write01480984/88 and storedPP0148BB70; paired renderer primary independently roots literal descriptor-bank receivers. That supports these specific cell roles in each build, not a theorem that every global has common identity.

## 3. Visibility and reflection

| Role | Steam | GOG | Scope |
|---|---|---|---|
| Six-frustum producer |006B62E0|006B6230|**V selected projection/far-bank mechanism**, current rebuild/admission conditional |
| Common objectslot24 predicate |006BD320|006BCE30|**V selected flags/resource/main/rescue structure**, all class populations unknown |
| Frustum-class decoder |006BB4F0|006BB440|Steam selected call; GOG priority body checked; **SI bounded corresponding role** |
| Three-frustum rescue |006C4630|006C4140|Steam selected target/GOG checked body; no blanket all-input or live equivalence |
| Directional extrusion |006C4390|006C3EA0|Selected target/checked opposite mechanism; units/populations unknown |
| Packet LOD metric |Qualified render-packet path; exact counterpart not newly asserted|006DCF90|GOG exact arithmetic/consumer scope; no guessed uniform delta |
| LOD/pass classifier |Steam renderer-side selected consumer paths|006D5380|GOG flag/metric/list mechanics checked; complete paired classifier not asserted |
| Secondary-pass orchestrator |006D8B50|006D8720|**SI bounded call/structure correspondence**; Steam deeper plane/target/pass reconstruction |
| Reflected matrix builder |006D9340|006D8F10|Phase8 **V plane/reflection/projection operands** |
| Clip/matrix publisher |006D93D0|006D8FA0|**V selected matrix/plane/clip request mechanics**, shader/API success unknown |
| Adjusted projection |006DBCD0|006DB8A0|**V copied matrix/depth-term/view multiply** |
| Identity texture-bias caller |006E07D0|006E0760|**V twice-read width** in each selected body; bug causality unknown |
| Bias helper |006B7020|006B6F70|**V selected affine/half-texel arithmetic** |
| Width / height getters |00409F60 /00409F70|00409F30 /00409F40|**V zero-extended WORD+C/+E**, not one shared function address |
| Bias matrix interface |006E0830|006E07C0|Selected registerEF request; full shader selection/sampling/result unknown |

The reflection flow's GOG mapping is independently found through its MatrixReflect import/call and helper operands, not inferred from neighboring code relocation. Exact current plane/surface/shader populations and runtime offset symptoms remain unknown.

## 4. Input and camera control

| Role | Steam | GOG | Scope |
|---|---|---|---|
| CInput core ctor |00707D90|00707D40|Historical pair plus selected pipeline structure; Steam holder/type acquisition deeper |
| Commit |00708350|00708300|**V selected previous/pending/live operations**, no cadence proof |
| Producer |00708B00|00708AB0|**V six-helper/action/aggregate/pending structure** |
| Raw acquisition/action output |00709C40|00709BA0|**V own-build raw36/action6C mechanism**; shiftA0, not neighboring50 |
| Stick filter |00709400|007093B0|**V selected numeric fields/filter arithmetic** |
| Poll/handoff wrapper |007099D0|00709980|SelectedBC0 gate/protocol; runtime dormancy only secondary here |
| Public digital / axes |00708910 /00708A30|Seed007088C0 /007089E0|Both selected digital/axis accessor operands checked; opposite caller/provider/type ancestry not assumed |
| Generic camera dispatcher |005354D0|005355A0|Phase7 A06 **V signed range/nonnull/mode9 handler skip** |
| Initial table9 handler |00537F10|00537FE0|**V independent physical table cell initializer**, current cell may differ |
| Non-generic table9 caller |005577D0, site00558275|005578A0, site00558345|**V bypass route** through008A9BEC; caller live type/mode/cadence unknown |

Input record offsets may be mechanically matched in selected bodies while root/provider/holder histories differ. Do not transfer Steam CInput ownership/finalizer/camera-instance conclusions merely because a GOG copy loop agrees.

## 5. Resources and typed families

| Role | Steam | GOG | Scope |
|---|---|---|---|
| Current CRdData acquisition |004051F0|004051C0|Phase7 independently named/used roots; old same-address prose not adopted |
| Installed extension callback |00408310|004082D0|Selected extension/type/request/commit seam |
| CLoadThread ctor / worker |006B4D70 /006B4DE0|006B4C50 /006B4CC0|Phase7 P7X05 independent RTTI/root/slot checks |
| Worker dispatch |006B4F20|006B4E00|**V local transport**; GOG006B4F20 is erase code, not this dispatcher |
| Queue producer wrapper |0040B4C0|0040B490|**V literal mode1 route**, actual ID domain/epoch/admission unknown |
| CRdMesh ctor / normalizer |004087E0 /0070BD20|004087A0 /0070BCC0|Selected typed candidate/descriptor18/1C stores; full XMD grammar unknown |
| Picture parser |006B5190|006B50E0|Phase8 current-child/source/metadata seam checked |
| Child texture handoff |006B53EA→006B58D0|006B533A→006B5820|**V currentP8+index20/stack operands**; allocation/API/capture unknown |
| Child row/column accessor |006B5660|006B56A0|**V explicit address/bounds**, not same-numbered identity |
| Metadata offset helper |006B5710|006B5660|Role split independently checked |
| Picture deleting interface |00408900|004088C0|Named available/callback interface, not completed destruction |
| Model resource binding |006BE6E0|006BE1F0|Selected stores/preparation request; early publication/packet freshness qualified |
| XAM setter |006B9020|006B8F70|A11 **V pointer-address/retained-block protocol**, content/owner unknown |
| XCA1 setup |006B9E70|006B9DC0|A10 **V magic/count/cleanup/conditional arrays**, no success/readiness guarantee |
| XCA scene handoff |004275E0|00427600|Selected CRdInterp-compatible field2E8 request; full scene/load chronology unknown |
| DSB wrapper |0070DD40|0070DCE0|Selected signed-slot/state dispatch; complete bytecode/execution unknown |
| Core initializer source |00408960→0070D150|00408920→0070D0F0|Checkpoint234 transport/type/interior scopes; not full coherent cohort |
| XWP name loader |00677CF0|00677C40|Selected package/dependency/CEffect request; grammar/ready/lifetime unknown |
| CEffect ctor / update |006750C0 /0071C190|00675010 /0071BEA0|Named candidate; selected scalar/fixed-bit mechanism independently checked |
| EffectAdmin getter / request |004052E0 /00677960|004052B0 /006778B0|A16 **V typed root/request mechanics**, pointedroot4 installation remainsunknown |
| Item selector wrapper/helper |00456FB0 /00456C70|00456FE0 /00456CA0|A17 **V EAX selector/reacquired root/table tuple**; readiness/success unknown |

Shared archive-helper addresses are accepted only under their existing explicit paired-primary scopes, not a blanket equality rule. Direct-file texture and dedicated audio resource paths are not automatically generic callback families in either build.

## 6. Physics, audio, UI and event state

| Role | Steam | GOG | Scope |
|---|---|---|---|
| CPhysicsCore getter |0040E470 (thunk0040E4F0)|0040E440|Independently named selected root; note address collision with Steam deleting interface |
| Ordinary submit / producer |006EB3C0 /006EAB60|006EB3D0 /006EAB70|Selected transport/policy correspondence; actual scene/schedule success unknown |
| Capacity / timing config |006EACE0 /006EAE20|006EACF0 /006EAE30|Phase8 selected request operands checked, ABI namingSI |
| Synchronous first/second sweeps |0040B750 /0040B780|0040B720 /0040B750|A07 exact request order windows, not every scene/generation |
| Per-context halves |0040B850 /0040B940|0040B820 /0040B910|A07 untested result/current array qualifications |
| Worker packet copy / enqueue |0040BCE0 /0040BC50|Seed0040BCB0 /0040BC20|Steam concrete same-vector join; full GOG activation/lifetime not transferred |
| CSdCore getter / ctor |0046F9D0 /0072BA00|0046FAD0 /0072B710|Independent type/selected construction/static-root scope; deepest audioAPI flowSteam-only |
| CSdMain ctor / token generator |0071FDD0 /00720890|0071FAE0 /007205A0|A05 local named row/sequence protocol; no eternal epoch identity |
| Main free-selector / resolver |007208E0 /00720830|007205F0 /00720540|V128-row producer/full-ID/unchecked resolver index; all callers unknown |
| Gameplay audio callback |00454D50→0046F360|UNKNOWN at this validated use scope|Do not port Steam task/world/Main chronology by Core resemblance |
| Speaker/DSP/completion callback flow |0072DA80/DB80/DCB0;0071FED0/FD50|UNKNOWN whole-flow counterpart|Selected Steam nodes mapped; no invented GOG provider/cue/loop mapping |
| NativeUI category / flag leaf |0045A590 /0045A610|0045A5C0 /0045A640|A08 directory cell versus body, unchecked readiness/domain |
| Named message ctor |0040A270|0040A240|Available named construction, not loaded table44 producer |
| CMessage numeric dispatcher |0045C9F0|0045CA20|H0204 selected routing; category result≠command/render completion |
| CEvent organizer |00405360→0042D5B0;00446AF0 selecteduse|UNKNOWN whole organizer counterpart|Checkpoint233 available type/transport/localmask scope, not all-current controller |
| CGame restore / CPlayer reconstruction |0061A830 /00506F70/00509080|UNKNOWN selected genealogy|Same-numbered exports/body proximity not homology; C0007 broad schema not promoted |
| Save physical writer |00408BD0|00408BA0|Phase7 P7X04 paired file/count/diagnostic requests; protocol0 not full/durable transfer |
| Save request / coordinator / callback |004094C0 /00409790 /00409980|00409490 /00409760 /00409950|Paired selected admission/result publication; Steam Sysutil activation/poll scope not wholesale transferred |
| Save operation6 gate |006AC720|006AC650|Selected request and context-step publication, no admitted-request certificate |
| Save staging-builder prefix |006ACAB0|006AC9F0|Selected staged copies/state geometry; coherent whole-image/schema unknown |

## 7. Checkpoint238 named construction/configuration candidates

| Node / P0 companion | Steam | GOG | Qualification |
|---|---|---|---|
| CBossBase P0-H01 |0067A250|0067A1A0|V selected named construction; SI boundedhomology; lowerbound694, notsizeof698 |
| CMotionDriver P0-H02 |0069ED40|0069EC90|V named/offered11C candidate; backpointer4 is not ownership |
| George2 P0-H03 |00681C00|00681B50|Separate named derived construction, offered818 |
| George3 P0-H04 |00687A30|00687980|Separate sibling/profile, not identical toGeorge2 |
| Table88 numeric admission P0-H05 |0067A620|0067A570|driver8+selector44→17DWORD copy2C; selector28store, currentdomain/epochunknown |
| C0314 ingress P0-H06/H07 |00685180 /00683360|006850D0 /006832B0|Prefix/normal-return savedESI evidence only, not wholebody/liveRjoin |
| C0318 ingress P0-H08/H09 |0068B9C0 /0068A0B0|0068B910 /0068A000|Independent selected profile, not commonclass/schema |
| CNpcEnemy method P0-H10 |00497560|00497640|Static named table170 relation; separate fromboss/driver lineage |

The **actual-current receiver/driver/admitted-row/provider/ownership join remains UNKNOWN**. No ANIMATION/resource-owned classification is inferred from names, ranks or common offset620.

## 8. Semantic differences, structural asymmetry and uncertain joins

- **VERIFIED build-specific application difference:** Steam platform initialization/account/shutdown work surrounds the common Windows-loop pattern; GOG is non-Steam.
- **VERIFIED role/address differences:** texture accessor versus metadata helper, worker dispatcher versus erase, input acquisition shiftA0 versus nearby50, physics getter versus other-build deleting interface, SteamHookChain cleanup versus GOGsame-address constructor. These are why equal addresses/deltas are unsafe.
- **VERIFIED export asymmetry:** GOG dispatcher continuation and selected camera/type paths have raw/PE-only/orphan channels. Corrected CFG/static fragments must be used without pretending exported caller/body metrics are complete.
- **UNKNOWN behavior differences:** deep audio/provider/loop paths, full save/load reconstruction, complete renderer pass selection/materials, current input-holder ancestry, actor handler populations and timing/cadence. Selected operand agreements do not prove no semantic difference.
- **UNKNOWN complete actor homology:** a137-cell physical table with119nonzero/18holes/82unique targets is not82 behavioral homologues or all admitted states (A12).
- **UNKNOWN Xbox mapping:** this deliverable isSteam/GOG. Supplied Xbox sources/XEX can corroborate explicitly matched timing/effect context, but no full third-build address map is fabricated.

## Evidence and navigation

Detailed source/field/function tables: the ten `FLOW_*.md` subsystem maps. OriginalHOMOLOGY ledger and corrected reuse projection retain identity history; Phase8 finite primary files supply only the newly necessary edges. Current scope controls: `maps/PHASE7_CLOSEOUT_RELIANCE.md`, renderer236 contract, A01–A17 companions, P0 `HOMOLOGY_COMPANION.csv`/`CLASS_COMPANION.csv`, both semantic overlays. Every uncertain pair above remains uncertain rather than being filled with a common relocation assumption.

Short getter/Present paired mechanical checks: `audit/phase8_synthesis_2026-10-07/GETTER_PRESENT_EDGE_PRIMARY.json`; no whole provider/cadence/GPU success promotion.

## Post-synthesis finite correspondence additions

These are selected local matches, not general relocation or whole-build parity. Historical232-node inventory and original homology ledger remain sealed; campaign companions are separate.

| Selected role | Steam | GOG | Qualified scope |
|---|---|---|---|
| CPut resource caller / binder |0041C4C0 /005E4A20|0041C4E0 /005E4AF0|**VERIFIED** operands/state; prior typed cohort reused; current row/epoch/ownership UNKNOWN |
| CPut row discriminator / source accessor |005E2150 /006B2BE0|005E2220 /006B2BE0|**VERIFIED** independent own-build bytes; signed/onepast/tag bounds qualified |
| Effect producer / PRM view |00675390 /0044C060|006752E0 /0044C090|**VERIFIED** borrowed view/195×20 static geometry, not current success |
| Item installed base / named SRL adapter |00456D30 /00408250|00456D60 /00408210|**VERIFIED** historical installer reuse +new791×12 normalization;774wrapper domain separate |
| SRL normalizer / DWORD reversal |0070BBE0 /0070BAF0|0070BB80 /0070BA90|**VERIFIED** selected named admission/format/cursor mechanics; no arbitrary schema |
| CMessage binder / selected category caller |0045F610 /00431BA0 selectedwindow|0045F640 /00431C20 selectedwindow|**VERIFIED** typed operand/source lineage; raw setup windows UNASSIGNED |
| Actuator initialization / first and second read |00735AB0 /00735BE0 /00735C20|007357C0 /007358F0 /00735930|**VERIFIED available** seven-pair constructor/read ABI; no invoked backend |
| Actuator pair address / lock / unlock |00735C60 /00735C80 /00735CA0|00735970 /00735990 /007359B0|**VERIFIED available** interfaces, no admitted index/current instance guarantee |
| Actuator opposite-build ancestry |Prior qualified Steam holder0070B990/lazy0070B780|Own GOG0070B930/lazy0070B720/wrapper0070A2A0|**VERIFIED selected** type/call/root operands; not whole provider population |
| Selected reflection bank/token consumer |VS32 00A2EFC0 /PS8 00ADE838|UNKNOWN, not transferred|**VERIFIED conditional Steam** constant239/TEXCOORD5/projecteds5/s6; runtime/bugcause UNKNOWN |

Own-PE/original-ASM receipts and catalog/type/token checks: `audit/post_phase8_closure_2026-10-07/`. Same006B2BE0 and same data/root addresses are independently checked local facts, not permission to reuse equal addresses elsewhere.

Audio PGC007 FILEITEM producer/type/selectedSE ingress is **Steam-only** (00701CB0;00701730/00706AF0;00BD9E18). No GOG table/address/type/currentbank transfer. Opposite-build coverage is unexamined, not disproven or claimed impossible.


## Targeted selected correspondences — TBC001/TBC002

**VERIFIED independent own-PE/ASM/type/data matches:** TBC001 bracket005DA8E0/005DA9B0; setters00409D20/00409CF0; clears00409D50/00409D20; getters00409DC0/00409D90; initial member00886FE0 -> CDrawLoadingThread tables0076F65C/0076F64C. TBC002 caller0061F660/0061F5E0, ECX-preserving field-state helper0061F170/0061F0F0 and selected producer006227A0/00622720 agree in argument transport, local-table branches, stores and gate values. Shared global addresses do not alone establish homology. These are bounded mechanism correspondences, not equal live worker/request/receiver/device epochs, allpaths, cadence or successes. Exact receipts: `audit/targeted_bridges_2026-10-07/{REPEAT,PRESENT,SCHEDULER_QUALIFIERS}_PRIMARY.json`.


## Targeted bridge connections — TBC003/TBC004

**VERIFIED selected static scope** (semantic/epoch ceilings retained): Independent paired selected TBC003 CHelp factory/callback/type/deleting-interface transport: ctor0062E250/0062E1A0,table0078100C/00780FFC,adapter006347D0/00634720 ->00633B80/00633AD0. Only Steam nativeimport body is reused; no fullGOGimport parity. TBC004 same-modelE=M1B4 caller006BF8F9/006BF409 ->update006BA150/006BA0A0,evaluator006BA330/006BA280,blend006BA5C0/006BA510. Arrays/flags/cursor/output arithmetic matched in each ownPE. Currentresource/admission/runtime/fullgrammar/ownership equivalence remains UNKNOWN. TBC005/TBC006 areSteamqualified only; no address-delta extension.


## Targeted bridge connections — TBC010/TBC011

**VERIFIED selected static scope:** TBC010 pairedconstructorreceiver/directtargets0070802B->00709BB0 and00707FDB->00709B20 independentlymatch7stride36rows/twoflagbytes/initialrow0selectedflag. Not allselection/device/acquisition/workerpolicy. TBC011 is anewownGOGconditionalregistration/event1/type/use path, notgeneralSteam/GOGaudioequivalence: raw00454D80,table/remap,event1selectedout-of-range arm,typedgetterCSound004183F0,selectedCore0072C100beforeMain00720690continuation. In-rangearms/currentroot/backend/runtime/wholeaudio equality UNKNOWN. TBC007/TBC009 remainSteamqualified only; no fabricatedGOGallocation/qproduction homology.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** New independentselectedcallbackpair TBC012:0076F65C+4=0040A860 and0076F64C+4=0040A830; each230bytebodycapturesW/testsW18/repeats,offersownstage2/clear2/presentation0 anddeclaredSleep16 requests. Ownconstants1/30branchreconvergesbeforethese requests. Steamexactgenericdispatcherprefixonly; no newGOGgenericdispatcher orfullthread/livetype/cadence/API/device/equivalence proof. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.


## Final static campaign amendment — FSC001 / checkpoint252

**VERIFIED selected mechanics; STRONG_INFERENCE conditional composition.** New current companions connect busy-gated `00401050` key/payload insertion and comparator `00401020` to the accepted `00401080` drain. Selected Steam event0x12 paths (including only the established CHelp-installed path into00633B80) enqueue key0 then exit on admission; available independently named CObjectTarget slot98 methods005B19A0/GOG005B1A70 offer a wrapping460+0xA key then exit or request localfallback. Own-build producer/comparator/drain/type-method correspondence is qualified, not numeric-address transfer. Comparator equalkeys return-1/nozero: no stable/total/FIFO order. Hook mutation/current callback generations, arbitrary payload type/key domain/capacity, actualdraw/cadence/globalserialization/ownership/lastuse remainUNKNOWN. Details/current companion identity: `findings/boundaries/final_static_optional_record_admission.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical Phase8/through250 provenance remains intact.


## Final static campaign amendment — checkpoint254

**VERIFIED selected mechanics / STRONG_INFERENCE typedfamily:** CPasswordselectedthreebytes641 andCChesssixsymbolorder124035 haveclass-provenreset/callbackpaths, lowbyte0setup/1update/2cleanuprequests/0x12drawrequests andselectedphase/data transitions. Steamidentifierlookup0070E9A0 scans32currentCEvCore-relatedslots butre-fetchesup to3times; finalpointer+38 isconditional, notstablematchedbank/null-safe/observedgameoutcome. Passwordcancel2phase1 canoverridesubmit;1/0phase2/3,Chesscancelcount<=0/deletelastpositive andfeedback-gated1/0. Phase4predicate-gatedvirtual30TAIL isnotcompletedretirement.16own-buildrolepairsSI doNOT transferexternalcallees/globals orGOGlookupalgorithm. Fourlarge methods/buildselectedEXPORT_ATTRIBUTION/PEtablepaths,notreconstructedwholebodies.

Evidence/qualification: `findings/subsystems/final_static_password_chess_puzzle_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint255

**VERIFIED selected mechanics / STRONG_INFERENCE family:** World progress/prefetch feeds multiple XPM forms and model-key NxStream imports;97initialSteamXPM/XMD ID pairs plusTREEPHYterminal are executable catalog witnesses. Modelreader sends cachedpointer todescriptorB8kind4 and physicallynamedNxTriangleMeshShapeDesc6C; separate region/part cachekind5 converges ontriangle-shapepreparation. Writerboolword isnotbatchID, FULL32batchkeys differfromsignedLOW16readerkeys, cursorlengthnotstreamlimit, stagezeroSETZnotSDKsuccess, pre-callflag/separatecurrentSDKloads notcompletedvalidmesh. Unsignedguard/physicalcasearms doNOTrepairbaselineCFG.13SIpairedmechanisms doNOTextendGOGshape/corpus/runtime/lifetime ownership.

Evidence/qualification: `findings/subsystems/final_static_xpm_model_physics_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint258

**VERIFIED numericalmechanics / STRONG_INFERENCE typedfamily:** ThreeDartsreferences/pool→launchfollowtarget/nativepacket; namedboardevent1C usespacket-minusposition/inverseY/radius20sectorclassification withBOTHbull50×1. Independentcurrentminigamescore addsmultiplier×number thenmultipliesWHOLEaccumulator10×or5×perpositiveprefixmatch. Boardinteger470..478 anddartfloatoffsetsseparate,board470laterfeedbackindex. PackethelpercanmakeNOevent beforelaterindependentscore,currentretainer110canbeold,so nolivehit/classifier/scorecoherence. DrivergetterOUTPARAM andmatchingborrowedslotclear/camera5 areaccessrequests,notowner/free.16SIownpairedroles keep544GOGRAW_ONLY/nooldctors/externalcallee transfers explicit.

Evidence/qualification: `findings/subsystems/final_static_darts_board_driver_score.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint259

**VERIFIED mechanics / STRONG_INFERENCE typedtext/savepresentationconnection:** EightprivateFISHINGXLYchildren coupletoCLayoutMessage32display/64transitionbanks and1000scratchramp/temporarydrawstateoverride. Restore reloadscurrentstate/slots/backing,notpostcallequality/transaction. Shared4Crecord current28/desired48 selectedpresentationselector4359origin iscontext3C slot3 desired168→nine-recordcurrent148; notfilemagic/checksum orsave-success/content proof. Constructorpartiallyinitializes/readsoldcurrentvectorbeforestatecheck; arbitrarycounts/index/scalars/currentepochs unadmitted.9SIownrolepairs include6ADCC0GOGdisplacementwithoutallcallee/privateXLY/saveorigin transfer.

Evidence/qualification: `findings/boundaries/final_static_layout_text_save_presentation.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint260

**VERIFIED mechanics / STRONG_INFERENCE controlpresentationfamily:** ActualCGame-relative8DWORDmaskbank mapscapturedInputqueries andcustomESIWORDtoken/EDIcode/EBXflagsmessageformatting. Recognizedmasksdropunrelatedbits, composite/unmatchedresultsleavepartialtokenpolicy, earlyspecial/04000000returnsbypassfinal3C53. Stageddirty/default/copy producersarefunction/mode-scoped,sparse52bytedefaultsnotfullreset/schema,204/208maymaskorbyte. RepeatedGame/capturedInput/opaqueapply/glyphrefsnotcoherentsnapshot/API/persist/drawsuccess.3SIownGOGlocalroles/datamaps doNOTextendproducercallees ormapnumeric55970. FSC002newown23byteOR helper issupport-onlyeffectqualification,notnewfamily/activationproof.

Evidence/qualification: `findings/boundaries/final_static_control_bank_message_tokens.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
