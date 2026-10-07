# Planar reflection rendering — geometry, targets and sampling state

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint 238 plus bounded requested missing-edge reconstruction.** Existing SceneDraw/packet/request/lifetime qualifications remain mandatory. The primary receipt `audit/phase8_synthesis_2026-10-07/DISPLAY_REFLECTION_EDGE_PRIMARY.json` matches selected original ASM to each build's PE; it does not certify whole renderer behavior. No runtime bug probe or rendering modification was performed.

## 1. Concrete end-to-end flow

```text
selected model packet P + material/event parameter request
     ├─ P position-like quartet
     ├─ metadata pointer P+1AC: normal and offset-like values
     └─ current SceneDraw callback+66FC(event10, objectP34, parameter output)
                      │ selected masks/positive returned parameter
                      ▼
 plane point = P[0..2] − metadata[C/10/14]
 plane normal = metadata[0/4/8]
 retain selected P in SceneDraw+6440
                      │
 post-dispatch renderer006D32E0 / qualified GOG006D2EB0
                      ▼
 secondary pass006D8B50 (Steam)
     save target/depth/viewport
     select half- or quarter-resolution color/depth holders
     build PlaneFromPointNormal
     build Reflection(plane)
     adjust projection near/far terms; multiply view/projection
     reflected matrix = Reflection × adjusted(view×projection)
     publish translated/current shader matrix
     plane → inverse-transpose matrix → clip-plane API request
     selected packets / secondary-frustum tests / submesh pass lists
     draw requests; clear clip-enable; restore target/depth/viewport
                      │
 later same renderer traversal
     selected color holders → SetTexture stages5/6
     full-resolution holder → stage7
     texture-coordinate bias matrix → shader constants
                      ▼
 current selected shaders / indexed draw requests
     embedded g_tReflect / g_vMirror names corroborate intended semantics
     exact shader sample/instruction/result relation remains UNKNOWN
```

All numerical stores/calls above are **VERIFIED selected mechanics** [R1,R2]; “planar reflection” is **STRONG_INFERENCE** jointly supported by imported PlaneFromPointNormal/MatrixReflect, clip transform, secondary targets and later sampling bindings. This does not claim every surface uses this path or that a draw succeeded.

## 2. Origin and surface/plane selection

| Producer / state | Transformation | Downstream consumer | Evidence state / qualification |
|---|---|---|---|
| Current scene packet collection | Selected tag1 model packets; masks at`P+30` include`0x44000` | Surface parameter/plane selection in`006D32E0` | **VERIFIED selected mask/caller mechanics**, actual current class selection **UNKNOWN** |
| Current scene callback cell`+66FC` | Called with object-like`P+34`, numeric event`0x10`, local parameter output | Positive parameter at output+8 admits selected surface; another output value feeds blend-like branch guards | **VERIFIED request/value use**; callback implementation, complete output schema and successful material query **UNKNOWN** |
| Packet`P+1AC` metadata pointer | Point components=P+0/+4/+8 minus metadata+C/+10/+14; normal=metadata0/+4/+8 | `D3DXPlaneFromPointNormal` | **VERIFIED selected reads/copies**; selected producer joined by TBC005 below; complete metadata type/domain/currentness/ownership **UNKNOWN** |
| Selected packet | Store P at scene`+6440`; masks derive per-packet/shared plane and reflection-flag arguments | Later pass culling/submission branch choices | **VERIFIED local state**, not stable instance/generation |
| Blend-like selected local scalar | `>0.01` and `<0.99` gates choose two pass invocations; absent selected surface clears+6440 | `006D8B50` last argument0/1 | **VERIFIED numeric thresholds**; “reflection versus refraction contribution” **SI**, human/material units **UNKNOWN** |

Important local source anchors: Steam C`451039–451065` selection/point/normal and `450670–450697` pass calls. Decompiled float/tag representations are checked against raw operands; packet fields are not reclassified as actors merely because they contain object/resource pointers.

## 3. Reflected camera is a transform, not a proved new camera object

Define C as the supplied staged render context, M as a local matrix output, p/n as selected point/normal, F as reflection-enable argument, and zFar as the caller's literal.

