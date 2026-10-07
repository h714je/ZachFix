# CRdPicture / CLevel Resource Attachment

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the paired CRdData typed-record to CLevel setup boundary; final draw semantics remain `UNKNOWN`.

## Bounded source universe

This batch examined functions that both use the valid CRdData record `+0x1C` accessor and contain explicit XPC evidence. Direct CRdPicture-vtable xrefs are construction/destruction only in both PC builds, so those references cannot expose a downstream owner. The level pair below is the first paired static context that resolves a named XPC record, directly supplies its returned object pointer to a concrete CLevel setup body, and stores that pointer on the CLevel object.

## Paired attachment path

| Stage | Steam | GOG | Direct evidence |
|---|---:|---:|---|
| named record lookup | `005DCA50` | `005DCB20` | builds `LEVEL%03d.XMD` and `LEVEL%03d.XPC`; calls name wrapper `006B2C30`, which resolves through valid record `+0x1C` accessor `006B2C70` |
| CLevel creation | `005EA8D0 -> 005E7620` | `005EA9A0 -> 005E76F0` | request selector `0x28`; existing paired factory/class evidence identifies this selector as CLevel |
| typed-object handoff | `006BE6E0` | `006BE1F0` | receives the resolved XMD and XPC object pointers and writes them at CLevel `+0x160` / `+0x164` (`this[0x58]` / `this[0x59]`) |

The CLevel setup bodies require both slots to be nonzero, allocate animation/auxiliary state, invoke the XMD preparation sequence, then dispatch CLevel virtual slot `+0x4C`. Existing vtable evidence resolves that slot to `006C0620` Steam / `006C04D0` GOG as CLevel resource/attachment update work.

## Consequence

The callback-created CRdPicture pointer is not merely parser-local: the CRdData record object slot is retrieved by level name and becomes CLevel `+0x164` alongside the corresponding CRdMesh at `+0x160`. This is a concrete resource-loading-to-world/level-object ownership boundary in both PC builds.

The attachment is a paired resource-object relationship, not evidence that every CRdPicture record is a level resource or that the CLevel virtual update performs final D3D9 submission. The XPC2 parser and texture helper remain separate evidence layers.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c`: `005DCA50`, `005EA8D0`, `006BE6E0`, `006B2C30`, `006B2C70`
- `inputs/decompiler/gog/DP_decompiled.c`: `005DCB20`, `005EA9A0`, `006BE1F0`, `006B2C30`, `006B2C70`
- `findings/boundaries/clevel_virtual_4c.md`
- `findings/boundaries/xmd_object_render_handoff.md`
- `findings/formats/resource_descriptor_fields.md`
