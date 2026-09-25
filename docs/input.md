# Gamepad, input, and glyphs

## Native XInput

```ini
[Gamepad]
NativeXInput = false
```

When enabled, ZachFix reads XInput directly and presents a compatibility controller state to Deadly Premonition while preserving the game's existing action/binding system.

When disabled, the vanilla WinMM controller path remains in control.

`NativeXInput` is a startup/backend choice and requires a restart to change the active controller backend.

## Input profile

```ini
InputProfile = Xbox360
```

Available profiles:

- `PC` - Director's Cut stick evaluator/filtering and aim shaping;
- `Xbox360` - restores the proven Xbox 360 stick normalization and aim shaping while keeping Director's Cut control routing, including right-stick aiming.

The profile is hot-applicable from F10 while Native XInput is active.

The Xbox 360 profile also restores the original digital LT/RT press threshold used by DP's action bindings.

## Analog vehicle triggers

```ini
AnalogVehicleTriggers = true
VehicleTriggerDeadzone = 30
```

This option is independent from the stick/aim profile.

When enabled, the confirmed vehicle LT/RT consumers receive continuous XInput trigger values instead of the vanilla PC digital throttle/brake state.

`VehicleTriggerDeadzone` is in raw XInput counts, `0..254`. The original Xbox 360 value is `30`.

Both controls are live from F10.

## Vibration

```ini
Vibration = true
VibrationStrength = 1.0
```

ZachFix reconnects the PC executable's surviving native two-channel rumble behavior to `XInputSetState`.

The game still owns its event timing, duration/countdown, motor balance, and stop behavior. `VibrationStrength` is a final `0.0..1.0` gain.

Both settings are live from F10. Disabling vibration or setting strength to zero stops the motors immediately.

F10 also includes a short direct vibration test pulse when the native XInput path is available.

## Automatic input switching

```ini
[Input]
AutoSwitch = false
```

When enabled, ZachFix switches DP's own keyboard/mouse vs controller mode based on recent device activity.

This is independent of Native XInput. With both `NativeXInput = false` and `AutoSwitch = false`, ZachFix leaves the vanilla input path alone.

`AutoSwitch` is a startup setting.

## Native input architecture and scope

Native XInput intentionally feeds Deadly Premonition's existing logical action/binding layer instead of replacing it. The game still owns action state, edge/repeat derivation, Player/camera/UI routing, and `configJ.cnf` semantics.

This distinction matters for restoration work. The original Xbox 360 executable contains Combat Strafe and Quick Turn ingress behavior whose PC state consumers survive in Director's Cut, but those controls are **not** enabled by the current production build. They remain runtime-validation candidates in the research archive rather than being silently folded into the Xbox360 input profile.

For the underlying CInput/camera map and those restoration candidates, see [`../research/input/README.md`](../research/input/README.md).

## Controller rebinding

Deadly Premonition stores controller action bindings in `configJ.cnf` beside `DP.exe`. ZachFix intentionally continues using that file instead of adding a second binding database.

Close the game before editing `configJ.cnf`. DPLauncher can rewrite the file when saving controller settings.

With Native XInput enabled, supported binding values include:

| Value | XInput control |
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

When Native XInput is disabled, ZachFix does not reinterpret the binding file.

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

`Native` keeps the original atlas captured from the game. Bundled gamepad themes include `xbox`, `playstation`, `steamdeck`, and `switch`.

Enabling or disabling `DynamicAtlas` requires a restart because it changes glyph texture ownership. Once the binder is active, theme selection and file hot reload are live.

See `ZachFix/glyphs/README.md` and `GAMEPAD_ATLAS.md` for custom-theme authoring information.
