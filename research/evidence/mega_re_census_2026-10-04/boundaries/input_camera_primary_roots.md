# Steam CInput / CCamera roots and sample-to-camera boundary

**Date:** 2026-10-03; sequence16. **State:** VERIFIED selected types/acquisition/transfers/static call order; STRONG_INFERENCE player-state receiver identity and orientation labels; UNKNOWN runtime cadence, full mode/lifetime policy and other-build correspondence. C0123–C0124 / BND-093–095. This is normal Phase3 root/use research, not a Phase2 reopen.

## Source / novelty / checks

Contract: `scratch/phase3_input_camera_seq16_contract.md`. Main inspected camera table/type/state/caller evidence and independently verified all decisive input findings returned by one read-only bounded subagent. Steam only; no established homology for queried getter/setter/dispatcher. Historical input/camera notes supply candidates, not proof. Do not transfer secondary GOG addresses or runtime observations.

`python3 scripts/inspect_phase3_input_camera.py` compares explicit selected instruction extents against `inputs/binaries/steam/DP_STEAM.exe`, decodes raw RTTI/bases/tables and checks relative calls. Receipt: `scratch/phase3_input_camera_primary.json`. Min/max exported function extents can bridge shared tails: `0052E0D0` tail-jumps to separately exported `00537270`; checking the entire min/max range is not a function-body proof. Mechanical matching is not semantic closure.

Primary source references below are workspace-relative and 1-indexed. Derived JSON preserves per-instruction address/line/bytes. No supplied file was changed.

## Typed acquisition / object model

**VERIFIED CInput:** cached `00BD9E10`; getter `00409610` allocates **0xBD0 holder bytes**, supplies ECX to `00409370`, publishes returned pointer and returns EAX. Same-ECX wrapper ctor calls `00707D90`, which writes CInput table `00827604`; wrapper then writes `0076F5DC` and clears +BC8. Raw RTTI names **TSiHolder<CInput>**, not CSingleton<CInput>. Its CInput base PMD is (0,-1,0), so the root is a valid zero-offset CInput receiver. Own one-slot tables point to `00709AC0` and `004098C0`. The core ctor's +BC0 dword gives lower bound0xBC4, not standalone sizeof0xBD0.

- `inputs/decompiler/steam/DP_full.asm:9954-9980` wrapper; `:10156-10194` getter; `:878996-879005` core vptr.
- `inputs/decompiler/steam/DP_decompiled.c:8382-8399`, `:8546-8580`, `:486285-486376`.
- Raw holder COL008757C8 / TD00886664; base BCD00875814 / TD00886688. CInput COL0087D29C. Exact raw structures are in receipt.

Selected fields: +668 embedded acquisition region (class name UNKNOWN); +7E4 pending7x40; +9A4 index / +9A8 pending count; +9AC aggregate7x4C; +C8 live7xCC records; +BC0 poll-state gate; holder+BC8 finalizer guard. Ctor initializes index/count/poll-state zero. These are one object's layouts, not seven input objects.

**VERIFIED CCamera:** getter `00449550` and selected lazy callers acquire **0x1AC** at `00BE1EA4`, write own one-slot `007721BC` CSingleton<CCamera>. Raw RTTI COL008768BC/TD008A75B8 and BCD00876854/TD008A7588 identify zero-offset CCamera; base table007721A4 has the same deleting target00449130. Selected lazy acquisition initializes a vptr, not every camera field. Setter/body evidence must not stand in for an unseen full initializer.

- `inputs/decompiler/steam/DP_full.asm:84466-84479` getter, `:84054-84065` deleting implementation; receipt raw structures.
- Deleting implementation rewrites base vptr and optionally calls0074E82B. Actual root-clearing/deleting invocation is UNKNOWN, not globally absent.

## Sample producer -> pending -> live -> public accessor

**VERIFIED selected transfers:** `00708B00` receives CInput ECX; calls `00709C40` on this+668 with a stack output of seven0x6C logical records, then calls six aggregation helpers on the original CInput. Primary acquisition calls joyGetPosEx plus active/foreground/cursor APIs. No conventional Win32 message helper source is reopened. Embedded device/config/binding ownership remains UNKNOWN.

Under +9A8 != 1, producer increments count and copies seven0x4C aggregate entries into seven0x40 pending entries at +7E4. Constructor-normal index/count and modulo-one publication establish the selected one-deep pending mechanism; arbitrary corrupt/alternative state support is not proven. Producer resets aggregate flag/digital/accumulators, not every axis. `00708350` first preserves live +CC into previous+D8 for each of seven0xCC records, then consumes pending if count!=0 and decrements count. It replaces flags/buttons/axes, adds two accumulated fields and derives rising/repeat masks. Sample freshness is not a timestamp observation.

| Aggregate field (+slot*4C) | Pending offset (+slot*40) | Live field (+slot*CC) |
|---|---|---|
| byte9AC / digital9E8 | 00 / 04 | C8 / CC |
| 9B0 / 9B4 | 08 / 0C | DC / E0 |
| 9B8 / 9BC / 9C0 / 9C4 | 10 / 14 / 18 / 1C | E4 / E8 / EC / F0 |
| accumulated9C8 / 9CC | 20 / 24 | F4 / F8 (added, not replaced) |
| 9D0..9E4 | 28..3C | FC..110 |

- `inputs/decompiler/steam/DP_decompiled.c:486648-486703` producer; `:486437-486530` commit; `:487196-487252` API acquisition.
- `inputs/decompiler/steam/DP_full.asm:879873-879967` acquisition/aggregation/gate, `:880043-880044` axisEC pending copy; `:879334-879438` snapshot/commit and corresponding receipt instructions.