| Function | Receiver / inputs | Output / side effects | Selected callers |
|---|---|---|---|
| Steam`006D9340` / GOG`006D8F10` | Scene receiver; outputM, outputPlane, p,n,C,F,zFar | F0: viewC0 × projectionC40. Fnonzero: plane→reflection matrix; adjusted projection helper; Reflection×adjustedM | Secondary pass`006D8B50/006D8720`; selected callsites independently rooted |
| Steam`006DBCD0` / GOG`006DB8A0` | out, view, projection, near/far | Copies projection; `_33=far/(far−near)`, `_43=−near*far/(far−near)`; view×adjustedProjection | Reflection builder and selected render orchestration |
| Steam`006D93D0` / GOG`006D8FA0` | Scene receiver, M, plane, F | Matrix publication; ifFnonzero inverse(M), transpose, plane vector transform, current device clip-plane0 request and enable | Secondary pass |
| `006E13E0` Steam | Current SceneDraw, M | `scene+66BC = scene+667C × M`; returns address+66BC | Matrix publisher`006D93D0` |
| `006E0010` Steam | Acquired CRdModel/config receiver, supplied matrix pointer | Selected shader-matrix setter request | Follows SceneDraw matrix-result transport, not SceneDraw ownership transfer |

**VERIFIED operand arithmetic/call relationship** for checked builder/clip/projection; matrix-publication leaf source is [R2]. Near is forced1 in the reflected builder; callers use far200000 or10000 in different subbranches. Those values are secondary projection/culling choices, not the six object main-frustum distance classes.

The clip-plane path is **explicit API clipping**, not evidence of an oblique-projection algorithm. It uses inverse-transpose of the local matrix before a device slotDC plane request and slotE4 state`0x98=1` request. Selected exit writes that state0 and restores a normal matrix/target stack. Whether the runtime device accepted the plane, the plane's complete coordinate-space contract and all bypasses are **UNKNOWN**.

No separate allocated reflected-camera object, persistent reflected eye/target record, gameplay CCamera mode transition or camera ownership is established by this path. The reflected-view effect is represented by the matrix transform.

## 4. Target and viewport selection

Let R be the current SceneDraw. The selected secondary pass has two independent selectors: byte`0148BC48` chooses resolution bank, and its last argument chooses the color member. [R1]

| Bank selector | Pass argument0 | Pass argumentnonzero | Associated depth holder | Requested native dimensions |
|---|---|---|---|---|
| byte0 | color`R+6228` | color`R+6248` | `R+6268` | W×0.5 / H×0.5 |
| byte nonzero | color`R+6288` | color`R+62A8` | `R+62C8` | W×0.25 / H×0.25 |

W/H originate in SceneDraw`+65E4/+65E8`, initialized from logical render globals, not necessarily actual backbuffer dimensions. Startup literal1280×720 therefore offers640×360 or320×180 in these banks. Float→integer conversions and successful allocation are separate; the table describes requests.

`006B5DE0` reads holder WORD+C/+E, updates render-height state, requests viewport0,0,w,h,0,1 and obtains/binds a surface through current holder COM state. Holder surface`+10`, texture`+14`, dimensions`+C/+E` and registry output-cell address are distinct. Target-stack save/restore uses a separate current platform/context receiver, not an inline SceneDraw descriptor bank. See display/device maps.

The pass clears using selected context color values; saving/clearing/binding/Release are requests. No final-use, successful draw or restored-state theorem follows from a returning pass.

## 5. Submission and secondary-pass culling

**VERIFIED selected `006D8B50` mechanics:**

- It iterates a pointer collection through count/index interfaces, not an assumed exhaustive object population.
- A preliminary eligible group reaches `006D7C40`; the full draw policy of that helper is not inferred here.
- Shared-plane branch builds a six-plane set from its selected transformed matrix, then runs five numeric classification/submission iterations via`006D6890 → 006D6E70`.
- Per-packet-plane branch can replace the point's second component with `P+4 − P+28`; masks control whether it first runs a six-plane secondary test (`006DBD30`) before submission.
- Reflection-enable and selected packet identity control a numeric classifier argument; current scene`+6440` is a value input, not a proof of same object generation.
- `006D6E70` reaches current-device slot148 indexed draw-shaped requests and shader/texture state interfaces. API execution and GPU sampling are **UNKNOWN**.

