# XMD Resource to Actor Lifecycle Boundary

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for the call ordering and resource-validity gate; actor class semantics remain conservative.

Steam `00461060` is a direct higher-level consumer of the XMD/animation path:

- Checks an indexed resource/object record through `006B2F30` and requires its validity marker to be `Y`.
- Runs an actor/resource readiness helper `0044B2A0`.
- On success, invokes `006BE460` to populate/classify the XMD-derived animation array, then invokes `006C1430(0)` to perform the animation-to-actor transform update.
- Iterates three additional attached object slots and dispatches their virtual update slot when their validity marker is `Y`.

This establishes the sequence `CRdData/resource validity -> actor readiness -> CRdMesh/XMD animation population -> actor animation update`, with a sibling attached-object update loop. The exact owner class of `00461060` and final renderer invocation remain open.

Evidence: `inputs/decompiler/steam/DP_decompiled.c:75007-75042`; `inputs/decompiler/steam/calls.csv:40179-40180`; XMD animation findings.
