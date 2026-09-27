# CEffect Xbox 360 / PC fixed-delta evidence

**Status:** cross-version contract confirmed; runtime repair validation pending.
**Compared builds:** original PAL Xbox 360, Steam 1.01b, GOG 1.01b.
**Reconciled:** 2026-09-27.

This note keeps the compact evidence behind the timing section in `research/engine/effects.md`. It intentionally separates confirmed cross-version mechanics from the still-unvalidated production repair.

## Update homolog

| Semantic | Xbox PAL | Steam PC | GOG PC |
| --- | --- | --- | --- |
| main inherited effect update | `sub_82548180` | `0x0071C190` | `0x0071BEA0` |
| ordinary delta source | `*(float*)0x842AD3B0` | `gameDelta60` | `gameDelta60` |
| fixed-delta flag | `object+0x25C & 2` | `object+0x1C0 & 2` | `object+0x1C0 & 2` |
| completion-suppression flag | `object+0x250 & 2` | `object+0x1B4 & 2` | `object+0x1B4 & 2` |
| fixed value | `1.0f` | `1.0f` | `1.0f` |

Equivalent control flow:

```text
delta = central gameplay scalar
if (fixedFlag & 2)
    delta = 1.0
if (!(finishFlags & 2))
    completionCheck()
UpdateEffectParts(delta)
```

This proves that the literal fixed step predates the Director's Cut PC port.

## Setter parity

Xbox `sub_82489478` and the PC type-create paths set the fixed bit for the same numeric types:

```text
0x1E
0x2A
0x2B
0x2C
0x2D
0x2E
```

Type `0x2C` also receives the independent bit-1 behavior on both sides.

The Xbox name-create path `sub_82488650` has the same six-check fixed-bit structure as the PC name path. The PC literals are `P0MZL001`, `P0MZL003`, `P0MZL004`, `P0MZL005`, `P0MZL007`, and `P0MZL009`; the broader PC init-existing path covers `P0MZL00*`, `W0BUR*`, `P0TYA001`, `W1SPL*`, and `P0HIT*`.

The structural match strongly supports one authored contract across versions. Do not expand those abbreviations into friendly effect names without asset/runtime evidence.

## Cadence context

The original Xbox gameplay scalar is expressed in 1/60-second units and normal gameplay is gated around the native ~30 Hz cadence. A common native update therefore advances by about `2` ordinary delta-units while the selected fixed family advances by `1`.

```text
Xbox ~30 Hz:
  ordinary ~= 30 * 2.0 = 60 units/s
  fixed     = 30 * 1.0 = 30 units/s

PC 60 Hz:
  ordinary ~= 60 * 1.0 = 60 units/s
  fixed     = 60 * 1.0 = 60 units/s

PC 120 Hz:
  ordinary ~= 120 * 0.5 = 60 units/s
  fixed     = 120 * 1.0 = 120 units/s
```

The exact runtime effect rate still depends on where each object is scheduled, so this arithmetic is a cadence consequence, not by itself a user-visible bug measurement. It does isolate the narrow boundary that needs A/B validation.

## Vtable / porting split

The recovered Xbox `CRdObjectEffect`/`CEffect` table exposes 17 virtual slots. PC exposes 19. The inherited simulation core maps across builds; the two extra PC tail virtuals correspond to the mapped render-packet scratch/build and spatial/tree integration.

Working architecture:

```text
Xbox effect simulation/content contract
    -> retained in PC

PC/DC renderer/world integration
    -> extended/reworked around it
```

## Repair candidate, not production behavior

Do not remove the fixed flag and do not globally rescale CEffect. A narrow 30-Hz-normalized A/B candidate is conceptually:

```text
fixedEffectiveDelta ~= gameDelta60 * 0.5
```

Before promotion, validate at 30/60/120/200+ FPS and across both PC builds:

- effect lifetime;
- emission and animation rate;
- collision/hit/event behavior;
- one-shot callbacks;
- pause/load/reset transitions;
- low-FPS behavior and hitch recovery.

Until that closes, ZachFix should document the boundary but leave production CEffect timing unchanged.
