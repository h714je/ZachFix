# CEvent Stream Initial-Producer Census

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the bounded negative result; initial stream-buffer producer is `UNKNOWN`.

## Bounded source universe

The paired direct CEvent `+0x18` writer census examined 17 Steam/GOG-aligned parser-side candidates beginning at `00434920/004349A0` and ending at `0043E850/0043E8D0`, including prior candidate `0042B1C0/0042B240`.

Every reviewed candidate either:

- saved and restored the existing pointer across an external operation; or
- incremented it while decoding a compact or variable-length event record.

The final pair `0043E850/0043E8D0` advances the pointer over a 16-bit value plus byte flag before optional event dispatch. None assigns `CEvent +0x18` from an external buffer, resource, archive payload, or caller-supplied pointer.

## Consequence

CEvent parser dispatch and record-consumer semantics are strongly bounded, but the initial stream-buffer installation source is absent from this direct parser-side writer universe. The initial buffer producer must remain `UNKNOWN` until a higher-level indirect lifecycle, loader, or runtime trace provides a new discriminator.
