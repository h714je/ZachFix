# Static CSdCore -> shared backend/storage cleanup

**2026-10-04; STEAM_PC; sequence56; ADVANCE.** C0178/C0179; BND-164–166. Conditional static local mechanism, not observed shutdown or whole-audio lifetime closure.

## Contract and primary scope

Selected `AUDIO_STATIC_CORE_BACKEND_CLEANUP`, parent validated sequence55. `scratch/phase4_seq56_audio_contract.md` fixes the universe to caller **0076DF60–0076DF6F** and cleanup **0072C2E0–0072C3EB**, exclusive ends, plus their raw CSdCore type/import/metadata anchors. Heuristics RH05/RH07/RH08/RH10/RH20–RH25 and EP05/EP07 rank verification obligations only; none supplies semantic evidence. No constructor/root/bank/request/CRT/free/GOG/runtime census or opaque helper implementation was added.

**VERIFIED reuse, not new credit:** C0118 and H0268 establish separate in-place **CSdCore 014B0400** and lazy **CSingleton<CSdCore> cache0138A6E4**, same constructor family, shared backend acquisition and literal registered finalizer0076DF60. The accepted task installation/event1/core-main maintenance chain remains conditional; B0008 cadence remains trace-blocked. GOG acquisition/static-init correspondence does not establish this new Steam cleanup contract in GOG.

The novel discriminator is the previously unreconstructed **44 instructions/159 bytes middle**, not sequence51 caller/head/tail availability36/123 or sequence55 caller/type-prefix12/33. Main personally checked the returned branch facts against the full assembly/decompilation and executable; independent GNU decoding agrees on the entire selected caller/body.

## Storage, receiver and acquisition distinction

**VERIFIED:** `0076DF63` supplies ECX=**014B0400**; `0076DF68` directly calls0072C2E0. The callee saves incoming ECX at `[EBP−10]` (`0072C2E6`) and writes **00828548** through that saved pointer (`0072C2EC`). Raw table00828548 -> COL0087D578 -> TD008A8358 -> `.?AVCSdCore@@` independently types the receiver. PE section mapping places014B0400 in virtual-zero-filled `.data`, not a heap allocation or on-disk initialized object pointer.

**VERIFIED:** the following storage is accessed by **absolute address**, not receiver-relative offsets:

| Domain | Literal storage touched here | Mechanical role / limit |
|---|---|---|
| E | 014B01E4, one dword | Shared backend/interface cell from accepted acquisition. This body reads but does not directly clear it. |
| A | 014B0268–014B0297, 12 dwords | Values tested as −1/0 and passed to imported CloseHandle; no direct A-slot clear. |
| V | 014B0298, one dword | Value passed to imported UnmapViewOfFile, then directly cleared. |
| B | 014B029C–014B02CF, 13 dwords | Values conditionally passed to opaque0074EE0B, then each dword directly cleared. Concrete pointee types/ownership UNKNOWN. |

Adjacency A→V→B does not prove a common allocation, resource identity or exclusive owner. B is not the separately accepted indexed bank storage014B01FC. None of these arrays is a +offset field of static014B0400. The only non-stack store sites in the whole inspected body are receiver-vptr0072C2EC, indexed B-clear0072C3B4 and V-clear0072C3D7; opaque/imported callees' effects are not constrained by that local inventory.

## Shared backend attempts

**VERIFIED branch/transport:** guard0072C2F2 tests a sample of `[E]`; zero jumps to0072C322. On the nonzero arm:

1. `0072C2FB` reloads E for vptr/slot **+1C**. `0072C303` separately reloads E and pushes that value; `0072C30C` calls the loaded slot.
2. After that opaque operation, `0072C30E` freshly reloads E for vptr/slot **+8**; `0072C316` separately reloads E for the pushed receiver; `0072C320` calls the slot. There is **no fresh null gate** before this second dereference/call.

