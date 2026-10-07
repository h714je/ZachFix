# Whole resource system — identity, loading, publication, consumers and cleanup

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint238 evidence boundary.** This is a family-and-boundary map, not a complete grammar, universal cache or ownership model. Typed records, raw/normalized payloads, consumers, descriptor banks and backend resources remain distinct. The detailed texture/audio maps provide the deepest reconstructed paths.

## 1. Whole-system data-flow

```text
name / numeric selector / event / actor / stage request
                 │
       ┌─────────┴──────────┬─────────────────────┐
       ▼                    ▼                     ▼
 CRdData/archive        direct file/helper      dedicated audio banks
 identity/records       DDS/media/etc           XGS/XWB/XSB/backend
       │                    │                     │
 queue OR direct slot       │                     │
       │                    │                     │
 archive/raw/zlib            │                     │
       ▼                    ▼                     ▼
 local descriptor      temporary bytes       backend bank/interface state
 installed callback    typed candidate/API    named-node/cue protocols
       │
 extension branch / named special branch
       ├─ CRdMesh/XMD → model state/matrices/bounds → packet → renderer
       ├─ CRdPicture/XPC → child holders → D3D textures → model/UI/world
       ├─ XAM binding → selected CObjectSpecies state / attachment
       ├─ XCA1 → CRdInterp-compatible state / conditional arrays
       ├─ XWP → effect package/dependencies → runtime CEffect requests
       ├─ DSB → event-core slot/state requests
       ├─ XNV → selected actor/resource-binding requests
       └─ recognized other families → exact consumers/grammar UNKNOWN
       │
 conditional CRdData30-byte record publication
       ▼
 manager lookup + actor/world/script/UI service retainers
       │
 count/tag/pending/callback/free/clear mechanisms
       ▼
 local retirement requests — final borrower/ownership/success UNKNOWN
```

No single arrow joins every direct-file/audio family to CRdData. Callback recognition is not proof of parsing or one distinct class per extension. Allocation/publication/request/consumption/retirement are separate milestones.

## 2. Identity and storage layers

| Layer | Stored identity/state | Producer | Consumer | Qualification |
|---|---|---|---|---|
| Path/name | Normalized path, uppercase basename/key | File/path helpers and game formatters | Archive lookup, selected named special handlers | Different path/key/cue domains not equated |
| Archive metadata |0x20-byte records, selected manager-index18/path1C; hash chain | Archive metadata setup/lookup | Extraction and descriptor builder | Complete segment/class population **UNKNOWN** |
| Request transport | PackedIDlow16/control8 or separate directIDword/pending | Public/wrapper/current service request | CLoadThread dispatch | Truncation/remap/no-work alternatives verified |
| CRdData local descriptor |30 bytes; key0..17, payload/state18, wrapper1C, auxiliary20, counter/status tail | Extractor + installed callback | Conditional table copy | Stack descriptor is not retained pointer ownership |
| CRdData table | `[M+C]+index*30`; WORD2C, BYTE2E/BYTE2F | Nonzero callback arm / prior-reference increments | Field accessors and release sweep | Status/count≠full resource readiness or owning reference |
| Typed wrapper | Mesh54 / Picture10 selected candidates | Extension callback allocation/constructor requests | Manager records, actor/world/model retainers | Available release responsibility, exclusive ownership **UNKNOWN** |
| Model binding | M160/M164 resource values, state1E4, output1E8 | Early-pointer setup + conditional preparation | Animation/bounds/packet population | Not atomic successful publication |
| Model packet | M148, capacity144, P80 interior matrix payload, P84/P88 resource pointers | Conditional virtual44→28 | Scene pointer vector + renderer | Cache can bypass; current generation/GPU completion **UNKNOWN** |
| D3D registry | Pointed descriptor bank/output-cell addresses/COM values | Target/import/registry paths | Loss/reset/create/bind | Not CRdData record table or actor packet |
| Audio metadata/backend | PRM views, inline namedP, MainT/CoreK/statusQ, bank/cue outputs | Sound coordinator/dedicated bank load | Playback/spatial/status/cleanup requests | Loop/cue/node grammar and successful playback **UNKNOWN** |

## 3. Loading/control-flow lifecycle

