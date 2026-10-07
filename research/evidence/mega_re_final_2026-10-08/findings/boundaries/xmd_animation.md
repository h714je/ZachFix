# XMD to Animation / CRdMesh Boundary

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for payload-state extraction and animation transform conversion; exact header field names remain conservative.

## XMD helper

Steam `0070BD20`, reached directly by the XMD callback branch after CRdMesh construction, performs typed payload-state setup:

- Stores the supplied XMD payload/state pointer at helper `+0x04`.
- Reads payload words at `+0x08`, `+0x0C`, byte fields at `+0x39/+0x3A`, and a ushort count at `+0x2A` to allocate/populate dependent blocks through animation/resource helpers.
- Iterates the `+0x2A` count and applies a scalar based on payload byte `+0x34` to matrix/state blocks with `0x90` spacing.
- Copies the normalized payload into owned storage and reports the normalized size plus `0x54` through the output parameter.
- Uses `0070BF30` for state reset and cleanup support.

The field names are intentionally not promoted from offsets alone; the count/stride pattern is consistent with skeletal/animation node data and is linked to the CRdMesh XMD branch by the callback call edge.

## Transform extraction

Adjacent helper `0070C020` reads the normalized XMD state, loops over the payload ushort count at `+0x2A`, converts each `0x90`-spaced matrix through `D3DXQuaternionRotationMatrix`, copies translation-like values from matrix offsets `+0x40/+0x44/+0x48`, and writes normalized per-node records with scale defaults and a state/type value.

This is a direct XMD payload -> animation-transform representation boundary. It explains why CRdMesh construction initializes child arrays. Steam `006BE460` is the first verified consumer beyond the helper: it invokes `0070C020` into owner field `+0x1E4` and classifies each `0xA0` animation record by node-name prefixes and known wiper/window names. The final renderer method and complete CRdMesh field mapping remain open; details are in `findings/boundaries/xmd_crdmesh_consumer.md`.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:488940-489004` (`0070BD20`)
- `inputs/decompiler/steam/DP_decompiled.c:489061-489089` (`0070C020`)
- `inputs/decompiler/steam/DP_decompiled.c:7537-7556` (XMD callback branch)
- `inputs/decompiler/steam/calls.csv:594,81981-81996`
- `ledgers/CLASS_LEDGER.csv` CRdMesh rows
- `findings/formats/typed_resource_dispatch.md`
