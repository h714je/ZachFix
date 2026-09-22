# ZachFix

**ZachFix** is an unofficial continuation and modernization of Peter Thoman's original **DPFix** for **Deadly Premonition: The Director's Cut** on Windows.

It is a game-specific Direct3D 9 fix, restoration, and compatibility layer for the original PC executable. ZachFix works with native Windows D3D9 and can also run alongside DXVK or dgVoodoo2.

> "Zach, do you see this?"

## Maintenance mode

ZachFix is currently feature-complete and in maintenance mode. No new features are planned. Future updates will focus only on reproducible ZachFix bugs and regressions.

Bug reports are still welcome, but fixes may require a reliable reproduction case.

## Current release

**v0.2.1** is the current release.

ZachFix currently supports the 32-bit Steam 1.01b and GOG 1.01b executables. The same `ZachFix.asi` is used for both and the build is detected automatically.

| Build | PE TimeDateStamp | SizeOfImage |
| --- | ---: | ---: |
| Steam 1.01b | `0x529721DC` | `0x010B5000` |
| GOG 1.01b | `0x52970AF6` | `0x010B5000` |

Unknown executables are logged and build-specific fixes fail closed instead of applying known offsets blindly.

## Highlights

### Rendering and display

- Native monitor resolution and borderless display handling.
- Arbitrary internal rendering resolution and supersampling.
- Higher-resolution shadows, reflections, and legacy depth of field.
- Original DPFix shadow-depth precision and pixel-offset compatibility fixes.
- Smart bilinear or anisotropic texture filtering.
- DPFix-compatible texture replacement with an optional live texture-authoring mode.
- Optional PostFX stack with ambient occlusion, bloom, depth of field, auto exposure, and highlight shoulder shaping.
- Optional Xbox 360 ENV tone/color restoration and HDTV/BT.709 display-transfer emulation.
- Native D3D9, DXVK, and dgVoodoo2 support.

### Game restoration and world fixes

- Original Easy / Normal / Hard New Game selector restored.
- Building day/night behavior restored for the Director's Cut `HOUSE_LIST.NOD` regression.
- Interior visibility-volume fix for props incorrectly disappearing near walls/mirrors.
- Independent high-detail streaming, main-frustum visibility, object activation, and native mesh LOD distance controls.
- Transactional Save Safety with validated writes and compressed rolling backups.

### Input and UI

- Optional native XInput backend while keeping DP's existing action/binding system.
- PC and Xbox 360 stick/aim profiles.
- Analog vehicle LT/RT controls.
- Restored native two-channel vibration.
- Automatic keyboard/mouse and controller switching.
- Runtime keyboard/gamepad glyph themes and hot reload.
- F10 configuration UI with Graphics, PostFX, Gamepad, Diagnostics, and About tabs.

### Diagnostics

- Runtime resource counters for Hot Apply.
- Texture inspector when Texture Developer Mode is active.
- Optional gameplay tuning pause and frozen PostFX preview.

## Installation

ZachFix is an ASI plugin. An ASI loader is required. The tested deployment uses **Ultimate ASI Loader** as `winmm.dll`.

Typical layout:

```text
Deadly Premonition The Director's Cut/
|-- DP.exe
|-- ZachFix.ini
|-- winmm.dll
|-- scripts/
|   `-- ZachFix.asi
`-- ZachFix/
    |-- glyphs/
    `-- textures/
```

Press **F10** in game to open the ZachFix settings UI.

For the complete installation guide, backend layouts, Large Address Aware recommendation, and DirectX troubleshooting, see [docs/installation.md](docs/installation.md).

## Renderer backends

| Backend | ZachFix | F10 UI | Hot Apply |
| --- | --- | --- | --- |
| Native Windows D3D9 | Yes | Yes | Yes |
| DXVK | Yes | Yes | Yes |
| dgVoodoo2 | Yes | Yes | Yes |

Only one active graphics-wrapper `d3d9.dll` should normally exist beside `DP.exe`.

## Configuration

The main configuration file is `ZachFix.ini` beside `DP.exe`. F10 can edit, apply, save, and reload the settings that support runtime changes.

Some settings intentionally require a restart, especially options that determine resource ownership or formats at creation time.

See:

- [Configuration reference](docs/configuration.md)
- [Rendering and world settings](docs/rendering.md)
- [PostFX](docs/postfx.md)
- [Gamepad, input, and glyphs](docs/input.md)
- [Textures and texture packs](docs/textures.md)
- [Gameplay and restoration fixes](docs/gameplay.md)
- [Save Safety](docs/save-safety.md)
- [Diagnostics](docs/diagnostics.md)

## Documentation

The full documentation is indexed at [docs/README.md](docs/README.md).

Useful entry points:

- [Installation and backend setup](docs/installation.md)
- [Supported game builds](docs/supported-builds.md)
- [Configuration reference](docs/configuration.md)
- [Troubleshooting](docs/troubleshooting.md)
- [Building ZachFix](docs/development.md)

## Building

Requirements:

- Windows
- Visual Studio 2022 with C++ desktop tools
- CMake 3.20+
- x86 / Win32 target

```powershell
cmake -S . -B build -A Win32
cmake --build build --config Release
cpack --config build\CPackConfig.cmake -C Release
```

The built module is `ZachFix.asi` and the packaged release is `ZachFix-v<version>-win32.zip`.

See [docs/development.md](docs/development.md) for the complete build/package notes.

## Credits

- **Peter Thoman (Durante)** for the original DPFix/DSFix work and rendering research.
- **Tsuda Kageyu and contributors** for MinHook.
- **Omar Cornut and contributors** for Dear ImGui.
- **Paul Hsieh** for SuperFastHash.
- **DXVK**, **ReShade**, **dgVoodoo2**, and **Ultimate ASI Loader / ThirteenAG** for the surrounding compatibility/modding ecosystem.
- The Deadly Premonition modding community and testers.

See [THIRD_PARTY.md](THIRD_PARTY.md) for dependency and license details.

## License

ZachFix is distributed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE).

This project is unofficial and is not affiliated with Access Games, SWERY, Rising Star Games, Marvelous, or the publishers/rightsholders of Deadly Premonition.
