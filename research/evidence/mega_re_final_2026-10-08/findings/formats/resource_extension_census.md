# DPserial Resource Extension Census

**Reviewed:** 2026-10-01
**Evidence state:** asset counts are `VERIFIED`; literal-string-to-code links are `STRONG_INFERENCE` loader boundaries, not complete loader identification.

## Method

The census counted non-sidecar files under `inputs/assets/dpserial/`, matched executable strings whose values end in an extension, and joined those string addresses to `xrefs.csv` source functions for Steam and GOG. A direct xref proves that code references a literal string; it does not prove that the function parses the format or owns the resulting object. Results are in `ledgers/RESOURCE_TYPE_LEDGER.csv`.

## High-signal families

| Extension | Asset files | PC literal strings | Literal string refs | Code-source pattern | Current state |
|---|---:|---:|---:|---|---|
| `.XPC` | 3,819 | 3,990/3,989 | 153/145 reported string references | Steam `00428F30`, `004C3240`, `00627240`; GOG `00428F60`, `004C3320`, `006271C0`, plus related callers | `STRONG_INFERENCE` resource-package boundary |
| `.XMD` | 3,033 | 3,217/3,215 | 171/164 | Steam `004C3240`, `00627240`, `0041D970`; GOG `004C3320`, `006271C0`, `0041D990`, plus related callers | `STRONG_INFERENCE` model-resource boundary |
| `.XAM` | 3,456 | 3,565 | 97 | Steam `005DD600`, `005AD940`; GOG `005DD6D0`, `005ADA10`, plus related callers | `STRONG_INFERENCE` animation-resource boundary |
| `.XCA` | 1,641 | 1,642 | 1 | Steam `004213D0`; GOG `004213F0` | `STRONG_INFERENCE` facial-animation boundary |
| `.PRM` | 410 | 447 | 16 | Steam `00427AF0`, `004213D0`, `005DFB40`; GOG `00427B20`, `004213F0`, `005DFC10`, plus related callers | `STRONG_INFERENCE` parameter-resource boundary |
| `.XNV` | 318 | 335 | 23 | Steam/GOG `006272xx`, `0042A7xx`, `006A3Axx`, `005A58xx` | `STRONG_INFERENCE` navigation-resource boundary |
| `.XWP` | 221 | 226/225 | 6 | Steam `00438D10`, `0044FA20`, `00597420`, `005C4CB0`, `00614F00`; GOG corresponding nearby functions | `STRONG_INFERENCE` effect-package boundary |
| `.XLY` | 175 | 189 | 6 | Steam `006195F0`, `00464640`, `00464860`; GOG corresponding nearby functions | `STRONG_INFERENCE` layout-resource boundary |
| `.XPM` | 137 | 194 | 4 | Steam/GOG `006F72xx`, `006F78xx`, `006F7Cxx` | `STRONG_INFERENCE` resource-family boundary |

The ledger contains 33 extension/container rows. Families with no direct literal xref remain `UNKNOWN` unless an independent prior report provides a bounded interpretation.

## Important negative evidence

`.DSB` has 656 asset files and 656/657 literal event-stream strings in the two packs, but no direct literal-string xref in the current exports. Prior content analysis establishes that these are event command streams, so the resource row is `STRONG_INFERENCE` for format identity while the native loader function remains unresolved.

`.XFE` has 70 assets and no direct literal-loader xref. Prior research identifies the inspected XFE payload as a facial-expression asset, not a CEvent bytecode stream; this is retained as a bounded `STRONG_INFERENCE`, not a loader claim. `.XUL`, `.XUS`, `.XVO`, `.XCM`, `.GRS`, `.MES`, and small auxiliary extensions also lack sufficient direct loader evidence and remain `UNKNOWN` or generic family candidates.

## Next discriminators

1. Inspect the highest-source functions for `.XPC`/`.XMD`/`.XAM` together with callers, allocation sites, vtable writes, and resource-type branches.
2. Compare the paired Steam/GOG functions using normalized control flow and string/global access, not address proximity alone.
3. Identify generic extension dispatch tables or format magic checks that explain why some formats have no literal xrefs.
4. Link resource loaders to constructor/vtable candidates and concrete asset directory families.
