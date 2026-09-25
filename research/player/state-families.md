# CPlayer state families

**Target:** PC Deadly Premonition / ZachFix
**Primary state:** `CPlayer+0x654`
**Previous state:** `CPlayer+0x658`
**Dispatch:** `0x008A9758[state]`
**Recovered PC range:** `0x00..0x88`

This document classifies states by **real subsystem edges**, not by address proximity or animation appearance.

Evidence priority for this map:

```text
raw Steam/GOG assembly
    > PC decompiler when boundaries survive
    > cross-build agreement
    > reconciled runtime research
    > semantic inference
```

A family label means the state is demonstrably fed by, reads, or returns through that subsystem. It does **not** imply that every individual state in the family has a final player-facing name yet.

---

## 1. Current family map

| States | Family / role | Status | Strongest mechanical anchor |
|---|---|---|---|
| `00` | ordinary locomotion anchor | **CONFIRMED** | real movement handler; forward path contains known high-FPS displacement bug |
| `01,02` | context-target acquisition / interaction pre-states | **PARTIAL / STRONGLY_SUPPORTED** | scan/validate world targets; can hand off to `45/46` |
| `03` | special CCT-bypass Player mode | **PARTIAL** | grouped with `7D/87` at Player CCT boundary; exact gameplay role open |
| `04..08` | short core Player transition states | **PARTIAL** | compact handlers; most return to `00`; several entered directly from state-0 manager |
| `09,0A` | original Xbox combat strafe left/right; PC consumer remnants | **CONFIRMED Xbox semantics / CONFIRMED PC consumers** | Xbox shoulder ingress selects `09/0A`; PC retains mirrored `0x242C..0x2431` handler, mode-2 combat camera, completion to `0E` |
| `0B` | original Xbox Quick Turn temporary heading state | **CONFIRMED Xbox semantics / CONFIRMED PC consumer** | Xbox stick-back + Run/X enters `0B`; PC retains yaw-to-target interpolation and returns to saved state `+0xA11` |
| `0C` | short motion/alignment transition | **PARTIAL** | motion resources + return to ordinary state; exact label open |
| `0D` | operated-object interaction with `LEVER.PRM` resource clue | **STRONGLY_SUPPORTED** | dedicated ingress sets `0D` and loads `LEVER.PRM`; exact object ownership still needs receiver evidence |
| `57` | Kaysen2-side object-action choreography | **STRONGLY_SUPPORTED owner** | live native Event `65` sender builds the action packet from Kaysen2-side code; old Lever/BoxBase labels remain withdrawn |
| `0E,0F` | weapon/equipment mode boundary / action selector | **CONFIRMED family** | synchronize active weapon object and dispatch into weapon action states |
| `10,11` | weapon action/submode states | **CONFIRMED family** | direct `player+0x864` weapon-object use; return through `0F` |
| `14..2C` | weapon/combat action family | **CONFIRMED** | active weapon handle `+0x864`, weapon subtype `+0x4A8`, weapon-specific action tables |
| `2E..37` | object-action choreography; several object families now resolved | **CONFIRMED protocol / PARTIAL identities** | `2E/2F/30/31` Door-family; `32` shared Event2E manipulation; `37` Phone via native Event `2F`; others remain open |
| `38` | vehicle entry/exit animation hub | **CONFIRMED static structure** | seat marker, car fields, motion pairs, entry commit and exit cleanup |
| `39,3A,3B,41,42,51,55,56,65,67` | object-action choreography participants | **CONFIRMED protocol / PARTIAL identities** | `39/3A` HideBox; `55/67` Jukebox; `56` Autoslide; `65` Telescope; `51` Kaysen3-side owner strongly supported; `3B/41/42` owners open |
| `43,44,5A..63,66,68` | shared object-action completion | **CONFIRMED static structure** | packet status `20` wait, status `1D` plus one native event; `66` anomalous default branch |
| `3C,3D,3E,3F,40,45,4B,4C,4D,4E` | context/world-object interaction family | **CONFIRMED family** | `FUN_0050B7B0` maps target subtype directly to these Player states |
| `4F` | Item/Coffee object-action state | **CONFIRMED family** | native Event `3A` is accepted by `CObjectItem`/`CObjectCoffee` and selects `4F` |
| `50` | interaction / inventory-result sibling | **PARTIAL** | shares handler with `4F`, but exact object protocol identity remains open |
| `52,53,58,59,64` | interaction/animation-adjacent states | **PARTIAL** | `58` has live native Event `66` senders in CNpcEnemy/CNpcThrow-family code; `59` is a separate internal-code-`66` companion state, not another Event66 ingress; `52/53/64` remain open |
| `54` | scripted health-loss / reaction state | **PARTIAL** | subtracts `0.2 * maxHealth`; exact damage source not proven |
| `69..79` | null table slots | **CONFIRMED** | `0x008A9758` entries are null in recovered range |
| `7A` | special Player transient | **UNKNOWN / PARTIAL** | real handler with `0x2472..0x2476` motion set; ingress semantics not yet recovered |
| `7B,7C` | item-use special presentation/action states | **STRONGLY_SUPPORTED** | item-effect dispatcher enters them from distinct item command classes |
| `7D` | special external-object / scripted interaction | **STRONGLY_SUPPORTED** | stores external object handle at `+0x874`; dedicated cleanup signals object and returns to `00`/`82` |
| `7E,7F,80,81,82,83,84,85` | damage / hit-reaction / recovery family | **STRONGLY_SUPPORTED overall** | shared health-depletion checks route to `85` |
| `80` | heavy vehicle-hit reaction | **CONFIRMED** | car collision path, large `abs(car+0x13E4)`, `-20 HP` |
| `84` | light vehicle-hit reaction | **CONFIRMED** | car collision path, smaller impact, `-7 HP` |
| `82` | nonfatal damage follow-up / recovery | **STRONGLY_SUPPORTED** | entered from nonfatal `7F`; also special-object cleanup path |
| `85` | fatal/death reaction state | **STRONGLY_SUPPORTED** | health-zero/fatal ingress from multiple paths and terminal-style handler setup |
| `86` | special-action transient | **PARTIAL / UNKNOWN** | writers sit in broad Player/world setup; handler quickly returns to `00` |
| `87` | active / in-player-car | **CONFIRMED** | vehicle entry writes `87`; ordinary on-foot CCT reconciliation bypassed; car `0x8000` mode active |
| `88` | vehicle-exit prelude leading back to hub `38` | **CONFIRMED static structure** | four local phases; car yaw transfer, car+DC bit 2 clear, transition to `38` |

