# ZachFix

**ZachFix** is an unofficial continuation and modernization of Peter Thoman's original **DPFix** for **Deadly Premonition: The Director's Cut** on Windows.

It is a game-specific Direct3D 9 fix layer. ZachFix does **not** require a particular graphics wrapper: it can run on native D3D9, DXVK, or dgVoodoo2.

> "Zach, do you see this?"

## Status

`v0.1.0-rc5` is the current release candidate. It fixes DPLauncher exclusive fullscreen device creation, improves D3D9 device-reset handling, and preserves genuine exclusive fullscreen while retaining the stability, PostFX, rendering, compatibility, and tuning improvements from previous release candidates.

Primary development/test target:

- **Steam version** of *Deadly Premonition: The Director's Cut*
- `DP.exe`, 32-bit (x86)
- Windows 11

Other executable builds may work, but game-code hooks and executable-specific behavior are validated against the Steam build. Common one-byte/header tweaks such as the optional No Intro and Large Address Aware changes do not change the executable layout used by ZachFix, but always keep a backup of `DP.exe` before modifying it.

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
- F10 Dear ImGui configuration UI with separate Graphics, PostFX, Diagnostics, and About pages.
- Hot Apply for render-resource settings without `IDirect3DDevice9::Reset`.
- Runtime replacement-resource audit for hot-apply lifetime validation.
- Wrapper-independent D3D9 hook path with native-D3D9 UI fallbacks.
- PostFX stack with GTAO-lite ambient occlusion, adaptive exposure, HDR bloom, DoF NG, and highlight shoulder/tone shaping.
- Frozen PostFX Preview for tuning the PostFX stack against one captured frame while the game continues running underneath.
- Optional gameplay-only F10 timer pause for normal gameplay tuning.

## Recommended game setup

ZachFix can run without every item in this section, but these are the fixes and compatibility choices used for current development and testing.

### Large Address Aware / 4 GB patch

Deadly Premonition is a 32-bit game. I strongly recommend marking `DP.exe` as **Large Address Aware**, especially when using high internal resolutions, large shadow/reflection targets, texture replacements, and the PostFX stack.

I use the **NTCore 4GB Patch**:

https://ntcore.com/4gb-patch/

Back up `DP.exe` before patching it. This is a modification to the game executable, not part of ZachFix.

### DirectX 9 and `d3dx9_43.dll`

If the game crashes before ZachFix even gets a chance to load, check `d3dx9_43.dll` as part of the base-game troubleshooting.

I have encountered two different Microsoft-signed builds/copies of this DLL where one would not start Deadly Premonition correctly. A copy known to work on my system is:

```text
File version: 9.29.952.3111
SHA-256: 0b28546be22c71834501f7d7185ede5d79742457331c7ee09efc14490dd64f5f
```

Reinstalling the DirectX End-User Runtimes (June 2010) did **not** replace the problematic copy for me, so do not assume a reinstall necessarily changed the DLL that the game is loading. If you have a local `d3dx9_43.dll` beside `DP.exe`, compare its version/hash and make sure the game is not picking up an unwanted copy.

Use the official Microsoft DirectX runtime rather than downloading individual DLLs from third-party DLL sites.

### Exclusive fullscreen and Alt-Tab

Deadly Premonition's D3D9 exclusive-fullscreen recovery is unreliable across repeated Alt-Tab cycles. ZachFix normalizes the game's fullscreen device/reset parameters and releases its own reset-sensitive resources, but the game can still become stuck after a later Alt-Tab and leave a black screen without issuing another device Reset.

For reliable Alt-Tab behavior, the recommended setup is:

1. Disable **Fullscreen** in `DPLauncher.exe`.
2. Set `Borderless = true` in ZachFix's `[Display]` configuration.

This keeps the D3D9 device in windowed mode while presenting a borderless full-monitor window. True exclusive fullscreen remains available, but repeated Alt-Tab recovery is a known game limitation and is not guaranteed.

## Backend compatibility

These combinations have been tested in-game during development:

