# PostFX

ZachFix can replace selected parts of the game's final-composite/PostFX path while leaving the rest of the game renderer intact.

All PostFX settings have dedicated F10 controls and dedicated INI sections.

## Ambient occlusion

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

Modes:

- `Off`
- `ShowRaw`
- `ShowFiltered`
- `ShowEnhanced`
- `CompositeHDR`

Resolution values are `Full`, `Half`, or `Quarter`.

The production AO implementation is a GTAO-lite style depth/normal effect integrated into the ZachFix PostFX path.

## Bloom

```ini
[PostFX.Bloom]
Mode = Legacy
ThresholdEV = 0.0
SoftKnee = 0.50
Intensity = 0.35
Scatter = 0.70
Levels = 5
```

Modes:

- `Legacy` - use the game's existing bloom path;
- `BloomNG` - use ZachFix Bloom NG;
- `ShowBloom` - debug/preview bloom output.

F10 exposes live threshold, knee, intensity, scatter, and level controls.

## DoF NG

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

Modes:

- `Legacy`
- `DoFNG`
- `ShowCoC`
- `ShowNear`
- `ShowFar`

DoF NG is separate from `[DepthOfField] ImproveResolution` / `AdditionalBlur`, which operate on the game's legacy DoF path.

## Exposure and highlight shoulder

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

Modes:

- `Legacy`
- `AutoExposure`
- `Shoulder`
- `AutoExposureShoulder`

The F10 Exposure page also exposes adaptation reset and runtime telemetry when available.

## PostFX Preview Freeze

F10 can freeze the PostFX input set so AO, bloom, DoF, and exposure can be tuned against one captured game frame while the game itself continues running.

This is preferable to gameplay timer pause during cutscenes.

The preview freeze is for PostFX tuning only. Geometry-dependent settings such as internal resolution, shadow scale, reflection scale, or streaming changes still require live game rendering.
