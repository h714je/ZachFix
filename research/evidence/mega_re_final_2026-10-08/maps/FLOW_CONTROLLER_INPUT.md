# Controller / gamepad input flow

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; evidence boundary: validated checkpoint 238.** This map consumes C0123/C0124 and the input construction/lifetime companions without extending their success, ownership or runtime-cadence claims. Bounded existing-primary checks of the missing raw → action → aggregate edge are retained in `audit/phase8_synthesis_2026-10-07/CONTROLLER_EDGE_PRIMARY.json` (Steam 1,768 and GOG 1,732 original-ASM instructions matched to their own PE bytes). These counts describe checking scope, not newly understood functions.

## 1. End-to-end state flow

`V = VERIFIED selected static mechanism; SI = STRONG_INFERENCE; ? = UNKNOWN.` Arrows describe data or local call order, not successful acquisition or universal execution.

```text
WinMM numeric device IDs 0..6                         Win32 keyboard / cursor
            │ joyGetPosEx request [V]                         │ [V]
            ▼                                                ▼
 P = CInput+668: seven raw records, stride 36            window/focus/key state
     P[i]+0: selected-slot flag
     P[i]+1: joyGetPosEx result==0 flag
     P[i]+2: 34-byte JOYINFOEX-shaped storage
            │                                                │
            ├──── USEJOY byte 014810F0 + configured binding ────┤
            ▼                                                │
 00709C40 (Steam) / 00709BA0 (GOG): select/evaluate actions [V]
            ▼
 L[i] = temporary 0x6C-byte logical action record, seven slots [V]
            │ six aggregation helpers, on original CInput receiver [V]
            ▼
 A[i] = CInput+9AC+i*4C: activity, ORed mask, filtered axes,
        motion accumulators and persistent prior-sample history
            │ local lock; if pending count !=1 [V]
            ▼
 Q[i] = CInput+7E4+i*40: one pending seven-slot snapshot
            │ main tick calls COMMIT BEFORE its next POLL [V]
            ▼
 I[i] = CInput+C8+i*CC: live flags, held/rising/repeat/previous,
        filtered channels and per-bit repeat bookkeeping
            │ public getters; no fresh OS sampling here [V]
            ├── player / selected actor action tests
            ├── camera +6C/+70 and mode-specific transforms
            ├── menu / Native UI selection predicates
            └── vehicle control consumers

 Actual device topology / successful input / all consumer populations [?]
```

**The five record domains are different.** Raw `0x36`, action `0x6C`, aggregate `0x4C`, pending `0x40` and live `0xCC` must not be substituted for one another. The seven slots are records in one object, not seven separately owned input objects.

## 2. Acquisition, selection and enumeration boundary

| Edge / function | Receiver and input | Output / state change | Confidence and scope |
|---|---|---|---|
| `00409610` acquires input root | Cached `00BD9E10`; selected allocation request `0xBD0` | `TSiHolder<CInput>` construction through `00409370 → 00707D90`; returned pointer publication | **VERIFIED Steam** named zero-offset CInput base; offered holder size is not standalone CInput `sizeof` or allocation success [I1] |
| `00708B00 → 00709C40` | Original CInput; callee ECX is `I+0x668`; caller supplies temporary output `L` | Physical/evaluator pass and seven `0x6C` records | **VERIFIED Steam** raw ECX/call/store mechanics; GOG corresponding calls `00708AB0 → 00709BA0` independently checked [I1,I3] |
| Numeric device scan | `i=0..6` | `JOYINFOEX.dwSize=0x34`, flags=`0xFF`, raw result fields initially zero; `joyGetPosEx(i,&P[i]+2)` | **VERIFIED both selected bodies** [I3] |
| WinMM result | MMRESULT | `P[i]+1=1` only for return zero; otherwise byte zero | **VERIFIED** result testing, not a promise that every device exists or supplies usable data [I3] |
| Selected logical slot | First `P[i]+0==1`; active-window helper gate | Only selected slot gets evaluated and `L+0=1`; other output slots zeroed for `0x6C` bytes | **VERIFIED** selection mechanism. Initial flag producer joined by TBC010 below; later producer/persistence and complete device-selection policy remain **UNKNOWN** [I3] |
| Keyboard/mouse branch | `014810F0==0`, key/focus/cursor globals | `GetAsyncKeyState`, cursor deltas `014B0040/44`, selected recenter/clamp/scaling requests | **VERIFIED selected code**; window identity/current focus and successful cursor operations are separate unknowns [I3] |

