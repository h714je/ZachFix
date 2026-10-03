# Steam audio descriptor → named inline record → retained request chain

**2026-10-03; Phase4 AUDIO_DESCRIPTOR_REQUEST_BANK_CHAIN; ADVANCE.** C0155–C0157 / BND-134–136. Selected static reconstruction only, not complete audio, successful playback, or bank ownership. C0118's exact CSound / CSdMain / CSdCore getter and type scopes are reused; no root, task, initializer, DirectShow, shutdown or GOG recensus.

## Contract and first missing edge

Start at the new nonconstructor writer0046C980/0046EA40, prove resource3A0B return and entry4E, then follow the same selected value through typed main/core records. Include only directly necessary parameter/list/named-lookup accessors and first opaque bank/per-record interface. Stop at the missing selected named-node field producer and foreign-interface identity. Independent Opus branches are subordinate evidence; important results were personally checked before canonical promotion.

**UNKNOWN first edge:** the successful `PLSE066.PCM` named-node's population/type and actual P+4/P+14/P+2C contents. The symbolic pointer and conditional consumer instructions are proven; a concrete bank index, cue name, bank receiver, API identity, resident node, safe lifetime and successful operation are not. Missing selected content is not a global absence bound or a reason to continue generic container/bank discovery.

## VERIFIED producer, returned view and selected identity

1. Raw00454EB6 calls accepted CSound getter004183D0;00454EBC moves EAX to ECX;00454EBF is an **E9 tail jump**, not a CALL, to0046EA40. The latter saves this receiver, invokes the acquired main setup helper, restores saved ECX and calls0046C980 atEA51. Writer saves it in ESI. Together with accepted raw CSound RTTI00773164/COL00876D80/TD008A8370, this is a class-proven CSound writer.
2. At0046C9CB–DC, getter0040EDE0, literal3A0B and0044C060 yield EAX stored in that CSound+6604. The incoming CFunc-like getter result is **not** the resource consumer receiver:0044C060 ignores incoming ECX, acquires CRdData via004051F0, switches ECX to EAX and calls accepted006B2BE0 with `(ID,0)`. Null payload returns null; otherwise its first signed dword H drives `payload + H - signed_rem(H+1,4) + 13`. This is not an invented generic ceil-alignment or full PRM parser.
3. Primary static resource record0094CED8 has ID3A0B at+18, path0080BCD0 at+1C=`UPDATA/PRM/SND_SE.PRM`, size3068 at+8. The supplied asset has exactly3068 bytes, H=8 and table offset14hex. Count407hex=1031 atfile10 agrees with `(3068hex-14hex)/12` and consumer's inclusive upper index406hex. Selected4E is table+3A8/file+3BC: six little-endian words **(64,30,30,60,0,0)**. This establishes static resource/table/index correspondence, not live payload residency or bytes observed in a running game.
4. Earlier writer3A05/+54AC binding is independently anchored by record0094CE18/path0080BC44=`UPDATA/PRM/SE_LIST.PRM`, size3F88. Asset H=14decimal/0Ehex gives table offset18hex. Selected row64 atfile418 has raw string `plse066`.0046B6B0 indexes sixteen-byte rows, formats `%s.PCM`, uppercases, queries00702480 and copies its written output-wrapper dword into CSound+54B0[64]=CSound+55B0.00702480 returns the output-address in EAX, not P; the writer loads P from the output dword. The resulting **lookup key** is `PLSE066.PCM`; it is not thereby a cue/API or proof an identically named asset was supplied. If the written P is nonzero, this population method also writes row byte+E=1, calls0072CFB0 on an acquired core with P, and writes returned AL to row byte+F. Those two parameter-view state writes are proven; their numeric meaning and the helper's fuller effect remain UNKNOWN.
5. Successful00702480/00703600 lookup yields an inline address:00703E00 adds C to node;006C4840 adds14 to that address; neither loads a pointee. Thus **P=node+20hex**, not a dereferenced owning payload pointer. Lookup compares its key string at node+18; the downstream name-pointer field P+2C=node+4C is different. Matching `PLSE066.PCM` does not prove that later name's content.00720D90 wraps P in one dword;0046F800 copies P. CSound retains this address; named-container/node ownership and lifetime remain UNKNOWN.

Writer checks prior3995/+648C and3A05/+54AC results, not the3A0B result. Outer0046EA40 ignores writer return and itself returns1. Neither outer1 nor the computed-address check in0046AD80 proves descriptor readiness.

