# CEvent Stream Writer `00435110/00435190` Negative

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for stream consumption; initial-buffer installation remains `UNKNOWN`.

Paired `00435110/00435190` reads a byte from CEvent stream `+0x18`, advances it through a compact record containing a 16-bit mode and RGB-like bytes, and routes resulting scene/camera-state operations. Both bodies only increment the existing stream pointer and do not assign it from an external buffer or resource owner.

This is a parser consumer, not the initial CEvent stream producer. Along with `00434920/004349A0` and prior `0042B1C0/0042B240`, it narrows the bounded direct-writer census without identifying an initial buffer source.
