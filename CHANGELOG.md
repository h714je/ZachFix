# Changelog

## Unreleased

## v0.1.0-rc4

Stability-focused release candidate.

### Stability

- Fixed a vanilla engine hang caused by a zero frame delta producing a `0/0` movement-speed calculation when an actor had not moved.
- Prevented the resulting NaN from propagating into the game's angle/state processing and triggering its intentional infinite-loop NaN guard.
- Limited the fix to the confirmed `distance == 0 && frameDelta == 0` case; all other movement-speed calculations retain the original game behavior.
- Added executable build and instruction-signature validation so the fix is applied only to the validated Steam `DP.exe` code path. LAA and the documented optional No Intro tweak remain compatible.

### Diagnostics

- Added lightweight stability logging when the zero-delta NaN guard prevents an invalid vanilla speed calculation.

### Compatibility

- Retains the PostFX NG, rendering, UI, compatibility, and tuning improvements from RC3.

## v0.1.0-rc3

PostFX and compatibility-focused release candidate.

### PostFX and rendering

- Added the PostFX stack with GTAO-lite ambient occlusion, adaptive exposure, HDR bloom, DoF NG, and highlight shoulder controls.
- Added frozen PostFX Preview for tuning AO, Bloom, DoF, and Exposure against a captured HDR/G-buffer frame while the game continues running underneath.
- Added persistent PostFX configuration and live hot-apply support.
- Added full-resolution G-buffer resource tracking used by native composite diagnostics and PostFX effects.
- Added the original DPFix shadow-depth precision correction using D32F_LOCKABLE for recognized shadow maps, with safe D16 fallback.
- Added the original DPFix dual-view correction path alongside the existing enemy shadow/afterimage trail fix.

### UI and tuning

- Reorganized the F10 interface with a dedicated PostFX page and compact AO, Bloom, DoF, and Exposure sub-tabs.
- Added a gameplay-only timer pause for normal gameplay tuning.
- Latched gameplay pause state per F10 session so Apply/Reload cannot unexpectedly enable it while the settings panel is already open.
- Added runtime Binary ID logging so the exact loaded `ZachFix.asi` can be identified from the log.

### Compatibility and packaging

- Fixed the CPack runtime layout so `ZachFix.asi` is packaged under `scripts/`, matching the documented installation path.
- Excluded MinHook development headers, static libraries, and CMake package files from the redistributable ZIP while retaining its license notice.
- Kept native D3D9, DXVK, dgVoodoo2, and ReShade compatibility paths validated during development.

### Documentation

- Expanded installation and troubleshooting guidance for the Steam version, NTCore 4GB / Large Address Aware, `d3dx9_43.dll`, DXVK, dgVoodoo2, ReShade, XiDi, and the optional no-intro executable tweak.

### Known limitations

- Gameplay timer pause is intended for normal gameplay. Some cutscenes may hang if their timers are frozen; use Frozen PostFX Preview for visual tuning in those scenes instead.

## v0.1.0-rc2

Compatibility follow-up to the first public release candidate.

### Rendering fixes

- Fixed enemy shadow/afterimage trails rendering inside a fixed 1280x720 region at higher internal resolutions.
- Ported the original DPFix enemy shadow-trail correction: the identified offscreen stream path now restores VS c254 to `{640, 360, 640, 360}` before drawing.
- Kept the correction independent of InternalScale, ReflectionScale, and backend selection; it applies only to the original DPFix stream signature on offscreen render targets.

### Research cleanup

- The temporary Effect Frame Probe / Shader Sweep / Shader Isolation hooks used to diagnose the issue remain research-only and are not part of the production hook surface.

## v0.1.0-rc1

First public release candidate under the **ZachFix** name.

### Rendering and compatibility

- Native-resolution and borderless display handling.
- Arbitrary internal resolution / supersampling.
- Scaled shadows and reflections.
- Improved DoF resolution and optional live Off / Soft / Stronger additional blur.
- Pixel-offset correction for scaled rendering paths.
- Extended high-detail world streaming option.
- Smart Bilinear and Anisotropic texture filtering modes.
- Backend-independent D3D9 integration validated with:
  - native Windows D3D9;
  - DXVK / Vulkan;
  - dgVoodoo2 / D3D11.
- Native-D3D9 UI compatibility through EndScene/Present fallbacks and one-toggle-per-keypress input latching.

### Runtime UI and hot apply

- F10 Dear ImGui settings panel with game input isolation.
- Settings / Diagnostics / About tab layout.
- Hot Apply without a D3D9 device Reset.
- Transactional replacement backing resources.
- Runtime Resource Audit with created/released/outstanding/active counts and estimated memory.

### Texture workflow

- Original DPFix-compatible SuperFastHash naming.
- DDS/PNG override lookup.
- Original DPFix `dpfix\tex_override` compatibility.
- `DPFix` and exact-size `Preserve` dimension modes.
- Developer Mode with original-baseline retention, texture inspection, dumping, and live add/edit/remove reloads.
- Pre-public `DPFixNG` config/texture paths are accepted as migration fallbacks; new saves and dumps use the ZachFix name.

### Project/release engineering

- Renamed project, binary, config, logs, UI, source tree, and texture directories from the private development name to **ZachFix**.
- Added GPLv3 project license and expanded third-party attribution.
- Added CPack release packaging and GitHub Actions x86 build validation.

### Pre-public history

The `0.0.x` versions were private development checkpoints used while reverse-engineering and validating Deadly Premonition's rendering pipeline. `v0.0.51` was the final private baseline before the ZachFix rename and first public release candidate.
