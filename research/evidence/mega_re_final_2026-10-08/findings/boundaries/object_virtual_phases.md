# Generic Object Virtual-Phase Boundary

**Addresses:** Steam `006C5FF0`; GOG `006C5AF0`  
**Evidence state:** `VERIFIED` for the visible paired dispatcher mechanics; `STRONG_INFERENCE` for generic actor-family consumer grouping.

## Direct static evidence

Steam `FUN_006C5FF0` at `inputs/decompiler/steam/DP_decompiled.c:440978-441306` constructs per-pass object sets, writes manager phase state at `this+0x18E0`, and invokes active-object vtable slots under visible gates. The visible late passes call:

- `+0x0C` after phase `2`;
- `+0x30` during the retirement-mark broadcast path;
- `+0x10`, `+0x14`, and `+0x18` in later gated passes;
- `+0x1C` with Event `6` after phase `8`;
- `+0x48` during the later object pass.

The paired GOG dispatcher begins at `006C5AF0`, but its decompiler extent is affected by the known false `noreturn` recovery at `006C5AD0`; its raw assembly continuation is the authoritative paired evidence for the later phase region. See `inputs/decompiler/gog/DP_full.asm:006C5AF0-006C6F17` and `findings/boundaries/application_object_dispatch.md`.

## Bounded conclusion

The dispatcher is a real active-object virtual-phase boundary. Slot calls and their visible static phase ordering are not inferred from names. The current ledger preserves `OBJECT_DISPATCH -> ACTOR_OBJECTS` as `STRONG_INFERENCE` because the complete class taxonomy and all concrete virtual targets are not recovered.

## Explicit limits

- This does not identify every class receiving each slot, every indirect entry route, or runtime cadence.
- It does not prove that all visible phases occur in every game state.
- It does not promote a final actor/gameplay subsystem ownership model from slot offsets alone.
