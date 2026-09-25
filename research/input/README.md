# Input, camera, and original-control research

**Source:** engine map v10 input/camera reconciliation, 2026-09-26.

## Native PC input pipeline

```text
Win32 keyboard/mouse + WinMM joystick
    -> FUN_00709BA0 physical acquisition (GOG)
    -> 0x6C logical action record per slot
    -> digital mask + movement/look/trigger channels
    -> FUN_007093B0 PC stick filtering
    -> FUN_00708AB0 one-slot staged snapshot
    -> FUN_00708300 commit + held/rising/repeat derivation
    -> public CInput getters
       FUN_007088C0 digital masks
       FUN_007089B0 trigger-like floats
       FUN_007089E0 movement/look floats
    -> Player / camera / UI / vehicle consumers
```

`USEJOY` selects controller versus keyboard/mouse evaluation throughout the engine.

## Controller binding contract

Vanilla Director's Cut expects a legacy WinMM layout:

```text
left stick     X / Y
right stick X  U
right stick Y  R
LT / RT style  opposite directions of shared Z
```

`configJ.cnf`/`configJex.cnf` feed the logical action -> binding enum used by
`FUN_006B1780`.

ZachFix Native XInput preserves that action layer by exposing a synthetic JOYINFOEX
layout and remapping legacy binding meanings:

```text
Left stick  -> X/Y
Right stick -> Z/R
LT          -> U high
RT          -> V high
```

This is why the Native XInput implementation must bridge into DP's evaluator rather
than simply replace `configJ.cnf` semantics.

## Input staging / latency

The main loop normally consumes the previously staged CInput snapshot and polls the
next physical sample later in the same game tick. This establishes approximately one
normal game tick of staging latency.

A ~33.333 ms background input callback also exists, but it shares the same one-slot
pending buffer. It does not prove a 30 Hz gameplay-input cap.

A latency patch remains research-only because an extra or reordered commit can alter
rising/repeat/previous-mask semantics.

## UI direction masks

Confirmed combined digital/analog direction composites:

```text
Up    0x4001
Down  0x8002
Left  0x10004
Right 0x20008
```

Friendly names for other low logical bits should be derived from concrete consumers,
not assigned globally by resemblance.

## Camera state -> mode architecture

The CPlayer state-to-camera table at `0x008A9980` selects `CCamera+0x154`. The normal
camera dispatcher uses `0x008A9BC8[mode]`.

Confirmed anchors:

```text
mode 2  aim/combat
mode 9  player vehicle camera; states 87/88
mode 10 context-target path; state 02
mode 11 context-target path; state 46
mode 16 Telescope; state 65
```

Important corrections:

- ordinary mode-0 free-look re-anchors the camera target before the common look pass;
  the local `lookX * 2 degrees` instruction does not by itself prove an FPS bug;
- vehicle mode 9 already has a signed 0.25 deadzone, 4/3 renormalization,
  data-driven sensitivity, and anchor-relative target semantics;
- modes 10/11 remain the strongest static incremental-camera timing candidate and
  require runtime A/B before any patch;
- the old suspected second 0.25 live-aim deadzone was a false read; the compared PC
  constant is zero.

## ZachFix Xbox aim shaping guard

The caller-scoped Xbox aim-shaping hook is valid only in controller mode. The same
CInput pair carries mouse look while `USEJOY == 0`, so a production hook must also
require controller mode or it can deform mouse aiming.

## Original Xbox controls retained by the PC state machine

### Combat Strafe

Static Xbox -> PC comparison recovers:

```text
Xbox LB edge, RB not held -> state 09
Xbox RB edge, LB not held -> state 0A
```

The recovered gate also uses capability `0x2000` and a Player flag corresponding to
`+0x638 & 0x10` on the PC layout.

GOG/Steam retain the `09/0A` consumers, mirrored motions `0x242C..0x2431`, and return
behavior, but the identified high-level PC Player update omits the homologous Xbox
ingress call.

### Quick Turn

Original Xbox behavior uses a stick-back condition plus the original Run/X action to
request a target yaw of current yaw + pi and temporarily enter state `0x0B`.

Director's Cut retains the full state-`0x0B` heading interpolation consumer, but its
input gating was changed. This is a strong restoration candidate, not a shipped
feature.

## Production status

Shipped today:

- Native XInput bridge;
- PC/Xbox360 stick and aim profiles;
- analog vehicle LT/RT restoration at the three confirmed binary consumers;
- vibration bridge;
- automatic keyboard/controller switching;
- dynamic glyph themes.

Research-only:

- one-tick input staging reduction;
- camera modes 10/11 timing patch;
- Combat Strafe `09/0A` restoration;
- Quick Turn `0x0B` trigger restoration.

## Remaining high-value tests

1. Read-only timestamps around CInput poll/commit/consumer stages.
2. Runtime A/B for camera states 02 and 46 / modes 10 and 11.
3. Runtime validation of states 09, 0A, and 0B before any control restoration.
4. Finish confirm/cancel/menu action semantics from concrete UI consumers.
