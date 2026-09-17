# ZachFix

**ZachFix** is an unofficial continuation and modernization of Peter Thoman's original **DPFix** for **Deadly Premonition: The Director's Cut** on Windows.

It is a game-specific Direct3D 9 fix layer. ZachFix does **not** require a particular graphics wrapper: it can run on native D3D9, DXVK, or dgVoodoo2.

> "Zach, do you see this?"

## Status

`v0.1.1-rc1` is the current release candidate. It carries the native Easy / Normal / Hard difficulty restoration and per-difficulty save profiles introduced in `v0.1.0-rc10`, with no gameplay or compatibility changes relative to RC10. The new `0.1.1` prerelease line keeps subsequent release ordering unambiguous while retaining the controller-fidelity, vibration, glyph, Steam/GOG compatibility, save-safety, rendering, PostFX, and tuning work from earlier release candidates.

### Supported game builds

ZachFix currently supports the 32-bit (x86) **Steam 1.01b** and **GOG 1.01b** executables of *Deadly Premonition: The Director's Cut*. The same `ZachFix.asi` is used for both; the executable is detected automatically from its PE identity, so there is no Steam/GOG switch in `ZachFix.ini`.

| Build | PE TimeDateStamp | SizeOfImage | Status |
| --- | ---: | ---: | --- |
| Steam 1.01b | `0x529721DC` | `0x010B5000` | Supported |
| GOG 1.01b | `0x52970AF6` | `0x010B5000` | Supported |

Build-specific hooks first select the matching profile and then verify the expected code signature at the selected address before patching. An unknown executable is reported in `ZachFix.log`, and build-specific hooks fail closed rather than applying Steam/GOG offsets blindly.

Current development/testing is primarily on Windows 11. Common one-byte/header tweaks such as the optional No Intro and Large Address Aware changes do not change the executable layout used by ZachFix, but always keep a backup of `DP.exe` before modifying it.

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
- Native XInput controller backend with hot-applicable PC/Xbox 360 input profiles, analog vehicle triggers, native two-channel vibration, vanilla DP action/binding compatibility, and automatic keyboard/controller switching.
- Runtime keyboard/gamepad glyph themes with F10 selection and hot reload.
- F10 Dear ImGui configuration UI with separate Graphics, PostFX, Diagnostics, and About pages.
- Hot Apply for render-resource settings without `IDirect3DDevice9::Reset`.
- Runtime replacement-resource audit for hot-apply lifetime validation.
- Wrapper-independent D3D9 hook path with native-D3D9 UI fallbacks.
- PostFX stack with GTAO-lite ambient occlusion, adaptive exposure, HDR bloom, DoF NG, and highlight shoulder/tone shaping.
- Frozen PostFX Preview for tuning the PostFX stack against one captured frame while the game continues running underneath.
- Optional gameplay-only F10 timer pause for normal gameplay tuning.
- Native Easy / Normal / Hard difficulty restoration through a startup command-line switch, with separate per-difficulty save profiles and read-only F10 status.
- Transactional save protection with validated temporary writes, compressed rolling backups, and rejected-save diagnostic bundles.

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

With dgVoodoo2 + ReShade DXGI, DisplayDepth, MXAO/SSAO, CAS, and ZachFix Hot Apply were validated together. DXVK + ReShade depth/MXAO has also been validated. The current Steam/GOG build-selection path has been exercised successfully with both DXVK and dgVoodoo2.

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
    |-- textures/
    |   |-- override/
    |   `-- dump/
    `-- glyphs/
        |-- keyboard/
        `-- gamepad/
```

The release package includes the `ZachFix/glyphs/keyboard` theme directory, bundled `xbox`, `playstation`, `steamdeck`, and `switch` gamepad themes, plus `ZachFix/glyphs/README.md` with the known atlas layout for custom theme authors. Texture working directories are created automatically when their subsystem initializes, and missing glyph directories are also recreated at runtime.

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

## Controller support

ZachFix can use XInput directly while keeping Deadly Premonition's vanilla controller action/binding system. No XiDi installation or controller proxy is required for this path.

```ini
[Gamepad]
NativeXInput = true
InputProfile = Xbox360
AnalogVehicleTriggers = true
VehicleTriggerDeadzone = 30
Vibration = true
VibrationStrength = 1.0

[Input]
AutoSwitch = true
```

