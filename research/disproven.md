# Disproven / corrected interpretations

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](README.md) · [Topics](INDEX.md) · [Open questions](unresolved.md)

> **Reading note:** Negative constraints are intentionally preserved. A rejected interpretation does not invalidate every weaker observation.

<details><summary><strong>On this page</strong> · 10 sections</summary>

- [`state 0x38` is ordinary on-foot idle after leaving a car](#state-0x38-is-ordinary-on-foot-idle-after-leaving-a-car)
- [Vehicle continuation corrections (2026-09-25)](#vehicle-continuation-corrections-2026-09-25)
- [Object-action taxonomy corrections (2026-09-25 continuation)](#object-action-taxonomy-corrections-2026-09-25-continuation)
- [Live player-car steering producer correction](#live-player-car-steering-producer-correction)
- [Input/camera corrections (2026-09-26)](#inputcamera-corrections-2026-09-26)
- [PhysX production-candidate corrections (2026-09-26 final)](#physx-production-candidate-corrections-2026-09-26-final)
- [Native UI / COption corrections (2026-10-01)](#native-ui-coption-corrections-2026-10-01)
- [PhysX closure corrections (2026-10-01)](#physx-closure-corrections-2026-10-01)
- [Mega RE Census corrections (2026-10-04)](#mega-re-census-corrections-2026-10-04)
- [MegaRE final corrections (2026-10-08)](#megare-final-corrections-2026-10-08)

</details>
<!-- END AUTO RESEARCH NAV -->

These entries are intentionally retained so future research does not rediscover them.

| Old interpretation | Current status | Correction |
|---|---|---|
| `0x008A9758` has 22 Player states | DISPROVEN | `CPlayer+0x654` is used through at least `0x88`; recovered range contains 137 slots. |
| `FUN_00522E20` is invalid/assert fallback | DISPROVEN | It is a real large gameplay/locomotion handler and state-0 anchor. |
| CPlayer Event 5 pushes CCT transform | DISPROVEN | Player Events 5/6/7 are default/no Player-specific work. |
| Event 6/7 are Player post-physics CCT sync | DISPROVEN | Same reason; relevant work lives elsewhere in common Actor/Player phases. |
| `vtable ~0x007741F0` is the missing Player CCT component | DISPROVEN | The region belongs to `CNpcDog` vtable inheritance; York has its own post-state CCT path. |
| `FUN_0048B0A0` is the York CCT bridge | DISPROVEN | It is an NPC-dog override calling common NPC character code. |
| Saving in transient Player state `0x38` necessarily reloads state `0x38` and dereferences the serialized `record+0x14` pointer | **DISPROVEN** | `CPlayer+0x654` is not restored from GameRecord. In the reviewed Steam load path, the persistent high-mask adapter selects state `0x40`, while the corresponding default Player-init branch selects state `0x00`. The live `FUN_004DDDB0` dereference is real, but the claimed post-load reachability edge is not. |
| `FUN_00506F70` owns the whole `0x45CC0` GameRecord memcpy restore | **DISPROVEN / corrected ownership** | `FUN_0061A830` performs `Game+0xBE8 -> Game+0x8C568`; `FUN_00506F70` performs subsequent Player/world reconstruction and resume application. |
| Steam `0x00577B1E` proves normal York dismount clears `CAutomobile+0x434 bit 0x8000` | **DISPROVEN ownership attribution** | The matching `object+0x434` clear is a non-car object-family false lead. Exact normal-dismount lifetime/transfer of the real car scheduler bit remains OPEN. |
| `FUN_0050B240` is a generic `AbortAction` / SaveAnywhere cleanup primitive | **DISPROVEN** | It is a bounded map-target packet setup path (`0x759..0x766`) that clears the packet object field and writes coordinate anchors. It does not provide generic Player/camera/object teardown. |
| State 5 global phase / `FUN_0052E0D0` is PhysX submit | DISPROVEN | It is camera-related. Real PhysX boundary is State 14. |
| State 11 is `fetchResults` | DISPROVEN | State 11 is `PhysicsResist` processing; completion/fetch bridge is State 7. |
| Generic slots `+0x10/+0x14` can be named PrePhysics/PostPhysics | DISPROVEN as naming | Those semantic names depended on the false State-5 PhysX interpretation. |
| `FUN_0040A920` is animation/skeleton update | DISPROVEN | It is effectively a tiny accessor returning `this+0x160`. |
| `FUN_006C5AF0` is just a small render/culling routine | DISPROVEN | Ghidra false `noreturn` at `FUN_006C5AD0` pruned most of the real generic dispatcher. |
| `MovePlyCar 0x0054C090` is the whole live York car update | CORRECTED | It is not the whole `0x8000` scheduler path; live high-level/wheel/PhysX branches also route through `0x5588F0`, `0x5578A0`, `0x555C20`. However, `0054C090` **does contain a real current-player LT/RT consumer** gated by `car+434 & 0x8000` and Player state `87`. |
| Water regression is simply lost UV scrolling | DISPROVEN | PC has a real scroll-state ordering bug, but fixing it does not restore the major Xbox surface-quality difference. |
| Every scene-object type independently implements/selects its own six-frustum consumer at `vtable+0x24` | DISPROVEN as general architecture | GOG PE inspection finds 158 named vtables sharing common `FUN_006BCE30`; class selection is normally decoded centrally by `FUN_006BB440` from `object+0x138` bits. Specialized consumers still exist. |
| Main-frustum class 1 (`80000`) is York/Player | DISPROVEN / unsupported | The direct factory maps classifier type `0x05` to `CNpcAnimal`; its `+0x621==6` subtype selects class 1. No York-specific class-1 mapping is currently proven. |
| `object+0x444/+0x12/+0x416` swaps to a 2D billboard impostor | DISPROVEN | `FUN_005C87F0` swaps paired normal/alternate model resources through the regular resource-binding path; 75 alternate resource pairs are preloaded by `FUN_005D0C40`. Use “alternate low-detail 3D representation / far residency package”. |
| NPC/full skeletal evaluation is cut off at 1995 units in `FUN_004D03C0` | DISPROVEN | `FUN_004ACE30` gates the call at 500 units; `FUN_004D03C0` still calls `FUN_004CCD10`. `0x00773438` is 500.0; 1995.0 is adjacent at `0x00773428` and used by unrelated environmental-range logic. |
| A specific bridge was proven to use both native mesh LOD and shadow-distance stages | DOWNGRADED | Both systems exist, but that asset's own submesh/resource evidence has not been isolated. Treat the bridge-specific attribution as LIKELY / OPEN. |
| Stationary AO band proves the native scene Z-buffer is broken | DISPROVEN / too broad | Runtime work isolated the problematic signature to DP's packed PostFX depth input; native D24/INTZ is a separate high-precision source. |
| Canonical PC tree is a tiny crossed-plane/billboard replacement | DISPROVEN at tested scene | Runtime/XMD attribution ties the suspect `CObjectMergeTree` to large `T18_00_00.XMD` submeshes, including 12,600-vertex `MAGE_B_NSR_3`. |
| Tree density loss is simply `ALPHAREF=0xF0` | NOT SUPPORTED as complete explanation | `0xF0` is real on the foliage draw, but alpha-ref A/B changed other alpha-tested surfaces far more than the suspect tree silhouette. |
| `F1FIR005` maps to `CEffect+0x31C == 3` | DISPROVEN | Raw x86 `strstr -> neg -> sbb al,al -> add al,3` gives `2` on match. Correct mapping: `F1FIR005 -> 2`, other `F* -> 3`. |
| CEffect owns its own main simulation/render/spatial virtuals | DISPROVEN architectural framing | `CEffect` and `CRdObjectEffect` vtable slots 1..18 are identical. CEffect adds resource/catalog/tail state and callback-driven game policy; base class owns simulation/render/spatial behavior. |
| CEffect fixed `delta = 1.0` is a Director's Cut / PC invention | DISPROVEN | Original PAL Xbox `CRdObjectEffect` homolog `sub_82548180` contains the same fixed override at `object+0x25C & 2`; Xbox type creation also marks `0x1E` and `0x2A..0x2E`, matching PC. The open issue is cadence preservation, not branch provenance. |
| PC terrain flicker is caused by collapsed grass DDS resources | DISPROVEN as sole cause | Grass resource collapse is real, but Xbox resource restoration does not remove the canonical breakthrough artifact. |
| PC `HOUSE_LIST.NOD` asset itself contains 64 swapped/corrupt keys | DISPROVEN | Raw normalized PC/Xbox assets are byte-identical; the 17/64 split appears only after PC runtime preprocessing. |
| Global frustum disable is the pillow/mirror fix | DISPROVEN | The candidate passes normal frustum and is rejected by a subsequent visibility-volume test; production bypasses only the affected outer-world volume callsite. |
| Native Gamepad replaces DP's entire action/binding system | DISPROVEN | Production feeds canonical SDL3/XInput state into DP's existing binding IDs and action helpers, then resumes the native `0x6C -> CInput` pipeline. `configJ.cnf` and downstream action routing remain authoritative. |
| PC gameplay input is capped at 30 Hz because CInput has a ~33.333 ms callback | **DISPROVEN** | The callback worker implementation is real, but the normal shipped CInput lifecycle leaves it inactive. Runtime census recorded zero background producer calls/threads, `+0xBC0` stayed in mode 0, and the handoff setter/query have no shipped xrefs. The real PC artifact is one-deep staging and roughly one game-tick sample latency. |
| A synthetic XInput JOYINFOEX layout can be dropped into vanilla `configJ` semantics unchanged | **DISPROVEN / retired path** | Vanilla expects right-stick U/R and shared-Z triggers. The experimental synthetic X/Y, Z/R, U/V bridge required semantic remapping; production removed that bridge entirely and evaluates canonical `GamepadState` directly at the native action boundary. |
| The gameplay mouse-look transform after `GetCursorPos/SetCursorPos` is still unknown | **SUPERSEDED** | The route is now mapped through mouse look producers, CInput temp +44/+48, PC filtering, public +EC/+F0, `FUN_007089E0(pair 1)`, and camera consumers. Exact FPS sensitivity remains an open timing question. |
| Xbox360 profile should restore original Xbox left-stick aiming | DISPROVEN as production goal | ZachFix restores Xbox shaping but intentionally preserves Director's Cut right-stick aim routing. |
| Vehicle analog triggers require a late PhysX torque multiplier | DISPROVEN | Restoring the three native analog vehicle consumers propagates analog input through the existing game-side vehicle/PhysX path. |
| DP mouse-look can be assumed to be DirectInput-only | DISPROVEN as assumption | A material Win32 `GetCursorPos`/`SetCursorPos` recenter path is confirmed; any DirectInput contribution remains unproven. |
| `FUN_00537730` proves ordinary free-look rotates at `2 degrees per frame` and therefore scales directly with FPS | **DISPROVEN as timing inference** | Camera update calls the mode handler first. Mode0 reconstructs the yaw target from Player/anchor state, then the common path may call `00537730`. The local 2-degree operation is frame-local relative to a refreshed target; it does not by itself define angular velocity. |
| `CPL01.XFE` is the CEvent bytecode stream referenced by the Prologue manifest | **DISPROVEN** | The inspected payload is an `XAM2` facial-expression asset containing face-node names. CEvent command streams are `.DSB` files under `UPDATA/EVENT`. |
| CEvent `A6/4` is a plausible common shipped-gameplay producer of the historical aim bug | **DISPROVEN as normal-gameplay cause** | Structural scan of all 656 shipped DSB files / 61,498 commands found exactly two `A6/4` commands, both in `EVENT/91/0_0455.DSB` routine `銃撃テスト` (Shooting Test), an internal developer-test collection. |
| Common primary weapon handlers repeatedly reissue a mode-2 Player state during held aim | **DISPROVEN** | Direct SetState exit census of the stock primary weapon handlers resolves to state 0 or non-mode2 families; normal class-based ingress passes through camera-mode-0 boundary states `0E/0F`. |
| `CCamera+0x120 bit0` has a PC-only writer policy that explains the aim regression | **DISPROVEN as platform divergence** | The corresponding Xbox writer/set-clear policy matches. It remains a downstream persistence/follow-policy input, but the confirmed PC/Xbox policy difference is the camera setter's repeated-mode2 initialization suppression. |
| x87 PC=53 is already proven to be the spontaneous failure's root cause | **NOT PROVEN** | PC=53 causally reproduces the same edge-follow failure, PC=24 removes it, and the scoped guard corrects the affected behavior. What remains unproven is what puts a normal session into that precision-sensitive state; local Alt+Tab does not change ambient x87 precision. |


## `state 0x38` is ordinary on-foot idle after leaving a car

**Status:** DISPROVEN.

Raw Steam state `0x38` retains the selected vehicle from `Game+0x8C57C`, uses the vehicle seat marker `PM_SEAT_ML`, and can tail-enter the confirmed vehicle-entry routine `FUN_004DDDB0`. The correct conservative label is a **vehicle-context post-exit / re-entry-capable transition hub**.

## Vehicle continuation corrections (2026-09-25)

| Old interpretation | Correction and status |
|---|---|
| `Game+8C57C` is an exclusively selected-car pointer | **DISPROVEN exclusivity:** common ingress copies a 0x48-byte action packet there; first word is its action object. |
| `+8C57C` and `+8C5A8` are separate context structures | **CORRECTED:** `+8C5A8` lies at packet+2C; pointer and index have distinct uses inside one packet. |
| All `2E..38` and the shared 14-state family are car choreography | **WITHDRAWN attribution:** field consumption alone does not prove vehicle ownership. `38/87/88` have independent car evidence. |
| Shared completion emits first Event1D and then a second event | **DISPROVEN:** 1D is written to packet+3C; one selected event is passed to global/object callbacks. |
| State66 has a normal default event pair | **UNSUPPORTED / corrected:** default reads both event/status from a stack local with no initialization on the reviewed phase-2 path; runtime reachability open. |
| State88 completes all dismount work | **CORRECTED:** it is a prelude leading to state38; motion handling, detach and Player cleanup continue there. |
| State88 directly clears player-car scheduler bit8000 | **NOT SHOWN by its code:** its direct clear is car+DC bit2. Car Event35 phase12 clears car+434 bit100. Keep all three separate. |
| Any raw `object+434 & ~0x8000` site is evidence of vehicle exit | **DISPROVEN as method:** the same offset/bit pattern occurs in non-car classes (for example a door-family virtual path). Ownership must be established before assigning car semantics. |
| Unqualified `004DCED0` is the GOG M_CENTER mode setup | **CORRECTED build:** Steam004DCED0, GOG004DCFA0. |

See `player/action_protocol.md` and `player/vehicle_choreography.md` for raw address evidence.


## Object-action taxonomy corrections (2026-09-25 continuation)

| Intermediate / old interpretation | Current status | Correction |
|---|---|---|
| `state 57 = BoxBase / Toolbox / Suitcase` because their handler contains `0x65` | **DISPROVEN** | The class callback receives native Event `0x19`; `0x65` is a packet subcommand/payload value inside that event. It is not native Event `65`. |
| `states 58/59 = Shaft` because the Shaft handler contains `0x66/0x67` | **DISPROVEN** | `0x66/0x67` are packet subcommands inside native Event `0x19`. The Shaft's independently observed native action event is `0x41`, which the selector maps to candidate `0x6C`, not gameplay states `58/59`. |
| `state 32 = Freight` | **DISPROVEN as exclusive identity** | Native Event `0x2E → candidate/state 32` is accepted by several object classes: Freight, Chest, BronzeStatue, BrianStoneConstruction and PushWardrobe. Use “shared Event2E object-manipulation state”. |
| mutable callback `0x005F5EF0` proves native Event `0x2F → state37` ownership | **DISPROVEN** | The large switch in that callback reads the callback data/argument domain, not the native event-ID argument. Its installation is real, but it does not identify state37. |
| `0D/57` is a confirmed Lever object family because `0D` loads `LEVER.PRM` | **CORRECTED / DOWNGRADED** | `0D` retains a strong Lever-resource clue. `57` may transition back toward `0D`, but its concrete object/native-event ownership is not established. |
| shared Player handler identity alone proves a shared object family | **DISPROVEN as method** | Example: the same Player handler serves Jukebox states `55/67`, Autoslide state `56`, and still-unresolved state `3B`. Classify from native event ownership, not handler sharing alone. |

### Late owner/lifecycle corrections

| Intermediate / old interpretation | Current status | Correction |
|---|---|---|
| `state 37` ownership can be inferred from mutable callback `005F5EF0` | **DISPROVEN route; ownership recovered elsewhere** | `005F5EF0` switches on another callback-data domain. The actual owner chain is `CObjectPhone/type 0x63 -> native Event 2F -> state 37`. |
| `state 57` remains completely ownerless after BoxBase was disproven | **CORRECTED** | A live native Event `65` producer is tied to Kaysen2-side choreography; exact action semantics remain open. |
| `state 58/59` are a paired Shaft/native-Event66 family | **DISPROVEN** | Shaft attribution was already false. Native Event `66` selects `58` from CNpcEnemy/CNpcThrow-family senders; `59` is selected by a different internal/global code `66`, i.e. another numeric domain. |
| normal York dismount must immediately clear `car+434 & 0x8000` | **NOT ESTABLISHED / likely wrong assumption** | `0x8000` selects player-car scheduler ownership, while scheduler code separately tests Player gameplay states `87/88`. Event35 phase12 bypasses `FUN_00550390`; known ownership-transfer clears live in NPC/AI car code. |

## Live player-car steering producer correction

**Old interpretation:** `FUN_00551640` is the unqualified current-player steering producer.

**Status:** **DISPROVEN for the live `0x8000` player-car branch.**

`FUN_00551640` belongs to the alternate `car+434 & 0x20000` branch. In the established live player-car mode, Player state `87` writes `car+0x4E0` with a `gameDelta60`-scaled slew, and `FUN_00555C20` later applies that value to front wheel slots `2..3`. The fixed-degree recurrence inside `FUN_00551640` remains valid for its own alternate branch, but it is not evidence that live `0x8000` steering is fixed per render dispatch.



| `09/0A` are only generic pre-weapon/equipment transition states | **SUPERSEDED** | Xbox raw ingress proves shoulder-triggered combat strafe left/right. PC retains the mirrored motion/state consumer and mode-2 combat camera, but the identified Xbox ingress call is absent from the homologous PC update tail. |

## Input/camera corrections (2026-09-26)

### `0x008A9980` is only a vague Player secondary-mode table
**DISPROVEN.** CPlayer transition code indexes it by committed Player state and passes the value to the CCamera mode setter, which writes `CCamera+0x154`. The camera dispatcher uses that field to index `0x008A9BC8`. It is the CPlayer-state → CCamera-mode table.

### The background 33.333 ms input callback caps gameplay input at 30 Hz
**DISPROVEN for the normal path.** The worker implementation exists, but CInput's shipped normal lifecycle does not activate its async-handoff path. Runtime producer census recorded only main-thread producer calls, zero background threads, and `+0xBC0 == 0` throughout steady-state gameplay. The confirmed artifact is one-deep staging and approximately one tick of latency.

### An extra CInput commit is equivalent to reordering poll and commit
**DISPROVEN.** Commit begins by copying current -> previous before deriving `rising = current & ~previous`. A second same-tick commit can therefore erase a fresh rising edge. The validated low-latency transaction keeps one commit and reverses only the native main-tick order to `poll -> commit`.

### PC mode-2 aim already applies a second 0.25 deadzone after the ZachFix Xbox curve
**DISPROVEN.** The compared PC constant at `0x00872B00` is zero; the local test is a nonzero check, not another 0.25 camera deadzone.

### One global right-stick timing fix is safe for every camera family
**DISPROVEN as an architectural assumption.** Mode0 re-anchors its target before the common free-look pass, mode2 aim has separate downstream delta-aware math, mode9 vehicle camera is anchor-relative with 0.25+4/3 shaping, modes3/6/7/13/14 are target-oriented signed-deadzone families, and modes10/11 are a separate incremental interaction camera consuming both pair0 and pair1.


## PhysX production-candidate corrections (2026-09-26 final)

### Broad/common realtime correction with live `maxIter=4` is production-safe
**DISPROVEN.** Correcting the identified elapsed boundaries did not eliminate the ~24 FPS collapse, and raising live solver capacity can participate in a self-sustaining hitch -> more substeps -> slower frame feedback loop.

### Scene-0 QPC elapsed alone is a complete physics fix
**DISPROVEN as a whole-system repair.** With eligible live `maxIter=1`, the solver timebase and high-refresh stability became good, but player throttle became extremely weak because persistent motor/brake setter magnitude remained tied to per-render `gameDelta60`.

### Gating only the active player-car phase to Xbox-like 30 Hz reconstructs Xbox vehicle behavior
**DISPROVEN.** The v7 runtime experiment reached the intended ~30 Hz cadence and `passedDelta60~=2`, then generated a second ~22–24 FPS feedback collapse while Scene-0 `maxIter` remained `1`. The original Xbox contract couples gameplay, setter cadence, scene timing and solver capacity; one isolated gate is not equivalent.

### One global vehicle `gameDelta60` division is a safe general repair
**DISPROVEN as a general rule.** Persistent motor/brake setters are a special case after solver normalization, but other vehicle/game quantities legitimately integrate over elapsed time. Normalize only a proven boundary, never the scalar globally.

## Native UI / COption corrections (2026-10-01)

| Old interpretation | Correction and status |
|---|---|
| `COption` is the natural reusable shell for ZachFix Settings | **REJECTED as the preferred architecture.** It is tightly coupled to stock row tables, global layouts, retail setting writes and Pause-specific tracking. A base selector-0 `CRdObject` task with a custom callback is the retail-proven lightweight path. |
| `COption` vtable `+0x30` is the scalar deleting destructor | **DISPROVEN.** Steam target is `006BAB20`, GOG `006BAA70`; it marks normal deferred removal. Final manager cleanup later calls `+0x08`, unlinks, then `+0x00(1)`. |
| `FUN_006B2BE0` is the XLY loader/parser | **DISPROVEN.** It is a guarded existing-resource accessor. Layout binding/parsing is a separate path (`00459A90` Steam / `00459AC0` GOG). |
| `FUN_004588C0` takes semantic control IDs | **DISPROVEN.** It bounds-checks a layout-local index and returns `base + index * 0x50`. |
| CLayout slots `10..15`, especially slot 15, are safe/reserved for ZachFix | **DISPROVEN.** Slot 13 has direct retail users, many accesses are dynamic, and COption can reset all 16 global slots. No slot is proven private. |
| `this+0x200` is COption's top-level selected row | **DISPROVEN.** Top-level category selection is `this+0x1FC`; `+0x200` is a value/subselection field in stock states. |
| `FUN_006245B0` is a generic native highlighter | **DISPROVEN.** It is stock COption logic tied to ten rows, static tables and slot 0. |
| `FUN_00624E40` is a generic action dispatcher | **DISPROVEN.** It writes retail player/settings state. |
| `FUN_0061F660` is a universal Confirm/action seam | **DISPROVEN.** It is a stock category-specific staged controller. |
| `DAT_01474CE8` is a general child owner suitable for ZachFix | **DISPROVEN.** It is stock Pause/COption tracking and is cleared by COption terminal behavior. |
| Existence of a child object automatically suppresses parent menu input | **DISPROVEN.** Pause suppression is explicit state/context gating; a ZachFix integration needs its own WAIT state. |
| A `+0x30` removal request means no more callbacks can occur that frame | **DISPROVEN.** Manager update precedes event-`0x12` rendering and cleanup occurs afterward, so same-frame render can still reach the marked object. |
| External state can be erased immediately when `+0x30` is requested | **DISPROVEN.** Keep a closing/tombstone record through the close frame and parent resume. |
| ZachFix external task state can be created after `FUN_006BAB80` | **DISPROVEN.** The callback setter synchronously emits event 0, so the state entry must exist before callback installation. |

See `ui/README.md` and `evidence/native_ui/README.md` for the current architecture.

## PhysX closure corrections (2026-10-01)

| Old / inherited interpretation | Current status | Correction |
|---|---|---|
| `gameDelta60 / 60` is always the exact real elapsed that should be passed to PhysX | **DISPROVEN as an exact-runtime assumption** | `gameDelta60` is a 60-Hz-relative wall-derived scalar and dividing by 60 is dimensionally seconds, but native rounding/mode/baseline behavior creates observed downstream differences from direct QPC. Use direct runtime evidence for an exact replacement contract. |
| Scene 0 is the only ordinary PhysX elapsed boundary that matters | **DISPROVEN** | The common ordinary producer fans one incoming elapsed value into a table of up to 20 active scenes before per-scene timing policy. Scene 0 was the main measurement target, not the full boundary. |
| All secondary scenes are always fixed `1/30,maxIter1` | **DISPROVEN** | `1/30,maxIter1` belongs to special timing records 1..19. The ordinary common path is not equivalent to this special policy. |
| Queue task `+0x08` is the live `maxIter` control | **DISPROVEN** | Live capacity is installed synchronously through `NxScene::setTiming` before enqueue. The ordinary worker reads scene/elapsed and does not reapply the copied queue value. |
| `NxScene` vtable `+0xFC` is `isWritable` | **DISPROVEN** | Current desktop mapping and queue use are consistent with an actor-count/`getNbActors`-style method. Do not use the older `isWritable` label. |
| `maxIter=4` is intrinsically unsafe | **DISPROVEN as a general rule** | The ~24-FPS collapse is caused by elevated capacity paired with legacy oversized elapsed, producing repeated multi-substep work and feedback. Capacity is required at fractional/low FPS once elapsed is corrected. |
| GOG `FUN_004E31E0` has 37 verified direct GroundSnap callers in the current export | **DISPROVEN for the current export** | The function body remains present, but current calls/xrefs/raw scans do not reproduce that caller census. Live reachability may be indirect, shifted, or superseded and remains OPEN. |
| `FUN_0053E8C0` is the verified current Event 6 dispatcher | **DISPROVEN** | Current Steam/GOG function indexes do not support that identity. Current GOG `FUN_0055F6A0` explicitly handles event ID 6 for one bounded prop state machine; generic Event-6 production is a separate question. |
| Event 6 can be treated as an internal solver-substep callback | **DISPROVEN** | Steam `FUN_006C5FF0` orders state 14 physics submission, state 7 completion/fetch, then state 8 object `+0x1C` Event 6 delivery. |
| GOG-only queue/catch-up mapping is sufficient and Steam homologs remain unknown | **SUPERSEDED** | Steam queue copy/enqueue is `FUN_0040BCE0/FUN_0040BC50`, ordinary worker `FUN_0040BAF0`, catch-up save/simulate `FUN_0040B850`, and fetch/restore `FUN_0040B940`. |

See `physx/README.md` and `evidence/physx_timing/README.md` for the current canonical map.

## Mega RE Census corrections (2026-10-04)

| Old / tempting interpretation | Current status | Correction |
|---|---|---|
| The selected CObjectCar control selector is `0x35` / decimal 53 | **DISPROVEN** | Raw Steam code writes and later matches `0x53` / decimal 83 at actor `+0x30`. The older numeral was a source-card transcription error. |
| Steam `005C92C0` is only a CRdDebug/serial-overlay helper | **DISPROVEN as an exclusive label** | One typed CObjectCar producer actually installs `005C92C0` as its callback. Debug/serial branches inside it remain real, but the routine also participates in vehicle object control. |
| PhysX record `+0x10/+0x14` are generic queue timing fields | **REJECTED** | The typed context/vector reconstruction shows context-specific uses, including alternate synchronous consumers. Keep them unnamed until a discriminating producer/consumer closes semantics. |
| The raw PhysX worker constant `0.0666666701` proves a live worker cadence | **REJECTED** | It is a verified numeric comparison constant in one worker path, not proof of unit, scheduling rate, or actual activation. |
| A normal `CLoadThread` direct-slot return or pending clear proves the resource loaded successfully | **DISPROVEN as a success rule** | Protocol completion and resource registration success are separate; selected registration helpers have nonuniform failure/"already present" behavior. |
| The new save read protocol result proves an exact full-file `ReadFile` success | **DISPROVEN as an exact-success rule** | The selected raw reader uses file-size information and does not itself establish a full bytes-read validator. |
| The apparent Rain/Haze globals are independent weather-manager singletons | **DISPROVEN** | Their addresses are fields inside the static CMap object at `013936F0 + 0xA3D24/+0xA3D28`. CMap is a retainer at the selected scope. |

## MegaRE final corrections (2026-10-08)

The final MegaRE correction register retires or narrows several additional interpretations that must not re-enter ZachFix work:

- the historical Ghidra `Functions` set is not the complete executable/callable universe;
- a CRT address range or forwarding stub is not proof of CRT-only/non-game ownership;
- a min/max function envelope is not a valid universal function-body ownership rule;
- the renderer resource registry is not an inline `owner+0x0C+index*0x18` array; the bank is pointed, and the COM object is further indirect through an output cell;
- D3D pool literal `0` is `D3DPOOL_DEFAULT`, not `D3DPOOL_MANAGED`;
- both selected physics halves are not phase-7 work; phase 14 precedes the first half and phase 7 precedes the second/fetch-shaped half;
- phase 8 / event 6 is not proof that all physics work succeeded;
- an audio Main token is not a globally unique playback-episode handle;
- the analyzed NativeUI cached directory cell is not a message body pointer;
- XAM `NULL` is not universally equivalent to detach;
- XCA rejection/failure does not guarantee old state remained untouched;
- a valid model packet does not guarantee latest-pose freshness;
- the old EffectAdmin 500-DWORD zeroed region is not established as the installed effect descriptor table;
- ItemManager construction/root existence does not prove the item table is already populated/ready;
- save/result/staging state does not imply one coherent durable read/write/reconstruction transaction;
- handle registration is not universally idempotent;
- cleanup/request/clear does not prove the final borrower/backend operation has completed;
- retained shadow/frustum state is not automatically a same-frame fresh build.

The authoritative full register is imported as [`DP_RE_CORRECTIONS_FINAL.md`](evidence/mega_re_final_2026-10-08/reports/DP_RE_CORRECTIONS_FINAL.md).
