# Configuration reference

`ZachFix.ini` lives beside `DP.exe`.

Startup and F10 **Reload INI** use the same canonical parser for normal ZachFix settings. Invalid values fall back to the documented safe defaults. PostFX sections use their dedicated runtime parser.

Boolean values accept normal forms such as `true/false`, `yes/no`, `on/off`, and `1/0`.

F10 exposes the settings intended for interactive tuning. **Save to INI** persists editable settings. **Reload INI** rereads the file. **Apply** commits settings that support runtime changes; startup-only fields wait for the next launch.

## Display

```ini
[Display]
Width = 0
Height = 0
Borderless = true
```

| Setting | Meaning | Runtime behavior |
| --- | --- | --- |
| `Width`, `Height` | Output size. `0/0` resolves to the target monitor. | Startup/device creation |
| `Borderless` | Borderless windowed presentation when the game is windowed. | Startup/device creation |

## Rendering

```ini
[Rendering]
InternalWidth = 0
InternalHeight = 0
InternalScale = 1.0
FixPixelOffset = true
```

If both explicit internal dimensions are non-zero, they take priority over `InternalScale`.

`InternalScale` accepts `0.25..4.0`; F10 exposes `0.50..4.00`.

Internal resolution and pixel-offset changes are live on Apply.

## Shadows

```ini
[Shadows]
Scale = 4
ImprovePrecision = true
```

- `Scale`: `1..8`, live on Apply.
- `ImprovePrecision`: selects the shadow depth format at resource creation and requires restart.

## Reflections

```ini
[Reflections]
Scale = 4
```

`Scale` accepts `1..8` and is live on Apply.

## DepthOfField

```ini
[DepthOfField]
ImproveResolution = true
AdditionalBlur = 0
```

- `ImproveResolution`: live on Apply.
- `AdditionalBlur`: `0` Off, `1` Soft, `2` Stronger; immediate from F10.

## World

```ini
[World]
HighDetailDistanceScale = 1
ObjectActivationDistanceScale = 1
ObjectLODDistanceScale = 1
FixInteriorOcclusionBugs = true
```

- `HighDetailDistanceScale`: `1` or `2`; applies live, with full visual update as cells transition.
- `ObjectActivationDistanceScale`: `1` or `2`; immediate after Apply.
- `ObjectLODDistanceScale`: `1..4`; immediate after Apply.
- `FixInteriorOcclusionBugs`: boolean; immediate after Apply.

## Filtering

```ini
[Filtering]
Mode = Original
MaxAnisotropy = 16
```

`Mode` values:

- `Original`
- `Bilinear`
- `Anisotropic`

`MaxAnisotropy` accepts `2..16` and is also limited by device capabilities. Filtering changes are live on Apply.

## Textures

```ini
[Textures]
EnableOverride = true
DeveloperMode = false
DimensionMode = DPFix
DumpTextures = false
```

`DimensionMode` values are `DPFix` or `Preserve`.

- `EnableOverride`: applied to the active texture path.
- `DeveloperMode`: restart required to enable/disable.
- `DimensionMode`: used for new override loads; Developer Mode hot reload uses the applied mode.
- `DumpTextures`: Developer Mode only, for new texture loads.

See [textures.md](textures.md).

## SaveSafety

```ini
[SaveSafety]
Enabled = true
BackupCount = 10
```

`BackupCount` accepts `1..100`.

These are startup settings. See [save-safety.md](save-safety.md).

## Gamepad

```ini
[Gamepad]
NativeXInput = false
InputProfile = Xbox360
AnalogVehicleTriggers = true
VehicleTriggerDeadzone = 30
Vibration = true
VibrationStrength = 1.0
```

- `NativeXInput`: startup backend choice.
- `InputProfile`: `PC` or `Xbox360`; immediate in F10.
- `AnalogVehicleTriggers`: immediate in F10.
- `VehicleTriggerDeadzone`: `0..254`; immediate in F10.
- `Vibration`: immediate while Native XInput is active.
- `VibrationStrength`: `0.0..1.0`; immediate while Native XInput is active.

## Input

```ini
[Input]
AutoSwitch = false
```

Automatic keyboard/mouse vs controller mode switching. This is a startup setting.

## Glyphs

```ini
[Glyphs]
DynamicAtlas = false
HotReload = true
KeyboardSet = Native
GamepadSet = Native
```

- `DynamicAtlas`: restart required to enable/disable.
- `HotReload`: live once Dynamic Atlas is active.
- `KeyboardSet`, `GamepadSet`: live once Dynamic Atlas is active.

Theme names are file stems under `ZachFix\glyphs\keyboard` and `ZachFix\glyphs\gamepad`.

## UI

```ini
[UI]
Enabled = true
ToggleKey = F10
PauseGameWhileOpen = false
```

`ToggleKey` accepts F1-F12, common named keys such as Home/End/Insert/Delete/Pause/ScrollLock/NumLock, a single letter/digit, or a numeric virtual-key code.

`PauseGameWhileOpen` is applied with **Apply** and takes effect the next time F10 is opened.

## PostFX.AO

```ini
[PostFX.AO]
Mode = Off
Radius = 4.0
Strength = 1.0
Bias = 0.04
Thickness = 0.35
Power = 1.0
Resolution = Half
```

Modes: `Off`, `ShowRaw`, `ShowFiltered`, `ShowEnhanced`, `CompositeHDR`.

Resolution: `Full`, `Half`, `Quarter`.

## PostFX.Bloom

```ini
[PostFX.Bloom]
Mode = Legacy
ThresholdEV = 0.0
SoftKnee = 0.50
Intensity = 0.35
Scatter = 0.70
Levels = 5
```

Modes: `Legacy`, `BloomNG`, `ShowBloom`.

## PostFX.DoF

```ini
[PostFX.DoF]
Mode = Legacy
MaxRadiusPixels = 12.0
NearStrength = 1.0
FarStrength = 1.0
DepthReject = 1.5
HighlightBoost = 0.25
Resolution = Half
```

Modes: `Legacy`, `DoFNG`, `ShowCoC`, `ShowNear`, `ShowFar`.

Resolution: `Full`, `Half`, `Quarter` in the INI parser; current F10 DoF quality control exposes the supported live choices for the active implementation.

## PostFX.Exposure

```ini
[PostFX.Exposure]
Mode = Legacy
CompensationEV = 0.0
MeterMinEV = -10.0
MeterMaxEV = 6.0
MinExposureEV = -8.0
MaxExposureEV = 4.0
BrightenSpeed = 1.5
DarkenSpeed = 3.0
ShoulderStrength = 1.0
WhitePoint = 4.0
```

Modes: `Legacy`, `AutoExposure`, `Shoulder`, `AutoExposureShoulder`.

PostFX controls are live through their F10 page and dedicated runtime parser.
