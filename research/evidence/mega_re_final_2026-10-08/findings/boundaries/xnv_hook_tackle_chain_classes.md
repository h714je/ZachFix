# XNV Hook/Tackle Chain Object Families

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for selector-class mapping and XNV family placement; XNV grammar remains `UNKNOWN`.

The paired selector factory directly resolves the XNV family classes:

| Selector | Steam constructor | GOG constructor | Allocation | Class |
|---:|---:|---:|---:|---|
| `0x40` | `005F5000` | `005F5100` | `0x690` | `CObjectCHookChain` |
| `0x41` | `005F5130` | `005F5230` | `0x708` | `CObjectCTackleChain` |

Each paired constructor writes its named vtable and initializes embedded `CSoundLoop` state. The XNV setup root creates five HookChain objects and one TackleChain object, resolves `OGE60511.XNV` and companion resources through CRdData, then forwards them into paired setup.

This verifies XNV resource -> HookChain/TackleChain family setup. It does not establish XNV file grammar or payload semantics.
