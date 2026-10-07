# D3D9 device and GPU-resource lifecycle

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint 238.** Mandatory corrected renderer authority: checkpoint236 `audit/renderer_dependency_2026-10-07/RELIANCE_CONTRACT.json`, including active H0232 scope override. This map separates interface/device, bank/backing, descriptor, holder, output cell, COM object and GPU-use domains. **VERIFIED** calls/stores are not successful API execution or completed destruction.

## 1. Initialization and normal-use spine

```text
004017C0 application initialization
 → acquired platform/graphics context00BD7678
 → Steam006CC290 / GOG006CBD30
      logical render-size globals; physics-initialization request
      Direct3DCreate9(32) →01480988 interface cell
      adapter mode/capability requests
      local presentation parameters + saved Reset parameters
      CreateDevice(...,0x44,...,&01480984)
 → separate renderer/resource/shader/SceneDraw initialization requests

normal eligible idle iteration
 → current01480984 TestCooperativeLevel-shaped call
 → tick / BeginScene-shaped request
 → render-target + viewport + shader/texture/buffer state requests
 → packet/submesh DrawIndexedPrimitive-shaped requests
 → EndScene → Present request
```

**VERIFIED selected initial calls/state** [G1]. `01480988` is the Direct3D interface pointer cell; `01480984` is the current device pointer cell, neither the SceneDraw object nor a resource holder. Null Direct3DCreate9 and selected capability checks can return0. The selected CreateDevice result itself is not checked before continued work/return1. Local readiness/publication must therefore not be narrated as successful device creation.

The supplied named imports identify Direct3DCreate9 and D3DX helpers. D3D COM virtual names below are **STRONG_INFERENCE ABI matches**, supported by argument shapes/diagnostic strings; actual current runtime vptr/API binding remains **UNKNOWN**.

## 2. Separate roots and important state

| Domain | Established location / layout | Producers and consumers | Confidence / ownership ceiling |
|---|---|---|---|
| Direct3D interface | `01480988` | Create9 result; adapter/caps/CreateDevice/Release requests | **VERIFIED pointer flow**; exclusive lifetime **UNKNOWN** |
| Device | `01480984` | CreateDevice output; virtually every GPU-state/create/draw/reset operation | Current reloaded global, not a stable generation theorem |
| Presentation state | `0148BB70`,14 DWORDs | Initial local copy; refresh overwrite; later Reset input | Creation and retained Reset inputs can differ |
| Platform target stack | Current context`+A8`; records`+AC+i*20` | Save target/depth/viewport; restore/bind/Release/clear | **VERIFIED selected protocol**, balancing and API success **UNKNOWN** |
| SceneDraw | Application alias`00BD7670`, acquired cache`00BD9E68` | Query, packet collection, embedded targets and render orchestration | Not the device or named exclusive descriptor-bank owner |
| CRdModel cache/config service | `00BD9E60`, startup alias`00BD7794` | Fourteen fixed shader regions; selected shader/state requests | Available renderer-associated service, not whole active renderer authority |
| CRdTexture holder H | Observed0x20; WORD`C/E`; cells`10/14/18/1C` | Texture/surface creation, target/texture binding, cleanup | Named selected ctor; dimensions/access do not establish resource ownership |
| Model packet P | `M148`; capacity`M144`; resources`P84/P88`; object`P34` | Conditional population→SceneDraw pointer collection→draw consumption | Borrowing **SI**; latest generation/GPU last use **UNKNOWN** |

## 3. Resource creation paths

### Native engine target/resource requests

`006B5AD0` Steam (GOG selected corresponding texture helper`006B5A20`) uses H as receiver, performs existing cleanup, builds a local descriptor containing dimensions/usage/format/output-cell/holder bits, and offers it to the registry adapter. Selected family arms request:

| Holder/output | API slot / stored inputs | Subsequent engine state |
|---|---|---|
| 2D `H+14` | Device`+5C`; w,h,levels1,usage,format,pool0,output | H+4 numeric marker; H+8 role; WORD+C/+E dimensions |
| Cube `H+18` | Device`+64`; edge,levels1,usage,format,pool0,output | Separate family descriptor/slot |
| Volume `H+1C` | Device`+60`; w,h,depth,levels1,usage,format,pool0,output | Separate family descriptor/slot |
| Surface `H+10` | Selected texture/cube slot48 surface request, then target/depth binding | Auxiliary surface lifetime separate from texture COM cell |

**VERIFIED argument/storage mechanics**, API-family names **SI** [G1,G2]. Pool literal0 is **DEFAULT under D3D9 ABI**, not MANAGED1. Historical “managed resources” means bookkeeping only and must not be read as the D3DPOOL_MANAGED flag. Dimensions/markers can be written without checking a creation HRESULT.

