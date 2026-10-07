# MegaRE final architecture integration (2026-10-08)

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** The readable MegaRE synthesis. Check the primary findings and latest applicable corrections for the exact scope.

<details><summary><strong>On this page</strong> · 24 sections</summary>

- [1. Research state and accounting](#1-research-state-and-accounting)
- [2. Structural-foundation correction](#2-structural-foundation-correction)
- [3. Whole-engine control spine](#3-whole-engine-control-spine)
- [4. Timing model: multiple islands, not one delta](#4-timing-model-multiple-islands-not-one-delta)
- [5. Controller/input architecture](#5-controllerinput-architecture)
- [6. Object, handle and lifetime architecture](#6-object-handle-and-lifetime-architecture)
- [7. Player, actor and gameplay state](#7-player-actor-and-gameplay-state)
- [8. World, visibility, frusta and LOD](#8-world-visibility-frusta-and-lod)
- [9. Renderer, D3D9 and resolution domains](#9-renderer-d3d9-and-resolution-domains)
- [10. Reflection pipeline](#10-reflection-pipeline)
- [11. Model and animation](#11-model-and-animation)
- [12. Resource/loading architecture](#12-resourceloading-architecture)
- [13. NativeUI / message resources](#13-nativeui-message-resources)
- [14. Audio architecture](#14-audio-architecture)
- [15. PhysX and frame dependence](#15-physx-and-frame-dependence)
- [16. Save/persistence architecture](#16-savepersistence-architecture)
- [17. World/environment and CPut](#17-worldenvironment-and-cput)
- [18. Scheduler auxiliary queues and presentation](#18-scheduler-auxiliary-queues-and-presentation)
- [19. Additional final-static gameplay families](#19-additional-final-static-gameplay-families)
- [20. Cross-build discipline](#20-cross-build-discipline)
- [21. Repeated engine design patterns](#21-repeated-engine-design-patterns)
- [22. Major corrections that must not regress](#22-major-corrections-that-must-not-regress)
- [23. Remaining high-value unknowns](#23-remaining-high-value-unknowns)
- [24. ZachFix engineering boundary](#24-zachfix-engineering-boundary)

</details>
<!-- END AUTO RESEARCH NAV -->

This document is the ZachFix-oriented readable synthesis of the final MegaRE snapshot.
The exact imported findings, maps, ledgers and final reports are preserved under
[`../evidence/mega_re_final_2026-10-08/`](../evidence/mega_re_final_2026-10-08/).
When this summary and an imported primary finding differ in scope, the imported finding wins.

## 1. Research state and accounting

The final supplied snapshot extends the older 2026-10-04 Phase-4 import through Phase 7 scientific closeout, Phase 8 synthesis, post-Phase8 bridge closure and final broad static archaeology.

Current supplied state:

```text
Phase 7 scientific closeout        checkpoint 238
Phase 8 synthesis                  complete through checkpoint 243
Post-Phase8 gap closure            checkpoints 244-245
Targeted bridge campaign           checkpoints 246-250
Final broad static archaeology     checkpoints 251-260
Latest sequence                    260
Latest phase state                 PHASE_8_COMPLETE
Latest gate                        READY_FOR_AUTONOMOUS_STATIC_RESEARCH
Runtime observations in these campaigns: 0
```

Historical function accounting remains separate from the repaired structural universe:

```text
historical/export-recognized rows      20,252
historically classified                 5,180 (25.6%)
historically named                      3,481 (17.2%)
historical VERIFIED                     4,056
historical STRONG_INFERENCE             1,124
historical UNKNOWN                     15,072
corrected structural entries           21,871
new-entry candidates                    1,619
```

Do not divide historical semantic labels by 21,871 to invent a repaired semantic-coverage percentage. The repaired population and old semantic ledger use different universes.

## 2. Structural-foundation correction

The most important foundational correction is that Ghidra's recognized `Functions` set was not the complete callable/code universe.

Confirmed structural problems included:

- a false `noreturn` boundary in the GOG generic dispatcher that truncated real fallthrough;
- callable callbacks and initializer roots outside the exported/recognized function set;
- CRT initializer tables with many real non-null callback pointers not represented as ordinary functions;
- tens of thousands of orphan decoded instructions in each build;
- direct/candidate code targets and function-body fragments that could not safely be represented by a simple `min(address)..max(address)` hull;
- old ownership assumptions that treated a CRT address range or forwarding stub as proof of CRT/non-game ownership;
- a Steam vtable-enrichment path that had falsely zeroed some references.

The corrected structural model therefore records 21,871 entry records rather than treating the historical 20,252 function rows as executable exhaustion.

Practical consequence: future ZachFix locators/hooks should use build-qualified signatures and independently verified local control flow, not Ghidra function boundaries as an authority.

## 3. Whole-engine control spine

The final integrated flow is approximately:

```text
build-specific startup
    -> Win32 message / idle loop
    -> D3D cooperative-level gate
    -> QPC timing / game scalar publication
    -> main tick
       -> commit prior input
       -> conditionally acquire next input
       -> native tasks/services
       -> selector-sensitive repeated object passes
          -> model / animation
          -> camera
          -> physics producer / first half
          -> phase 7 second half
          -> phase 8 / event 6
          -> spatial membership / other object work
       -> camera/frustum staging
       -> visibility / packet collection
       -> BeginScene-shaped request
       -> scene draw / lights / shadows / reflection / movie / UI / fade
       -> retirement / pending-resource work
       -> separate late WinMM probe
       -> EndScene / Present
       -> optional post-tick extra presentation
```

This is a partial order, not proof that one tick equals one rendered frame. The object body can repeat, worker activity exists, several services are conditional, and current-generation/success relationships remain separate questions.

## 4. Timing model: multiple islands, not one delta

The central PC game scalar is QPC-derived and approximately 60-Hz-relative rather than seconds. Separate timing domains coexist:

- main game-time scalar and its clamp/rounding/carry policy;
- fixed per-producer input filtering/history steps;
- PhysX scene/timestep/debt/capacity policy;
- vehicle steering with explicit delta-aware slew;
- persistent wheel motor/brake state;
- CCT per-call displacement;
- effect paths, some of which use fixed values while others consume the game scalar;
- animation packet/cache reuse;
- Timer/Fade/CFunc accumulators;
- audio recurrence/state;
- Present/display cadence.

A global “divide everything by FPS” repair is therefore architecturally unsafe.

The final targeted bridge also found a loading worker loop that can request presentation work and call `Sleep(16)`. This is a loading-thread availability/cadence path, not evidence of a universal gameplay 60-FPS limiter. A nearby `1/30` threshold likewise does not gate that rendering work.

## 5. Controller/input architecture

Five record domains are now separated:

```text
P  raw physical WinMM record
L  0x6C logical action record
A  aggregate state
Q  pending state
I  live committed state
```

The established flow is:

```text
joyGetPosEx / physical acquisition
    -> seven raw rows (stride 0x36)
    -> binding interpreter
    -> logical 0x6C action record
    -> aggregate
    -> pending
    -> next commit
    -> live state
    -> camera / player / menu / vehicle consumers
```

Important details:

- seven numeric WinMM slots are represented;
- raw records contain selected-controller and acquisition-success bytes plus `JOYINFOEX` data;
- initialization explicitly selects row 0 by writing selected byte `== 1`;
- selected-row consumers test exact `== 1`, not merely nonzero;
- trigger-like values use the confirmed threshold/normalization path rather than a native independent-XInput trigger model;
- getters are downstream of the OS/API boundary and do not themselves poll the operating system;
- pending/live transport introduces an approximately one-update staging relationship, not a guaranteed one-rendered-frame delay;
- filtering/history use fixed per-producer increments, so provider invocation cadence is semantically relevant.

Checkpoint 260 additionally identified a CGame-relative 8-DWORD control-mask bank used by captured-input queries and custom message/glyph-token formatting. Recognized masks lose unrelated bits; composite/unmatched cases can preserve partial old-token policy; several special paths bypass the final generic map. This is directly relevant to future SDL3 and native-glyph integration.

Actuator storage is better bounded (seven pairs plus read/address/lock interfaces), but the actual native hardware executor remains unproven.

## 6. Object, handle and lifetime architecture

The object system contains typed factories, active-object dispatch, virtual-slot phases, native tasks, resource-bearing actors, embedded children and handle services.

`CRdHandleUtil` exposes a fixed 0xA00-pointer slot population plus generation state. Registration is not universally idempotent. The CThrowLure path is a particularly important static lifetime warning: the same object can be registered twice, overwrite its retained current handle and leave the first mapping outside ordinary current-handle retirement. No reviewed post-startup cleanup path was shown to reclaim that first mapping. Runtime frequency/exhaustion consequences still require instrumentation.

This combines with a broader engine pattern: fixed-capacity pools, sentinel scans and weak or non-obvious exhaustion behavior recur across handles, audio, physics, input finalizers and other managers. Long-session failures should therefore be investigated with pool high-water diagnostics as well as heap/memory metrics.

## 7. Player, actor and gameplay state

`CPlayer` remains a large stateful actor with separate state-selector/action domains; state numbers, logical masks, action packets and factory/type selectors must not be conflated.

The final campaigns added or strengthened:

- a player/CShot path from numeric/resource state through an available typed CShot payload, scene-query/raycast response, face/triangle geometry and conditional native hit packet delivery;
- parent/child attachment flow from descriptor/gameplay creation through indexed parent-matrix lookup and local-left/parent-right matrix multiplication to child position/orientation;
- concrete password/chess mini-game reset/update/cleanup/draw and result-state mechanisms;
- a substantial darts family: pool/launch/trajectory/board classification/sector/multiplier/score/camera-follow mechanics;
- selected world/environment and save-presentation mini-system connections.

These are durable gameplay-family findings, not proof that every related state has a complete semantic name.

## 8. World, visibility, frusta and LOD

There is no single engine “draw distance.” Distinct domains include:

- six main frustum classes with far distances 200000, 80000, 20000, 5000, 1000 and 500;
- active-pass/object thresholds;
- directional secondary/shadow distances;
- mesh LOD metrics;
- reflection ranges;
- NPC/character policies;
- world streaming/residency;
- effect-specific distance gates.

The main six-class selection is driven by object classification flags and feeds the common AABB visibility predicate plus secondary rescue paths. Mesh LOD remains a packet/resource metric rather than the same actor-activation mechanism.

A later bridge established a CEffect numeric-result -> inherited bit-4 -> per-part early-return chain. The associated distance-like calculation is around 2000 units, but this is an effect-update/part gate, not proof of a universal render far plane.

NPC research also established an 80/90 hysteresis-style camera-distance marker policy in a selected path, again separate from the six frustum far planes.

Directional shadow state uses its own retained bank/secondary frusta. A consumer can read retained shadow state before a later builder, so “current shadow bank” must not automatically be read as “freshly built from this rendered frame's camera.”

## 9. Renderer, D3D9 and resolution domains

Important corrected roots and contracts:

- the D3D9 interface and device are separate known globals;
- renderer/SceneDraw state is separate from those raw D3D roots;
- resource descriptors use a pointed bank, not an inline `owner + 0x0C + index*0x18` array;
- the COM resource key is further indirect through the descriptor output cell;
- D3D pool literal 0 is `D3DPOOL_DEFAULT`, not `D3DPOOL_MANAGED`;
- reset eligibility and the outer return path do not prove every recreation request succeeded.

Resolution is several state domains, including logical dimensions, backbuffer dimensions, holders, viewport, projection aspect and fixed/full/half/quarter render-target families. CreateDevice and Reset do not have to source every presentation/refresh field identically.

This supports ZachFix's policy of treating reset/refresh, render size, present size, projection/aspect and fixed target scaling as separate controls rather than one width/height variable.

A possible output-cell alias hazard remains conditional: if two descriptors share one output cell, reset recreation can overwrite a newly created COM pointer without an intermediate release. Actual duplicate output cells must be demonstrated before treating this as a live leak.

## 10. Reflection pipeline

Reflection is now one of the best connected graphics paths:

```text
model/animation matrix
    -> compact packet/interior plane record
    -> point + normal
    -> reflection plane / MatrixReflect-style transform
    -> projection/depth adjustment
    -> clip plane
    -> half/quarter target and viewport
    -> secondary visibility/submission
    -> shader constants 239..242
    -> selected VS32 varying
    -> selected PS8 projected sampling path
```

The old suspicious width/width sampling-bias call is therefore connected much farther downstream than before. The helper has width/height-shaped inputs while the selected caller obtains width twice. This remains a strong bug-shaped static candidate, not proof of the visible reflection-offset symptom until a live permutation/A-B test confirms causality.

## 11. Model and animation

`CRdObjectModel` resource/state/matrix/packet/capacity/spatial/flag fields are connected through selected evaluation and packet-population paths. Packet validity can survive without proving it contains the latest pose generation; cache reuse must remain distinct from same-tick freshness.

XCA progressed from parsing to a same-model scalar evaluator with cursor/key access and snapshot/current blending. A selected blend uses a factor derived from stored state without a proven universal clamp.

XAM-compatible binding gained a retained-block allocation/transform/conditional-cleanup path. Requested/recorded size does not by itself prove decoded extent or successful payload validity.

The final XPM work found 97 initial XPM/XMD catalog pairs plus a terminal TREEPHY witness and linked selected XPM/model-key data to kind-4/kind-5 triangle-shape preparation and an `NxTriangleMeshShapeDesc`-named path. This strongly supports an XPM pre-cooked/physics-geometry family without claiming a complete file grammar.

## 12. Resource/loading architecture

The supported whole-resource spine is approximately:

```text
DPSERIAL archives / named IDs
    -> raw or XZP1/zlib-like path
    -> CRdData / descriptor state
    -> queue or direct request route
    -> installed typed callback/parser
    -> resource-family state
    -> game consumer
```

`CRdData` descriptor geometry and request/publication state are mapped at selected fields. Resource publication, successful decode, owner lifetime and final use remain separate facts.

Post-Phase8 closure connected several previously open producers:

- `MES_ALL.MES`, `GLOBAL.FLG`, `GLOBAL.IDX` installation into `CMessage` fields used by NativeUI/message consumers;
- `EFF_LIST.PRM` into an EffectAdmin table with 195 records of 20 bytes;
- SRL item normalization/admission into the ItemManager-facing tuple family;
- CPut resource binding into the large placement/world cohorts used by known world/car/model consumers;
- typed audio FILEITEM production;
- selected XAM/XCA transforms and evaluation.

The CLoadThread path is real, but global request serialization, stop/join and universal current-worker ownership are not established.

## 13. NativeUI / message resources

The final static closure substantially strengthens NativeUI architecture:

```text
UPDATA/MESSAGE/MES_ALL.MES
UPDATA/MESSAGE/GLOBAL.FLG
UPDATA/MESSAGE/GLOBAL.IDX
    -> resource IDs
    -> CMessage binder
    -> CMessage +44 / +4C / +50 installed state
    -> category/code/numeric/message consumers
```

A prior correction remains mandatory: the selected `R+48`-style field in the analyzed path is a cached directory cell, not a body pointer.

Low-level resolvers can dereference directory/category state before late sentinel validation. Therefore direct ZachFix integration must preserve upstream readiness/admission and must not treat a late `-1/-2/-3` result as a universal null/out-of-range guard.

The final control-bank/token work also ties game-relative control masks to message/glyph formatting, useful for a future native ZachFix Settings frontend.

## 14. Audio architecture

The selected control spine is:

```text
resource/PRM/name input
    -> CSound row
    -> CSound coordination ID q
    -> Main token T / stored operand
    -> Core index/token K
    -> status Q
    -> cue/bank/backend requests
```

These are separate identity domains. One integer must not be reinterpreted as a universal sound handle.

Organizing capacities include approximately 256 CSound rows, 128 Main rows and 32 Core rows. Main token values can wrap/reuse; equal numbers at different times do not prove the same episode.

Post-Phase8 work connected a typed `CAudio_Data::FILEITEM` producer and selected bank/control/name sources, plus a GOG installed callback path that can arm event/state and issue typed CSound -> Core/Main requests. Current recurrence and actual live-episode identity remain runtime questions.

X3DAudio initialization consumes the backend mix-format speaker mask, while the X3DAudio handle is a different object. A later selected path applies an explicit 2x2 diagonal matrix, so the 5.1/7.1 brake-loop investigation has a concrete game-side matrix seam, but static evidence alone does not identify it as the cause.

Audio logical completion/reuse can precede all lower-level backend destruction/stop completion. “Row reusable” and “backend voice physically gone” are separate lifetime statements.

## 15. PhysX and frame dependence

The corrected selected order is:

```text
phase 14
    -> physics producer / first half
    -> phase 7 second half / fetch-shaped continuation
    -> phase 8
    -> object event 6
```

The two halves must not both be attributed to phase 7.

The game-time scalar reaches physics-facing logic, while a worker/timing path can clamp a value around 0.06666667 (1/15) in a seconds-style API context. This is a dimension/timing warning, not proof of a specific “60x PhysX bug.”

CCT, vehicle steering, wheel state and other physics consumers have different persistence and cadence contracts. No production-wide physics normalization follows from the static map.

## 16. Save/persistence architecture

`CGame`, `CSaveData` and `CPreserve` are distinct layers. A selected GameRecord is roughly 0x45CC0 bytes; the preserve/staging image contains a 0x120 header plus 28 such records.

A selected load path is connected from disk/staging through record selection into the CGame live block and later reconstruction consumers, but this is not proof of one atomic “load transaction.” Raw read/result state does not universally prove a full valid buffer was read, and publication/reconstruction/durable-write completion are separate stages.

Checkpoint 259 additionally connected selected XLY/CLayoutMessage display/transition banks to a save-presentation selector path. This is a presentation-state relation, not proof of save-content success or checksum semantics.

## 17. World/environment and CPut

CPut is no longer merely a large unexplained record family. A resource binder resets and populates multiple large placement cohorts, retains interior pointers and activates state consumed by known world/car/model providers. Payload mutation/search includes an observed `0x54585440` marker path. Full schema/ownership remains open.

Environment work also connected CMap cloud callbacks/time-compatible weights/pulse state to an 84-byte render/presentation packet and a thunder-associated `N_THUNDER1..9`/CSound request family. This supports an environment/thunder association but does not prove synchronized visible lightning/audio episodes.

## 18. Scheduler auxiliary queues and presentation

The small deferred-record drain gained a real producer/comparator:

- `00401050` inserts key/payload records under a busy/admission gate;
- `00401020` is the associated comparator;
- `00401080` drains/applies the records.

The comparator returns `-1` for equal keys rather than `0`, so stable/total/FIFO order must not be assumed.

A saved numeric transition producer is also connected to the post-tick extra-presentation gate. Values 2/3 are not iteration counts; the internal presentation loops have their own fixed iteration structure.

## 19. Additional final-static gameplay families

The final broad static campaign added bounded, source-qualified families that are useful for future openDP/archaeology work even when they are not immediate ZachFix features:

- password and chess mini-game state/callback/result families;
- XPM/XMD/triangle-shape physics geometry;
- environment/cloud/thunder packet/audio association;
- player shot/raycast/hit-packet flow;
- darts board/trajectory/score/camera flow;
- XLY/layout-text/save-presentation connection;
- CGame-relative control bank and custom message token formatting;
- parent-matrix attachment/world-transform flow;
- optional deferred-record queue producer/comparator/drain.

The preserved `findings/` directory contains the complete individual reports for these and earlier Phase 1–8 families.

## 20. Cross-build discipline

Steam/GOG homology is substantial but never reducible to one RVA delta. The historical homology ledger contains 273 rows, mostly verified/strongly inferred, but there are real role collisions, moved helpers, different structural geometry and build-local raw-only regions.

Rules that survive the final audit:

- never infer GOG as `Steam + constant` globally;
- never treat equal numeric addresses as equal semantic functions;
- qualify both builds independently where the patch depends on code identity;
- preserve build-local signatures and fail closed on unknown/mismatched executables.

## 21. Repeated engine design patterns

The final maps and adversarial review expose several recurring patterns useful for future ZachFix work:

1. **Published is not completed.** Logical state often changes before the lower backend finishes.
2. **Current is not necessarily current-frame.** Input, animation packets, shadow banks, reflection resources and save staging have different epochs.
3. **Failure is not rollback.** Some loaders clean old state or publish size/state before later validation fails.
4. **Deep helper safety can live upstream.** Several low-level dispatch/resolver paths assume state/index/table admission was already validated.
5. **Locking one operation is not transaction identity.** A shared pending cell can still lack request/completion correlation.
6. **Token equality is not episode equality.** Audio and other reusable numeric identities have generation/reuse problems.
7. **Fixed pools matter.** Long-session faults can be capacity/exhaustion problems without a heap leak.
8. **Diagnostic/error helpers are not automatically `noreturn`.** The structural repair proved that mistaken termination assumptions can hide real code.

These are heuristics, not permission to promote unverified semantics. See the imported `maps/RESEARCH_HEURISTICS.md`, `ARCHITECTURAL_PATTERNS.md` and `ENGINE_RECOGNITION_PATTERNS.md`.

## 22. Major corrections that must not regress

Among the most important retired interpretations:

- historical Ghidra Functions are not the complete callable universe;
- CRT address range/forwarding shape is not proof of CRT-only/non-game ownership;
- min/max function envelopes are not safe function-body ownership;
- renderer descriptor bank is pointed, not inline;
- D3D pool literal 0 is DEFAULT, not MANAGED;
- both physics halves do not belong to phase 7;
- phase 8/event 6 is not a universal physics-success certificate;
- audio Main token is not a globally unique episode handle;
- NativeUI cached directory cell is not a message body pointer;
- XAM `NULL` is not universally detach;
- XCA failure is not guaranteed rollback/preservation of old state;
- a valid model packet is not proof of latest-pose freshness;
- EffectAdmin's old 500-DWORD region was not established as the installed descriptor table;
- ItemManager construction does not prove its table is installed/ready;
- save result/staging state is not a single coherent durable transaction;
- handle registration is not universally idempotent;
- cleanup/request/clear is not proof the final borrower/backend use has ended;
- retained shadow/frustum state is not automatically freshly built for the current frame.

See imported `reports/DP_RE_CORRECTIONS_FINAL.md` for the authoritative correction register.

## 23. Remaining high-value unknowns

The final snapshot deliberately leaves important boundaries unresolved. High-value examples include:

- current boss -> MotionDriver -> admitted resource/schema/owner chain;
- actual Actuator hardware/backend executor;
- universal resource/packet/COM final-use and worker-quiescence guarantees;
- complete NativeUI MES/FLG/IDX body grammar and all command codes;
- complete XAM/XCA/XWP/XPM and other resource grammars;
- runtime audio looping/episode semantics and several bank/cue identity questions;
- actual runtime causality of the reflection width/width candidate;
- global PhysX cadence/result authority and high-FPS behavior;
- complete script/event VM scheduling and ownership;
- complete light/shadow/specialized visibility policy;
- a coherent whole-world ownership snapshot;
- runtime concurrency/cadence/success relationships that static analysis cannot certify.

The final sequence-260 strategic review still considered additional static work worthwhile. Surviving candidate families include external CMeshOctTree queries, encounter-record publishing, startup `ADDON/*.*` enumeration, opaque cache/UI control, typed shockwave feedback, Talk request/backend, world-record controls, typed NPC time state and camera/light motion. They are future candidates, not current ZachFix facts.

## 24. ZachFix engineering boundary

The final MegaRE is strong enough to guide architecture, local hooks, locators, protocol boundaries and runtime probes. It is not a license to ship fixes that require unobserved runtime claims.

Good static-to-engineering uses include:

- build-specific signature-gated binding layers;
- SDL3 provider work at the physical-input seam while retaining native logical action transport;
- NativeUI integration through proven task/message/resource readiness boundaries;
- domain-specific visibility controls rather than one generic draw-distance multiplier;
- targeted reflection/audio/resource/reset experiments at established seams;
- high-water diagnostics for fixed pools and lifetime diagnostics for known suspicious paths.

Runtime validation remains mandatory whenever a proposed claim depends on words such as `always`, `every frame`, `one owner`, `final use`, `completed`, `safe to free`, `leaks`, `hangs`, `current episode` or `success`.
