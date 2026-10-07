# DPserial Archive Stream Loader

**Reviewed:** 2026-10-01
**Builds:** Steam and GOG PC
**Evidence state:** `VERIFIED` for archive segment initialization, path normalization, stream open, seek, read, and size operations.

## Verified archive path

The initializer `FUN_006B1500` calls `FUN_006AFD60` with the literal base path `UPDATA/_FLINK/DPSERIAL`. `FUN_006AFD60` formats three numbered paths:

- `UPDATA/_FLINK/DPSERIAL.001`
- `UPDATA/_FLINK/DPSERIAL.002`
- `UPDATA/_FLINK/DPSERIAL.003`

Each path is passed to `FUN_006AF720`.

`FUN_006AF0C0` normalizes the supplied path. It resolves a configured path prefix when applicable, strips a drive-prefix component after `:`, and converts backslashes to forward slashes. The Steam and GOG implementations are structurally matched.

## Stream object operations

`FUN_006AF720` builds the normalized path and, when object field `+0x108` is null, opens the stream through the build-specific file-open helper (`FUN_00712DD0` Steam / `FUN_00712AE0` GOG). The returned `FILE*` is stored at `object+0x108`.

The same object family exposes:

- `FUN_006AF7B0`: closes the stream and resets stream state;
- `FUN_006AF7F0`: `fseek` on `object+0x108`;
- `FUN_006AF820`: `fread` from `object+0x108`;
- `FUN_006AF850`: seeks to end, reads `ftell`, then rewinds;
- `FUN_006AF8A0`: clears stream handle and sets `object+0x10C = -1`.

This establishes an actual archive/file I/O boundary, unlike the earlier `006B2C30` cache-record helpers.

## Resource-record lookup relationship

`FUN_006AFAD0` uppercases the requested key through `006B02A0`, hashes it through `006B0350` using seed `0x015A4E35`, folds the hash to an 11-bit bucket when requested, follows a bucket chain at the manager's `+0x0C` table, and compares candidate record keys. Candidate records are addressed from a base at `manager+0x04` with a `0x20` stride. The resolver can compare either the basename stored at record `+0x1C` or an alternate `0x30`-byte record-table location supplied by its third argument.

`006B2AD0` uses the resolver to obtain a record index; `006B2BA0` and `006B2C30` then return different fields from the associated `0x30`-byte record entries after checking validity. The exact relationship between the archive stream and the record table population remains unresolved, but both are now evidenced as one resource subsystem boundary.

## Verified archive record extraction

The parser/extractor layer is now visible in the same helper family:

- `006B0000` seeks through a record stream, reads a 0x100-byte header, a four-byte field, and a 0x10-byte framing header. It recognizes magic `0x31505A58`, the little-endian `XZP1` marker.
- `006AF5B0` initializes a zlib 1.2.1 `INFLATE_INIT` stream and inflates compressed data in bounded chunks, using the archive stream read helper.
- `006AFE40` handles record extraction. For raw records it reads the payload directly; for compressed records it either inflates into an output buffer or reconstructs an `XZP1`-framed buffer containing magic, lengths, and compressed bytes.
- `006B0490` and `006B0510` provide additional record-buffer/payload reads using the archive stream and the parser's record metadata.

This closes archive record/header parsing and raw/compressed payload extraction. It does not identify the extension-specific interpretation of an extracted XPC/XMD/XAM/XNV/XWP buffer.

## Typed-registration seam

`006B33F0` handles one record and `006B3760` handles a batch. Both build a zeroed 0x30-byte descriptor, invoke an indirect callback through the resource-manager object at `this+0x10` while holding the manager mutex at `+0x388`, and then commit the descriptor into the per-record table at `this+0x0C`. The record validity/reference field at `+0x2C` is incremented after successful registration.

This is direct evidence of a typed-registration seam after archive extraction. The outer manager initializer `006B2570` receives the record count and callback pointer, allocates the `+0x0C` table, stores the callback at `+0x10`, initializes support state, and creates the mutex at `+0x388`; `006B2670` performs cleanup. Startup calls it with `ECX` from `004051F0`, which initializes the `0x38C` `CSingleton<CRdData>` object and writes its vtable; see `findings/formats/resource_manager_root.md`. Registration invokes the constructor-supplied callback as `(0, descriptor, record_id)` while holding the mutex, then commits the descriptor and increments validity/reference state.

The callback implementation, concrete descriptor fields, and extension-specific resource class remain unresolved. The batch parser constructs a separate local CFlkUtil archive utility (`006B4B80` Steam / `006B4AE0` GOG) on its stack; its destructor (`006B4BE0` / `006B4B40`) writes `CFlkUtil::vftable` at Steam `0x008268E4` / GOG `0x008268D4`. This local utility is distinct from the outer manager.

## What remains unresolved

- The manager root/global owner and startup sequencing beyond `006B2570`.
- The exact mapping performed by `006AF930/006AFAD0` between segment/index records and the manager table.
- Descriptor field semantics at `+0x18`, `+0x1C`, `+0x20`, and `+0x2C` as consumed by typed branches.
- Typed payload parsers and constructors for XAM/XCA/XFE/XNV families and the concrete class/vtable of objects registered after lookup.
- Whether the three segments are concatenated, independently indexed, or selected by resource class.

No typed resource factory is promoted solely from archive extraction evidence.
