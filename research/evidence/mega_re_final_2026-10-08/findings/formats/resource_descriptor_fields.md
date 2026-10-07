# Resource Descriptor Field Map

**Date:** 2026-10-01

## Archive-record index fields

**VERIFIED** from `006AF930/006AFAD0`:

- Archive candidate records use a `0x20` stride from the archive record base at manager-side `+0x04`.
- Archive record `+0x18` is used as a manager-record index: `006AFAD0` computes `manager_record_base + archive_record[+0x18] * 0x30` when resolving through a manager table.
- Archive record `+0x1C` supplies the stored key/path string. `006AF930` compares its basename against the normalized lookup key; `006AFC90` returns the same field for descriptor construction.
- The bucket-chain entry stores a hash and archive-record index pair; the resolver uses the hash first and then string-comparison confirmation.

## Callback descriptor fields

`006B33F0` zeroes a local `0x30`-byte descriptor before extraction and passes it to the startup callback `(0, descriptor, record_id)`:

- `+0x00..+0x17`: uppercase basename copied by `006B31F0` from the extracted/archive path basename. This is the descriptor key/name area.
- `+0x18`: initialized by the descriptor path and then written by typed callback branches with callback-owned resource state/payload. XMD and XPC branches read the pre-callback value as the extracted payload pointer and replace it with the resulting resource state.
- `+0x1C`: callback-owned object slot. XMD writes a CRdMesh pointer; XPC writes a CRdPicture pointer; mode-one cleanup reads this slot as a vtable-bearing object.
- `+0x20`: auxiliary archive/path metadata passed to typed helpers. The descriptor builder obtains it from the archive record/path helper; exact field type remains unresolved.
- `+0x2C`: validity/reference count in the manager table after successful registration. The registration path increments it under the mutex; byte subfields near the descriptor tail also carry callback/platform metadata, but their exact independent meanings remain open.

The callback result is copied as twelve dwords into the manager table at `+0x0C + record_id*0x30`. `006B2BE0`, `006B2C70`, and `006B2D00` expose distinct manager-table fields for typed consumers. The paired `006B2CC0` wrapper returns the `+0x20` field after validity/index checks. Both PC call manifests contain only the wrapper's internal call to `006B2D00`, and targeted raw executable scans find no literal occurrences of either accessor address; no recoverable direct or address-taken consumer exists in the supplied static corpus. The field subtype remains `UNKNOWN`.

## Confidence and limits

Archive-record index/key roles and callback object/state slot roles are `VERIFIED` from direct field use. The auxiliary metadata and tail-byte semantics remain `UNKNOWN`; no class or format is inferred solely from the fixed stride.

## Phase4 Steam selected request-control subfields

**VERIFIED Steam, not transferred to GOG:** the selected CLoadThread direct/queued argument route supplies an opaque control8 to `006B2780` and then `006B33F0`. Callback-success commit writes that exact byte at descriptor `+0x2E`, numeric status2 at `+0x2F`, copies twelve dwords to manager `[+0x0C]+index*0x30`, and increments the **word** reference count at `+0x2C`. This narrows the selected tail's data widths/value provenance, not its full named tag/group/format semantics. In particular descriptor `+0x20` remains under C0044's original accessor bound, and pending clear/worker return is not resource success. Source and exact ABI/identifier limits: `findings/boundaries/resource_worker_typed_handoff.md`.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:426710-426809`
- `inputs/decompiler/steam/DP_decompiled.c:429236-429261`
- `inputs/decompiler/steam/DP_decompiled.c:429388-429508`
- `inputs/decompiler/steam/DP_decompiled.c:428828-428968`
- `inputs/decompiler/steam/DP_decompiled.c:7489-7612`
- `findings/formats/resource_manager_population.md`
- `findings/formats/resource_manager_root.md`
