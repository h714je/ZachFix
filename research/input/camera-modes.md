# Camera input architecture and timing

**Status:** cross-build PC architecture confirmed at the state-to-camera dispatch layer. Full mode census is now available. The earlier ordinary-free-look FPS inference from the local `2 degrees` instruction has been withdrawn after reconstructing the per-update re-anchor order. Modes 10/11 remain the main static camera-timing candidate.

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

Mode `2` is selected by state `01`, `09/0A`, `0C`, and almost every even weapon-action state `14..2B`. Its dispatch target `0053B980` contains the live aim CInput consumers already used by ZachFix.

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

## 3. Current ZachFix aim-shaping guard bug

ZachFix `HookStickFloatGetter` applies the exact Xbox aim curve to the four mode-2 aim callers when `GamepadInputProfile=Xbox360`:

```text
abs(axis) <= 0.25 -> 0
otherwise subtract signed 0.25 and multiply by 4/3
```

However, CInput pair 1 is also the public mouse-look pair while `USEJOY == 0`. The hook previously checked profile/caller but not the game's active input mode. Therefore mouse deltas in the same aim camera could be clipped and renormalized by the controller curve after AutoSwitch returned to keyboard/mouse.

Status:

```text
Xbox aim shaping can affect mouse look in mode 2
    CONFIRMED source/static integration bug
```

Minimal source fix: additionally require `TryGetVanillaInputMode(controllerMode) && controllerMode` before applying `ApplyXboxCameraDeadzone`.

See `evidence/input/native_xinput_aim_mouse_guard.patch`.

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
    fix current mouse contamination guard only

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
