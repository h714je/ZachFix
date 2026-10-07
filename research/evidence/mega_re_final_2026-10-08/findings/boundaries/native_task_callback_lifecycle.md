# Native Task Callback Lifecycle — `00647730 / 00647680`

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for observed registration and initial callback dispatch; runtime cadence/lifetime remain `UNKNOWN`.

## Registration chain

| Role | Steam | GOG |
|---|---:|---:|
| registration initializer | `0064D810` | raw block `0064D760`–`0064D7B1` |
| selector-0 task factory | `006C5930` | `006C5430` |
| task callback setter / initial dispatch | `006BAB80` | `006BAAD0` |
| registered callback | `00647730` | `00647680` |

Steam `0064D810` calls a gate (`00405360 -> 0042D010`), loads active-object manager root `00BD7670`, and invokes selector-0 factory `006C5930(0, 0, 0, 1)`. Selector 0 allocates `0x160` bytes, initializes CRdObject base state, then registers the task through `006C5AE0`; the factory result is the task object.

On success, `0064D810` initializes state through `005FF330`, sets `0147693C = 1` and `014768F5 = 0`, then calls `006BAB80(task, 00647730, 0)`. The paired callback setter writes the callback pointer to task `+0x44` and directly invokes the task callback seam (`00402870`) with event code `0` and the supplied registration argument. GOG uses the structurally paired factory and callback setter, with the raw registration block carrying literal callback `00647680`.

## Proven task shape and limits

- The selector-0 task is an independently registered active object, not an alias for the active-object manager.
- The callback field is `task +0x44`; the setter supports arbitrary callback pointers at that field, so the infrastructure is generic rather than exclusive to this callback.
- Initial event-0 dispatch is directly proven.
- `00647730/00647680` have no direct ordinary callers in the supplied call manifests; entry occurs through the callback pointer.
- Steam `0064D810` has a direct callsite at `004F5029`; the corresponding GOG registration raw block lacks a recovered Ghidra function boundary.
- Static evidence here does not establish later event codes, recurrence/cadence, unregistration, the task's ultimate owner, or teardown timing.

## Relation to generic object dispatch

The task is registered into the same active-object manager root, but `00647730/00647680` themselves are state-driven callbacks over private `01474xxx` state and call UI/helper families including `0045C9F0/0045CA20`. They are not established as generic active-object iterators. Scheduler `00401A70` directly calls generic dispatch `006C5FF0/006C5AF0`; no direct static nesting from that scheduler to `00647730/00647680` is established by this package.
