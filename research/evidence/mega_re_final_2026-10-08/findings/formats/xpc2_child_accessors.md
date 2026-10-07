# XPC2 Child-Record Accessors

**Date:** 2026-10-01

## Verified structure

The XPC2 parser stores a child-grid descriptor at the CRdPicture-side parser state and allocates one `0x20`-byte record per child. The helper layer exposes indexed and key-based access:

- Steam `006B5660` checks row/column bounds against the XPC2 header dimensions, invokes a state check, and returns `child_base + ((row_count * row) + column) * 0x20` when valid. Invalid indices return zero.
- Steam `006B55D0` walks child records from a base obtained through `006B5710`, compares a key through `0040ECB0`, advances by `row_count * 0x20`, and returns the matching child through `006B5660`.
- Steam wrappers `006B5550` and `006B5590` call the key/index accessors and, on success, pass the selected record to `006B6020`.
- GOG `006B5660` uses an offset table rooted at the parser state (`+0x20 + index*4`) and returns the child address relative to the parser payload base. GOG's neighboring `006B56A0` applies the same row/column bounds and `0x20` stride. The build-specific helper split differs, but the child-grid abstraction is structurally matched.

## Confidence

The child-record allocation, fixed stride, bounds, and accessor arithmetic are `VERIFIED`. The meaning of individual fields, whether rows/columns represent tiles, layers, frames, or another package dimension, and the consumer-side rendering interpretation remain `UNKNOWN`.

The repeated callers of `006B5660` include scene/resource and rendering-adjacent code, but caller presence alone does not establish a CRdPicture field schema. The next discriminator is a consumer that reads a child record field and sends it to a concrete D3D9 or actor operation.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:431403-431469`
- `inputs/decompiler/gog/DP_decompiled.c:329231-329291`
- `inputs/decompiler/steam/DP_decompiled.c:431227-431327`
- `inputs/decompiler/gog/DP_decompiled.c:328984-329088`
- `inputs/decompiler/steam/calls.csv` callers of `006B55D0/006B5660`
- `inputs/decompiler/gog/calls.csv` callers of `006B5660`
