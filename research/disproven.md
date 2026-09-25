# Disproven / corrected interpretations

These entries are intentionally retained so future research does not rediscover them.

| Old interpretation | Current status | Correction |
|---|---|---|
| `0x008A9758` has 22 Player states | DISPROVEN | `CPlayer+0x654` is used through at least `0x88`; recovered range contains 137 slots. |
| `FUN_00522E20` is invalid/assert fallback | DISPROVEN | It is a real large gameplay/locomotion handler and state-0 anchor. |
| CPlayer Event 5 pushes CCT transform | DISPROVEN | Player Events 5/6/7 are default/no Player-specific work. |
| Event 6/7 are Player post-physics CCT sync | DISPROVEN | Same reason; relevant work lives elsewhere in common Actor/Player phases. |
| `vtable ~0x007741F0` is the missing Player CCT component | DISPROVEN | The region belongs to `CNpcDog` vtable inheritance; York has its own post-state CCT path. |
| `FUN_0048B0A0` is the York CCT bridge | DISPROVEN | It is an NPC-dog override calling common NPC character code. |
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
| PC terrain flicker is caused by collapsed grass DDS resources | DISPROVEN as sole cause | Grass resource collapse is real, but Xbox resource restoration does not remove the canonical breakthrough artifact. |
| PC `HOUSE_LIST.NOD` asset itself contains 64 swapped/corrupt keys | DISPROVEN | Raw normalized PC/Xbox assets are byte-identical; the 17/64 split appears only after PC runtime preprocessing. |
| Global frustum disable is the pillow/mirror fix | DISPROVEN | The candidate passes normal frustum and is rejected by a subsequent visibility-volume test; production bypasses only the affected outer-world volume callsite. |
| Native XInput replaces DP's entire action/binding system | DISPROVEN | ZachFix presents a synthetic WinMM-compatible state to DP's common evaluator and keeps `configJ.cnf`/native action routing. |
| PC gameplay input is capped at 30 Hz because CInput has a ~33.333 ms callback | **DISPROVEN** | The background sampler is real, but the normal main thread also polls every game tick. The normal-path artifact is a one-slot staged snapshot and roughly one game-tick sample latency, not a 30 Hz input cap. |
| A synthetic XInput JOYINFOEX layout can be dropped into vanilla `configJ` semantics unchanged | **DISPROVEN / corrected** | Vanilla expects right-stick U/R and shared-Z triggers. ZachFix's X/Y, Z/R, U/V synthetic layout needs the binding-evaluator remap already implemented in `native_xinput.cpp`. |
| The gameplay mouse-look transform after `GetCursorPos/SetCursorPos` is still unknown | **SUPERSEDED** | The route is now mapped through mouse look producers, CInput temp +44/+48, PC filtering, public +EC/+F0, `FUN_007089E0(pair 1)`, and camera consumers. Exact FPS sensitivity remains an open timing question. |
| Xbox360 profile should restore original Xbox left-stick aiming | DISPROVEN as production goal | ZachFix restores Xbox shaping but intentionally preserves Director's Cut right-stick aim routing. |
| Vehicle analog triggers require a late PhysX torque multiplier | DISPROVEN | Restoring the three native analog vehicle consumers propagates analog input through the existing game-side vehicle/PhysX path. |
| DP mouse-look can be assumed to be DirectInput-only | DISPROVEN as assumption | A material Win32 `GetCursorPos`/`SetCursorPos` recenter path is confirmed; any DirectInput contribution remains unproven. |
| `FUN_00537730` proves ordinary free-look rotates at `2 degrees per frame` and therefore scales directly with FPS | **DISPROVEN as timing inference** | Camera update calls the mode handler first. Mode0 reconstructs the yaw target from Player/anchor state, then the common path may call `00537730`. The local 2-degree operation is frame-local relative to a refreshed target; it does not by itself define angular velocity. |


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
**DISPROVEN for the normal path.** The main thread also polls every normal game tick; the confirmed artifact is one-slot staging and approximately one tick of latency.

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
