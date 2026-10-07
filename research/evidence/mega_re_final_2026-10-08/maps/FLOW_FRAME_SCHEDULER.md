# Frame scheduler — conditional control-flow and state-flow map

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; evidence boundary: validated checkpoint 238.** The engine is reconstructed as nested conditional paths and partial orders, **not** one unconditional timeline. `VERIFIED` here means the selected executable operands/branches/calls agree with primary evidence. Callback occurrence, call frequency, coherent receivers/generations and API success are separate **UNKNOWN** propositions.

## 1. Top-level application path

```text
Steam00700650 / GOG00700670
   platform/build startup; 004017C0 initialization request
   │ successful local startup return
   ▼
loop until message==WM_QUIT
   ├─ message available → TranslateMessage → DispatchMessageA
   │                       → build-local window procedure / message boundary
   │                       [no automatic gameplay tick on this arm]
   └─ idle → selected pre-gate helper
             → D3D cooperative gate006CCEF0 /006CC990
                ├─ denied / lost / failed Reset → wait request, no tick
                └─ permitted → clock + delta publication
                               → 00401A70 engine tick
                               → 004011B0 post-tick conditional presentation work
                               → foreground/cursor requests
```

**VERIFIED paired order:** gate before timing before one `00401A70` invocation per eligible idle traversal. This is not a frame-rate cap or a statement that all messages, all iterations or all GPU presentations cause an update. The post-tick helper contains conditional multi-iteration render/Present calls, so even a returning ordinary tick does not prove a global 1:1 update/Present relationship. [F1,F9]

Steam has SteamAPI startup/shutdown and account gates surrounding this application path; GOG's corresponding non-Steam path is independently established. Do not infer general Steam/GOG behavioral equality from their shared tick address.

## 2. Main tick control spine

| Selected local position | Caller → callee / receiver | State / downstream effect | Evidence state / limit |
|---|---|---|---|
| Entry | `00401A70` increments `008A971C`, calls `00405270`, other service/profiling-like interfaces | Tick counter and early service requests | **VERIFIED visible stores/calls**; full service semantics **UNKNOWN** |
| Input commit | Site `00401AB0 → 00708350` Steam | Previous←held; pending→live; edges/repeat | **VERIFIED selected same-root** [F2] |
| Separate provider accounting | Selected current provider, local lock, 64-bit increment at+1C38/+1C3C | Independent request/accounting state | Not global serialization or a timing source |
| Input poll | `00401AF0 → 007099D0` Steam | Conditional acquisition/evaluation→aggregate→pending | **VERIFIED** BC0 gate; can be suppressed [F2] |
| Other early services | `007078E0`, `00705920`, `00702890`; copy `014AFFE0→00BD766C`; `006E1800` | Pre-dispatch service/value operations | Exact selected calls known; not all named as keyboard/audio/world by analogy |
| Repeat-loop body | Order selector, dispatcher and context companion | Object phases and companion work may repeat | **VERIFIED cycle**; eventual exit **UNKNOWN** [F3] |
| After loop | `006B62E0` and selected 0x40 matrix copies/multiply/inverse | Context/projection/staging state | Returning-loop position **VERIFIED**; no universal scene generation |
| Pre-render | `00401C42 → 006D2B40` Steam / `006D2710` GOG; ECX current application scene root, selected context argument | Query/visibility/conditional packet population→submission collection | **VERIFIED selected pointer genealogy** [F5] |
| Render/media phase | `006CC6D0(2)`, selected service, `00401C75 → 00401440(context)` | Staging, conditional scene consumption, movie request, later presentation/fade | **VERIFIED returning finite paths** [F3,F5] |
| Resource/retirement tail | selected helpers; `006C7070`; `00401C98 → 006B2A40` with acquired CRdData | Manager retirement request; pending resource sweep/callback/free/clear requests | **VERIFIED** order, not safe final-use retirement [F6] |
| Late WinMM call | `joyGetPosEx(0, stack storage)` | Separate acquisition request | **VERIFIED call**; no proved edge into the earlier action-record producer [F9] |
| Presentation | `006CC8D0(0)` | Current device EndScene/Present-shaped virtual requests | **VERIFIED operands**, API names **SI D3D9 ABI**; success/blocking **UNKNOWN** [F9] |
| End | `01480960++`, toggle`01480968`, rotate`01480964 mod3`; `0040BE50` | Frame-index/phase bookkeeping and tail request | Numeric state is not proof of completed GPU use |

