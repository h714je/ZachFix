# Save writer / live staging / operation-6 physical write boundary

**Date:** 2026-10-03. **Phase 4 sequence 47. Build:** Steam PC. **Scope:** bounded conditional static reconstruction of a typed save-builder and write-request path. **Confidence:** `VERIFIED` for listed bytes, calls, receiver continuity, pointers/counts and operation dispatch; `UNKNOWN` for activation, admission, staging generation/chronology, disk success, schema, lifetime and runtime.

## Contract and evidence

Sequence46 selected this source because it provides a new typed live-CGame → staging → operation-6 request → physical writer discriminator outside accepted C0146-C0148 read/header/direct-live scopes. C0125/C0126 CGame identity and C0146-C0148 CSaveData request/operation mechanics are reused only at their accepted scope. This report does not reconstruct the 28-record schema, full image, worker cadence, GOG counterpart or successful disk transaction.

Main replay `scripts/inspect_phase4_save_writer_main.py` → `scratch/seq47_save_main_receipt.json` passed 208 instructions / 713 bytes across thirteen aligned Steam windows. The main check includes the typed CPreserve call, operation-4 receiver, builder staging initialization/copy, same-context dispatch, operation-6 request, accepted operation-table writer branch and imported `WriteFile` argument boundary. Independent builder replay `scratch/seq47_save_builder_branch.py`/`.json` passed72 instructions/263 bytes across6 parts; writer replay `scratch/seq47_save_writer_branch.py`/`.json` passed101/351 across7 windows,3 rawtables/5 calls. Main personallyread the decisive ASM/rawdispatch cells, replayed both stablecheckers locally, and verified208/713 selectedbytes beforepromotion. Counts overlap, notadditive.

## 1. Typed CPreserve entry and live builder

At `004394CD–004394D2`, a selected CPreserve root path sets `ECX=00BE5970` and calls `00449620`. The method retains CPreserve in ESI, sets byte `CPreserve+0x160=4`, forms `ECX=CPreserve+0x28C`, and calls `006ACAB0` at `00449635`. Receiver continuity is concrete; operation-4 meaning and outer activation are not assigned beyond the visible state write.

`006ACAB0` receives the embedded save context in EBP. It initializes shared staging `00BE5EF0` with zero/count `0x7A2620`, writes local embedded fields including `+4=1`, `+8=0`, `+0x18=0`, `+0x2D=1`, acquires CGame via `0040A320`, calls opaque `004524C0`, then copies CGame live source `+0x8C568` with count `0x45CC0` to `00BE6010`. A repeated loop copies the bounded record-source region to `00C2BCD0` at stride `0x45CC0`; the mechanical bound is 27 iterations, not a promoted 28-record schema. The repeated calls reacquire CGame, so this is not a claim that all copied regions form one stable image or that a later writer consumes this generation.

The first opaque synchronization/policy edge is `006ACB15→004524C0`; its effects, synchronization role, and relation to staging generation remain unknown. No builder success/error result is assigned.

## 2. Same-context dispatch and operation-6 request

A later selected CPreserve path at `00468A1A–00468A25` passes the same embedded context `CPreserve+0x28C` to `006ACF30`. `006ACF30` preserves that context as ESI, invokes `006ADC70` on `context+0x3C`, then gates through `006AC4B0`. In the selected state-table route, it reaches `006AC720` at `006AD096`.

`006AC720` checks embedded `+0x120==4` and `+0x148==0x4359`, then uses the acquired CSaveData root `00BD9FF8` and calls accepted `004094C0` with literal operation `6`, buffer `00BE5EF0`, and count `0x7A2620`. It writes embedded byte `+0x2D=2` after the request call. This is a typed operation/value interface, not proof that CSaveData admission succeeded or that the buffer is the staging generation produced by the earlier builder.

The crucial state distinction is preserved: the builder initializes embedded `+4=1`, while the selected `006ACF30` state route that invokes `006AC720` is reached through a separate state transition requiring `+4=2` in the raw dispatch table. The same embedded pointer and staging address do not prove chronology or intervening activation. The exact producers of `+4=2`, `+0x120=4`, `+0x148=0x4359`, and the staging last-writer are unvisited.

## 3. CSaveData operation dispatch and physical writer

Accepted `004094C0` stores the supplied operation/buffer/count into the established CSaveData manager request fields at `+0x3C/+0x40/+0x44`; admission/worker execution remains at C0146-C0148 scope. In `00409790`, the manager is retained in ESI. The raw operation table at `00409868` maps the selected operation 6 arm to `0040982B`; that arm loads `EBX=[manager+0x44]` and `EDI=[manager+0x40]`, then calls `00408BD0` at `00409831`.

`00408BD0` constructs/opens `savedata/dp.sav` through the existing Win32 file boundary and, on a non-invalid handle path, pushes the caller-provided handle/EDI buffer/EBX count into the imported `WriteFile` call at `00408C39`. The path and call/value boundary are primary-backed. Imported `WriteFile` return/error handling, transfer success, format validity, close/rollback policy and timing are not promoted. `00409790`’s protocol return is not treated as a `WriteFile` Boolean or byte-count result.

## 4. Architectural chain and limits

**Verified conditional chain:** typed CPreserve root → embedded CAutoSave context operation4 → live CGame `+0x8C568` copy to shared staging → later same-context state dispatch → CSaveData operation6 with staging buffer/count → operation-table write arm → `savedata/dp.sav` / imported `WriteFile` argument boundary.

The chain is conditional and crosses a missing temporal/admission edge. It does not establish that the builder’s staging bytes remain unchanged until operation6, that operation6 is admitted/executed, that `WriteFile` succeeds, or that the saved image is valid/complete. No full record/schema field meanings, 28-record naming, load→commit chronology, outer callback/thread activation, object lifetime/free, GOG/Xbox correspondence or runtime cadence is claimed.

## Primary references

- CPreserve/operation4: `inputs/decompiler/steam/DP_full.asm:66438-66442`, `:84549-84565`.
- Builder/staging: `DP_full.asm:762169-762228`; calls at `006ACB15`, `006ACB2F`; `DP_decompiled.c:424415-424471`.
- Same-context dispatch: `DP_full.asm:120593-120608`; `:762489-762597`; `DP_decompiled.c:78094-78096`, `424776-424781`.
- Operation6 request: `DP_full.asm:761935-761968`; `DP_decompiled.c:424274-424299`.
- Accepted request/operation writer: `DP_full.asm:10278-10333`; `:9428-9463`; calls/import rows for `004094C0`, `00408BD0`, `KERNEL32.DLL::WriteFile`. Existing C0146-C0148 finding remains authoritative for its accepted read/header/direct-live scope.
- Main raw replay and branch receipts are derived checks; raw PE/ASM/call evidence outranks decompiler shapes and does not establish runtime execution.

## Checker history

Builder branch initial contiguous-window assertion failed at the omitted alignment gap006ACB3D..006ACB40; selected partitions were split. A transient branch-address assertion used006ACAEC inside an immediate instead of006ACAEE; a root-immediate read used MOV opcode0076ACF0 instead of0076ACF1. Metadata key case was normalized. These mechanical checker errors and the actualfinalpass are preserved in branch JSON/markdown; supplied evidence and semantic conclusions unchanged. Main’s first expanded count note203 was corrected to actual208 instructions/713bytes beforepromotion.
