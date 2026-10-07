# Application to Object-Dispatch Boundary

**Addresses:** Steam `00401A70 -> 006C5FF0`; GOG `00401A70 -> 006C5AF0`
**Evidence state:** `VERIFIED` for the direct call/data relationship; subsystem role names remain partly `STRONG_INFERENCE`.

## Direct evidence

Both `FUN_00401A70` bodies increment `DAT_008A971C`, perform timing/critical-section work, then enter a loop. The loop loads the context at `DAT_00BD7670` into `ECX` and calls the build-specific dispatcher in both branch orders. The loop calls a follow-up helper and repeats while `FUN_00409D90`/`FUN_00409DC0` returns nonzero.

Steam assembly shows the two calls at `00401B68` and `00401B9F` targeting `006C5FF0`; GOG shows the same call sites targeting `006C5AF0`. The corresponding decompiler slices are Steam lines 567-630 and GOG lines 565-628.

The scheduler then copies/updates state around `DAT_00BD96B0`, matrix staging at `DAT_00BD96B4` and `DAT_00BD96F4`, and counters/flags at `DAT_01480960`, `DAT_01480964`, and `DAT_01480968`. These are recorded as global candidates, not friendly semantic names.

## Interpretation

The stable call loop and direct context load establish an application-to-object-dispatch boundary. Prior focused research identifies the dispatcher as a generic active-object manager with multi-pass phases; the current Steam body directly exposes phase state writes and virtual calls, while the GOG export has a false `noreturn` split that requires raw assembly for the continuation. The scheduler should therefore be treated as a high-centrality lifecycle root, but not as proof that every called helper is a frame update.

## Open questions

- Exact producer and lifetime of `DAT_00BD7670`.
- Whether `00401A70` is the only recurring top-level frame root.
- Shutdown and loading-mode paths through the same loop.
- Build-specific meaning of the scheduler counters and matrix staging globals.
