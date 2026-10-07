# XPC2 D3D9 Registry Cleanup Bridge

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for texture-pointer reverse lookup and registry cleanup bridge.

Paired `006CBAC0/006CB560` scans the 2D texture registry for a descriptor whose COM texture slot at entry `+0x10` equals the input pointer. On match, it derives paired index-related state and invokes cleanup helpers; no match is a no-op.

The only direct callers are XPC2 texture-holder cleanup Steam `006B59A0` / GOG `006B58F0`. Thus XPC2 texture cleanup is directly connected to the generic D3D9 2D texture registry through pointer reverse lookup and registry-side cleanup.

This does not identify the registry owner or descriptor provenance, but distinguishes this path from resource acquisition: it is cleanup/unregistration-facing.
