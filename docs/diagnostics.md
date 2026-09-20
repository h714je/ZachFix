# Diagnostics

The F10 **Diagnostics** tab contains runtime information and opt-in diagnostic tools that are part of the current production build.

This document covers the supported product diagnostics. Internal reverse-engineering controls are intentionally not documented here.

## Runtime Resource Audit

ZachFix Hot Apply can substitute replacement backing resources without resetting the D3D9 device.

The Runtime Resource Audit reports the current replacement generation, managed resources, active references, estimated active memory, and create/release totals.

The useful lifetime invariant is:

```text
Created - Released == Active replacements
```

When no replacement resources are needed, outstanding replacements should return to zero.

## D3D9 Resource Lifetime Audit

This audit is intended for long-session performance degradation or suspected resource-lifetime problems.

It is deliberately session-only and is not stored in `ZachFix.ini`.

To start it:

1. open F10;
2. open **Diagnostics**;
3. enable **Record D3D9 resource lifetime audit**;
4. click **Apply**.

Starting a capture creates a new timestamped file beside `ZachFix.asi`, normally under the game's `scripts` directory:

```text
ZachFix-resource-audit-YYYYMMDD-HHMMSS.log
```

If a filename already exists, ZachFix adds a numeric suffix instead of overwriting it.

The audit samples every 30 seconds and records:

- interval FPS and audit runtime;
- process private/working/commit memory;
- process handles, GDI objects, USER objects, and thread count;
- D3D9 available-texture-memory as a trend-only value;
- created, released, and live D3D9 resource counts by type;
- approximate live bytes where the resource description allows an estimate;
- top groups of still-live resources by creation site, including `DP.exe+RVA` attribution when available;
- DP build, GPU/driver, and privacy-safe D3D9 backend origin.

Only resources created after activation belong to that capture's baseline.

The optional audit hooks are installed lazily on first use. If coverage is incomplete, both the UI and log report `PARTIAL` instead of presenting incomplete counters as complete.

## Tuning Pause

`[UI] PauseGameWhileOpen` can freeze gameplay timers while F10 is open during ordinary gameplay.

Changing the checkbox requires **Apply** and takes effect the next time F10 is opened. Apply/Reload does not change the pause state in the middle of an already-open panel session.

Some cutscenes can hang while the gameplay timers are frozen. Use PostFX Preview Freeze instead when tuning cutscene visuals.

## PostFX runtime information

The Diagnostics and PostFX pages expose runtime information for the active AO, bloom, DoF, exposure, and preview-freeze paths. This can be useful when checking whether an effect is active and whether runtime telemetry is available.

## Texture Inspector

When Texture Developer Mode is active, Diagnostics includes texture-load/override counters and inspection information for recently observed textures and override hits.

See [textures.md](textures.md) for Developer Mode requirements.
