# Visibility, frustum, draw distance and LOD

**Notation:** addresses and `+offset`/stride/mask operands are hexadecimal; byte sizes use explicit `0x` where hexadecimal. Object/record counts, distances, rates and screen dimensions are decimal unless prefixed `0x`.

**Phase 8; validated checkpoint 238 plus finite requested edge checks.** Visibility is a stack of independent state paths. A failed main-camera test can still admit shadow work; a selected render packet is not necessarily freshly populated; residency/activation/LOD are not one “draw distance”. Primary checks: `audit/phase8_synthesis_2026-10-07/VISIBILITY_EDGE_PRIMARY.json` (GOG1,822 / Steam2,055 matched original-ASM instructions). Seed names/ranges remain qualified where not joined to primary consumers.

## 1. Integrated visibility flow

```text
input / actor state → gameplay CCamera mode/control values
                   → render-camera context eye/target/up/FOV/aspect/near
                   → view/projection/frustum rebuild under its own gate

world/resource/model state → animation/base transform → object bounds
                                              │
                         object spatial membership O14C / tree node+58
                                              │ query returns originalO
                         object flags + resource pointers + main-class bits
                                              ▼
                    common object virtual24 visibility predicate
                       ├─ hidden/missing/bypass flags
                       ├─ selected one-of-six main AABB frusta
                       └─ three-cascade/extrusion rescue → temporaryD8:10
                                              ▼
                       conditional virtual44 → packet M148 or cached bypass
                                              ▼
                       scene63A8 pointer collection
                          ├─ packet distance metric → submesh LOD bits3000
                          ├─ main/shadow/depth/reflection pass lists
                          └─ transformed secondary-pass culling
                                              ▼
                                  resource-aware draw requests
```

**VERIFIED selected mechanisms** [V1–V4]. Actual object population, resource/API success, latest bounds/packet state and all draw results remain **UNKNOWN**. Arrows join specifically established values/interfaces, not a coherent whole-world snapshot.

## 2. Camera and frustum state

| State | Origin / transform | Consumer | Confidence / limit |
|---|---|---|---|
| Gameplay camera singleton`00BE1EA4` | Acquired named0x1AC wrapper; selected CInput axes and actor-mode mapping | Mode-specific control helpers; render-context producers | **VERIFIED selected Steam identity/inputs**, full context-output producer population **UNKNOWN** |
| Camera numeric mode`+154`, previous`+158`, flags`+11C` | Actor state mapping plus overrides | Generic dispatcher and separate routes | Not the render-context frustum fields |
| Render context eye`+4`, target`+14`, up`+24` | Context initialization/copies/control | LookAtLH→view`+6C` | **VERIFIED selected producer body** |
| FOV`+54`, aspect`+58`, dimensions`+5C/+60`, near`+64` | Selected context initializer and later controls | Projection`+AC`; six variant projections | Projection aspect is not automatically backbuffer aspect |
| Combined`+EC`, inverse`+12C/+16C` | View×projection/inverse | Spatial query and renderer staging | Gate can skip rebuild; no every-frame freshness theorem |
| Six frustum banks | Producer Steam`006B62E0` / GOG`006B6230`; store helper`004022C0` selected GOG | AABB consumer indexes selected class | **VERIFIED numeric/layout path** [V1] |
| Secondary pointer`context+67C` | Pre-render links toSceneDraw+5F88 or0 based on current+6708 | Three-frustum rescue | Pointer/current contents are separate epochs |

**Camera-mode correction:** generic`005354D0/005355A0` skips the table9 handler but does not globally exclude camera work. Non-generic callers`005577D0/005578A0` call current cell`008A9BEC`, whose independently read initializers are`00537F10/00537FE0`, without the generic mode9 filter. Handler reacquires its own current camera root. Actual caller type/current mode/cadence/cell mutation remains **UNKNOWN**. [V5]

## 3. Six main-frustum classes — producer → flags → consumer

The projection producer uses native far values below; frusta begin at`context+1FC+class*C0`. Their absolute-plane coefficients begin`+25C+class*C0`; each bank includes six float4 planes and corresponding absolute coefficients. [V1]

| Priority bit in object`+138` | Decoder class | Native far | Origin / selection |
|---|---:|---:|---|
| `0x00040000` |0|200000|First selected mask |
| `0x00080000` |1|80000|Only if preceding mask absent |
| `0x00100000` |2|20000|Then selected |
| `0x00200000` |3|5000|Then selected |
| `0x00400000` |4|1000|Then selected |
| None of those |5|500|Default |

**VERIFIED GOG decoder`006BB440`:** exact priority order, not a most-recent-bit rule. Steam common predicate calls corresponding`006BB4F0`; paired mechanism is supported at selected-call/structure scope, not whole class domains.

