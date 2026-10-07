# Outer Resource Manager Root

**Date:** 2026-10-01

## Startup handoff

**VERIFIED:** Steam and GOG startup call `004051F0` immediately before loading callback `00408310` / `004082D0` and calling `006B2570(0x4BD8, callback)`. The raw startup sequence is:

```text
CALL 004051F0
PUSH callback
PUSH 0x4BD8
MOV ECX,EAX
CALL 006B2570
```

`004051F0` lazily allocates `0x38C` bytes at `DAT_00BD9E3C`, initializes it through `00402140`, writes `CSingleton<CRdData>::vftable`, and stores the pointer in the CRdData singleton global. The raw call context passes the resulting pointer as `this` to the resource-manager initializer. GOG has the same structural sequence with build-specific callback address.

## Consequence

The object previously tracked only as `resource_record_manager_candidate` is strongly linked to the `CSingleton<CRdData>` root. This explains the observed manager lower bound/size (`0x38C`) and gives the manager a concrete vtable/class anchor at the root. The semantic claim that `CRdData` is the complete public engine meaning of every field remains out of scope; current primary evidence establishes its resource-manager role at startup.

This finding supersedes the narrower statement that the outer manager had no proven vtable, but the historical ledger entry should remain until a permitted ledger update can preserve the transition explicitly.

## Remaining questions

- Whether later `004051F0` calls always return the same global object or only initialize it lazily.
- Exact field names and ownership for `+0x0C`, `+0x10`, `+0x14`, `+0x384`, and `+0x388` within CRdData.
- Whether CRdData has additional non-resource responsibilities outside the archive manager path.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:4368-4397`
- `inputs/decompiler/gog/DP_decompiled.c:4470-4499` (paired CRdData initializer)
- `inputs/decompiler/steam/DP_decompiled.c:437-476`
- `inputs/decompiler/gog/DP_decompiled.c:439-478`
- `inputs/decompiler/steam/DP_full.asm:580-586`
- `inputs/decompiler/gog/DP_full.asm:580-586`
- `findings/formats/resource_manager_population.md`