`12` and `13` remain intentionally unlabelled. `13` is a tiny tail-dispatch stub; neither currently has enough subsystem evidence for a useful gameplay name.

---

## 2. Context fields and object-action packet

Several old interpretations became much easier once the Player state machine was separated by the data it consumes.

### Active weapon object

```text
CPlayer+0x864
    ↓
weapon object
    ↓
weapon+0x4A8 subtype
```

This is the central anchor for `0E..2C`.

The weapon update constructs/uses nodes including:

```text
PN_SIGHT
PN_MUZZLE
```

and performs sight/muzzle transform and ray work. This removes the earlier ambiguity that `+0x864` might be a generic component.

### Object-action packet and index-based target

`Game+0x8C57C` begins a **0x48-byte object-action packet**. Its first word is the action object pointer, containing the selected car in vehicle paths. The common ingress copies 18 DWORDs from the incoming action packet, so this field is not vehicle-exclusive.

`Game+0x8C5A8` is packet offset `+0x2C`, used as a world-resource target/index. Index-based setup can clear the first word and fill this index. These fields have distinct uses inside one packet, not two separate context structures.

Vehicle setup GOG `004DC4A0` / Steam `004DC3D0` supplies a car, but does not establish the meaning of every other writer. See `action_protocol.md` for the raw packet-copy and Event `67` refresh evidence.