| Stage | Concrete function/state seam | Established effect | Explicit missing edge |
|---|---|---|---|
| Acquire/configure | Steam`004051F0`, GOG`004051C0`; initializer`006B2570`, callbackpair`00408310/004082D0`, count4BD8 | Cache/result transport, table/callback/support setup | Actual ready/current generation/allocation |
| Request route |`006B1310`; literalqueued producer`0040B4C0/0040B490`; worker`01481130` | Nonzero-mode queue versus mode0 direct service | All producer/admission/serialization population |
| Archive open | `006B1500 →006AFD60 →006AF720`; DPSERIAL.001–.003 | Path/stream requests | Actual I/O success/content/full segment mapping |
| Extraction | `006B0000/006AFE40/006AF370`, zlib helper`006AF5B0` | Selected raw/XZP1/inflate framing/payload requests | Full payload length/grammar, every error policy |
| Callback | `006B33F0/006B3760 → callback at M+0x10, arguments (event0,D,index)` | Type-specific state/wrapper candidate | Nonzero response≠valid complete resource |
| Publication |12DWORD copy to current pointed30-byte table; reference WORD increment; control/status bytes | Selected local commit | Coherent allocation/epoch/ownership/fail-atomicity |
| Lookup |Name/index→distinct field18/1C/20 accessors | Conditional returned state/object/view | Returned pointer≠owning reference or complete subtype |
| Consumption |Actor/world/model/UI/effect/event service methods | Selected value/field/call transformations | Whole current consumer population/latest state |
| Counter/tag release |`006B2830`, tagwalk`006B29E0`; M14pending | Numeric count/deferred-work state | Exact borrowed-resource lifetime/admission closure |
| Frame-tail retire |`006B2A40 →006B3AA0`, event1 callback/free/clear | Request/callback/normal-return sequence | Callback mutation/reentrancy, final destruction/last use |
| Worker/manager shutdown |Manager record/table cleanup; later worker stop/storage routes | Selected local order | Stop/join/global quiescence/API outcomes |

**VERIFIED selected mechanics** [S1,S2]; lifecycle completion is **UNKNOWN**. Negative/existing-reference branches can returnFFFFFFFF without new publication; direct pending clear/outstanding decrement can follow no work or callback rejection. These are validated static alternatives, not runtime failure reports.

## 4. Models and animation flow

```text
.XMD callback candidate
 → CRdMesh ctorSteam004087E0 /GOG004087A0 (offered54)
 → normalizer0070BD20 /0070BCC0
 → descriptor18=[Mesh+4], descriptor1C=Mesh pointer
 → conditional manager table publication
 → selected lookup / actor-model binding006BE6E0 /006BE1F0
 → model160/164 EARLY pointer stores
 → state-array1E4 (A0 stride), matrix-output1E8 (40 stride)
 → selected same-model006C1430 →resource0070C130
 → output matrices →bounds006C2020 / conditional packet006BD4C0
 →0070CF90 transforms/transposes/index-selects12DWORD copies
 → packetP80 payload, P84/P88 associations →scene63A8→draw requests
```

Typed Mesh wrapper and state pointer are separate from each model actor's evaluation buffers. The updater takes current resource receiverR=M160 and offers state/base matrices/output pointers; it is not itself the same object as M. PacketP80 is aligned interior packet storage, not an alias ofM1E8. **VERIFIED selected value genealogy; SI broader animation/pose role.** Full XMD header/node names/interpolation/bone/mesh/shader grammar remains **UNKNOWN**. [S3]

Phase7 A03/A14 preserves early resource publication, conditional allocation, cached packet bypass, cleanup requests and missing freshness/last-use. Do not call a retained resource or packet “the latest successful pose”.

## 5. Family flow register

