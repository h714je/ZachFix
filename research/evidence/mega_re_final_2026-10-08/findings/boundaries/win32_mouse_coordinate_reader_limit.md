# Win32 Mouse Coordinate Reader Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for direct reader census; downstream mouse consumer remains `UNKNOWN`.

Complete paired xref census for WndProc-written mouse coordinate globals `014AFFC4/C8` found direct reads/writes only in the outer application roots (`00700650/00700670`) and WndProcs (`00700C90/00700BF0`). No direct camera, player, UI, or other game-owned reader is present in the supplied PC xref manifests.

The direct static input path is bounded at Win32 coordinate globals. Gameplay mapping requires an indirect/global aggregate reader, a different input state path, or runtime trace. Do not repeat direct coordinate-global reader census without new evidence.
