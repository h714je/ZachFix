# Win32 Mouse Translator No-Op

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired no-op body; downstream mouse representation remains `UNKNOWN`.

Paired WndProcs call `004012B0` after storing mouse coordinates at `014AFFC8/C4`, preparing button state, and collecting keyboard state. In both PC builds, `004012B0` is a direct no-op (`return`).

Thus the direct engine-side mouse state established by this ingress path is the shared coordinate/global representation, not a translator-owned object. The next discriminator is the direct reader/writer census for those globals; gameplay/input semantics remain unknown.
