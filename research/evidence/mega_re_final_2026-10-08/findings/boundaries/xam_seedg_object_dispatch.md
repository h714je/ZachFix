# XAM `SEEDG` Resource to Object Dispatch

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired named XAM resource resolution and virtual object handoff; owner class and slot semantics remain `UNKNOWN`.

Paired Steam `005AD940` / GOG `005ADA10` are the first concrete XAM consumer boundary recovered from explicit `.XAM` names plus CRdData accessors.

The pair ensures two companion character resources through `006B2C30`, resolves their record objects, and passes them to paired object setup `006BE6E0/006BE1F0`. It then selects one named XAM resource according to `*param_2`:

| selector | Resource |
|---:|---|
| 0 | `SEEDG001.XAM` |
| 1 | `SEEDG002.XAM` |
| 2 | `SEEDG003.XAM` |

For the selected name it calls CRdData name lookup `006B2BA0`, then passes the returned resource value to the object's virtual slot `+0x54` with fixed control arguments. The function also copies caller-provided configuration fields into the owner afterwards.

This verifies resource-loading -> object virtual dispatch for these XAM assets. It does not establish XAM file grammar, a distinct XAM class/vtable, the concrete owner class, or that slot `+0x54` is specifically an animation method.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c`: `005AD940`
- `inputs/decompiler/gog/DP_decompiled.c`: `005ADA10`
- `006B2BA0` CRdData name lookup
- `006BE6E0/006BE1F0` paired resource setup