**UNKNOWN:** equality of guard/slot-source/pushed-receiver samples, preservation across the first operation, actual backend class/API operation identities, successful shutdown/release or release counts. +8 and COM-oriented acquisition do not alone prove an actual Release or balanced retirement. Neither return is tested; the following XOR overwrites EAX. There is no direct E-cell reset in the inspected body, but this does not prove the callees leave E unchanged.

## Newly reconstructed middle: two different cleanup domains

**VERIFIED reachability:** `XOR EAX,EAX; JZ` at0072C322/324 and0072C36D/36F force entry into their respective loops on ordinary entry/control flow. Decompiler warnings about removed0072C326/371 concern the **unreachable bypass JMPs**, not unreachable cleanup loops. These XOR/JZ pairs do not test a backend result or readiness state.

### A: 12 conditional handle-close attempts, no local slot reset

**VERIFIED:** `[EBP−4]` starts0 at0072C328, increments by1 at0072C331–337, and signed `CMP index,0C / JGE` at0072C33A/33E exits at12. It visits indices0..11 if the selected calls return normally. Each iteration:

- separate sample `A_minus1` at0072C343: −1 skips the call;
- otherwise separate sample `A_zero` at0072C350: 0 skips the call;
- otherwise reload `A_arg` at0072C35D, push at0072C364, call **KERNEL32.dll!CloseHandle** through IAT0076E01C at0072C365;
- continue to the increment with no BOOL check and **no direct A-slot clear**.

**UNKNOWN:** that A_arg equals either tested sample or is still a valid nonzero/non-−1 handle, creator/type/ownership/last-use and successful handle closing. The local guards do not supply serialization or idempotence; unchanged stored values do not prove a live leaked handle either.

### B: 13 conditional opaque calls, unconditional dword clears

**VERIFIED:** `[EBP−8]` starts0 at0072C373, increments at0072C37C–382, and signed `CMP index,0D / JGE` at0072C385/389 exits at13. It visits indices0..12 on the ordinary returning-call path. Each iteration:

- a guard sample at0072C38E chooses whether to call;
- on nonzero, a distinct reload at0072C39B is staged through `[EBP−C]`, pushed at0072C3A8 and passed to **0074EE0B** by0072C3A9; caller adjusts ESP by4 at0072C3AE;
- both null-skip and normal-return arms reach **0072C3B4**, which stores dword0 to the indexed B cell, then increments.

**UNKNOWN:** helper implementation/effect, allocation/free/deleting semantics, concrete pointee class, guard-to-argument equality and ownership. The export label `FUN_0074ee0b` and cdecl-shaped caller stack adjustment are only locators/ABI facts. This report intentionally stops at that opaque edge. The clear is not a proof that the prior value was destroyed; after the call it clears the current cell without validating its contents or the helper's result.

## Terminal view/COM boundary

**VERIFIED:** guard0072C3C1 tests V. Nonzero reloads V at0072C3CA and passes it to **KERNEL32.dll!UnmapViewOfFile** through0076E15C at0072C3D1. Both zero-skip and returning-call arms reach **V=0 at0072C3D7**, then **ole32.dll!CoUninitialize** through0076E2F8 at0072C3E1. Epilogue0072C3E7–EA returns; caller0076DF6D–6E also returns. Raw PE import-directory decoding independently verifies all three named imports.

**UNKNOWN:** guard/argument equality, successful unmap, corresponding mapping's creator/last use, current executing thread/apartment and balanced COM initialization. Unmap BOOL is not tested before clear; CoUninitialize is not a success-producing protocol return. Normal control-flow completion is not backend/storage retirement success.

## Lifecycle obligation matrix