This is **numeric ID probing**, not a demonstrated OS capability-enumeration database. The supplied imports and this path establish `joyGetPosEx`; they do not establish `joyGetNumDevs`, `joyGetDevCaps`, hot-plug generation tracking or a named device list. Raw acquisition occurs before the action evaluations; the selected-slot flag is not the same byte as the acquisition-result flag.

## 3. Bindings and the WinMM-shaped evaluation layer

**VERIFIED Steam numeric mechanics:** `006B1520(binding-key)` switches selected keys onto current DWORDs in `00BE58E8..00BE5944`. `006B1780` receives the copied 13-DWORD raw record plus a binding enum. Its default arm tests a shifted bit against the raw button word. Enums `0x29..0x2C` test POV intervals; `0x2D..0x38` select signed axis ranges. The checked ASM includes the x87 normalization operations omitted/misrepresented by the decompiler's compact `return 1` shape. Do not use the C prototype alone as a full ABI or return-value specification. [I3]

| Layer | Input representation | Transformation / result | Evidence state |
|---|---|---|---|
| Binding lookup `006B1520` | Abstract key used by individual action helper | Current numeric binding enum or unchanged default argument | **VERIFIED numeric selection**, friendly names and complete config admission **UNKNOWN** |
| Raw binding evaluator `006B1780` | WinMM-shaped DWORD axes/buttons/POV + enum | POV tests; axis threshold/scale/conversion; button-mask predicate | **VERIFIED selected branches**. Still tied to the raw field representation, even though it does not call WinMM |
| Per-action helpers `0070A580..0070B640` | Slot byte, raw receiver, USEJOY and binding state | Discrete or integer analog action values into `L` | **VERIFIED individual calls/stores**; not every action's gameplay meaning established |
| Aggregate/filter helpers `00709100..007097A0` | `L` fields, slot index, original `I` | Flags/mask/filtered axes/history in `A` | **VERIFIED arithmetic and stores**, provider-independent at this numeric boundary |
| Commit and public getters | Pending/live engine records | Held/rising/repeat/previous and engine float channels | **VERIFIED engine-state mechanics**, no WinMM invocation in these operations |

**Exact boundary:** OS/WinMM acquisition ends at `P` plus acquisition status. The binding-evaluation layer is *API-call independent but representation dependent*: it still interprets WinMM-shaped axes/POV/buttons and configured enums. The first fully engine-shaped interchange is `L`, the `0x6C` action record. From the six aggregate helpers through `Q`, commit, public getters and gameplay consumers, the selected logic is independent of the WinMM acquisition API. This describes existing separation, not a replacement design.

The seed names `configJ.cnf`/`configJex.cnf` and legacy X/Y, U/R, shared-Z layout. The map relies on the checked binding/evaluator operands, not on proving every file's loader, field, device model or active configuration. Those producer joins remain **UNKNOWN** unless individually reconstructed. [I2]

## 4. The 0x6C action record — concrete consumer-defined layout

`L` is caller-local; `00709C40` writes it and the six helpers consume it before returning. DWORD offsets below are verified stores/loads; semantic axis names are not inferred from offsets alone. [I3]

| Offset in L | Producer / interpretation established by consumer | Downstream state |
|---|---|---|
| `+00` | Selected/evaluated-record DWORD flag | `00709100` sets aggregate byte `I+9AC+i*4C` when nonzero |
| `+04..+38` | Fourteen action predicates, written by individual helpers | `00709130`: nonzero fields set digital bits 0..13; OR into `I+9E8+i*4C` |
| `+3C,+40` | Integer axis-like pair | Digital signs outside ±`0x32` set bits 16/17 and 14/15 respectively; `00709400` filters into `I+9B8/9BC` |
| `+44,+48` | Second integer axis-like pair | Signs outside ±`0x32` set bits 20/21 and 18/19; filtering into `I+9C0/9C4` |
| `+4C,+50` | Trigger-like integer channels | `>0x32` sets bits 22/23; `00709350` transforms `(value−50)/202`, clamps target/output domain to `[0,1]` |
| `+54,+58,+5C,+60` | Additional integer channels, selected keyboard-oriented helper outputs in inspected bodies | `007097A0` copies to aggregate `+9D0..9DC`; then pending/live channels |
| `+64,+68` | Angle-like helper outputs | Copied to `+9E0/9E4`; `+64` also participates in the absolute-difference accumulator |

Action helpers called for the first fourteen fields are, in offset order: `0070A7B0, A950, AA20, A880, A700, A580, AAF0, ABA0, A620, A690, AC50, AD00, ADD0, AEA0` (all with `0070` prefix, Steam). These are **producer identities**, not invented names such as “confirm”, “attack” or “jump”. The last fields are written through `0070B0B0/B180`, `0070AF50/B000`, `0070B250/B2A0`, repeated `0070B2F0`, and `0070B350/B640`. [I3]

