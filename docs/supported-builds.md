# Supported builds and backends

## Supported executables

ZachFix currently supports these 32-bit Director's Cut executables:

| Build | PE TimeDateStamp | SizeOfImage | Status |
| --- | ---: | ---: | --- |
| Steam 1.01b | `0x529721DC` | `0x010B5000` | Supported |
| GOG 1.01b | `0x52970AF6` | `0x010B5000` | Supported |

The same `ZachFix.asi` supports both builds. Detection is automatic and there is no Steam/GOG selector in `ZachFix.ini`.

Build-specific hooks select the matching executable profile and validate expected local instruction signatures before changing game code. If the executable identity or a required signature does not match, the affected fix is disabled instead of using known offsets blindly.

## Executable modifications

Common header or one-byte changes can alter the whole-file hash without changing the image layout used by ZachFix. Examples include:

- Large Address Aware / 4 GB flag changes;
- the optional intro-skip byte edit.

For this reason the current compatibility contract is based on PE identity plus local signatures where required, not one universal whole-file SHA-256.

## Renderer backends

These paths have been exercised during current development:

| Backend | Core ZachFix | F10 UI | Hot Apply | Notes |
| --- | --- | --- | --- | --- |
| Native Windows D3D9 | Tested | Tested | Tested | Exclusive-fullscreen Alt-Tab remains a game limitation. |
| DXVK | Tested | Tested | Tested | ReShade depth/MXAO path has been tested. |
| dgVoodoo2 | Tested | Tested | Tested | D3D11 output path and ReShade DXGI have been tested. |

ZachFix does not need to own the local `d3d9.dll` slot, so the renderer can remain native D3D9, DXVK, or dgVoodoo2.

## Unsupported executables

If ZachFix reports an unsupported `DP.exe`:

- do not force Steam/GOG offsets manually;
- keep the executable intact;
- include the opening `[Build]` lines from `ZachFix.log` when reporting the issue.
