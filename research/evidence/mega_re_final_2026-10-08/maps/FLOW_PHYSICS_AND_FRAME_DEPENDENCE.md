# PhysX and frame-rate-dependent state flows

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint 238 boundary.** Mechanically established operands/calls/stores are **VERIFIED**. PhysX virtual-method names are **STRONG_INFERENCE** from the documented 2.8.1 ABI and matching protocols, not literal symbols in the game. API success, solver debt, live scene identity, cadence and safe retirement remain **UNKNOWN**. A request is never called a successful result merely because the dispatcher advances.

## 1. Time originates in the application, not in CTimer

```text
QPC frequency → cached 014AFFF0 [V]
QPC count / frequency → seconds-shaped application clock [V]
      │ Steam00701040 / GOG00700FA0
      ▼
eligible idle iteration: (now−previousEligibleTime)*60, upper clamp8 [V]
      │ selected rounding / fractional accumulation policy
      ▼
014AFFD4 and 014AFFE0 = published gameplay scalar [V]
      ├─ object dispatcher / camera / animation / effects
      ├─ CTimer accumulation and CFunc countdown, separate consumers
      └─ CPhysicsCore ordinary producer, scalar transported unchanged [V]
```

**VERIFIED:** central application producers are Steam `00700650`, GOG `00700670`. Their target literal is `60.0`; `014AFFE0` is not dimensionally elapsed seconds. The selected raw clock returns count/frequency; downstream clamp, selector-dependent rounding and accumulator policy prevent the shorthand `gameDelta60/60` from being an exact all-state wall-time theorem. The first eligible iteration initializes its local baseline. See [P1] and the display/timing map.

The same scalar is consumed by independently acquired `CTimer` (`00470C20` Steam / `00470D20` GOG), whose accumulator/calendar-compatible fields are not the QPC provider. CFunc/fixed-word control is separate again. **DISPROVEN:** treating every acquired Timer/Core receiver or mutex bracket as one global pause/clock authority. [P7]

## 2. Organizing root, SDK and storage

| State/object | Origin | Consumer / transition | Lifetime boundary |
|---|---|---|---|
| `00BDA0C8` cached root, selected offered size `0x67EDC` | Steam `0040E470 → 0040E340`; `0040E4F0` is a JMP thunk | Named `CSingleton<CPhysicsCore>` / zero-offset CPhysicsCore; startup and dispatcher consumers | **VERIFIED construction/publication mechanism**; actual allocation, uniqueness, final deletion **UNKNOWN** [P2] |
| Shared SDK `01493FA8`, core `+11C` alias | `006EC450`, `NxCreatePhysicsSDK(0x02080100,...)` request | 20 scene-like pointer slots and controller support | **VERIFIED pointer transport**; successful SDK creation, actual loaded version and exclusive ownership **UNKNOWN** |
| Scene-like slots `core+50..9C` | Initialization clears 20 DWORD cells | Producer enumerates pointer/index records; selected SDK cleanup calls and slot clears | **SI scene role**; no complete scene-population/ownership theorem |
| Core event `+48` / private heap `+67ED8` | Named PhysicsEvent / HeapCreate requests in ctor | Producer wait; available destructor signaling/close/heap-destroy | **VERIFIED requests**; valid handles and global synchronization **UNKNOWN** |
| Controller support `+67CD4` | Separate offered `0x18` wrapper during SDK setup | Selected controller cleanup/manager release | Distinct from scene array and per-player controller indices |
| Four `CPhysicsThread` contexts | Static root `00BDA010`, stride `0x2C`; ctor `0040BA40` | Packer, synchronous sweeps and available worker entry | Named static-array construction/finalizer registration; actual execution/quiescence **UNKNOWN** [P3] |

## 3. Ordinary submit flow

