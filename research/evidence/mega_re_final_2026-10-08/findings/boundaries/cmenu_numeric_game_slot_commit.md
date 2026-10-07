# CMenu numeric policy → selected value → CGame indexed mutation

**2026-10-03; Phase4 NATIVE_UI_STATE_COMMIT; ADVANCE.** Steam C0158/C0159 / BND-137/138. Conditional static mechanism, not complete UI/inventory, friendly item/discard semantics or observed live execution.

## Contract / exact reuse

Analyze new00654A80 policy/commit slices outside C0142/C0143's accepted input/root/task prefix: numeric40 producer and28+4 transition, selected float gate, ID80 latch, direct004FDAF0(ID,999). Reuse only accepted CMenu01476978/type/same-receiver seam and C0125 CGame getter/type. Independent Opus consumer branch was read, replayed and personally assembly-checked. Exclude root/task/callback/prefix, CMessage/CEvent/availability, alternate0064FA30 application, allstates/list grammar, actor retirement, auxiliary leaf, GOG and runtime cadence.

The positive concrete projection has explicit conditions: later selected dispatch9/10, ordered progress endpoints9520=9530=1, policy40=1, M20=1, signed index(M30+M38)>=signed G837474, consumer G98E48!=85, stable cached root/shared scalar and normal return of opaque earlier helpers. These are code conditions, **not observed initial state or proven live availability**. Actual mode/index/count population is the first unresolved producer edge. No skipped actor/helper branch is pronounced safe.

## VERIFIED typed numeric producer / transition

- Accepted00652E2A moves ECX=ESI and calls00654A80. New prologue saves same CMenu inESI, loadsEDI=1, and dispatch clearsEBX=0. Raw table00656268/map00656284 route numeric **5/6→0065559C**, **9/10→006558D3**. No full mode labels.
- Available reset slice00654E9D writes M40=0 andM2C=M28+4 plus selected float rates/control fields. Preceding actor/guard/setup policy remains unvisited; this is not universal menu initialization.
- In5/6, two progress fields9520/9530 use shared scalar014AFFE0 and rates9524/9534 with ordered comparisons/clamps. Exact selected endpoint/interior examples were executed against the checked x87/branch stream; both endpoints1 reach input006556A5, interior values defer, and zero9530 restoresM28 fromM2C. No NaN, overflow, full extended-precision, clock units or live frequency guarantee.
- First CInput query's continuing arm `(byte77B4,1,dwordA00218)` produces **M28+=4** at006556CD andM94E4=30, then calls an opaque acquired-CSound helper and jumps to common tail006560F5 after policy normalization. It does **not directly fall through into the new9/10 dispatch**. Subsequent/re-entrant evaluation and cadence remain UNKNOWN.
- A second query can decrement the future targetM2C by4 and set9534=-0.1. Two other queries `(byte77B4,2,4001/8002)` increment M40 byEDI=1, then raw00655770–7D implements signed remainder-style normalization modulo2. Starting at nonnegative0/1, this toggles0↔1; it is not an unconditional unsigned boolean for all inputs. Named buttons/channel/full input algorithm remain UNKNOWN.

## VERIFIED selected commit gate and actual value

In9/10, new006558D3–006559CC updates/compares9530 and9520. The actual checked ordered delta0 examples distinguish interior/zero routes from both1→JP00655B5E. Zero routes include other deferred state/cleanup code and are outside acceptance.

At00655B5E, acquired CGame result is supplied to004FECF0 alongside three stack arguments: sign-extended category byte008C5728[M20], indexM30+M38, optional output pointerEBX=0. Raw first four category bytes are **07 01 02 03**; conditionalM20=1 means category1. Caller dereferences returned EAX and latches the same dword inM80 at00655B84.

004FECF0 **ignores incoming ECX** and reacquires CGame internally. Category1 and signedindex>=signed[G+837474] write **0x55 / decimal85** to mutable global008A9514 and return its address (RET C). The literal is produced by this branch, not inferred from the cell's file initial value. No optional flag is written when its third argument is0. Ordinary alternative returns `G+836270+4*(128*category+index)` with wrapping address arithmetic and no local full bounds/domain check; pointed value population remains UNKNOWN.

Selected policy40==1 excludes raw special IDs21/22/23/120/121 and signed34..66. Raw85 maps to the permitted arm. Policy40==0's application path is explicitly excluded; negative/superlarge IDs passing these local gates do not establish a safe consumer domain.

## VERIFIED UI→game slot boundary and machine arithmetic

At00655C28, accepted0040A320 returns actual CGame;00655C33 pushes999,00655C38 pushes M80,00655C39 switches ECX=EAX and00655C3B calls004FDAF0 (RET8). Following UI code does not inspect returned EAX as an operation-success status; it later invokes opaque same-CMenu00650C60 and conditionally writes rate9524=-0.1. Full refresh/reset/transaction is not proven.

004FDAF0 retains incoming receiver R inEBX for a different auxiliary arm. Every+98270/+98E48 access instead uses its actual internally reacquired CGame result G. Do not conflate R with every G for all callers.

