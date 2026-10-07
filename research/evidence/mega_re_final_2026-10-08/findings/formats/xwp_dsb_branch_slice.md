# XWP / DSB Branch Slice

**Date:** 2026-10-01

## Scope

This is a targeted continuation from the verified typed-resource callback. It does not replace the resource ledger; the attempted focused ledger-promotion script was blocked by auto-mode permission. Evidence remains build-specific and confidence is conservative.

## XWP

**STRONG_INFERENCE:** XWP is not directly constructed as a typed object by the generic callback's mode-zero path. The callback recognizes `.XWP` among the extension families, but the direct allocation/vtable path is limited to XMD and XPC; XWP falls through the recognized-family branch without a visible typed allocation.

A separate Steam/GOG paired path is evidenced:

- Steam `0044FA20` / GOG `0044FA50` enumerate `UPDATA/EFFECT/XW*` directories and register paths formatted as `<root>/<name>/<name>.XWP` through the resource registration helper.
- Steam `00677CF0` / GOG `00677C40` perform manager-backed XWP lookup under a mutex. If the record is absent, they construct a default `UPDATA/EFFECT/XWP/` path and register it. They then obtain the resource descriptor, require/check the literal `ver1.5`, and traverse nested records with the manager record accessors.
- Steam `00438D10` / GOG `00438D90` parse an event/effect command stream, construct an XWP name from a command string, resolve an event-core singleton, and use the resource lookup/result in effect placement and runtime object creation. They then create a runtime object from the resolved XWP name and apply parsed transform/parameter state.
- Steam `00597420` loads `S0TYA001.XWP` through `00677CF0`, establishing another effect-owned consumer path.

This supports an XWP resource-loading-to-effects boundary, but does not prove a dedicated XWP C++ class or vtable. The generic callback's no-allocation behavior is a useful negative constraint: package loading and effect consumption are distributed across later subsystem paths.

## DSB

**STRONG_INFERENCE:** The DSB callback branch initializes an event-core singleton and invokes paired helper/release paths:

- Steam `00408960` / GOG `00408920` lazily allocate `0x13EC` bytes, initialize the object through `CEvCore` support code, and write `CSingleton<CEvCore>::vftable`.
- Steam `0070DD40` / GOG `0070DCE0` resolve an event slot through a paired lookup helper. On success they select/update an event slot and pass the command-side byte to a paired state-update helper (`0072A5A0` Steam / `0072A2B0` GOG).
- Release-side helpers are paired by build (`0070DE80` / `0070DE20`), each resolving state and invoking the corresponding event-core release helper.
- The callback invokes the helper for `.DSB` and invokes the release-side path for mode one. The stream grammar, command record format, and complete ownership/lifetime remain unresolved.

The DSB path therefore crosses resource loading into the `EVENT_SCRIPT` subsystem, but no final event class or parser schema is promoted here.

## Remaining branch targets

XAM, XCA, XFE, and XNV still have asset/string and consumer evidence but no directly verified typed allocation/parser in this slice. Their next discriminator is a paired helper with a format/header check or a concrete consumer-side object handoff. XWP package object identity and DSB command-stream semantics remain open.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:7489-7664`
- `inputs/decompiler/gog/DP_decompiled.c:7443-7663`
- `inputs/decompiler/steam/DP_decompiled.c:391707-391845`
- `inputs/decompiler/gog/DP_decompiled.c:300433-300560`
- `inputs/decompiler/steam/DP_decompiled.c:43667-43867`
- `inputs/decompiler/gog/DP_decompiled.c:40651-40851`
- `inputs/decompiler/steam/DP_decompiled.c:257018-257075`
- `inputs/decompiler/steam/DP_decompiled.c:7770-7798`
- `inputs/decompiler/gog/DP_decompiled.c:7724-7752`
- `inputs/decompiler/steam/DP_decompiled.c:490010-490028`
- `inputs/decompiler/gog/DP_decompiled.c:387214-387232`
