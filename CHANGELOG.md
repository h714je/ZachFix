# Changelog

## Unreleased

### World object activation distance

- Added a production `World.ObjectActivationDistanceScale = 1 | 2` option for the confirmed native per-object active-list distance gate: `1` preserves the original 1000-unit radius and `2` extends it to 2000 units to reduce visible world-prop pop-in.
- The implementation redirects only the active-list builder's threshold-load operand to private ZachFix storage; the shared DP.exe constant, streaming cell arrays, spatial tree, and normal renderer pipeline remain untouched.
- Added reversible F10 hot apply and build-profiled Steam 1.01b / GOG 1.01b instruction validation.

## v0.1.1-rc1

Release-line rollover after `v0.1.0-rc10`. There are no gameplay or compatibility changes relative to RC10; this release carries the same native-difficulty restoration and per-difficulty save-profile implementation under the new `0.1.1` prerelease line.

Steam 1.01b and GOG 1.01b were both rechecked successfully before the rollover.

## v0.1.0-rc10

Native-difficulty restoration and per-difficulty save-profile release candidate.

### Native difficulty restoration and save profiles

- Restored the surviving native Easy / Normal / Hard state through `-zachfix-difficulty=0/1/2`; no custom difficulty coefficients are used.
- Enforced the selected native difficulty at the confirmed Director's Cut reset writes and full live-record restore copies so loaded records cannot silently return gameplay to Easy.
- Added separate physical save profiles at `savedata\easy\dp.sav`, `savedata\normal\dp.sav`, and `savedata\hard\dp.sav`; F10 reports the active difficulty/profile read-only.
- Foreign/mismatched saves are accepted without persistent-state migration. The session difficulty wins in live state and the next normal save persists it, allowing deliberate progress transfers between profiles.
- Added one-way compatibility import for legacy `savedata\dp.sav` into the Easy profile only; the original legacy file is never moved or overwritten by the import.
- Namespaced SaveSafety backups and failure bundles by difficulty under `ZachFix\save_backups\<difficulty>\dp.sav\`.

## v0.1.0-rc9

Controller-fidelity and native-vibration focused release candidate.

### Gamepad profile production cleanup

- Replaced the temporary stick/aim research behavior with a hot-applicable `Gamepad.InputProfile = PC | Xbox360` runtime switch.
- `PC` now cleanly preserves Director's Cut stick preprocessing and aim behavior, while `Xbox360` restores the confirmed Xbox stick deadzone/scale, bypasses the confirmed PC secondary `+/-16 / 109` plus `0.5` slew layer, and applies the confirmed `0.25` plus `4/3` aim shaping without moving aiming back to the left stick.
- Added independent hot-applicable `Gamepad.AnalogVehicleTriggers`; the production path uses all three confirmed Xbox LT/RT vehicle consumers and falls back to vanilla PC digital throttle/brake when disabled.
- Kept `Gamepad.VehicleTriggerDeadzone = 30` live and preserved the original Xbox semantics: values at or below the threshold are zero, while values above it remain `raw / 255` without renormalization.
- Matched the Xbox 360 digital LT/RT press threshold for ordinary actions as well: in the `Xbox360` profile, raw trigger values `> 30` are exposed to the PC action evaluator as a pressed digital axis while vehicle consumers still use the independent continuous `raw / 255` floats.
- Made the live vehicle trigger deadzone thread-safe by moving the hook-side read to an atomic runtime value, and prevented Xbox aim shaping from running if the exact-stick restoration hook failed to install.
- Removed the gamepad-specific consumer radar, downstream vehicle pipeline logging, PhysX scalar/torque probes, wheel-loop probes, live setter traces, and the obsolete v10 torque-scaling experiment from the production Native XInput path.

### Native vibration restoration

- Restored Deadly Premonition's surviving native two-channel vibration path through ZachFix Native XInput instead of inventing per-event rumble.
- Re-enabled the dormant `CRdInput` actuator gate while preserving DP's original motor amplitudes, duration/countdown, channel balance, and automatic stop behavior.
- Added build-profiled, signature-gated Steam 1.01b and GOG 1.01b vibration hooks; unsupported/mismatched builds fail closed without disabling controller input.
- Added `Gamepad.Vibration = true` and `Gamepad.VibrationStrength = 1.0`; both are hot-applicable from F10.
- Disabling vibration or moving strength to `0.0` stops the current motors immediately; changing strength during an active effect reapplies the current native actuator state.
- Added safe motor stop on XInput controller disconnect and controller-index changes.
- Preserved the original 16-bit second-actuator value before the PC port's `>> 6` truncation. The bridge now pairs the synchronous `CRdInput::SetActuator` command directly: its inner truncated `SetSecond` update is suppressed, then the exact original 16-bit A/B pair is published after the native call returns. DP's separate countdown/expiry caller continues to publish the final stop state.

### Glyph themes

- Rebuilt the bundled Xbox and PlayStation controller glyph themes from CC0 Kenney Input Prompts artwork using the validated Deadly Premonition gamepad-atlas layout.
- Added bundled Steam Deck and Nintendo Switch controller themes.
- Standardized visible controller-glyph sizing to nearly fill each 128x128 UV cell while retaining a small transparent padding margin.
- Corrected Nintendo A/B and X/Y face-button labels for Nintendo's physical button layout while preserving DP's semantic action slots.
- Documented the currently mapped 1024x1024 gamepad atlas regions for custom theme authors.
- Added the Kenney Input Prompts provenance/license notice without bundling the full upstream asset source pack.


## v0.1.0-rc8

Executable-compatibility focused release candidate.

### Game build compatibility

- Added first-class support for both the Steam 1.01b and GOG 1.01b `DP.exe` builds from the same `ZachFix.asi`; no Steam/GOG switch is required in `ZachFix.ini`.
- Added centralized `DpBuildProfile` detection using the executable PE `TimeDateStamp` plus `SizeOfImage`, with build-specific RVAs kept in one table instead of duplicated across hook implementations.
- Mapped and validated the GOG 1.01b addresses used by the early D3D9 startup hook, zero-delta stability fix, Native XInput, automatic input switching, and runtime world-detail hook.
- Kept per-hook instruction-signature validation after build detection so a matching profile still fails closed if the expected code bytes are not present.
- Kept the early `Direct3DCreate9` IAT hook loader-lock safe: build detection reads only the already-mapped PE image and performs no config/file I/O, logging, allocation, or synchronization from `DllMain`.
- Unknown `DP.exe` builds are reported in `ZachFix.log`; build-specific hooks remain disabled instead of applying Steam/GOG offsets blindly.

### Stability and diagnostics

- Aggregated repeated zero-delta `0/0` prevention messages so sustained bursts no longer flood `ZachFix.log`; the first hit, active batches, and the final quiet-period tail remain visible.
- The zero-delta fix itself is unchanged and still sanitizes only the confirmed exact `distance == 0 && frameDelta == 0` case.
- Confirmed the GOG 1.01b zero-delta guard at runtime, including repeated real gameplay hits under dgVoodoo2.

### Compatibility

- Exercised the Steam/GOG build-selection path with DXVK and dgVoodoo2.
- Preserved the existing native D3D9, DXVK, dgVoodoo2, Save Safety, PostFX, input, glyph, and rendering behavior while adding the second executable profile.

## v0.1.0-rc7

Save-safety and runtime-hardening focused release candidate.

### Save safety

- Replaced the experimental pre-overwrite backup with transactional `dp.sav` writes through a same-directory `dp.sav.zachtmp` file.
- Flushes and reads the completed temp save back before commit, then validates exact size plus conservative header/live mirrors, playtime, Trading Card, and Side Mission invariants.
- Creates the rolling backup only after the temp save passes validation and before the final write-through replace.
- Preserves the previous live `dp.sav` when temp creation, flush, read-back, validation, backup creation, or final replace fails.
- Enables transactional redirection only after the full save-I/O hook set is installed, avoiding partial-hook startup races.
- Captures rejected save attempts into diagnostic failure bundles containing the previous live save, rejected temp save, current ZachFix log, and a short reason file; failed bundles rotate separately using `BackupCount`.
- Stores normal rolling backups as standard Deflate ZIP archives containing a directly restorable `dp.sav`; every archive is fully reopened and CRC-validated before the transaction may commit.
- Stores rejected-save diagnostics as one `failure_<timestamp>.zip` containing `before.sav`, `failed.sav`, `ZachFix.log`, and `reason.txt`; ZIP failure falls back to the raw diagnostic directory/temp instead of discarding evidence.
- Serializes the full destructive-save transaction so overlapping `dp.sav` writes cannot reuse/truncate the shared temp file while validation, backup, or commit is still in progress.
- Fails closed if a transactional save handle cannot be registered for tracking, and keeps the transaction gate locked if emergency cleanup itself fails.
- Flushes each verified backup ZIP to disk before allowing the live-save replacement to proceed.

### Runtime hardening

- Serialized ZachFix log writes and failure-bundle log snapshots so hooks from different threads cannot interleave or race the copied diagnostic log.
- Made lazy `DP.exe` image metadata publication thread-safe for hook subsystems that can query it concurrently.
- Hardened the live world-detail switch by serializing hot applies and changing only the single low byte of the validated `0/1` instruction immediate instead of an unaligned 32-bit executable-code write.

## v0.1.0-rc6

Controller/input and glyph-theme focused release candidate.

### Controller and input

- Added an optional native XInput backend that feeds DP an XInput-backed compatibility `JOYINFOEX` view while preserving the game's vanilla controller action/binding system.
- Added separate right-stick and LT/RT handling while preserving DP's existing binding-code semantics, including `49 = RT` and `50 = LT`.
- Added automatic keyboard/mouse vs controller switching through DP's own `USEJOY` mode byte, driven by real device activity before the game's central input update.
- Kept the new input paths opt-in: with `NativeXInput = false`, ZachFix does not install the XInput bridge or controller-binding remap; with `AutoSwitch = false`, it does not change DP's input mode.
- Removed the earlier XiDi/ExtendedGamepad compatibility experiment from the production input path.

### Glyph themes

- Added dynamic keyboard/gamepad glyph-atlas switching tied to DP's current keyboard/controller mode.
- Added human-readable theme files under `ZachFix\glyphs\keyboard` and `ZachFix\glyphs\gamepad`, with DDS, PNG, and TGA support.
- Added `Native` and gamepad `xbox` fallback handling for sessions where DP does not create both original glyph atlases.
- Added F10 glyph-theme discovery and live theme selection.
- Added glyph-theme hot reload, including add/edit/remove detection without restarting the game.
- Isolated DP's two managed glyph atlases from the generic texture-override path while `DynamicAtlas = true` so theme selection and ordinary texture overrides cannot compete for the same texture.
- Made `DynamicAtlas` enable/disable restart-only because it changes glyph-texture ownership; theme selection and hot reload remain live.

### Documentation

- Documented controller rebinding through DP's existing `configJ.cnf` rather than adding a second ZachFix rebinding database.
- Added the Native XInput binding-value table and clarified that the original WinMM binding semantics remain untouched when the native backend is disabled.
- Added glyph-theme directory, fallback, F10, hot-reload, and texture-override interaction documentation.
- Added the `ZachFix\glyphs` keyboard/gamepad directory skeleton and a dedicated glyph-theme README to the release package.

## v0.1.0-rc5

Fullscreen and D3D9 device-recovery focused release candidate.

### Fullscreen and device recovery

- Fixed DPLauncher exclusive fullscreen startup when ZachFix overrides the display resolution.
- Preserved genuine D3D9 exclusive fullscreen instead of converting fullscreen requests into windowed presentation.
- Normalized exclusive-fullscreen presentation parameters consistently during both device creation and device reset.
- Fixed invalid fullscreen reset parameters after Alt-Tab.
- Improved D3D9 device-reset handling for ZachFix-owned resources.
- Properly invalidated and recreated Dear ImGui DX9 device objects around device resets.
- Released NativeComposite G-buffer references before device reset.
- Cleared stale render-target tracking and reacquired the backbuffer after a successful reset.

### Compatibility

- Verified exclusive-fullscreen startup and reset recovery with native D3D9, DXVK, and dgVoodoo2.
- DXVK and dgVoodoo2 tolerate repeated Alt-Tab cycles in exclusive fullscreen in current testing.
- Native D3D9 can still fail to recover after repeated Alt-Tab cycles because the game may reach `D3DERR_DEVICENOTRESET` without issuing another Reset. Borderless windowed mode is recommended for reliable Alt-Tab behavior.
- dgVoodoo2 PostFX rendering issues remain a separate compatibility problem and are not part of this fullscreen/device-reset fix.

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
