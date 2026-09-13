# ZachFix

**ZachFix** is an unofficial continuation and modernization of Peter Thoman's original **DPFix** for **Deadly Premonition: The Director's Cut** on Windows.

It is a game-specific Direct3D 9 fix layer. ZachFix does **not** require a particular graphics wrapper: it can run on native D3D9, DXVK, or dgVoodoo2.

> "Zach, do you see this?"

## Status

`v0.1.0-rc2` is the current public release candidate. It adds the original DPFix correction for Deadly Premonition's enemy shadow/afterimage trail path while keeping the RC1 feature set otherwise frozen.

Development/test target:

- `DP.exe`, 32-bit (x86)
- SHA-256: `cf4022a85faf8a837c34a5b17897236cfbc1ea0e54b4d58f0575d6013568a285`
- Windows 11 was used for current validation

Other executable builds may work, but game-code patches are validated against the target above.

## Features

- Native monitor resolution and borderless display handling.
- Arbitrary internal rendering resolution through explicit dimensions or `InternalScale` supersampling.
- Higher-resolution shadows and reflections.
- Higher-resolution depth of field plus optional additional softening.
- Pixel-offset corrections for the game's scaled rendering paths.
- Original DPFix enemy shadow/afterimage trail correction for high internal resolutions.
- Optional extended high-detail world streaming from the original 2x2 core to the existing 4x4 ring.
- Smart bilinear / anisotropic texture filtering overrides.
- DPFix-compatible texture hashing and texture replacement.
- Exact-dimension NPOT texture replacement mode for AI/upscaled texture packs.
- Texture Developer Mode with dumping, inspection, and live add/edit/remove override reloads.
- F10 Dear ImGui configuration UI.
- Hot Apply for render-resource settings without `IDirect3DDevice9::Reset`.
- Runtime replacement-resource audit for hot-apply lifetime validation.
- Wrapper-independent D3D9 hook path with native-D3D9 UI fallbacks.

## Backend compatibility

These combinations have been tested in-game during development:

| D3D9 backend | ZachFix | F10 UI | Hot Apply | ReShade depth / MXAO |
| --- | --- | --- | --- | --- |
| Native Windows D3D9 | Yes | Yes | Yes | Not separately tested |
| DXVK -> Vulkan | Yes | Yes | Yes | Yes |
| dgVoodoo2 -> D3D11 | Yes | Yes | Yes | Yes |

With dgVoodoo2 + ReShade DXGI, DisplayDepth, MXAO/SSAO, CAS, and ZachFix Hot Apply were validated together. The Runtime Resource Audit returned `Outstanding` to `0` after returning managed resources to native dimensions.

## Installation

ZachFix is an ASI plugin. An ASI loader is required. The current tested deployment uses **Ultimate ASI Loader** via `winmm.dll`, but the loader is not bundled with ZachFix.

A typical installation looks like this:

```text
Deadly Premonition The Director's Cut/
|-- DP.exe
|-- ZachFix.ini
|-- winmm.dll                 <- ASI loader, if using Ultimate ASI Loader this way
|-- scripts/
|   `-- ZachFix.asi
`-- ZachFix/
    `-- textures/
        |-- override/
        `-- dump/
```

The `ZachFix/textures` directories are created automatically when the texture subsystem initializes.

Press **F10** in-game to open the settings UI.

### Native D3D9

Do not place a local graphics-wrapper `d3d9.dll` beside `DP.exe`.

```text
DP.exe -> Windows D3D9
         ^ ZachFix hooks the game's D3D9 interface
```

### DXVK

Place the **32-bit** DXVK `d3d9.dll` beside `DP.exe`.

```text
DP.exe -> ZachFix -> DXVK D3D9 -> Vulkan
```

For ReShade in this stack, use the Vulkan path.

### dgVoodoo2

Place the **x86** dgVoodoo2 D3D9 DLL beside `DP.exe`. The currently validated path uses dgVoodoo2's D3D11 output.

```text
DP.exe -> ZachFix -> dgVoodoo2 D3D9 -> D3D11
                                      -> ReShade DXGI (optional)
```

## Configuration

The main configuration file is `ZachFix.ini` beside `DP.exe`. The F10 UI can edit, apply, save, and reload supported values.

Important sections:

