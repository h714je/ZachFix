# CThrowLure Retirement Marker and Iterator Census

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for static iterator/marker control flow; runtime execution and aftermath remain `UNKNOWN`.

## Scope

This bounded census answers how the active-object manager selects objects for `006C7070` Steam / `006C6B70` GOG retirement, what directly marks an object for selection, and whether the duplicate-registered CThrowLure can traverse that path cleanly.

It does not infer a runtime hang, a use-after-free observation, gameplay impact, or how often reset occurs.

## 1. Retirement consumes the active-object list, not a separate queue

| Operation | Steam | GOG | Direct behavior |
|---|---:|---:|---|
| first matching entry | `0041B640` | `0041B660` | Seeds traversal from manager `+0x10` |
| next matching entry | `0041B5E0` | `0041B600` | Follows the candidate's `+0x54` link |
| retirement-mark test | `00427320` | `00427340` | Returns `object[+0x29] & 0x80` |
| retirement | `006C7070` | `006C6B70` | Iterates with category selector `0xFF` |

The next helper skips an entry only when its `+0xDC` high bit is set, then accepts an entry when `object[+0x29] <= requested_category`. Retirement requests `0xFF`, so that category comparison accepts every non-excluded byte value. It separately tests `+0x29 & 0x80` before unlink, local cleanup, current-handle release, and delete.

**Result:** the `0xFF` loop is a full active-list traversal with a retirement-mark predicate, not an enqueue/dequeue operation over an independent retirement list.

## 2. Direct retirement-marker census

The direct exact-write census found one paired function that assigns the retirement bit:

| Operation | Steam | GOG | Static effects |
|---|---:|---:|---|
| `active_object_mark_for_retirement` | `006BAB20` | `006BAA70` | ORs `+0xDC` with `0x60`; if not already marked, sets `+0x29 = 0x80`, clears `+0x2A`, and dispatches callback/event code `2` |

No direct call to this function appears in the supplied call graph; it is a virtual implementation. Raw CThrowLure vtable bytes establish the following slots:

| CThrowLure slot | Steam | GOG | Result |
|---|---:|---:|---|
| `+0x04` | `006BA7C0` | `006BA710` | scale initialization; does not write `+0x29` |
| `+0x08` | `006BCB20` | `006BC630` | local runtime cleanup; does not write handles/list links |
| `+0x30` | `006BAB20` | `006BAA70` | retirement-marker implementation |

CThrowLure construction inherits the CRdObject base initialization, which sets `+0x29` to zero. Neither CThrowLure construction nor registration itself marks it for retirement.

### Direct-call census boundary

`CRdHandleUtil::release` remains narrower still:

- Steam `006C7810` has exactly one direct caller: `006C7070`.
- GOG `006C7310` has exactly one direct caller: `006C6B70`.
- The complete direct `01481358` reference set remains registration, lookup, retirement release, static initialization, and at-exit vtable teardown.

This exhausts the direct release and direct exact `+0x29 = 0x80` writer census in the supplied PC exports. It does **not** prove that every indirect vtable/task source capable of reaching vtable `+0x30` is semantically mapped, nor does it establish runtime behavior.

## 3. Manager dispatch paths

`006C5DA0/006C58A0` dispatches a selected active object's vtable slot `+0x30` only when manager field `+0x18E0 != 2`; state `2` takes a separate manager-flag path instead. `006C5DF0/006C58F0` walks the matching active list and invokes that dispatcher for objects passing its selector.

Its selector predicate is explicit:

- selector `0` returns true unconditionally;
- selectors `1`–`6` apply category, state, bitmask, or callback tests.

Manager reset is the material path:

```text
Steam: 006C58B0 -> 006C5DF0(0, 0, 0) -> 006C7070
GOG:   006C53B0 -> 006C58F0(0, 0, 0) -> 006C6B70
```

Selector `0` accepts every iterator entry. Therefore, when manager `+0x18E0 != 2`, reset dispatches the `+0x30` virtual operation before attempting generic retirement; for CThrowLure, that operation is the paired `+0x29 = 0x80` marker above. Manager state `2` bypasses this virtual marker path.

## 4. CThrowLure self-link interaction

Duplicate registration already establishes that CThrowLure fields `+0x50/+0x54` become self-links. Both the reset broadcast and generic retirement compute their next iterator cursor from `current + 0x54` **before** the virtual dispatch, unlink, release, or delete sequence.

Therefore, when traversal reaches the self-linked CThrowLure:

1. the reset broadcast computes itself as the next cursor;
2. when manager `+0x18E0 != 2`, it marks the object through CThrowLure slot `+0x30` on the first visit;
3. the next broadcast iteration uses the same object again, whose `+0xDC` high bit remains clear and whose `+0x29 <= 0xFF` still passes the iterator filter; and
4. under that enabled marker path, the reset broadcast cannot statically be shown to advance past that entry to the subsequent generic-retirement call.

If another path marks CThrowLure before generic retirement is entered, retirement likewise saves the self pointer as its next cursor before its first unlink/release/delete sequence. The first iteration releases only current H2; any subsequent iteration would use the previously saved pointer after deletion. This is a static cursor/lifetime condition, not a claimed runtime observation.

## 5. Bounded conclusions

### `VERIFIED`

- Retirement is list iteration plus `+0x29 & 0x80` selection, not a distinct retirement queue.
- `006BAB20/006BAA70` is the sole direct exact marker writer found in the bounded static census.
- CThrowLure vtable `+0x30` resolves directly to that marker.
- Manager reset broadcasts the dispatcher path with unconditional selector `0` before calling generic retirement; its virtual marker operation requires manager `+0x18E0 != 2`.
- A self-linked CThrowLure causes the source-level next-cursor calculation to produce the same object in both relevant loops.
- Direct CRdHandleUtil release callers are exhausted: only generic retirement calls release.

### `UNKNOWN`

- Whether runtime reset occurs while a live duplicate-registered CThrowLure is reachable in the manager traversal.
- Runtime behavior after a self-linked traversal repeats or after retirement deletes the object while its saved next cursor remains the same address.
- Any indirect vtable/task source beyond the reviewed manager broadcast/dispatcher paths that invokes CThrowLure slot `+0x30`.
- Any gameplay, stability, or persistence consequence.

## Cross-build basis

All claims are paired structural matches between Steam and GOG, including iterator control flow, mark predicate, mark writer, manager broadcast/reset ordering, and CThrowLure vtable slot contents.