Thus reflection submission is not merely a reuse of the main-camera AABB result. It has its own transformed frusta, packet masks, per-plane branches and pass-list selection. Detailed LOD/pass classification is in the visibility map; no all-object mirror population is asserted.

## 6. Final texture binding and sampling-transform state

**VERIFIED later holder→stage join:** after the secondary calls, `006D32E0` binds half-bank`R+6228/+6248` or quarter-bank`R+6288/+62A8` to stages5/6 using`006B6020`. It also binds full-resolution`R+60C8` to stage7, and an optional separate holder to stage8. `006B6020` selects H14, otherwiseH18, otherwiseH1C and issues current-device slot104. These are concrete producer-bank→later-consumer binding edges, not proof the texture contents are fresh. [R1]

Shader constant bias chain:

```text
full-resolution holder R+60C8 (selected caller)
 → Steam006E07D0 / GOG006E0760
 → identity local matrix
 → width getter called TWICE on supplied H
 → Steam006B7020 / GOG006B6F70 bias multiplication
 →006E0830 /006E07C0(registerEF, matrix,count1,width4)
 → transpose / current shader-stage selector
 → vertex178 or pixel1B4 constant API request, four registers
```

**VERIFIED:** Steam width getter`00409F60` is `MOVZX EAX,WORD[ECX+C]`; height getter`00409F70` independently reads+E. GOG`00409F30/00409F40` have matching selected operands. The bias builder's translation entries are `0.5+0.5/argumentWidth` and `0.5+0.5/argumentHeight`, with X scale+0.5 and Y scale−0.5. The selected identity-bias caller supplies width for **both** arguments; do not repair the diagram by pretending its second getter is height.

The emitted register start is`0xEF` (239), four float4 registers. Matrix upload helper transposes before the API request. Post-synthesis PGC006 establishes **VERIFIED conditional selected Steam** VS32/PS8 register/varying/projected-sampler correspondence below. Actual live shader permutation/addressmode/coherenttarget/image sample remains **UNKNOWN**; string names alone are not the evidence.

## 7. Known reflection-offset issue — state path versus causality

The reconstructed state that can participate in an offset is now concrete:

1. surface point/normal and metadata offsets;
2. packet/world-versus-camera-relative transforms, including scene`+667C/+66BC`;
3. copied projection depth terms and reflection multiplication order;
4. clip-plane inverse-transpose;
5. selected half/quarter target viewport;
6. independent full-resolution holder dimensions;
7. the bias path's twice-read width and emitted register239 matrix;
8. shader sampling of bound stages5/6 and associated constants.

**VERIFIED local discrepancy:** the two-dimensional bias helper supports distinct dimensions, but this selected caller uses W,W. Relative to the *same helper* supplied W,H, its Y translation differs by `0.5/W−0.5/H` for finite nonzero dimensions. This is an ideal real-number comparison of established arithmetic; native floating-point rounding can affect its numerical value. It is not a runtime image measurement or a fix recommendation.

**HYPOTHESIS:** that discrepancy contributes to the user-described known reflection-offset bug. **UNKNOWN:** a matched validated symptom/trace assigning the bug to this path, whether another coordinate-space/projection/viewport edge dominates, exact visible displacement, and cross-build runtime occurrence. The supplied seed water note concerns a separate c243 scroll-order overwrite; it must not be relabeled as reflection-offset evidence. This map does not claim the bug is explained or solved.

## 8. Lifetime and Steam/GOG scope

Local plane/matrix/frustum buffers exist during the pass. Scene-held target holders, their surface/texture cells, shader service receivers and packet pointers have separate lifetimes. Reset descriptor banks capture output-cell addresses and can recreate objects in those cells; same holder address does not imply same COM/GPU generation. Returning pass restoration is not GPU completion or permission to destroy packets/targets.

