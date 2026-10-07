# XMD / CRdMesh Descriptor-Commit Reconciliation

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the paired typed-resource object ownership boundary; downstream actor/render ownership is separately scoped.

## Direct construction and commit

The XMD branch of the constructor-supplied typed-resource callback allocates `0x54` bytes, constructs `CRdMesh`, invokes the paired XMD state helper, then commits the result into the callback descriptor:

| Build | Callback | CRdMesh constructor | XMD state helper | Commit |
|---|---:|---:|---:|---|
| Steam | `00408310` | `004087E0` | `0070BD20` | descriptor `+0x18 = *(CRdMesh + 0x04)`; descriptor `+0x1C = CRdMesh*` |
| GOG | `004082D0` | `004087A0` | `0070BCC0` | descriptor `+0x18 = *(CRdMesh + 0x04)`; descriptor `+0x1C = CRdMesh*` |

`006B33F0/006B3760` subsequently copy that `0x30`-byte callback descriptor into the `CSingleton<CRdData>` record table under the manager mutex. Thus the first directly evidenced non-parser owner of the constructed CRdMesh is the CRdData typed-record object slot, not an actor/world object directly returned by the constructor.

## Reconciliation with earlier XMD findings

The supplied static corpus has no direct recovered caller of `004087E0/004087A0` beyond the typed extension callback, and no non-constructor CRdMesh-vtable xref. Existing XMD reports therefore describe later consumers of state reachable from the committed typed record, rather than direct constructor result consumers:

- `0070BD20/0070BCC0` normalize XMD state;
- Steam `006BE460` invokes the transform extraction path and populates owner animation records;
- Steam `006C1430`, `006C2020`, and `006BE6E0` form the later animation/actor/bounds-preparation chain.

These observations support a typed-resource-to-animation relationship but do **not** prove that every animation owner is a CRdMesh owner, nor do they identify a final D3D9 draw consumer. The prior Steam-only actor/animation boundary remains unchanged.

## Scope limit and next discriminator

This batch closes the specific result-store question for XMD/CRdMesh in both PC builds. It does not establish a direct actor/world/render owner beyond the CRdData record slot. The independently unresolved XPC/CRdPicture path should next be traced from the same descriptor `+0x1C` commit to a non-parser record consumer; its `006B5190/006B50E0` parser and XPC2 texture boundary must not be treated as CRdMesh evidence.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c` (`00408310`, XMD branch; `004087E0`)
- `inputs/decompiler/gog/DP_decompiled.c` (`004082D0`, XMD branch; `004087A0`)
- `findings/formats/resource_manager_population.md`
- `findings/formats/resource_descriptor_fields.md`
- `findings/boundaries/xmd_animation.md`
- `findings/boundaries/xmd_crdmesh_consumer.md`
