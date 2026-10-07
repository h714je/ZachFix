# Audio — game requests, identities, voices, backend and retirement

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint238 plus bounded necessary speaker/completion edge checks.** Primary receipts: `AUDIO_EDGE_PRIMARY.json` (Steam1,780 original-ASM/own-PE matches) and `AUDIO_COMPLETION_EDGE_PRIMARY.json` (63). Existing token/node/instance/backend/lifetime qualifications remain. Audio is not substituted by movie/DirectShow audio; these are independent game-audio roots.

## 1. Supported game-to-backend spine

```text
actor/event/native request + numeric sound selector / position / controls
 → CSound coordination + parameter resources / name tables
 → selected PRM row → retained named-container inline addressP
 → request/control block
 → CSdMain128-row allocation + tokenT
 → CSdCore32-row selectionK + separate statusQ
 → P fields select bank/name/control byte
 → current shared bank interface → cue index / prepare-or-play request
 → output cue/interface in core rowE14
 → spatial setup / parameters / play-shaped request

installed task event1
 → CSound maintenance → Core work / Main delta-fed record update
 → cue status query / conditional cleanup
 → current callback may mark Main rowA0=FFFFFFFF
 → backend stop/destroy-shaped requests / cell clear / status operations

UNKNOWN: named-node producer/contents, concrete cue success, retained-token epochs,
         complete looping grammar, live cadence and final borrower/cleanup completion
```

Most concrete request/record mechanics are **VERIFIED Steam** [A1,A2]. XACT/voice API interpretations are **STRONG_INFERENCE** where based on matching bank paths, COM acquisition and ABI-shaped slots. A returned token or nonnegative K is not proof of audible playback.

## 2. Separate organizing objects and storage

| Object / storage | Origin | Stored state / consumers | Lifetime qualification |
|---|---|---|---|
| CSound cache`0138A6E0`, offered68A0 | Getter`004183D0`, ctor`0046A7B0`, named table`00773164` | 256 records from+C, stride54; parameter/name/control arrays; request coordinator`0046AD80` and maintenance`0046F360` | Cached construction/use; actual final free/current instance **UNKNOWN** |
| CSdMain cache`00BDBCC0`, offered2838 | Getter`00427700`, ctor`0071FDD0` | 128 rows`+30+i*50`; cursor28, sequence2C; update`00720980` | Row token protocol, not owner of every bank/voice |
| CSdCore cache`0138A6E4`, offered50C | Steam`0046F9D0`, GOG`0046FAD0`; ctor`0072BA00/0072B710` | 32 rows`+4+i*24`; cursor484, callback488, indexed counts48C..508 | Separate from static Core; actual generation/last-use **UNKNOWN** |
| Static CSdCore`014B0400` | Independently typed initializer/static constructor | Same constructor family and absolute backend globals | Not lazy-cache allocation or exclusively owned shared state |
| Backend/interface cell`014B01E4` | CoInitializeEx; registry/CoCreateInstance request | Engine init/work, bank creation, mix-format and variable requests | Shared absolute cell; repeated sample≠same interface generation |
| Sound-bank cells`014B01FC...` | Constructor .xsb file/resource requests | Bank index→current bank virtual operations | Bank index/name content and complete ownership **UNKNOWN** |
| Wave-bank/mapping/file state | InMemory.xwb mapping;12 streaming .xwb file slots; other banks | Backend CreateWaveBank/streaming-bank-shaped interfaces | Dedicated audio loading, not automatically CRdData descriptors |

The named Main/Core/CSound vtables are not the foreign backend/cue interfaces. Core globals adjacent in memory are not all fields of the Core receiver. [A1,A5]

## 3. Audio resource identity and request inputs

**VERIFIED selected static asset chain** [A2]:

- ResourceID`3A0B` corresponds to `UPDATA/PRM/SND_SE.PRM`; selected consumer computes its table view from the signed header and remainder formula, not a generic guessed alignment.
- Supplied asset size`0x3068`, tableoffset`0x14`, count`0x407`; selected request entry`0x4E` atfile`0x3BC` provides sound/name selector64 decimal (`0x40`) and additional WORD controls.
- ResourceID`3A05` corresponds to `UPDATA/PRM/SE_LIST.PRM`; selected row64 decimal (`0x40`), fileoffset`0x418`, contains `plse066`. Formatting/uppercase yields lookup key`PLSE066.PCM`.
- Named lookup writes an output wrapper; the retained P is an **inline node address `node+0x20`**, not a dereferenced owning PCM payload pointer. CSound retains P in the selected name-table cell.

