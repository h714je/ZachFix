# Texture and resource flow — identity to engine/D3D consumers

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; checkpoint238 evidence boundary with one bounded necessary child/texture-edge check.** `TEXTURE_EDGE_CONTRACT.json` names the exact missing transition; `TEXTURE_EDGE_PRIMARY.json` matches Steam420/GOG292 original-ASM instructions to their own PE. This is a new finite site-centered check, not continuation of seq62's closed prologue method or a broad XPC/renderer investigation. All success/ownership/generation/borrower qualifications remain.

## 1. Complete supported spine, with the missing guarantees visible

```text
resource name / numeric request
 → archive identity/lookup → request route (queue OR direct slot)
 → selected archive stream/header/raw-or-zlib extraction
 → local30-byte CRdData descriptor, callback event0
 → .XPC branch: named10-byte CRdPicture wrapper offered
 → XPC2 magic/grid metadata / child allocation / inflate requests
 → current child H=[Picture+8]+index*20
 → in-memory image probe → 2D/cube D3DX creation request into H14/H18
 → normalized metadata pointer Picture4 + child state Picture8
 → conditional descriptor publication: +18state / +1Cwrapper
 → indexed/name lookup returns wrapper to CLevel/model/UI consumers
 → child accessor → texture stage binding / surface-target requests

release request → descriptor count/pending → later callback event1
 → wrapper cleanup request / child deleting interface / payload free / clears
reset episode → distinct D3D output-cell registry → Release/Reset/Create attempts

UNKNOWN: valid payload/schema/allocation, successful API/callback, matching epochs,
         exclusive owner, final CPU/GPU borrower and content restoration
```

The archive, wrapper, normalized metadata, child holder, temporary decoded image, D3D output cell and COM object are **different objects/state domains**. An access path does not transfer ownership.

## 2. Identity and lookup before loading

| Origin / identity domain | Transformation | Consumer / stored state | Confidence / limit |
|---|---|---|---|
| String/name | Path normalization; uppercase key; basename/hash lookup | Archive record path/key and mapped manager index | **VERIFIED selected archive/lookup mechanisms** [T1] |
| Archive record | Stride20; selected field18 manager-record index; field1C path/key | Descriptor builder / extractor | No one-to-one global asset/type/admission theorem |
| Public numeric ID/control/mode | Current CRdData route | Queue packed low16/control8, or direct pending slot | **VERIFIED widths**, not full-width ID preservation [T2] |
| Request versus extraction identity | Conditional segment/group enumeration; selected0B46 extraction remapped4784 | Callback/manager destination may retain original record index | Request ID, physical extraction source and destination index must stay separate |
| CRdData descriptor table | `[manager+C]+index*30` | Accessors select state18/object1C/aux20 under gates | **VERIFIED pointed table**, not inline records in manager |

Steam acquired root getter`004051F0`; current GOG companion`004051C0`. Startup callback pair`00408310/004082D0`. Historical same-address getter shorthand is not cross-build identity proof. The manager cache`00BD9E3C` has startup alias`00BD768C`; later getters/current pointers are not automatically one stable generation. [T2]

## 3. Request, loading and conditional publication

CRdData initializer receives record-count literal`0x4BD8` and stores callback at+10; descriptor table at+C and mutex+388 are separate support state. CLoadThread static`01481130` is not the manager. Its invoked startup route has a named OS-thread entry/receiver chain, but thread success/interleaving is **UNKNOWN**.

| Route | State created | Service / downstream consumer | What completion does not mean |
|---|---|---|---|
| Nonzero low-byte mode | Packed DWORD `(ID&FFFF)|((control&FF)<<16)` in worker vector | Pop saved value→signed16 ID→current manager dispatch | Outstanding decrement is not successful publication |
| Mode0 direct | Worker pending+3C; WORD+3E ID; BYTE+40 control | Worker reads slot, dispatches, then clears pending after normal return | No token ties clear to one producer or one typed result |
| Existing record arm | WORD descriptor2C increment; returnFFFFFFFF | Retains existing state without new extraction/callback | FFFFFFFF is not a uniform failure or success code |
| Per-record extraction | Zeroed local30-byte descriptor; temporary payload | Callback event0 while manager mutex is held | Extraction/result/read completeness is individually gated/unknown |
| Nonzero callback arm | control2E, status2F=2; twelve-DWORD table copy; WORD2C increment | Current lookup consumers | Local commit≠valid full resource, current instance or exclusive ownership |
| Zero callback / extraction failure | No successful descriptor-copy arm | Worker can nevertheless return/clear/decrement | A requested load is not a loaded texture |

