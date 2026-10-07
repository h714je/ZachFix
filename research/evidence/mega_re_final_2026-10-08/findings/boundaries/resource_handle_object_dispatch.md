# CRdData Resource Handle to Object Dispatch

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for the manager-field handoff and virtual dispatch.

Steam `006118C0` demonstrates a typed consumer of the CRdData manager table:

- It resolves an indexed resource through `006B2BE0(index, 1)`, which returns manager record field `+0x18` after validity checking.
- It passes that returned handle/state to an object virtual slot `+0x54` with state/control arguments.
- It then switches animation/resource state based on the selected index and may initialize or release associated object phases.

This confirms manager record `+0x18` is a live resource-state/handle consumed by object dispatch, not merely an archive key. It complements the callback descriptor map and provides a concrete CRdData -> actor/object boundary. The object class and exact meaning of the indexed resource table remain open.

Evidence: `inputs/decompiler/steam/DP_decompiled.c:336756-336856`; `findings/formats/resource_descriptor_fields.md`.