## 3. The repeat loop and independent context reads

```text
read CDemo+4 bit31 (008A6074) via0041C270 /0041C290
   ├─ 0: context companion(delta) → object dispatcher(delta)
   └─ 1: object dispatcher(delta) → context companion(0)
                  │
          006C75F0 /006C70F0 continuation
          00409DC0 /00409D90 reads byte00886FF8
                  ├─ nonzero → repeat entire body
                  └─ zero → exit
                            read selector AGAIN
                            ├─1 → source00BD86B8
                            └─0 → source00BD9648
```

Dispatcher receiver is the current pointer in `00BD7670`, an application alias of the CRdSceneDraw cached root `00BD9E68`. Companion receiver is in-place `00BD9648`; Steam helper`00701350`, GOG`007012B0`. Source context and later fixed staging context `00BD9E70` are not the same domain. [F3,F5]

**VERIFIED current C0117 qualification:** both finite returning order/context arms converge on the post-loop media-containing phase. **UNKNOWN:** repeat count/eventual exit, intervening normal returns, allocation survival, selected root currentness and equal selector readings. The media call is not unconditional over infinite or exceptional executions. CDemo's selected installed callback/reset paths supply bit31 producers, not a complete user-facing mode taxonomy. [F3]

## 4. Object-dispatch phase order — Steam selected pass

Receiver `D = current CRdSceneDraw`; marker field `D+18E0`. These numbers are **object-pass markers**, not research phases or real-time timestamps. The dispatcher constructs selected original-object sets before invoking virtual interfaces. Full pointer-collection membership is qualified by Phase7 A02: duplicates, reloaded nodes and current populations are not excluded. [F4]

```text
0 → 1 → 2 → 3 → 4 → 5 → 6 → 10 → 14 → 7 → 8 → 11 → 12 → 9 → 13
```

| Marker | Selected state/calls | Proven consumer role / unknown boundary |
|---|---|---|
| 0 | Clear selected scratch/pointer storage at+1CAC; enumerate/classify original object pointers | Builds per-pass collections; no complete object universe |
| 1 | Select eligible objects into `D+1CAC`, update count`+1CA8`, apply mode/distance/control predicates | Selection is not one universal draw-distance test |
| 2 | Eligible original object `vtable+0C(delta)` | Native object/task update; installed callbacks can emit event1. Audio/world/menu/effects tasks are conditional branches here |
| 3 | Registry walk; pending flag/mask reconciliation; selected `vtable+30` retirement-mark path | Mark/request is not destruction or completed retirement |
| 4 | Eligible `vtable+10(0)` | Typed model `006BCBE0 → same-M006C1430` conditional animation/state evaluation; resource/mask gates can skip |
| 5 | `0052E0D0` camera wrapper | Gated generic camera route; **not** primary physics submit. Mode9 alternate route preserved |
| 6 | Selected `vtable+14()` | Interface call under predicates; complete gameplay semantics **UNKNOWN** |
| 10 | Selected `vtable+18(4)` | Numeric event4 interface under masks; full class/consumer meaning **UNKNOWN** |
| 14 | Acquired core `006EB3C0` plus synchronous first sweep`0040B750` | Physics producer and first-half requests, before marker7 |
| 7 | Second sweep`0040B780` | Fetch/restore-shaped requests, not verified successful completion |
| 8 | Eligible `vtable+1C(6)` | Post-second-sweep event6; not a PhysX-substep callback |
| 11 | Acquired core`006EB400` | Separate physics-facing resistance/refresh-compatible continuation; **not** primary fetch |
| 12 | Selected type/mask gate→`006BCF10` | Specific post-physics object interface; complete effect **UNKNOWN** |
| 9 | Acquired core`006F9800` | Selected refresh/reset branch; may reinitialize session state |
| 13 | Eligible `vtable+48(D+1CA4)` | Spatial membership/query update using named COctTree member; object+14C node retention |

