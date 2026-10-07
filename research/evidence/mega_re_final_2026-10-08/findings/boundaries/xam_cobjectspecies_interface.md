# XAM CObjectSpecies Interface Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for CObjectSpecies vtable placement and XAM resource handoff; slot semantics remain `UNKNOWN`.

Raw paired vtable evidence identifies the helper that selects `SEEDG001/002/003.XAM`:

| Build | CObjectSpecies vtable | XAM helper | slot | XAM-resource receiver |
|---|---:|---:|---:|---:|
| Steam | `0077E2A4` | `005AD940` | `+0x84` | `006C0480` at `+0x54` |
| GOG | `0077E294` | `005ADA10` | `+0x84` | `006BFF90` at `+0x54` |

`005AD940/005ADA10` selects the named XAM by input selector, resolves it through CRdData `006B2BA0`, and calls the CObjectSpecies inherited virtual `+0x54` with the resolved resource and fixed control arguments. This concretely places the first recovered XAM object boundary in CObjectSpecies.

No XAM parser/header, dedicated XAM class, or semantic name for virtual `+0x54` is claimed. The next discriminator is the paired `006C0480/006BFF90` body and callers.
