# Whole-Program Structural Overview

**Reviewed:** 2026-10-01
**Phase:** 1 structural census
**Evidence state:** structural facts are `VERIFIED`; semantic interpretations are labeled separately.

> **Phase 1 closeout update (2026-10-01):** This map's early quantitative snapshots are historical. The authoritative Phase 1 endpoint is `reports/PHASE1_CLOSEOUT_2026-10-01.md`: 4,620/20,252 classified, 15,632 explicit UNKNOWN, 344 class records, 170 homology groups, and 47 boundaries. The paired selector factory and high-fanin registry source have since been structurally censused; Phase 2 begins at `maps/OBJECT_MODEL_OVERVIEW.md`.

## Build-separated executable shape

Steam and GOG are PE32 x86 executables with image base `0x00400000`, six Ghidra memory blocks, and the following matched major sections:

| Section | Steam range | GOG range | Structural role |
|---|---|---|---|
| Headers | `00400000-004003ff` | `00400000-004003ff` | PE headers |
| `.text` | `00401000-0076dfff` | `00401000-0076ddff` | Code and compiler/runtime support |
| `.rdata` | `0076e000-008847ff` | `0076e000-008841ff` | Read-only data, imports, strings, tables |
| `.data` | `00885000-014b26ff` | `00885000-014b26ff` | Writable globals and object/state storage |
| `.rsrc` | `014b3000-014b49ff` | `014b3000-014b49ff` | PE resources |
| `tdb` | `ffdff000-ffdfffff` | `ffdff000-ffdfffff` | Ghidra-mapped auxiliary block; semantics unresolved |

The exact ranges come from `memory_blocks.csv`; they are not inferred from similar addresses. Steam and GOG share data-section endpoints despite differing code and read-only-data extents.

## Function-universe baseline

| Build | Functions | Calls | Xrefs | Strings | Imports | Symbols | Decompiler errors |
|---|---:|---:|---:|---:|---:|---:|---:|
| Steam | 10,322 | 89,516 | 382,880 | 37,217 | 187 | 125,040 | 0 |
| GOG | 9,930 | 68,506 | 311,551 | 37,131 | 183 | 114,367 | 0 |

The canonical function identity remains `(build,address)`. A similar address in the other PC build is not a homologue without call/data/control-flow evidence.

## Coarse ownership map

The Phase 1 classifier uses only direct structural anchors:

- `CRT_STL_RUNTIME`: CRT/STL tail region and named runtime support.
- `THUNK_IMPORT_WRAPPER`: export-marked thunks.
- `PLATFORM_WIN32`: functions directly calling imported Win32/COM/shell/multimedia APIs.
- `RENDER_D3D9`: functions directly calling D3D9/D3DX9 imports.
- `PHYSX`: functions directly calling PhysX loader/cooking/character imports.
- `AUDIO`: functions directly calling X3DAudio.
- `MIDDLEWARE_STEAM`: functions directly calling Steam API imports.
- `UNKNOWN`: functions lacking a safe coarse assignment from current exports.

Combined counts are: 2,848 CRT/STL, 190 thunks, 329 Win32, 731 D3D9/D3DX9, 38 PhysX, 4 audio, 2 Steam API, and 16,110 explicit UNKNOWN. Direct API callers are boundaries, not automatically pure library code; game ownership remains possible at those call sites.

## Structural roots and call-graph anchors

### `VERIFIED` export facts

