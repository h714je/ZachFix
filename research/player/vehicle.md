# Vehicle entry, active control, exit, and action packet

**Pass:** 2026-09-25. **Primary:** GOG raw PE. **Comparison:** Steam raw PE. Static findings; no new runtime test.

The vehicle-specific spine is `38 ↔ 87 → 88 → 38 → 00`, with conditional branches and helper calls described below. The wider `2E..68` region is an object-action domain, not a proven contiguous vehicle family. See [action_protocol.md](action-protocol.md).

## Address dictionary

| Role | GOG VA | Steam VA |
|---|---|---|
| Vehicle setup / store car in action packet | `004DC4A0` | `004DC3D0` |
| Entry commit | `004DDE80` | `004DDDB0` |
| Enable player-car mode after `M_CENTER` lookup | `004DCFA0` | `004DCED0` |
| State `38` | `004DE6A0` | `004DE5D0` |
| State `87` | `004DD230` | `004DD160` |
| State `88` | `004DD0B0` | `004DCFE0` |
| Exit completion / restore Player | `004DE2A0` | `004DE1D0` |
| Full gameplay transition hub | `00529010` | `00528F40` |
| Car native-event handler | `00544B80` | `00544AB0` |
| Car Event `35` packet-phase consumer | `005422A0` | `005421D0` |

The previous unqualified `004DCED0` name was a Steam address. It must not be labelled GOG.

## Vehicle Event `35` is bidirectional protocol traffic

The common action selector maps native Event `35` to candidate `38` behind capability `0x20` (`GOG 005093C8..005093DF`). Normal ingress copies the packet and may commit the selected gameplay state after its own checks.

On the car side, native-event dispatcher `00544B80` decodes Event `35` to `00544D6E`, then calls `005422A0` at `00544D7E` when `car+1FD4 == 0`. The consumer switches on `packet+3C`, subtracting `0x0F`. Its branch table is at `00542418`.

| Packet phase | Car-side effect at GOG | Interpretation allowed by code |
|---|---|---|
| `0F` | `005422BF`: play resource `29D3`, then common `11` effects | begin car-side entry animation |
| `10` | `0054236E`: play `29D4`, then common `12` cleanup; does not take the exact-`12` finalization | begin car-side exit animation |
| `11` | `0054230B`: set `car+434` bit `100`; call `5415F0`, `5480B0`; `car+3A=0`; helper argument float `1.0` | establish occupied/entry-side setup; exact helper semantics partly open |
| `12` | `005423BB`: clear bit `100`; call `540F30`, `548010`; then `car+3A=1` and helper float `1.0` | release car-side setup |
| `13` | `0054233A`: if animation data exists, copy its frame count into `car+194` | force animation to terminal frame |
| `14` | `005423DB`: same terminal-frame operation, then `car+3A=1` and helper float `1.0` | force exit-side completion |
| `15` | `00542401`: `car+3A=1`; helper float `1.0` | finalization without replaying animation |

`5480B0` allocates/initializes a `0x1C20` block at `car+19AC`; `548010` releases it. `540F30` resolves four handles at `car+1FE4` and invokes virtual `+30` for the live objects. Exact names for those auxiliary objects are not assigned here.

The `0x100` bit controlled here is **not** the live player-car scheduler bit `0x8000`.

## State `38`: animation-driven entry/exit hub

The handler obtains the packet's car pointer, anchors the camera at `PM_SEAT_ML`, and branches on the active Player motion resource. All completion branches also check animation flags at `Player+1FC`.

| Player motion / condition | Observed path |
|---|---|
| `258B` or `258D` completes | set a Player action flag, select intermediate motion `2471` (`004DEDC7..004DEE5E`) |
| `2471` completes with tested entry flag set | clear that flag and tail-jump to entry commit `004DDE80` (`004DE93F..004DE9A9`) |
| `2471` completes without that flag | choose `258C` for `car+424==0`, or `258E` for `car+424==1`; write packet phase `10`; send car Event `35` (`004DE9AE..004DEA5C`) |
| `258C` or `258E` is active | detach Player if attached, update car flags; wait for animation completion (`004DEC65..004DECF8`) |
| `258C` or `258E` completes | write phase `15`, send car Event `35`, invoke event-manager helper and Player exit cleanup (`004DECFE..004DED8D`) |
| Other motion, relevant action gate clear | reset car movement fields and choose vehicle setup helpers; this is another branch of the same hub (`004DEB25..004DEC5A`) |

