# Win32 Dropfiles Helper No-Op

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired no-op body; direct file-drop engine handoff remains `UNKNOWN`.

WndProc `WM_DROPFILES` handling enumerates and uppercases files, then calls `004012C0` with begin marker, each file, and end marker. In both PC builds, `004012C0` is a direct no-op (`return`).

Thus the explicit drag/drop helper layer has no direct application/resource consumer in the supplied builds. Any file-drop behavior must occur through another path, indirect state, or runtime behavior. The custom WndProc message `0x8001` remains the only nontrivial direct route not yet inspected.