Phase7 P7X05 verifies these alternatives in both builds, including typed XPC rejection on nonmatching magic. Local mutex intervals do not serialize every producer or close all admissions. [T2]

## 4. XPC2 parsing and concrete child handoff

Let P be the CRdPicture receiver, B the incoming payload and M the current metadata pointer. Selected Steam parser`006B5190`, GOG`006B50E0`:

| Operation | Established state / value | Evidence state and scope |
|---|---|---|
| Admission | B nonnull; firstDWORD=`0x32435058` (XPC2) | **VERIFIED local magic gate**, not valid length/full grammar |
| Early state | `P+4=B` before child work | **VERIFIED early publication**, not atomic successful parse |
| Child cardinality | BYTE metadata+C × USHORT metadata+A | **VERIFIED selected product**, no all-input overflow/admission guarantee |
| Allocation/construction request | count×20+header bytes; count header; iterator offered ctor`00402220`/dtor`00402250`; returned storage candidate storedP8 | **VERIFIED operands/store**; named CRdTexture-compatible array **SI**, every initialized live child **UNKNOWN** |
| Metadata row | Step20; nonzero row+14 selects work | Row role/complete schema **UNKNOWN** |
| Image source | payload base + row+10 | **VERIFIED address arithmetic**; valid extent **UNKNOWN** |
| Size/control | size from three bytes atrow+1D; control=`(BYTE row+1C==2)` | Exact numeric fields, not invented names such as cube flag |
| Decode request | Allocate declared size; zlib1.2.1 init/inflate/end with source/size/output state | **VERIFIED request operands**, successful complete inflation **UNKNOWN** |
| Child call receiver | Current`[P+8]+index*20` | **VERIFIED exact Steam006B53DB–53EA / GOG006B532B–533A** [T3] |
| Arguments to texture helper | Temporary buffer Btmp, numeric control, declared size, literal1 | Exact raw stack order; valid decoded size/content **UNKNOWN** |
| After helper | Temporary buffer deallocation request | Not proof the API captured all data or final GPU lifetime |
| Metadata normalization | Pointer-difference size; allocate/copy incoming payload; replaceP4; optional size output; free request on incoming B; storeP+C input | **VERIFIED selected local sequence**, not fail-atomic publication or complete payload ownership |

This fills the **address/argument seam** left unresolved in seq62, at a new specifically authorized Phase8 scope. It does not retrospectively make that old failed qualification a success. The current child field is reloaded after opaque calls: no stable earlier allocation/type/generation theorem follows solely from the common P receiver.

## 5. Decoding/probing → D3D resource creation

Steam`006B58D0` (selected GOG call target`006B5820`) saves incoming H, first requests existing cleanup`006B59A0`, then offers source Btmp and a local image-info output to`00732C20`.

- **VERIFIED:** probe AL0 skips both imports and post-import dimension stores.
- **VERIFIED:** probe nonzero selects2D when local type-like DWORD is0, cube otherwise. The parser's numeric control argument is not proven to be that type discriminator.
- **VERIFIED:** 2D D3DX request output is`H+14`; cube output is`H+18`. Current device is loaded from`01480984`, source buffer and supplied source-size are forwarded.
- **VERIFIED:** import EAX is not tested beforeH8=0 and WORDHC/HE stores. A MOV AX replaces the low16 of EAX before return; the full return is **not** preserved HRESULT.
- **UNKNOWN:** valid image grammar, successful decode/create, written output cell, usable dimensions/current device and every branch's lifetime semantics. [T3,T4]

The previously established native-instruction path is a distinct producer: InstructionJoy.dds/Instruction.dds literal choice→file-size/allocation/read requests→named20-byte CRdTexture candidate→the same helper, but source-size operand`0x7FFFFFFF`, not the observed file length. It then closes/frees temporary-source state by requests. Later presentation/cleanup reloads an independently current global`01472224`. This does not certify source extent, capture, same episode or safe use. [T4]

## 6. Engine resource objects and consumer flows