**UNKNOWN concrete selected episode:** actual populated PLSE066 nodeP and current bank/control/name values. **VERIFIED post-synthesis producer:** available typed CAudio_Data FILEITEM writer suppliesP4 from offered bankselector, P14 from two metadata DWORDs and initialP2C from the stored key-name pointer. P2C/node18 remain distinct cells; initial producer correspondence is not all-epoch equality. Lookup key is not thereby a cue name, wave-bank sample or owning PCM payload; static PRM bytes do not certify resident backend voice data.

Selected untyped outer`005BB0D0` supplies position-like+58 pointer, entry4E, scalarcontrols0.5/0/100/150 andflags0, then marks outer+458 after normal request return. That marker is not checked playback success. CSound request adapter scales/selects controls and can reject withFFFFFFFF; the continuing block contains uninitialized padding/tail words in the inspected adapter before copy. No completely initialized universal request schema is claimed.

## 4. Request → main token → core/status → foreign interface

| Transition / function | Receiver / inputs | Stored output or side effect | Confidence / qualification |
|---|---|---|---|
| CSound`0046AD80` | Index, P from selected name table, controls/position/flags | Unsigned index≤0x406 (1030 decimal); computed row; selected short→name-table pointer | **VERIFIED gates**, computed-address test≠table readiness |
| Adapter`0046AA90` | Saved CSound/flags and descriptor/control values | Stack request word0=1, word2=selector0x40, DWORD4=P, float8=1; controlsB | Exact block transport, not complete typed policy |
| Main`007203A0` | Acquired Main, request/control block | Copies28-byteB; appends localA/P/flag-derived byte for Core | Local request is not an owned retained stack object |
| Free-row selector`007208E0` | Main128-row cursor scan | RowA with sentinelFFFFFFFF or0 on exhaustion | Producer bounds do not constrain all later token inputs |
| Token generator`00720890` | Old sequence and selected rowi | `T=(oldSequence<<8)|i`; publishA0; advance/wrap sequence | **VERIFIED protocol**, not resource ID or successful voice |
| Core request`0072CFE0` | Current Core; outputK address and composed blockC | E row`Core+4+24*K`; E0=statusQ, E4=A, E8=request marker, EC=P4-derived bank index, E10=P14-derived byte | **VERIFIED selected fields**, all live identity/value validity **UNKNOWN** |
| Main retention | Selected signedK≥0 | A18=K, A1C flags, A1E marker, A20 selector0x40, A14=1; controltailA28..4F | T, K, Q are distinct domains; copied tail does not include local appendedP field |
| Name/status retention | Same selected P and nameN=`[P+2C]` | Selected statusrecord+10=P, +34=N | Pointer retention≠exclusive node/name ownership |
| Bank/cue operation | `[014B01FC+4*bankIndex]`, transformed N | Bank slot0 index; AX!=FFFF→slotC or10 request intoE14 | **VERIFIED calls/arguments**, Prepare/Play interpretation **SI** |
| Prepared branch | E8/request byte nonzero | Spatial calculate/apply; currentE14 slot0 request | **SI cuePlay**; operation returns/current interface success **UNKNOWN** |

The bank null-looking guard in the selected request code tests a literal bank-array address, not every selected bank pointer. Bank-operation result is not always inspected before E14 use. Name-null/indexFFFF can skip backend work while nonnegative K/token publication remains possible. [A2]

## 5. Token identity, reuse and lookup qualification

Phase7 A05 independently checks Main constructor/generator/free-selector/resolver in both builds:

- Constructor cursor28=0, sequence2C=1, all128 row sentinelsFFFFFFFF.
- Normal sequence values1..7FFFFE; after increment unsigned≥7FFFFF, reset1.
- Resolver rejects fullFFFFFFFF, then uses `h&FF`, stride50, sentinel and full-ID comparison; no local `index<128` guard precedes its row reads.
- Same T can recur after wrap or in a newly constructed Main instance. Full-ID equality is not permanent allocation/epoch/instance identity.

Paired local primitives: generatorSteam`00720890`/GOG`007205A0`; free-selector`007208E0/007205F0`; resolver`00720830/00720540`; ctor`0071FDD0/0071FAE0`. These establish bounded structural correspondence, not whole GOG coordinator/backend/schedule parity. [A3]