With `NativeXInput = true`, ZachFix reads `XInputGetState`, exposes a compatibility `JOYINFOEX` layout to the game, and translates DP's legacy trigger/right-stick binding codes at the common controller evaluator. `NativeXInput = false` leaves the vanilla WinMM controller path and its binding semantics untouched.

`InputProfile` hot-applies from F10. `PC` keeps the Director's Cut stick evaluator, secondary `+/-16 / 109` filtering, `0.5` slew, and PC aim shaping. `Xbox360` keeps the Director's Cut control routing, including aiming on the **right stick**, but restores the proven Xbox 360 stick normalization (`7864` raw deadzone, exact `0x38286CF7` scale), bypasses the confirmed PC secondary stick filter, and applies the original Xbox aim shaping (`0.25` deadzone followed by `4/3` renormalization) only at the confirmed live-aim consumers.

For LT/RT action bindings, the `Xbox360` profile also matches the original digital trigger threshold: raw values `0..30` are released and `31..255` are pressed. This is separate from the continuous trigger floats used by the optional analog vehicle path.

`AnalogVehicleTriggers` is a separate hot-applied option. When enabled, ZachFix restores all three confirmed Xbox 360 LT/RT vehicle consumers. `VehicleTriggerDeadzone` is in raw XInput trigger counts; the Xbox default is `30`, with `raw <= deadzone -> 0` and surviving values left as `raw / 255` without post-deadzone renormalization. Disabling the option immediately routes those consumers back through the vanilla PC digital throttle/brake path.

The PC executable still contains Deadly Premonition's original two-channel vibration logic, including its event timing, duration/countdown, channel balance and automatic stop behavior, but its final actuator output is disabled. With `Vibration = true`, ZachFix re-enables that surviving path and forwards the native motor state to `XInputSetState`. The PC port truncates actuator B with `>> 6` immediately before storing it; ZachFix now pairs each `CRdInput::SetActuator` command synchronously and publishes the original full 16-bit A/B amplitudes only after the native setters finish, so the truncated intermediate state is never sent to XInput. DP's native countdown/expiry path still owns the stop timing. `VibrationStrength` is a global `0.0` to `1.0` gain applied after DP chooses the two motor amplitudes. Both vibration controls hot-apply from F10: disabling vibration or setting strength to `0.0` stops the motors immediately, and changing strength during an active effect reapplies the current native state without restarting the game.

`AutoSwitch` is independent of the controller backend. When enabled, keyboard/mouse activity selects DP's native keyboard/mouse mode and controller activity selects its native controller mode. Set both `NativeXInput = false` and `AutoSwitch = false` if you want ZachFix to leave input behavior completely vanilla.

### Controller rebinding via `configJ.cnf`

Deadly Premonition already stores its controller action bindings in `configJ.cnf` beside `DP.exe`, so ZachFix intentionally does not add a second rebinding database. Close the game, make a backup of the file, edit the numeric values, then start the game again. DPLauncher can rewrite this file when saving controller settings, so make manual edits after using the launcher.

The stock file contains these controller actions:

```ini
[SETTING]
    RELOAD = 1
    OBSERVE = 3
    LIGHTONOFF = 2
    INTERACT = 0
    AIM = 49
    HOLDBREATH = 5
    RUN = 4
    ATTACK = 50
    USEJOY = 1
[END]
```

`USEJOY = 1` starts DP in controller mode and `USEJOY = 0` starts it in keyboard/mouse mode. With `AutoSwitch = true`, this is only the initial mode because ZachFix updates the same vanilla mode flag at runtime.

When `NativeXInput = true`, use the following binding values:

| Value | XInput control |
|---:|---|
| `0` | A |
| `1` | B |
| `2` | X |
| `3` | Y |
| `4` | LB |
| `5` | RB |
| `6` | Back / View |
| `7` | Start / Menu |
| `8` | Left Stick Click |
| `9` | Right Stick Click |
| `41` | D-pad Up |
| `42` | D-pad Down |
| `43` | D-pad Left |
| `44` | D-pad Right |
| `45` | Left Stick Left |
| `46` | Left Stick Right |
| `47` | Left Stick Up |
| `48` | Left Stick Down |
| `49` | RT |
| `50` | LT |
| `51` | Right Stick Left |
| `52` | Right Stick Right |
| `55` | Right Stick Up |
| `56` | Right Stick Down |