```text
Steam dispatcher phase14
    → acquired CPhysicsCore006EB3C0(014AFFE0)
    → 006EAB60: scalar/world gates, scene+ordinal staging, core-event wait [V]
    → selected scene 006EACE0(scene, scalar, ordinal) [V]
         ├─ scene eligibility predicate
         ├─ choose numeric capacity1/2/3/4 from scalar thresholds1.5/2.5/3.5
         ├─ 006EAE20 → current scene vslot148 timing request [V / setTiming SI]
         └─ 0040B7B0 → 0040BCE0 → 0040BC50: six-DWORD record append [V]
                                     ▼
                    same CPhysicsThread +20 storage / +28 count
                                     │
                     available worker0040BAF0 [V mechanism; occurrence ?]
                         nonnull record+0
                         elapsed=min_ordered(record+4,0.0666666701)
                         current scene slots230→144→238
                         count clear; signal request
```

**Important join established:** packer offers **context+1C** to the appender; its array+4/+C are the same physical context+20/+28 read by the worker. The older suggestion of a separate queued→active record transfer is not required for this selected route. **UNKNOWN:** actual producer/worker ordering, concurrent safety and stable backing/scene generations. [P3]

The game-owned timing configuration occurs before enqueue in the selected returning path. The copied record `+8` is not the live SDK `maxIter` authority. The incoming gameplay scalar reaches record `+4` without conversion to seconds in the selected producer, while the worker applies the literal ~`1/15` upper bound. **SI:** the ABI interprets that transported scalar as PhysX elapsed seconds. This is the exact dimensional mismatch path, not a conclusion that every runtime submission advances 60× or that every scene runs. [P1,P3]

### Timing policy is selected, not universal

| Path | Requested timestep / capacity | Qualification |
|---|---|---|
| Ordinary configurator arm | `0x3C888889` ≈1/60, chosen capacity1..4, method0 | **VERIFIED literals/call**; fixed-step/setTiming names **SI** |
| Special ordinal0 record | ≈1/60, table capacity0 with incoming0 promoted to1 | **VERIFIED selected table/branch** |
| Special ordinals1..19 | `0x3D088889` ≈1/30, table capacity1, method0 | Not all secondary scenes in every mode |
| Synchronous catch-up | Saved timing; temporary ~0.05/capacity20; fetch and optional restore requests | Separate context transaction, not ordinary configuration |

Repeated `setTiming` effects on PhysX's internal fractional remainder/debt remain **UNKNOWN** (B0001). Historical low-FPS/high-capacity runtime results are retained as secondary reports, not reproducible traces supplied here. No solver repair is proposed.

## 4. Context and record fields

| CPhysicsThread offset | Established role | Evidence state |
|---|---|---|
| `+00` | Named vptr, CThfunc zero-offset base; slot+4→`0040BAF0` | **VERIFIED** |
| `+08,+0C,+10,+14` | Thread/event/event/mutex handle locations on available generic start route | **VERIFIED request/storage geometry**, actual creation **UNKNOWN** |
| `+18` byte | Ctor1; loop gate; destructor0 | **VERIFIED**, not a complete stop/join protocol |
| `+19` byte | Synchronous/destructor local protocol flag | Not a successful-completion certificate |
| `+1C` | `CNArray<CPhysicsThread::SCENE>` interface | **VERIFIED RTTI** |
| `+20/+24/+28` | Storage/capacity/count; offered eight `0x18` records | **VERIFIED**; appender has no selected capacity/growth test |

| SCENE record offset | Producer → consumer | Qualification |
|---|---|---|
| `+00` | Scene-like pointer → current virtual-call receiver | Null records skipped; same value/generation across calls **UNKNOWN** |
| `+04` | Incoming gameplay float → worker ordered clamp → virtual230 argument | **VERIFIED** value path; elapsed-seconds semantics **SI** |
| `+08` | Opaque forwarded argument | Full meaning **UNKNOWN**, not live timing authority |
| `+0C` | Staged slot ordinal byte, zero-extended into DWORD | Alternate consumers test signed positive; not interchangeable with+8 |
| `+10/+14` | Reserved local-area words copied/compared; selected synchronous get/restore operands | Local initialization/complete timing schema **UNKNOWN** |

