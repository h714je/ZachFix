# Typed movie request → task/private state → CFade data and presentation latch

**2026-10-03; Phase4 sequence41; ADVANCE.** Selected conditional Steam mechanism. C0162–C0164; BND-142–144. No full movie/fade/weather algorithm, runtime/GOG/lifetime or Phase4-closeout claim.

## Contract, accepted reuse and independent verification

`scratch/phase4_effects_seq41_contract.md` and `reports/PHASE4_PRESENTATION_FADE_ENTRY_SOURCECARD_2026-10-03.md` bound a newly located typed request/callback/state/latch producer. Reuse C0134–C0136 for CFadeManager/CFade/CRdPrim and selected tail consumer; C0137/C0138 for separate in-place CDemoMovie; C0113/C0117 for CRdMovie acquisition/conditional frame ordering; selector0 task/setter infrastructure only at accepted scope. No root/factory/free/XWP/weather/event-stream recensus.

Independent Opus branches checked the callback's selected event/private-state/control path and the CFade NULL/request7/latch bodies. Main read their reports/checkers and decisive assembly, separately replayed both, and personally checked raw table cells, widths, receiver loads, state stores, snapshot ordering, AL-only return and literal latch before promotion. Original branch receipts remain unchanged. Main owns producer/install/consumer composition.

## Object and value domains

**VERIFIED selected identities:**

- **M** = typed in-place CDemoMovie **008A7218**; dword+8=008A7220 is numeric request, dword+14=008A722C is callback private state; **+C=008A7224 is separate**.
- **T** = newly returned selector0 CRdObject task that receives callback+44; callback's first stack argument, **not M**. ESI initially holds T but can later hold CMessage allocation.
- **R** = separately acquired CRdMovie00BD9E48, saved in EDI; selected **byte+4** is a gate, not M+4's dword bit31.
- **F_A** = [manager returned at0042816C +4] for NULL source helper; **F_B** = [independently returned manager at00428198 +4] for request7; **F_C** = later consumer's manager+4 load. Accepted retainer genealogy supplies CFade type, **not atomic alias/lifetime/freshness proof**.
- **L** = receiverless dword **00BE1EAC**. CRdPrim is the renderer receiver; CFade supplies data.

Numeric **movie request0x40/decimal64**, private-state0/3, CFade request/state7, scalar30.0 and latch1 are distinct domains. No friendly cutscene/fade-direction/completion labels are assigned from them.

## Typed producer and actual task installation — VERIFIED selected mechanics

At005DF190, under **00BDBCD0==4**,005DF1A8 sets **ECX=M** and005DF1AD calls00428870 with request0x40 and four controls **0,0,1,0**. The original incoming receiver and producer of that outer command word remain UNKNOWN. After normal return the caller clears00BDBCD0 without testing acceptance; this clear is **not successful request admission**.

00428870 saves M in ESI and request in EDI. Unsigned(request−3)>0x22 for0x40 directly selects the movie arm. **M+4 bit31 set rejects** before the new stores/installation. Otherwise opaque004288DB→0040EDE0 and returned-receiver004288E2→0044E560 occur first; their effects/return/preservation are not expanded. Continuing code writes M+8=0x40 and bytes+1C/+1D/+1E/+1F=0/0/1/0.

A further opaque00428906→00406FF0 result receives byte+24D=1; its type/purpose is intentionally unresolved, not guessed as camera. The accepted application manager then returns **T** from006C5930 with numeric factory arguments **0,0xF,0,1**. Actual0042892C sets **ECX=T** and0042892E supplies literalcallback **00427AF0**, auxiliary0, to reused006BAB80. It publishes T+44 and synchronously emits **event0**, not actual event1 delivery. Valid helper/factory results, initialization and lifetime are explicit prerequisites, not checked success guarantees.

## Available state0 seed / selected event1 route

**VERIFIED conditional event0 availability:** callback receives T as first stack argument and low8 event as second; event0 selects00428302. After opaque task/media setup, a **current** request0x40 uses raw map00428858=5/table004287F0→00428452. Normal return from opaque R-receiver00700260 reaches0042876C, ORs M+4 bit31, separately clears **M+C and M+14**, and issues a retained-F request7. This is a concrete available state0 writer, **not successful media setup, persistent state0 or future event1 activation**.

