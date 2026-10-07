# Embedded Object Lifecycle Pattern and Selector-Factory Handle Production

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for each listed construction/registration fact; `STRONG_INFERENCE` for the cross-family pattern.

## CMustachesAdmin

CMustachesAdmin has a directly named vtable and RTTI. Paired helpers establish:

| Role | Steam | GOG |
|---|---:|---:|
| field initializer | `004274B0` | `004274D0` |
| vtable reset | `004274D0` | `004274F0` |
| deleting-destructor helper | `004274E0` | `00427500` |

The initializer writes the vtable and `-1` values through `+0x34`, establishing a `0x3C` observed allocation size. Its own helper paths have no direct active-object-manager or CRdHandleUtil operation.

Observed owner forms:

| Outer owner | Storage | Construction / teardown evidence | Independent registration evidence |
|---|---|---|---|
| CPlayer | in-place `+0x80C` | outer constructor/teardown write vtable | none observed |
| CFishingPerson | parent-held pointer at `+0x7F0`; allocation `0x3C` | constructor allocates and initializes record | none observed |
| CMenu | in-place `+0x9478` | CMenu constructor/teardown write vtable | none observed |

CFishingPerson’s separately allocated member is evidence of parent-held helper storage, not proof of an independently registered game object. Its exact release path remains unresolved.

## CNpcRecord comparison

CNpcRecord similarly has vtable-only initializer/deleting-destructor helpers, while CNpcKiller writes the record vtable at outer `+0xD28`. Reviewed lifecycle code reaches no direct CRdHandleUtil or active-object-manager operation for the record. The independent embedded lifecycle conclusions are therefore aligned across both families.

## Selector-factory outer-handle rule

The paired selector-factory success tail loads `00BD7670`, passes the newly constructed outer object to active-object registration, and thereby stores a CRdHandleUtil packed handle at outer `+0x3C`.

This is direct assembly evidence for successful outer factory results—including CPlayer, CNpcKiller, and CFishingPerson. It does not imply that a vtable-bearing subobject receives a separate handle.

## Pattern status and limits

The examined families support a reusable static pattern: outer objects own registration; embedded or parent-held helper objects are initialized/reset through local lifecycle code without a direct independent handle path. This is not universal proof for all engine classes: indirect virtual dispatch and runtime-only manager relationships remain unresolved unless directly traced.
