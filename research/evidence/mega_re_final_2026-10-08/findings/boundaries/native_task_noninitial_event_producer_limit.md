# Native Task Non-Initial Event Producer Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for source limitation; non-initial producer remains `UNKNOWN`.

The paired common callback seam has 36 direct callers per PC build. Native task registration setter `006BAB80/006BAAD0` is the only direct source that identifies callback `00647730/00647680`, and it immediately dispatches event `0`.

Other seam callers dispatch generic object callbacks, but the selector-0 task stores callback identity indirectly at object `+0x44`; no static direct caller or raw pointer reference ties any non-initial event source to this callback. The literal event-`0x12` seam search also recovered no source.

Non-initial native task event producers, cadence, and ordering remain `UNKNOWN` pending a class-discriminated task dispatch path or runtime trace. Do not repeat generic seam-caller enumeration.