## VERIFIED selected request and typed receiver transition

Untyped outer005BB0D0 supplies its+58 pointer, entry4E, scalar controls **0.5,0,100,150**, and flags0; accepted getter returns actual CSound ECX for0046AD80. After normal return it sets outer+458=1 without checking request result. Outer class, flag meaning and activation remain UNKNOWN.

0046AD80 unsigned-checks index<=406, computes `CSound6604+12*index`, sign-extends its first short and requires `CSound54B0[short] != 0`. Its TEST is on the **computed entry address**, not table base; no local full table/short-range safety proof. For the statically matched entry, s=64 and the selected raw value is P above.

0046AA90 preserves CSound and flags, conditionally checks a separately acquired numeric control root, copies four caller dwords to scratch and invokes opaque validators00469C90/00469B20. Their rejection arms returnFFFFFFFF; no friendly mode/geometry meaning is assigned. Descriptor word+2 is zero-extended to004692C0 and its numeric result scales0.5. Selected positive100/150 bypass descriptor fallbacks.

The continuation builds stack request R: word0=1, word2=64, dword4=P, float8=1. Controls B: first scaled scalar, second0, byte8=1, floatsC/10=100/150, and first three caller dwords at14/18/1C. **Bytes9..B and dwords20/24 are not initialized by this adapter before the ten-dword copy**. No fully initialized block claim. Getter00427700 returns actual CSdMain ECX atAC22;AC24 invokes007203A0. Adapter RET1C; main RETC; no producer stack-address ownership inferred.

## VERIFIED conditional main/core state and three numeric domains

| State | Exact mechanism | Responsibility / limit |
|---|---|---|
| Main row A |007208E0 scans128 rows `M+30+50hex*i`, cursorM28; free markerFFFFFFFF.00720890 generates **T=(old M2C<<8)\|i**, advances/wraps sequence, storesA0. | Generated main token, not resource ID, core index or backend success. Exhaustion returnsNULL, which main's selected tail dereferences; no clean numeric failure guarantee. |
| Main local C |007203A0 copies B's **0x28 bytes**, then appends A atC28, P atC2C and flags40-derived byte0 atC30. Flags0 skips optional flag transforms. | Same symbolic P/value and main-row backreference; clamp/scaling instructions are observed, not named audio policy. |
| Core call/output |00720659 acquires core via0046FA70→0046F9D0;ECX=EAX;00720660 passes **(&K,&C)** to0072CFE0. Core returns EAX=&K, RET8. | Numeric K is an output dword, not EAX's pointer value or an interface. |
| Core row E |0072CD80 selects up to32 slots;0072C690 computes `G+4+24hex*K`. Independently allocated status index **Q** is stored atE0; E4=A, E8=1. | **T, K and Q are distinct domains**. No generation-safe reuse or owning-reference guarantee. |
| Main retention |Signed K>=0 predicate007386C0 permits K→A18;A1C=flags0,A1E=marker1,A20=64,A14=1;ten control dwords→A28..4F; returns **A0=T**. | Tail copy excludes appended localC28/C2C/C30: do not invent a direct P field in that copied main tail. |
| Named value retention |Core forwards same P to status-record association00704C70/00736FE0; selected windows store P atstatusrecord10 and N=`[P+2C]` at34. | Raw retention/backreferences, not exclusive node/name/interface ownership. |

Main preselection007206F0, occupied-core-slot policy and later auxiliary tail calls remain opaque. Described return paths assume those invoked opaque helpers return normally; no universal total-function or live-path claim.

## VERIFIED consumer instructions; concrete bank correspondence UNKNOWN

The selected same-P accessors return **dword[P+4]→E+C**, **byte[P+14]→E+10**, and **dword[P+2C]→status name N**. They are raw offsets, not hidden owning-handle resolution. First missing selected field/type/content evidence remains here.

Available conditional consumer code: if N!=NULL, core copies/transforms its string, loads Bnk=`[014B01FC+4*b]` with b=E+C, calls Bnk slot0 and takes low AX. AX!=FFFF and selected byte8=1 choose **bank slot+C**, passing output storage E+14; not marker0's slot+10. The output interface E+14 is later used at its slot0. These are **untyped foreign-interface operations**, not promoted API names or proof that PLSE066 identifies a particular bank/cue.