- `00401A70` is a recognized function in both builds with one incoming edge and 49 outgoing call edges.
- Steam `006C5FF0` and GOG `006C5AF0` are recognized functions with two incoming edges. The Steam body has 90 recovered direct outgoing edges; GOG records only 14 because `006C5AD0` is falsely marked `noreturn` and the visible dispatcher continuation is split in the export. Raw assembly confirms the dispatcher setup and continuation shape; the GOG entry remains `STRONG_INFERENCE` until the split is normalized.
- Steam `006C5930` and GOG `006C5430` are recognized functions with 63/53 incoming and nine outgoing edges respectively. Prior research identifies the pair as the selector-0 native UI task factory; current structural evidence confirms high reuse, not the full UI contract.
- Steam `004E3280` is recognized with one incoming and 90 outgoing edges. The GOG prior counterpart `004E3350` is not a current function entry in the supplied GOG function export and is retained as an unresolved build-specific address claim rather than silently inserted.
- The CRT startup symbols are Steam `___tmainCRTStartup` at `007513A8` and GOG `___tmainCRTStartup` at `007510B8`; both are in the runtime tail and connect the PE entry path to CRT initialization.
- `CreateObject` is a named Steam symbol at `00408070`; the corresponding GOG address is not a current function entry under that name in the supplied index.

### `STRONG_INFERENCE` candidates

- `00401A70` is now a verified application-to-dispatch call-loop boundary with one caller and 49 outgoing edges in each build. Its status as the only frame root remains open.
- The `006C5FF0`/`006C5AF0` pair is a verified Steam / strong GOG generic object-dispatch entry from prior static work plus direct assembly. GOG function-boundary recovery is incomplete because of the false `noreturn` split at `006C5AD0`.
- The `006C5930`/`006C5430` pair is a native UI/task factory candidate from prior reports, direct selector/size/constructor evidence, and high fan-in.
- Direct D3D9/D3DX9, PhysX, X3DAudio, Steam, and Win32 API callers form coarse subsystem boundaries. They do not prove ownership of every referenced field or object.

## High-information unknowns

The largest non-runtime fan-outs are:

| Build | Address | Body bytes | Outgoing call edges | Current status |
|---|---|---:|---:|---|
| Steam | `00647730` | 24,286 | 951 | `UNKNOWN`; likely high-level initializer/dispatcher or export artifact |
| GOG | `00647680` | 24,264 | 951 | `UNKNOWN`; structurally adjacent to Steam candidate |
| Steam | `00642640` | 18,198 | 834 | `UNKNOWN`; large central function |
| GOG | `00642590` | 18,156 | 832 | `UNKNOWN`; large central function |
| Steam | `00614F00` | 17,568 | 629 | `UNKNOWN` |
| GOG | `004213F0` | 20,449 | 759 | `UNKNOWN` |
| Steam | `0045C9F0` | 9,684 | 86; 353 incoming | `UNKNOWN`; high-centrality shared function |
| GOG | `0045CA20` | 9,684 | 86; 342 incoming | `UNKNOWN`; likely structural counterpart, not yet promoted |

The highest fan-in tiny functions are kept separate from high-information targets because they may be compiler helpers, thunks, or shared accessors: Steam `006C5FD0` has 8,594 incoming edges and 27 body bytes; GOG `006C5AD0` has 2,228 incoming edges and 27 body bytes. Their bytes and caller families must be inspected before assigning game semantics.

## Known subsystem seeds

The first subsystem candidates are recorded in `ledgers/SUBSYSTEM_LEDGER.csv`: application lifecycle, object dispatch, player/actor, resource loading, world streaming, event/script, save/GameRecord, input/camera, animation, PhysX, vehicle, effects, D3D9 renderer, native UI, audio, and Win32 platform. Only direct API boundaries and prior-research roots are seeded; no complete architecture is claimed.

## Next census operations

1. Inspect startup/entry and CRT-to-game transition around both `___tmainCRTStartup` symbols.
2. Compare `00401A70`, the dispatcher candidates, and high-fanout pairs using assembly, indirect-call tables, data references, and caller neighborhoods.
3. Extend the initial `.data` read/write aggregation and explicit `.rdata` vftable-symbol census into constructor/destructor and class ownership analysis.
4. Use resource extension strings and loader xrefs to begin `RESOURCE_TYPE_LEDGER.csv`.
5. Revisit coarse ownership only when a new structural discriminator exists; leave plausible but unsupported game interpretations as `UNKNOWN`.