### Loaded image path

`XPC/XPC2 payload → CRdPicture/CRdTexture-associated holder → D3DXCreateTextureFromFileInMemoryEx or cube variant → output cell`. `006B58D0` and the native-instruction direct-import path are separately established; request arguments and low-word return behavior do not make the outer return a preserved full HRESULT. Model/resource texture flow is detailed in `FLOW_RESOURCES_TEXTURES.md`. [G4]

### Buffers and shaders

The reset registry has a separately established vertex-buffer family. Model packet/submesh draw consumers reach current device state and indexed draw requests. CRdModel's fixed aggregate contains paired vertex/pixel shader-array interfaces, not fourteen alternative factory products. Complete shader/buffer/index-buffer allocation populations, every reset callback and payload reconstruction remain **UNKNOWN**; do not extend the four inspected reset banks to every D3D resource kind. [G3]

## 4. Descriptor registry — corrected indirection

```text
literal registry receiver R (2D:0148BBD8)
    R+0C = B, bank begin POINTER
    R+10 = E, population end POINTER
    R+14 = K, capacity end POINTER
             │ index*18 + B
             ▼
           descriptor D
             +10 = S, output-cell ADDRESS
             +14 = holder bits H
                      │
                   P = [S], current COM POINTER
```

**VERIFIED corrected formula:** `D = [R+0x0C] + index*0x18`, with selected valid-range/x86 arithmetic qualification. **DISPROVEN:** inline `R+0x0C+index*0x18`. Growth can replace B while R remains numerically stable. An index check occurs, but a returning invalid-parameter diagnostic may permit subsequent arithmetic. Backing/current population/allocation success and safe descriptor retention are not proved. [G2]

The concrete2D producer supplies `S=H+14`, and a shallow six-DWORD copy captures descriptor bits. This is not a deep copy, retained owning reference, or proof H/S/COM remains live. Bank teardown's element leaf and backing deallocation are separate from holder/COM/GPU retirement.

| Family receiver | Descriptor stride | Output-cell address field | Inputs retained for reset |
|---|---:|---:|---|
| 2D`0148BBD8` |18|+10|width0,height4,usage8,formatC; holderbits14 |
| Cube`0148BBF0` |14|+C|edge0,usage4,format8 |
| Volume`0148BC08` |1C|+14|width0,height4,depth8,usageC,format10 |
| VB`0148BC20` |C|+8|length0,usage4 |

Receivers and field widths are independently checked in both builds. Their named owning C++ classes, exclusive resource responsibilities and alias policy remain **UNKNOWN**. Different bank strides are not one uniform texture record.

## 5. Lost/reset/recreation control flow

```text
TestCooperativeLevel result
 ├─ nonnegative → local AL1 eligibility
 ├─ DEVICELOST88760868 → Sleep50ms request → AL0
 ├─ other negative except DEVICENOTRESET → Sleep50ms → AL0
 └─ DEVICENOTRESET88760869
      → pre-loss resource requests
      → current device Reset(&0148BB70)
          ├─ negative → diagnostic / Sleep50ms → AL0
          └─ nonnegative → post-reset Create attempts → AL1
```

Steam gate`006CCEF0`, GOG`006CC990`; pre helpers`006CC030/006CBAD0`; post helpers`006CBD00/006CB7A0`. **VERIFIED paired selected order**, not complete device-lifecycle semantics. [G2]

### Pre-loss per-descriptor effect

1. Reacquire descriptor and associated holder/auxiliary surface interface.
2. Read S from the descriptor; P=`[S]`.
3. If P is nonnull, request its virtual+8 Release.
4. Reacquire the descriptor/S and write0 through S.

Release's result is not tested before clearing. A remaining reference or active GPU borrower is compatible with this local clear. A null slot skips Release. Descriptor population itself is not the COM object.

### Post-reset per-descriptor effect

Read stored dimensions/usage/format and S, then request CreateTexture/CubeTexture/VolumeTexture/VertexBuffer on the current supplied device. Forced inputs include levels1, pool0 and family-specific zero values. Negative HRESULTs produce real diagnostic calls, then iteration continues; no aggregate “every resource restored” result is computed.

**DISPROVEN:** all resources must be successfully recreated before AL1/frame eligibility. A selected normal path with Reset nonnegative and CreateTexture negative can leave S null and still return AL1. **UNKNOWN:** content restoration, successful later draw, API-side output behavior, valid device/record populations and retried recovery.