`0040B7F0` compares all six DWORDs; `0040BC50` appends only when no exact match is found. That establishes a local duplicate test, not unique scene membership or a sufficient capacity under all admitted loads. [P3]

## 5. Synchronous half-transactions and event continuation

```text
phase14
  ordinary producer request
  0040B750 → per-context0040B850
     context19/count28 gates
     current scene slots14C /148 /230 /144
     write context19=1 without checking operation success
        ↓
write dispatcher phase7
  0040B780 → per-context0040B940
     current scene slot238(1,1,0)
     optional current timing restore at148
     clear context count28 and flag19
        ↓ no operation-result success branch
write phase8
  eligible original object's virtual1C(event6)
```

**VERIFIED both exact Steam/GOG windows:** first half precedes the phase7 write; second half follows it; phase8 follows the returning sweep without a fetch-result success test. GOG corresponding sweeps are `0040B720/0040B750`, per-context `0040B820/0040B910`; dispatcher window `006C6770..006C67FE`. [P4]

**DISPROVEN historical interpretation:** both synchronous helpers are under phase7; state5 is the primary submit phase; state11 is the primary fetch phase. State11's `006EB400` is a separate selected physics-facing continuation. [P2,P4]

**UNKNOWN:** whether fetched scenes equal submitted scenes, successful SDK execution, actual event6 delivery, coherent context populations and global schedule. Phase8's numeric write and context clear do not supply those premises. A zero-count context can clear without making a scene request; an untested non-success return can still reach the continuation. These are static countermodels, not observed game failures.

## 6. Physics state consumers

| Boundary | State carried | What is established | What is not |
|---|---|---|---|
| Object event6 after phase8 | Numeric event and current object receiver | Selected guarded virtual1C invocation after synchronous second-half request | Success-authorized physics results or one event per substep |
| CCT/root-motion path | Caller displacement → common controller wrapper `006F9DC0` Steam / seed GOG`006F9DD0` | Selected game→controller virtual request; immediate-move semantics **SI ABI** | Live caller population, cadence and ordering against scene work |
| Player support teardown | Player `94C/950` guards and independently reloaded index → current controller-compatible provider | Request and normal-return core cell clear; separate typed consumer/provider mechanics | Child ownership, matched epoch, completed release/quiescence |
| Car wheel path `00555B50/00555C20` | Delta-scaled motor/brake/steering values → wheel interfaces | Selected persistent-property setter seam from accepted vehicle evidence | Universal per-step scaling or exclusive actor ownership |
| Direct chassis `005577D0/005578A0` | Pose/velocity read/modify/write requests | Separate vehicle actor-state path, selected branches | Exact cadence and whether it overwrites recently fetched state |
| Prop event6 example | Object-local phase → one force request and later cleanup | Historical GOG`0055F6A0` bounded state machine | Continuous force stream or all event6 consumers |

**128 controller cells are not 20 scene slots.** Phase7 A09 independently distinguishes those domains and current controller-compatible virtual consumers; no provider identity is borrowed from equal addresses or field similarity. [P5]

## 7. Other FPS-dependent paths — different persistence contracts

