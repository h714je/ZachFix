# CObjectCar producer → installed callback → selected numeric control endpoint

**2026-10-03; Phase4 sequence39; ADVANCE.** Steam-only selected static/conditional mechanism. C0160/C0161; BND-139–141. No Phase4 closeout or Phase5.

## Contract, reuse and review

`scratch/phase4_vehicle_seq38_contract.md` includes the sequence38 live-control correction and resumed sequence39 scope. Reuse C0127/C0128/H0063 only for distinct CCar/CObjectCar identity, accepted constructor/table/allocation, and `006BAB80` publication/initial-event mechanics. The new custom producer/install/event1/selected-state chain is outside the accepted own-event5 scope. No CCar root/task, registration, event5, wheel/SDK, later-state or GOG recensus.

Two independent Opus branches checked the necessary side effect and selected state endpoint. Main read their reports/checkers and decisive assembly, replayed both into **separate main receipts**, and personally checked masks, raw cells, arguments, float, receiver and state stores before canonical promotion. Original branch receipts remain preserved. Raw/supplied evidence is unchanged.

## Producer and actual installation — VERIFIED selected scope

`005ED660` first conditionally asks the existing `008C11C4` handle result through virtual+30, then allocates **0x2008** and passes the successful allocation to accepted **00541AA0/CObjectCar**. Constructor result EAX is retained in **ESI**; incoming outer ECX is separately saved at stack1C/later EBP. No identified CCar-to-actor owner follows from that outer argument.

The producer writes actor words **+2C=0xE, +2E=7, +30=0x53/decimal83**. It passes the actor through registration and opaque setup, explicitly forwarding ESI to `006BE6E0` and conditionally `006C30D0`; visible +1332=1/+424=0/+474/+4F8 copies also precede installation. Resource/handle/game lookups, vector math and outer+28000-child setup are required intervening operations, **not a reconstructed full setup algorithm or safe lifetime proof**. No direct replacement of ESI occurs after acquisition in the checked producer body; pointed-memory mutation/reentry/free by opaque callees remains UNKNOWN.

At **005ED84A–005ED853**, arguments are **callback005C92C0, auxiliary0**, with **ECX=the saved actor ESI**, to reused `006BAB80`. That helper publishes **actor+44** and synchronously sends **event0** via `00402870`. This is an actual installed callback, **not event1 activation**. The producer's normal return returns ESI (005ED9BA/RET8). Allocation failure sets ESI0 and subsequent stores dereference it; no failure-safety claim. Incoming callsite **00529F73** has blank exported caller identity: do not invent an enclosing function/owner or trust C's incomplete prototype.

## Callback/state route — VERIFIED conditional mechanics

The callback loads first stack argument into ESI, event into EBX, third argument into EBP; the first dispatch uses **low8(event)**. Raw event1 cell/index1/table005C9B8C maps to **005C93D2**. Nonzero **[013936F8]&0x268** jumps directly to the epilogue and skips the selected side effect/handler.

Mask-clear event1 passes ESI as an explicit stack actor argument to **005C61B0**. **After return**, `005C93E8` reloads **word[actor+30]**, subtracts9 and uses an **unsigned** range check/map. Only the post-call selector **0x53** yields raw index13/pointer005C9C70→**005C9494**, which sets **ECX=ESI** and calls **00543160**. The earlier producer selector does not prove the post-call selector survives.

**DISPROVEN historical locator numeral:** sequence37's0x35/decimal53 interpretation is contradicted by raw `B8 53 00 00 00` at005ED6E0. Decimal53 maps index99/default005C9B15, not the vehicle handler. Sequence38 repaired live controls; historical originals remain unchanged. See `reports/PHASE4_VEHICLE_SELECTOR_CONTROL_CORRECTION_2026-10-03.md`.

## Necessary side effect / first opaque edge

**VERIFIED:** complete005C61B0 retains the actor argument in ESI. Its first accessor `004607A0` is a checked26-byte read-only leaf. Direct mutation is skipped if its signed return is negative, actor byte+416==FF, or sign-extended byte+12 equals dword+444. Otherwise the four direct actor stores clear/set **+DC mask0x8 (bit3)** and rewrite **+D8** snapshots; no direct +30/+1FD4/+1FD8 store occurs in this108-byte body.

The selected path invokes PE-imported **InterlockedIncrement(&0139368C)**, then **005C61FA→00712D50** with explicit stack arguments **literal0139368C,FFFFFFFF**. ECX is **not reloaded after the import**, so the inferred thiscall prototype does not identify an actor receiver. The checked prefix's first nested outgoing edge is **00712D54→00712D80**.

**UNKNOWN / bounded-analysis stop:** opaque effects, memory/state preservation and lifetime across that call. The returned-value signed<=3 test conditionally ORs mask0x8; it is not an established actor count/scheduler budget. No synchronization/helper implementation expansion. A later route can be stated **conditionally on normal return, live actor and the reloaded selector**, not as unconditional preservation.

