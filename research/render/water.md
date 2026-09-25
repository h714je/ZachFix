# Water / river rendering

## Current status

The water investigation is **partially closed**:

- one real PC port regression is confirmed;
- the main Xbox-vs-PC surface-quality difference remains unresolved;
- the research branch intentionally did not ship speculative lighting/UV hacks.

## What survived the port

The following simple explanations are ruled out for the main river regression:

- `CObjectWater` is not missing on PC;
- PC does animate the river material tuple;
- the river normal map is alive and sampled;
- reflection/refraction are alive;
- PC has lit/specular water shader permutations;
- the main problem is not merely an absent second obvious water pass.

## Confirmed PC ordering regression

PC applies the animated river material/scroll tuple and then later overwrites the complete `c243 / g_vScrlParam` with:

```text
[1.5, 1.0, 0.0, 0.0]
```

before the draw.

Xbox performs the writes in the opposite useful order:

```text
base/default state
    ↓
animated per-material override last
    ↓
draw
```

A live PC A/B correcting the order visibly changes river flow/character.

**Status: CONFIRMED port regression.**

## What it does NOT solve

Correcting `c243` ordering does **not** restore the characteristic Xbox surface:

- larger/readable ripple structures;
- dense silver/white breakup;
- stronger small-scale brightness variation.

Therefore:

```text
lost scroll ordering != main Xbox water-quality regression
```

## Current open boundary

The remaining high-value comparison is inside the rich river shader contract:

```text
Xbox RE_LIGHTING river permutation
        ↕
PC lit-water permutation
```

with attention to:

- normal/light/view coordinate spaces;
- specular response;
- shadow attenuation;
- interpolator semantics;
- effective UV/spatial frequency.

## Production rule

Do not revive broad substitutions such as arbitrary:

- UV scaling;
- specular gain;
- shader replacement;
- shadow disabling;
- guessed texture substitutions.

The confirmed scroll-order bug can remain a future narrow fix candidate, but it should not be advertised as full Xbox-water restoration.

## Evidence

Primary retained handoff:

- `ZachFix-water-research-final-handoff-2026-09-20.md`

Supporting research:

- `ZachFix-water-rendering-research-handoff-2026-09-20*.md`
- river runtime traces / `RIVER_XBOX_ORDER_AB_v2`
