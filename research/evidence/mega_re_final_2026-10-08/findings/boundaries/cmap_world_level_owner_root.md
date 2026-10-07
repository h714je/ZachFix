# CMap static world/level organizing root

**Date:** 2026-10-02. **Frontier:** WORLD_LEVEL_OWNER_ROOT. **Outcome:** ADVANCE. Evidence states below apply to particular mechanics, not full world-system semantics.

## Contract and source boundary

Question: what concrete object supplies the incoming receiver of Steam `005DCA50`? Universe: the five calls in `calls.csv` owned by `005D26B0/005E10A0`, their raw receiver definitions, and only static construction/table/RTTI/finalization edges of the resulting receiver. Reuse H0207 at named-resource-consumer scope; do not rederive XPC parsing, getters, selector creation, or CLevel setup. GOG inspection is justified for the newly foundational root/type/constructor correspondence and orphan-call limit. Success: root identity and acquisition/storage provenance. Negative: record the first missing receiver edge, without an untyped offset census.

## Receiver and storage — VERIFIED

| Build | Attach sites | Immediately preceding receiver definition |
|---|---|---|
| STEAM_PC | 005D3EFF, 005E1229, 005E1243, 005E1299, 005E12A8 | `MOV ECX,013936F0` at each site minus five bytes |
| GOG_PC | 005D3FCF, 005E12F9, 005E1313, 005E1369, 005E1378 | Same independently checked constant at each site minus five bytes |

`013936F0` is the **object's in-place base**, not a pointer cell to dereference. Its `.data` storage and raw table installation distinguish it from lazy allocated `00BD7670`, from CLevel objects, and from the large state returned by `0040A320`. The attach method indexes base `+0xA3F98 + 4*index`, hence absolute **01437688**. Paired constructors clear **0x40 dwords** there. At the already evidenced successful resource/factory boundary H0207, this slot retains the registered CLevel result. The Steam step routine visits 0..0x3F (or advances its caller's +0x32C cursor under gated paths); its own incoming receiver is not substituted for the fixed CMap attach receiver. The first caller's event-code taxonomy is not inferred here.

## Concrete type, construction and interface — VERIFIED

- Steam `005D0680`, GOG `005D0750` preserve incoming ECX as ESI and install tables **00779AB4 / 00779AA4**. They initialize the 64-pointer array and expose addresses of object +0x14/+0x18 through globals 013936E0/013936E4. Highest reviewed dword store at +0xA40BC gives **size lower bound 0xA40C0**, not a proven allocation size.
- Raw complete-object-locator chains point through Steam 00877F64 / GOG 00877A9C to independently decoded type descriptor strings **`.?AVCMap@@`**. This supports the imported CMap symbols rather than trusting their spelling alone.
- Observed contiguous table function extent is **one slot**, Steam `005D07A0`, GOG `005D0870`; following words are float/data constants, not executable pointers. Slot-zero rewrites the CMap table, tears down embedded **CTargetManager at +0xA3F04**, and conditionally frees this when deleting flag bit 0 is set. This is not evidence that the static root is ever heap-deleted.
- Raw non-exported static construction stubs **0076B720 / 0076B430** pass 013936F0 directly to the paired constructors, then register **0076DC50 / 0076D960** through exported `_atexit` at **0074F52B / 0074F23B**. Pointer words to the stubs occur in `.rdata` at 0076E398 / 0076E384. No function-inventory entries are invented for these raw stubs, and complete CRT initialization execution/order is not claimed from this pointer alone.
- Registered finalizers rewrite the fixed base's CMap table and pass absolute **014375F4 = base +0xA3F04** to CTargetManager teardown `005E7430/005E7500`. This is a positive static finalization registration/embedded-cleanup mechanism; it is not level-array teardown or runtime shutdown proof.

## Primary anchors and reproducibility

- Steam five-call receiver universe: `inputs/decompiler/steam/DP_full.asm:524840-524873` and `inputs/decompiler/steam/DP_full.asm:539349-539409`; GOG orphan and step anchors: `inputs/decompiler/gog/DP_full.asm:389226-389250` and `inputs/decompiler/gog/DP_full.asm:402788-402844`. Direct call inventory: each build's `calls.csv`.
- Construction/slot-zero: Steam `inputs/decompiler/steam/DP_full.asm:521165-521257`; GOG `inputs/decompiler/gog/DP_full.asm:386192-386284`. Supporting Steam decompiler: `inputs/decompiler/steam/DP_decompiled.c:295252-295333`.
- Static stubs/finalizers: Steam `inputs/decompiler/steam/DP_full.asm:992080-992085` and `inputs/decompiler/steam/DP_full.asm:994150-994156`; GOG `inputs/decompiler/gog/DP_full.asm:813893-813898` and `inputs/decompiler/gog/DP_full.asm:815964-815969`. `_atexit` identities: corresponding `decompile_index.csv`; embedded target name/body: Steam `inputs/decompiler/steam/DP_decompiled.c:310869-310881`, GOG `inputs/decompiler/gog/DP_decompiled.c:239815-239827`.
- Raw primary source: `inputs/binaries/steam/DP_STEAM.exe` and `inputs/binaries/gog/DP_GOG.exe`. `python3 scripts/inspect_phase2_world_owner.py` independently decodes PE sections, RTTI/table words, checks all ten callsite receiver bytes, verifies both construction-stub relative call targets/finalizer operands, and finds the stub pointer locations. Derived output: `reports/PHASE2_WORLD_OWNER_RAW_CHECKS_2026-10-02.json`. Selected stub disassembly was also checked with objdump; supplied bytes/exports are unchanged.

## Architectural consequence and red-team limits

**VERIFIED:** a static CMap organizes/retains the named CLevel array; it is not itself proven independently registered through the active-object manager. This resolves the first missing organizing-root identity/acquisition edge in the Phase 2 matrix. H0256-H0258 cover constructor, deleting interface, and registered finalizer mechanics; H0207 remains the narrower attach mapping. GOG `005D3FCF` has no exported caller owner; no full 005D26B0 homologue is manufactured. Static stubs are raw blocks, not exported functions.

**UNKNOWN:** complete CMap mode/update placement relative to application/object phases; level-array retirement/clear/resource dependency coordination; exact full CMap size; complete CRT lifetime order; whether an indirect deleting invocation ever targets this static root; detailed embedded CTargetManager ownership. Pointer retention does not prove exclusive CLevel lifetime ownership. Level teardown is separate from embedded target cleanup.

**Carry-forward:** `WORLD_LEVEL_LIFECYCLE_PLACEMENT` tracks a concrete class-proven CMap call/update/reset edge for Phase 3; `XPC_CLEVEL_UNLOAD_COORDINATION` retains its original resource-versus-retainer discriminator. Neither blocks naming/acquiring this root, and neither is a waiver of the selected central active-manager interface still missing from Phase 2. Reopen WORLD_LEVEL_OWNER_ROOT identity only on conflicting primary receiver/type evidence or a raw boundary expanding the reviewed direct-call source, not another getter/constant scan. Xbox remains unmapped.
