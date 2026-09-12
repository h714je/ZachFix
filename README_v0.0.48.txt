# DPFix-NG v0.0.48 UI Organization & Credits

This checkpoint reorganizes the F10 panel without changing rendering behavior.
The goal is to keep normal gameplay settings compact while moving developer
information and attribution out of the main settings flow.

## F10 layout

The panel now has three top-level tabs:

    Settings | Diagnostics | About

The common action row remains visible above the tabs:

    Save to INI | Reload INI | Apply

The current status message is also kept above the tab contents.

### Settings

Normal rendering controls remain here:

- Internal Scale and Pixel Offset
- Shadow and Reflection scaling
- DoF resolution improvement
- live Additional DoF Blur radio buttons
- World Detail
- Texture Filtering

Texture Override settings are now grouped under a collapsed `Textures` header.
The section contains the production/developer override controls, dimension mode,
dumping and Reload Overrides. Texture statistics and the Inspector no longer
occupy the normal Settings page.

### Diagnostics

Developer/runtime information is separated into collapsible sections:

- Runtime Resource Audit
- Texture Inspector

Texture Inspector also contains the texture load/dump/hot-reload counters that
previously lived in the main Settings page. When Texture Developer Mode is off,
the diagnostics remain available but clearly state that per-texture inspector
tracking is inactive.

### About

The new About tab contains:

- DPFix-NG version and target information
- acknowledgements for the original DPFix/DSFix work and the external projects
  used alongside DPFix-NG
- a collapsible Third-party software section for bundled/build dependencies and
  code lineage
- a pointer to THIRD_PARTY.md for full attribution/license notes

## Rendering behavior

No rendering algorithm, texture ownership rule, hot-reload policy, DoF behavior,
resource replacement behavior or D3D9 hook set changed in this checkpoint.
Additional DoF Blur remains immediate; other editable settings retain their
existing Apply/restart semantics.