### Filtering and history are persistent engine state

- **VERIFIED:** inner axis region `[-16,+16]` becomes zero; otherwise `(x−16)/109` or `(x+16)/109`. First pair uses a `[-1,+1]` clamp; second pair uses `[-2,+2]` in the checked calls. `0070A370` approaches the target by a supplied fixed step (`0.5` for axes, `0.4` for trigger-like channels), not by the frame delta. Thus these filter recurrences are per producer invocation; actual wall-time behavior needs the producer cadence. [I3]
- **VERIFIED:** `00709660` compares current `L+3C/+40` with persistent aggregate history `I+9EC/+9F0`, updates those history values, adds a magnitude to `I+9C8`, and uses `L+64` with history `+9F4` for an absolute-difference accumulator `+9CC`. These history cells are not the pending snapshot.
- **VERIFIED:** publication clears aggregate activity byte, digital mask and two accumulators; it does not clear every filtered axis or prior-sample field. Later samples can therefore merge behind an occupied pending slot.

## 5. Aggregate → pending → live

| Aggregate (slot stride 4C) | Pending (stride 40) | Live (stride CC) | Operation at commit |
|---|---|---|---|
| `9AC` byte / `9E8` mask | `+00/+04` | `C8/CC` | Replace |
| `9B0/9B4` | `+08/+0C` | `DC/E0` | Replace trigger-like floats |
| `9B8..9C4` | `+10..+1C` | `E4..F0` | Replace axis floats |
| `9C8/9CC` | `+20/+24` | `F4/F8` | **Add**, not replace |
| `9D0..9E4` | `+28..+3C` | `FC..110` | Replace additional channels |

**VERIFIED selected normal-initialized mechanism:** `+9A8!=1` allows publication; count increments and seven records are copied. `+9A4` supplies the pending index. Constructor-normal states and modulo-one processing support one pending `7*0x40` snapshot, not a general ring buffer. Do not infer support for arbitrary corrupt count/index states. [I1,I3]

Commit `00708350` first preserves each live held mask into previous, even before deciding whether a pending record is available. When pending count is nonzero it consumes the snapshot and decrements the count. Then it derives `rising = held & ~previous` and repeat-related masks. Public digital selection corresponds to held/rising/rising|repeat/previous, not four independent samples. Per-bit repeat state and the microsecond time anchor persist across calls. [I1,I2]

## 6. Temporal placement and consumers

```text
eligible application tick 00401A70 [V]
   ├─ 00401AB0: commit previously pending snapshot
   ├─ 00401AF0: conditional poll wrapper
   │     BC0==2 -> no producer
   │     otherwise -> producer; nonzero BC0 subsequently becomes 2
   └─ selected object-dispatch/control phases consume current live state
```

A sample produced during this selected synchronous pass becomes eligible for the *next* commit. **STRONG_INFERENCE conditional one-update staging latency**, not a measured millisecond delay or a universal once-per-render rule. Repeats, suppressed polling, alternate callers and actual runtime scheduling can change the relationship. The dormant-worker and producer-census claims in historical notes are **HYPOTHESIS/secondary observations in this workspace**, not newly admitted runtime proof: K0002/B0010 preserve the missing raw traces.

| Consumer | Concrete relation | Confidence / limits |
|---|---|---|
| Camera common look helper `00537660` | Reads input root; calls axis getter `00708A30` with selected slot/pair 1/component 0; writes same named camera root `+70` | **VERIFIED Steam** values/calls; yaw-like interpretation **SI** [I1] |
| Camera mode 0 `00538980` | Reads pair 1/component 1 and can write camera `+6C` | **VERIFIED Steam conditional**; not all modes [I1] |
| Public axis getter `00708A30` | `I+slot*CC+E4+4*(2*pair+component)`; component 1 negates | **VERIFIED selected valid indices**, no device sampling [I1] |
| Public digital getter `00708910` | `I+slot*CC+CC+4*selector`, masked by supplied DWORD; no clear/sample | **VERIFIED paired selected leaf**, GOG`007088C0`; final getter primary receipt |
| Public trigger getter `00708A00` | Reads `I+slot*CC+DC+4*channel` | **VERIFIED inspected expression**; no unbounded index domain [I4] |
| Player state / mode control | `00528F40` commits selected state and camera-mode mapping; the selected typed CPlayer genealogy is established by C0126 | **VERIFIED selected chain**, not the complete action-state population [I1] |
| Menu `00654490/00654A80` | Native task→static CMenu; input predicates advance numeric menu state; selected camera flag touch | **VERIFIED selected Steam seams**, full action naming/mode policy **UNKNOWN** [I5] |
| Vehicle | Selected state-87 steering target consumes delta-aware input control; camera mode 9 has its own route | **Qualified evidence**, see physics/frame-dependence and visibility maps; no blanket per-mode timing rule |

