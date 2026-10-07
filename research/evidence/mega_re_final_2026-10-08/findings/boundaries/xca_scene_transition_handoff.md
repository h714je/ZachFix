# XCA Scene-Transition Resource Handoff

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired XCA resource-to-scene transition control flow; XCA grammar remains `UNKNOWN`.

Paired scene-transition roots Steam `004213D0` / GOG `004213F0` construct a scene-specific XCA name `D%02d.XCA` using current scene index, derive the `UPDATA/SCENE/%02d/` path, and request it through the resource-loading path when absent.

They then resolve the named resource through CRdData `006B2BA0`, set scene-owner field `+0x2B8 = 1`, and pass the resolved resource to paired scene helpers Steam `004275E0` / GOG `00427600`. The same block runs paired attachment refresh calls `006BEB90/006BE6A0` and `006C07D0/006C02E0` for configured entries before updating scene transition state.

This establishes XCA resource-loading -> scene-transition/attachment refresh boundary. It does not establish XCA binary grammar, a dedicated XCA object type, or field meanings in the resource payload.
