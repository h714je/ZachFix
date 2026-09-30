# Gamepad, input, and glyphs

## Native Gamepad

```ini
[Gamepad]
NativeGamepad = false
Backend = Auto
```

`NativeGamepad` enables ZachFix's provider-neutral controller path. The selected backend
produces a canonical `GamepadState`, ZachFix evaluates DP's existing `configJ.cnf`
binding IDs and native controller action helpers, and the resulting `0x6C` logical
controller record continues through the game's normal CInput pipeline.

The production backends are:

- `Auto` - recommended; try SDL3 first, then fall back to XInput;
- `SDL3` - use the SDL3 provider only;
- `XInput` - use the XInput provider only.

SDL3 is embedded in ZachFix. An optional `ZachFix\SDL3.dll` can override the embedded
runtime through SDL Dynamic API without replacing the ZachFix ASI.

Both `NativeGamepad` and `Backend` are startup choices and require a restart. When Native
Gamepad is disabled, Deadly Premonition keeps its vanilla WinMM controller path. The old
`NativeXInput` INI key is accepted as a read-only migration alias; saving settings writes
`NativeGamepad` and removes the old key.

## Input profile

```ini
InputProfile = Xbox360
```

Available profiles:

- `PC` - Director's Cut stick evaluator/filtering and aim shaping;
- `Xbox360` - restores the proven Xbox 360 stick normalization and aim shaping while
  keeping Director's Cut control routing, including right-stick aiming.

The profile is hot-applicable from F10 while Native Gamepad is active. The Xbox 360
profile also restores the original digital LT/RT press threshold used by DP's action
bindings.

## Analog vehicle triggers

```ini
AnalogVehicleTriggers = true
VehicleTriggerDeadzone = 30
```

This option is independent from the stick/aim profile. On the Native Gamepad path, the
three confirmed vehicle LT/RT consumers receive continuous trigger values instead of the
vanilla PC digital throttle/brake state.

`VehicleTriggerDeadzone` is a raw 8-bit trigger threshold, `0..254`. The original Xbox
360 value is `30`; values at or below the threshold become zero, while values above it
keep their raw `/255` scaling without renormalization.

Both controls are live from F10.

## Vibration

```ini
Vibration = true
VibrationStrength = 1.0
```

ZachFix reconnects the PC executable's surviving native two-channel rumble behavior to
the active Native Gamepad provider. DP still owns event timing, duration/countdown,
motor balance, and stop behavior; `VibrationStrength` is a final `0.0..1.0` gain.

Both settings are live from F10. Disabling vibration or setting strength to zero stops
the motors immediately. Gameplay vibration also follows DP's current input mode, so
switching to keyboard/mouse stops active controller rumble and switching back lets the
current native actuator state drive the provider again.

F10 includes a short direct vibration test when the active provider supports rumble.

## Automatic input switching

```ini
[Input]
AutoSwitch = false
```

When enabled, ZachFix switches DP's own keyboard/mouse versus controller mode based on
recent device activity. With Native Gamepad active, controller activity comes from the
selected SDL3/XInput provider; otherwise the legacy WinMM fallback remains available.

`AutoSwitch` is a startup setting and is persisted by the normal F10 save path.

## Low-latency input

```ini
[Input]
LowLatencyInput = false
```

The PC port normally commits the previously queued CInput snapshot before polling the
next physical sample. This adds one full input-staging tick before the newly sampled
state reaches gameplay.

When `LowLatencyInput` is enabled, ZachFix reversibly changes only those two verified
native main-tick calls to `poll -> commit`, allowing the freshly sampled snapshot to be
committed in the same tick. The game's native action state, edge/repeat derivation,
controller evaluation, bindings, and gameplay consumers remain in control.

The switch is immediate in **F10 -> Gamepad**. Disabling it restores the exact vanilla
`commit -> poll` ordering. Unsupported or signature-mismatched executables fail closed
and are left untouched.

