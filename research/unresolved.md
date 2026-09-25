## Physics / PhysX

Production timing work is **suspended**, not awaiting another local patch. The remaining question is architectural: can one end-to-end contract simultaneously preserve solver time, vehicle forces/steering/readback, CCT behavior, prop forces, hitch recovery, pause/load/reset semantics and low/high-FPS behavior on both PC builds?

Do not reopen with another Scene-0-only, common-boundary-only, `maxIter=4`, fixed-60 dispatcher, or player-car-only 30 Hz experiment. Any future attempt must define the entire timing contract first and validate it as one system.

# Unresolved targets

## Player

The 137-slot table is established. The vehicle pass corrected broad mid-state ownership: see `player/action_protocol.md`. Some previously vehicle-labelled families require concrete object attribution.

- exact gameplay identity of state `03`;
- exact meanings of early short transitions `04..08`, `0C`, `12`, `13`;
- individual action labels for weapon/combat pairs `14..29`;
- remaining concrete object producers/receivers inside action choreography `2E..68`; Phone `37`, Telescope `65`, Kaysen2-side `57`, Kaysen3-side `51`, and CNpcEnemy/CNpcThrow-family `58` now have producer-side ownership evidence; packet `+0x8C57C` alone is not vehicle evidence;
- runtime reachability and stale-stack event/status behavior of shared-handler state `66`;
- producers of packet status `20`, including the confirmed Player Event `67` packet-refresh route;
- exact friendly semantics of interaction-adjacent states `47..4A,53,59,64`; state `52` is now special actor-target choreography, `58` has CNpcEnemy/CNpcThrow-family ownership evidence, and `59` is an internal-code companion state rather than native Event66 ingress;
- exact causes/types represented by damage states `7E,7F,81,83`;
- identity of state `7A`;
- exact special action behind state `86`;
- map target subtypes `0x759..0x766` to concrete world-object classes/names rather than guessing from downstream behavior.

## Character controllers

- harden the exact shape assignment of Player `+0x94C` vs `+0x950` with a preserved raw descriptor-construction trace;
- document Event-9 controller synchronization separately from Event-1 state→CCT reconciliation.

## Vehicle

- semantic label for the historical `0x20000 → MovePlyCar` branch relative to the live `0x8000` player-car branch;
- determine whether ordinary dismount is expected to clear `car+434 & 0x8000` at all. Current evidence distinguishes scheduler/ownership mode from Player gameplay state: the `0x8000` scheduler branch itself separately tests Player states `87` and `88`. Event35 phase12 does not call `FUN_00550390`. A real `0x8000 -> 0x10000/0x20000 + 0x08000000` transfer helper exists in NPC/AI vehicle ownership code, not yet in York normal-exit choreography;
- friendly identities of car discriminator `+424` values 0/1 and Player motion pairs 258B/258D versus 258C/258E;
- runtime confirmation of the statically reconstructed 38 → 87 → 88 → 38 → 00 path, including the cleanup gate before state00.


## PhysX / game-cadence boundary

Production timing work is retired after the v6-A/v7 closure. There is no pending local
Scene-0/common-boundary/cadence patch to validate. A future restart would require one
end-to-end contract covering solver elapsed and capacity, persistent vehicle setters,
state-87 steering/readback, CCT helpers, prop forces, hitch recovery, pause/load/reset
semantics, low/high FPS, and both supported PC builds. See `physx/README.md`.

## World / LOD

### Frustum taxonomy

- identify the semantic/factory identity of explicit classifier types `0x2B` and `0x4F`;
- classify the specialized `vtable+0x24` consumers that do **not** use common `FUN_006BCE30`;
- audit late/resource-specific class overrides such as `FUN_0057C650` and determine which concrete object families exercise them at runtime.

### Alternate 3D residency

- map all 75 `DAT_008AA6B0/DAT_008AA6B4` pairs to resource names/types;
- find the exact policy/writers driving `object+0x444` and characterize its distance/cell thresholds;
- identify which object classes actually use the `+0x416` alternate-resource index.

### Shadows

- finish a GPU-state-level documentation of `renderer+0x63C4` and its three-cascade render pass, even though the 1000/3000 distance mechanics are now resolved;
- decide whether a production shadow-distance control is desirable and what performance/safety bounds it should have.

### Asset-specific LOD

- isolate the bridge model/resource package before assigning its visible near/mid/far transitions to mesh LOD, alternate residency, shadow distance, or a mixture.

## Rendering

### Depth / PostFX

- preserve a final runtime evidence bundle for packed-vs-INTZ band A/B;
- map the native DP packed-depth **producer** and packing shader/formula, not only ZachFix's decoder;
- keep compatibility fallback semantics documented separately from high-precision native INTZ.

### Water

- main Xbox-water shading/interpolator regression beyond the confirmed `c243` ordering bug;
- direct rich Xbox river pixel-shader vs PC lit-water comparison, including normal/light/view spaces and effective UV frequency.

### Trees

- extract and compare Xbox/PC `T18_00_00.XMD` submeshes and render flags;
- resolve `MAGE_B_NSR_3` material 6 and its Xbox/PC texture bindings;
- compare foliage texture alpha/mips and Xbox `RE_LEAVES` vs corresponding PC shader;
- do not assume alpha threshold or billboard LOD until this comparison is complete.