A separate CSound256-row token/record adapter, used by retained loop-compatible clients, indexes lowbyte/full value atstride54. Its token domain must not be merged with MainT, CoreK or statusQ merely because each is numeric.

## 6. Playback parameters and per-update transformations

| State / producer | Transform | Consumer | Scope |
|---|---|---|---|
| PRM WORD/scalar controls and caller0.5/0/100/150 | Descriptor fallback/scale and opaque validation | Request/control blocks | Exact transport **VERIFIED**; friendly volume/radius/unit taxonomy partly **UNKNOWN** |
| Main row fade-compatible fields | Delta adds to local accumulator; interpolation; numeric1000/4000 flags | A28 scalar/control temporary values; optional cleanup request | **VERIFIED selected recurrence**, fade policy **SI** |
| Main+2830/+2834 scalars | Chosen by row byte; multiply current scalar | Core parameter update | Separate scalar banks; not every audio master/category meaning inferred |
| Position controlsC14/18/1C | Copy toCoreE18/1C/20 and emitter globals`014B0370/74/78` | X3DAudio calculation | Position-like semantics **SI API/structure** |
| Current listener/transform provider | Selected helpers fill listener position/orientation globals | X3DAudioCalculate | Complete camera/listener ancestry/current generation **UNKNOWN** |
| Initial spatial DSP output | Matrix, distance, Doppler and orientation fields | `0072DCB0`: cue matrix request and variables`Distance`, `DopplerPitchScalar`, `OrientationAngle` | **VERIFIED string/field/argument use** |
| Later Core update`0072CB00` | Distance-based factor/clamp; construct explicit2×2 diagonal scalar matrix; selected`pitch` variable | Current cue slot10/14/18 requests | Distinct from initial general-channel DSP application |

No one uniform audio delta/unit domain is assumed. Numeric pitch/Distance strings are direct evidence; successful variable lookup/application and authored bank interpretation remain **UNKNOWN**.

## 7. Where speaker layout enters the flow

```text
current shared backend014B01E4
  ├─ slot14 final-mix-format-shaped request → local format buffer
  │      WORD+2 → DSP destination channel count014B02FC
  │
  └─0072DA80
       variable lookup/read: "SpeedOfSound"
       slot14 mix-format-shaped request → local buffer
       DWORD+14 → X3DAudioInitialize speaker-mask argument
       speed scalar → same imported call
       output instance handle →014B01E8

014B01E8 opaque HANDLE bytes + listener014B0320 + emitter014B0354
 + DSP014B02F0 → X3DAudioCalculate(flags0x61)
```