**VERIFIED selected numeric classifier`0040AC20` GOG:** reads **WORD object+30**, conditionally writes+138. Type5 with byte+621==6 adds class1 and falls through to a class3 addition; decoder priority therefore chooses1. Types33/46 test resource-selected scale against450: selected small arm addsclass3, other arm class2; type62 addsclass2. Other listed numeric cases addclass3 or4. `0040ADE0` adds independent flags for selected types. Complete factory/type/class population and permanently immutable class bits are **UNKNOWN**.

The historical Player/York-class1 label is not used: type5's named class association in the seed is CNpcAnimal, while primary here verifies numeric/field mechanics. Names of all classifier types are not inferred from numbers or guessed from similar field layouts.

## 4. Common AABB predicate and secondary rescue

Steam`006BD320`, GOG`006BCE30`, incoming receiverO and render contextC:

1. **VERIFIED:** clear temporary objectD8 mask`0x10` through its mask interface.
2. Explicit hidden/suppressed flagsD8`0x40000000` orDC`0x2000` reject.
3. Unless selected D8bit31 bypasses the inner test, require nonnullO160/O164 resource/bounds-associated pointers.
4. Decode class; test bounds through main-frustum AABB helper (GOG`006BB290 →006BB2C0`). Selected operands use object`+F4` and related bounds data, not an invented actor visibility cache.
5. If main test fails, test optional C67C's three frusta through GOG`006C4140` / Steam selected target`006C4630`.
6. If still needed and C44/48/4C direction is nonzero, use extrusion helperGOG`006C3EA0` / Steam selected`006C4390` against the selected main family.
7. Rescue setsD8`0x10`; otherwise return0. Accepted paths return1.

**VERIFIED AABB arithmetic:** selected planes test center-like input plus absolute plane coefficients×extent-like input. Geometry interpretation is **SI** from that algebra and bound producer/consumer joins; complete malformed/NaN/admissible input behavior is not generalized.

Named CLevel and CPlayer tables install the same Steam slot24 predicate, proving concrete shared interface use. The secondary seed's broad vtable census is not repeated or promoted as current population coverage. Specialized light/effect/other slot24 targets remain individually distinct. **DISPROVEN general architecture:** every object type necessarily implements a unique independent culler.

## 5. Membership, bounds and packet publication are separate stages

| Boundary | Concrete state path | Lifetime / success qualification |
|---|---|---|
| Bounds preparation`006C2020` Steam | Model state array`+1E4`, strideA0, node flags+70 and transform operations →bounds aroundO+F0/+F4 | **SI animation-to-visibility preparation**; no final draw in this body [V2] |
| Phase13 membership | Scene+1CA4 query member offered to objectslot48 | Selected conditional phase order, not all-object periodic rebuild |
| Object membership`004029B0` Steam | O14C old node; unchanged node no-op; changed node removal/store/insert originalO intoN+58 | **VERIFIED pointers**, non-owning membership **SI**; uniqueness/currentness/retirement **UNKNOWN** |
| COctTree initialization | Separate0x70 root; child construction; node+58 pointer vector | Selected childcount4; name does not prove eight children; collection deletion≠object deletion |
| Query→packet | Node pointer→originalO→slot24/slot44→packet getter | Exact pointer lineage; actual CLevel/Player selection **UNKNOWN** |
| Cached packet | M148 validity flag; conditional bypass of repopulation | Not a latest-pose/current-bounds publication guarantee |
| Scene vector | Pointer-only append at+63A8; clear/end-shrink | Does not transfer exclusive packet ownership or prove final GPU use |

Phase7 A02/A03/A14 retains duplicate/currentness/reentrancy/bypass countermodels. Do not make a clean visibility diagram depend on uniqueness or coherent generation. [V2,V3]

## 6. Directional shadows, lights and secondary frusta

**VERIFIED selected mode input:** GOG`00450920` returns `(receiver+8C5EC >>15)&1`. Existing game/stage writers feed current SceneDraw+6708; some stage-specific arms force1. This is not proof that one getter controls every renderer mode or is called once per frame. “Outdoor/sun” is **SI**, not literal mode naming. [V1,V4]

- Pre-render normalizes/uses directional vectorSceneDraw+6418..6424 and copies it intoC44..50.
- Nonzero+6708 linksC67C toSceneDraw+5F88; zero clears the pointer.
- A separate selected projection/frustum set uses far1000 for mode0,3000 for nonzero.
- Directional-light render helperSteam`006D9480` builds three view/projection pairs via`007334C0`, extracts three sets of six planes and conditionally copies72DWORDs (`3*0x60`) intoSceneDraw+5F88.
- Submesh classification can populate shadow-compatible list+63C4 separately from+63C8/+63CC and main lists. Pass masks/fade/resource flags matter in addition to main visibility.

