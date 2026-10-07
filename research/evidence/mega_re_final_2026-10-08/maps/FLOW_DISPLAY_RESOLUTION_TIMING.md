# Display, resolution, presentation and frame timing

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint 238 boundary.** The finite display/projection/target checks are retained in `audit/phase8_synthesis_2026-10-07/DISPLAY_REFLECTION_EDGE_PRIMARY.json`. Requests do not establish successful initialization, current resource generations or observed cadence. Values below are native game values, not modification settings.

## 1. Resolution is several state domains

```text
command line / numeric configuration
 → fullscreen0148BBC0 + variant0148BBC4
 → local CreateDevice parameters → backbuffer request
 → separately retained Reset parameters0148BB70

startup1280×720 → logical rectangle globals01480974/01480970
 → SceneDraw+65E4/+65E8
 → full/half/quarter/fixed-size CRdTexture creation requests
 → holder WORD+C/+E → target binding / viewport request
 → screen constants and texture-coordinate consumers

camera-context1280/720 → +5C/+60 → aspect+58
 → projection+AC/+1BC and six frustum variants
```

All shown stores/calls are **VERIFIED selected mechanics** [D1,D2]. Actual backbuffer/target dimensions and successful API effects are **UNKNOWN**. No universal equality joins backbuffer, logical rectangle, selected render target, viewport, projection aspect and sampling bias. The fullscreen1280×1024 branch can coexist with startup1280×720 render values and camera aspect.

## 2. Origin → state → transform → consumer

| Origin / writer | Stored state | Transform / consumer | Scope and lifetime |
|---|---|---|---|
| Steam`00700650` / GOG`00700670` command-line branch | `0148BBC0` fullscreen-like flag; `0148BBC4` display variant | Device initializer selects presentation fields | **VERIFIED selected writes**; complete configuration population **UNKNOWN** |
| `006CC290` Steam / `006CBD30` GOG | `0148096C=720`; rectangle globals X/Y0, width1280, height720 via`0044AD00` | Initial SceneDraw render-target sizing; later viewport/screen consumers | **VERIFIED literals**; not a client-size query |
| Device initializer | 14-DWORD presentation block, copy to`0148BB70` | Local block→CreateDevice; stored block→Reset | Separate snapshots; stored refresh later overwritten |
| `006D1610` current SceneDraw receiver | Float`+65E4=width`, `+65E8=height` | Float→integer dimensions; scales1,0.5,0.25; embedded target holders | **VERIFIED selected operands**; successful resources/ownership **UNKNOWN** |
| `006D2290` current SceneDraw | Replacement floats`+65E4/+65E8` | Selected target recreation requests | An available/invoked size-reset seam, not all display-resize policy |
| `006B5AD0(H,w,h,...)` | Holder WORD`+C=low16(w)`, `+E=low16(h)`; role`+8`; COM cells`+14/+18/+1C` | Descriptor capture and D3D texture requests | Width/height publication occurs without proving creation success |
| `006B5DE0(H,0,...)` | Uses holder`+C/+E`; surface cell`+10` | Updates height global; viewport0,0,w,h,0,1; surface request; target/depth binding | **VERIFIED conditional mechanics**, successful binding **UNKNOWN** |
| `0044ADA0(x,y,w,h,minZ,maxZ)` | Six-DWORD viewport request | Current device slotBC; then logical rectangle/screen constant update | Device state request, not observed viewport |
| `006B6130(context)` | `+5C=1280`, `+60=720`, `+58=width/height` | Auxiliary projection`+1BC`; `006B62E0` later builds main projection/frusta | Render-camera context, **not** gameplay CCamera0x1AC |

**Important receiver distinction:** gameplay CCamera controls feed render-camera context values, but context`+67C` frusta and matrices are not fields of the small gameplay singleton. Equal “camera” terminology cannot merge these objects. [D2,D5]

## 3. CreateDevice presentation fields

The local block is verified by raw stores and matches the D3D9 ABI (**STRONG_INFERENCE field names**). Both selected build-local initializers exhibit the same field construction. [D1]

| Offset from block / stored0148BB70 | Established value / branch | Meaning under D3D9 ABI |
|---|---|---|
| `+00/+04` | Default zero from memset; fullscreen variants below | BackBufferWidth/Height |
| `+08` | Default0; fullscreen0x16 | BackBufferFormat |
| `+0C` | 2 | BackBufferCount |
| `+10/+14` | Zeroed | Multisample type/quality |
| `+18` | 2 | SwapEffect |
| `+1C` | Zeroed in this block; separate window argument passed to CreateDevice | DeviceWindow / focus-window distinction |
| `+20` | 1 normally; fullscreen0 | Windowed |
| `+24/+28` | Auto-depth enable0; format0x4B | AutoDepthStencil request fields |
| `+2C` | Zeroed | Flags |
| `+30` | Selected mode refresh on fullscreen Create; later stored refresh overwritten | FullScreen_RefreshRateInHz |
| `+34` | 0 | PresentationInterval DEFAULT, **not** literal IMMEDIATE0x80000000 |

