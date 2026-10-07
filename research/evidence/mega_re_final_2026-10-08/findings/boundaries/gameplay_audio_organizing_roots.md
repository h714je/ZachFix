# Independent gameplay-audio organizing roots and conditional use

**Date:** 2026-10-03. **Frontier:** AUDIO_ROOTS. **Outcome:** ADVANCE. C0118 / BND-086 / BND-087. This is independent of CRdMovie/DirectShow.

## Source contract

Start from Steam X3DAudioInitialize/Calculate callers `0072DA80/0072DB80`, follow only their concrete higher acquisition/use chains, then the sole game coordinator use source. Include typed tables/constructors/getter result flow and one startup-installed callback discriminator. Exclude middleware internals, movie wrappers, generic callback/global/offset censuses and accepted Phase 2 factory/dispatcher rediscovery. Consult prior ledgers first. GOG deepening is restricted to newly foundational CSdCore type/acquisition/constructor/static receiver correspondence; CSound/CSdMain and callback placement remain Steam-only in this batch.

## Organizing receivers — VERIFIED at mechanical/type scope

| Root | Concrete acquisition and type | Observed interface/layout | Ownership/lifetime limit |
|---|---|---|---|
| Steam `0138A6E0` -> CSound | `004183D0` allocates **0x68A0** through `_malloc`, publishes pointer, passes same EAX as ECX to `0046A7B0`; ctor installs `00773164`, raw COL 00876D80 -> TD 008A8370 -> `.?AVCSound@@`. | One observed code slot -> `0046FA50`; ctor initializes 256 records at +0x0C, stride 0x54, and other embedded arrays/volume interfaces. Getter supplies result to `0040CF30` via `00403690`; that tracking call is not assigned fuller ownership semantics. | Cached allocation and direct use proven. Slot performs destructor call plus optional flag-1 free; actual root-free/clear coordination UNKNOWN. Not the CRdSceneDraw manager and not a CSdCore alias. |
| Steam/GOG `0138A6E4` -> CSingleton<CSdCore> | Steam `0046F9D0`, GOG `0046FAD0`: allocate **0x50C**, retain allocator result in ESI, ECX=ESI -> ctor `0072BA00/0072B710`, install singleton table `00773154/00773144`, return/publish ESI via EAX. Raw singleton/base RTTI independently decoded. | Base tables `00828548/00828078`; ctor initializes 32 records at +4, stride 0x24, +0x484/+0x488 and 32 dwords at +0x48C..+0x508. Own one-slot tables are not the backend COM interface. | Getter exports falsely appear void and hide this/result flow. Distinct static constructor callers `0076D9B0/0076D6C0` pass **014B0400** to the same ctor. Backend globals are therefore not exclusive per-instance fields; actual static/lazy lifetime coordination UNKNOWN. |
| Steam `00BDBCC0` -> CSingleton<CSdMain> | `00427700` allocates **0x2838**, ECX=ESI -> `0071FDD0`, installs `007713FC`, publishes/returns saved ESI. Raw COL -> TD 008A68AC -> `.?AV?$CSingleton@VCSdMain@@@@`; base table `00828150` -> `.?AVCSdMain@@`. | Ctor initializes **128 records at +0x30, stride 0x50**, marks first dword -1, sets scalars +0x2830/+0x2834. Update `00720980` selects non--1 records, adjusts state with supplied scalar, and reaches `0072CB00 -> 0072DB80 -> X3DAudioCalculate`. | Singleton deletion slot `00427540` and base `00720CC0` call destructor then optionally free. No actual free caller/cache clear or exclusive sound-record resource ownership proven. |

Object roots and `014B0400` are .data virtual zero-fill; they are not raw on-disk pointer literals. No object size is inferred solely from a decompiler struct.

## Independent backend acquisition/use — VERIFIED mechanics; backend label STRONG_INFERENCE

CSdCore ctor `0072BA00` calls CoInitializeEx, then `0072D960(0,&014B01E4)`. The latter consults registry `Software\\Microsoft\\XACT` / `DebugEngine` and calls CoCreateInstance with concrete GUID data. Constructor reads `UPDATA\\SOUNDEX\\sound.xgs`, `InMemory.xwb`, streaming `.xwb` paths and `.xsb` paths including `bgm.xsb`, `se.xsb` and voice banks; retained backend slots +0x18/+0x24/+0x28/+0x2C are used. X3D initializer receives `[014B01E4]` and **014B01E8**. The registry, bank paths and imported X3DAudio interfaces support a **STRONG_INFERENCE XACT-oriented gameplay sound backend**, not a fully decoded API/format implementation.

`014B01E4` is a **shared COM/interface pointer cell**, not CSdCore this. `014B01E8` and calculation scratch `014B0320/014B0354/014B02F0` are also globals. CSdCore maintenance `0072C3F0`, called with the acquired core as ECX, conditionally calls backend virtual +0x20 with `[014B01E4]` as receiver. The exact backend operation name is not promoted. No audio timing, successful live initialization, all bank ownership or error recovery claim follows from these callsites.

## Concrete application/object use boundary — VERIFIED static conditional chain