### Special scripted/external object

```text
CPlayer+0x874
```

Used by state `7D`; cleanup later resolves this handle, sends a native event, clears the handle, and returns control to the ordinary state machine.

---

## 3. Weapon/combat family

### Boundary states `0E/0F`

Both states synchronize the active weapon object through the weapon-side helpers. The action selector reads the current `ITEM.PRM` row and uses byte `ITEM+0x6D` as a **weapon-class ID**.

The recovered class-to-primary-state map is:

| weapon class | primary Player state | secondary/Event44 state | status |
|---:|---:|---:|---|
| `1` | `14` | `15` | live |
| `2` | `16` | `17` | live |
| `3` | `18` | `19` | live |
| `4` | `1A` | `1B` | live |
| `5` | `1E` | **reuses `1B`** | live; dedicated `1F` inert |
| `6` | `20` | `21` | live |
| `7` | `24` | `25` | live |
| `8` | `28` | `29` | code survives, but no `ITEM.PRM` record uses class 8 |
| `9` | `2A` | none in Event44 selector | live |
| `10` | `2B` | none in Event44 selector | live |
| `11` | `26` | none; `27` inert | live primary only |
| `12` | `22` | none; `23` inert | live primary only |
| `13` | `1C` | none; `1D` inert | live primary only |
| `99` | `2C` | none | special/rare item class |

This replaces the earlier weaker description of `14..2C` as merely a contiguous combat family.

### Base inventory item groups

The Xbox `ITEM.PRM` used for the combat comparison has 379 records of `0xDC` bytes, and the compared PC payload differs in only ten bytes, all in damage/tuning fields rather than the class byte `+0x6D`. The first normal weapon block therefore gives a reliable internal item-ID-to-class map for PC.

The externally documented inventory order supplies human-readable names for IDs `1..30`; use those names only as descriptive labels, not as the source of the class mapping.

| class | base item IDs | internal codes | descriptive weapon group |
|---:|---|---|---|
| `1` | `1..7` | `CWP1011..CWP1061` plus `CWP1001` | Wrench / Hammer / Bar / knives / Sickle / Hatchet family |
| `2` | `8..12` | `CWP1071..CWP1111` | Golf Club / Bat / Steel Pipe / Shovel / Hoe family |
| `3` | `16..18` | `CWP1151..CWP1171` | Saber / Sword / Light Sword |
| `4` | `22,23,30` | `CWP0011`, `CWP0021`, `CWP0091` | FBI 9mm / Sheriff 9mm / Dart Gun |
| `5` | `24` | `CWP0031` | 10mm SMG |
| `6` | `27` | `CWP0061` | 12Ga Shotgun |
| `7` | `13..15,20,21` | `CWP1121..CWP1141`, `CWP1181`, `CWP1201` | Fork / Axe / Ice Axe / Chain Saw / Legendary Guitar family |
| `8` | none | none | legacy/orphan candidate |
| `9` | `29` | `CWP0081` | RPG |
| `10` | `28` | `CWP0071` | Wesley Special / flamethrower |
| `11` | `19` | `CWP1191` | Grass Cutter |
| `12` | `25` | `CWP0041` | 5.56 Assault Rifle |
| `13` | `26` | `CWP0051` | .357 Magnum |
| `99` | `32` | `CWP1231` | special/hidden weapon; friendly name unresolved |

A second weapon-item block around IDs `34..66` mostly repeats the same `CWP` resources as variant records. Do not assume every duplicate has an identical class: at least `CWP1121` changes class between the two blocks.

### Event `0x44`: target-bound weapon action entry

`FUN_00509C20` handles Player Event `0x44` (`'D'`). It reads the active item class, selects:

```text
1 → 15
2 → 17
3 → 19
4 → 1B
5 → 1B
6 → 21
7 → 25
8 → 29
```

then stores the event source object in:

```text
CPlayer+0x878
```

and enters the selected state through `FUN_00528F40`.