Load-bearing limits:
-007386CA/CD is just signed dword>=0, not interface/backend validation.
-Name=NULL or AX=FFFF leaves K nonnegative and skips interface-output operation. If subsequent opaque calls return, main can retain K and return T without that operation.
-Bank slot+C return is not tested; E+14 is later consumed without a local null guard.
-The apparent bank null test checks literal014B01FC, **not** selected `[014B01FC+4*b]`; no local full bank-index/range/null guarantee.
-Static bytes establish instructions, not live crash frequency, successful playback, once-per-frame cadence, balanced retirement or safe node/bank/name lifetime.

## Primary evidence and personal verification

- New writer/request: Steam `inputs/decompiler/steam/DP_full.asm:125396-125511`, `:127836-127847`, `:123167-123195`, `:122925-123059`, `:497293-497321`; C ranges in original sourcecard `reports/PHASE4_AUDIO_DESCRIPTOR_ENTRY_SOURCECARD_2026-10-03.md` remain locator history.
- PRM view `inputs/decompiler/steam/DP_full.asm:87349-87367`; raw PE records0094CED8/0094CE18 and path strings; supplied assets `inputs/assets/dpserial/UPDATA/PRM/SND_SE.PRM` and `SE_LIST.PRM`. Main asset receipt preserves full SHA256 and selected offsets/bytes: `scratch/phase4_audio_main_asset_verification.json`.
- Named lookup and exact producer/check locators: `scratch/phase4_audio_descriptor_branch.md`, its checker and receipt. Main personally checked raw resource IDs/paths, both selected asset headers/entries, getter formula and terminal inline-address instructions before accepting.
- Main/core `inputs/decompiler/steam/DP_full.asm:908135-908414`, `:908561-908641`, `:925305-925676`; detailed helper/window/ABI references in `scratch/phase4_audio_record_branch.md` and receipt. Main personally reviewed copy boundaries, actual acquired receivers, K/T/Q distinction, status P/name retention, marker1 bank+C and failure paths; locally reproduced the complete branch checker.
- Main checker `scripts/inspect_phase4_audio_main.py` / `scratch/phase4_audio_main_checks.json`: **326 instructions/1163 bytes**, six fully contiguous instruction-aligned spans,12 selected E8/E9 targets and26 byte anchors, rawCSound type and selected floats.
- Record branch `scripts/inspect_phase4_audio_record_branch.py --receipt`, locally rerun in `scratch/phase4_audio_record_main_replay.txt`: **1332 PE instructions/4284 bytes**,31 independently aligned continuous spans;1331 ASM-matched/4279 bytes plus raw-only0072CE30 JMP0072CF66,1/5 omitted by ASM.85 operands,38 important calls,144 decoded E8/E9 and32 RET sites. This omission is preserved, not transformed into a complete export claim.

- Descriptor branch `scripts/inspect_phase4_audio_descriptor_branch.py`, locally rerun in `scratch/phase4_audio_descriptor_main_replay.txt`: **624 instructions/1860 bytes**,26 continuous aligned windows,85 operands/34 calls, two raw types/records/assets;145/423 are explicit accepted resource-primitive rechecks. Omitted0070424D–4E bytes are excluded explicitly, not silently bridged. Main personally reviewed the population/output ABI and lookup/key/inline address continuation.

Counts overlap and are not additive executable coverage. No runtime, selected resident named-node contents, full PRM grammar, new GOG/Xbox homology or all-audio ownership proof. Both branch replays and main326/1163 checks actually passed before promotion; all branch source-history/conditional limits remain preserved.

## Carry-forward and architectural gain

The three formerly separate typed roots now have one static symbolic request/value/record connection anchored in actual parameter assets, an inline named-node retainer and distinct main/core/status record domains. This improves the model of responsibilities without calling numeric success a successful resource/audio operation.

`AUDIO_NAMED_RECORD_VALUE_LIFETIME` carries a concrete discriminator: class-proven population/insertion for the selected PLSE066 named-node P, its4/14/2C fields, correlated bank/name/interface data, or a reproducible selected pointer trace. It is **unvisited continuation**, not a fabricated global static bound. No repeated getters/setup/PRM/table/slot/wholebank census. Original B0008 cadence, AUDIO_SHUTDOWN_COORDINATION, all Phase3/earlier bounded sources and lifetime/build limits remain unchanged. Pivot to the independently located CMenu→CGame numeric commit mechanism after synchronized closeout; no Phase4 closeout or Phase5.
