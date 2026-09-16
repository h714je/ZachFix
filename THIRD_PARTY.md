# Third-party components and research lineage

ZachFix is distributed under GPLv3. The project also uses or interoperates with the components below.

## Original DPFix / DSFix

Peter Thoman (Durante)'s original DPFix and DSFix are the primary historical and rendering-research lineage for ZachFix. DPFix/DSFix source is GPL-licensed. ZachFix retains the applicable GPL obligations and attribution for code and behavior derived from that work.

- DPFix: https://github.com/PeterTh/dpfix
- DSFix: https://github.com/PeterTh/dsfix

## MinHook v1.3.4

MinHook by Tsuda Kageyu and contributors is used for API/function hooks and is fetched by CMake at the pinned `v1.3.4` tag.

License: BSD-2-Clause, with the additional bundled Hacker Disassembler Engine notices present in MinHook's `LICENSE.txt`.

Repository: https://github.com/TsudaKageyu/minhook

The CPack release package installs the exact `LICENSE.txt` from the fetched MinHook source as `licenses/MinHook.txt`.

## Dear ImGui v1.92.9b

Dear ImGui by Omar Cornut and contributors provides the in-game settings UI and Win32/DX9 backends. It is fetched by CMake at the pinned `v1.92.9b` tag.

License: MIT.

Repository: https://github.com/ocornut/imgui

The CPack release package installs the exact fetched license as `licenses/DearImGui.txt`.

## miniz 3.0.2

miniz by Rich Geldreich and contributors provides the standard ZIP/Deflate writer and validator used for compressed save backups and diagnostic bundles. It is fetched by CMake at the pinned `3.0.2` tag and linked statically into ZachFix.

License: MIT.

Repository: https://github.com/richgel999/miniz

The CPack release package installs the exact fetched `LICENSE` as `licenses/miniz.txt`.

## Paul Hsieh's SuperFastHash

ZachFix contains a compatibility implementation of Paul Hsieh's SuperFastHash, including the historical signed-byte tail behavior used by original DPFix. This is required so existing DPFix texture-pack filenames remain compatible.

License: Paul Hsieh OLD BSD license. The notice is included in `licenses/SuperFastHash.txt` and alongside the implementation source.

## Acknowledgements / interoperability

The following projects are part of the tested ZachFix deployment/modding ecosystem but are **not bundled dependencies** merely by being listed here:

- DXVK: D3D9-to-Vulkan compatibility path tested with ZachFix.
- ReShade: external post-processing/depth effects tested with DXVK and dgVoodoo2 paths.
- Ultimate ASI Loader by ThirteenAG: ASI-loading method used by the current test/deployment setup.
- dgVoodoo2: D3D9-to-D3D11 path tested with ZachFix and ReShade DXGI.

Their own licenses and distribution terms apply separately.