The two motion pairs correlate with `car+424` values `0/1`. This pass does **not** label them left/right doors; model/archetype or another variant remains possible.

Entry setup `004DC4A0` selects `258B/258D` using that same car discriminator. Thus the entry/exit interpretation is grounded in both the producer and consumer, while friendly resource names remain unverified.

## Entry commit and control authority

GOG `004DDE80`:

1. requests gameplay state `87` at `004DDF2B..004DDF3E`;
2. applies Player/camera flags and movement setup;
3. attaches Player to the car's `PM_SEAT_ML` marker (`004DE116..004DE124`);
4. sends car Event `35` with phase `11`, then again with phase `13` (`004DE148..004DE1E4`);
5. calls `004DCFA0` at `004DE224`; successful `M_CENTER` lookup sets `car+434 |= 8000` and `|= 08000000` (`004DCFFF`, `004DD010`).

The already established live scheduled branches remain high-level `005588F0`, wheels `005578A0`, and PhysX `00555C20` (GOG). Their ownership discriminator is bit `8000`, not the older `20000 → 0054C090` branch.

### Active state `87` steering producer

Raw-PE review closes the live steering producer for the `0x8000` player-car branch. Ghidra did not recover GOG `004DD230` as a normal function boundary, so the decisive writer is visible only in raw disassembly.

State `87` computes a bounded steering target and approaches it using the central `gameDelta60` scalar before writing `car+0x4E0`:

```text
steer target bound ~= +/-0.62831855 rad (+/-36 deg)
max step           = gameDelta60 * 0.045378558 rad
                   = ~2.6 deg at gameDelta60 1.0
                   = ~156 deg/s wall-time slew
```

Writer ranges:

| Build | State 87 | `car+0x4E0` read/slew/write |
|---|---:|---:|
| GOG | `004DD230` | `004DDCB0..004DDD54` |
| Steam | `004DD160` | `004DDBE0..004DDC84` |

This overturns the old unqualified statement that `FUN_00551640` is the current player steering producer. `FUN_00551640` is selected by the alternate `car+434 & 0x20000` branch. The live `0x8000` steering application remains inside `FUN_00555C20`, which reads `car+0x4E0` and applies the resulting angle to front wheel slots `2..3`.

The production implication is narrow: do not gate or rescale the state-87 steering producer merely because it runs at render cadence; its slew is already delta-aware. Any Xbox-like vehicle cadence experiment should instead preserve input/target production and reconcile the actual Event-5 wheel/drive application plus Event-6 post-physics readback/correction phases.

## Exit request, prelude, choreography, cleanup

In state `87`, the guarded exit request at `004DD8A0..004DD954` checks input and car conditions, checks Player capability `0x40`, zeroes local phase counter `0139277C`, and requests state `88`.

State `88` has four local phases, distinct from packet phase codes:

| Local phase | GOG behavior |
|---|---|
| `0` | start presentation/transition helper; increment counter |
| `1` | wait for helper completion; request an event/task; select phase `2` if a handle exists, otherwise `3` |
| `2` | wait until the task lookup no longer resolves; select `3` |
| `3` | copy car yaw to `Player+6C/+7C`, clear bit `2` at **car+DC**, request gameplay state `38`, restart presentation helper |

The final phase is `004DD150..004DD217`. It does **not** contain a clear of `car+434 & 8000` and does **not** finish the entire dismount. Calling it “completed exit → idle” was too strong.

