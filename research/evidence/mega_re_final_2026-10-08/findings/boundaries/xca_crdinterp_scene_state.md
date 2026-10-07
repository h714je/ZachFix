# XCA CRdInterp Scene-State Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired CRdInterp allocation and XCA forwarding; XCA grammar remains `UNKNOWN`.

Paired scene helpers Steam `004275E0` / GOG `00427600`, called only from the recovered XCA transition roots, lazily allocate a `0x30`-byte `CRdInterp` object, write `CRdInterp::vftable`, initialize its observed fields to zero, and store it at scene-owner `+0x2E8`.

They then forward the resolved `D%02d.XCA` resource value to paired `006B9E70/006B9DC0`. This verifies an XCA resource -> CRdInterp-backed scene state handoff. It does not establish XCA payload grammar or the exact meaning of CRdInterp state.