The exact phase14→first-half→7→second-half→8 windows and matching GOG request order are verified by Phase7 A07. Other whole-method GOG conclusions must use repaired baseline/selected overlays, not a truncated decompiler or an assumed uniform relocation. [F4]

## 5. Concrete producer/consumer branches inside the spine

| System | Installed/typed origin → transformation → later consumer | Relative placement established |
|---|---|---|
| Audio | Startup selector0 task installs raw`00454D50`; conditional event1→acquired CSound`0046F360`; core maintenance then CSdMain update | Can occur through marker2 task interface; no total order relative to other selected tasks [F7] |
| World | Installed event1 tasks conditionally write CMap+10=3/4; independent raw selector15→world handler, selector13→resource/code consumer | Task occurrence conditional; raw selector's incoming frame placement **UNKNOWN** [F6] |
| Animation | Named model/player slot10→same M state/resource evaluation→M1E8 output matrices | Eligible marker4 before post-loop pre-render; actual selected instance/latest pose **UNKNOWN** [F5] |
| Spatial query | Object slot48 retains/removes node; scene+1CA4 query exposes original pointers; visibility/slot44 packet path | Marker13 before later pre-render on a returning traversal; coherent all-object membership **UNKNOWN** |
| Rendering | Selected original M→packet M148→scene+63A8 pointer collection→fixed-staging consumer→resource-aware submission | Packet can bypass repopulation; vector clear is not packet deletion or GPU completion [F5] |
| Menu/UI | Native task callback`00654490` switches from task to static CMenu; input predicates/camera flag changes; NativeUI directory/body selection is separate | Conditional task event1; full menu mode/scheduler and table/body producer **UNKNOWN** [F8] |
| Movie | Current CRdMovie root`00BD9E48`→`00700500/00700520(0,1)` in post-dispatch phase | Normal-return convergence established; entry-helper presence AL is not playback/completion success [F3] |
| Fade/presentation | Installed callback updates retained CFade data/latch`00BE1EAC`; selected application-tail latch test/clear→CRdPrim request | Latch clear can occur without draw; no freshness/generation/GPU completion guarantee [F8] |
| Resource retire | Tag/counter force sets manager+14; later frame-tail sweep calls installed event1 callback then free/clear requests | Deferred mechanism; independently occurring world handler is not joined to safe final use [F6] |

**Animation ordering does not imply pose freshness:** existing valid M148 can bypass the population path, and callback/virtual calls can change current resources or state. **Render access does not imply packet/resource ownership.**

## 6. Threads, callbacks and local synchronization

| Actor / thread mechanism | Concrete chain | Scheduling ceiling |
|---|---|---|
| Application main thread | Win32 idle/message loop→eligible tick→nested dispatcher/render/tail | Exact local control flow, not measured cadence |
| CLoadThread static`01481130` | CRdData initializer→start→generic CreateThread argument W/entry`00712D30`→W slot4`006B4DE0` | Invoked API route **VERIFIED**; successful OS activation/interleaving **UNKNOWN** |
| Resource queue/direct service | Packed queue value or one direct slot→worker dispatch→current CRdData callback/conditional descriptor commit | Mutex intervals are not global producer serialization or success receipts; direct clear/outstanding decrement can follow no work/rejection |
| CPhysicsThread contexts | Available `0040B630` start sweep→same context generic entry→slot4 worker | Incoming activation **UNKNOWN**; synchronous sweeps are a distinct positive main-spine path |
| CInput callback worker | Embedded helper/callback and ~33333µs-style implementation; BC0 handoff API | Historical dormancy/census is secondary without raw traces; actual activity here **UNKNOWN** |
| Native object callbacks | Installed code at object+44; numeric object interfaces invoke events | Initial synchronous event0 is not event1 recurrence; independent task order **UNKNOWN** |
| Deferred root/resource callbacks | Carrier current-code call; callback/free/clear/mark order | Reentrancy, mutation, borrower closure and callback epochs remain **UNKNOWN** |
| Backend/media callbacks | API/interface requests from current audio/movie receivers | Actual external-thread callback schedule and completion **UNKNOWN** |

