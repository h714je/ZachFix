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

## Reverse-engineering workflow

The current reconciled architecture is indexed in [`../research/README.md`](../research/README.md). Product code should not be changed from an isolated decompile label or an old research note alone.

Preferred evidence order:

```text
raw PC machine code
    > PC decompile
    > Steam/GOG cross-build agreement
    > reconciled notes
    > Xbox/Xenia clues
    > inference
```

Build-specific hooks should continue to use centralized `DpBuildProfile` mappings plus local signature checks and fail closed on unknown/mismatched executables.

When promoting research into production, document the exact subsystem boundary being changed, what neighboring native behavior is deliberately preserved, and the runtime validation that closed the change. Keep disproven interpretations in `research/disproven.md` rather than deleting them from history.
