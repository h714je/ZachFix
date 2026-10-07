# CNpcKiller Factory-to-Registry Registration

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for outer CNpcKiller construction and registration; `UNKNOWN` for independent CNpcRecord registration.

Paired source-type dispatch helpers map source object byte `+0x30` to active-object selectors:

| Source value | Selector |
|---:|---:|
| 1 | 2 |
| 2 | 4 |
| 3 | 3 |
| 4 | 6 |
| 5 | 7 |
| 6 | 5 |
| 7 | 8 |

- Steam: `004AB200`
- GOG: `004AB2E0`

Therefore source value `7` directly selects selector `0x08`, which constructs outer CNpcKiller (`005F2930/005F2A00`) with embedded CNpcRecord at outer `+0xD28`.

The shared paired selector-factory completion path then configures the new object and calls active-object registration (`006C5AE0/006C55E0`). That routine inserts the outer object in active-manager lists and obtains a CRdHandleUtil packed handle from `01481358`, stored at outer `+0x3C`.

This establishes an actual external construction-to-registration route for outer CNpcKiller, not an inference from CPlayer. It does not establish the source object's class/name, the gameplay meaning of source value `7`, CNpcKiller's broader NPC ownership, or an independent registry entry for embedded CNpcRecord.