| State/object | Stored origin | Transform / downstream consumer | Lifetime qualification |
|---|---|---|---|
| CRdPicture10-byte wrapper | Typed .XPC callback allocation/vptr | P4 normalized metadata, P8 child array candidate, PC identifier/control field | Record retains pointer; not exclusive ownership proved |
| CRdTexture-compatible child20 | Offered iterator ctor; currentP8 indexed address | WORDC/E dimensions; surface10;2D14/cube18/volume1C; texture/target binders | API success/current type/capture unknown |
| Manager descriptor18 | Callback-selected normalized state pointer | State lookup; later payload free request | Not identical to wrapper1C or every child |
| Manager descriptor1C | Callback-held wrapper pointer | `006B2C70` typed-object accessor | Validity/count gate≠guaranteed object readiness |
| Named CLevel consumer | `LEVEL%03d.XPC` lookup pair; selected selector28 CLevel | `006BE6E0/006BE1F0` storesXMD/XPC values atO160/O164 | Concrete retention, no acquired owning reference/lifetime closure |
| Model packet | `P84=M160`, `P88=M164`, `P34=M` | Resource-aware picture/mesh selection and shader/texture submission | P88 is not actor pointer; cached packet may bypass fresh population |
| Picture indexed accessor | Steam`006B5660` | Nonnegative first index<USHORT metadataA, second<BYTE metadataC, separate state gate; returns`P8+(BYTE C*first+second)*20` | Record address, not copied/new texture; actual state/current generation unknown |
| Name/key child selection | `006B55D0` and wrappers`006B5550/006B5590` | Select child then texture-stage binder`006B6020` | Selected APIs, not all texture consumers |

**Cross-build role collision:** GOG`006B5660` is the metadata-relative offset-table helper; GOG child row/column accessor is`006B56A0`. Equal Steam/GOG006B5660 does not establish the same function role. Both selected bodies are retained in [T3].

## 7. Cache/publication boundaries

Three caches/retainers must remain distinct:

1. **Archive identity structures:** path/key/hash/record metadata.
2. **CRdData descriptor table:** reference/status/bookkeeping and state/wrapper pointers.
3. **D3D descriptor banks / output cells:** dimensions/usage/format/output-cell addresses for recreation.

Scene/model/world objects additionally retain resource pointers or submission packets. None is automatically the sole owner of all lower layers. `006BE6E0` can publish resource pointers before later allocation/preparation/null gates; selected pose/packet mechanisms are not atomic successful load publication. Phase7 A03/A14 governs freshness and early-pointer limits.

## 8. Release, reset and recreation

```text
resource/tag release request
 → descriptor WORD2C decrement/force + manager14 pending flag
 → later frame-tail006B2A40 sweep
 → zero-count/nonempty descriptor006B3AA0
      event1 callback → wrapper deleting-interface request / field clear
      current payload free request →0x30-byte descriptor clear

D3D device loss (separate protocol)
 → registry descriptor → output-cell addressS → COM=[S]
 → Release request / clear[S]
 → successful local Reset branch
 → Create requests with stored descriptors, no all-success gate
```

**VERIFIED selected order**, safe/destructive completion **UNKNOWN** [T2,T5]. Callback reentrancy/mutation can break a same-record epoch inference. CLevel clearing, descriptor erase, wrapper cleanup, child destruction requests, surface Release and GPU final use are not interchangeable events.

Picture cleanup`006B54B0` first clearsP4 without loading its old value, then processes currentP8/count-header/delete-interface, clearsP8 and setsPC=−1. The manager's descriptor18 payload/free path is separate; do not invent a standalone P4 allocation/free pairing from this preclear. Child cleanup and payload free do not prove all resource/packet borrowers are retired.

Loaded-image helper does not set the same registry marker as native target creation in every path; the reset banks do not prove all loaded textures are recreated with their original contents. Exact retention and recreation/content policy for a selected loaded image remains **UNKNOWN**. Renderer236 requires `descriptor=[owner+C]+index*18` and COM lookup`[[descriptor+10]]`, not the retired inline/direct forms.

## 9. Build correspondence and unresolved edges

| Node | Steam | GOG | Supported scope |
|---|---|---|---|
| Callback |00408310|004082D0|Typed branch/mechanics, qualified request/commit |
| Picture parser |006B5190|006B50E0|Selected child/metadata/request structure independently checked |
| Child texture call |006B53EA→006B58D0|006B533A→006B5820|Current-indexed receiver and stack transport; GOG whole helper not freshly re-certified |
| Picture own deleting interface |00408900|004088C0|Named available/selected callback interface |
| Child row/column accessor |006B5660|006B56A0|Explicit bounds/address mechanics |
| Metadata offset helper |006B5710|006B5660|Different physical address/role, not uniform delta |
| CLevel resource attachment |006BE6E0|006BE1F0|Selected retained values, not full publication/lifetime equivalence |

