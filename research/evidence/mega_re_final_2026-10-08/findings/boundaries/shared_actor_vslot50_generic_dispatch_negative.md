# Shared Actor Virtual `+0x50` — Generic Dispatch Negative

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for generic active-object dispatch slot coverage; handler frame-phase placement remains `UNKNOWN`.

## Bounded source universe

This batch compared the paired scheduler-driven generic active-object dispatchers Steam `006C5FF0` / GOG `006C5AF0` against the shared actor-family virtual `+0x50` handler `00417650/00417620`.

Both dispatchers iterate active objects from the manager and execute explicit phases through slots:

`+0x0C`, `+0x10`, `+0x14`, `+0x18`, `+0x1C`, `+0x30`, and `+0x48`.

Neither body performs an indirect call at `+0x50`.

## Consequence

`00401A70 -> 006C5FF0/006C5AF0` establishes a recurring scheduler-to-active-object update boundary, but does not directly establish that the shared actor virtual `+0x50` handler runs in that generic frame phase.

A decompiled textual census found 171 Steam and 153 GOG functions containing an indirect `+0x50` expression. Without a class/vtable discriminator, those results span unrelated interfaces and cannot be treated as call sites for `00417650/00417620`. This is a bounded static negative result, not evidence that the handler is never called per frame.

## Follow-up limit

Further static placement requires a class-discriminated indirect-call site, a newly recovered dispatcher, or runtime traces. The current CLevel/actor attachment family should not be extended by generic offset searching.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c`: `006C5FF0`
- `inputs/decompiler/gog/DP_decompiled.c`: `006C5AF0`
- `inputs/decompiler/{steam,gog}/DP_full.asm`
- `findings/boundaries/shared_actor_attachment_interface_correction.md`