The live paired handlers for `14/15` and related classes use this object as an attack target: they consume its world position, orient Player toward it, construct an attack/hit packet, and dispatch object Event `0x1C` to the target. Therefore the safe label for the working odd states is:

```text
15/17/19/1B/21/25/29
    = target-bound / object-event weapon variants
```

Do **not** currently call Player Event `0x44` an enemy-hit event. A previous investigation accidentally conflated Player Event `0x44` with an unrelated enemy internal action-state ID `0x44`; that chain was disproven.

The actual producer of Player Event `0x44` is not present as a simple literal direct dispatch in the PC executable. Current best status is **data/script-driven or indirect producer OPEN**.

### Inert odd aliases: `1D/1F/23/27`

These four slots share handlers with live primary states, but their handlers explicitly gate on the even state:

```text
1C/1D handler: if state != 1C → return
1E/1F handler: if state != 1E → return
22/23 handler: if state != 22 → return
26/27 handler: if state != 26 → return
```

A binary census of all 161 direct calls to `FUN_00528F40` found no immediate ingress to `1D`, `1F`, `23` or `27`, and no simple `MOV reg, immediate → PUSH reg → transition` ingress either.

Current classification:

```text
1D, 1F, 23, 27
    = reserved / inert weapon-state aliases
      (strongly supported; hidden data-driven ingress not absolutely excluded)
```

This also explains why Event `0x44` does not select them.

### Legacy class `8`: `28/29`

Both states have real code and Event `0x44` knows about class `8`, but the recovered 379-row `ITEM.PRM` contains **zero** records with class byte `8`.

Therefore ordinary inventory-driven PC runtime cannot select class 8 from the current ITEM table. Treat:

```text
28/29 = legacy/orphan weapon-class path candidate
```

until a non-ITEM producer is found.

### States `10/11`

Both directly resolve `player+0x864`, inspect the active weapon subtype, perform weapon-side work, and later return through `0F` or ordinary state. Their exact action distinction remains open.

### Motion-family corroboration

The state handlers use distinct Player motion banks that agree with the class separation (`P0A0`, `P0A2`, `P0A4`, `P0S*`, etc.). Motion naming is corroborative only; the authoritative class mapping is the `ITEM+0x6D` selector described above.

---

## 4. Generic world-object interaction family

`FUN_0050B7B0` is a high-level context interaction dispatcher. It validates interaction state, target angle and capability masks, obtains the target subtype via `FUN_005E2150`, then chooses the Player state.

Recovered direct mapping:

| `FUN_005E2150(target) - 0x759` | Player state |
|---:|---:|
| `0` | `3F` |
| `1` | `3C` |
| `2` | `3D` |
| `3` | `40` |
| `6` | `45` |
| `7` | `4B` |
| `10` | `4C` |
| `11` | `4D` |
| `12` | `4E` |
| `13` | `3E` |

After selection it stores target/context data and enters the chosen state through the normal Player transition routine.

This proves the **family** but does not yet prove friendly subtype names such as `door`, `vendor`, `pickup`, etc. Some downstream logic definitely reaches `CDoorManager`, but the exact `0x759..0x766` subtype dictionary is still open.

### `4F/50`

A separate interaction result path performs inventory/message work and enters `4F` or `50` on successful branches, while error/fallback messaging can enter `45`.

Conservative label:

```text
4F/50 = interaction / inventory-result subfamily
```

---

## 5. State `0D` and the corrected status of `57`

The strongest ingress to state `0D` is `FUN_00507840`:

```text
configure Player interaction transform/camera
    ↓
state = 0D
    ↓
load LEVER.PRM
    ↓
store resource in Player-side interaction field
```

This remains a strong operated-object / lever-resource clue for **state `0D` itself**.

The older conclusion that `0D ↔ 57` therefore forms a confirmed Lever object family was too strong. `57` does transition back toward `0D` in the recovered Player logic, but receiver-side protocol work did not establish a Lever native-event ownership chain for `57`. A temporary `57 = BoxBase` interpretation was also disproven: the apparent `0x65` in `CObjectBoxBase` is a **packet subcommand inside native Event `0x19`**, not native Event `0x65`.