| System | Origin → state → transformation → consumer | Dependence established / limit |
|---|---|---|
| Input filters | Action integer → aggregate float → fixed approach0.5 or0.4 per producer call → live getters | **VERIFIED per-call recurrence**; cadence converts it into wall-time behavior only conditionally. See controller map |
| Fixed-effect families | `014AFFE0` normally; effect+1C0 bit2 overrides to1 → `0071CC60` → local effect-scale/part simulation | **VERIFIED Steam/GOG selected override**; ordinary effects are not all fixed. `+1B4 bit2` completion gate is separate [P1,P6] |
| Effect local scale | Base delta × effect+1BC → part updates | **VERIFIED selected field consumption**; full XWP grammar/emission/event/lifetime behavior **UNKNOWN** |
| Vehicle wheel properties | Gameplay scalar × requested motor/brake values → persistent SDK properties | **Qualified selected flow**; property persistence differs from per-call displacement |
| Vehicle state87 steering | Desired steering → car+4E0 slew using gameplay scalar | Delta-aware selected recurrence; older fixed-degree branch is distinct, not a universal vehicle claim |
| CCT | Per-call displacement → immediate move/hit-callback request | Scene-solver timing does not establish CCT caller cadence |
| Camera modes | Live input → mode-specific target/increment/integration | No global right-stick delta rule. Modes10/11 rate behavior **UNKNOWN**; mode9 has separate signed-deadzone route |
| Fade / Timer / CFunc | Shared scalar → local accumulators/counters | Selected delta-aware arithmetic, not the same unit/policy as scene elapsed; fixed-word gates do not freeze all writers |
| Animation/packet | Delta/state/resource evaluation → matrices → conditional packet copy | Repeated render rate does not prove latest-pose repopulation; cached packet can bypass |

For an actually fixed-per-call state increment, the mathematical wall-time rate is increment×call frequency. The engine's *live call frequency* is not reconstructed merely by knowing a renderer FPS. This distinction explains the dependence path without inventing measured behavior or recommending changes.

## 8. Activation, cleanup and unknown seams

Available physics activation `0040B630` walks the four contexts and calls generic `00712A20`; CreateThread's argument is the same context, entry`00712D30` restores ECX and invokes slot+4. **VERIFIED available chain; actual incoming activation UNKNOWN.** This differs from CLoadThread's positively invoked startup route. [P3]

`0040B700` nulls the first matching scene pointer in each context, not all duplicates and not the entire record/count. The worker's null test supplies a concrete downstream skip. Context destructor clears run byte, signals, then requests storage deallocation and clears fields; no demonstrated join precedes the free. Core session cleanup calls dependent/SDK release interfaces and clears scene/controller cells; available root destructor additionally closes event/destroys heap. **UNKNOWN:** actual finalizers, API outcomes, active borrowers, coherent generations and final SDK/global/cache destruction. [P2,P3,P5]

## Evidence anchors

- **[P1]** `audit/phase8_synthesis_2026-10-07/TIMING_EDGE_PRIMARY.json`: own-PE matched original ASM for selected clocks/timing/effect/Present nodes; Steam C`479957–480143`, `480264–480278`, `466172–466313`; GOG entry `00700670` and its original-ASM line anchors in [P1]; `inputs/knowledge/zachfix_research_2026-10-01/physx/README.md:9–89` is ABI/policy seed, not runtime authority.
- **[P2]** `findings/boundaries/cphysicscore_lifecycle_root.md:9–60`, C0121/C0122; original ASM/PE receipts there.
- **[P3]** `findings/boundaries/physics_context_vector_activation.md:11–69`, C0149–C0151; stack-coordinate and uninitialized-tail qualifications preserved.
- **[P4]** `findings/boundaries/phase7_architecture_a07_falsification.md:15–59`; `audit/phase7_architecture_2026-10-07/A07V3_PRIMARY.json`.
- **[P5]** `findings/boundaries/phase7_architecture_a09_falsification.md`; `phase5_player_physics_dependency_teardown.md`; `phase5_physics_scene_controller_release.md`.
- **[P6]** Steam C`498495–498518`, part path`498810–498882`, same-node checked ASM [P1]; `inputs/knowledge/zachfix_research_2026-10-01/engine/effects.md:389–478` as comparative seed. No new Xbox semantic promotion.
- **[P7]** `findings/boundaries/phase7_timer_fixed_word_falsification.md`, checkpoint235; `maps/PHASE7_CLOSEOUT_RELIANCE.md:107–132`.


## Targeted bridge connections — TBC004/TBC006

