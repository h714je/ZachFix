# Installation and renderer setup

## Requirements

ZachFix targets the 32-bit PC version of *Deadly Premonition: The Director's Cut* and is distributed as an ASI plugin.

You need:

- a supported Steam 1.01b or GOG 1.01b `DP.exe`;
- an ASI loader;
- the 32-bit ZachFix release package.

The tested loader arrangement uses **Ultimate ASI Loader** as `winmm.dll`.

## Typical installation

```text
Deadly Premonition The Director's Cut/
|-- DP.exe
|-- ZachFix.ini
|-- winmm.dll                 <- ASI loader
|-- scripts/
|   `-- ZachFix.asi
`-- ZachFix/
    |-- glyphs/
    |   |-- keyboard/
    |   `-- gamepad/
    `-- textures/             <- created/used by texture features
```

Press **F10** in game to open the settings UI.

The default configuration file is `ZachFix.ini` beside `DP.exe`.

## Native Windows D3D9

Do not place a graphics-wrapper `d3d9.dll` beside `DP.exe`.

```text
DP.exe -> Windows D3D9
         ^ ZachFix hooks the game's D3D9 interface
```

## DXVK

Use the **32-bit** DXVK `d3d9.dll` beside `DP.exe`.

```text
DP.exe -> ZachFix -> DXVK D3D9 -> Vulkan
```

When ReShade is used with this stack, use the Vulkan path.

## dgVoodoo2

Use the **x86** dgVoodoo2 D3D9 DLL beside `DP.exe`.

```text
DP.exe -> ZachFix -> dgVoodoo2 D3D9 -> D3D11
                                      -> ReShade DXGI (optional)
```

Only one active graphics-wrapper DLL should normally be named `d3d9.dll` beside `DP.exe`.

## Large Address Aware / 4 GB patch

Deadly Premonition is a 32-bit game. Marking `DP.exe` as **Large Address Aware** is strongly recommended when using high internal resolutions, large shadow/reflection targets, texture replacements, or the PostFX stack.

Always back up `DP.exe` before modifying it. ZachFix does not apply the LAA flag itself.

LAA/header changes do not alter the executable layout used by current ZachFix build detection.

## DirectX 9 runtime

A base-game crash can happen before ZachFix has a chance to initialize. If that occurs, verify the legacy DirectX 9 runtime and especially the `d3dx9_43.dll` copy being loaded.

A Microsoft-signed copy known to work during ZachFix testing reports:

```text
File version: 9.29.952.3111
SHA-256: 0b28546be22c71834501f7d7185ede5d79742457331c7ee09efc14490dd64f5f
```

Use the official Microsoft DirectX runtime rather than DLL download sites.

## Fullscreen and Alt-Tab

Deadly Premonition's native D3D9 exclusive-fullscreen recovery can fail after repeated Alt-Tab cycles. ZachFix normalizes its own fullscreen/reset handling, but the game can still stop recovering the device.

For reliable task switching:

1. disable **Fullscreen** in `DPLauncher.exe`;
2. keep `Borderless = true` in `[Display]`.

This keeps the D3D9 device windowed while presenting a borderless full-monitor window.

True exclusive fullscreen remains available, but repeated Alt-Tab recovery is not guaranteed.

## First-run checks

After launching:

- open F10 and confirm the settings UI appears;
- check `ZachFix.log` for the detected `DP.exe` build;
- confirm the intended renderer backend is active;
- keep a backup of your original `DP.exe` and save data before adding unrelated executable patches or mods.