| D3D9 backend | ZachFix | F10 UI | Hot Apply | ReShade depth / MXAO |
| --- | --- | --- | --- | --- |
| Native Windows D3D9 | Yes | Yes | Yes | Not separately tested |
| DXVK -> Vulkan | Yes | Yes | Yes | Yes |
| dgVoodoo2 -> D3D11 | Yes | Yes | Yes | Yes |

With dgVoodoo2 + ReShade DXGI, DisplayDepth, MXAO/SSAO, CAS, and ZachFix Hot Apply were validated together. DXVK + ReShade depth/MXAO has also been validated.

Only one active D3D9 proxy should normally be named `d3d9.dll` beside `DP.exe`. Keeping inactive backup copies under names such as `D3D9.dll.dgvd` or `d3d9.dll.dpfix` is fine.

## Installation

ZachFix is an ASI plugin. An ASI loader is required. The current tested deployment uses **Ultimate ASI Loader** via `winmm.dll`, but the loader is not bundled with ZachFix.

A typical installation looks like this:

```text
Deadly Premonition The Director's Cut/
|-- DP.exe
|-- ZachFix.ini
|-- winmm.dll                 <- Ultimate ASI Loader
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

## Controller support with XiDi

ZachFix's tested ASI-loader setup uses `winmm.dll`, which conflicts with XiDi when XiDi also uses its default `winmm.dll` proxy name.

The XiDi layout I use is:

```text
Deadly Premonition The Director's Cut/
|-- DP.exe
|-- winmm.dll                 <- Ultimate ASI Loader
|-- winmmHooked.dll           <- XiDi proxy renamed from winmm.dll
|-- Xidi.32.dll
|-- Xidi.ini
|-- ZachFix.ini
|-- scripts/
|   `-- ZachFix.asi
`-- ZachFix/
```

This allows Ultimate ASI Loader and XiDi to coexist in the tested Steam setup. If you use another XiDi/loader configuration, make sure the renamed XiDi proxy is actually being chain-loaded by your setup.

## Configuration

The main configuration file is `ZachFix.ini` beside `DP.exe`. The F10 UI can edit, apply, save, and reload supported values.

Important sections:

- `[Display]`: output resolution and borderless mode.
- `[Rendering]`: internal resolution / scale and pixel-offset correction.
- `[Shadows]`: shadow resolution scale and optional D32F precision correction.
- `[Reflections]`: reflection resolution scale.
- `[DepthOfField]`: higher-resolution legacy DoF and optional additional softening.
- `[World]`: original or extended high-detail streaming grid.
- `[Filtering]`: Original, Bilinear, or smart Anisotropic filtering.
- `[Textures]`: overrides, NPOT dimension behavior, Developer Mode, and dumping.
- `[UI]`: UI enable state, toggle key, and optional gameplay tuning pause.
- `[PostFX.AO]`: GTAO-lite mode and quality controls.
- `[PostFX.Bloom]`: legacy or Bloom NG controls.
- `[PostFX.DoF]`: legacy or DoF NG controls.
- `[PostFX.Exposure]`: auto exposure and highlight shoulder controls.

### Hot Apply

Hot Apply does not reset the D3D9 device. The game keeps its original logical COM resources while ZachFix creates and substitutes replacement backing resources where needed.

The **Diagnostics** tab contains the Runtime Resource Audit. In a steady state, the useful invariant is:

```text
Created - Released == Active replacements
```

When no replacement backing resources are needed, `Outstanding` should return to `0`.

### Gameplay pause and PostFX Preview Freeze

`PauseGameWhileOpen` is a **gameplay-only** timer freeze intended for tuning during normal gameplay. Some cutscenes can hang if their timers are frozen, so it is disabled by default.

```ini
[UI]
PauseGameWhileOpen = false
```

The pause state is latched when the F10 panel opens. Apply/Reload will not suddenly change the pause state in the middle of an already-open F10 session.

For cutscenes and fine PostFX tuning, use **Freeze PostFX Preview** instead. It captures the relevant HDR/G-buffer inputs and keeps re-running the ZachFix PostFX stack against the same frame without pausing the game simulation.

This is useful for tuning:

- ambient occlusion,
- Bloom NG,
- DoF NG,
- exposure,
- highlight shoulder/tone shaping.

Geometry-dependent settings such as internal resolution, shadow scale, reflection scale, and world streaming still require live game rendering to see their effect.

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

DoF NG is a separate PostFX path exposed in the PostFX page and `[PostFX.DoF]` configuration section.

## Optional game tweaks

These are executable/game tweaks I use alongside ZachFix. They are not required by the mod and are not applied automatically.

### Skip intro videos

On the tested Steam executable, the startup logos/videos can be skipped with a one-byte edit.

1. Back up `DP.exe`.
2. Open it in a hex editor.
3. Go to **file offset `0x243333`**.
4. Change:

   ```text
   B3 -> 00
   ```

5. Save the executable.

Do not apply this offset blindly to a different executable/version. This tweak changes the executable hash.

## Troubleshooting

### The game crashes even without ZachFix

If the game also crashes when ZachFix/ASI loading is removed, start with the base game rather than the mod:

- Confirm that you are using the Steam Director's Cut executable targeted by current development.
- Check `d3dx9_43.dll` as described above.
- Try the official DirectX End-User Runtimes (June 2010), but remember that reinstalling it did not replace an already-present problematic DLL in my own case.
- If native D3D9 still fails, DXVK is a useful compatibility path and is actively tested with ZachFix.

### The game starts with DXVK, but F10 does nothing

Check whether ZachFix is loading at all:

- `winmm.dll` should be the ASI loader in the game directory.
- `ZachFix.asi` should normally be under `scripts\` in the documented deployment.
- Check for the latest `ZachFix*.log`/ZachFix log output.
- Include the DXVK version and a listing/screenshot of the directory containing `DP.exe` when reporting the problem.

Recent development builds also log a **Binary ID** for the actually loaded `ZachFix.asi`, which makes it easier to confirm that the intended build is running.

### F10 pause hangs a cutscene

This is a known limitation of the gameplay timer freeze. Disable `PauseGameWhileOpen` for cutscenes and use **Freeze PostFX Preview** for visual tuning instead.

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

The package mirrors the documented runtime layout with `scripts/ZachFix.asi`, plus the default INI, documentation, GPLv3 license, and third-party license notices. Development headers, import/static libraries, and dependency CMake package files are not included.

## Known limitations

- ZachFix targets the 32-bit Steam Director's Cut executable. It is not a generic D3D9 injector for arbitrary games.
- Some fixes depend on Deadly Premonition-specific resource dimensions, shaders, and render behavior.
- Texture Developer Mode requires a restart to change its ownership model.
- World-detail changes become fully visible as streaming cells transition.
- The additional legacy DoF blur is a lightweight compatibility-oriented approximation, not a byte-for-byte recreation of original DPFix's Gaussian implementation.
- The gameplay timer pause is not safe in every cutscene.
- Repeated Alt-Tab recovery in true D3D9 exclusive fullscreen can leave the game on a black screen; borderless windowed mode is recommended for reliable task switching.
- Native D3D9, DXVK, and dgVoodoo2 are supported paths, but external wrappers, loaders, controller proxies, and overlays can still conflict with one another independently of ZachFix.

## Credits

- **Peter Thoman (Durante)** for the original DPFix/DSFix work and rendering research that made this project possible. ZachFix retains/adapts several DPFix compatibility fixes and preserves attribution in the relevant source paths.
- **Tsuda Kageyu and contributors** for MinHook.
- **Omar Cornut and contributors** for Dear ImGui.
- **Paul Hsieh** for SuperFastHash.
- **DXVK**, **ReShade**, **dgVoodoo2**, and **Ultimate ASI Loader / ThirteenAG** for the modern compatibility/modding ecosystem used alongside ZachFix.
- **XiDi** for the controller compatibility layer used in the tested game setup.
- The Deadly Premonition modding community and testers for compatibility findings and validation.

See [THIRD_PARTY.md](THIRD_PARTY.md) for dependency and license details.

## License

ZachFix is distributed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE).

This project is unofficial and is not affiliated with Access Games, SWERY, Rising Star Games, Marvelous, or the publishers/rightsholders of Deadly Premonition.
