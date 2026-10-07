# Phase 6 sequence 143 — GOG `0040A2F0` accessor qualification card

**GOG_PC only.** Frozen residual rank 8 / metadata 115 bytes / high fan-in was selected as an independent build-specific test. Anti-loop found an existing format report naming the body as a `CSingleton<CGame>` initializer, but the function ledger remained UNKNOWN and no canonical focused identity was present. Primary exact source/PE receipt: `scratch/phase6_gog_accessor_personal_seq0144.json`.

## VERIFIED own body and table

The complete exported interval is `[0040A2F0,0040A363)`, 115 bytes, 35 instructions; PE and Ghidra ASM agree exactly. The body checks cached pointer `00BDA004`, and on null requests `0x838E30` through `00404370`, calls `00451DC0` on the result, writes table pointer `0076F66C` at `[ESI]`, and publishes the result back to `00BDA004`. It restores the guarded stack and returns the cached/new pointer. This is construction/publication/accessor shape, not a general resource request helper.

Primary table metadata at `0076F66C` has locator `008755F0`, type descriptor `0088709C`, decorated name `.?AV?$CSingleton@VCGame@@@@`, and one observed executable slot `00409FE0`. The one-slot extent is an observation, not a universal interface definition. `00BDA004` is a cached CGame-related pointer at this selected body; complete ownership, destructor, current generation, caller state or service policy remain UNKNOWN.

## Disposition

**VERIFIED bounded identity:** this build-specific residual is a cached CGame singleton construction/accessor, consistent with the older format-anchor report and not a new cross-subsystem manager or callback service. **STRONG_INFERENCE:** its 4,860-call popularity reflects a shared accessor boundary, not evidence of every caller's semantics. No Steam homologue is inferred and no GOG/Steam address transfer is made.

A first source edge that could change architecture is an owning destructor/retirement path or a typed caller state transition; neither was acquired. No support body, caller, GOG runtime, Steam body or canonical ledger was changed. A first metadata command failed by attempting to interpret a zero immediate as a mapped table address; it was corrected by selecting the exact `[ESI]` vtable write only. The failure is preserved in the JSON receipt and earns no evidence credit.

Phase 5 remains closed; all 88 guards and Phase 7 prohibition remain.
