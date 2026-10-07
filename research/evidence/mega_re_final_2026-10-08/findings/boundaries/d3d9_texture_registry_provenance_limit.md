# D3D9 Texture Registry Provenance Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for bounded accessor chain; registry owner provenance remains `UNKNOWN`.

The non-reset XPC2 cleanup bridge reaches registry metadata copy `006CD4F0/006CCF60`, then opaque wrapper `006CDAE0/006CD540`, then backing accessor `006CEC00/006CE720`.

The backing accessor only validates a computed `index * 0x18` pointer against a generic bounded-memory range and advances an iterator pointer. It has no callers beyond the wrapper and exposes neither a registry owner global nor descriptor creation provenance.

The 2D texture registry layout and reset lifecycle remain verified, but owner/provenance is `BOUNDED_STATIC` pending a new creation/insertion caller or runtime trace. Do not continue the wrapper chain.
