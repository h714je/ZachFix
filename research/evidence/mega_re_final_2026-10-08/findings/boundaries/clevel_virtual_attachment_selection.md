# CLevel Virtual Attachment-Selection Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for CLevel vtable slot resolution and paired attachment-selection continuation; final renderer semantics remain `UNKNOWN`.

## Slot `+0x4C`: bounded result

Raw CLevel vtable entries remain direct evidence:

| Build | CLevel vtable | `+0x4C` target |
|---|---:|---:|
| Steam | `007798AC` | `006C0620` |
| GOG | `0077989C` | `006C04D0` |

The bodies are build-specific rather than instruction-level homologues. Steam `006C0620` requires CLevel `+0x160` nonzero and dispatches resource/attachment update branches. GOG `006C04D0` creates or updates a three-entry `0x1C`-stride attachment slice at CLevel `+0x280` before invoking `006C02E0`. Neither body dereferences the CLevel CRdPicture slot `+0x164`, and neither has a direct D3D9/D3DX import/call edge.

Therefore the virtual call reached after paired XMD/XPC CLevel setup is a CLevel attachment/update boundary, not direct proof of renderer submission.

## Paired continuation

Steam `006C07D0` and GOG `006C02E0` are structurally paired CLevel helpers:

- reset CLevel selection fields `+0x170` and `+0x174`;
- under CLevel `+0x168` and a shared manager gate, iterate three entries at `+0x280 + i*0x1C` with associated `0xA0`-stride transform arrays;
- inspect positive record field `+0x94` and compare node names against literal `N_HEAD`;
- retain the selected entry pointer/index at `+0x170/+0x174` or use the default inline state at `+0x178`;
- route to paired attachment/update helpers, with no direct D3D9/D3DX call.

The selected attachment’s eventual consumer is unresolved. This establishes an object/attachment selection layer after the CLevel resource setup, not a final render path.

## Limits

- The Steam/GOG slot bodies are paired only by the same verified CLevel vtable position and shared CLevel attachment-update context; their differing implementation shapes are retained as a build-specific constraint.
- The source/meaning of CLevel `+0x168` and `+0x280` attachment entries, and the consumer of selected `+0x170/+0x174`, remain `UNKNOWN`.
- No runtime timing, final render behavior, or universal CLevel relationship is inferred.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c`: `006C0620`, `006C07D0`
- `inputs/decompiler/gog/DP_decompiled.c`: `006C04D0`, `006C02E0`
- raw CLevel vtable evidence `007798AC` / `0077989C`
- `findings/boundaries/clevel_virtual_4c.md`
- `findings/boundaries/crdpicture_clevel_resource_attachment.md`
