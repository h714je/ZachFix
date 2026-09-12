# Changelog

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