## 7. Lifetime, other input objects and explicit unknowns

The root holder's callback-registration and available cleanup interfaces are verified. Registration is not dispatch; a cleanup call and root clear are not proof of successful deallocation, stopped producers or last borrower. Later Phase 5/7 input-provider companions retain independent provider samples, dependency-counter limits and unknown generations. [I6]

**Do not conflate** CInput with `CRdInput`, formal `CActuator` setter requests, or PhysX character controllers. Actuator `+28/+2C` stores have no established physical executor/backend/units in the evidence boundary. PhysX's 128 controller cells and 20 scene slots are different domains again. [I7]

Remaining **UNKNOWN** edges: complete selected-slot producer; device/capability enumeration beyond numeric probing; every configuration producer/admitted enum; all logical-action friendly names; actual asynchronous-worker activation; runtime cadence and input latency; every gameplay consumer; finalizer occurrence/producer quiescence/ownership; operation-specific opposite-build behavior beyond the checked pairs.

## 8. Steam / GOG correspondence

| Node | Steam | GOG | Qualification |
|---|---|---|---|
| Core ctor | `00707D90` | `00707D40` | Historical pair; Steam named holder/type scope independently established; do not export all lifetime conclusions |
| Commit | `00708350` | `00708300` | **VERIFIED selected raw/copy mechanism**, GOG producer/commit checks in [I3] |
| Producer | `00708B00` | `00708AB0` | **VERIFIED corresponding six-helper/snapshot structure** |
| Acquisition/evaluation | `00709C40` | `00709BA0` | **VERIFIED selected raw/action mechanism**; note difference is `0xA0`, not the neighboring `0x50` delta |
| Poll wrapper | `007099D0` | `00709980` | Selected BC0 protocol; historical source plus GOG checked instructions |
| Filter | `00709400` | `007093B0` | Selected field/arithmetic correspondence checked in [I3] |
| Public digital / trigger / axes | `00708910` / `00708A00` / `00708A30` | `007088C0` / `007089B0` / `007089E0` | **VERIFIED selected getter operands** in final paired receipt; GOG full caller/provider/type genealogy remains **UNKNOWN** |

## Evidence anchors

- **[I1]** `findings/boundaries/input_camera_primary_roots.md:13–69`, correction `:71–73`; C0123/C0124/C0126. Primary locations and byte-check receipt are listed there.
- **[I2]** `inputs/knowledge/zachfix_research_2026-10-01/evidence/cinput_pipeline/README.md:25–116,153–189`; `input/README.md:5–35`. Secondary claims remain qualified by K0002/B0010.
- **[I3]** `audit/phase8_synthesis_2026-10-07/CONTROLLER_EDGE_PRIMARY.json`: entry-address → original ASM line/bytes/operands, independently matched PE. Steam C `487196–487355` acquisition/action writes; `486648–486940` producer/helpers; `428043–428232` binding; GOG C `384375–384534`, `383862–383920` corresponding nodes. New checks close only the specific missing transition, not full subsystem research.
- **[I4]** Steam `DP_decompiled.c:486598–486601`; I1 gives assembly-checked axis getter.
- **[I5]** `findings/boundaries/cmenu_organizing_task_use.md`; `findings/boundaries/cmenu_numeric_game_slot_commit.md`.
- **[I6]** `findings/boundaries/phase5_input_registered_finalizer_lifetime.md`; `phase5_input_provider_accounting_finalizers.md`; Phase7 A15/current reliance companions.
- **[I7]** `maps/PHASE7_CLOSEOUT_RELIANCE.md:93–105`; `findings/boundaries/phase7_architecture_a09_falsification.md` and `phase7_architecture_a15_falsification.md`.

Final narrow accessors: `audit/phase8_synthesis_2026-10-07/GETTER_PRESENT_EDGE_PRIMARY.json`. Separate frame-tail WinMM probe `joyGetPosEx(0,stack)` has no proved edge into the earlier logical producer; input getter reads do not clear action masks.

## Post-synthesis PGC005 — available Actuator accessors, not an executor