**VERIFIED selected event1 mechanics:** subtraction/conditional branches (not an event map) select00427EBF. Dword M+14 is unsigned-bounded0..4; raw table004287C8/index0→00427ED4. A selected sufficient state0 bypass uses a current request0x40, returning acquired-CInput query00708910 with EAX0, a subsequent nonzero M+8 read, then **GetAsyncKeyState(0xD) AX==0 OR byte014810F0!=0** to reach0042810A. Other query/mode paths are excluded. Actual inputs/OS outcomes/register/pointee continuity remain UNKNOWN.

0042810A independently requires current **R+4 byte0**. Its CMessage acquisition/opaque0045A560 returning continuation re-reads M+8 and stores **M+14=3**. Only a newly read0x40 uses the selected path avoiding0x46/0x47/0x48 branches. It supplies **F_A, NULL** to00449890, then **reacquires** the manager and supplies **F_B, [7,0x41F00000,0,1]** to00449A00. Raw scalar0077036C is float32 **30.0**; units/policy are UNKNOWN.

The three helper-return continuations test **no AL/EAX success**. In particular00449A00 return is followed by a **separate current R+4 byte test**, not by testing its byte result. If current R+4 is zero under the preserved caller carriers,004282EA calls00449690; otherwise the callback exits. Earlier R-zero/input/request reads do not prove later values survive opaque calls. No single-this or unconditional completion chain is asserted.

## CFade source versus request-state/output — VERIFIED local endpoint

Require valid incoming CFade and the call-free **manager-root-nonzero** local path, or explicit postconditions if an optional allocator route is taken.

00449890 selected NULL stores **F_A+19C/+1A0/+1A4=0.0, +1A8=1.0**. It does not locally clear+164/+168/+188 or current+18C..+198. RET4 defines no scalar/this return. Its optional root-zero allocator/publication routes are not safe-retainer initialization proofs.

00449A00 retains incoming F_B and reads the full32-bit request. For7: nonzero/unsigned<=8 gates admit it; raw remap00449D9B=1/table00449D8C→00449A74 increments dword+188; raw request cell00449DBC→00449C6B selects the actual arm. **No old+164/+168/source-data admission test** occurs on this route. Controls occupy four-byte stack slots but comparisons use low bytes; third-control0 is untested by7, fourth-control1 controls optional cache acquisition, not numeric acceptance.

Selected ordered effects:

| Field / stage | Exact local effect |
|---|---|
| +17C | float0 |
| +188 | dword INC, modulo2^32; meaning/signedness unknown |
| +178 | float1 |
| +18C/+190/+194/+198 | raw copies of F_B's own+19C/+1A0/+1A4/+1A8 |
| +164 | dword7 |
| +16C | float30.0 |
| +170 | operational x87 reciprocal1/+16C, float-store rounding under live environment |
| +198 | **snapshot of+178=1**, overwriting the fourth copied source word |
| +178, afterwards | FLD live014AFFE0 → FMUL stored+170 → double → add current+178 → float store; **no new+198 snapshot** |

Thus **VERIFIED distinct data responsibilities:** source quartet19C..1A8, copied/presentation quartet18C..198, numeric control164/scales16C/170 and progression178 are not one buffer/value. If F_A=F_B remains valid/preserved, the first three copied words become0; this identity is **UNKNOWN**, so unconditional[0,0,0,1] output is not claimed. Request7's local+198 snapshot1 itself does not depend on the NULL-source identity.

014AFFE0 is mapped but **not file-backed**; its live value, units/cadence, x87 environment and final+178 are UNKNOWN. +198 is not automatically the latest progression scalar. Optional root acquisition can introduce opaque effects; no transitive preservation/free/safety proof. **Return defines AL=1 only**, upper EAX unproved; the decompiled32-bit `return1` is not an API-width proof. This numeric admission/return does not mean movie/fade/draw completion.

## Literal latch and accepted consumer — VERIFIED joins, conditional chronology

00449690 is exactly **dword[L]=1; RET**,11 bytes/two instructions, no ECX read or scalar return. Callback policy is independent of the F-helper result as described above.