| Family | Established typed/data seam | Consumer / transformation | Cleanup/publication limit |
|---|---|---|---|
| XPC/XPC2 | CallbackPicture10, magicXPC2, child allocation/current0x20-byte holder, probe/import requests | CLevel164/modelP88; child key/index→texture stage / surface binding | Concrete address seam checked; valid decode/API/current allocation/exclusive owner **UNKNOWN** |
| XMD | NamedMesh54 + normalizedstate4/descriptor18/1C | Actor state/matrices/bounds/packet | Grammar/ownership/latest publication **UNKNOWN** |
| XAM | Selected namedSEEDG resources→CObjectSpecies virtual54 / generic bindingstate | Setter`006B9020/006B8F70`; currentresource14, retainedblock4/size8 | Pointer-address equality only; NULL does not necessarily detach; selected block allocation/transform-request producer joined by TBC007; owner/decode success/epoch **UNKNOWN** |
| XCA | SelectedD%02d.XCA→sceneowner2E8 CRdInterp-compatible object | Setup`006B9E70/006B9DC0`: XCA1/countbyteA, resource4, arrays8/C/10 | Cleanup-before-reject; zero-count returns1 without arrays; full grammar/ready/success **UNKNOWN** |
| XWP | Lookup/defaultpathUPDATA/EFFECT/XWP; ver1.5 check; dependency requests | NamedCEffect348 candidate; configuration/callback/active registration | Full package/part schema, ready members/current typed result/lifetime **UNKNOWN** |
| DSB | Callback event-core setup`00408960/00408920`→`0070DD40/0070DCE0` | Signed slot resolver`0070E820/0070E7C0`; selected slot/state dispatch | No complete bytecode grammar or admitted event/scheduler ownership |
| XNV | NamedOGE60511.XNV / selected selector40/41 setup | Hook/tackle/physics-chain actor resource binding | “Navigation” remains label-level **SI**; full data grammar **UNKNOWN** |
| PRM / SRL | Named sound/item/NPC parameter/list branches; selected actual soundPRM bytes | Sound selectors/control/name views; other consumers separately qualified | Sound table exact slice only; no universal PRM/SRL schema |
| ROA / NOD / PEA specialnames | PARKING/LOADPOS/PLACE.ROA, HOUSE_LIST.NOD, SCREEN.PEA recognized callback branches | Dedicated helper seams | Complete producer/product/count/schema/domain **UNKNOWN** |
| MES / NativeUI body | GenericMES branch plus separately established directory/category/body arithmetic | Selected UI message/cache/control consumers | Actual directory/body producer/schema/count/lifetime join **UNKNOWN**, not assumed same buffer |
| DDS / instruction texture | Direct file-read/buffer→CRdTexture→D3DX import | Native instruction/presentation consumer | Source-size7FFFFFFF literal, no extent/success/capture guarantee |
| XGS/XWB/XSB / PCM key | Dedicated backend settings/wave/sound-bank load; selected named key/inlineP | Audio bank/cue/spatial/voice protocols | PCM name≠decodedPCM sample; backend loop grammar/current resources **UNKNOWN** |
| PAM / PNG / XPM | Existing named use/load locators | Selected media/native/resource helper families | Typed parser/output/lifetime incomplete; retain **SI/HYPOTHESIS** only |
| DPB/FLG/GRS/IDX/XCM/XFE/XLY/XOP/XPF/XUL/XUS/XVO/XMI/XFX/POS/TBL/DIR | Callback/extension or name recognition at documented scopes | Most typed consumers/data meaning **UNKNOWN** | Recognition is not grammar or a distinct class |
| DB/LDP/PAR/SFO/ZIP | Historical extension inventory entries | Loader/product/consumer **UNKNOWN** | No inferred resource implementation |

The historical resource ledger's34 records are not the full format universe or34 decoded grammars. Semantic names such as “facial animation”, “expression”, “navigation” remain qualified where only extension/consumer analogy supports them.

## 6. XAM and XCA state transitions — exact nonpromotion rules

### XAM-compatible binding

**VERIFIED paired:** offered nonzero pointer unequal to current14 can request deallocation of separately retainedblock4 after size bookkeeping; pointer14 itself is not passed to that cleanup boundary. Same nonzero pointer skips prior-block cleanup while state stores can continue. Offered0 can preserve current14/block4. Resource pointer, backing block, tracked size and bookkeepingglobal`01481328` are different domains. **DISPROVEN:** NULL universally detaches; pointer equality proves unchanged content/generation. [S4]

### XCA1 preparation

**VERIFIED paired:** cleanup precedes null/magic test; rejection0 can follow old state clears/free requests. Acceptedmagic publishesresource4 before allocations. CountbyteA0 writesarrays0 and returns1; nonzero count is independently reread for size/zero requests2n/4n/4n. The scene caller does not gate continuation on EAX setup success. **DISPROVEN:** fail-atomic preserve-old transaction, success1 implies nonempty initialized arrays, complete source validation from magic. [S4]

## 7. Effects, item services and opaque descriptor-table producers

