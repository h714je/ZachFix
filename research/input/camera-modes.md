# Camera input architecture and timing

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Camera families and mode-2 precision need their own evidence and runtime validation.

<details><summary><strong>On this page</strong> · 8 sections</summary>

- [1. CPlayer state selects CCamera mode](#1-cplayer-state-selects-ccamera-mode)
- [2. Aim/combat camera: mode 2](#2-aimcombat-camera-mode-2)
- [3. ZachFix aim-shaping controller-mode guard](#3-zachfix-aim-shaping-controller-mode-guard)
- [4. Ordinary walking/free-look PC path](#4-ordinary-walkingfree-look-pc-path)
- [5. Vehicle camera: mode 9](#5-vehicle-camera-mode-9)
- [6. Interaction/target camera: modes 10/11](#6-interactiontarget-camera-modes-1011)
- [7. Patch-scope rules](#7-patch-scope-rules)
- [8. Full mode census](#8-full-mode-census)

</details>
<!-- END AUTO RESEARCH NAV -->

**Status:** cross-build PC architecture confirmed through state-to-camera dispatch and the live mode-2 aim handoff. Mode 2 now has a confirmed x87 precision-sensitive edge-follow mechanism and a separate PC/Xbox repeated-entry reset-policy divergence. The scoped PC24 guard corrects the affected behavior, but the normal-session trigger/root cause remains unproven. The earlier ordinary-free-look FPS inference from the local `2 degrees` instruction remains withdrawn. Modes 10/11 remain the main unrelated static camera-timing candidate.

## 1. CPlayer state selects CCamera mode

The table at `0x008A9980` is not a vague secondary Player mode. CPlayer transition code indexes it with the committed gameplay state and passes the resulting value to the CCamera mode setter.

GOG transition anchors:

```text
005295C5 / 005295E3  read [0x008A9980 + state*4]
005296xx             call CCamera mode setter
```

Steam decompiler shows the same structure: the selected table value is passed to `FUN_00534C60`, which copies `CCamera+0x154` to `+0x158` and stores the new mode in `+0x154`.

The normal camera dispatcher reads `CCamera+0x154` and dispatches through `0x008A9BC8[mode]`. Mode `9` is deliberately skipped by this generic dispatcher and is called from the separate vehicle-camera schedule.

### State -> camera mode

```text
mode 0:
00 04 05 06 08 0B 0E 0F 10 11 12 13 15 17 19 1B 1D 1F 21 23 25 27 29 2C 2D
2E 2F 30 31 32 33 34 35 36 37 38 39 3A 3B 3C 3D 3E 3F 41 42 43 44 45 47 48 49
4A 4B 4E 4F 50 51 52 53 54 55 56 58 59 5A 5B 5C 5D 5E 5F 60 61 62 63 64 66 67 68
69 6A 6B 6C 6D 6E 6F 70 71 72 73 74 75 76 77 78 79 7A 7B 7C 7E 80 81 83 84 86

mode 2:  01 09 0A 0C 14 16 18 1A 1C 1E 20 22 24 26 28 2A 2B
mode 3:  03 07
mode 4:  7D
mode 6:  40
mode 7:  7F 82
mode 8:  85
mode 9:  87 88
mode 10: 02
mode 11: 46
mode 12: 4C
mode 13: 0D 57
mode 15: 4D
mode 16: 65
```

No gameplay state in the extracted table selects modes `1`, `5`, or `14` directly.

### Camera-mode dispatch table

```text
mode  0 -> 00538A50
mode  1 -> 0053B0B0
mode  2 -> 0053B980
mode  3 -> 0053A1E0
mode  4 -> null
mode  5 -> 00539AF0
mode  6 -> 00539B00
mode  7 -> 00537A00
mode  8 -> 00537EE0
mode  9 -> 00537FE0
mode 10 -> 0053C600
mode 11 -> 0053C600
mode 12 -> 00538A40
mode 13 -> 00538D20
mode 14 -> 00539580
mode 15 -> 00537FB0
mode 16 -> 0053CEA0
```

Useful semantic anchors:

```text
mode 2     aim/combat camera                         CONFIRMED family
mode 9     player vehicle camera, states 87/88      CONFIRMED
mode 10/11 context-target acquisition/handoff       STRONGLY_SUPPORTED
mode 16    Telescope state65 camera                  CONFIRMED ownership
```

## 2. Aim/combat camera: mode 2

Mode `2` is selected by state `01`, `09/0A`, `0C`, and almost every even weapon-action state `14..2B`. GOG dispatches to `0053B980`; Steam's homolog is `FUN_0053B8B0`. This is the live aim handler used by the ZachFix camera research.

Proven GOG return/caller RVAs for the final stick getter:

```text
0013C0E0
0013C113
0013C338
0013C36B
```

Steam:

```text
0013C010
0013C043
0013C268
0013C29B
```

These cover horizontal and vertical aim look.

The PC aim path contains `gameDelta60` in downstream smoothing/integration. The earlier ordinary-mode0 `input * gameDelta60` proposal has itself been withdrawn after reconstructing the re-anchor order; mode2 is further evidence that camera timing must remain mode-specific rather than getter-global.

The PC comparison around the live getter uses a zero constant at `0x00872B00`; it is not a second `0.25` deadzone. The theory that ZachFix currently stacks a second PC `0.25` deadzone on top of Xbox aim shaping is DISPROVEN.

### 2.1 Mode-2 target -> camera handoff

The live PC handler has a two-stage model. Input first moves bounded aim/reticle targets; when a target reaches its edge, motion is handed to the actual camera orientation. Steam fields:

```text
CCamera+0x6C = live pitch/orientation
CCamera+0x70 = live yaw/heading
CCamera+0x8C = smoothed pitch offset
CCamera+0x90 = smoothed yaw offset
CCamera+0x9C = pitch target accumulator
CCamera+0xA0 = yaw target accumulator
CCamera+0x120 bit0 = mode-2 persistence/follow policy input
```

Horizontal handoff uses a limit derived from `weaponAimParams[5] * 3.2f * DEG_TO_RAD`; vertical uses `weaponAimParams[5] * DEG_TO_RAD`. The target is clamped, then the code tests exact equality against the edge before updating `+0x70` / `+0x6C`.

On x87 the edge limit can remain in extended precision while the clamped target has been spilled to float32. With PC=53 the retained limit can differ by a representational sliver from the float32 target, making the exact equality false. Local A/B established:

```text
Native ambient PC24 -> normal
Force53             -> restricted reticle-edge / no camera-follow phenocopy
Force24             -> normal
```

This is a confirmed causal precision hazard inside mode 2. It is **not yet proof** that the spontaneous failure is caused by an ambient PC=53 transition: on the local machine Alt+Tab did not change the observed ambient x87 control word. A narrowly scoped PC24 guard around the native mode-2 handler corrects the affected behavior, but remains experimental because the normal-session trigger/root cause is still unidentified.

### 2.2 Fresh mode-2 initialization: PC vs Xbox

The Player state setter maps state -> camera mode and calls the camera setter explicitly. Xbox performs fresh mode-2 setup on every explicit request. PC added previous-mode suppression. Two distinct PC `previousCameraMode != 2` gates matter:

```text
Gate A: fresh orientation / anchor synchronization
Gate B: clear +0x74/+0x78 and +0x8C..+0xA8 transient state
```

Therefore:

```text
Xbox explicit SetState(mode2) -> SetCameraMode(2) -> fresh mode-2 setup
PC   explicit SetState(mode2) -> SetCameraMode(2) -> setup skipped when previous mode == 2
```

The surrounding quartet writers/resetters otherwise match Xbox closely, including the target-lock reset, alternate small-window aim producer, 0x2000 decay/neutralization path, and broad restore resets. The setter suppression remains the principal confirmed platform-specific policy difference in this subsystem.

Normal PC weapon ingress is staged through states `0E/0F`, both camera mode 0, before entering primary weapon mode-2 states. Ordinary weapon handlers also exit to state 0/non-mode2 paths rather than reissuing a primary mode-2 state every held frame. So the common shipped weapon path does not by itself exercise the repeated-mode2 hazard.

Preserved states `09/0A` are different: original Xbox combat strafe uses mode 2 and returns through `0E`; PC's camera setter deliberately preserves mode 2 across the `09/0A -> 0E` boundary, after which primary weapon selection requests mode 2 again. This is a real programmed repeated-mode2 sequence. Director's Cut removed the original stock ingress, so it matters today only when ZachFix restores Combat Strafe.

### 2.3 Alt+Tab repair path

The PC input producer gates active controller state on `GetForegroundWindow() == GetActiveWindow()`. Losing focus neutralizes the logical controller record. Common state-01 and firearm aim handlers interpret held AIM as released and transition to Player state 0 / camera mode 0. After focus returns, re-aim performs a genuine mode0 -> mode2 entry, so both PC fresh-init gates run.

This explains why Alt+Tab can repair a stale/bad mode-2 session without requiring a D3D device-reset theory. It is independent of the x87 precision phenocopy.

### 2.4 CEvent A6/4 branch closed as shipped-gameplay cause

`A6/4` calls the weapon-class selector and can explicitly request a primary mode-2 Player state without a current-state guard on both PC and Xbox. This is useful architectural proof that explicit same-mode state requests are valid engine semantics.

However, a full structural census of shipped PC event content closed it as the historical gameplay producer:

```text
656 .DSB files
61,498 decoded CEvent commands
opcode A6: 55 occurrences in 17 files
A6/subcommand 4: exactly 2 occurrences
location: UPDATA/EVENT/91/0_0455.DSB
routine: 銃撃テスト ("Shooting Test")
```

`0_0455.DSB` is an internal developer-test collection. No ordinary story/COMMON/SUBEVENT script uses A6/4. The earlier assumption that `CPL01.XFE` carried CEvent bytecode was also disproven: the inspected file is an `XAM2` facial-expression resource; actual CEvent scripts are `.DSB` under `UPDATA/EVENT`.

See [`../evidence/aim_mode2_precision/README.md`](../evidence/aim_mode2_precision/README.md).

## 3. ZachFix aim-shaping controller-mode guard

ZachFix `HookStickFloatGetter` applies the exact Xbox aim curve to the four mode-2 aim callers when `GamepadInputProfile=Xbox360`:

```text
abs(axis) <= 0.25 -> 0
otherwise subtract signed 0.25 and multiply by 4/3
```

CInput pair 1 is also the public mouse-look pair while `USEJOY == 0`. The pre-fix hook checked profile/caller but not the game's active input mode, so mouse deltas in the same aim camera could be clipped and renormalized by the controller curve after AutoSwitch returned to keyboard/mouse.

Status:

```text
pre-fix Xbox aim shaping could affect mouse look in mode 2
    CONFIRMED source/static integration bug

production fix
    require TryGetVanillaInputMode(controllerMode) && controllerMode
    before applying ApplyXboxCameraDeadzone
```

The controller-mode guard is now part of the production hook.

## 4. Ordinary walking/free-look PC path

The common PC camera helper `FUN_00537730` reads CInput look pair 1 X through `FUN_007089E0` and contains a direct `lookX * 2 degrees * DEG2RAD` target modification. Read in isolation, this previously looked like a fixed-angular-step-per-update bug.

The full camera schedule changes that interpretation. In two independent game-side callers the order is:

```text
FUN_005355A0
    -> mode-specific handler

then
FUN_00537340
    -> common camera post/update path
    -> possible FUN_00537730 free-look branch
```

For ordinary mode 0, `FUN_00538A50` reconstructs the camera yaw target from Player/anchor state before the common free-look pass. The later `lookX * 2 degrees` operation therefore modifies a freshly established frame-local target rather than necessarily accumulating an angular velocity from the previous update.

Mode 0 also directly consumes pair-1 Y with its own target-style pitch logic (0.15 threshold and anchor-based shaping).

Current correction:

```text
ordinary mode0 horizontal free-look scales with FPS solely because of the local 2-degree instruction
    DISPROVEN / insufficient inference

PC and Xbox camera response models differ
    CONFIRMED structural divergence
```

The Xbox walking camera still uses a signed-0.25-deadzone, anchor-relative target with a 120-degree sensitivity. That difference may be interesting for an optional Xbox-restoration mode, but it is no longer evidence that a simple `gameDelta60` multiplier belongs in the PC path.

For mouse input, pair 1 contains cursor displacement over the sampling interval. A global timing multiplier remains incorrect.

See `input/camera_modes_census.md` for the full per-mode reconstruction.

## 5. Vehicle camera: mode 9

Player states `87/88` select mode `9`, whose handler is `FUN_00537FE0`. The generic dispatcher skips mode 9; vehicle scheduling calls it separately.

The vehicle camera already performs an Xbox-like controller response:

```text
abs(stick) > 0.25
    -> subtract signed 0.25
    -> multiply surviving range by 4/3
    -> multiply by data-driven sensitivity and DEG2RAD
```

Horizontal input modifies a yaw target anchored to the current car/camera configuration rather than accumulating a fixed angle indefinitely. Vertical input is similarly target-oriented, and smoothing uses time-sensitive camera state.

Conclusion:

```text
vehicle camera lack of dt in immediate stick shaping
    NOT sufficient evidence of a timing bug
```

Do not apply a global right-stick timing hook to mode 9.

## 6. Interaction/target camera: modes 10/11

Modes `10` and `11` both dispatch to `FUN_0053C600`.

State ownership:

```text
state 02 -> mode 10
state 46 -> mode 11
```

State `02` is the context-target acquisition/pre-state; it scans for a target and can transition to `46`. This gives modes 10/11 a strong interaction/target-acquisition family label.

`FUN_0053C600` reads both look pair 1 and movement pair 0. After a `0.25` threshold it directly adds/subtracts approximately `input * 1 degree` to `CCamera+0x70/+0x6C` per invocation, guarded by the relevant mode/context state. No delta multiplier is present in these direct updates.

Status:

```text
modes10/11 contain incremental X/Y camera input against existing camera values
    CONFIRMED static math

visible FPS dependence
    STRONGLY_SUSPECTED; runtime A/B still required
```

Unlike modes 3/6/7/13/14, no equivalent per-invocation anchor reconstruction is visible before these direct updates. This makes 10/11 the highest-value camera timing test, but still not a justification for a global input hook.

Because this is a special interaction family and it consumes pair 0 as well as pair 1, do not fold it into the ordinary free-look patch without a dedicated test.

## 7. Patch-scope rules

The current evidence argues against a single global camera-input hook.

```text
mode 2 aim:
    Xbox controller shaping already restored by caller-scoped hook
    downstream delta-aware math exists
    exact edge handoff is x87-precision-sensitive
    keep PC24 mitigation scoped to the native mode-2 handler as an experimental best-effort workaround
    keep repeated-mode2 reset-policy research separate from the precision mitigation

ordinary mode0 free-look:
    per-update anchor reconstruction invalidates the old simple FPS-scaling inference
    Xbox response restoration remains a separate optional design question

mode 9 vehicle:
    anchor-relative 0.25 + 4/3 response already present
    no global timing patch

modes 10/11 interaction:
    separate incremental X/Y family without the re-anchor seen in target modes
    dedicated runtime timing test before patching
```

This mode-aware separation is the safe basis for future Controller Camera Restoration work.


## 8. Full mode census

See `input/camera_modes_census.md` for modes 0..16, GOG/Steam handlers, direct CInput call counts, response constants, dormant modes, and the special-actor override into mode 13.