**VERIFIED selected static scope** (semantic/epoch ceilings retained): TBC004 joins explicit E=M1B4 same-model delta*M1F8 update to parsedXCA arrays, signedcursor scalar evaluation andsnapshotblend. This is an additional concrete resource-to-state delta consumer, not a newtimeunit/cadence or fullgrammar premise. TBC006 joins selected CEffectevent1 2000comparison/1B4bit4 writer to sameE inheritedperpart update earlyreturn. That gate is distinct from1C0bit2 fixeddelta override and1B4bit2 completion. Actualinput/admission/output/epoch/lifetime/eventfrequency remain UNKNOWN. Details in the targeted XCA/effect reports under findings/boundaries.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** Availableloading-worker presentationloop TBC012 shares00886FF8 with mainrepeatcontrol atofferedroot. Itdoesnotestablish actualthreadactivation, repeatedphysicswork/populations, liveupdate:Present ratio, clockunits orfixedcadence. Its1/30accumulatorcomparison isnotarendergate; Sleep16 isonlyarequest. Do notderive aphysicssubstep orsafeparallelD3D theorem fromthisadditionalcontrolorigin. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.


## Final static campaign amendment — checkpoint255

**VERIFIED selected mechanics / STRONG_INFERENCE family:** World progress/prefetch feeds multiple XPM forms and model-key NxStream imports;97initialSteamXPM/XMD ID pairs plusTREEPHYterminal are executable catalog witnesses. Modelreader sends cachedpointer todescriptorB8kind4 and physicallynamedNxTriangleMeshShapeDesc6C; separate region/part cachekind5 converges ontriangle-shapepreparation. Writerboolword isnotbatchID, FULL32batchkeys differfromsignedLOW16readerkeys, cursorlengthnotstreamlimit, stagezeroSETZnotSDKsuccess, pre-callflag/separatecurrentSDKloads notcompletedvalidmesh. Unsignedguard/physicalcasearms doNOTrepairbaselineCFG.13SIpairedmechanisms doNOTextendGOGshape/corpus/runtime/lifetime ownership.

Evidence/qualification: `findings/subsystems/final_static_xpm_model_physics_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint257

**VERIFIED mechanics / STRONG_INFERENCE family:** Numericresource/latch/variant/Gameflag producers feedavailabletypedCShotpayload/callback andscenequery mode0withSEPARATEflags9. TypedCNxRaycast signedcount/28-byte response fields canrefineengineface/triangle geometry thenconditionalnative1Cpacket targetdelivery; D3DX/platformimports aredependenciesnotrender-only/TLSsemantics. Rawactionnonentries/CShotcandidatequalificationpreserved. Upper-only/reloadedcount,truncatedSIGNEDface,independentrecordreads,conditionalpacket44overwrite,unclampedwrappingR90/unsigned20arrayguard,9keyedCAS+sharedfallback,currentcallback/nullablepayload/lifetime limits remainUNKNOWN. Noall-pathnearest/safeperthread/actualhit/FPSfault theorem.

Evidence/qualification: `findings/boundaries/final_static_player_shot_hybrid_query.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint258

**VERIFIED numericalmechanics / STRONG_INFERENCE typedfamily:** ThreeDartsreferences/pool→launchfollowtarget/nativepacket; namedboardevent1C usespacket-minusposition/inverseY/radius20sectorclassification withBOTHbull50×1. Independentcurrentminigamescore addsmultiplier×number thenmultipliesWHOLEaccumulator10×or5×perpositiveprefixmatch. Boardinteger470..478 anddartfloatoffsetsseparate,board470laterfeedbackindex. PackethelpercanmakeNOevent beforelaterindependentscore,currentretainer110canbeold,so nolivehit/classifier/scorecoherence. DrivergetterOUTPARAM andmatchingborrowedslotclear/camera5 areaccessrequests,notowner/free.16SIownpairedroles keep544GOGRAW_ONLY/nooldctors/externalcallee transfers explicit.

Evidence/qualification: `findings/subsystems/final_static_darts_board_driver_score.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