| Service | Origin/state/request path | What remains explicitly UNKNOWN |
|---|---|---|
| EffectAdmin cachedroot`00BD9E54` | Named construction; **VERIFIED selected static producer**00675390/006752E0 installs ID39C1 EFF_LIST.PRM interior view at+4; supplied195×20 name/default-selector records feed `[root+4]+index*0x14`; sharedstaging`0147FFDC/E0`, optionalmutex7D8 remain separate | Actualcurrenttable/admittedpayload/override/readiness; meaning of500zeroedDWORDs; outputtype/owner/success; process-wide serialization |
| Effect package/runtime | XWP/ID/name requests→CEffect configuration and active-object registration; selected fixed-delta flags | Fullpart/resource ownership, completion callback/admission/cleanup safe lifetime |
| ItemManager cachedroot`00BDA0F8` | Wrapper overwrites incomingECX; helper independently reacquires manager. **VERIFIED named source/admission**39E3 ITM_LIST.SRL→callback791×"444" endian normalization→historical00456D30/00456D60 base installation at+C; originalselector*12 triple→two field1C/one field18 paths; selected normalized row1 names CWP0011.XMD/XPC | Currentinstalled/readiness/epoch/allrowtypes/ownership; wrapper1..0x306 (774 decimal) stays distinct from791physical tuples; no uniformboolean/success certificate |
| Named boss/MotionDriver candidate | SelectedCBossBase/George2/3 and CMotionDriver construction; outer620 publication; driver8+selector*44→17DWORD copydriver2C; driver28 selectorstore | Actual-current receiver/driver/admitted-row/schema/provider/lifetime join; no inventedXAM/XMD/resource-owned common class |

EffectAdmin's zeroed500-cell bank is not its pointed0x14-byte descriptor table. ItemManager's selector gate is not installed-table readiness. Distinct resource-source helpers can discard formally supplied service ECX and independently acquire CRdData. Formal receiver transport is not exclusive resource ownership. [S5]

The boss/driver overlay is a **separate static candidate/configuration seam**, not a proved resource family. CNpcEnemy's independently named table/method association remains separate; no sharedR/class/schema is synthesized.

## 8. World, actor, script and save consumer boundaries

- **VERIFIED selected world retention:** CLevel stores returnedXMD/XPC values at160/164; CMap retains64 level pointers. State/tag clearing does not prove those wrappers/payloads are destroyed or all borrowers have stopped.
- **VERIFIED selected alternate representation:** desired444/current12/index416→two resource accesses→same-object binding→current-byte commit. All75pair identities and range/admission/currentness remain **UNKNOWN**.
- **VERIFIED selected actor factory namespaces:** general game selector factories, native/model task factories and service-specific factories are not interchangeable. A numeric selector/result does not supply dynamic type/lifetime after opaque calls.
- **Qualified event/script:** DSB/CEvCore/member requests, loaded/native command streams and callback tasks have concrete local transport/dispatch seams; actual producer/admitted schema/full executor/schedule remain distinct **UNKNOWN** joins. Do not equate decoded event requests with successful gameplay effects.
- **Save/GameRecord is an independent persistence system:** staging/serialization/physical-write and actor reconstruction paths interact with world resources, but are not one CRdData resource cache transaction. Copy/write requests do not prove coherent/durable load reconstruction.

## 9. Cleanup/reset/recreation are layered

| Layer | Local mechanism | Non-equivalence to preserve |
|---|---|---|
| Descriptor reference/control | WORD count, active bookkeeping, tag/pending | Count0≠no external borrower; mark≠immediate unload |
| Resource callback | Event1, wrapper interface, current payload free/descriptor clear | Callback≠completed destruction; reentrancy/epoch unknown |
| Actor/model | Rebind cleanup; state/matrix/packet deallocation requests and clears | Resourcepointer≠ownedbacking; cleanup≠latestpacket invalidation/final GPU use |
| Query membership | Node pointer removal/end-shrink/clear | Detach≠object/packet deletion |
| D3D | Output-cell Release/clear→Reset→Create attempts | GateAL1≠all resources/content restored; descriptorbank lifetime separate |
| Audio | Token/sentinel/cue/status/sharedbank cleanup requests | Reuse marker≠backend instance destruction; globals not exclusiveCore fields |
| Worker/threads | Running/pending/storage/stop protocols | Localwake/clear/mutex≠join or global admission/last-user closure |

Phase7 A04, A03/A14, P7X05/renderer236 and all88 inherited obligations remain unwaived. No universal final-resource owner is invented to make this diagram tidy.

