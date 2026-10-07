# Win32 Activation Helper No-Op

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired no-op body; direct activation-state handoff remains `UNKNOWN`.

Paired WndProcs route `WM_ACTIVATE` parameters to `004012D0`. In both PC builds, the helper is a direct no-op (`return`).

Together with the mouse translator and resize helper no-ops, this bounds the explicit WndProc helper layer for those messages. Any application focus/activation behavior must derive from stored globals, a separate Win32 path, indirect state polling, or runtime evidence.
