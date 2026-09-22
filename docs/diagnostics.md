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

## Tuning Pause

`[UI] PauseGameWhileOpen` can freeze gameplay timers while F10 is open during ordinary gameplay.

Changing the checkbox requires **Apply** and takes effect the next time F10 is opened. Apply/Reload does not change the pause state in the middle of an already-open panel session.

Some cutscenes can hang while the gameplay timers are frozen. Use PostFX Preview Freeze instead when tuning cutscene visuals.

## PostFX runtime information

The Diagnostics and PostFX pages expose runtime information for the active AO, bloom, DoF, exposure, and preview-freeze paths. This can be useful when checking whether an effect is active and whether runtime telemetry is available.

## Texture Inspector

When Texture Developer Mode is active, Diagnostics includes texture-load/override counters and inspection information for recently observed textures and override hits.

See [textures.md](textures.md) for Developer Mode requirements.
