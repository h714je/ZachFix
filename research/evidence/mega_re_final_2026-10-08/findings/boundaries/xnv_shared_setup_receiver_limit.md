# XNV Shared Setup Receiver Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for generic object setup behavior; XNV-specific payload attachment remains `UNKNOWN`.

Paired `006BE6E0/006BE1F0`, called from HookChain/TackleChain XNV setup, stores only its first two resource arguments at owner `+0x160/+0x164`, requires them nonzero, allocates generic `0xA0` and `0x40` stride state arrays, runs animation/attachment preparation, and calls owner virtual `+0x4C`.

Although XNV is passed as the third setup argument by the XNV root, the receiver does not retain that argument in an observed owner field; it may influence deeper generic helpers, but no XNV-specific field/format application is directly recovered. The object-side XNV path is bounded here pending a class-discriminated XNV consumer or parser/header evidence.