These threads are not collapsed into one globally locked frame transaction. Wait/lock requests, stable receiver addresses and cleared flags do not establish coherent generations or quiescence. [F6,F7,F10]

## 7. Alternate menu, movie, loading and reset paths

- **VERIFIED:** Win32-message and denied-device-gate arms do not traverse the ordinary idle tick. Device-loss sleep/Reset paths belong before delta/tick eligibility.
- **VERIFIED:** selector-dependent companion/dispatcher order and repeat loops can alter how many object passes precede pre-render. Human “gameplay/cutscene/pause” names are not assigned merely from bit31.
- **VERIFIED:** movie work remains inside the selected post-dispatch phase; numeric`008A7220=46..49` skips a later helper only. That is not a movie-only frame scheduler.
- **VERIFIED selected seams:** CMenu and transition/resource presentation callbacks use input/world/resource state, but TBC012 below supplies an available initial-type flag-controlled loading-worker presentation loop; complete actual activation/menu/loading taxonomy remains UNKNOWN. Show their branches inside/adjacent to the shared spine rather than inventing a total alternate scheduler.
- **VERIFIED:** resource mode0 direct service can block its caller on a pending flag while worker service occurs separately. Normal return/clear is not loading success or selected asset readiness.
- **VERIFIED:** `004011B0` can execute additional presentation work after the main tick when `00BD77BC!=0`; exact request sequence is separate from ordinary tick count. TBC002 below supplies selected paired numeric state producers; complete/live trigger policy remains **UNKNOWN**.

## 8. Explicit unresolved edges

**UNKNOWN:** every object's admission/selection, total callback order and live cadence; whole current world snapshot; all API results; actual thread activation/interleavings; UI/movie/menu/loading mode taxonomy; final GPU and resource borrower completion; successful reset/resource reconstruction; matching submit/fetch populations; complete GOG dispatcher behavior outside qualified fragments; progress through repeat/exception/allocation paths. None is filled by a familiar engine-loop analogy.

## Evidence anchors

- **[F1]** `findings/boundaries/application_idle_frame_loop.md`; C0096/H0228. `frame_d3d9_cooperative_gate.md` must be read with renderer236 request/success correction.
- **[F2]** `findings/boundaries/input_camera_primary_roots.md:28–67`; detailed controller map and bounded primary receipt.
- **[F3]** `findings/boundaries/application_media_phase_placement.md`; `phase7_c0117_exact_fragment_falsification.md`; checkpoint232 exact-primary receipt. CDemo producer: `cdemo_context_selector_provenance.md`.
- **[F4]** `findings/boundaries/object_virtual_phases.md`; Steam C`441058–441304` checked against its cited ASM/accepted structural scope; `cphysicscore_lifecycle_root.md`; Phase7 A02/A06/A07 companions. GOG baseline-v7/revalidation continuation, not export extent alone.
- **[F5]** `findings/boundaries/animation_model_state_submission.md`; `scene_submission_pointer_pipeline.md`; `scene_query_member_lifecycle.md`; Phase7 A03/A14 qualifications.
- **[F6]** `findings/boundaries/cmap_state_resource_phase_placement.md`; `resource_worker_typed_handoff.md`; `phase7_resource_request_completion_falsification.md`; Phase7 A04 callback qualification.
- **[F7]** `findings/boundaries/gameplay_audio_organizing_roots.md`; Phase7 A05 token/instance limits.
- **[F8]** `findings/boundaries/cmenu_organizing_task_use.md`; `presentation_movie_fade_latch_chain.md`; Phase7 A08 NativeUI producer/body limits.
- **[F9]** Steam C`565–677`, `88–125`, `444520–444559`; Present node checked in `audit/phase8_synthesis_2026-10-07/TIMING_EDGE_PRIMARY.json`; C0117 exact frame primary for local callsites.
- **[F10]** `findings/boundaries/physics_context_vector_activation.md`; `phase5_resource_worker_shutdown_coordination.md`; `maps/PHASE7_CLOSEOUT_RELIANCE.md` and identity-matched companions.


