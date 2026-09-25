# ZachFix documentation

This directory documents the functionality that is present in the current ZachFix production tree.

It is intentionally product-focused. Historical reverse-engineering notes, abandoned experiments, and unfinished research are not part of this documentation set.

## User documentation

- [installation.md](installation.md) - installation, ASI loader layout, renderer backends, recommended base-game setup.
- [configuration.md](configuration.md) - `ZachFix.ini` reference and runtime/restart behavior.
- [rendering.md](rendering.md) - display, internal resolution, shadows, reflections, DoF, filtering, world-detail controls, and comparison screenshots.
- [postfx.md](postfx.md) - AO, Bloom, Depth of Field, exposure, Xbox 360 color/output restoration, and PostFX Preview Freeze.
- [input.md](input.md) - Native XInput, Xbox 360 profile, analog triggers, vibration, auto-switching, bindings, and glyph themes.
- [textures.md](textures.md) - DPFix-compatible texture replacement, dimension modes, dumping, and Developer Mode.
- [gameplay.md](gameplay.md) - restored difficulty, building day/night behavior, interior visibility fix, and world-distance controls.
- [save-safety.md](save-safety.md) - transactional saves, validation, compressed backups, and failure bundles.
- [diagnostics.md](diagnostics.md) - production diagnostics available through F10.
- [troubleshooting.md](troubleshooting.md) - common startup, backend, UI, fullscreen, and compatibility problems.
- [architecture.md](architecture.md) - product-facing map of the native input, Player/camera, world-distance, renderer, and physics boundaries ZachFix preserves.

## Project documentation

- [supported-builds.md](supported-builds.md) - supported `DP.exe` builds and compatibility rules.
- [development.md](development.md) - building and packaging ZachFix.

For release history, see the repository root [CHANGELOG.md](../CHANGELOG.md).

## Reverse-engineering research

The reconciled engine research is indexed at [`research/README.md`](../research/README.md). It includes the Player/action protocol, input/camera architecture, world-distance/LOD/residency map, renderer investigations, disproven interpretations, unresolved targets, and the retired PhysX timing campaign.

Experimental PhysX/physics-timing hooks are not part of the production tree. Their final record and retirement rationale are kept in [`research/physx/README.md`](../research/physx/README.md).
