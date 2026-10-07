# Final preserved corrections and nonpromotion rules

**Authority:** checkpoint 230–238 component-matched companions, both semantic overlays and the checkpoint-238 closeout reliance map. Phase 8 applies these corrections without rewriting historical evidence or the ten sealed semantic ledgers. `DISPROVEN` applies to the exact stronger component—not automatically every claim in the same subsystem.

Addresses and field offsets are hexadecimal. Counts are decimal unless prefixed `0x`.

## 1. Material architecture corrections

| Retired or stronger interpretation | Current disposition and supported replacement | Authority |
|---|---|---|
| Renderer descriptors are inline at `owner+0x0C+index*0x18` | **DISPROVEN. VERIFIED replacement:** `[owner+0x0C]+index*0x18`. Bank, descriptor, holder, output cell, COM object and device lifetimes remain separate. | C0098, renderer236, H0232 active override |
| Reverse lookup matches a descriptor address, or its `+0x10` field directly contains the COM object | **DISPROVEN. VERIFIED replacement:** compare `[[descriptor+0x10]]`; return the first matching index. Aliasing and currentness remain **UNKNOWN**. | C0099, P7X06 |
| Reset eligibility implies every resource was successfully recreated | **DISPROVEN. VERIFIED local protocol:** nonnegative Reset → creation attempts/diagnostics → AL=1. Negative creation can coexist with a null output slot and continued eligibility. | C0097, P7X06, renderer236 |
| “Managed resources” means `D3DPOOL_MANAGED` | The selected literal pool 0 is DEFAULT under the D3D9 ABI, not MANAGED (1). Bookkeeping management is not a pool flag. | P7X06 argument grammar |
| Both synchronous physics halves execute under phase 7 | **DISPROVEN. VERIFIED selected order:** phase 14 → producer/first half → write 7 → second half → write 8/Event 6. | C0122, A07 |
| State 5 is submit and state 11 is primary fetch | **DISPROVEN at the selected spine:** phases 14/7 contain the relevant requests; phase 11 contains separate physics-facing work. | C0013/C0014, A07 |
| Marker 8/context clears certify successful fetch on the same scenes | **DISPROVEN entailment:** operation returns are untested; counts, arrays and scenes are independently reloaded. Successful/coherent simulation remains **UNKNOWN**. | A07 |
| Generic camera filtering of mode 9 excludes every mode-9 route | **DISPROVEN:** an alternate current-table cell-9 route lies outside that filter. Actual mode and cadence remain **UNKNOWN**. | A06 |
| One-slot input staging drops all subsequent activity; an active 30 Hz worker is proved here | Aggregate state behind a pending snapshot persists. Worker/cadence summaries lack supplied raw traces; the static `+0xBC0` protocol survives locally. | C0123, K0002, B0010 |
| The 128 controller cells are the 20 scene slots; an equal address identifies the provider across builds | **DISPROVEN domain/identity transfer:** provider-compatible use, current type and release ownership retain separate qualifications. | A09 |
| A full Main audio token is an eternal instance/epoch identity; its resolver locally bounds the index below 128 | **DISPROVEN:** sequence wrap/reconstruction can repeat token T; the resolver's `h & 0xFF` path has no local `<128` gate. | A05 |
| NativeUI `R+0x48` caches a body pointer; a sentinel validates the index before dereference | **DISPROVEN:** `R+0x48` caches a directory cell, the body is relative, and selected category dereference precedes later sentinel checks. | A08 |
| XCA rejection preserves old state; return 1 requires nonempty output arrays | **DISPROVEN:** cleanup occurs first; accepted zero-count cases can retain a resource and return 1 with null arrays. | A10 |
| XAM NULL always detaches; pointer equality compares content/generation | **DISPROVEN:** the address-bit gate can preserve current state for NULL. Block `+0x04` and resource `+0x14` are distinct. | A11 |
| The actor table proves a complete state population or 82 behavioral homologues | **DISPROVEN extrapolation:** 137 physical cells contain 119 nonzero entries, 18 holes and 82 unique nonzero targets. Admitted states/execution remain **UNKNOWN**. | A12 |
| GOG NPC table/body agreement proves Event 2 admission and safe cleanup | The stronger join remains **UNKNOWN**. Conditional typed-target/provider/child effects do not supply admission or lifetime proof. | A13 |
| Packet acknowledgement/cache validity proves latest pose; cleanup means the final GPU borrower is gone | **DISPROVEN entailments:** valid packets can bypass population. Local cleanup/clears/requests do not establish freshness or retirement. | A03, A14 |
| Two actuator setters form one atomic physical command with known units/backend | **DISPROVEN extrapolation:** two independently sampled formal setter requests/stores survive. Executor, physical meaning and paired execution remain **UNKNOWN**. | A15 |
| EffectAdmin's 500 zeroed DWORDs constitute a ready owning descriptor table; cleanup deletes all 500 objects | **DISPROVEN:** descriptor access uses the separate pointed table at root `+0x04`; selected available installation is now **VERIFIED** byPGC002; currentadmission/readiness/ownership remain **UNKNOWN**. Known cleanup requests concern the mutex child/outer storage, not a 500-object walk. | A16 |
| ItemManager wrappers consume incoming manager ECX; nonzero always means success; construction makes its table ready | **DISPROVEN:** ECX is overwritten, the helper reacquires the root, and rejected EAX can remain nonzero. Named SRL normalization and selected available installation atroot `+0x0C` are refined byPGC003; currenttable/readiness/success remains **UNKNOWN**. | A17 |
| Pending clear/outstanding decrement certifies a matching full-width successful resource load | **DISPROVEN:** truncation, remapping, zero/many results, no-work and rejection paths survive. No request token or aggregate success certificate is established. | P7X05 |
| Save protocol 0/step 2 certifies admission, full transfer or coherent staging | **DISPROVEN:** rejection can leave old fields; step/poll lacks a request epoch; diagnostics and the result protocol are separate. | P7X04 |
| A local mutex or all-ones mask proves global serialization/pause authority | **DISPROVEN:** fixed-word participants, writes, alternative Timer writers and independent getter epochs are distinct. | P7X02 |
| Factory/handle lookup yields a complete product lifecycle; cleanup destroys every descendant | Stronger ownership, success, generation and completeness entailments remain unlicensed. Selected registry/slot/request mechanics survive at their documented scope. | P7X01/P7X03 and prior companions |
| Steam `004B76D0` / GOG `004B77B0` is a shared request initializer | **DISPROVEN attribution:** selected CNpcMain cache getter. A representative caller constructs its own `0x40`-byte stack request. | C0106 current companion |
| Anonymous rank/offset `+0x620` identifies one resource-owned ANIMATION class | **DISPROVEN promotion basis:** independently named boss/driver candidates and a separate NPCEnemy static relation survive. Actual-current composition remains **UNKNOWN**. | P0, checkpoint238 |