1. Startup `004017C0` loads the accepted active manager `[00BD7670]`, calls selector-0 task factory `006C5930` at `004019CF`, passes result EAX as ECX to `006BAB80`, and supplies literal callback **00454D50** at `004019D5`. Factory selector-0 registration/base task and setter/event-0 mechanics are reused from H0202/C0116 and the existing native-task portfolio, not reopened as a generic seam.
2. **00454D50 is an orphan/raw callback block**, absent from Steam `functions.csv`. It dispatches byte argument event through map bytes **00455068** and target table **00455054**. Event **1** maps to **00454EC4**. Selected control flow converges on `00454F7F -> 004183D0`, ECX=returned CSound, then `00454F86 -> 0046F360`. Raw callback is not misassigned to preceding exported `00454C60` or invented as a function-ledger entry.
3. Reused CRdObject table +0x0C target `006BA8A0` emits event 1; the accepted manager phase-2 pass invokes that interface on selected registered objects (C0112/C0116). This supplies a **conditional static** application `00401A70` -> object dispatcher -> installed task -> raw callback -> CSound chain. It does not establish that every task is selected or a once-per-frame schedule.
4. `0046F360` saves the CSound receiver in ESI. Its first guard returns early if **008A6074 bit 31 is 1 AND 008A670C != 1**. Remaining human mode meaning is UNKNOWN.
5. On the continuing path, `0046F4F3 -> 0046F9D0`, ECX=EAX at `0046F4F8`, then `0046F4FA -> 0072C3F0` maintains the **core**. Next `014AFFE0` is passed to `00427700`, whose getter preserves it on the stack, ECX=returned **main** at `0046F516`, then `0046F518 -> 00720980`. The decompiler's void getters obscure these class-discriminated receiver transitions.

This callback is **not** the bounded `00647730` native-task availability question. Its new concrete audio use/installation source does not reopen that old generic search or resolve B0006. Live selection/cadence of this independently identified callback is separately B0008.

## Concrete world-state input — VERIFIED, BND-087

Within `0046F360`, raw `0046F385` tests absolute **01393700 == 3**, i.e. the accepted in-place CMap base **013936F0 +0x10**. Non-3 branches jump to `0046F4EB`; the conditional auxiliary CSound work is skipped but core/main maintenance still follows. This is a **typed world-root state -> audio control-flow input**, not a friendly name for CMap state 3, proof a level is loaded, or complete CMap update placement. C0115 root identity is reused; constructor/attach work remains closed. It provides a concrete reason to prioritize `WORLD_LEVEL_LIFECYCLE_PLACEMENT` next.

## Primary references and verification

- Steam CSound getter `inputs/decompiler/steam/DP_full.asm:28074-28107`, constructor `:122764-122944`, use `:128527-128830`; supporting `DP_decompiled.c:21378-21413`, `:81656-81787`, `:85755-85909`.
- Steam CSdMain getter `inputs/decompiler/steam/DP_full.asm:44307-44346`; constructor `:907579-907641`; update entry `:908647`; supporting `DP_decompiled.c:29923-29955`, `:500221-500256`, `:500867-500935`.
- Startup installed callback `inputs/decompiler/steam/DP_full.asm:673-682` (literal/root/factory context around 004019C4..004019DC); raw callback `:97467-97672`, especially audio call `:97608-97610`. The verifier's instruction receipt supplies exact line/VAs where these broad ranges include neighboring code. Raw switch table/map bytes are read from the executable.
- CSdCore paired anchors/type/limits: `reports/PHASE3_AUDIO_INDEPENDENT_TYPE_REVIEW_2026-10-03.md`, with direct source refs and independent main verification. Acquisition H0266, ctor H0267, static caller H0268 are limited to this exact mechanical/type scope.
- `scripts/inspect_phase3_audio_roots.py` / `scripts/phase3_primary_tools.py`: **1,337 Steam use-chain instructions / 4,895 bytes**, plus **592 core acquisition/constructor/static-caller instructions / 2,400 bytes per build**, against primary PE bytes. Raw RTTI, selected call targets, publication operands, callback event map and no-export identity are asserted. Derived result: `reports/PHASE3_AUDIO_ROOT_RAW_CHECKS_2026-10-03.json`.

## Architectural gain and first missing edges

A previously unserved AUDIO family now has independent typed organizing roots and a positive conditional startup/object/frame-use boundary, not a movie-audio substitution. CSound game coordination, CSdMain record control, CSdCore/backend support are separate receivers. Current-primary AUDIO acquisition/use obligation is reduced at Steam scope, with foundational core correspondence in GOG.

**UNKNOWN:** actual audio root shutdown/reset/cache-clear and shared backend coordination with static 014B0400; full named mode semantics; safe bank/resource release; GOG outer use/callback placement; detailed slot/record algorithms and live frequency. `AUDIO_SHUTDOWN_COORDINATION` carries the typed lifetime continuation (unvisited, not bounded). B0008 requires reproducible callback/root/mode timestamps. Reopen the selected acquisition/use result only on conflicting primary receiver/type/caller evidence or a raw boundary expanding this exact chain. These are not Phase 3 closeout or Phase 4 mechanism results.