State `38` then handles the exit animation branch above. Its completion helper `004DE2A0` computes/restores Player position, calls Player controller-position helper `004E31E0`, sets `Player+638` bit `1`, and conditionally requests state `00` when `0042D090()` is nonzero (`004DE485..004DE4C4`). It later clears Player `D8` bit `80000000` and sends car Event `35` with packet phase `12` (`004DE513..004DE59A`). Further orientation/flag cleanup follows.

This is the supported normal structure; script calls can enter these helpers directly, and the transition to `00` in the cleanup helper has an explicit gate.

## Scheduler-bit reconfiguration outside the normal dismount spine

A separate car mode/configuration dispatcher, GOG `00550390`, shows that the live-player scheduler bit `car+434 & 0x8000` **can** be removed, but the observed clears are mode-reconfiguration paths rather than a direct instruction in the normal `87 -> 88 -> 38` exit sequence.

When `car+434` carries the reconfiguration trigger `0x08000000`, the dispatcher clears a group of mode bits and then selects by the remaining ownership/mode bits:

- existing `0x10000` -> tail-jump `0055F220`; that initializer performs `AND flags, 0xFF7F7FFF` (clears `0x8000` and `0x800000`) and sets `0x10000`;
- existing `0x20000` -> tail-jump `005548D0`; it performs the same `AND flags, 0xFF7F7FFF` and sets `0x20000`;
- neither -> tail-jump `005502C0`; that path initializes the player-car mode and sets `0x8000`.

This establishes a **real clear/transfer mechanism for bit `0x8000`**, but does not yet establish when ordinary post-exit choreography requests this reclassification. Therefore normal-exit ownership transfer remains open. Do not reinterpret these branches as direct evidence that state `88` or the state-`38` cleanup clears `0x8000`.


## Late continuation: what `0x8000` now appears to mean

Further static work separates **car scheduler/ownership mode** from the instantaneous Player gameplay state more strongly than before.

1. Inside the established `0x8000` player-car scheduler branch (`005578A0` family), the code separately tests `CPlayer+0x654 == 0x87` and `== 0x88`. Therefore `car+434 & 0x8000` cannot simply mean “York is currently in gameplay state 87 / physically seated”. It is a broader car-side scheduler/ownership classification.
2. Car Event `0x35` phase `12` does **not** flow into `FUN_00550390`; after the packet consumer it returns through the common event epilogue. This disproves the tempting chain `phase12 -> mode dispatcher -> clear 0x8000`.
3. A real ownership-transfer helper, GOG `004893F0`, clears `0x8000`, selects `0x10000` or `0x20000` (depending on car-side state), calls `0054E550`, and sets reclassification bit `0x08000000`. Its recovered callers belong to NPC/AI vehicle ownership contours (`CNpcNormal` and shared `CNpc`/`CNpcBird`/`CNpcEnemy`/`CNpcMob` paths), not to the proven York `87 -> 88 -> 38` dismount spine.

Current best model:

```text
car+434 & 0x8000
    = live player-car scheduler / ownership classification
    != direct synonym for Player state 87

Player state 87 / 88
    = instantaneous York gameplay choreography inside that broader car mode
```

This means an immediate `0x8000` clear on ordinary dismount is no longer an assumption. The next question is whether the same car remains designated as the player-car after York exits, and only loses `0x8000` on ownership transfer/despawn/reclassification.

## Remaining limits

- Exact lifetime/transfer policy of car mode bit `8000` remains open. Immediate clear on ordinary dismount is no longer assumed. Do not confuse `car+DC bit 2`, `car+434 bit 100`, and `car+434 bit 8000`.
- The identity of `car+424` values `0/1` and friendly motion resource names remain open.
- Full producer/receiver classification of the other object-action states remains separate work.
- No runtime timing, state reachability, or visible animation sequence was newly tested here.

## Evidence

See the GOG/Steam `vehicle_entry`, `vehicle_hub38`, `vehicle_exit_prelude`, `vehicle_cleanup`, `active_car_exit_request`, and `car_packet_consumer` assembly extracts under [evidence/vehicle_protocol](../evidence/vehicle_protocol/README.md). They preserve instruction addresses, making the claims independently reviewable against the binaries.
