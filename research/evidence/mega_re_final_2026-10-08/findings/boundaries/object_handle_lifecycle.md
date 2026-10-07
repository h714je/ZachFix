# Active-Object / CRdHandleUtil Handle Lifecycle

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for CRdHandleUtil and its registration/release boundary; `STRONG_INFERENCE` for the active-object manager class identity.

## CRdHandleUtil static singleton

`01481358` is a static `CRdHandleUtil` object in both PC builds. The conclusion is directly supported by the named vtable written by paired constructors:

| Role | Steam | GOG |
|---|---:|---:|
| static initialization wrapper | `0076D6D0` | `0076D3E0` |
| constructor | `006C7AF0` | `006C75F0` |
| vtable | `00826ADC` | `00826ACC` |
| at-exit wrapper | `0076DE90` | `0076DBA0` |
| static destructor | `006C7B80` | `006C7680` |
| allocation | `006C7650` | `006C7150` |
| lookup | `006C7780` | `006C7280` |
| release | `006C7810` | `006C7310` |

The constructor writes the named vtable, zeroes cursors at `+0x04` and `+0x08`, zeroes `0x2800` bytes beginning at `+0x0C`, and zeroes `0x7FFF` bytes beginning at `+0x280C`. This establishes an observed static-object lower bound of `0xA80B`.

`CRdHandleUtil::allocate` scans for a vacant pointer slot among `0xA00` entries, stores the object pointer, scans for a clear generation flag among `0x7FFF` entries, sets that flag, advances both cursors, and returns `(generation << 16) | slot`. It is not safe to interpret the high field as a monotonically incremented per-slot generation counter: the direct evidence establishes an independently allocated occupied-generation index, not its broader policy.

The paired lookup wrappers `006C5FD0/006C5AD0` remain the high-fanin access path. They reject `0xFFFFFFFF`, validate the low index, non-null slot, and high field against the current object-handle value before returning the slot pointer.

## Active-object manager boundary

`00BD7670` points to a lazily cached `0x6778`-byte vtable-bearing object:

| Role | Steam | GOG |
|---|---:|---:|
| cached allocation/root acquisition | `00406F70` | `00406F30` |
| installed vtable | `0076F048` | `0076F038` |
| startup initialization | `006D1610` | `006D11E0` |
| registration | `006C5AE0` | `006C55E0` |
| retirement | `006C7070` | `006C6B70` |
| shutdown teardown | `006D2880` | `006D2450` |

Startup assigns this object to `00BD7670` and immediately calls the paired initialization method. The top-level frame scheduler passes the root to retirement. Its registration routine links the incoming object into manager lists, invokes `CRdHandleUtil::allocate` on `01481358`, and stores the returned packed handle at object `+0x3C`. Retirement removes objects from manager lists, invokes virtual slot `+0x08`, then calls `CRdHandleUtil::release`, which reads the same `+0x3C` field and clears both the pointer slot and generation flag.

This proves active-object-manager behavior and the ownership relationship with CRdHandleUtil. The manager’s exact RTTI class name and the meaning of its many initialized fields are still `UNKNOWN`; the canonical name remains `active_object_manager_candidate` with `STRONG_INFERENCE` confidence.

## CPlayer and CNpcKiller

The player initialization callback provides a direct paired construction-to-registration path:

```text
Steam:  005FBDC3 CPlayer constructor -> 005FBDEB active-object registration
GOG:    005FBEC3 CPlayer constructor -> 005FBEEB active-object registration
```

Both blocks load `00BD7670` as `this`, pass the newly constructed CPlayer as the registration object, and persist it into the player pointer array before the registration call. Therefore the outer CPlayer receives a CRdHandleUtil packed handle at `+0x3C`. This does not assign a registry role to the embedded CMustachesAdmin subobject at `+0x80C`.

CNpcKiller is verified as a paired selector-factory construction root (`005F2930/005F2A00`, selector `0x08`) but does **not** have an observed direct constructor-to-registration bridge in the supplied static call/assembly corpus. No registry linkage is promoted for either CNpcKiller or its embedded CNpcRecord at `+0xD28`.

## Evidence limits

- No runtime traces are available to characterize packed-handle reuse, exhaustion, or broader producer taxonomy.
- `CRdHandleUtil` naming is primary-symbol and constructor evidence, not a decompiler guess.
- The active-object manager’s behavioral role is structural and cross-validated, but its concrete class name is not established.