**VERIFIED selected build pairs:** builder`006D9340/006D8F10`, clip`006D93D0/006D8FA0`, adjusted projection`006DBCD0/006DB8A0`, identity-bias caller`006E07D0/006E0760`, bias builder`006B7020/006B6F70`, width/height getters`00409F60/70 ↔00409F30/40`. **SI bounded orchestration correspondence:** Steam`006D8B50` / GOG`006D8720` identified from actual build-local callers. Complete opposite-build pass/body/material/population/runtime equivalence remains **UNKNOWN**.

## Evidence anchors

- **[R1]** `audit/phase8_synthesis_2026-10-07/DISPLAY_REFLECTION_EDGE_PRIMARY.json`: selected carrier/helpers, original ASM lines/bytes and own-PE comparisons. Steam C`450670–450697`, `451039–451065`, `452977–453174`, `453182–453227`, `454349–454364`, `458337–458360`, `432539–432546`; GOG C`350375–350389`, `355742–355762` and selected getter/projection/clip bodies. Direct-call locators are build-local calls.csv, not complete live producer sets.
- **[R2]** Steam C`459124–459128` SceneDraw translated-matrix publication, `457828–457832` shader wrapper; caller raw `006D93D9–006D93ED` in [R1] establishes receiver/result transport. Existing scene/packet maps: `findings/boundaries/scene_submission_pointer_pipeline.md`, `animation_model_state_submission.md`; Phase7 A03/A14 qualifications remain.
- **[R3]** Both build import/string exports contain PlaneFromPointNormal/MatrixReflect and embedded`g_tReflect/g_vMirror`; Steam descriptor string getter`00404880` names mirror constant. These are corroborating labels, not shader-execution proof.
- **[R4]** `maps/RENDERER_C0098_DEPENDENCY_RELIANCE.md` and renderer236 contract; `inputs/knowledge/zachfix_research_2026-10-01/render/water.md` is a separate secondary scroll-order account, not primary reflection-offset causality.

## Post-synthesis PGC006 — selected constant/varying/sampler bridge

**VERIFIED conditional Steam static contract:** initialization bank00A25278→DiffuseV member40 and00ADCE20→DiffuseP_RAIN memberD0; selected helper arguments can chooseVS32/PS8. Bias006E07D0 explicitly offers root+40→constant239..242 request. VS32 at00A2EFC0 declares g_mLightProj239,count4 and writes TEXCOORD5 via four DP4s; PS8 at00ADE838 declares that input varying and executes projected TEXLD from samplers5/6. Constant10.y interpolation and13 add-color have separate CPU request endpoints.

The emitted UV/sample-register seam is now exact at those selected compiled rows, not merely a string inference. **UNKNOWN:** actually chosen live permutation, all other rows, coherent current roots/targets/COM/device/material, successful draw/finalimage/addressmode/GPU lastuse/GOG parity. W,W discrepancy→reportedbug causal join remains **HYPOTHESIS/UNKNOWN**, unchanged.

Separate MIRR member118's single selected pixel blob00BC3C7C samples onlydiffuse0; its name alone does not identify the stage5/6 consumer. This rejects a candidate-name hypothesis, not a sealed canonical claim.

Evidence: [selected shader consumer report](../findings/boundaries/post_phase8_reflection_selected_shader_sample.md), `audit/post_phase8_closure_2026-10-07/REFLECTION_SHADER_PRIMARY_V2.json`, `REFLECTION_CONSTANT_PRIMARY.json`, `SHADER_TOKENS_V3.json` and selected unchanged PE ranges. No shader/runtime modifications.


## Targeted bridge connections — TBC005

**VERIFIED selected static scope** (semantic/epoch ceilings retained): Known animation outputM1E8 + signedWORDM2F4 index -> selected matrix/copy/XYZ-W transform and Mposition-minus-matrixtranslation offset -> packetP1AC interior0x1Crecord -> known point=Pposition-offset/normal consumer. Negativeindex/default, independentmask800 override and nativefallback retained. Translationzeroing isnot orthonormalrotation/inversetranspose/normalization; actualcurrentpose/material/plane/shader/GPU/offsetbugcause remain UNKNOWN. Selected Steam relation only. See `findings/boundaries/targeted_bridge_model_reflection_plane_record.md`.