The gate also reaches AL1 directly after a nonnegative cooperative test, without reset/recreation. AL1 is not an exclusive completed-reset marker.

## 6. Normal usage and texture sampling

| Caller / state | Transformation | Consumer | Qualification |
|---|---|---|---|
| Holder `006B5DE0` | Own WORD dimensions→viewport; surface request→H10 | SetRenderTarget/SetDepthStencilSurface-shaped slots94/9C | Requests, no successful binding/current generation proof |
| Holder `006B6020(H,stage)` | Select first available H14/H18/1C pointer | Current device slot104 SetTexture-shaped request | Stage and pointer flow **VERIFIED**; shader sampling result **UNKNOWN** |
| Shader constant matrix path | Transpose matrix, numeric register/count | Slots178/1B4 vertex/pixel constant requests | Direct API boundary; current shader/semantic unit **UNKNOWN** |
| Packet/submesh draw `006D6E70` | Resource-selected mesh data and shader states | Slot148 indexed draw-shaped request | **VERIFIED selected operands**, GPU execution **UNKNOWN** |
| Reflection subpass | Current plane/matrix/clip and target holders→object pass classification | Later stages5/6 bind the same holder regions; stage7 full-resolution holder | Detailed in reflection map; no frame freshness guarantee |
| Main packet cache | Model state/matrices→conditional packet population→scene vector | Resource-aware render consumers | Existing valid packet can bypass population; vector access≠ownership |

## 7. Cleanup, lookup and shutdown

**VERIFIED lookup direction:** Steam`006CBAC0` / GOG`006CB560` compares the supplied COM value with **`[[descriptor+0x10]]`**, returning the first matching index under its selected protocol. It does not match descriptor addresses or direct+10 values. The selected XPC2 holder cleanup`006B59A0/006B58F0` is a source-bound direct caller, not the complete producer/consumer universe. [G2,G4]

Holder cleanup, reverse lookup/removal, surface Release, bank erase/deallocation, device loss and application shutdown are different operations. Descriptors can alias the same S; the inspected loops do not establish uniqueness. Repeated reacquisition also does not prove a stable bank/device episode.

Application shutdown`00401310 → 006CC670` Steam reaches physics session cleanup, then requests current device/interface Release and clears`01480984/88`. **VERIFIED selected call/clear order** [G1]. It does not prove final COM destruction, GPU quiescence, all packet borrowers gone, safe resource-worker shutdown or balanced every reference. The normal lost-device branch calls Reset on the existing device path; it does not call CreateDevice there. Complete alternate recreation/shutdown populations remain **UNKNOWN**.

## 8. Explicit UNKNOWN edges

Actual CreateDevice/Create/Reset/Release outcomes; stable device/holder/bank/cell/COM generations; every shader/index-buffer/surface/query resource reset path; restored payload contents; output-cell alias policy; descriptor class/ownership; last GPU/CPU borrower; complete callback/thread ordering; actual app root/free/cache coordination; whole GOG renderer body outside selected correspondence. These are retained boundaries, not inferred absences.

## Evidence anchors

- **[G1]** `audit/phase8_synthesis_2026-10-07/DISPLAY_REFLECTION_EDGE_PRIMARY.json`, selected initialization/target/viewport/save-restore/texture/draw nodes and own-PE matches. `TIMING_EDGE_PRIMARY.json` Present requests. Exact original C/ASM ranges are recorded per entry.
- **[G2]** `findings/boundaries/phase7_renderer_descriptor_reset_falsification.md`; `findings/boundaries/c0098_storage_dependency_reconciliation.md`; `maps/RENDERER_C0098_DEPENDENCY_RELIANCE.md`; renderer236 primary/contracts/companion rows. These override older reset/layout prose.
- **[G3]** `findings/boundaries/scene_submission_pointer_pipeline.md`; `animation_model_state_submission.md`; Phase7 A14; `findings/subsystems/phase6_crdmodel_fixed_shader_composition.md` and GOG companion; current root-service qualification.
- **[G4]** `findings/boundaries/xpc2_d3d9_texture.md`; `xpc2_d3d9_registry_cleanup.md`; `phase6_native_instruction_texture_import.md`; C0033/C0312 at their exact import scopes, not historical ownership rhetoric.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** Availableinitialloading-thread callback TBC012 requestsstage/clear/presentation through independentlyreloadedcurrentrender roots. Thisaddsarequestorigin/possibleborrower domain to the lifecyclemap, notobservedconcurrentD3D execution orpositivefinalborrowerclosure. Flagclear/memberstop requests do notcertifyalreadyenterediteration/CPU/GPU completion; actualactivation/currentvptr/device/result/quiescence remain UNKNOWN. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.