## Targeted post-Phase8 bridges — TBC001/TBC002

**VERIFIED selected static connections:** TBC001 links typed CMap polling005DA8E0/GOG005DA9B0 to start/clear wrappers00409D20/00409D50 (GOG00409CF0/00409D20), byte00886FF8 and the known frame-repeat read/backedge. Initial member00886FE0 has independently RTTI-named CDrawLoadingThread vptr. The byte is set before the start request and cleared after a selected member request; neither store certifies worker activation/join/quiescence or eventual polling exit. See `findings/boundaries/targeted_bridge_loading_repeat_bracket.md`.

TBC002 links selected numeric receiver S fields25C/260 in0061F660/GOG0061F5E0, proven local ECX-preserving state helper, and006227A0/GOG00622720 to extra-presentation00BD77BC. Selector1/arg2=0 writes2+(S22C!=0); selector3/arg2=0 writes2. Known004011B0 consumes nonzero as a gate to two separate local three-iteration loops and clears it. Values2/3 are not loop counts and do not admit the value1 reset branch. S class, actual cadence/API success and all-producer policy remain UNKNOWN. See `findings/boundaries/targeted_bridge_extra_presentation_producer.md`.


## Targeted bridge connections — TBC011

**VERIFIED selected static scope:** GOGstartupstoredcallback00454D80 (rawORPHAN) admitsnumeric event1through ownremap/table. On the selectednestedstateout-of-range arm itacquiresindependentlytypedCSound andrequests0046F450; its selectedbit31/world10!=3 continuingarm requestsknownCorebeforeindependentlyacquiredMain. This extendsconditionalGOGaudioorganizingwork, notactualtaskdispatch/recurrence/everyevent1arm/fullGOGdispatcher/currentepochs/success. See `findings/boundaries/targeted_bridge_gog_installed_audio_task.md`.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** InitialCDrawLoadingThread table+4 -> available0040A860/GOG0040A830 callback -> W+18-controlledloop -> selectedstage2/clear2/presentation0 -> declaredSleep16 request/backedge. OfferedW00886FE0 givesW18=00886FF8, joining TBC001/start/clear/frame-repeat. SteamgenericprefixreadsCURRENTvptr+4; initialtypeisnotactivation/currentvptr proof. Native1/30localaccumulator branchreconvergesbeforerenderrequests:NOT30Hzgate. Existingcallee/currentdevice/API/concurrency/lastborrower/progress/cadence guards unchanged. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.


## Final static campaign amendment — FSC001 / checkpoint252

