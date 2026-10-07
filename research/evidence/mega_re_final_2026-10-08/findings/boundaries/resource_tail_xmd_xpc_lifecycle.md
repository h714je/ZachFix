# CRdData +0x1C XMD/XPC Lifecycle Boundary

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for accessor use and release/control flow; exact object-vs-state subtype remains conservative.

Steam `004C60B0`, `004C62E0`, and `004C63B0` provide a concrete consumer set for CRdData manager field `+0x1C`:

- `004C60B0` invokes `006BE460` to populate animation state, then selects XMD/XPC names from actor/object state and calls manager release helper `006B28F0` for matching resource records.
- `004C62E0` obtains a paired record/key set from `004C6230`, checks `006B2C70(record_id, 0)` for both entries, and either accepts the existing resources or runs load/reconstruction paths before storing the active record ID at owner `+0x898`.
- `004C63B0` performs the paired validity checks and, when a third record ID is present, queries `006B2BE0(record_id, 0)` before selecting the release/load path.

Because `006B2C70` returns manager table field `+0x1C`, these functions establish that `+0x1C` participates in XMD/XPC object/state lifetime and paired resource reconstruction. The exact concrete type of that field, the role of `+0x20`, and final renderer ownership remain open.
