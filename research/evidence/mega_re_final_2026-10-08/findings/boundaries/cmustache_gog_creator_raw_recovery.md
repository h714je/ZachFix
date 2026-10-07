# GOG CMustachesAdmin Creator Raw Recovery

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED`

GOG `00527240` is the structural counterpart of Steam `00527170` despite a Ghidra function-boundary failure. The GOG decompiler/export records only the prologue and initial packed-handle lookup, then marks the continuation as orphaned after calls to `006C5AD0`; its generated function record therefore reports only 79 bytes and two callees.

Raw GOG executable disassembly from `0052728E` through `005276A6` establishes the complete paired body. For each parent CMustachesAdmin handle slot `+0x2C`, `+0x30`, and `+0x34`, it:

1. checks the slot through `006C5AD0`;
2. on an unresolved slot, allocates `0x628` bytes;
3. initializes the base and stores `CMustache::vftable` `007764F4`;
4. registers the child through `006C55E0`;
5. applies the same paired `0xA80/0xA81`, `0xA82/0xA83`, or `0xA84/0xA85` resource setup and callback `005270F0`;
6. stores the returned packed handle from `006C5AB0` to the matching parent slot.

The three raw blocks begin at `0052729B`, `005273FA`, and `0052755D`; their handle stores occur at `005273DE`, `00527541`, and `005276A4` respectively. Direct callers include paired scene/owner contexts `004213F0`, `00507BA0` raw-call site `00507ED5`, `00614F20`, and `00662080`.

This verifies the same parent-to-three-registered-child control boundary in both PC builds. It does not establish a direct parent-triggered CMustache retirement path or runtime child lifetime/cadence.