### HOUSE_LIST / day-night

- isolate the exact generic resource preprocessing callback that creates the `17 converted / 64 unconverted` key pattern;
- test whether the same descriptor/converter bug affects other Xbox-identical structured resources.

### Terrain

- branch remains closed unless exact same-location Xbox hardware evidence or an exact geometry/state mismatch appears.

### Other

- any remaining native PC/PS3 renderer-contract regressions.

## Input

### CInput timing / latency

- runtime-measure the confirmed normal one-slot staging path: main-thread commit of the prior pending snapshot followed by synchronous polling for the next snapshot;
- design a read-only/A-B experiment for latency before changing order. A naive extra `FUN_00708300` commit can disturb rising/repeat/previous masks, so do not patch by analogy alone;
- determine whether the dormant `CInput+0xBC0` background-only mode is ever activated by indirect/data-driven code in shipped gameplay.

### Camera / look timing

- ordinary mode0 free-look no longer needs an FPS-normalization A/B based on the local `2 degrees` instruction alone; the mode handler re-anchors the target before the common free-look pass. Keep Xbox-restoration as a separate response-model experiment, not a timing fix;
- test camera modes `10/11` (`states 02/46`, handler `0053C600`) separately. They directly accumulate pair0/pair1 X/Y as approximately one degree per update after a 0.25 threshold; exact runtime cadence/effect remains to be measured;
- do **not** include mode 9 vehicle camera in a global timing patch: it already uses signed 0.25 deadzone subtraction, 4/3 renormalization, data-driven sensitivity, and an anchor-relative target model;
- keep mode 2 aim separate because downstream aim math already consumes `gameDelta60`; only the device-mode guard on Xbox aim shaping is currently a confirmed ZachFix-side fix;
- runtime A/B camera modes `10/11` first. They remain the strongest static incremental-camera timing candidate; test state02 and state46 separately and record camera mode, input device, and update cadence.

### Action semantics / UI

- finish friendly naming of low logical bits only from concrete consumers; keep generic bit identity separate from context-specific meaning;
- preserve the now-confirmed UI direction composites `0x4001/0x8002/0x10004/0x20008` and held/rising/repeat selector domains;
- map confirm/cancel/menu-family consumers (`0x1000`, `0x800`, etc.) far enough to support glyph/remapping work without globally overnaming them.

### Original Xbox control restoration

- runtime-validate the `Gameplay.RestoreCombatStrafe` bridge on GOG `00504567`; Steam `00504497` ingress, native combat gates, and one-press/one-request shoulder edge behavior are validated;
- extend state `09/0A` validation across more combat contexts and 30/60/high-FPS cases, including animation completion to `0E`, collision, and camera behavior;
- decide eventual production binding policy: retain fixed physical LB/RB semantics for the Xbox360 profile or introduce explicit remappable Strafe Left/Right actions. Do not reinterpret Xbox logical `0x100/0x200` as PC action masks;
- Quick Turn `0x0B` remains research-only after the first runtime trigger experiment was removed; revisit only if its original control semantics can be exposed without an awkward binding policy.

### Controller / vehicle

- preserve the distinction between vanilla generic shared-Z trigger-like channels and the three vehicle-specific binary LT/RT cuts;
- continue consumer-level Xbox-vs-PC comparisons only where they affect a concrete gameplay behavior.

## Runtime stability

- long-session FPS degradation: continue from D3D9 resource-lifetime audit and creation-site attribution; avoid assuming a generic memory leak without evidence.

## Weapon-state remaining questions (2026-09-25)

The class/state taxonomy is now mostly closed. Remaining weapon-specific questions:

- producer/source semantics of **CPlayer Event `0x44`**; current evidence points to indirect/data/script-driven production;
- friendly identity and gameplay use of ITEM class `99` / `CWP1231` / state `2C`;
- whether class `8` (`28/29`) can be activated by any non-ITEM path, or is fully orphaned;
- exact roles of boundary states `10/11` and the precise gameplay distinction of some primary class handlers;
- variant semantics of the second ITEM weapon block (`34..66`), including the class change observed for `CWP1121`.


## Object-action protocol taxonomy (updated 2026-09-26)

Receiver/producer ownership has now closed or strongly grounded Door `2E/2F/30/31`, shared Event2E state `32`, Phone `37`, car `38`, HideBox `39/3A`, Item/Coffee `4F`, Kaysen3-side `51`, Jukebox `55/67`, Autoslide `56`, Kaysen2-side `57`, CNpcEnemy/CNpcThrow-family `58`, Telescope `65`, and the special actor/scripted-control role of `52/53/54`.

Highest-value remaining identities/chains:

- native Event `38 -> state41` object owner;
- native Event `39 -> state3B` object owner;
- native Event `4D -> state42` object owner;
- native Event `5E -> state52` exact external producer/receiver ownership. State52 itself is no longer semantically unknown because the Event6D special-actor ingress is mapped;
- exact internal role/producer of sibling state `59`;
- **native Event `4E -> state66` producer and runtime reachability**. The selector/commit path is structurally live, and phase-2 state66 has a static stale-stack hazard if reached.

Protocol-only candidates intercepted before gameplay-state commit remain a separate domain. Confirmed examples include `41->6C` (Shaft), `50->71` (Exclusive), and `62->73` (Vendor).
