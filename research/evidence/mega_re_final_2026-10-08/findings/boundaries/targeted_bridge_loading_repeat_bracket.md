# TBC001 — CMap polling bracket to frame-repeat state

**Result: CONNECTED; evidence state: VERIFIED at selected static scope.** This closes the missing producer relation, not eventual progress or successful thread activation.

## Anchors and missing edge

Known typed CMap polling and known frame getter/conditional backedge were separate regions. The new relation is `005DA8E0 → start/repeat setter → CMap polling → stop/repeat clear → known frame read` (GOG independently `005DA9B0`). Prior C0117/A01 covered the getter/frame branch; the corrected Phase6 world result covered typed polling. Neither supplied this intervening producer bracket.

## Primary connection

- Steam `005DA8E0` clears its incoming receiver+4, calls `00409D20`, then repeatedly calls `005D4DE0` with independently reacquired static CMap `013936F0`. A nonzero return takes the tail to `00409D50`. Zero keeps polling; eventual nonzero is UNKNOWN.
- `00409D20` tests byte `00886FF8`. Only its zero arm writes1 at `00409D3C` **before** offering member `00886FE0` and arguments1/FFFFFFFF/10000/DrawLoadingThread to `00712A20`.
- `00409D50` acts only when that byte is nonzero: `00712B90(1)` request, then byte0 and `00886FFC=FFFFFFFF`, followed by `00712BC0` and tail `00712BE0` requests on the same address. The clear is not a successful join/quiescence receipt.
- Existing `00409DC0` returns that same byte; tick `00401BAF/00401BB6` uses its nonzero result to repeat the conditional dispatcher/context body.
- Own-PE initial data at `00886FE0` contains table `0076F65C`; independent RTTI names **CDrawLoadingThread**. This is initial static type evidence, not newly recovered construction or current-vptr proof. The offered literal start name is separately consistent.
- GOG independently reproduces these selected mechanics with setter `00409CF0`, clear `00409D20`, getter `00409D90`, bracket `005DA9B0` and initial table `0076F64C`.

## Evidence and ceiling

`audit/targeted_bridges_2026-10-07/REPEAT_PRIMARY.json` and `SCHEDULER_QUALIFIERS_PRIMARY.json`: original ASM/own-PE/accepted fragment/type/data replay. Existing typed CMap/frame receipts are reused. No generic thread internals are newly certified. Current member state, admitted calls, reentrancy, thread activation, temporal overlap, interleaving, eventual exit, readiness, lifetime and final borrower remain UNKNOWN. A set byte before a request cannot prove the worker is running; a later clear cannot prove it stopped safely.
