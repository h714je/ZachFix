# Paired Selector Factory Direct-Vtable Census

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for direct named-vtable construction roots.

## Scope

This census exhaustively inspected paired selector factories Steam `005E7620` and GOG `005E76F0`. A class root was promoted only where both builds had a matched selector, paired constructor, direct matching named-vtable write, and allocation evidence.

## Results

- 136 single-vtable paired roots were recovered in the full-factory census.
- Two multi-vtable roots were separately resolved:
  - selector `0x01`: outer CPlayer with embedded CMustachesAdmin at `+0x80C`.
  - selector `0x08`: outer CNpcKiller with embedded CNpcRecord at `+0xD28`.
- One selector (`0xAA`) is direct reuse of an already recovered CObjectFieldPhysics_Type3 class rather than a new root.
- All other skips were previously censused roots or multi-vtable cases already resolved as outer-object plus embedded-CSoundLoop patterns.

## Exhaustion conclusion

The selector factory is exhausted for direct recoverable class construction roots. This is a bounded result: it does not claim that the executable has no additional classes outside the factory, nor that every recovered class has a known lifecycle, slot map, owner, or behavior.

## Deferred evidence

- Semantic behavior and subsystem ownership of most named object classes remain deferred.
- Secondary vtable writers are recorded as lifecycle candidates only unless their cleanup body was independently reviewed.
- CObjectFieldPhysics_Type3 selector reuse is intentionally not duplicated as a separate class root.

## Evidence

- `inputs/decompiler/steam/DP_decompiled.c`, factory `005E7620`
- `inputs/decompiler/gog/DP_decompiled.c`, factory `005E76F0`
- Paired xref manifests for constructor and named-vtable writes
- `scratch/selector_factory_direct_vtable_census.json`