## 10. Steam/GOG qualification

The major callback/type seams and selected archive primitives have explicit paired records. Workerqueue/direct/commit/type facts were independently extended in Phase7 P7X05. XMD/Picture constructors and selected CLevel bindings are paired. XAM/XCA setup and cleanup have paired local qualifiers. XPC child accessor/helper addresses have a real role split. Audio's deepest request/spatial/completion flow remains Steam-qualified while Core roots and Main token primitives have narrower GOG correspondences. Effect/item/boss candidates use their exact companion scopes, not whole engine parity. See the correspondence map.

## Evidence anchors

- **[S1]** `findings/formats/resource_manager_root.md`, `resource_manager_population.md`, `resource_descriptor_fields.md`, `dpserial_archive_loader.md`, `typed_resource_dispatch.md`; historical stronger wording narrowed by later companions.
- **[S2]** `findings/boundaries/phase7_resource_request_completion_falsification.md`; `resource_worker_typed_handoff.md`; `cmap_state_resource_phase_placement.md`; Phase7 A04 callback limits.
- **[S3]** `findings/boundaries/xmd_crdmesh_descriptor_commit_reconciliation.md`, `xmd_animation.md`, `xmd_crdmesh_consumer.md`, `animation_model_state_submission.md`; Phase7 A03/A14. Texture details and boundednewedge: `FLOW_RESOURCES_TEXTURES.md`.
- **[S4]** `findings/boundaries/phase7_architecture_a10_falsification.md`, `phase7_architecture_a11_falsification.md`; `xca_crdinterp_scene_state.md`; XAM/Species reports only at corrected pointer/request scopes.
- **[S5]** Phase7 A16/A17 dossiers and `maps/PHASE7_CLOSEOUT_RELIANCE.md:19–29,103–132`; P0 typed/class/homology companions. Post-synthesis selected table producers/admission refined below; stronger current/live joins remain unknown.
- **[S6]** `findings/boundaries/xwp_ceffect_package_boundary.md`, `xwp_ceffect_callback_lifecycle.md`, `dsb_event_core_dispatch.md`, XNV selected actor reports; resourceledger family names are historical declarations, not full grammar proofs.
- **[S7]** Detailed audio map and its primary/companion sources; worldrepresentation/query/save reports and exact closeout limits.

## Post-synthesis closure — PGC001–PGC004 (not a numbered phase)

The original checkpoint243 package is preserved separately; these current-map amendments use new finite primary evidence without changing sealed Phase1–8 claims or guards.

| Connected static seam | Origin → carrier → known consumer | Residual limit |
|---|---|---|
| **PGC004 VERIFIED mechanics / SI world-provider role** |CRdData field18→0041C4C0/0041C4E0→005E4A20/005E4AF0→CPut[bank] count8/source18/gate4→prior CMap progress and car/model providers |Reacquired pointer not certified by earlier null test; payload/key/bank domains, mutable interior lifetime/currentness UNKNOWN |
| **PGC002 VERIFIED selected producer** |ID39C1 EFF_LIST.PRM view→00675390/006752E0→EffectAdmin4→195×20 descriptors→existing name/default-ID effect request |Separate500-DWORD bank; currentpayload/defaultoverride/readiness/owner UNKNOWN |
| **PGC003 VERIFIED named admission/schema** |ID39E3 ITM_LIST.SRL→named callback→791×"444" in-place endian normalization→historical ItemManagerC install→selected XMD/XPC tuple requests |791physical rows≠wrapper774admitted selectors; no currentproduct/loadsuccess/owningreference proof |
| **PGC001 VERIFIED selected typed installation/use** |ID2400 MES_ALL.MES field18→0045F610/0045F640→CMessage44 directory→independently typed category/numeric reader |R48 is directorycell; loadedcode/count/body domain, epoch/lifetime/finaldraw UNKNOWN |

CPut binder retains resource interiors at18/1C/20/24/28 and relocates selected@TXT markers **in the offered payload**; activation precedes completing helper-keyed inline tables. It is neither an owning copy nor validated all-count readiness. NativeUI binder separately installs23FB GLOBAL.FLG at4C and23FC GLOBAL.IDX result+4 at50; task result is distinct from CMessage.

