# XPC positive resource portfolio and ownership split

**Date:** 2026-10-02. **Outcome:** ADVANCE at the integrated owner/interface scope; archive/parser/name-consumer anchors already existed and are not counted as new discoveries.

## Contract and provenance

Selected `RESOURCE_XPC_PORTFOLIO_PRIMARY_RECHECK`: Steam-first named archive/callback/descriptor/+0x1C/LEVEL.XPC/CLevel chain. Discriminator is exact wrapper pointer and manager-copy/consumer-ECX provenance, plus the raw typed deletion entry, not a parser/registry census. Reuse H0207/H0208 and existing paired resource correspondences; inspect GOG deeply only at foundational type/owner/receiver anchors. Success: resolve manager-owner versus consumer-retainer split and a distinct positive resource chain. Negative: identify the first missing/untyped edge. No new generic vtable/offset scan.

## Positive chain

`VERIFIED` mechanical edges, with explicitly conditional success paths:

1. **Archive/lookup:** CRdData `006B33F0` obtains the selected archive record through `006B0000` or its resolved alternate-source path. Existing `dpserial_archive_loader.md` establishes the DPSERIAL segment/header/raw-or-zlib extraction seam. This batch does not reparse assets or claim every source goes through the same physical segment.
2. **Typed construction:** startup-installed callback `00408310/004082D0` sees `.XPC`, allocates **0x10** bytes, writes named CRdPicture table `0076F2B4/0076F2A4`, zeroes +4/+8 and sets +0x0C=-1. Raw assembly passes that allocated pointer as ECX to XPC parser `006B5190/006B50E0`, and preserves the same pointer into descriptor +0x1C. The callback's descriptor +0x18 is separately replaced by normalized resource state. These are distinct fields/objects, not interchangeable buffer pointers.
3. **Manager owner:** on nonzero callback result, mutex-protected `006B33F0` copies **12 dwords/0x30 bytes** from the local descriptor into `manager[+0x0C] + record_id*0x30`, then increments the +0x2C validity/count field. Thus the typed wrapper in descriptor +0x1C becomes the manager's retained record pointer. Manager callback event 1 clears +0x1C and invokes the wrapper's slot zero with flag 1. That is a positive manager-side release/deletion interface, not merely a parser-local temporary.
4. **First concrete consumer:** named `LEVEL%03d.XPC` in `005DCA50/005DCB20` resolves via `006B2C30` -> `006B2C70`; the latter returns **record +0x1C**, conditional on range/nonzero +0x2C. Paired creation selects `0x28` CLevel. Raw callsite loads the retained CLevel pointer as ECX and forwards XMD/XPC lookup results; `006BE6E0/006BE1F0` writes them to CLevel +0x160/+0x164. H0207/H0208 are reused at this exact proven scope.
5. **Release limit:** callback event-one mechanics and typed deletion target are positive, but the particular unload caller/count policy that ensures CLevel +0x164 stops being used before manager deletion is UNKNOWN. Retention is not proof of an owning reference acquired by CLevel. The first missing edge is dependency/lifetime coordination between manager unload and this concrete consumer, not absence of all release mechanics.

## Selected non-dispatcher interface

`VERIFIED`: CRdPicture's observed **one-slot** typed-wrapper table ends before the next RTTI metadata word. Steam slot `0076F2B4 -> 00408900`, GOG `0076F2A4 -> 004088C0`. Each target rewrites the class table, invokes `006B54B0/006B5400` cleanup, and conditionally frees the same this when deletion flag bit 0 is set. Callback event-one supplies flag 1 and the exact stored wrapper. Construction/destruction, table extent, concrete target, direct class-discriminated indirect call context, and manager-owner/consumer-retainer split are now integrated rather than left as separate annotations.

This 0x10 callback-created wrapper must not be merged with the movie root's separate 0x20 surface/helper allocation merely because both interact with picture/renderer helpers. The latter's concrete type is still UNKNOWN in the movie finding.

## Primary anchors

- Typed construction/parser/commit: Steam `inputs/decompiler/steam/DP_full.asm:8874-8902`; GOG `inputs/decompiler/gog/DP_full.asm:8844-8872`; decompiler Steam `inputs/decompiler/steam/DP_decompiled.c:7559-7575`, GOG `inputs/decompiler/gog/DP_decompiled.c:7513-7529`.
- Manager callback/copy: Steam `inputs/decompiler/steam/DP_full.asm:770562-770605`; GOG `inputs/decompiler/gog/DP_full.asm:594438-594481`. Steam `inputs/decompiler/steam/DP_decompiled.c:429386-429509` identifies archive lookup and conditional successful commit. The identical addresses here were independently checked, not assumed.
- Record accessor: Steam `inputs/decompiler/steam/DP_decompiled.c:428875-428922`; GOG `inputs/decompiler/gog/DP_decompiled.c:326703-326729`.
- Named consumer: Steam `inputs/decompiler/steam/DP_decompiled.c:302959-303034`; raw `inputs/decompiler/steam/DP_full.asm:534768-534782`; GOG raw `inputs/decompiler/gog/DP_full.asm:398470-398484`. Concrete field stores: Steam `inputs/decompiler/steam/DP_decompiled.c:437023-437038`; GOG `inputs/decompiler/gog/DP_decompiled.c:334556-334571`.
- Callback release: Steam `inputs/decompiler/steam/DP_decompiled.c:7642-7652`; GOG `inputs/decompiler/gog/DP_decompiled.c:7596-7606`. Typed deleting entries: Steam `inputs/decompiler/steam/DP_decompiled.c:7736-7751`; GOG `inputs/decompiler/gog/DP_decompiled.c:7690-7705`; raw table pointers decoded separately from `inputs/binaries/steam/DP_STEAM.exe` / `inputs/binaries/gog/DP_GOG.exe` at the VAs above.
- Existing archive/startup provenance: `findings/formats/dpserial_archive_loader.md`, `findings/formats/typed_resource_dispatch.md`, `findings/formats/resource_manager_root.md`. Existing H0207/H0208 and `findings/boundaries/crdpicture_clevel_resource_attachment.md` remain valid at their declared scopes; prior informal class/resource citations are replaced with this normalized primary-backed integration report.

## Architectural outcome and carry-forward

A distinct resource chain now records archive/lookup -> typed wrapper -> manager record retention/release interface -> named CLevel consumer. The new owner/interface reduction is **manager owns the callback-created wrapper; CLevel visibly retains the returned pointer but independent ownership/dependency coordination is unproven**. Existing field names remain conservative. Release-to-consumer coordination is a non-blocking later lifecycle question for this portfolio, carried by `XPC_CLEVEL_UNLOAD_COORDINATION`; it cannot be used to claim full world/resource ownership closure.

Phase 2 closeout still requires all-major-root visibility, active-dispatch interface integration, and a targeted foundational red-team decision. The next step is the scheduled strategic/readiness review, not an automatic claim of phase completion.
