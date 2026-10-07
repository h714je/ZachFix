# XNV Selector Object-Family Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired selector construction and XNV resource handoff; XNV grammar remains `UNKNOWN`.

Paired roots Steam `006A3BA0` / GOG `006A3AF0` build a persistent object family around named `OGE60511.XNV`:

- create five selector-`0x40` objects and store them in global array `0148010C[]`;
- create one selector-`0x41` object and store it at `01480108`;
- resolve `OGE60511.XNV` through CRdData `006B2BA0`, along with companion resource records `0x3311` and `0x3313`;
- pass all three resolved values into paired object setup `006BE6E0/006BE1F0`;
- set object selector/type fields and configuration flags, then call a paired global-family helper.

This verifies XNV resource-loading -> persistent selector-`0x40/0x41` object-family setup. It does not establish XNV file grammar, the concrete classes behind selectors, or the payload role.