## First selected state endpoint — VERIFIED conditional writes

At00543171 the handler retains incoming ECX in ESI. **byte+1FD4 must be nonzero**; zero branches to00543BCC (not an immediate return). **dword+1FD8** is unsigned-bounded0..6. Raw table **00544248/index1→0054325D** selects this arm.

Conditional on those post-side-effect inputs, the same actor receives:

| Field | Exact selected effect |
|---|---|
| dword+D8 | OR0x8000, later explicit post-OR snapshot rewrite |
| float+11D8 | **-1000.0f**, raw00776E28 bitsC47A0000/x87 store |
| dword+24 | OR0x40 |
| dword+DC | explicit earlier snapshot rewrite, then OR0x08000000 |
| dwords+538/+53C/+540/+550/+554/+558 | zero, two three-dword layout groups only |
| byte+438 | 1 |
| dword+1FD8 | **2**, selected numeric1→2 transition |

Byte+1FD4 is not written by this arm. The only call is the exported cookie helper; RET005432E0 ends the arm, with **no state2 fallthrough or SDK call**. C's integer-looking assignment for+11D8 and apparent self-assignments do not override x87/store evidence.

**UNKNOWN:** actual nonzero1FD4/state1 producer, field purpose/units, selected mode/event reach, actor ownership/free and physical/control API semantics. After handler return, the callback jumps to005C9B15 and event1 reaches same-actor virtual **+A4(full event,argument)**. That unvisited own-event1 post-effect may change state; endpoint stores are **not the final callback state**. Do not reopen accepted own-event5 merely because this table is available.

## Responsibilities and first missing edges

**VERIFIED selected architectural responsibility:** the new custom producer supplies actor identity and callback publication; callback supplies world-mask/event/post-selector policy; common side helper mutates actor control bits and invokes an opaque global-pointer operation; typed actor handler owns the first numeric control-state writes. CCar is a separate organizing root, not proven owner.

**UNKNOWN first inputs/effects:** normal successful producer/setup and live pointer; actual event1 delivery; side-helper preservation; post-call selector0x53; nonzero1FD4/state1 activation; own+A4 event1 post-effect; outer retainers/retirement/free. These have precise new discriminators in `VEHICLE_CONTROL_ACTIVATION_AND_POSTEFFECTS` and surviving `VEHICLE_LIFECYCLE_COORDINATION`. No runtime traces/GOG/Xbox/higher states or complete vehicle protocol are established.

## Reproduction / primary references

- `python3 scripts/inspect_phase4_vehicle_main.py`: **303instructions/1063bytes/8continuous windows,41anchors/21calls**, raw event/selector/type cells and blank incoming-caller check. Includes40bytes accepted-setter reuse and full881-byte producer check, **not full producer semantics**.
- Side branch main replay: `PYTHONPATH=scripts python3 -B` import `inspect_phase4_vehicle_sideeffect_branch`, redirect `OUTPUT` to `scratch/phase4_vehicle_sideeffect_main_replay.json`, call `main()`: **60instructions/226bytes/5directtargets/4actorstores/2selectedmappings/1import-name**.
- State branch main replay analogously redirects `inspect_phase4_vehicle_state_branch.OUTPUT`: **384assertions,52supplied/52raw-decoded instructions,217bytes each,1table cell+4scalar bytes,0selectedASM gaps**. Same bytes, not additive coverage. Original branch reports/receipts are `scratch/phase4_vehicle_{sideeffect,state}_branch.*`.
- `inputs/decompiler/steam/DP_full.asm:553631-553774` producer-to-install; `:553807-553878` selected later return/outer separation; `:513036-513047`,`:513121-513131`,`:513167-513169`,`:513602-513634` callback ABI/map/gate/route/post-effect; `:780947-780963` reused setter.
- Side helper/accessor/opaque-prefix: same ASM`:509414-509440`,`:111078-111085`,`:893285-893288`; C`:287651-287672`,`:74453-74463`,`:494103-494111`.
- State entry/arm: ASM`:363869-363884`,`:363947-363972`,`:365084-365092`; C`:203847-203882`. Producer C`:314866-314982`; selected callback C`:289908-289965`.
- Raw evidence: `inputs/binaries/steam/DP_STEAM.exe`; functions/calls/xrefs/imports/decompile_index at exact rows retained in receipts. State export4312aggregate bytes versus4321bounding interval retains9-byte discrepancy outside selected spans; no full contiguous-body assertion.

**Coarse correction:** imported Win32/D3DX calls do not identify game-owned producer/actor dispatch as platform/renderer ownership. Four existing selected function rows are reclassified with exact primary scope; their other calls/full algorithms remain unvisited. All exported identities, prior claims, homology and supplied evidence are conserved. Counts/labels are not semantic closure.
