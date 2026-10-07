# Resource-Path Anchors: XPC/XMD/XAM Families

**Reviewed:** 2026-10-01
**Evidence state:** `VERIFIED` for observed path/table operations; loader/factory ownership is `STRONG_INFERENCE` or `UNKNOWN` unless stated.

## Regional menu/resource table initialization

Steam `00428F30` and GOG `00428F60` initialize a structure with defaults, allocate a `0x4400` byte table, and populate entries containing resource IDs and literal `.XPC` paths such as regional `PS3EYE_CHECK` and `OPTION05` files. The function branches on `DAT_00BE58E4` and writes related manager fields. This is a table initializer/resource manifest builder, not evidence of parsing XPC payloads.

## Character model/package path selection

Steam `004C3240` and GOG `004C3320` select paired `.XMD` and `.XPC` paths from a subtype byte at `this+0xD54`, writing them into caller-provided buffers. The cases shown include `CBS0211`, `CNM0231`, `CBS0241`, `CBS0251`, `CBS0111`, `CBS0121`, and `CBS0131`. The default path delegates to another helper with additional subtype bytes.

This directly connects character subtype selection to model/package resource names. It does not prove that this function opens, parses, allocates, or owns the resulting model object.

## Item/resource cache lookup and object setup

Steam `00627240` and GOG `006271C0` derive basenames from literal item paths and query resource records through `006B2C30`/related helpers. `006B2C30` performs an index lookup through `006B2AD0` and returns a record field through `006B2C70`; the sibling `006B2BA0`/`006B2BE0` path returns a different record field. These helpers establish cache/record access, not file opening or payload parsing.

The previously suspected `0040A2F0`/Steam counterpart is not a resource request helper: the reviewed body initializes the `CSingleton<CGame>` object and writes its vtable. `00627240`/`006271C0` then create/configure an object through `006C55E0`/`006C5AE0` after setting packet fields, but the exact object class and ownership remain unresolved.

The same function family references `.XAM`, `.XMD`, and `.XNV` strings in the surrounding item/resource initialization region. It should be compared with constructor/vtable writes before being classified as a factory.

## Implications for the resource ledger

- Literal `.XPC`/`.XMD` xrefs frequently identify path selection or table initialization rather than payload parsing.
- Asset extension identity, path selection, existence checks, asynchronous requests, and typed construction are separate evidence layers.
- The current `RESOURCE_TYPE_LEDGER.csv` preserves those layers and leaves `factory_or_constructor`/`consumers` empty until object ownership is established.

## Next discriminators

1. Inspect `006B2AD0` and the surrounding resource-record table for archive/file-open boundaries, type dispatch, allocation, and vtable writes.
2. Follow the object created by the `006C55E0`/`006C5AE0` registration path to its vtable and manager list.
3. Compare the paired Steam/GOG resource paths and xrefs against asset directory names, including regional and character-family differences.
