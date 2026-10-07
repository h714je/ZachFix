# XAM / XCA / XNV Targeted Slice

**Date:** 2026-10-01

## XAM

**STRONG_INFERENCE:** XAM has direct actor/object consumers outside the generic callback rather than a newly identified standalone parser in this slice.

- Steam `006271C0` / GOG `006271C0` copy four-word parameter blocks into an object, look up `77JMLI02.XAM` through the resource manager accessor, and invoke the object's virtual slot `+0x54` with the resolved resource and zeroed options.
- Steam/GOG character setup paths also derive `.XAM` names from model/character strings and pass them through resource lookup or animation/effect setup helpers. The repeated pattern is resource-name resolution followed by object-side virtual dispatch.
- This supports an animation-resource-to-actor boundary, but does not establish the XAM binary grammar, a dedicated XAM class, or a callback-owned typed constructor.

## XCA

**STRONG_INFERENCE:** XCA is integrated into scene/resource transition rather than directly typed by the generic callback.

- Steam `004213D0` / GOG `004213F0` format `D%02d.XCA` under `UPDATA/SCENE/%02d/`, test/register the path, then look up the basename through the resource manager.
- On successful lookup, the paired path updates scene/event state through `004277F0`, stores a scene-related flag, and triggers subsequent resource/object transition work.
- The function also performs broad scene initialization and path-table work; therefore it is a scene transition consumer, not proven to be an XCA parser. No XCA class or vtable is promoted.

## XNV

**STRONG_INFERENCE:** XNV is bound into repeated navigation/scene object setup.

- Steam `006A3BA0` / GOG `006A3AF0` create five objects of allocation selector `0x40`, resolve `OGE60511.XNV`, combine it with manager records `0x3313` and `0x3311` through `006BE6E0` Steam / `006BE1F0` GOG, and initialize object type/flags/index fields. A second object of selector `0x41` receives the same XNV/resource combination.
- The repeated resource lookup and object setup establish a resource-loading-to-navigation/world-object boundary, but no XNV header parser, class name, or vtable relationship is visible here.
- Scene transition code also rewrites resource names to `.XNV` and routes them through path/resource registration helpers, reinforcing usage without proving format semantics.

## XFE

No new primary discriminator was found in this targeted slice. The existing ledger keeps XFE as a callback-recognized family with prior facial-expression evidence but no direct loader xref or concrete typed construction. Prior content evidence disproves the older interpretation of `CPL01.XFE` as CEvent bytecode: the inspected payload begins with `XAM2` and contains facial-expression node names, while `.DSB` is the event-script family. Leave XFE `STRONG_INFERENCE` for executable branch recognition and unresolved for parser/class semantics.

## Remaining discriminators

For XAM, XCA, and XNV the next useful evidence is a format/header check or the first consumer that dereferences parsed payload fields. For XFE, prioritize a paired function containing `.XFE` or a facial-expression consumer with a resource lookup and object handoff. Do not infer class identity from the repeated resource names alone.

## Evidence references

- `inputs/decompiler/steam/DP_decompiled.c:349632-349661`
- `inputs/decompiler/gog/DP_decompiled.c:270514-270732` (paired `006271C0` setup; export ordering is build-specific)
- `inputs/decompiler/steam/DP_decompiled.c:26044-27720`
- `inputs/decompiler/gog/DP_decompiled.c:24770-26500`
- `inputs/decompiler/steam/DP_decompiled.c:418811-418913`
- `inputs/decompiler/gog/DP_decompiled.c:318858-318955`
- `inputs/decompiler/steam/DP_decompiled.c:26556-26643`
- `inputs/decompiler/gog/DP_decompiled.c:25849-25990`
