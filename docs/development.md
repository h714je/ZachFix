# Building and packaging ZachFix

## Requirements

- Windows
- Visual Studio 2022 with Desktop development with C++
- CMake 3.20+
- x86 / Win32 target
- internet access during initial configure for pinned FetchContent dependencies

The project intentionally rejects a 64-bit configuration because `DP.exe` is 32-bit.

## Configure and build

From a Visual Studio Developer PowerShell:

```powershell
cmake -S . -B build -A Win32
cmake --build build --config Release
```

The module is produced as:

```text
ZachFix.asi
```

## Package

```powershell
cpack --config build\CPackConfig.cmake -C Release
```

The package name follows:

```text
ZachFix-v<version>-win32.zip
```

The runtime package contains:

- `scripts/ZachFix.asi`;
- `ZachFix.ini`;
- root README/CHANGELOG/license files;
- the `docs/` product documentation;
- bundled glyph assets/documentation;
- third-party license notices.

Pinned build dependencies currently include MinHook, Dear ImGui, and miniz through CMake FetchContent.