Fullscreen numeric variants: `BBC4==1 →1280×1024`; `==2 →800×600`; other→1280×720. The selected mode scan compares1280×1024 for variant1, otherwise1280×720, and prefers59 over60 when present. The displayed800×600 creation arm does not itself prove a matching mode-selection/admission path. Default/windowed zero dimensions are API requests, not proof the actual backbuffer is zero-sized.

**VERIFIED refresh distinction:** the game copies the local block to`0148BB70`, issues CreateDevice with the local block, then queries adapter display mode and writes the returned refresh into stored`0148BBA0` (`PP+30`). Thus the later Reset refresh input is not necessarily the initial preferred59/60 request. Neither value proves a measured presentation rate.

CreateDevice receives adapter0, device type1, supplied focus window, behavior flags`0x44`, local PP and output cell`01480984`. The selected initializer does not check its HRESULT before following work/return1. `0x44` is consistent with SOFTWARE_VERTEXPROCESSING|MULTITHREADED in D3D9; no FPU_PRESERVE bit is supplied. API-level effects and runtime x87 mode are not inferred from flags alone. [D1]

## 4. Backbuffer, target and viewport relationships

Let `R` be the current SceneDraw receiver and `W/H` its stored logical dimensions. Raw startup target stores/calls establish these selected requests [D1]:

| Holder region | Requested dimensions | Selected role established |
|---|---|---|
| `R+60A8,+60C8,+60E8,+61E8` | Integer-convertedW/H | Full-resolution color/depth-compatible targets, differing role/format arguments |
| `R+6228,+6248,+6268` | ConvertedW×0.5 / H×0.5 | Two planar secondary-pass color targets and associated depth holder |
| `R+6288,+62A8,+62C8` | ConvertedW×0.25 / H×0.25 | Alternate secondary-pass color/depth holders |
| `R+6108,+6128,+6148,+61A8,+61C8` | 128×128 | Fixed-size target requests |
| `R+6168,+6188` | 256×256 | Fixed-size target requests |
| Other selected banks |512×512,1024×1024, fixed1280×720 | Distinct requests; do not label every bank as reflection/shadow by size |

The target binder reads each holder's own WORD dimensions, not the backbuffer PP. `0044AD00` additionally publishes `(x+w/2,y+h/2,w/2,h/2)` to current device slot178 at register`0xFE` when requested (**SI vertex constant API**). It separately stores rectangle values in`0148097C/78/74/70`. This is concrete viewport→screen-constant propagation, not evidence all shaders consume every component.

Target stack helpers`006CCA80/006CCB20` save/restore target, depth and viewport through a current platform/context receiver: stack index`+A8`, records`+AC+i*20`. Saving invokes GetRenderTarget/GetDepthStencil/GetViewport-shaped calls; restore binds the saved pointers, requests their Release, clears them, and restores viewport via`0044ADA0`. Nested balancing, API success, current stack generation and last GPU use remain **UNKNOWN**.

## 5. Projection propagation

**VERIFIED `006B62E0` selected arithmetic:** noncoincident eye/target gate→LookAtLH to context`+6C`; optional incoming matrix multiply; PerspectiveFovLH to`+AC` using FOV`+54`, aspect`+58`, near`+64`, far literal200000; selected mirror-like flag negates the projection's first scalar. View×projection goes to`+EC`; inverses to`+12C/+16C`; six far-distance variants are published separately. [D2]

The frame subsequently copies selected view/projection matrices into`014811B8/014811F8`, multiplies to`01481238`, and stores an inverse at`01481278`. Post-dispatch staging also carries FOV/aspect into`00BD7684/00BD7664`. Explicit dimension→aspect rewrites beyond the selected context initializer are **UNKNOWN**; a changed render target alone does not prove changed FOV/aspect.

Reflection has a second projection path: `006DBCD0` copies projection, replaces depth terms using supplied near/far, then multiplies by view. Sampling bias is a separate matrix path. See the reflection map; do not collapse projection, viewport and UV bias.

## 6. Clock → delta → state consumers

```text
startup clock init00701040(1) /00700FA0(1)
 → QueryPerformanceFrequency → integer frequency014AFFF0
eligible idle clock read(0)
 → QPC count / cached frequency
 → previous eligible-time difference ×60
 → upper clamp8
 → selector-dependent quantization/fractional carry
 →014AFFD4 /014AFFE0
 → tick, objects, physics, animation, camera, audio, Timer/effects
```

**VERIFIED selected clock/arithmetic** [D3]. First eligible tick initializes local prior-time baseline. Selector`0041C270/0041C290` reads CDemo fieldbit31, not a refresh-rate query. Selector0 can use upward rounding after a fractional predicate; selector1 uses downward rounding plus a retained fractional accumulator that can add whole units back. Full floating-input/NaN/discontinuity policy remains **UNKNOWN**.

`014AFFD0` counts eligible ticks; once a local QPC-second interval is exceeded, `014AFFCC` receives the interval's tick count. That is not a proven GPU FPS counter when repeats/extra Present requests exist. The core shared scalar is60-Hz-relative, not seconds; physics transports it to an elapsed-shaped boundary, while other consumers use different persistence/unit contracts.