Reused C0135 tail reads M+8 and excludes **0x46/0x47/0x48/0x49**;0x40 is not excluded at that particular read, not a frame-wide mode guarantee.0044AC10 tests L!=0, **compares0146E8C0 before the clear**, stores **L=0**, then branches on the earlier guard flags. Nonzero guard consumes the latch **without drawing**. This exact compare→clear→branch order refines, but does not contradict, the accepted clear-before-guarded-continuation scope.

On the admitted consumer continuation, F_C is reloaded from manager+4 and **+18C/+190/+194/+198** are copied as four words into the local value passed to acquired **CRdPrim/006E3360**. It reads+198, not+178. This verifies the new writer's state/output interface to the accepted consumer. **UNKNOWN:** F_B=F_C, last writer, intervening CFade event update, actual event/order/draw selection, latch coalescing/concurrency, fresh geometry, final GPU or success. No new renderer-body rediscovery or RGBA/vertex semantics beyond accepted scope.

## First missing edges / dependency consequence

- **UNKNOWN:** outer00BDBCD0==4 producer/receiver, earliest prepublication helper effects, returned+24D receiver type, successful/valid task/retainers and actual event1/state0 inputs.
- **UNKNOWN:** CRdMovie+4 byte production/policy, input/OS outcomes, CMessage/other opaque effects, each later request/root/F/byte read and safe lifetime.
- **UNKNOWN:** source-field population beyond selectedNULL, F_A/F_B/F_C correspondence, progression/output freshness, scalar units, mode semantics, complete request3/other CFade algorithms, all movie/weather policy and material GOG/Xbox scope.
- Preserve original free/weather/XWP/CDemo scene/input/runtime bounds. `PRESENTATION_FADE_VALUE_POLICY_AND_LIFETIME` tracks new residuals; old living effects/mode/lifetime rows retain their exact triggers. No Phase3 regrade or Phase4 readiness inference.

The bounded architectural mechanism is established at conditional state/data/latch/consumer scope. Further local fade states are not pursued merely because implementation exists. Global candidate review follows, with a separately located UI/Fade-predicate/camera source offered only as a fresh successor locator.

## Reproduction and primary citations

- Main: `python3 scripts/inspect_phase4_presentation_main.py` → **136instructions/443bytes/7windows/51anchors/11calls**, including **265bytes accepted consumer/tail/setter reuse**. Full producerbody and alternate request arms are not analyzed.
- Callback separate main replay: import `inspect_phase4_presentation_callback_branch`, redirectOUTPUT to `scratch/phase4_presentation_callback_main_replay.json`, callmain(): **179instructions/678bytes/10 independently decoded windows,64anchors/21calls/21edges/1483assertions**, no selected gaps/orphans/false-noreturn correction. Optional event0 seed57/257 is a conditional producer, not initialization success. Envelope3288 agrees with export; no full CFG proof.
- CFade: `python3 -B scripts/inspect_phase4_presentation_fade_branch.py --verify-receipt`; `--json` was separately stored as `scratch/phase4_presentation_fade_main_replay.json` and compared to preserved original. **154records/592bytes +61 rawtable/scalar bytes,9targets/15branches**. New local118/469; ancillary ABI28/90; accepted state-reader8/33. Selected request7 bytes319/901; all narrow ABI checks349/901; **552bytes intentionally unexamined**. Counts overlap, not additive coverage.
- Producer/request/install: `inputs/decompiler/steam/DP_full.asm:537037-537052`,`:45509-45565`; C`:304323-304334`,`:30746-30810`.
- Callback/gates/input/state/fade/latch caller: ASM`:44611-44640`,`:44891-44913`,`:44923-44928`,`:45049-45096`,`:45183-45196`; optional seed`:45197-45229`,`:45276-45286`,`:45480-45492`; C anchors in branch receipts.
- NULL/request7/latch: ASM`:84703-84749`,`:84809-84841`,`:84936-84985`,`:84585-84586`; C`:56650-56684`,`:56737-56874`,`:56520-56526`. Raw cells are in `inputs/binaries/steam/DP_STEAM.exe` and separate derived receipts.
- Accepted consumer/tail context: ASM`:85874-85919`,`:514-527`; C`:57323-57373`; exact reused scopes in `findings/boundaries/effects_organizing_roots_and_use.md`.

All supplied sources and prior accepted scopes are conserved. New canonical function classifications concern selected game-owned request/callback/data/latch mechanics, not whole-body algorithms or completion. No supplied evidence, asset, binary, production source or runtime was modified.