## 2. Qualifications that must survive reuse

- An original export body is not the complete callable universe, unique semantic owner or corrected CFG. GOG false-noreturn, orphan, shared and raw-only channels remain explicit under `baseline_v7`.
- Constructor/destructor/finalizer availability is not actual invocation, successful free, exclusive ownership or final-borrower closure. Access/backpointers/retention do not establish ownership; equal addresses/IDs do not establish equal generations.
- Directional-shadow rescue, mesh LOD, active-list distance, streaming cells, alternate 3D representations and NPC secondary work are different domains. The old 1995 skeletal-cutoff attribution is retired; the selected GOG caller's 500 gate is separate from the adjacent 1995 constant.
- The far representation is not proved to be a 2D billboard/impostor. Selected model/resource-pair binding survives; all 75 friendly names/policies remain **UNKNOWN**.
- Shared actor `+0x50` transform helpers are not direct CLevel methods solely because fields look similar. Receiver/class/slot scopes remain qualified.
- GameRecord's pre-copy bit-31 OR can be overwritten by the copy; the roundtrip's post-copy OR is distinct. Selected copies do not validate the complete on-disk grammar or load chronology.
- Movie-helper AL=1 establishes helper presence—not playback, completion or session/backing success. C0117's finite returning convergence does not prove eventual repeat-loop exit or allocation/exception progress.
- Core's 13 interior starts/static cohorts and Event-compatible use do not prove owned members, one current VM, a coherent generation or all caller types.
- Independent inline-field proofs in other objects are not retired merely because the renderer descriptor bank was corrected. Apply identity/component-matched precedence, not keyword blacklists.

## 3. Phase 8 draft corrections—not new engine contradictions