**VERIFIED selected mechanics; STRONG_INFERENCE conditional composition.** New current companions connect busy-gated `00401050` key/payload insertion and comparator `00401020` to the accepted `00401080` drain. Selected Steam event0x12 paths (including only the established CHelp-installed path into00633B80) enqueue key0 then exit on admission; available independently named CObjectTarget slot98 methods005B19A0/GOG005B1A70 offer a wrapping460+0xA key then exit or request localfallback. Own-build producer/comparator/drain/type-method correspondence is qualified, not numeric-address transfer. Comparator equalkeys return-1/nozero: no stable/total/FIFO order. Hook mutation/current callback generations, arbitrary payload type/key domain/capacity, actualdraw/cadence/globalserialization/ownership/lastuse remainUNKNOWN. Details/current companion identity: `findings/boundaries/final_static_optional_record_admission.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical Phase8/through250 provenance remains intact.


## Final static campaign amendment — checkpoint253

**VERIFIED selected mechanics / STRONG_INFERENCE family:** shared child attachment setter0071EDE0 retains a parent-related token/index/localvectors;0071EEF0 requests indexedparent matrix006C3B80 and local-left/parent-right multiplication, storeschildposition thenoptionalorientation. Descriptor00461100 and004A1960/006A9F10 supplycreatedchildECX, notoriginalparent. Setup/earlyexits are nontransactional;210mask1 skipsorientation only;0xD48 isofferedlookupdiscriminator and comparedhelperRESULTs; reset uses signedcurrentcount/separatelyreloadedbacking. Dynamicclass/indexadmission/ownership/currentpose/packetfreshness/success/schedule/GOG remainUNKNOWN. Thisconnectsdescriptor/gameplaycreation toanimation/modelmatrix/world-transformuse; it doesnot establish streaming-subsystem ownership.

Evidence/qualification: `findings/boundaries/final_static_parent_matrix_attachment.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint256

**VERIFIED selected mechanics / STRONG_INFERENCE environment/thunder association:** CMapcloudcallbacks/time-compatibleweights/pulse connectsharedbanks toconcrete84-byte zero/seed packetproducer/nativeevent0E; nativeO44 andreceiver66FCcallbackchannels separate. OptionalchannelmathincludesHALF-luma0.5 andindependentpulse/fadeslotreloads; packet-tagcopy andflagmutationdomains differ. One66FCnonnulltest doesnotprotectlaterreload, noalias/weight/index/currentepoch proof. N_THUNDER1..9 andnumericCSound requestchain strengthenpulseassociationwithoutactual/synchronizedlightning/audio orsuccessfulrendering. Cleanup/null-basepathsconditionalstatic,notobservedfault/safe retirement. NoGOG/completepacket/schema/ownertheorem.

Evidence/qualification: `findings/subsystems/final_static_environment_packet_thunder_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint257

**VERIFIED mechanics / STRONG_INFERENCE family:** Numericresource/latch/variant/Gameflag producers feedavailabletypedCShotpayload/callback andscenequery mode0withSEPARATEflags9. TypedCNxRaycast signedcount/28-byte response fields canrefineengineface/triangle geometry thenconditionalnative1Cpacket targetdelivery; D3DX/platformimports aredependenciesnotrender-only/TLSsemantics. Rawactionnonentries/CShotcandidatequalificationpreserved. Upper-only/reloadedcount,truncatedSIGNEDface,independentrecordreads,conditionalpacket44overwrite,unclampedwrappingR90/unsigned20arrayguard,9keyedCAS+sharedfallback,currentcallback/nullablepayload/lifetime limits remainUNKNOWN. Noall-pathnearest/safeperthread/actualhit/FPSfault theorem.

Evidence/qualification: `findings/boundaries/final_static_player_shot_hybrid_query.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint258

**VERIFIED numericalmechanics / STRONG_INFERENCE typedfamily:** ThreeDartsreferences/pool→launchfollowtarget/nativepacket; namedboardevent1C usespacket-minusposition/inverseY/radius20sectorclassification withBOTHbull50×1. Independentcurrentminigamescore addsmultiplier×number thenmultipliesWHOLEaccumulator10×or5×perpositiveprefixmatch. Boardinteger470..478 anddartfloatoffsetsseparate,board470laterfeedbackindex. PackethelpercanmakeNOevent beforelaterindependentscore,currentretainer110canbeold,so nolivehit/classifier/scorecoherence. DrivergetterOUTPARAM andmatchingborrowedslotclear/camera5 areaccessrequests,notowner/free.16SIownpairedroles keep544GOGRAW_ONLY/nooldctors/externalcallee transfers explicit.

Evidence/qualification: `findings/subsystems/final_static_darts_board_driver_score.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
