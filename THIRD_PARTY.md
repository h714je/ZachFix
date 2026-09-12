# Third-party components and research lineage

- MinHook v1.3.4, Tsuda Kageyu et al., BSD-2-Clause. Pulled by CMake FetchContent.
- Dear ImGui v1.92.9b, Omar Cornut and contributors, MIT. Pulled by CMake FetchContent.
- The original DPFix/DSFix work by Peter Thoman (Durante) is an important research
  reference for Deadly Premonition rendering behavior and historical fixes. DPFix
  source is GPLv3. Any DPFix-NG code derived from GPLv3 source must retain the
  corresponding GPL attribution and license obligations when distributed.
- DPFix-compatible texture hashing uses Paul Hsieh's SuperFastHash algorithm with
  the historical signed-byte behavior used by DPFix. SuperFastHash is distributed
  under Paul Hsieh's BSD-style license; the copyright and conditions are preserved
  in the source comments and project attribution.
