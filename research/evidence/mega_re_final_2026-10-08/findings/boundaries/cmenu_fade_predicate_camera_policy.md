# Native UI → CFade predicate → camera policy chain

**Date:** 2026-10-03. **Phase 4 sequence 43. Build:** Steam PC. **Scope:** bounded conditional static reconstruction of the selected CMenu event1 late-state route. **Confidence:** `VERIFIED` for listed primary bytes, raw map cells, selected conditional register/data flows, and endpoint stores; `UNKNOWN` for live reachability, temporal order, identity stability, ownership, lifetime, policy meanings and other stated limits.

## Contract and evidence discipline

The parent handoff selected `NATIVE_UI_FADE_STATE_CAMERA_POLICY` after a global comparison. Existing C0142/C0143 CMenu/model-task/event1 adapter, C0123/C0124 camera identity, C0134/C0135 CFadeManager/retainer organization, C0158/C0159 older CMenu `+28` numeric mechanism, and C0163 request7 interface are reused only at their recorded scope. This report does not re-count those mechanisms, reopen the old UI prefix, or infer a GOG/Xbox counterpart. `00652CE0` is not a function entry in the supplied Steam export; the enclosing exported method is `006516E0`.

Primary sources are the Steam PE bytes and the aligned `DP_full.asm`/`DP_decompiled.c`/`calls.csv` locators. Main replay: `scripts/inspect_phase4_ui_fade_camera_main.py` → `scratch/seq43_ui_fade_camera_main.json` (165 instructions / 664 bytes across six selected windows). Branch A replay: `scratch/seq43_fade3_branch.py` → `scratch/seq43_fade3_branch_receipt.json` (5 parts / 251 checked bytes); main replay of its selected windows: `scratch/seq43_branch_main_verification.py` (55 instructions / 226 bytes for Fade-3 windows). Branch B replay: `scratch/seq43_ui_postpredicate_branch_checker.py` → `scratch/seq43_ui_postpredicate_branch_receipt.json` (132 instructions / 591 bytes; 33 direct call rows). Main independently replayed the branch: `scratch/seq43_branch_main_verification.py` (132 instructions / 591 bytes). Counts overlap and are not additive coverage.

## 1. Typed producer and actual method

The accepted event1 adapter at `006544B6–006544C5` loads the first stack argument, pushes it, sets `ECX=01476978`, and calls `006516E0`. Inside the method, `006516F8` saves `ECX` as `ESI=U` and `00651703` saves the first argument as `EDI=T`; the two domains remain distinct at the selected prefix. The prior `+8=1` route reaches `00652E03`. This is a receiver/argument identity result, not proof that a task is delivered on a recurring cadence.

At `00652E03`, the method loads `dword [U+0x14]`, accepts values through `0x65`, indexes byte table `006540C0`, and jumps through dword table `00654088`. The raw cells are decisive: `00654123=0x0A`, `00654124=0x0B`, `00654125=0x0C`; corresponding target dwords at `006540B0/B4/B8` are `00652ED5`, `00652F11`, and `00653181`. These are new `U+14` states 99/100/101, distinct from the previously accepted CMenu `+28` states 5/6/9/10.

The adjacent raw state 98 cell maps to `00652EC9`, which stores `U+14=99` and jumps common tail `00652B9E`. This establishes a local predecessor locator only; it does not establish who writes or selects state 98 or any live chronology.

## 2. Request-3 producer and immediate Fade state

State 99 begins at `00652ED5`: `ECX=[U+0x72AC]`, loads its vtable, reads virtual slot `+0x74`, and calls it at `00652EE0`. A zero result exits to common tail `00652B9E`. On the selected normal-return path, `00652EEA` calls `00427780`; `00652EF8` loads `ECX=[getter_result+4]`, the selected CFadeManager `+4` retainer input. The caller loads raw float `0077036C=0x41F00000` (`30.0`), pushes control carriers from `EBP` then `EBX`, overwrites the temporary slot with that float, pushes literal request `3`, and calls `00449A00` at `00652F00`. The strongest bounded statement requires a valid incoming Fade object, readable/nonfaulting stack/x87 operations, and preservation through the selected preceding calls; optional manager allocation paths are not expanded.

The selected `00449A00` request-3 path is independently byte-checked. The final control byte is nonzero, request 3 passes the nonzero/`<=8` bound, raw remap cell `00449D97` is `1`, counter-table cell `00449D8C` targets `00449A74` (incrementing `F+0x188`), and request-table cell `00449DAC` targets `00449AC7`. With selected low-byte control `0`, `00449AC7` branches to `00449B41`, copying `F+0x19C/+1A0/+1A4/+1A8` to `F+0x18C/+190/+194/+198`, then `00449B71` increments local `EDI` from request 3 to 4 and joins common code.