| Milestone | Evidence state / strongest supported result |
|---|---|
| Root/static storage | **VERIFIED:** literal typed static014B0400 differs from lazy cache0138A6E4; shared absolute E/A/V/B storage. |
| Acquisition/initialization | **VERIFIED reused C0118/H0268:** constructor/static init/lazy publication/backend acquisition/finalizer operand, not live success. |
| Backend/thread/task handoff | **VERIFIED reused C0118:** conditional installed task/core-main update; **VERIFIED new:** local backend slot/pushed-receiver calls. **UNKNOWN:** concrete cleanup/thread handoff or stop/join coordination. |
| Cleanup/reset | **VERIFIED new:** ordered local backend attempts, A CloseHandle attempts/no local A clear, B opaque call/clear, V unmap/clear/CoUninitialize, conditional on ordinary returning calls. |
| Actual destruction/free | **UNKNOWN:** no root deallocator, lazy-cache clear or invoked deleting interface is established by this finite body. Opaque0074EE0B is not assigned free semantics; handle close/unmap are distinct from object allocation free. |
| Safe retirement | **UNKNOWN:** no last-use match, shared-user quiescence, selected bank/node lifetime, cross-instance ordering or idempotence proof. Not a finding of an unsafe live run. |
| Runtime invocation/cadence | **UNKNOWN:** available registered adapter/direct call is not observed finalizer dispatch, shutdown chronology or frequency. B0008 remains unchanged. |

## Primary references, reconciliation and durable consequences

- Caller: `inputs/decompiler/steam/DP_full.asm:994541-994546`; supporting `DP_decompiled.c:571123-571134`; exact calls.csv row at0076DF68.
- Complete body: `inputs/decompiler/steam/DP_full.asm:924214-924287`; supporting `DP_decompiled.c:509339-509377`; exported body267 bytes/end0072C3EA. Main80 instructions/282 bytes includes caller15 and body267; two independently aligned GNU/PE-matched spans. Main middle44/159, earlier separate edges36/123 overlap that full proof and are not extra coverage credit.
- Replays: `scripts/inspect_phase4_seq56_audio_edges_main.py`, `scripts/inspect_phase4_seq56_audio_full_main.py`; exact receipts under `scratch/phase4_seq56_audio_*`; normalized aggregate `reports/PHASE4_AUDIO_CLEANUP_RAW_CHECKS_2026-10-04.json`.
- Fresh one-assignment Opus middle branch is personally reconciled against these main checks; its report/checker/receipt remain distinct, not substituted for primary verification.

**VERIFIED reconciliation:** old0072C2E0 import-only PLATFORM_WIN32 and0076DF60 generic CRT coarse classifications are narrowed to typed GAME_AUDIO cleanup and DATA_OR_INIT_HELPER/AUDIO adapter roles. Original imported edges and old metadata remain in notes. No prior accepted claim, GOG homology or historical sourcecard is regraded. No new executable contradiction is needed: decompiler bypass removal agrees with raw flags; cleanup/free conflation is explicitly prevented.

**UNKNOWN residuals:** `AUDIO_SHUTDOWN_COORDINATION` now has a positive local cleanup slice but still needs actual dispatch/order, quiescent shared users, matched last use, direct helper/pointee destruction evidence and lazy-root retirement. `AUDIO_NAMED_RECORD_VALUE_LIFETIME` remains independent: no PLSE066 population/type/P4/P14byte/P2C/name/bank value was derived. Reopen this local contract only for conflicting bytes/type/branch evidence or a genuinely expanded boundary; do not repeat these loops or widen into CRT/bank/global scans. A normal global Phase4 frontier reassessment follows this coherent gain; no Phase4 closeout or Phase5.

## Phase5 narrow shared-adapter qualification — 2026-10-04

C0199 independentlyexpands ONLY11-byte0074EE0B through concrete typedmodelbufferarguments: tailJMPaccepted0074E82B preservesstackargument. Thisnarrows formeropaquehelper boundary, nottheoriginalfinite267-byte audioanalysis/researchcredit oractualB13ownership/type/generation/destructor/success/lastuse/dispatch/COMbalance. Originalhistory/text unchanged. findings/boundaries/phase5_model_buffer_packet_retirement.md