**UNKNOWN:** full file grammar and admitted source extent; all child keys/control meanings; allocation/iterator/decode/import success; stable wrapper/child/backing/output/COM generation; every cache/alias/consumer; resource→GPU final sample correctness; lifecycle closure and all-build readiness. The bounded check resolves a missing address seam, not those stronger joins.

## Evidence anchors

- **[T1]** `findings/formats/dpserial_archive_loader.md`; `resource_descriptor_fields.md`; `resource_manager_population.md`; exact qualified original scopes, not old completion/ownership prose.
- **[T2]** `findings/boundaries/resource_worker_typed_handoff.md`; `phase7_resource_request_completion_falsification.md`; `cmap_state_resource_phase_placement.md`; Phase7 A04/A03/A14 and closeout reliance.
- **[T3]** `audit/phase8_synthesis_2026-10-07/TEXTURE_EDGE_PRIMARY.json`: selected original ASM/own-PE instructions; Steam C`431263–431320`, `431334–431352`, `431438–431469`, `431488–431501`; GOG parser/accessor counterparts. Seq62 historical missing-edge/method record remains unchanged.
- **[T4]** `findings/boundaries/phase6_native_instruction_texture_import.md` C0312/BND247, exact195 consumer bytes/imports/MOVAX qualification; old `xpc2_d3d9_texture.md` ownership wording is not universally consumed.
- **[T5]** `findings/boundaries/xpc_resource_positive_portfolio.md`; `crdpicture_clevel_resource_attachment.md`; renderer236 contracts and `FLOW_D3D9_DEVICE_RESOURCES.md`.


## Targeted bridge connections — TBC003

**VERIFIED selected static scope** (semantic/epoch ceilings retained): Selected CHelp construction/registration selector35 -> stored callback006347D0/GOG00634720 -> first-argument receiver transport -> known native instruction import00633B80/00633AD0. Independent RTTI names CHelp only on this installed path. Generic vslotC is not the adapter. Native source/import body is reused; allother endpoint callers/currentcode/vptr/texturegenerations/APIcapture/success/lastborrower remain UNKNOWN. Details: `findings/boundaries/targeted_bridge_help_callback_import_receiver.md`.


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


## Final static campaign amendment — checkpoint259

**VERIFIED mechanics / STRONG_INFERENCE typedtext/savepresentationconnection:** EightprivateFISHINGXLYchildren coupletoCLayoutMessage32display/64transitionbanks and1000scratchramp/temporarydrawstateoverride. Restore reloadscurrentstate/slots/backing,notpostcallequality/transaction. Shared4Crecord current28/desired48 selectedpresentationselector4359origin iscontext3C slot3 desired168→nine-recordcurrent148; notfilemagic/checksum orsave-success/content proof. Constructorpartiallyinitializes/readsoldcurrentvectorbeforestatecheck; arbitrarycounts/index/scalars/currentepochs unadmitted.9SIownrolepairs include6ADCC0GOGdisplacementwithoutallcallee/privateXLY/saveorigin transfer.

Evidence/qualification: `findings/boundaries/final_static_layout_text_save_presentation.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint260

**VERIFIED mechanics / STRONG_INFERENCE controlpresentationfamily:** ActualCGame-relative8DWORDmaskbank mapscapturedInputqueries andcustomESIWORDtoken/EDIcode/EBXflagsmessageformatting. Recognizedmasksdropunrelatedbits, composite/unmatchedresultsleavepartialtokenpolicy, earlyspecial/04000000returnsbypassfinal3C53. Stageddirty/default/copy producersarefunction/mode-scoped,sparse52bytedefaultsnotfullreset/schema,204/208maymaskorbyte. RepeatedGame/capturedInput/opaqueapply/glyphrefsnotcoherentsnapshot/API/persist/drawsuccess.3SIownGOGlocalroles/datamaps doNOTextendproducercallees ormapnumeric55970. FSC002newown23byteOR helper issupport-onlyeffectqualification,notnewfamily/activationproof.

Evidence/qualification: `findings/boundaries/final_static_control_bank_message_tokens.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
