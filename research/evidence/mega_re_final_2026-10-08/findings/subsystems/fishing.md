# CFishing Selector Root

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for paired class construction and teardown; lifecycle remains `UNKNOWN`.

## Class identity

Selector `0x38` in Steam `005E7620` and GOG `005E76F0` allocates `0x170` bytes and invokes paired constructors:

| Build | Constructor | Vtable | Teardown candidate |
|---|---|---|---|
| Steam | `0060EED0` | `00780234` | `0060D630` |
| GOG | `0060EFE0` | `00780224` | `0060D850` |

The constructors directly write `CFishing::vftable`, establish shared globals `01470674` and `01470684`, and register an auxiliary task/object through the generic object machinery. The teardown candidates rewrite the CFishing vtable, conditionally invoke a virtual slot at `object+0x58`, restore `CRdObject::vftable`, and call the base cleanup routine.

## Limits

The class name, paired construction, allocation lower bound, and teardown shape are directly evidenced. This does not establish the gameplay rules, input controls, object-list ownership, virtual-slot map, or rendering path of fishing.

## Evidence

- `inputs/decompiler/steam/DP_decompiled.c:333869-333890,335086-335125`
- `inputs/decompiler/gog/DP_decompiled.c:258597-258618,259657-259696`
- Steam/GOG selector factories `005E7620/005E76F0`
- `inputs/decompiler/*/xrefs.csv` CFishing vtable write references
