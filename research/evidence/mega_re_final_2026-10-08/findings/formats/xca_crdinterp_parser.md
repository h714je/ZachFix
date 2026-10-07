# XCA CRdInterp Parser Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for magic validation and header-driven CRdInterp state allocation; detailed field semantics remain `UNKNOWN`.

Paired `006B9E70/006B9DC0` reset CRdInterp state then accept only non-null resources beginning with dword `0x31414358` (ASCII byte order `XCA1`). On acceptance they retain the resource pointer at CRdInterp `+0x04`.

Header byte `resource +0x0A` controls initialization:

- zero: CRdInterp `+0x08/+0x0C/+0x10` are cleared;
- nonzero `N`: allocate and zero arrays of `2*N`, `4*N`, and `4*N` bytes at those three fields.

The helpers return success only for a matching resource. Their direct callers are the XCA scene helper and an attachment-related helper. This establishes XCA magic -> CRdInterp header-driven state initialization. It does not establish the meaning of the count or the three arrays.
