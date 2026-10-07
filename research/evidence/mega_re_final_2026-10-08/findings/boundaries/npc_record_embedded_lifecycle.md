# CNpcRecord Embedded Lifecycle and Factory Descriptor Boundary

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for bounded construction/teardown facts; `UNKNOWN` for indirect virtual behavior and descriptor ownership.

## Embedded CNpcRecord lifecycle

| Role | Steam | GOG |
|---|---:|---:|
| vtable-only initializer | `005F2900` | `005F29D0` |
| deleting-destructor helper | `005F2910` | `005F29E0` |
| outer CNpcKiller constructor | `005F2930` | `005F2A00` |
| outer CNpcKiller teardown | `005F29A0` | `005F2A70` |
| embedded vtable offset | outer `+0xD28` | outer `+0xD28` |

The standalone helpers write only the CNpcRecord vtable; the deleting-destructor helper conditionally calls the allocator delete helper after the vtable write. CNpcKiller construction and teardown each write the embedded vtable directly at outer `+0xD28`. The outer teardown cleans a distinct inherited/embedded region at `+0xF28` before restoring the CNpcRecord vtable.

No reviewed CNpcRecord initializer, deleting-destructor helper, outer constructor, or outer teardown calls CRdHandleUtil or active-object-manager registration/release. No independent packed-handle field is observed for the embedded record. This is a bounded static conclusion, not a claim that virtual methods can never reach manager state indirectly.

## Source descriptor boundary

`004AB200/004AB2E0` do not consume a CNpcKiller/CNpcRecord field. They consume byte `+0x30` from transient factory-initialization descriptors built on multiple paths, including event-interpreter and configuration accessors. Byte value `7` selects factory selector `0x08`, which produces/registers outer CNpcKiller.

The descriptor is therefore a source/type producer for the factory, not evidence that CNpcRecord owns a type byte at `+0x30`. Its exact class/name and all producer families remain unresolved.

## Remaining discriminators

- Locate direct calls or vtable dispatch into CNpcRecord methods after outer construction.
- Identify the class/type and lifetime of the transient factory-initialization descriptor.
- Compare the same embedded-vtable-only pattern with CMustachesAdmin and other selector-family embedded objects.