Runtime validation confirmed that normal gameplay uses the main-thread producer path
with no concurrent background CInput producer, so the steady-state same-frame path
remains `poll fresh -> enqueue -> commit fresh`.

## Experimental aim FPU workaround

```ini
[Experimental]
AimFpuPrecisionFix = false
```

This is an experimental best-effort workaround for the reported mode-2 aiming edge
lock, which cannot be reproduced locally. Forced x87 PC53 precision is the only mechanism
found that produces a closely matching restricted edge-follow failure, and PC24 removes
that forced failure locally.

When enabled, the workaround forces PC24 only while DP's native mode-2 aim handler
executes and restores the caller precision-control bits afterward. It is disabled by
default, build/signature gated, and can be toggled live from **F10 -> Gamepad** with
**Apply**. Turning it off disables the hook and returns the native handler path. This
remains an attempt to cover the only reproducible look-alike found, not proof of the
original bug's root cause. See
[`../research/evidence/aim_mode2_precision/README.md`](../research/evidence/aim_mode2_precision/README.md).

## Native input architecture and restoration scope

Native Gamepad replaces only controller acquisition/evaluation at the verified action
boundary. DP still owns CInput aggregation and staging, held/rising/repeat state,
Player/camera/UI routing, and `configJ.cnf` semantics.

This distinction matters for restoration work. The original Xbox 360 executable contains
Combat Strafe and Quick Turn ingress behavior whose PC state consumers survive in
Director's Cut. Combat Strafe is restored behind `Gamepad.RestoreCombatStrafe`; it is
limited to Native Gamepad plus the Xbox360 profile and remains disabled by default.
Quick Turn remains research-only.

For the underlying CInput/camera map and those restoration findings, see
[`../research/input/README.md`](../research/input/README.md).

## Controller rebinding

Deadly Premonition stores controller action bindings in `configJ.cnf` beside `DP.exe`.
ZachFix intentionally continues using that file instead of adding a second binding
database.

Close the game before editing `configJ.cnf`. DPLauncher can rewrite the file when saving
controller settings.

With Native Gamepad enabled, supported binding values include:

| Value | Canonical control |
| ---: | --- |
| `0` | A |
| `1` | B |
| `2` | X |
| `3` | Y |
| `4` | LB |
| `5` | RB |
| `6` | Back / View |
| `7` | Start / Menu |
| `8` | Left Stick Click |
| `9` | Right Stick Click |
| `41` | D-pad Up |
| `42` | D-pad Down |
| `43` | D-pad Left |
| `44` | D-pad Right |
| `45` | Left Stick Left |
| `46` | Left Stick Right |
| `47` | Left Stick Up |
| `48` | Left Stick Down |
| `49` | RT |
| `50` | LT |
| `51` | Right Stick Left |
| `52` | Right Stick Right |
| `55` | Right Stick Up |
| `56` | Right Stick Down |

Values `49` and `50` deliberately retain DP's original RT/LT action meanings.

When Native Gamepad is disabled, ZachFix does not reinterpret the binding file.

## Dynamic glyph themes

```ini
[Glyphs]
DynamicAtlas = false
HotReload = true
KeyboardSet = Native
GamepadSet = Native
```

Theme files live under:

```text
ZachFix\glyphs\keyboard\<theme>.dds|png|tga
ZachFix\glyphs\gamepad\<theme>.dds|png|tga
```

The load priority is DDS, PNG, then TGA.

`Native` keeps the original atlas captured from the game. Bundled gamepad themes include
`xbox`, `playstation`, `steamdeck`, and `switch`.

Enabling or disabling `DynamicAtlas` requires a restart because it changes glyph texture
ownership. Once the binder is active, theme selection and file hot reload are live.

See `ZachFix/glyphs/README.md` and `GAMEPAD_ATLAS.md` for custom-theme authoring
information.
