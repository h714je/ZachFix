# Rendering and world settings

## Display resolution

`[Display] Width` and `Height` control the requested output size. `0 / 0` lets ZachFix resolve the current target monitor size.

`Borderless = true` is the recommended windowed setup, especially for reliable Alt-Tab behavior.

Display/window-mode changes are startup/device-creation settings rather than ordinary F10 Hot Apply settings.

## Internal rendering resolution

`[Rendering] InternalWidth` and `InternalHeight` take priority when both are non-zero.

Otherwise:

```text
internal resolution = display resolution * InternalScale
```

Examples:

- `InternalScale = 1.0` -> native output resolution;
- `1.5` -> 150% per axis;
- `2.0` -> 200% per axis.

The parser accepts `InternalScale` from `0.25` through `4.0`; the F10 slider exposes the normal `0.50` through `4.00` tuning range.

`FixPixelOffset = true` applies ZachFix's scaled-rendering pixel-offset correction.

## Shadows

```ini
[Shadows]
Scale = 4
ImprovePrecision = true
```

`Scale` accepts `1..8` and scales recognized vanilla shadow resources.

`ImprovePrecision = true` requests the original DPFix shadow-depth precision fix for recognized 512/1024 shadow depth maps, using `D32F_LOCKABLE` instead of D16 where supported. This option is selected when shadow resources are created and therefore requires a restart.

If a backend rejects the requested precision format, ZachFix uses its compatibility fallback instead of making resource creation fatal.

## Reflections

```ini
[Reflections]
Scale = 4
```

`Scale` accepts `1..8` and scales the recognized vanilla reflection targets.

## Legacy depth of field

```ini
[DepthOfField]
ImproveResolution = true
AdditionalBlur = 0
```

`ImproveResolution` raises the game's original DoF intermediate resolution in proportion to the active internal resolution.

`AdditionalBlur` keeps the game's existing focus/depth behavior and only adds optional softening after the original DoF blur:

- `0` - Off
- `1` - Soft
- `2` - Stronger

The F10 Off/Soft/Stronger control is immediate.

ZachFix Depth of Field is a separate optional path documented in [postfx.md](postfx.md).

## World detail

```ini
[World]
HighDetailDistanceScale = 1
MainFrustumDistanceMode = 0
ObjectActivationDistanceScale = 1
ObjectLODDistanceScale = 1
FixInteriorOcclusionBugs = true
```

### HighDetailDistanceScale

- `1` - original high-detail 2x2 core;
- `2` - promotes the existing outer 4x4 streaming ring to high-detail content.

The existing streaming footprint is retained. Changes become fully visible as streaming cells transition.

### MainFrustumDistanceMode

This changes only the far planes of DP's existing short-range CRdCamera main-frustum classes. Object class selection, AABB/frustum tests, streaming, activation, LOD selection, shadow volumes and render submission remain native.

- `0` - Original: class 3/4/5 = `5000 / 1000 / 500`
- `1` - Extended: class 3/4/5 = `5000 / 1000 / 1000`
- `2` - Extended Plus: class 3/4/5 = `5000 / 5000 / 5000`
- `3` - Extreme: class 3/4/5 = `20000 / 20000 / 20000`

Classes 0/1/2 always remain at their native `200000 / 80000 / 20000` far planes. Extended Plus and Extreme affect every object assigned to the raised classes, so they can increase CPU/GPU load substantially in dense scenes. Extreme can also expose a farther, separate streaming/residency ceiling once the main-frustum limit is no longer the first cutoff.

### ObjectActivationDistanceScale

- `1` - original 1000 world-unit activation radius;
- `2` - extended 2000 world-unit radius.

This reduces visible world-prop pop-in while keeping the game's active-list and streaming structures.

### ObjectLODDistanceScale

- `1` - Original
- `2` - Extended
- `3` - High
- `4` - Extreme

This extends native mesh LOD transition distances only for type-1 render objects that actually contain native multi-LOD resource groups. Single-LOD resources and unrelated render-object types are unaffected. The native resource groups and LOD selector remain authoritative; ZachFix scales only the existing camera-distance metric.

This does not expand the streaming grid, main-frustum class, or object activation radius.

### FixInteriorOcclusionBugs

Enabled by default. This fixes the confirmed Director's Cut visibility-volume regression that can incorrectly hide visible props near interior walls/mirrors.

The fix is narrow: normal camera frustum culling, streaming, object activation, LOD, and unrelated visibility callers remain native.

## Texture filtering

```ini
[Filtering]
Mode = Original
MaxAnisotropy = 16
```

Modes:

- `Original` - leave the game's sampler filtering unchanged;
- `Bilinear` - DPFix-style compatibility behavior for ordinary 2D textures;
- `Anisotropic` - smart AF for eligible mipmapped, non-render-target 2D textures.

`MaxAnisotropy` accepts `2..16` and is also clamped to the device capability at runtime.

The smart anisotropic path avoids overriding intentional point sampling and preserves the game's MAG/MIP policy where appropriate.

## Hot Apply

The Graphics tab can rebuild ZachFix-managed replacement render resources without calling `IDirect3DDevice9::Reset`.

Important exceptions:

- shadow depth precision requires a restart;
- display/device-creation settings require a restart;
- texture ownership modes have their own restart rules documented in [textures.md](textures.md).