Current status:

```text
0D = operated-object state with LEVER.PRM evidence      STRONGLY_SUPPORTED
57 = Kaysen2-side object-action choreography          STRONGLY_SUPPORTED owner
0D/57 same secondary mode ID 13                         CONFIRMED numeric grouping
0D/57 same concrete object family                       DISPROVEN / not supported
```

---

## 6. Shared object-action choreography

**Correction:** the old vehicle-wide classification of `2E..68` was based on an overly narrow interpretation of `Game+8C57C`. That address starts a shared action packet. The wider range must be classified by concrete producers/receivers, not by this field alone.

The common selector GOG `005092A0` / Steam `005091D0` maps native action events to candidate action/state IDs behind capability checks. See `action_selector.md` for all 80 inspected entries; candidate IDs at `Player+660` are not automatically committed states at `Player+654`.

Receiver-side class handlers now resolve several live-state families independently of packet-pointer ownership:

```text
Door-family native 2D/45/46/47/48 → states 2E/30/2F/31
native 2E shared by Freight/Chest/BronzeStatue/BrianStoneConstruction/PushWardrobe → state 32
Car native 35 → state 38
HideBox native 36/37 → states 39/3A
Item/Coffee native 3A → state 4F
Autoslide native 4F → state 56
Jukebox native 51/52 → states 55/67
Phone native 2F → state 37
Telescope native 68 → state 65
Kaysen2-side native 65 → state 57 (owner strongly supported)
CNpcEnemy/CNpcThrow-family native 66 → state 58 (family strongly supported)
internal/global code 66 → state 59 (separate numeric domain)
```

Conversely, `CObjectShaft` native Event `41 → candidate 6C`, `CObjectExclusive` `50 → 71`, and `CObjectVendor` `62 → 73` demonstrate that the selector's candidate domain is wider than the committed gameplay-state domain. See `object_action_taxonomy.md`.

The 14-state handler `43/44/5A..63/66/68` is an object-action completion handler. It waits for packet status `20`, then 13 mapped states write packet status `1D` and dispatch one corresponding native event before requesting state `00`. The old “first event / second event” table conflated a packet field with an event ID.

State `66` selects the default branch, which reads event/status from a stack local without initializing it on the reviewed phase-2 path. Its runtime reachability and effects remain open.

State `51` consumes an action object and uses `N_WRISTL/N_ARML2/N_ARML1/N_SLDRL` alignment. A live native Event `5D` sender is now tied to `CBossKaysen3`-side code, so a Kaysen3-side owner is strongly supported; the exact player-facing action name remains open.

State `59` shares handler `004F8A70` with `58`, but the handler's native Event `66` acknowledgement is specific to the `58` branch. A separate internal/global choreography path selects `59` from a different code value `0x66`. This is another concrete example of identical numeric values in different domains.

See `action_protocol.md` for the packet protocol, `object_action_taxonomy.md` for receiver-grounded object families, and `vehicle_choreography.md` for the positively identified car subset.


## 6A. Special actor/scripted-control layer around 52/53/54

A separate context at `Game+0xCEB20/+0xCEB5C` is populated by boss/actor code and consumed by Player choreography. CPlayer Event `0x6D` selects `53/54/52` from payload modes `2/3/4`; `52` is additionally reachable from native object-action Event `0x5E`. This is a distinct protocol from the generic `Game+0x8C57C` action packet.

- `52`: special actor-target choreography, motions `24B8/24B9/24BA`, `N_ARML1`.
- `53`: scripted-control state, motion `24B3`; direct actor-context use not yet proven.
- `54`: special actor-context choreography, motions `24B4..24B7`; earlier scripted health-loss behavior is part of this state rather than evidence for a generic damage family.

See `special_actor_choreography.md`.

---

