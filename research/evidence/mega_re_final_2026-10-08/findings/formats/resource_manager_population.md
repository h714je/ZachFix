# Outer Resource Manager Population

**Date:** 2026-10-01

## Verified population path

The manager table rooted at `this+0x0C` is populated through paired single-record and batch paths:

- Steam/GOG `006B2730` first maps an incoming resource identifier through `006AF930`; a valid segment/index is passed to `006B2780`.
- `006B2780` checks segment state through `006AFCB0`, iterates segments with `006AFD10` when needed, and calls `006B33F0` for each candidate record.
- `006B33F0` obtains the archive record, builds a local twelve-dword (`0x30`-byte) descriptor, selects raw or alternate extraction through `006B0000` / `006AF370`, and only then acquires the manager mutex. It invokes the constructor-supplied callback at `this+0x10` as `(0, descriptor, record_id)`. A successful callback result copies the descriptor into the manager table at `+0x0C + record_id*0x30`, increments the record reference count at `+0x2C`, releases the mutex, and increments the active count at `this+0x08`.
- `006B3760` constructs a stack-local CFlkUtil, opens/iterates the archive metadata, filters entries by platform discriminator at `this+0x384`, builds the same twelve-dword descriptor, and invokes the same callback/table-commit path for each eligible record. CFlkUtil is destroyed on all completed paths and remains separate from the manager object.
- `006B3AA0` is the release path: it rejects absent or referenced entries, invokes the callback in mode one, frees the payload pointer at descriptor `+0x18`, clears the `0x30`-byte record, and returns success.

## Consequences

The `+0x0C` table is not merely an index cache: its active entries are callback-produced descriptors with reference counts and release-side callback ownership. The table population mechanism is now `VERIFIED` at the registration/writer level.

The startup/global root is now bounded to `CSingleton<CRdData>` through `004051F0 -> 006B2570`. Still unresolved are the exact descriptor auxiliary/tail field meanings at `+0x20/+0x2C` and the complete record-index mapping across all segment/index cases. See `findings/formats/resource_descriptor_fields.md`.

## Phase4 selected worker continuation / terminology correction

**VERIFIED Steam:** `01481130` is an in-place CLoadThread whose invoked start and distinct queued/direct request mechanisms hand low16 index/control8 to acquired CRdData `006B2780`. Successful callback registration commits that control8 at descriptor `+0x2E`, status2 at `+0x2F`, and updates word reference count `+0x2C`; worker return/pending clear is not registration success. See `findings/boundaries/resource_worker_typed_handoff.md` for exact type/ABI/value qualifications and unresolved lifetime/build edges.

The earlier wording “six-word (0x30-byte)” was imprecise: the verified commit copies twelve dwords / 0x30 bytes. That prose is corrected above without enlarging the accepted format or paired semantic scope; historical receipts remain unchanged.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:428591-428652`
- `inputs/decompiler/steam/DP_decompiled.c:429388-429632`
- `inputs/decompiler/gog/DP_decompiled.c:326398` and corresponding paired functions
- `findings/formats/dpserial_archive_loader.md`
- `findings/formats/typed_resource_dispatch.md`
