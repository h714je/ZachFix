# XWP CEffect Callback Lifecycle

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired CEffect event lifecycle; XWP-specific callback configuration remains `UNKNOWN`.

XWP setup assigns paired callback Steam `00676520` / GOG `00676470` to each created CEffect. The callback is a broad CEffect event dispatcher rather than an XWP parser:

- event `0` initializes CEffect runtime state, sets event/type field `+0x30` to `0x19`, initializes tracked values, and invokes effect activation/setup helpers;
- event `1` performs active update gating against engine/world state, changes effect flags and distance/position-derived state, can call update helper `00675540`, and releases a pending request-like value at `+0x330` when applicable;
- later event codes alter effect/world flags or perform further state transitions.

This verifies XWP-created CEffect -> generic CEffect lifecycle callback behavior. The package fields copied into CEffect and their effect-specific meaning remain `UNKNOWN`; the callback does not establish XWP binary grammar.