`00708A30` takes actual ECX and byte arguments(slot,pair,component), reads `this+slot*CC+E4+4*(2*pair+component)` for observed valid0/1 indices; component1 negates. It does not itself sample the device. Decompiled call expressions hide ECX; raw instructions establish it (`inputs/decompiler/steam/DP_full.asm:879807-879839`; C`:486609-486627`). No unrestricted index validity is claimed.

**VERIFIED class-proven use BND-094:** camera helper `00537660` loads CInput root into ECX and calls getter at005377DB/00537819/00537876 for slot byte00BD77B4,pair1,component0 (+EC). The final result changes the same raw-typed camera root's +70 field at00537888. Numeric threshold/scaling exists; horizontal-look/yaw meaning is STRONG_INFERENCE, not real-time angular-speed proof. `inputs/decompiler/steam/DP_full.asm:350978-351034`; C`:198217-198239`.

## Camera control tables are not input staging buffers

**VERIFIED mechanics C0124:** raw `.data` `008A9980` has the selected137 dwords of numeric modes; `008A9BC8` has17 handler pointers. `00528F40` commits receiver+654=requested state and previous+658, then indexes008A9980 with that requested state, can override the selected mode via separate conditions, acquires CCamera root and supplies it as ECX to00534C60. Setter copies camera+154 to+158 and writes selected+154 after an additional player-previous-state-related override. Therefore the mapping is not an unconditional one-table-only transition rule.

The state receiver's **CPlayer identity is STRONG_INFERENCE** from the accepted CPlayer type/factory and packed-player-handle consumers, not promoted solely from familiar offsets. Exact pointer genealogical proof from typed CPlayer publication to every state-setter invocation remains open. The numeric commit/table/typed-camera transfer itself is directly proven. C0004's whole CPlayer handler-table137-slot/extent claim is not settled by reading137 **camera mapping** dwords. It remains secondary/STRONG_INFERENCE.

Dispatcher `005354D0` reads camera+154, accepts numeric0..16, calls a nonnull handler **except mode9**. It loads the handler pointer into ECX and CALL ECX; that ECX is a code pointer, not CCamera this. Handlers reacquire the root. Observed table: mode0=00538980; mode2=0053B8B0; mode4=null; mode9=00537F10; modes10/11 share0053C530. No algorithm labels for all handlers. Mode0 directly gets CInput pair1/component1 at00538B17/00538B46 and can write camera+6C; its own conditions are retained, not all-mode movement.

- `inputs/decompiler/steam/DP_full.asm:336230-336297` commit/index/receiver call; `:348090-348130` setter start; `:348650-348663` dispatch; `:352325-352363` mode0 getter/use.
- `inputs/decompiler/steam/DP_decompiled.c:187247-187307`, `:196752-196790`, `:197123-197129`, `:198675-198704`.
- C0011 remains broader Steam/GOG STRONG_INFERENCE because its complete paired/type scope is not newly proven. Steam mechanisms are recorded by C0124; source/debt normalized without fictitious cross-build confirmation.

## Lifecycle placement and first missing edges

**VERIFIED BND-093:** startup004017C0 calls00408D80 at004019B5; that routine calls acquiring getter00409610 at00409151. Frame00401A70 loads same input root, calls commit at00401AB0, then conditional poll wrapper007099D0 at00401AF0 **before selected object-dispatch calls**. Wrapper skips producer for +BC0==2; otherwise calls00708B00 at007099E8 and changes nonzero+BC0 to2. No once-per-frame or always-poll assertion. Raw frame`:738-756`; startup/constructor references in receipt.

**VERIFIED BND-095:** accepted typed active manager006C5FF0 writes numeric phase5, calls0052E0D0 at006C6A81, then writes6. Wrapper checks resolved packed object nonnull, CMap005D07F0==0, camera flagbit2 clear, actor004FEE60(2)!=0 and01476988 bit0 clear before generic camera dispatch. It tail-jumps to00537270 when camera mode!=9 (body not fully interpreted here). This is selected conditional static placement, not every camera path, mode9 scheduling or traced cadence. `inputs/decompiler/steam/DP_full.asm:341045-341122`, `:795461-795465`.

**VERIFIED available finalizer; invocation UNKNOWN:** wrapper ctor registers raw00408A30 with006E1930. Callback tests holder+BC8==0, invokes own slot0 withargument1, then clears00BD9E10. Holder deleting implementation004098C0 calls core cleanup00708280 and optional00403540. Callback execution owner/actual shutdown invocation is first untyped lifetime edge; no global free-absence claim. Raw`:9291-9311`, `:10366-10377`, wrapper`:9967-9973`.

Further work requires new concrete typed player-handle publication/state-call proof, camera initializer or mode9 caller, callback-registry invocation/free coordination, or GOG structurally matched boundary. Runtime C0010 latency/dormancy/frequency needs reproducible traces (B0010). No repeated getters/API enumeration/table census can settle those questions. B0001–B0009 and all accepted Phase2 reopen limits remain unchanged.

## Sequence17 narrow genealogy continuation — 2026-10-03

C0126 / findings/boundaries/cgame_record_reconstruction_root.md supplies the previouslymissing selectedtypedCPlayer publication/callbackevent0/currenthandle/reconstruction/sameESIstate40 genealogy. The earlierSTRONG_INFERENCE history above ispreserved; current00528F40 receiverisVERIFIED for thispositivechain. No full137-slot/actionhandler/C0004/GOG/allcallers promotion. Newsource isexacttypedcallback descriptor008BF668 andevent0consumer, notanother untypedoffset scan.
