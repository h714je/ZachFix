# Win32 DirectShow Media-Event Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the paired notification, event-drain, parameter-release, and completion-latch path.

Paired audio/DirectShow setup functions Steam `00735D00` and GOG `00735A10` create the DirectShow Filter Graph (`CLSID_FilterGraph` `e436ebb3-524f-11ce-9f53-0020af0ba770`) and query it for `IID_IMediaEventEx` (`56a868c0-0ad4-11ce-b03a-0020af0ba770`) into global `014B0A44`.

The setup then invokes that interface's `SetNotifyWindow` with the application window global `014AFFD8`, message `0x8001`, and instance data `0`. The paired window procedures Steam `00700C90` / GOG `00700BF0` route only that custom message to Steam `007364B0` / GOG `007361C0`.

Each helper, while completion latch `014B0A74` is clear:

1. invokes `IMediaEventEx::GetEvent(&event_code, &param1, &param2, 100)`;
2. when `event_code == 1` (`EC_COMPLETE`), sets `014B0A74` and stops draining in the WndProc path;
3. invokes `IMediaEventEx::FreeEventParams(event_code, param1, param2)` for every fetched event.

The paired audio completion poll Steam `00736560` / GOG `00736270` uses the same interface with timeout `20`, frees every returned event parameter set, and also latches event code `1`. Its separate media-position comparison is not needed to establish the WndProc notification route.

`014B0A44` is released with the rest of the DirectShow interface globals by Steam `00736300` / GOG `00736010`; Steam `00736230` / GOG `00735F40` clears the completion latch during teardown.

This is a static control/data-flow result. It does not establish when DirectShow posts notifications at runtime, the playable asset/source semantics, or real-time audio cadence.