A **different** helper`00401F50` requests frequency and count per call, computes count×float(1,000,000/frequency), and reaches an integer-conversion helper. It supplies microsecond-shaped service/repeat timers, not the central application's seconds-clock interface. Historical equivalence of every clock domain is not adopted.

## 7. Waiting, Present and refresh are different edges

| Mechanism | Position / state | Established relationship |
|---|---|---|
| Cooperative gate`006CCEF0/006CC990` | Before eligible clock/tick | Lost/nonreset-error/failedReset arms request Sleep50ms; no tick on denied branch |
| Normal tick`006CC8D0` | After resource/retirement tail | Current device EndScene-shaped slotA8 then Present-shaped slot44(0,0,0,0); results untested |
| Additional posttick`004011B0` | `00BD77BC!=0` | Two three-iteration render/presentation sequences, with optional size-reset request between; not ordinary update count |
| Resource direct service | Caller polls pending while worker runs | A blocking resource-control protocol, not a display pacing clock or successful load certificate |
| Physics wait | Core event/context helpers | Separate submission/service synchronization requests, not a vsync relationship |
| PP refresh / interval | Device creation/reset inputs | API requests may influence presentation blocking; no explicit QPC→refresh lock or measured cadence proved |

**UNKNOWN:** actual driver/device Present blocking, effective vsync, monitor rate, delivered frames, native normal-frame limiter beyond selected code, baseline/debt policy across pause/load/Alt+Tab/debugger stalls (B0002). The code's60.0 gameplay multiplier is not evidence for a60-FPS cap.

## 8. Steam/GOG qualification and lifetime

**VERIFIED selected pairs:** application`00700650/00700670`, seconds clock`00701040/00700FA0`, device init`006CC290/006CBD30`, loss/reset gate`006CCEF0/006CC990`; renderer reset pairs retain checkpoint236 scope. Steam render-target/projection packet flow has deeper primary reconstruction here; GOG reflection/projection helpers are independently checked but whole opposite-build render/resize behavior remains **UNKNOWN**. Never use one address delta for all nodes.

Resolution values can outlive a COM resource; holder dimensions can remain after failure or Release. Stable receiver addresses do not establish stable device/target generations. Shutdown requests Release on the current device/interface and clears globals; it does not prove every borrower is retired. See D3D9 lifecycle map.

## Evidence anchors

- **[D1]** `audit/phase8_synthesis_2026-10-07/DISPLAY_REFLECTION_EDGE_PRIMARY.json`: own-PE matched ASM addresses/lines. Steam C`444304–444439` device init, `449784–449903` target init, `449989–450059` resize, `431717–431767` holder creation, `431824–431841` binding, `57402–57439` viewport. GOG C`341433–341568` independent device creation.
- **[D2]** Same primary receipt for`006B6130/006B62E0`; Steam C`432017–432139`. C0117 frame/staging primary and `findings/boundaries/scene_submission_pointer_pipeline.md`.
- **[D3]** `audit/phase8_synthesis_2026-10-07/TIMING_EDGE_PRIMARY.json`; `findings/boundaries/application_idle_frame_loop.md`; Steam C`480264–480278`, `480066–480132`.
- **[D4]** `maps/RENDERER_C0098_DEPENDENCY_RELIANCE.md`; `findings/boundaries/phase7_renderer_descriptor_reset_falsification.md`; checkpoint236 reliance contract.
- **[D5]** `findings/boundaries/input_camera_primary_roots.md`; Phase7 A06/camera mode9 qualification; separate render-context operand evidence above.


## Targeted producer connection — TBC002

**VERIFIED paired selected static scope:** saved numeric receiver S+25C/+260 ->0061F660/GOG0061F5E0 -> locally ECX-preserving0061F170/GOG0061F0F0 ->006227A0/GOG00622720 ->00BD77BC -> known004011B0 post-tick gate/clear. Selector1/secondarg0 writes2+(S22C!=0); selector3/secondarg0 writes2. This connects the formerly unspecified selected producer, not effective frame rate/presentation success. Nonzero gates two local three-iteration loops;2/3 is not a requested iteration count, and value1 reset is not admitted by these writes. Complete live policy, S class, API/device/currentness and display cadence remain UNKNOWN. Evidence: `findings/boundaries/targeted_bridge_extra_presentation_producer.md`.


## Fresh loading-worker bridge — TBC012

**VERIFIED selected available static scope:** A second availablepresentationorigin is initialtypedloadingcallback0040A860/GOG0040A830, gatedby W18 (sameglobal00886FF8 atofferedroot). Localdoublethreshold1/30 resetsaccumulator only, doesnotgatepresentrequests. OwnPE0076E028 declaredSleep target receives16; notactual16ms interval/fixedFPS/APIoutput/1:1tick-to-Present proof. Currentvptr/activation/deviceepochs andopaquehelpers remain UNKNOWN. Details/primary: `findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md`.
