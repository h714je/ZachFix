# PostFX

ZachFix can replace selected parts of the game's final-composite/PostFX path while leaving the rest of the game renderer intact.

All PostFX settings have dedicated F10 controls and dedicated INI sections.

## Tuning architecture

PostFX modes, user-facing values, and defaults are centralized in
`postfx_tuning.h` / `postfx_tuning.inl`. AO, bloom, DoF, exposure, and color/output modules
own render resources and runtime telemetry, but no longer own independent
setting globals. The existing per-effect Get/Set calls are compatibility
facades over the same central atomic store, and config persistence reads a
combined `PostFxTuningSnapshot`.

This separation is intentional groundwork for a future dedicated PostFX tuning
file: persistence can change without moving renderer state or duplicating
default values again.

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

The ambient-occlusion implementation is a GTAO-style depth/normal effect integrated into the ZachFix PostFX path.

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
- `Bloom` - use ZachFix Bloom. `BloomNG` / `NG` remain accepted as legacy INI aliases;
- `ShowBloom` - debug/preview bloom output.

F10 exposes live threshold, knee, intensity, scatter, and level controls.

## Depth of Field

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
- `DepthOfField` - use ZachFix Depth of Field. `DoFNG` / `DoF` / `NG` remain accepted as legacy INI aliases.
- `ShowCoC`
- `ShowNear`
- `ShowFar`

ZachFix depth of field is separate from `[DepthOfField] ImproveResolution` / `AdditionalBlur`, which operate on the game's legacy DoF path.

## Color and Xbox 360 output restoration

The **Color** tab exposes two independent restoration controls.

### Tone / color path

```ini
[PostFX.Color]
Mode = PC
```

Modes:

- `PC` - keeps the Director's Cut PC final-composite tone/color behavior.
- `Xbox360Grading` - restores the scene-authored Xbox 360 ENV grading tail driven by the game's live Contrast, Pitch, Chroma, and Addsub values.
- `Xbox360Full` - includes the grading restoration and also removes the Director's Cut PC-only exposure scaling from the native final-composite path. This covers the normal PC `x0.85` scale and the narrow state-`0x14` `x0.75` special case when detected.

For the cleanest native PC/Xbox comparison, leave AO Off and Bloom/DoF/Exposure on Legacy while switching this control. ZachFix Auto Exposure intentionally supersedes the authored native exposure path when enabled.

### Display gamma

```ini
[PostFX.Color]
DisplayGamma = PC
```

Modes:

- `PC` - normal PC/sRGB output.
- `Xbox360HDTV` - emulates the reconstructed Xbox 360 HDTV display-LUT transfer as a late full-frame pass: `sRGB decode -> BT.709 encode`.

The display transfer runs on the completed game backbuffer after the game's HUD/menu and before Present. It is channel-neutral, so it primarily changes shadow/lower-mid contrast rather than adding a color tint. The ZachFix F10 UI is rendered after this transfer and remains neutral.

Both controls apply live and do not patch `DP.exe`.

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
