# Troubleshooting

## The game crashes before ZachFix loads

If the game also crashes with ASI loading removed, start with the base game:

- verify that the executable is a supported Steam 1.01b or GOG 1.01b build;
- verify the legacy DirectX 9 runtime and `d3dx9_43.dll`;
- remove conflicting local wrapper/proxy DLLs;
- test DXVK as an alternate D3D9 compatibility path if native D3D9 fails.

See [installation.md](installation.md) for the DirectX note and known-good `d3dx9_43.dll` fingerprint.

## F10 does not open

Confirm that ZachFix loaded:

- the ASI loader should be present and functional;
- `ZachFix.asi` should normally be under `scripts\`;
- `ZachFix.log` should be created;
- the log should contain the detected game build and ZachFix binary/version information.

If a wrapper is used, include its version and the game-directory DLL layout when reporting the problem.

## Unsupported `DP.exe`

Do not force a Steam/GOG profile or manually copy executable-specific offsets.

Keep the unknown executable intact and include the opening `[Build]` lines from `ZachFix.log` in the report.

## Black screen after Alt-Tab

Repeated recovery from true D3D9 exclusive fullscreen is a known game limitation.

Recommended setup:

- disable Fullscreen in `DPLauncher.exe`;
- use `Borderless = true` in ZachFix.

See [installation.md](installation.md).

## F10 tuning pause hangs a cutscene

Disable `PauseGameWhileOpen` for cutscenes and use **Freeze PostFX Preview** when tuning visual effects.

After changing the pause setting in F10, click **Apply**. The new policy starts with the next F10 session.

## Texture override has the wrong dimensions

If an NPOT replacement was resized by D3DX, try:

```ini
[Textures]
DimensionMode = Preserve
```

Developer Mode's Texture Inspector reports both source-file and actual GPU dimensions and can confirm whether Preserve succeeded.

## A setting does not change immediately

Not every setting is live.

Common restart-required or startup-only settings include:

- display/window device-creation settings;
- shadow depth precision;
- Native XInput backend enable/disable;
- AutoSwitch startup enable/disable;
- Texture Developer Mode enable/disable;
- Dynamic Glyph Atlas enable/disable;
- Save Safety startup settings.

See [configuration.md](configuration.md) for the setting reference.