| Draft ambiguity | Primary-established correction |
|---|---|
| Effect descriptor stride “20” interpreted as hexadecimal | Index×5 followed by scale×4 is 20 decimal = `0x14` bytes, not the `0x20`-byte texture holder. |
| Item bound “306” interpreted as decimal | Compare `0x305` after selector−1 admits selected selectors `1..0x306` (774 decimal); tuple stride is `0x0C` (12 decimal). |
| Selected sound row 64 written as `0x64` | Selected SND_SE row `0x4E` contains first WORD 64 decimal = `0x40`; SE_LIST file offset `0x418` contains `plse066`. Row `0x64` selects different bytes. |
| MotionDriver “44-byte” wording | 17 DWORDs = `0x44` bytes = 68 decimal, not 44 decimal bytes. |
| `014B01E8` called a speaker-mask cell | It stores the opaque X3DAudio handle. The mask comes from local backend mix-format DWORD `+0x14`; destination count comes from a separate WORD `+0x02` query. |
| Bias algebra given without native rounding limits | Ideal real-number Y difference: `0.5/W − 0.5/H`. Native rounding, current shader selection and reported-bug causality remain qualified. |
| Legacy validator failure relabeled PASS because Phase 8 is authorized | Actual failure is retained. Separate Phase 8 state/provenance/conservation validation uses explicit authorization without changing historical control. |

Witnesses: `audit/phase8_synthesis_2026-10-07/NUMERIC_RECONCILIATION.json`, finite Phase 8 primary receipts and matched parent A16/A17/P0/selected-asset evidence. Raw sources and failed historical contracts were not rewritten into successes.

## 4. Execution/provenance history

Nine isolation-configured agent launches were stopped; no isolated scientific evidence, output or reviewer credit was adopted. Main-owned reconstruction and validation continued without reducing requested scope. Rejected tool arguments/payloads, the derived CSV field-size correction and legacy state-validator incompatibility are recorded in `audit/phase8_synthesis_2026-10-07/FAILURES_AND_CORRECTIONS.md`. These are execution events, not game observations or engine defects.

## Sources and nonpromotion

Use `maps/PHASE7_CLOSEOUT_RELIANCE.md`, renderer236's exact contract/companion, Event233/Core234/C0117-232, the A01–A17/P7X/P0 dossiers and both semantic overlays. Detailed Phase 8 maps retain component-specific evidence anchors. All 88 inherited guards remain unwaived. Completion/deferral never licenses stronger success, ownership, epoch or completeness claims. No implementation or runtime change follows from these corrections.

## Post-synthesis current supplement

Phase8 remains COMPLETE;[post-Phase8 white/gray closure](../maps/POST_PHASE8_GAP_CLOSURE_INDEX.md) refines six selectedstaticbridges (conditionalSteamshader/audioinput ceiling) andnarrowsoneActuatorABI gap. Originalcheckpoint243 report/package/validation remains preserved. Newclaim/boundary/function/site/class/global/vtable/homology/resource/subsystem companions live separately under`audit/post_phase8_closure_2026-10-07/`; tenhistoricalsemanticledgers unchanged.62entrylocators/48sitewindows/13declaredtables/17localcorrespondencegroups are scopedindexes, not62newly understoodfunctions orwholeenginecoverage. All88/checkpoint238 guards/renderer236/Phase6 DO_NOT_RENEW retained; currentepisode/success/ownership/runtime/bugcause limits notwaived.


## Targeted bridge campaign — current supplement

**VERIFIED selected static scope; stronger guarantees explicitly retained:** Targeted corrections preserve gate/count/reset distinctions, callback versus vslot, signed cursors, part offsets/masks, requested versus decoded size, q/T protocol separation and initial versus persistent input. Failed full thread-thunk acquisition is preserved;1/30 loading accumulator is not a render gate. See [the targeted bridge index](../maps/POST_PHASE8_TARGETED_BRIDGE_INDEX.md), `audit/targeted_bridges_2026-10-07/BRIDGE_REGISTER.csv`, reconciliation/frontier/handoff and manifest-bound validation. Protected checkpoint245 copies preserve the previous report. Ten canonical semantic ledgers/all88/overlays/renderer236/Phase6 DO_NOT_RENEW unchanged; no runtime or implementation work.


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
