# Selector 0x1A / 0x1B Effect Classes

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for class identity and lifecycle anchors.

Selector factory cases map to paired effect classes:

- Selector `0x1A`: Steam `006697C0` / GOG `00669710` construct `CEffectRain`; vtables are Steam `00782124` / GOG `00782114`; paired destructors are Steam `00669950` / GOG `006698A0`.
- Selector `0x1B`: Steam `005F5660` / GOG `005F5760` construct `CEffectHaze`; vtables are Steam `0077F5B4` / GOG `0077F5A4`; paired destructors are Steam `005F5730` / GOG `005F5830`.

Both constructors are reached through `005E7620/005E76F0` selector dispatch. The supplied call/xref manifests show constructor/destructor and base-initialization references but no independent external lifecycle/render consumer for either effect class. Their full update methods, virtual slot mappings, and relationship to the unresolved `006BE6E0` owner handoff remain open.