## 7. Vehicle-specific `38/87/88`

- `38`: vehicle entry/exit animation hub, using `258B/258D`, intermediate `2471`, and exit pair `258C/258E`; can enter `87` or invoke cleanup toward `00`.
- `87`: active player-car state; attached to `PM_SEAT_ML`, with car mode bit `8000` enabling the established gameplay/wheel/PhysX branches.
- `88`: exit prelude with a four-phase local counter; copies car yaw back to Player, clears **car+DC bit 2**, and requests `38`. It does not itself complete all exit choreography.

The car-facing protocol is native Event `35` plus a phase word at packet `+3C`. The car consumer `GOG 005422A0 / Steam 005421D0` interprets phases `0F..15`. These phase values are not additional native event IDs.

See `vehicle_choreography.md` for the address dictionary, phase tables, conditional state transitions, and retained limits on the exact clearing of car scheduler bit `8000`.

---

## 8. Damage / reaction family

The common discriminator is the Player health-depletion helper (`FUN_004FE670`) and convergence on `85`.

### Confirmed vehicle-hit states

The car collision path uses `abs(car+0x13E4)`:

```text
2 <= impact < 50
    → current HP -= 7
    → state 84

impact >= 50
    → current HP -= 20
    → store impact angle
    → state 80
```

So:

```text
84 = light vehicle-hit reaction   CONFIRMED
80 = heavy vehicle-hit reaction   CONFIRMED
```

### Fatal convergence

Several reaction states check health depletion and route to `85`:

```text
7E → 85 or 00
7F → 85 or 82
80 → 85 on fatal result
83 → 85 or 00
84 → 85 or 00
```

Independent Player health-management logic also sends fatal health-zero cases to `85` unless an explicit special state exemption applies.

Thus:

```text
85 = fatal/death reaction state
```

is strongly supported without relying on animation appearance.

### State `82`

`7F` chooses `82` specifically on the nonfatal path. Special-object (`7D`) cleanup can also enter `82`. It is therefore best treated as a nonfatal recovery/follow-up state until its exact animation meaning is proven.

### State `54`

This handler subtracts `0.2 * maxHealth`, but it is now also known to belong to the special actor/scripted-control context selected by CPlayer Event `0x6D` mode `3`. It consumes the `Game+0xCEB20` actor context and runs motions `24B4..24B7`. Keep the safe interpretation as a **scripted special-actor damage/choreography state**, not a generic damage reaction. See `special_actor_choreography.md`.

---

## 9. Special-object state `7D`

Entry stores an external object handle in:

```text
CPlayer+0x874
```

plus auxiliary context fields, changes camera/input state and enters `7D`.

Dedicated cleanup:

```text
resolve Player+0x874
    ↓
send object event 0x1D
    ↓
restore Player/camera state
    ↓
state 00
    or, under a separate condition, state 82
    ↓
clear +0x874
```

At the Player CCT boundary `7D` is grouped with `03` and `87`, indicating special controller handling.

Do not assign a prettier object-specific name until its ingress caller is semantically identified.

---

## 10. Item-use states `7B/7C`

The item/effect dispatcher `FUN_0064FA30` modifies Player meters/inventory and, for two distinct command classes, enters `7B` or `7C` after checking state/capability conditions.

This is enough to classify them as special item-use/action presentation states, but not enough to call either one `food`, `medicine`, `smoking`, etc.

`7A` remains separate and unclassified.

---

## 11. CPlayer state → CCamera mode table

The table at `0x008A9980` is now positively identified. The Player transition routine indexes it with the committed gameplay state and passes the result to the CCamera mode setter. That setter writes `CCamera+0x154`; the normal camera dispatcher then indexes `0x008A9BC8[mode]`. These values are camera modes, not secondary Player-state names.

