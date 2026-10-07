# CLevel Selected-Attachment Lifecycle and Transform Update

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired selected-attachment teardown and CLevel transform/update consumption; direct final-render behavior remains `UNKNOWN`.

## Bounded reader census

A broad textual offset scan produced 78 Steam and 73 GOG functions containing one of the generic offsets `+0x170/+0x174`. Intersecting that set with the CLevel attachment-slice layout (`+0x280 + i*0x1C`) reduced the meaningful local candidates to:

- Steam `006BF550`, `006C07D0`, `006C1320`, `00721030`;
- GOG `006BF060`, `006C02E0`, `006C0E30`, `00720D40`.

Already-analyzed `006C07D0/006C02E0` are the selection writers. This batch inspected the two directly adjacent paired consumers below; the broader generic offset population is not treated as CLevel evidence.

## Selected attachment teardown

Steam `006C1320` and GOG `006C0E30` take a slot index `< 3`, obtain the CLevel attachment object at `+0x280 + index*0x1C`, and:

1. compare it with selected pointer `+0x170`;
2. clear `+0x170` and reset selected index `+0x174` to `-1` when matched;
3. invoke the attachment object's virtual destructor/release entry;
4. free its associated transform/state pointer at `+0x284 + index*0x1C`;
5. clear the attachment entry, associated flags, and `+0x2D4 + index*4` state.

This is a concrete CLevel attachment-slot teardown boundary.

## Attachment-aware CLevel update

Steam `006BF550` and GOG `006BF060` are large structurally paired CLevel update bodies. They:

- require CLevel `+0x160` and a shared state gate;
- inspect the three attachment slots at `+0x280`;
- perform base transform/motion update work;
- invoke paired per-slot helpers for all three entries;
- when CLevel selected index `+0x174` is nonnegative, substitute `+0x2D4 + selected_index*4` as the selected attachment state for subsequent update logic;
- use selected pointer `+0x170` as a conditional timing/update gate.

The bodies do not have a direct D3D9/D3DX import/call edge. They establish that selected state feeds CLevel transform/attachment update semantics, not a directly recovered final submission path.

## Limits and next discriminator

No claim is made that the selected attachment is a character, render mesh, or final renderer input. The next discriminating source is the CLevel interface/caller context for `006BF550/006BF060` (including a vtable-slot mapping or recurring frame/scheduler call chain), which can determine whether this transform update is part of the frame object-update phase.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c`: `006C1320`, `006BF550`
- `inputs/decompiler/gog/DP_decompiled.c`: `006C0E30`, `006BF060`
- `inputs/decompiler/{steam,gog}/calls.csv`
- `findings/boundaries/clevel_virtual_attachment_selection.md`

## Scope correction — 2026-10-02

Raw `.rdata` pointer scans show that `006BF550/006BF060` do **not** occur in the CLevel vtables. They are paired shared actor-family helpers, conditionally reached from virtual `+0x50` handlers `00417650/00417620` in twenty actor-family tables per build. The compatible CLevel offsets support only a `STRONG_INFERENCE` of shared layout; no direct CLevel-to-helper call is recovered. CLevel slot teardown `006C1320/006C0E30` remains verified. See `shared_actor_attachment_interface_correction.md`.