Evidence/reconciliation: [CPut](../findings/boundaries/post_phase8_cput_resource_binding_activation.md), [Effect/Item](../findings/boundaries/post_phase8_effect_item_table_admission.md), [NativeUI](../findings/boundaries/post_phase8_native_ui_resource_directory_installation.md); `audit/post_phase8_closure_2026-10-07/GAP_REGISTER.csv`. Original A08/A16/A17/RS qualifications remain, with only the specifically evidenced static producer omissions refined.


## Targeted bridge connections — TBC003/TBC004

**VERIFIED selected static scope** (semantic/epoch ceilings retained): TBC003 connects selected CHelp factory result and callbackrecord35 to the already-known native import endpoint, without universalreceivertyping. TBC004 connects XCA1 retainedbacking/2Ncursor/4Noutput/4Nsnapshot arrays to explicit same-model E=M1B4 scalar evaluation/blend (Steam006BA150/330/5C0; GOG006BA0A0/280/510). SignedWORDcursor, variable4+q*0xC rowsteps, indexed scalar outputs and EC->E10 snapshot are established numerically; friendlychannelmeaning/admittedgrammar/currentepochs/arrayvalidity/ownership/cadence remain UNKNOWN. No scene-to-model same-resource episode is inferred. Reports: `targeted_bridge_help_callback_import_receiver.md` and `targeted_bridge_xca_model_scalar_evaluation.md` under findings/boundaries.


## Targeted bridge connections — TBC007

**VERIFIED selected static scope:** Known compatible binding S14 -> selected sameS/localtransport006B84E3 ->006B92D0/006B9450/006B9E30 -> retainedS4/requestedsize8 and01481328accounting -> knownconditional006B9020cleanup. The89-reference/1054-owned-row directlocal screen supports boundedentryS/capturedresource operand, not alias/callee/epoch invariance. Helper separatelyreacquires S14; explicitresource andhelperresource maynotbecoherent. Requestedsize recordedbefore uncheckeddecode-shapedstatuses; nonnullpointer isnot successfuldecode/outputextent. Divisor/linkedrecord/grammar/admission/currentblock/owner/finalborrower remainUNKNOWN. Steamqualified. Report: `findings/boundaries/targeted_bridge_xam_retained_block_producer.md`.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** WorldCMap polling/start-clear bracket TBC001 nowconnects initialnamedloadingmembervslot4 toavailableflag-controlledpresentationcallback TBC012. Thisjoin isavailability/type/byte/control/request evidence, not successfulresource readiness, actualOSworker execution, fullloadingUI taxonomy, currentdevice/resource generations or safe final use. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.


## Final static campaign amendment — checkpoint255

**VERIFIED selected mechanics / STRONG_INFERENCE family:** World progress/prefetch feeds multiple XPM forms and model-key NxStream imports;97initialSteamXPM/XMD ID pairs plusTREEPHYterminal are executable catalog witnesses. Modelreader sends cachedpointer todescriptorB8kind4 and physicallynamedNxTriangleMeshShapeDesc6C; separate region/part cachekind5 converges ontriangle-shapepreparation. Writerboolword isnotbatchID, FULL32batchkeys differfromsignedLOW16readerkeys, cursorlengthnotstreamlimit, stagezeroSETZnotSDKsuccess, pre-callflag/separatecurrentSDKloads notcompletedvalidmesh. Unsignedguard/physicalcasearms doNOTrepairbaselineCFG.13SIpairedmechanisms doNOTextendGOGshape/corpus/runtime/lifetime ownership.

Evidence/qualification: `findings/subsystems/final_static_xpm_model_physics_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint259

**VERIFIED mechanics / STRONG_INFERENCE typedtext/savepresentationconnection:** EightprivateFISHINGXLYchildren coupletoCLayoutMessage32display/64transitionbanks and1000scratchramp/temporarydrawstateoverride. Restore reloadscurrentstate/slots/backing,notpostcallequality/transaction. Shared4Crecord current28/desired48 selectedpresentationselector4359origin iscontext3C slot3 desired168→nine-recordcurrent148; notfilemagic/checksum orsave-success/content proof. Constructorpartiallyinitializes/readsoldcurrentvectorbeforestatecheck; arbitrarycounts/index/scalars/currentepochs unadmitted.9SIownrolepairs include6ADCC0GOGdisplacementwithoutallcallee/privateXLY/saveorigin transfer.

Evidence/qualification: `findings/boundaries/final_static_layout_text_save_presentation.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
