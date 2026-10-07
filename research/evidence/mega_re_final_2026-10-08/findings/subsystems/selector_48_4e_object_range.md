# Selector Range `0x48`–`0x4E` Object Census

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for paired class construction and teardown; lifecycle semantics remain `UNKNOWN`.

## Bounded source

The paired selector factories `005E7620` (Steam) and `005E76F0` (GOG) were exhaustively checked for cases `0x48` through `0x4E`. Each case has matched allocation size, paired constructor/teardown vtable writes, and an explicit cross-build homology group.

| Selector | Class | Allocation | Steam constructor / teardown | GOG constructor / teardown |
|---|---|---:|---|---|
| `0x48` | CObjectFreight | `0x580` | `005F3260` / `005F3280` | `005F3360` / `005F3380` |
| `0x49` | CObjectShelf | `0x728` | `005F5570` / `005F5590` | `005F5670` / `005F5690` |
| `0x4A` | CObjectLever | `0x500` | `005F3340` / `005F3360` | `005F3440` / `005F3460` |
| `0x4B` | CObjectLight | `0x4C8` | `005F32F0` / `005F3310` | `005F33F0` / `005F3410` |
| `0x4C` | CObjectWater | `0x4D0` | `005F3390` / `005F33B0` | `005F3490` / `005F34B0` |
| `0x4D` | CObjectTreeshaphand | `0x6B8` | `005F55C0` / `005F55E0` | `005F56C0` / `005F56E0` |
| `0x4E` | CObjectTreeshaphand_Museam | `0x6B0` | `005F5610` / `005F5630` | `005F5710` / `005F5730` |

## Exhaustion result

The selector range is exhausted for recoverable direct class roots: all seven cases resolve to the seven classes above, and each has a paired teardown source that rewrites the same named vtable. No non-class or unresolved constructor entry remains within this bounded range.

## Limits

Named class construction, vtable ownership, allocation bounds, and teardown pairing are direct. Object behavior, world/resource ownership, update slots, and render or physics semantics are not established by this census.

## Evidence

- Paired selector factories `005E7620/005E76F0`
- `inputs/decompiler/*/xrefs.csv` named-vtable write sources
- `inputs/decompiler/*/DP_decompiled.c` constructor and teardown bodies
