# Object Dispatcher to PhysX Boundary

**Address:** Steam `006C5FF0` (GOG raw continuation is build-split around `006C5AF0`/`006C6B70`)
**Evidence state:** `VERIFIED` for Steam phase ordering; GOG counterpart is `STRONG_INFERENCE` pending a repaired function-boundary model.

## Direct evidence

The Steam dispatcher stores phase values in `this+0x18E0` and iterates active objects through a manager-owned list. The phase sequence visible in the decompiler and raw assembly includes:

- state `0x0E`: calls `FUN_006EB3C0` with the elapsed scalar after a preceding object phase;
- state `0x07`: calls `FUN_0040B750` and `FUN_0040B780`, the completion/fetch-side bridge identified by the current static map;
- state `0x08`: iterates eligible objects and invokes virtual slot `+0x1C` with event value `6`;
- state `0x0B`: calls `FUN_006EB400` before later object phases.

The ordering is not an inference from function names: the phase writes and calls are adjacent in Steam `DP_decompiled.c` lines 441181-441204 and the corresponding assembly.

The GOG raw assembly at `006C5AF0` sets phase `1` and calls `006C5AD0`, which the export incorrectly marks `noreturn`; `006C5AD0` actually returns at `006C5AE8`, after which the raw listing continues through `006C6B70` and later code. This explains why the visible GOG decompiler body stops early and why direct GOG call-edge counts underreport the full dispatcher continuation.

## Interpretation

The direct Steam ordering establishes a cross-subsystem boundary from active-object scheduling to PhysX submission/completion and post-fetch object callbacks. It disproves the older State-5-submit and State-11-fetch interpretations retained in the claim ledger. It does not establish the exact internal PhysX scene policy, accumulated-debt behavior, or runtime cadence.

## Open questions

- Reconstruct the full GOG split function boundary and normalize it against Steam.
- Identify the manager/object list roots and class families receiving each virtual phase.
- Resolve the exact role of state `0x0B` and the second PhysX-facing call.
- Obtain runtime traces for CCT and vehicle actor writes relative to submit/fetch.

## Phase 3 narrow attribution correction — 2026-10-03

The newly selected primary receiver/lifecycle check supplied a genuine raw contradiction to the older line assigning both `0040B750` and `0040B780` to state `0x07`. **DISPROVEN C0122 at that exact Steam scope:** `006C6CB4 -> 0040B750` occurs before `006C6CC2` writes phase 7; `006C6CD4 -> 0040B780` occurs after it. Correct order is phase14 -> acquired CPhysicsCore producer -> synchronous first-half bridge -> phase7 -> second-half bridge -> phase8/event6. See `findings/boundaries/cphysicscore_lifecycle_root.md` and its independently checked byte receipt. The original wording above is retained as superseded history, not current attribution. Broad C0006 ordering and the State5/State11 disprovals remain intact. Accepted Phase2 manager/object/interface scope and closeout/audit are not regraded; GOG export-split limits remain, with no transferred marker correction.
