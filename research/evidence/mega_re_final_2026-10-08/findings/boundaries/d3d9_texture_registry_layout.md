# D3D9 2D Texture Registry Layout

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for bounds-checked descriptor layout; owner identity remains `UNKNOWN`.

Paired accessors Steam `006CCFE0` / GOG `006CCA80` validate index against a paired count helper and return:

```text
owner + 0x0C + index * 0x18
```

The frame device-loss and device-reset helpers consume these entries to release and recreate their COM texture slots. The only non-reset direct caller is Steam `006CBAC0` / GOG `006CB560`, providing the next ownership/provenance lead.

This verifies a 2D-texture descriptor registry with 0x18-byte entries; it does not identify the registry owning class or individual descriptor field semantics outside the reset-derived texture slot and CreateTexture arguments.