The common code at `00449CA7` writes `F+0x164=4`, not 3 or 7. It writes the selected float to `F+0x16C` (the comparison may select the same raw `30.0` constant), computes the operational x87 reciprocal into `F+0x170`, and snapshots the *current* `F+0x178` into `F+0x198`, overwriting the fourth source-quartet copy. The selected request-3 path does not locally initialize or update `F+0x178`. Because local `EDI=4`, `00449D1B` branches past the request-7-only live `014AFFE0` step block and returns `AL=1` at `00449D38` with `RET 0x10` at `00449D3B`. Upper `EAX` is not proven to be one. This is an immediate conditional interface result, not completion, draw success, or proof of later request 7.

## 3. Separate state-100 predicate and conditional state-101 endpoint

State 100 first writes `00BD77B8=0` and `0148C944=0` from accepted-prefix `EBX=0`. It then separately calls `00427780` and loads `F100=[getter_result+4]` at `00652F22`; `00652F25` compares `dword [F100+0x164]` to 7. No `+0x178` or `+0x198` test occurs in this predicate. The separate getter load does not prove `F99=F100`, and the shared offset does not prove same-instance identity or temporal stability.

Equality reaches `0040A320` and tests acquired `CGame+0x834FE0`. The zero arm alone loads a value through `006C5FD0`, adds `0x58`, and calls opaque `005EDB20` with `ECX=0143C078`; the returned/value meaning is not typed here. The nonzero arm bypasses only that zero-arm helper and begins seven locally unguarded virtual `+0x30` calls through `U+72B0/+B4/+B8/+BC/+C0/+C4/+C8`. It then records local zero stores for those seven slots, conditionally processes `U+72D0`, `U+72CC` (including a call with `ECX=&U+9478`), and `U+72AC`, and invokes repeated opaque `00458990` calls. These virtual calls are not classified as destructors/free or harmless operations. The recorded zero stores are instruction-local writes, not a final all-zero invariant because later calls can mutate or repopulate fields.

The raw branch also disproves carrying entry `EBP=1` through this whole body: `00653156` overwrites it with `LEA EBP,[U+0x67B8]`, then uses that same address at `0065315E` and `00653165`. Under readable/live memory, normal returns, and expected nonvolatile-register preservation assumptions, `00653175` stores `U+14=101` and `0065317C` jumps to common tail `00652B99`; it does **not** fall directly into target `00653181`. This is a conditional state transition endpoint, not proof of immediate state-101 execution, re-entry or recurrence.

## 4. Later state-101 camera endpoint

The separately mapped later target `00653181` begins by clearing the two state globals again, invokes `T` virtual slot `+0x30`, writes `00BD7790+0x248=1`, and passes through opaque calls `0040EDE0`, `0044E640`, `00405360`, and conditional `006B29E0(0x5A/0x5B)`. Their receiver/value semantics and ownership remain untyped.

The selected camera endpoint uses cached root `00BE1EA4`; the lazy `0x1AC` allocation/vtable publication path is reused from the accepted CCamera evidence. The camera table is raw-type-proven as `CSingleton<CCamera>` at `007721BC`. At `00653204`, the code clears bit `0x1` at `[CCamera+0x11C]`; at `0065320A`, it reloads `00BE1EA4`; at `0065320F`, it sets bit `0x4` at the reloaded camera `+0x11C`. The reload is explicit. These are conditional mask writes, not names for friendly pause/resume, mode, physical camera behavior, current-camera identity, lifetime safety or cadence. The bounded endpoint stops before later effects.

## 5. Architectural chain and limits

**Verified conditional chain:** typed CMenu event1 adapter → `U+14` raw states 99/100/101 → state-99 virtual-74 gate → request 3 into separately acquired CFadeManager `+4` → immediate Fade state/data `F+164=4` and `+198` snapshot of existing `+178` → separately acquired `F+164==7` predicate → opaque conditional child/control path → conditional state-101 store/common-tail → later state-101 dispatch → camera `+11C` bit-1 clear, root reload, bit-4 set.

The source distinguishes request 3's immediate state 4 from the later predicate value 7. It does **not** identify the producer that later writes 7, establish request3→7 causality, prove F99/F100 equality, prove request3 reaches state 101 in the same invocation, or assign completion semantics to either 7 or 101. The first untyped/opaque joins are `[U+72AC].virtual74` result, CGame/child/helper calls and the 28-call local control/cleanup-looking region. No destructor/free meaning is assigned to virtual `+0x30` calls. U/T/F/camera domains remain distinct; shared manager roots/offsets do not collapse them.

No GOG/Xbox transfer, runtime cadence, task lifetime, draw/GPU success, full UI/Fade/camera taxonomy, allocator safety, or Phase 5 transition is claimed. B0001–B0010 and all prior provenance, recovery, build, and runtime limits remain unchanged.
