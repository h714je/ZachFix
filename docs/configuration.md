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
MainFrustumDistanceMode = 0
ObjectActivationDistanceScale = 1
ObjectLODDistanceScale = 1
FixInteriorOcclusionBugs = true
```

- `HighDetailDistanceScale`: `1` or `2`; applies live, with full visual update as cells transition.
- `MainFrustumDistanceMode`: `0` Original (`5000/1000/500` for classes 3/4/5), `1` Extended (`5000/1000/1000`), `2` Extended Plus (`5000/5000/5000`), `3` Extreme (`20000/20000/20000`); immediate after Apply. Classes 0/1/2 remain `200000/80000/20000`.
- `ObjectActivationDistanceScale`: `1` or `2`; immediate after Apply.
- `ObjectLODDistanceScale`: `1..4`; immediate after Apply. It scales native mesh LOD transition distances for type-1 render objects with multi-LOD resource groups and does not alter streaming or activation.
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

## Screenshots and comparison presets

```ini
[Screenshots]
Enabled = true
CyclePresetKey = F6
CaptureKey = F7
CaptureAllKey = F8
SettleFrames = 6
AutoCaptureOnSwitch = false
PauseDuringCaptureAll = false
Directory = ZachFix\screenshots
PresetCount = 3
Preset1 = Original
Preset2 = ZachFix
Preset3 = Xbox360

[ScreenshotPreset.Original]
Base = Original

[ScreenshotPreset.ZachFix]
Base = Configured

[ScreenshotPreset.Xbox360]
Base = Configured
ColorMode = Xbox360Full
DisplayGamma = Xbox360HDTV
```

The screenshot workflow is designed for matched before/after captures without editing or replacing the main INI between shots.

- `CyclePresetKey` selects the next preset. The first press selects `Preset1`.
- `CaptureKey` writes the current final backbuffer to a lossless PNG.
- `CaptureAllKey` applies every preset in order, waits `SettleFrames` fully rendered frames after each switch, saves a timestamp-matched PNG set, then restores the exact pre-batch live render/PostFX state.
- `AutoCaptureOnSwitch = true` makes the cycle key capture automatically after the settle delay.
- `PauseDuringCaptureAll = true` freezes DP's gameplay timers during a batch for tighter normal-gameplay A/B framing. Leave it false for cutscenes because the existing gameplay-timer freeze is not cutscene-safe.
- Captures are rejected while F10 is open so the settings panel cannot accidentally appear in comparison images.

`Base = Configured` uses the normal render/PostFX state loaded from `ZachFix.ini` at process startup. `Base = Original` switches only settings that ZachFix can safely reverse at runtime: native internal scale, shadow/reflection scale, legacy DoF resolution/blur, pixel-offset correction, world-distance controls, interior-occlusion fix, texture filtering, and ZachFix PostFX/presentation modes. Restart-only state such as shadow depth precision and already-created texture ownership is intentionally preserved.

Each `[ScreenshotPreset.<name>]` section can optionally override these live values:

- `InternalWidth`, `InternalHeight`, `InternalScale`
- `ShadowScale`, `ReflectionScale`, `ImproveDoFResolution`, `AdditionalDoFBlur`, `FixPixelOffset`
- `HighDetailDistanceScale`, `MainFrustumDistanceMode`, `ObjectActivationDistanceScale`, `ObjectLODDistanceScale`, `FixInteriorOcclusionBugs`
- `FilteringMode`, `MaxAnisotropy`
- `AOMode`, `BloomMode`, `DoFMode`, `ExposureMode`, `ColorMode`, `DisplayGamma`

Up to eight presets are supported. Preset definitions are loaded at startup.

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

Modes: `Legacy`, `Bloom`, `ShowBloom`. `BloomNG` / `NG` remain accepted as legacy aliases.

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

Modes: `Legacy`, `DepthOfField`, `ShowCoC`, `ShowNear`, `ShowFar`. `DoFNG` / `DoF` / `NG` remain accepted as legacy aliases.

Resolution: `Full`, `Half`, `Quarter` in the INI parser; current F10 DoF quality control exposes the supported live choices for the active implementation.

## PostFX.Color

```ini
[PostFX.Color]
Mode = PC
DisplayGamma = PC
```

`Mode` values:

- `PC` - Director's Cut PC tone/color path.
- `Xbox360Grading` - restores the original scene-authored ENV Contrast/Pitch/Chroma/Addsub grading tail.
- `Xbox360Full` - grading restoration plus removal of the PC-only final-exposure scaling from the native tone path.

`DisplayGamma` values:

- `PC` - normal PC/sRGB output.
- `Xbox360HDTV` - late full-frame Xbox 360 HDTV transfer (`sRGB decode -> BT.709 encode`) after the complete game frame and before Present.

These controls are live from the F10 PostFX Color tab. Display gamma is separate from tone mapping/color grading and also affects the game's HUD/menu because it runs on the completed backbuffer. The ZachFix F10 UI is drawn after this pass and remains neutral.

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
