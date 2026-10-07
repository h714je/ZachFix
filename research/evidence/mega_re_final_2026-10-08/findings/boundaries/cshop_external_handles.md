# CShop factory-result retention and deletion interface

**Date:** 2026-10-02. **Outcome:** ADVANCE. Steam/GOG identities are separate below. No live cadence or indirect teardown invocation is claimed.

## Source contract

Selected `CSHOP_FACTORY_EXTERNAL_OWNERSHIP`: direct factory calls from Steam `calls.csv` (99 sites/51 callers) and GOG (91 sites/43 callers), included only where the exported caller explicitly supplies selector `0x34` and its result has checked raw pointer flow. The three constant-selector contexts per build occur in `00443060/004430E0` and `00647730/00647680`. Variable-selector callers are excluded, not proven incapable of producing CShop. Named CShop vtable bytes and deletion-entry bodies are included; generic offset scans and constructor enumeration are excluded.

## Positive connections

`VERIFIED`: paired callers preserve the selector result in ESI and pass it as ECX to `00633850/006337A0`. That method writes CShop setup fields `+0x150C`, `+0x1514`, `+0x152E/+0x152F`, and selection state; it is not invoked on the command/task receiver.

| Context | Steam factory/setup/get-handle sites | GOG sites | Retention |
|---|---|---|---|
| command 2000 | `00445687 / 004456A1 / 004456AD` | `00445707 / 00445721 / 0044572D` | packed handle -> caller `+0x10` and `00BDBEB8` |
| command `0x7D3` | `004457B7 / 004457C7 / 004457D3` | `00445837 / 00445847 / 00445853` | packed handle -> caller `+0x10` and `00BDBEC0` |
| native selector-0 callback state 10 | `0064C575 / 0064C587 / 0064C593` | `0064C4C5 / 0064C4D7 / 0064C4E3` | packed handle -> `014768F8`; state `01474CF8` becomes 11 |

`VERIFIED`: state 11 resolves `014768F8` through the active manager. Non-null lookup branches back to the callback's continuation; null lookup invokes **the task receiver's** virtual `+0x30`, not CShop's. Steam `0064C5D7–0064C5FA` and raw GOG `0064C527–0064C54A` match. GOG's export falsely treats `006C5AD0` as nonreturning and omits the test/virtual-call continuation; raw bytes recover it. This is a local export limit, not a new function inventory entry.

`VERIFIED`: handle-get wrappers `006C5FB0/006C5AB0` return `-1` for null object, otherwise forward the supplied object to its current-handle accessor. The stored values are handles, **not owned object pointers**. Existing selector registration mechanics connect successful CShop allocation/construction to manager registration; see `selector_object_factory.md` and `object_handle_lifecycle.md`. Retention and polling establish an external lifetime dependency, not an independent owning reference.

## Interface and teardown

`VERIFIED`: raw CShop tables `00780FBC/00780FAC` contain 19 function pointers (`+0x00..+0x48`, 0x4C bytes), followed by the next table's RTTI metadata. Slot zero is `00634830/00634780`, which calls `0062E1B0/0062E100` and conditionally frees the same `this` when deletion flag bit 0 is set. The destructor destroys the five embedded 0x178 records at `+0x18D8`, tears down two other members, restores CRdObject's vtable, and calls base teardown. This establishes deletion mechanics, not an observed invocation.

Other observed slots are inherited CRdObject entries, including `+0x2C -> 006BAAF0/006BAA40` (event 0x13 then helper), `+0x30 -> 006BAB20/006BAA70` (conditional retirement marker/event 2), and `+0x44 -> 00402910/00402900` (virtual `+0x28` staging wrapper). No class-specific update can be inferred merely from these inherited targets. **UNKNOWN:** CShop callback installation/use and class-discriminated retirement source. The next batch follows only the newly class-proven setup/control seam, not generic dispatcher callers.

## Primary references and reproduction

- Steam `inputs/decompiler/steam/DP_full.asm:80024-80048`, `:80108-80130`, `:657820-657863`; GOG `inputs/decompiler/gog/DP_full.asm:74834-74858`, `:74918-74940`, `:506674-506710`.
- Setup decompilation: Steam `inputs/decompiler/steam/DP_decompiled.c:356533-356653`; GOG `inputs/decompiler/gog/DP_decompiled.c:275210-275330`; ECX verified at the sites above.
- Destruction: Steam `inputs/decompiler/steam/DP_decompiled.c:353053-353083`, `:357189-357203`; GOG `inputs/decompiler/gog/DP_decompiled.c:273834-273864`, `:275866-275880`; assembly entries corroborate same-this forwarding and flag test.
- Raw table source: `inputs/binaries/steam/DP_STEAM.exe`, VA `00780FBC..00781007`; `inputs/binaries/gog/DP_GOG.exe`, VA `00780FAC..00780FF7`. Interpret little-endian pointers using PE sections; table extent excludes RTTI word at `+0x4C`.
- Recover omitted GOG continuation: `objdump -d -Mintel --start-address=0x64c527 --stop-address=0x64c54f inputs/binaries/gog/DP_GOG.exe`.

## Architectural gain and limits

The named menu class is now connected to two command paths and a native task's handle-poll lifetime dependency, rather than merely a selector constructor. Positive setup/retention plus a bounded table/delete interface advance Phase 2; this is **not yet** the rubric's complete active-object update-to-retirement chain. Constant-selector contexts are exhausted at this boundary. Reopen other factory-result contexts only with independently proven variable-selector value flow or a new caller. Continue callback/retirement work through the separate class-discriminated seam.