**VERIFIED operands/imports** [A4]; WAVEFORMATEXTENSIBLE/X3DAudio field/API interpretation **SI**. **`014B01E8 is the X3DAudio instance/handle storage, not the speaker mask.** The mask is a local value taken from the backend-returned format structure. `014B02FC` stores destination channel count; sourcecount`014B02F8=2`; emitter channelcount`014B0390=2`; coefficient buffer`014B03B8` is64bytes (16 floats) in selected initialization.

`0072DB80` fills default emitter azimuth arrays for channel counts2/3/4/5/6/8 when needed, and requests named X3DAudioCalculate with current handle/listener/emitter/DSP. Selected initial `0072DCB0` passes DSPsource/destination counts and matrix pointer to cue slot10, then selected named variables.

**Important independent later path:** `0072CB00` invokes spatial calculation but its selected subsequent cue matrix request uses literal source/destination counts2,2 and a local diagonal matrix. It also reads calculated emitter distance for a factor. Thus speaker-aware initialization/calculation and explicit later2×2 application are different state edges. Whether that produces a particular speaker-layout fault under the loaded backend is **UNKNOWN**; no fix or runtime failure is asserted.

The constructor count query and initializer mask query are separate backend calls; coherent same-format/epoch identity is not established. Actual backend mix-format result/currentness, device layout, channel-mask validity, every coefficient consumer and effective audible mapping remain **UNKNOWN**.

## 8. Where looping state is known, and where it is not

| Layer | Established state path | Evidence state / semantic ceiling |
|---|---|---|
| Retained client/component | Loop-compatible inline L stores numeric q at+4; q0/q1 observations→current CSound record operation; normal-return L4=−1 | **VERIFIED geometry/request protocol**; embeddedCSoundLoop naming **SI at validated companion scope** |
| Named metadata byte | New typed00701CB0 producer setsP14 from nonzero-OR of two metadata DWORDs; accessor→CoreE10; zero/nonzero selects distinct bank bookkeeping paths | **VERIFIED selected producer/byte path**; authored loop meaning remains **HYPOTHESIS**, selected row/current/schema unknown |
| Core request markerE8 | Selects prepare/spatial/play-shaped versus direct-play-shaped request branch | **VERIFIED control**, not a proved loop-count parameter |
| Authored sound/wave banks | .xsb/.xwb resources enter backend creation | Backend cue/wave looping metadata/count semantics **UNKNOWN**; no game-side parser/loop setter established here |
| Repeated game requests | Installed callbacks and retained client tokens can request continuing audio operations | Actual repetition/cadence/stop policy **UNKNOWN**; repeated request≠backend loop |

The map therefore does **not** invent a universal “loop bit”, loop-count field, loop resource owner or last-use guarantee. The exact bank/node→loop-semantic producer edge is deliberately **UNKNOWN**. A class name or bank file extension alone cannot close it. [A6]

## 9. Completion → callback → reuse → cleanup

Selected setup and completion edges are now concrete [A4]:

1. CSound setup`0046EA40` acquires Main and calls`0071FED0`; that routine independently acquires Core and stores code`0071FD50` atCore+488 through`0072C6C0`. Incoming Main is not a captured callback receiver in this setter.
2. Core selector/maintenance`0072CD80` walks current rows, requests currentE14 slot8 status output, and compares the output with literal0x20. It does not first certify that the query succeeded.
3. Selected status0x20 leads to`0072D6F0`; other explicit request/update paths can also reach cleanup.
4. Cleanup conditionally calls **currentCore+488 with currentE4**. If that code remains`0071FD50`, its direct operation is `DWORD[argument]=FFFFFFFF`—a main-row sentinel store for a matching admitted A pointer.
5. Later cleanup decrements selected per-bank bookkeeping under its byte gate, performs status operations, requests currentE14 slots4(0) andC, then clearsE14. Remaining helpers/status free/reset effects are separately qualified.

**VERIFIED local order:** sentinel callback can occur **before** the later foreign stop/destroy-shaped requests. Callback target, E4, cue pointer and root generations are independently current samples. Main-row availability is not proof the previous backend instance is fully destroyed or the final borrower retired. Callback reentrancy/mutation and API failures remain **UNKNOWN**.

The full-ID token resolver may therefore be locally satisfied on a reused/current row without certifying a continuous old voice instance. Historical repeated-token countermodels are not observed runtime defects.

## 10. Frame placement and final cleanup domains

Startup installs selector0 task callback`00454D50`; conditional object event1 reaches acquired CSound`0046F360`. Its bit31/world guards can skip selected work; continuing path performs Core maintenance then Main scalar-fed update. CMap+10==3 gates auxiliary audio, not every audio operation. **VERIFIED selected static path; actual B0008 cadence UNKNOWN.** [A1]

Static Core finalizer adapter`0076DF60 →0072C2E0` has available shared cleanup:

- independently sampled backend slots1C then8 requests;
-12 file-handle CloseHandle attempts, no local slot clear;
-13 allocation/deallocation-boundary attempts followed by DWORD clears;
-mapped view UnmapViewOfFile then clear;
-CoUninitialize request.

Those absolute arrays are not all receiver fields or the sound-bank table. Actual finalizer dispatch, lazy/static coordination, valid handles, successful shutdown/unmap/free, COM balance and final user/borrower are **UNKNOWN**. The later11-byte deallocator adapter qualification narrows the call boundary but does not prove pointee ownership or completed destruction. [A5]

## Evidence anchors

- **[A1]** `findings/boundaries/gameplay_audio_organizing_roots.md` (named roots/startup/task/use and core paired scope).
- **[A2]** `findings/boundaries/audio_descriptor_request_record_chain.md:11–55`; supplied selected PRM assets/primary receipts; `phase5_audio_client_selected_record_operation.md`.
- **[A3]** `findings/boundaries/phase7_architecture_a05_falsification.md` and A05 primary; exact wrap/range/instance qualifications.
- **[A4]** `audit/phase8_synthesis_2026-10-07/AUDIO_EDGE_PRIMARY.json`; `AUDIO_COMPLETION_EDGE_PRIMARY.json`. Steam C`509242–509259` mixcount/handle, `510443–510455` mask/speed, `510499–510535` calculate, `510551–510568` DSPapply, `509825–509888` later2×2 update, `509954–509960` status0x20, `510295–510335` cleanup, `500280–500282` callback installer, `500190–500193` sentinel. All checked entry ASM/bytes retained; no whole semantic closure.
- **[A5]** `findings/boundaries/audio_static_core_backend_cleanup.md`, Phase5 deallocator qualification; `phase5_audio_singleton_cleanup_interface_limit.md`.
- **[A6]** `findings/boundaries/phase5_hookchain_csoundloop_operand_retirement_limit.md`; verified q observations/currentCSound adapter, not full loop/provider/ownership semantics. P14 path remains at A2/A4 numeric scope.

## Post-synthesis PGC007 — typed FILEITEM producer and selected input ceiling

**VERIFIED Steam available ancestry:** named CAudio_Data/TSiHolder at00BD9E18 withdeclaredFILEITEM/CKey_Name hashmember1A8. New00701CB0 lookup/insert populatesP4 fromofferedbankselector, P14 fromnonzero-OR oftwo metadata DWORDs, andnew-keyP2C fromstoredkey-name pointer. Duplicate andsecondary arms meanthese are notimmutable/current all-node facts; identity00736980 isnotbyte reversal.

Selected offeredUPDATA/SOUND/SE.XSB ingress isconcrete, butexactselectedmetadata fileisabsentundersuppliedassets. ConcretePLSE066row/values/currentbank/node/authoredloopmeaning remain **UNKNOWN**; backendSOUNDEX/se.xsb isnotthesameproved metadata source. No I/O/backend success/GOG/ownership/currentness promotion.

Evidence/reconciliation: [FILEITEM population](../findings/boundaries/post_phase8_audio_fileitem_population.md), `audit/post_phase8_closure_2026-10-07/AUDIO_NODE_PRIMARY.json` and `AUDIO_SELECTED_INPUT_BOUND.json`. Stopatselectedmetadata/current-episode discriminator, notrepeatedPRM/key/allbank/token walks.


## Targeted bridge connections — TBC009/TBC011

**VERIFIED selected static scope:** TBC009 selectedSteamCSoundrequest0046EDC0/0046DB40 -> qpacking(((seq<<16)|selector)<<8)|index -> fullID256stride54 resolver0046A770 -> B2C copiedS6C4 -> latercurrentMain00720080. q/domain andMainT/domain aredifferentprotocols, notguaranteednumericallyunequal; B2Cfreshness/validity/S6C4continuity/rootepochs/success unproved. Returnedq isthen-currentB28afterothercalls; no NULLcheckbeforeB2Cdereference, no observedfault. TBC011 GOGstartupcallback00454D80installation -> ownrawevent1remap -> selectednestedstateout-ofrange arm -> independentlytypedCSoundgetter004183F0/ctor/table00773154 -> selectedmaintenanceworld10!=3 arm -> Core0072C100beforeMain00720690requests. Orphan/candidate/siteattribution retained; actualtaskinvocation/allarms/cadence/currentness/wholeGOGparity UNKNOWN. Focusedreports: `targeted_bridge_audio_coordination_id_transport.md`, `targeted_bridge_gog_installed_audio_task.md` under findings/boundaries.


## Final static campaign amendment — checkpoint256

**VERIFIED selected mechanics / STRONG_INFERENCE environment/thunder association:** CMapcloudcallbacks/time-compatibleweights/pulse connectsharedbanks toconcrete84-byte zero/seed packetproducer/nativeevent0E; nativeO44 andreceiver66FCcallbackchannels separate. OptionalchannelmathincludesHALF-luma0.5 andindependentpulse/fadeslotreloads; packet-tagcopy andflagmutationdomains differ. One66FCnonnulltest doesnotprotectlaterreload, noalias/weight/index/currentepoch proof. N_THUNDER1..9 andnumericCSound requestchain strengthenpulseassociationwithoutactual/synchronizedlightning/audio orsuccessfulrendering. Cleanup/null-basepathsconditionalstatic,notobservedfault/safe retirement. NoGOG/completepacket/schema/ownertheorem.

Evidence/qualification: `findings/subsystems/final_static_environment_packet_thunder_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