Values `49` and `50` deliberately preserve DP's original action semantics: the launcher/game treats `49` as RT and `50` as LT, while ZachFix maps those meanings onto the separate XInput trigger axes internally. The same applies to the right-stick codes, so `configJ.cnf` stays compatible with the game's own binding model instead of exposing ZachFix's internal synthetic axis layout.

Values `10` through `31` are not mapped to physical XInput buttons by ZachFix, and `53`/`54` are internal legacy V-axis directions rather than useful user-facing bindings in the native XInput layout. Prefer the table above.

When `NativeXInput = false`, ZachFix does not reinterpret these bindings. DP uses its original WinMM/controller semantics exactly as before.

### Glyph themes

ZachFix can switch keyboard and controller glyph atlases together with DP's native input mode:

```ini
[Glyphs]
DynamicAtlas = true
HotReload = true
KeyboardSet = Native
GamepadSet = xbox
```

Theme names are ordinary file stems:

```text
ZachFix/
`-- glyphs/
    |-- keyboard/
    |   |-- redseed.png
    |   `-- minimal.dds
    `-- gamepad/
        |-- xbox.png
        |-- playstation.png
        |-- steamdeck.png
        `-- switch.png
```

For example, `KeyboardSet = redseed` resolves `glyphs/keyboard/redseed.dds`, `.png`, or `.tga`; `GamepadSet = playstation` does the same under `glyphs/gamepad`. The load priority is DDS, PNG, then TGA. `Native` selects the captured original DP atlas when available, with the glyph subsystem's own fallback handling if DP did not create that atlas during the current run.

The bundled gamepad themes use CC0 Kenney Input Prompts artwork and are fitted to DP's existing atlas UV layout. `ZachFix/glyphs/README.md` documents the currently mapped controller regions and authoring dimensions.

The F10 UI discovers theme files dynamically, switches theme names at runtime, and can save the selected names back to `ZachFix.ini`. With `HotReload = true`, editing or replacing the active theme file is picked up without restarting the game. Enabling or disabling `DynamicAtlas` itself requires a restart because that choice determines glyph-texture ownership when DP first loads its atlases. While `DynamicAtlas = true`, the two DP glyph atlases are isolated from the generic texture-override path so the two systems cannot fight over the same texture.

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
- `[Gamepad]`: native XInput backend, hot-applicable PC/Xbox 360 input profile, independent analog vehicle triggers/deadzone, and native vibration/strength.
- `[Input]`: automatic keyboard/mouse vs controller mode switching.
- Difficulty: startup-only native Easy / Normal / Hard selection via `-zachfix-difficulty=0/1/2`, with separate save profiles.
- `[SaveSafety]`: transactional protection and rolling backups for the active difficulty save profile.
- `[Glyphs]`: dynamic keyboard/controller glyph themes and hot reload.
- `[UI]`: UI enable state, toggle key, and optional gameplay tuning pause.
- `[PostFX.AO]`: GTAO-lite mode and quality controls.
- `[PostFX.Bloom]`: legacy or Bloom NG controls.
- `[PostFX.DoF]`: legacy or DoF NG controls.
- `[PostFX.Exposure]`: auto exposure and highlight shoulder controls.

### Difficulty profiles

Director's Cut still contains the original Easy / Normal / Hard state and difficulty-dependent combat tables, but no longer exposes the selector. ZachFix restores the native state from the process command line:

```text
-zachfix-difficulty=0   Easy
-zachfix-difficulty=1   Normal
-zachfix-difficulty=2   Hard
```

If the switch is omitted or invalid, Easy is used. Difficulty is read-only in F10 and cannot be changed while the process is running. No custom balance coefficients are introduced.

Each mode uses an independent physical save profile while the game still sees its vanilla `savedata\dp.sav` path:

```text
savedata\easy\dp.sav
savedata\normal\dp.sav
savedata\hard\dp.sav
```

Any `dp.sav` can be copied manually into any profile. If its stored difficulty differs from the current session, ZachFix accepts it, logs the mismatch, applies the command-line difficulty to the live record, and performs no persistent-world migration. The next normal save persists the current session difficulty. This intentionally allows progress transfers and experimental mixed-state saves.

For compatibility, when the Easy profile does not yet exist but a legacy `savedata\dp.sav` does, ZachFix copies that file to `savedata\easy\dp.sav` once and leaves the original untouched. Normal and Hard never auto-import the legacy save.

### Save Safety