**Timing qualification:** the pre-render predicate accesses the linked secondary bank before the later render-phase shadow builder on the selected normal spine. The bank is retained state, not proof of newly computed same-pass cascades. Other writers/selected branches/current generations are **UNKNOWN**; no runtime stale-shadow defect is asserted.

**Lights:** named CRdObjectLight/CLight have their own spatial interface and selected detach protocol atslot48, retaining O14C. That proves query membership/conditional removal, not all light visibility, light influence, local-light shadow distances or resource ownership. Local-light/specialized culling beyond inspected directional/secondary paths remains **UNKNOWN**. [V6]

Reflection has its own plane/matrix/frustum/target branches. Main-camera acceptance is neither sufficient nor necessary for every secondary-pass draw; see reflection map.

## 7. Mesh LOD — render packet metric, not actor activation

GOG`006DCF90` receives a render packet P and camera-related argument, with a separate current renderer/service receiver. Its selected mode2 arm computes a distance-like value, then writes:

```text
P+20 = 0                         if computed distance>=1e8
P+20 = distance/(resourceScale*25) otherwise
P+20 = 0                         on alternate mode arm
```

**VERIFIED arithmetic/stores; SI distance semantics** from the vector-difference/length helper chain. The resource scalar is read through selected resource metadata+54. P20 is a **packet LOD metric**, not the actor's same-numbered control field.

GOG selector`006D5380` reads P20, repairs NaN to0 in its selected path, converts metric to integer, and chooses/clamps levels using resource feature masks1/100/2000 and maxima1/2/3. Selected extra controls can promote0→1. If no enabled level is selected, the branch can admit groups without a fixed0..3 match. **Do not simplify this to an unconditional floor(metric) four-level rule.** [V1]

| Selected level | Submesh flags`+14 &3000` |
|---:|---:|
|0|0000|
|1|1000|
|2|2000|
|3|3000|

Matched submeshes then enter pass-list classification: resource/object flags, main/secondary culling, fade and material/pass fields determine lists. Special shadow-compatible selection can use its own feature/group choices. The mechanism is mesh/submesh selection, not resource streaming, object retirement or a universal alternate-model switch. Exact bridge-specific transitions remain **UNKNOWN**.

## 8. All established distance-control domains

| Domain / native value | Origin → state → consumer | Evidence state / missing edge |
|---|---|---|
| Six mainfar200000/80000/20000/5000/1000/500 | Context projection producer→frusta; object138 bits→decoder→AABB | **VERIFIED selected path** |
| Active-pass thresholdSq1,000,000 (radius1000 for ordinary zero-W position differences) | Dispatcher local threshold; selected context/object vector difference→sum-of-squares comparison | **VERIFIED numeric gate**, full policy/population **UNKNOWN**; not frustum far |
| Directional secondaryfar1000/3000 | CurrentSceneDraw6708→adjusted projection/frusta; bank5F88→rescue | **VERIFIED selected state**, fresh/current cascade join **UNKNOWN** |
| LODscale25 | Resource scale54 and distance-like numerator→packet20→submesh flags | **VERIFIED arithmetic**, distance units/resource meaning **SI** |
| Secondary-passfar10000/200000 | Reflection builder/projection→transformed frusta→packet submission | **VERIFIED selected literals**; separate from main classes |
| NPC/character secondary500 | GOG`004ACE30`: masks/bytes627/member684/related target gates; range helper result against500→`004D03C0` | **VERIFIED selected call gate**; not full skeletal evaluation cutoff. Old1995 attribution **DISPROVEN** |
| High-detail streaming cells | World/stage cell policy→resource/placement admission | Topology-based seed architecture **SI**; complete distance producer/cell payload join **UNKNOWN** here |
| Alternate3D representation | DesiredO444/current byte12/index416→paired resource lookup→same model binding→current-byte commit | **VERIFIED selected Steam seam**; range producer/all75asset identities **UNKNOWN** [V7] |
| Effect event1~2000 | Selected effect callback range→inherited1B4 bit4 suppression | TBC006 below: **VERIFIED selected numeric/bit/part-update gate**; norm/metric SI, full/live policy UNKNOWN [V8] |
| Effect F-mode~40 and per-part geometry | Broad player proximity→per-part gameplay/radius tests | Distinct secondary seed paths; full producer/consumer/units **UNKNOWN** |
| Effect type19 contextual thresholds | Event12 data-driven distance→fade/HUD-compatible request | Separate from main frustum/activation; exact domain remains source-qualified |
| Tree configuration40000/2000/40000, depth5 | COctTree root initializer→partition/query state | **VERIFIED values**, not automatically world draw-distance limits |