**VERIFIED available named CInput_Actuator:** constructor00735AB0/GOG007357C0 zeros seven stride8 two-DWORD records over[A+28,A+60); holder accounting+60 is separate. Protected readers00735BE0/00735C20 ↔007358F0/00735930 match the existing stores. Pair-address/lock/unlock interfaces00735C60/80/A0 ↔00735970/90/B0 are also present. No local index admission or hardware operation is established.

**STRONG_INFERENCE:** this is the available counterpart record-access ABI. **UNKNOWN:** actual rooted caller→reader/executor/backend, physical mapping/units, pair/provider epochs, cadence, success and last borrower. Do not draw an executed arrow from formal stores into uninvoked accessors. Seven pairs are not seven proved devices or equivalence to Input's other record domains/PhysX128/20.

GOG holder0070B930→named base007357C0, lazy0070B720 and wrapper0070A2A0 have independently checked own-build type/root ancestry; not a guessed common delta. Evidence: [focused access-limit report](../findings/boundaries/post_phase8_actuator_record_access_limit.md), `audit/post_phase8_closure_2026-10-07/ACTUATOR_PRIMARY.json` and `ACTUATOR_TYPE_PRIMARY.json`.


## Targeted bridge connections — TBC010

**VERIFIED selected static scope:** TypedconstructorI668 ->00709BB0 (GOGindependent00709B20) -> zeroonlytwoflagbytes at7stride36rows -> setrow0selectedbyte+0=1 -> knownfirstbyte==1 selection. Initialproducerconnected, laterpolicy/persistence/devicepresence/acquisitionbyte+1/API/cadence/allbindings remainUNKNOWN. CursorrequestconversionsarenotAPI-success receipts. Exactprimary/report: `findings/boundaries/targeted_bridge_input_initial_selected_slot.md`.


## Final static campaign amendment — checkpoint257

**VERIFIED mechanics / STRONG_INFERENCE family:** Numericresource/latch/variant/Gameflag producers feedavailabletypedCShotpayload/callback andscenequery mode0withSEPARATEflags9. TypedCNxRaycast signedcount/28-byte response fields canrefineengineface/triangle geometry thenconditionalnative1Cpacket targetdelivery; D3DX/platformimports aredependenciesnotrender-only/TLSsemantics. Rawactionnonentries/CShotcandidatequalificationpreserved. Upper-only/reloadedcount,truncatedSIGNEDface,independentrecordreads,conditionalpacket44overwrite,unclampedwrappingR90/unsigned20arrayguard,9keyedCAS+sharedfallback,currentcallback/nullablepayload/lifetime limits remainUNKNOWN. Noall-pathnearest/safeperthread/actualhit/FPSfault theorem.

Evidence/qualification: `findings/boundaries/final_static_player_shot_hybrid_query.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint258

**VERIFIED numericalmechanics / STRONG_INFERENCE typedfamily:** ThreeDartsreferences/pool→launchfollowtarget/nativepacket; namedboardevent1C usespacket-minusposition/inverseY/radius20sectorclassification withBOTHbull50×1. Independentcurrentminigamescore addsmultiplier×number thenmultipliesWHOLEaccumulator10×or5×perpositiveprefixmatch. Boardinteger470..478 anddartfloatoffsetsseparate,board470laterfeedbackindex. PackethelpercanmakeNOevent beforelaterindependentscore,currentretainer110canbeold,so nolivehit/classifier/scorecoherence. DrivergetterOUTPARAM andmatchingborrowedslotclear/camera5 areaccessrequests,notowner/free.16SIownpairedroles keep544GOGRAW_ONLY/nooldctors/externalcallee transfers explicit.

Evidence/qualification: `findings/subsystems/final_static_darts_board_driver_score.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.


## Final static campaign amendment — checkpoint260

**VERIFIED mechanics / STRONG_INFERENCE controlpresentationfamily:** ActualCGame-relative8DWORDmaskbank mapscapturedInputqueries andcustomESIWORDtoken/EDIcode/EBXflagsmessageformatting. Recognizedmasksdropunrelatedbits, composite/unmatchedresultsleavepartialtokenpolicy, earlyspecial/04000000returnsbypassfinal3C53. Stageddirty/default/copy producersarefunction/mode-scoped,sparse52bytedefaultsnotfullreset/schema,204/208maymaskorbyte. RepeatedGame/capturedInput/opaqueapply/glyphrefsnotcoherentsnapshot/API/persist/drawsuccess.3SIownGOGlocalroles/datamaps doNOTextendproducercallees ormapnumeric55970. FSC002newown23byteOR helper issupport-onlyeffectqualification,notnewfamily/activationproof.

Evidence/qualification: `findings/boundaries/final_static_control_bank_message_tokens.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