- `[Display]`: output resolution and borderless mode.
- `[Rendering]`: internal resolution / scale and pixel-offset correction.
- `[Shadows]`: shadow resolution scale.
- `[Reflections]`: reflection resolution scale.
- `[DepthOfField]`: higher-resolution DoF and optional additional softening.
- `[World]`: original or extended high-detail streaming grid.
- `[Filtering]`: Original, Bilinear, or smart Anisotropic filtering.
- `[Textures]`: overrides, NPOT dimension behavior, Developer Mode, and dumping.
- `[UI]`: UI enable state and toggle key.

### Hot Apply

Hot Apply does not reset the D3D9 device. The game keeps its original logical COM resources while ZachFix creates and substitutes replacement backing resources where needed.

The **Diagnostics** tab contains the Runtime Resource Audit. In a steady state, the useful invariant is:

```text
Created - Released == Active replacements
```

When no replacement backing resources are needed, `Outstanding` should return to `0`.

## Texture overrides

ZachFix keeps original DPFix texture identity: filenames use an 8-digit lower-case SuperFastHash of the original encoded D3DX source bytes.

Lookup order:

1. `ZachFix\textures\override\<hash>.dds`
2. `ZachFix\textures\override\<hash>.png`
3. private pre-release compatibility path `DPFixNG\textures\override\...`
4. original DPFix compatibility path `dpfix\tex_override\...`

`DimensionMode = DPFix` reproduces original DPFix/D3DX default sizing behavior, which can round NPOT replacement dimensions upward. `DimensionMode = Preserve` requests the exact replacement dimensions and is recommended for true-size AI/upscaled packs when the device supports NPOT textures.

### Texture Developer Mode

Set:

```ini
[Textures]
DeveloperMode = true
```

and restart the game. In this mode ZachFix keeps the game's original logical texture as the baseline and substitutes overrides at bind time. This enables symmetric live **add / edit / remove** testing and reliable rollback to the original texture.

`DumpTextures = true` dumps original textures to:

```text
ZachFix\textures\dump\<hash>.tga
```

Developer Mode changes texture ownership strategy and therefore requires a restart when enabled or disabled.

## Depth of field

`ImproveResolution = true` raises the game's original 448x252 DoF target in proportion to the active internal rendering resolution.

`AdditionalBlur` keeps the original DPFix idea of applying extra softening after the game's own DoF pass, but ZachFix uses a lightweight filtered down/up pass rather than importing the old GAUSS effect framework:

```ini
[DepthOfField]
ImproveResolution = true
AdditionalBlur = 0   # 0 Off, 1 Soft, 2 Stronger
```

The F10 **Off / Soft / Stronger** controls are live and do not require Apply.

## Building

Requirements:

- Windows
- Visual Studio 2022 with C++ desktop tools
- CMake 3.20+
- x86 / Win32 build target
- Internet access during initial CMake configure for pinned MinHook and Dear ImGui dependencies

From a Developer PowerShell:

```powershell
cmake -S . -B build -A Win32
cmake --build build --config Release
```

The binary is produced as `ZachFix.asi`.

To create the redistributable ZIP through CPack:

```powershell
cpack --config build\CPackConfig.cmake -C Release
```

The package contains ZachFix, the default INI, documentation, GPLv3 license, and third-party license notices fetched with the pinned dependencies.

## Known limitations

- ZachFix targets the 32-bit Director's Cut executable. It is not a generic D3D9 injector for arbitrary games.
- Some fixes depend on Deadly Premonition-specific resource dimensions and render behavior.
- Texture Developer Mode requires a restart to change its ownership model.
- World-detail changes become fully visible as streaming cells transition.
- The additional DoF blur is a lightweight compatibility-oriented approximation, not a byte-for-byte recreation of original DPFix's Gaussian implementation.
- Native D3D9, DXVK, and dgVoodoo2 are supported paths, but external wrappers and overlays can still conflict with one another independently of ZachFix.

## Credits

- **Peter Thoman (Durante)** for the original DPFix/DSFix work and rendering research that made this project possible.
- **Tsuda Kageyu and contributors** for MinHook.
- **Omar Cornut and contributors** for Dear ImGui.
- **Paul Hsieh** for SuperFastHash.
- **DXVK**, **ReShade**, and **Ultimate ASI Loader / ThirteenAG** for the modern compatibility/modding ecosystem used alongside ZachFix.
- The Deadly Premonition modding community and testers for compatibility findings and validation.

See [THIRD_PARTY.md](THIRD_PARTY.md) for dependency and license details.

## License

ZachFix is distributed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE).

This project is unofficial and is not affiliated with Access Games, SWERY, Rising Star Games, Marvelous, or the publishers/rightsholders of Deadly Premonition.