For the explicitly conditional literal85 projection:
1. G98E48!=85 bypasses the current-handle/actor sidearm.
2.004FDBAC performs **32-bit SUB999** at **G+98270+4*85=G+983C4**.
3. Unsigned `(85-2)>40hex` bypasses the auxiliary loop.
4.004FDBDA/E2 signed-test the freshly read slot; negative bits trigger004FDBE9 zero write;004FDBF9 returns a fresh getter-read slot.

This is **not unrestricted mathematical saturating subtraction**. Initial bits80000000 minus999 wrap to positive7FFFFC19 and are not clamped. With a separately established nonnegative initial signed value and no intervening writes, the selected operation is max(old-999,0), but no initial-value invariant is proven here. There is also no demonstrated999 upper bound permitting the friendly name “remove all.”

### Existing record-copy relation — reused scope, not new save transaction

Slot983C4 is displacementBE5C inside accepted C0125/C0147 record0 envelope `G+8C568 .. G+D2228` (45CC0 bytes). Under the **already accepted selected direct staging→live copy**, the counterpart byte offset is `00BE6010+BE5C=00BF1E6C`. This is an exact address/known-copy containment relationship, not a new disk writer, schema/field name, save timing, persistence policy or atomic UI→save transaction. The previous copy proof is reused, not reopened or counted again.

## First missing edges / bypassed branches

**UNKNOWN selected producer:** actual M20/index/G837474 threshold/list population and live availability of the conditional85 choice. Ordinary list values, friendly categories/IDs/slot meaning and full cardinality remain unknown. Shared global008A9514 value stability under external writes and cached-root lifetime are not established.

**UNKNOWN auxiliary semantics:** admitted IDs2..20/24..33 can enter the unsigned2..66/signedquantity>0 arm and execute00451740(ID,0)999 times **after subtraction/before clamp**. Its leaf body is not checked/promoted by this batch. IfID==G98E48, the first current-handle helper result004FDB25 is dereferenced at+864 without a local null test; repeated results feed virtual30, pointer-relative864 clear and8DC zero. Those are **not CGame fields** or safe retirement/free proof. The concrete85 projection requires inequality and bypasses this arm.

**UNKNOWN postconsumer edge:**00650C60's selected value/refresh responsibilities; alternate policy0, other menu states, actor class/lifetime, actual finalization/task recurrence, GOG and runtime cadence. Unvisited is not bounded. Original CMessage/CEvent/availability/source bounds and B0001–B0010 remain unchanged.

## Primary evidence / actual personal verification

- Producer/gates/commit: `inputs/decompiler/steam/DP_full.asm:665652-665693`, `:665951-665965`, `:666475-666622`, `:666718-666787`, `:666906-666986`; exact VAs/byte windows in main receipt. C selected cases `inputs/decompiler/steam/DP_decompiled.c:371434-371519`, `:371574-371610`, `:371658-371738` checked against assembly, not semantic authority by themselves.
- Value helper `inputs/decompiler/steam/DP_full.asm:292493-292521`, C`:174698-174727`; consumer ASM`:291102-291170`, C`:173507-173551`. Exact source/call/export/ABI locators in `scratch/phase4_ui_game_consumer_branch.md` and its receipt.
- Main `scripts/inspect_phase4_ui_menu_main.py` / `scratch/phase4_ui_menu_main_receipt.json`: **357 instructions/1272 bytes**,8 continuous aligned windows,33 operand/14 call checks, raw typed/state/ID/category tables and **9 exact ordered x87 endpoint/interior examples at delta0**. This small interpreter verifies selected branch interpretation, not a runtime trace or full float algorithm.
- Independent Opus consumer `scripts/inspect_phase4_ui_game_consumer_branch.py`, personally rerun without overwriting its original receipt in `scratch/phase4_ui_game_consumer_main_replay.json`: **118 instructions/444 bytes**,4 independently objdump-aligned windows,65 operands/19 calls/10 conditional branches/3 RET sites, zero export gaps. Main personally read both compact bodies and reconciled ABI, threshold, literal85, subtraction/clamp, loop/actor boundaries and wrapped-value limitation.
- Prior static types/copy: `findings/boundaries/cmenu_organizing_task_use.md`, `cgame_record_reconstruction_root.md`, `save_disk_staging_record_chain.md` at their exact accepted scopes.

Counts overlap, not additive coverage. No primary source/evidence/production modifications, new GOG/Xbox homology, item/inventory/actor lifetime or save transaction claim.

## Architectural gain

An independent UI→shared-game-state mechanism now has a concrete numeric producer, policy/float gates, an actual literal-valued pointer alternative, and a typed indexed state mutation, with receiver/ABI and side-effect boundaries separated. This advances Phase4 beyond root/task acquisition without claiming full native-UI algorithms. `NATIVE_UI_VALUE_POPULATION_AND_AUXILIARY_POLICY` carries the remaining typed population/alternate/auxiliary/refresh edges. Reassess global independent frontiers after audio and UI gains; do not remain in UI because its large state body is available. No Phase4 closeout or Phase5.