Deadly Premonition overwrites its logical `savedata\dp.sav` directly. ZachFix first routes that path into the active difficulty profile, and with Save Safety enabled redirects the destructive open to `dp.sav.zachtmp` in that profile directory. DP writes its normal vanilla save bytes to the temp file; ZachFix flushes it, reads it back, runs conservative structural checks, backs up the previous live save, then replaces `dp.sav` with a write-through move.

```ini
[SaveSafety]
Enabled = true
BackupCount = 10
```

Backups are timestamped standard ZIP archives under `ZachFix\save_backups\<difficulty>\dp.sav\`. Each `dp_<timestamp>.zip` contains a directly restorable `dp.sav`; users can open it with Windows Explorer, 7-Zip, WinRAR, or any ordinary ZIP tool and extract `dp.sav` back into the desired difficulty profile directory. ZachFix verifies each completed archive by reopening and fully validating its compressed data/CRC before the live save may be replaced. The oldest backup archives are pruned after a successful backup; legacy uncompressed `dp_*.sav` backups from earlier builds are included in the same rotation. ZachFix never edits fields inside the save. If flushing, read-back, validation, backup creation/compression, or the final replace fails, the previous live `dp.sav` is left untouched and the failure is logged.

Rejected saves produce a self-contained `ZachFix\save_backups\<difficulty>\dp.sav\failed\failure_<timestamp>.zip`. The archive contains `before.sav` (the previous live save, when one exists), `failed.sav` (the rejected transactional save), `ZachFix.log`, and `reason.txt`. This preserves the before/after evidence needed to investigate or potentially repair repeatable save-state failures while avoiding two extra 8 MB raw save copies. If ZIP creation or verification itself fails, ZachFix deliberately falls back to retaining the raw failure directory and original `dp.sav.zachtmp` rather than risking loss of diagnostic evidence. Successfully archived rejected temps are removed. Failed bundles rotate independently from normal backups and use the same `BackupCount` limit.

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

- Confirm that you are using one of the supported Steam 1.01b or GOG 1.01b executables.
- Check the first lines of `ZachFix.log` for `[Build] Detected DP.exe: Steam 1.01b` or `[Build] Detected DP.exe: GOG 1.01b`.
- Check `d3dx9_43.dll` as described above.
- Try the official DirectX End-User Runtimes (June 2010), but remember that reinstalling it did not replace an already-present problematic DLL in my own case.
- If native D3D9 still fails, DXVK is a useful compatibility path and is actively tested with ZachFix.

### The game starts with DXVK, but F10 does nothing

Check whether ZachFix is loading at all:

- `winmm.dll` should be the ASI loader in the game directory.
- `ZachFix.asi` should normally be under `scripts\` in the documented deployment.
- Check for the latest `ZachFix*.log`/ZachFix log output.
- Include the DXVK version and a listing/screenshot of the directory containing `DP.exe` when reporting the problem.

ZachFix logs a **Binary ID** for the actually loaded `ZachFix.asi`, which makes it easier to confirm that the intended build is running.

### ZachFix reports an unsupported `DP.exe`

Do not force a Steam/GOG profile or copy executable-specific offsets from another build. Keep the unknown `DP.exe` intact and include the opening `[Build]` lines from `ZachFix.log` when reporting it. Unsupported build-specific hooks are deliberately left disabled until that executable has been mapped and its signatures verified.

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

The package mirrors the documented runtime layout with `scripts/ZachFix.asi`, the `ZachFix/glyphs` theme skeleton and its README, plus the default INI, documentation, GPLv3 license, and third-party license notices. Development headers, import/static libraries, and dependency CMake package files are not included.

## Known limitations

- ZachFix targets the supported 32-bit Steam 1.01b and GOG 1.01b Director's Cut executables. It is not a generic D3D9 injector for arbitrary games.
- Some fixes depend on Deadly Premonition-specific resource dimensions, shaders, and render behavior.
- Texture Developer Mode requires a restart to change its ownership model.
- Dynamic Glyph Atlas requires a restart when enabling or disabling the glyph-texture ownership model; selecting/editing themes remains live once enabled.
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
- The Deadly Premonition modding community and testers for compatibility findings and validation.

See [THIRD_PARTY.md](THIRD_PARTY.md) for dependency and license details.

## License

ZachFix is distributed under the **GNU General Public License v3.0**. See [LICENSE](LICENSE).

This project is unofficial and is not affiliated with Access Games, SWERY, Rising Star Games, Marvelous, or the publishers/rightsholders of Deadly Premonition.