No modification options or controls are recommended. Native control state and its distinct consumers are mapped; known but unjoined secondary claims are shown, not silently upgraded.

## 9. Alternate representation and resource lifetime

Current Steam canonical seam is `005C8720`, not the interior label`005C87F0`. It consumes desired+444/current+12/selector+416, obtains two resource values through CRdData, calls same-object`006BE6E0`, then writes current byte. Void setup and current-byte commit do not establish successful replacement, exclusive resource ownership, unloading or last use. The seed GOG75-pair preload walk and paired table are locators, not a fresh complete opposite-build semantic inventory. [V7]

Visibility flags, membership nodes, packet validity, alternate representations, resource counters and persistence records have separate lifetimes. “Invisible” does not prove inactive/unloaded/deleted; a shadow rescue does not make the main scene visible; a retained object pointer is not ownership.

## Evidence anchors

- **[V1]** `VISIBILITY_EDGE_PRIMARY.json`: build-local entry/ASM line/bytes and own-PE matches; GOG C`332978–333015` decoder, `334042–334092` predicate, `337455–337513` rescue, `352255–352280` metric, `348528–348724` LOD/pass lists, `114715–114736` NPC gate. Seed locators: `inputs/knowledge/zachfix_research_2026-10-01/world/frustum.md`, `lod.md`, `shadows.md`; only reconciled exact scopes consumed.
- **[V2]** `findings/boundaries/xmd_bounds_visibility_prep.md`; `animation_model_state_submission.md`; `scene_query_member_lifecycle.md`.
- **[V3]** `findings/boundaries/scene_submission_pointer_pipeline.md`; Phase7 A02/A03/A14 dossiers and closeout qualifications.
- **[V4]** Steam C`450228–450240`, `450481–450493`, directional helper`453285–453304`; primary receipts above and display/reflection primary. Game/stage writer complete cadence not inferred.
- **[V5]** `findings/boundaries/phase7_architecture_a06_falsification.md:15–55` and A06V2 primary.
- **[V6]** `findings/boundaries/phase5_spatial_light_detach_cleanup.md`.
- **[V7]** `findings/subsystems/world_resource_residency.md`; `findings/boundaries/world_object_representation_policy.md`; seed residency table/name limits remain.
- **[V8]** `inputs/knowledge/zachfix_research_2026-10-01/engine/effects.md:320–362,503–510`; secondary exact-event/range joins are not declared verified by a plausible decompiler shape.


## Targeted bridge connections — TBC005/TBC006

**VERIFIED selected static scope** (semantic/epoch ceilings retained): TBC005 links selected animated model matrix/plane metadata to reflection plane consumption, without freshpacket/currentmatrix or shader/bugcausal claims. TBC006 primaryqualifies typedCEffect event1 rawremap -> fourcomponentdifference/squaredsum/sqrt-shaped result comparedwith2000 -> inherited1B4bit4 set/clear -> samebase partupdate0071CFB0 earlyreturn0071E714. Numericbranch/bit/return VERIFIED; norm/metric SI. This is part-simulation suppression, not universalfrustum/finalvisibility/deletion/completion/fixed-delta. Independent type/base0/slot10 andexact constants replayed. Reports: `targeted_bridge_model_reflection_plane_record.md`, `targeted_bridge_effect_distance_part_update_gate.md`.


## Final static campaign amendment — checkpoint256

**VERIFIED selected mechanics / STRONG_INFERENCE environment/thunder association:** CMapcloudcallbacks/time-compatibleweights/pulse connectsharedbanks toconcrete84-byte zero/seed packetproducer/nativeevent0E; nativeO44 andreceiver66FCcallbackchannels separate. OptionalchannelmathincludesHALF-luma0.5 andindependentpulse/fadeslotreloads; packet-tagcopy andflagmutationdomains differ. One66FCnonnulltest doesnotprotectlaterreload, noalias/weight/index/currentepoch proof. N_THUNDER1..9 andnumericCSound requestchain strengthenpulseassociationwithoutactual/synchronizedlightning/audio orsuccessfulrendering. Cleanup/null-basepathsconditionalstatic,notobservedfault/safe retirement. NoGOG/completepacket/schema/ownertheorem.

Evidence/qualification: `findings/subsystems/final_static_environment_packet_thunder_family.md`; `audit/final_static_campaign_2026-10-07/FAMILY_REGISTER.csv`. Historical provenance and all stronger currentness/schema/result/lifetime guards remain intact.
