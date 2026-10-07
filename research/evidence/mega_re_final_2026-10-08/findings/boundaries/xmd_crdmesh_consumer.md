# XMD / CRdMesh Consumer

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for the first CRdMesh-side animation consumer.

## Consumer

Steam `006BE460` is a direct consumer of the XMD animation state produced by `0070C020`:

- Calls `0070C020(*(this+0x1E4))`, populating an animation/node record array rooted at the owning object field `+0x1E4`.
- Copies three values from `00402D70` into owner fields `+0x100/+0x104/+0x108`, establishing a nearby model/object transform or render-context handoff.
- Iterates the global node-name table through `00417CD0/00417CA0`.
- Assigns per-node type codes in each animation record at `record+0x70`, with records spaced `0xA0` bytes apart. Prefixes and names include `M_`, `N_`, `PM_`, `NTS_`, `PM_WIPER_L`, `PM_WIPER_R`, `PM_L_WINDOW`, and `PM_R_WINDOW`; the mapping distinguishes node categories such as general nodes, mesh/attachment nodes, wipers, and windows.

This is a concrete XMD -> animation array -> gameplay/render node classification boundary. It explains a CRdMesh consumer of the XMD-derived transforms but does not yet establish the complete owner class of `006BE460`, its vtable, or the final D3D9 draw path. A higher-level paired consumer, `006C1430`, passes owner animation/transform state into `0070C130`; see `findings/boundaries/xmd_actor_animation.md`.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:436925-437003`
- `inputs/decompiler/steam/calls.csv:74413`
- `findings/boundaries/xmd_animation.md`
- `ledgers/CLASS_LEDGER.csv` CRdMesh rows
