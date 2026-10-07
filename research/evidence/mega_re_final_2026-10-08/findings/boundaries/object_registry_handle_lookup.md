# Object Registry Handle Lookup Boundary

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED`

Steam `006C5FD0` and GOG `006C5AD0` are high-fanin wrappers over global `01481358`. They forward a packed handle into paired services `006C7780/006C7280`.

The services reject `0xFFFFFFFF`, require a low-16-bit index below `0xA00`, require a non-null pointer at `singleton + 0x0C + index*4`, and require that the current generated high-16-bit value equals the supplied handle generation. They then return the table slot pointer. Paired removal services clear the same pointer slot and its generation byte.

## Phase 2 resolution

The singleton class and lifecycle are now resolved: `01481358` is a static `CRdHandleUtil` object, directly established by paired constructors writing the named vtable (`006C7AF0` Steam / `006C75F0` GOG), static initializers, and at-exit teardown. Its allocation services are `006C7650/006C7150`; active-object registration writes their packed return value at object `+0x3C`, and active-object retirement releases it through `006C7810/006C7310`.

The complete producer taxonomy and reuse behavior remain unresolved. See `findings/boundaries/object_handle_lifecycle.md` for the paired lifecycle and the separately bounded active-object-manager inference.

## Evidence

- Steam `006C5FD0`, `006C7780`, `006C7810`
- GOG `006C5AD0`, `006C7280`, `006C7310`
- Original executable disassembly and paired decompilation