| State(s) | Camera mode | Strong anchor |
|---|---:|---|
| `00` | `0` | ordinary family |
| `01` | `2` | aim/combat family |
| `02` | `10` | context-target acquisition |
| `03` | `3` | unresolved state |
| `04..06` | `0` | |
| `07` | `3` | |
| `08` | `0` | |
| `09,0A` | `2` | aim/combat |
| `0B` | `0` | |
| `0C` | `2` | aim/combat |
| `0D` | `13` | paired structurally with 57 |
| `0E..13` | `0` | |
| even `14,16,...,28` | `2` | weapon/aim |
| odd `15,17,...,29` | `0` | |
| `2A,2B` | `2` | weapon/aim |
| `2C..3F` | `0` | |
| `40` | `6` | |
| `41..45` | `0` | |
| `46` | `11` | context-target handoff |
| `47..4B` | `0` | |
| `4C` | `12` | |
| `4D` | `15` | |
| `4E..56` | `0` | |
| `57` | `13` | Kaysen2-side action |
| `58..64` | `0` | |
| `65` | `16` | Telescope |
| `66..7C` | `0` | |
| `7D` | `4` | special external object |
| `7E` | `0` | |
| `7F` | `7` | damage/recovery family |
| `80,81` | `0` | |
| `82` | `7` | damage/recovery family |
| `83,84` | `0` | |
| `85` | `8` | fatal/death reaction |
| `86` | `0` | |
| `87,88` | `9` | vehicle camera |

Camera dispatch anchors:

```text
mode 0  -> 00538A50  ordinary/default camera family
mode 2  -> 0053B980  aim/combat
mode 3  -> 0053A1E0  states03/07 target-camera family
mode 4  -> null      state7D has no normal per-update camera handler
mode 6  -> 00539B00  state40 world-interaction target camera
mode 7  -> 00537A00  states7F/82 damage/recovery target camera
mode 8  -> 00537EE0  state85 fatal/death camera
mode 9  -> 00537FE0  vehicle (separately scheduled)
mode10  -> 0053C600  interaction/target acquisition
mode11  -> 0053C600  interaction/target handoff
mode12  -> 00538A40 -> 006102B0 scripted/global trajectory camera
mode13  -> 00538D20  state0D/57 + Kaysen2 special-actor override
mode15  -> 00537FB0  state4D conditional wrapper: mode0 or Darts/minigame camera 00606520
mode16  -> 0053CEA0  Telescope state65
```

Modes1,5,14 have no normal state-table ingress. Mode5 is a no-op; modes1/14 are implemented but dormant/legacy candidates. The special actor transition logic can force mode13 when the active discriminator is 6 (Kaysen2), independent of the static state-to-mode table.

See `../input/camera_modes_census.md` for the full 0..16 dispatch table, GOG/Steam direct-input call counts, response constants, and timing implications.

---

## 12. Remaining semantic islands

The high-value unknowns are now much narrower than the original 137-state problem.

### Still genuinely open

- exact gameplay identity of `03`;
- individual meanings of `04..08`;
- exact role of `0C`;
- `12` and `13`;
- per-action labels within weapon pairs `14..29`;
- concrete object identities/action meanings inside shared object-action choreography `2E..68`;
- exact semantics of interaction-adjacent `47..4A,52,53,58,59,64`;
- exact cause represented by damage states `7E,7F,81,83`;
- `7A`;
- exact special action behind `86`.

### No longer open at the family level

- State `0` is not an invalid/assert state.
- `09/0A` are no longer generic pre-weapon transitions: Xbox ingress proves combat strafe left/right, while PC retains the complete consumer state machine.
- `0B` is the original Xbox Quick Turn temporary heading state; Director's Cut preserves the consumer but altered its trigger conditions.
- `0E..2C` are not an unstructured animation blob; they are weapon/equipment states.
- `38/87/88` are positively vehicle-specific; wider `2E..68` object-action ownership needs producer/receiver evidence.
- `3C/3D/3E/3F/40/45/4B/4C/4D/4E` are a target-driven world interaction family.
- `7E..85` are a damage/reaction/fatal convergence family.
- `38` is not ordinary idle after vehicle exit.

