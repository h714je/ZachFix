# Typed Resource Extension Dispatcher

**Addresses:** Steam `00408310`; GOG `004082D0`
**Evidence state:** `VERIFIED` for callback role and extension dispatch; per-helper payload semantics are separately scoped.

## Construction contract

The main startup path at `004017C0` allocates/obtains the outer resource manager, pushes record count `0x4BD8` and a callback pointer, then calls `006B2570`. The callback argument is Steam `00408310` or GOG `004082D0`. `006B2570` stores it at manager `+0x10`; `006B33F0` and `006B3760` later invoke it as `(0, descriptor, record_id)` while registering extracted archive records.

## Verified dispatch behavior

With event/mode argument zero, the callback derives the final extension from the resource descriptor name and selects dedicated branches:

- `.XMD`: allocates a 0x54-byte helper object through `004087E0` and initializes record fields through `0070BD20`.
- `.XPC`: allocates/initializes a `CRdPicture` object, calls `006B5190`, and stores the returned resource state.
- Named non-extension resources: `PARKING.ROA`, `LOADPOS.ROA`, `PLACE.ROA`, `ITM_LIST.SRL`, `NPC_LIST.SRL`, `HOUSE_LIST.NOD`, and `SCREEN.PEA` each route to dedicated helpers.
- Explicit extension checks also cover `.XAM`, `.XFE`, `.XCM`, `.XUL`, `.XUS`, `.XCA`, `.POS`, `.XOP`, `.PRM`, `.MES`, `.FLG`, `.IDX`, `.TBL`, `.DIR`, `.DPB`, `.DSB`, `.XVO`, `.XWP`, `.XNV`, `.XMI`, `.GRS`, `.XLY`, `.XPF`, and `.XFX`.
- `.DSB` invokes a separate script-oriented helper; mode argument one performs XMD/XPC teardown and a DSB release-side call.

The extension branch itself is direct evidence. It does not establish that every named helper fully parses that file format, nor that all extensions produce a distinct C++ class.

## Concrete XMD/XPC classes

The XMD branch allocates a `0x54`-byte helper and calls `004087E0` Steam / `004087A0` GOG, whose constructor writes `CRdMesh::vftable` and constructs child arrays. The corresponding destructor candidates are Steam `00408860` / GOG `00408820`; vtables are Steam `0x0076F2AC` / GOG `0x0076F29C`.

The XPC branch allocates `0x10` bytes, writes `CRdPicture::vftable`, and calls `006B5190`/build counterpart before storing resource state. The destructor candidates are Steam `00408900` / GOG `004088C0`; vtables are Steam `0x0076F2B4` / GOG `0x0076F2A4`.

These class associations are direct, but the remaining helper payload fields, ownership, and downstream render/actor consumers are unresolved.

## XPC2 payload parser

Steam `006B5190` / GOG `006B50E0` validate the XPC magic `0x32435058`, derive a child grid from the header byte/ushort dimensions, allocate a 0x20-byte record for each child, and iterate child metadata. Each subpayload is inflated through the zlib helper when required, then the normalized XPC2 buffer is copied into CRdPicture state and its resource size/state fields are updated.

This closes the XPC2 payload-parser boundary. Child record semantics and the downstream D3D9/render consumer remain open.

## Consequences

The archive subsystem is now linked end-to-end through a verified chain:

`DPSERIAL.001-.003 stream -> record/header extraction -> raw/zlib payload buffer -> outer manager callback -> extension-specific helper/typed object seam`

This closes the generic resource architecture at the archive/record/extension-dispatch level. Remaining work is to identify each branch helper’s parser, object class/vtable, ownership, and gameplay/render consumers.

## Targeted XWP/DSB branch slice

A targeted follow-up found that the generic callback recognizes XWP and DSB but does not directly allocate an XWP typed object in mode zero. XWP has separate paired paths: Steam `0044FA20` / GOG `0044FA50` enumerate and register effect-package paths; Steam `00677CF0` / GOG `00677C40` perform manager-backed lookup, default path registration, `ver1.5` descriptor checking, and nested record traversal; Steam `00438D10` / GOG `00438D90` consume XWP names from effect/event command streams. This is a `STRONG_INFERENCE` resource-loading -> EFFECTS boundary, not proof of a dedicated XWP class or vtable.

The DSB callback branch initializes `CEvCore` through Steam `00408960` / GOG `00408920`, then invokes paired event-core helpers Steam `0070DD40` / GOG `0070DCE0`; release-side helpers are Steam `0070DE80` / GOG `0070DE20`. The helper resolves an event slot and updates command-side state. This is a `STRONG_INFERENCE` resource-loading -> EVENT_SCRIPT boundary; DSB grammar and ownership remain unresolved. Full details are in `findings/formats/xwp_dsb_branch_slice.md`.

The remaining XAM/XCA/XFE/XNV targets now have bounded consumer evidence but still require a concrete parser/header or class discriminator. XAM reaches actor virtual dispatch after named-resource lookup (`006271C0` paired setup); XCA is integrated into scene transition/resource registration (`004213D0/004213F0`); XNV is bound into repeated selector-0x40/0x41 object setup using `OGE60511.XNV` (`006A3BA0/006A3AF0`). XFE remains without a new primary discriminator. Details are in `findings/formats/xam_xca_xnv_slice.md`.

## XPC2 child accessors

The child-record layer is now bounded independently of the parser. Steam `006B5660` performs row/column bounds checks and returns a `0x20`-stride child record; `006B55D0` searches child records by a key before returning one; wrappers `006B5550/006B5590` pass selected records to `006B6020`. GOG `006B5660/006B56A0` use a build-specific offset-table/stride form of the same child-grid abstraction. A concrete render boundary is now verified: XPC2 reaches Steam `006B58D0`, which creates a 2D or cube D3D9 texture from normalized in-memory payload and stores/releases texture slots. Child field semantics and exact texture-holder identity remain open. See `findings/formats/xpc2_child_accessors.md` and `findings/boundaries/xpc2_d3d9_texture.md`.

## XMD animation boundary

Steam `0070BD20`, called by the XMD callback branch after CRdMesh construction, reads payload count/stride/flag offsets, allocates dependent blocks, copies normalized state, and reports normalized size. Adjacent `0070C020` converts `0x90`-spaced matrix blocks to quaternions/transforms with `D3DXQuaternionRotationMatrix`. This verifies XMD -> animation-transform state. Steam `006BE460` is the first concrete CRdMesh-side consumer: it invokes `0070C020` into owner field `+0x1E4` and classifies `0xA0`-stride node records by name prefixes and wiper/window names. Exact XMD field names and complete CRdMesh mapping remain open. Higher-level paired consumer `006C1430` passes animation/transform state into `0070C130`, establishing an animation-to-actor update boundary; final renderer ownership remains open. See `findings/boundaries/xmd_animation.md`, `findings/boundaries/xmd_crdmesh_consumer.md`, and `findings/boundaries/xmd_actor_animation.md`.
