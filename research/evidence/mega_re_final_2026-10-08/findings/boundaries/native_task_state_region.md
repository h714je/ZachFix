# Native Task Private State Region — Initial Field Map

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for paired callback-local control structure; semantic subsystem identity remains `UNKNOWN`.

## Bounded field map

| Address | Width / role | Evidence |
|---|---|---|
| `01474CF8` | 32-bit top-level selector | Both `00647730/00647680` repeatedly switch on it and write values including `0`, `1`, `3`, `4`, `6`, `9`, `0x0B`–`0x0E`, `0x5A`–`0x5C`, `0x62`, `0x65`, `0x66`, `99`, and `100`. |
| `01474CFC` | 32-bit deferred/companion selector | Both callbacks write transition values then later assign `01474CF8 = 01474CFC`. |
| `014768C0` | scalar selector/count gate | Compared against `5` and `0`; gates repeated helper/state setup. |
| `014768C8` | scalar sub-selector/index | Compared against local indices and passed to repeated table/helper accessors. |
| `014768F5` | byte sub-selector | Registration initializer writes zero; callback uses it to index local selector bytes. |
| `0147693C` | byte registration/availability flag | Registration initializer writes one; external code tests/clears it. |

The paired callbacks have matching selector values, write sites, and helper topology. This establishes a private state-machine region with a current selector and deferred selector, not a semantic UI/event manager identity.

## Limits

The call/xref manifests do not expose the `01474xxx` symbols as ordinary xref targets, so writer/reader census must use direct decompilation/assembly and explicit textual/static references. No runtime cadence, transition frequency, or user-facing meaning is claimed.
