# CObjectSpecies XAM Receiver Transition

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired resource-driven attachment/update control flow; XAM payload semantics remain `UNKNOWN`.

CObjectSpecies virtual `+0x54` resolves to Steam `006C0480` / GOG `006BFF90`. For a nonzero resource argument, the paired receiver:

- requires owner field `+0x160`;
- checks a resource header/count at argument `+8` against a shared node count;
- invokes paired resource-side helpers that can force a rebuild/update;
- on a change, writes a timing/scalar field at owner `+0x1F4` and calls paired attachment/update helpers (`006B9020/006B8F70`, then `006C07D0/006C02E0`).

The XAM-selection method at CObjectSpecies `+0x84` passes the resolved `SEEDG001/002/003.XAM` resource into this receiver. This verifies XAM resource selection -> CObjectSpecies resource-driven attachment/update transition. It does not prove XAM grammar, whether the resource is exclusively animation data, or an exact semantic name for the virtual slot.
