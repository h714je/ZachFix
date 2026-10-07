# Shared Actor Attachment Interface — Scope Correction

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the shared virtual-interface mapping and paired helper bodies; `STRONG_INFERENCE` for a common attachment-compatible layout shared with CLevel.

## Correction

The prior CLevel selected-attachment lifecycle slice correctly established CLevel slot teardown (`006C1320/006C0E30`) and its selection fields. It overstated the scope of paired `006BF550/006BF060` as CLevel methods.

Raw binary pointer scans show neither address occurs in the CLevel vtables. Instead, each PC binary contains twenty `.rdata` function-pointer occurrences for the paired virtual handler Steam `00417650` / GOG `00417620`, all at vtable byte offset `+0x50`. Directly identified table owners include CCharacter, CNpcAnimal, CNpcBird, CNpcDog, CNpcEnemy, CNpcMob, CNpcNormal, CMustache, CPlayer, CNpcKiller, and CFishingPerson, plus unnamed tables.

`00417650/00417620` conditionally invoke `006BF550/006BF060` while handling a resource/state branch, along with other object/attachment helpers. Their vtable occurrence demonstrates a shared actor-family interface, not CLevel ownership or direct scheduler placement.

## Retained and corrected facts

- **Retained `VERIFIED`:** CLevel `006C1320/006C0E30` clears matching selected attachment state and tears down one CLevel attachment slot.
- **Retained `VERIFIED`:** `006BF550/006BF060` are paired attachment-aware transform update helpers that read fields at the same offsets (`+0x160`, `+0x170`, `+0x174`, `+0x280`, `+0x2D4`) and perform no direct D3D9/D3DX call.
- **Corrected to `STRONG_INFERENCE`:** identical offsets suggest a shared attachment-compatible object layout, but no direct CLevel caller or CLevel-vtable entry reaches `006BF550/006BF060` in the supplied static corpus.
- **New `VERIFIED`:** `00417650/00417620` are a shared actor virtual `+0x50` implementation occurring in twenty raw vtable locations per build and conditionally call the transform helpers.

## Temporal limit

No direct call connects the frame scheduler `00401A70` or its generic active-object dispatcher to this virtual `+0x50` handler. Frame-phase cadence remains `UNKNOWN`; the next discriminating source is an indirect-call/dispatcher slot census, not additional direct caller searches.

## Evidence references

- raw scans of `inputs/binaries/steam/DP_STEAM.exe` and `inputs/binaries/gog/DP_GOG.exe`
- CCharacter/CPlayer/NPC/CFishingPerson vtable records
- `inputs/decompiler/steam/DP_decompiled.c`: `00417650`, `006BF550`
- `inputs/decompiler/gog/DP_decompiled.c`: `00417620`, `006BF060`
- `findings/boundaries/clevel_selected_attachment_lifecycle.md`
