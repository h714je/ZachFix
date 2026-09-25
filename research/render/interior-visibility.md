# Interior visibility-volume regression

## Production status

The disappearing-prop / pillow-near-mirror regression is **production-closed enough** with a narrow fix.

## Proven rejection order

Runtime captures established:

```text
candidate object
    ↓
normal camera frustum test = PASS
    ↓
interior / visibility-volume test = REJECT
    ↓
object never reaches later render-list builder
    ↓
D3D draw disappears completely
```

The pillow vertex buffer had visible draws in the good state and zero draws in the hidden state. Therefore this is a CPU visibility rejection, not a shader/Z/cull-mode failure after submission.

## Known PC sites

Steam 1.01b:

```text
frustum helper             DP.exe+0x002DBD30
visibility-volume helper   DP.exe+0x002DBE40
outer volume callsite      DP.exe+0x002D3974
```

GOG 1.01b:

```text
frustum helper             DP.exe+0x002DB900
visibility-volume helper   DP.exe+0x002DBA10
outer volume callsite      DP.exe+0x002D3544
```

## Production repair principle

Bypass only the confirmed rejecting **outer-world visibility-volume callsite** when the fix is enabled.

Keep native:

- normal camera frustum culling;
- streaming;
- object activation;
- mesh LOD;
- unrelated callers of the visibility helper.

A global frustum disable is not the fix and should remain research-only at most.

## Relationship to six-frustum map

This regression is separate from the six main-frustum distance classes documented under `world/frustum.md`.

The known pillow candidate already passes its normal frustum test. Extending a far plane may change other visibility behavior but does not address the root rejection demonstrated here.

## Evidence

- `ZachFix-occlusion-production-research-handoff-2026-09-18.md`
- `ZachFix-occlusion-frustum-handoff-2026-09-18*.md`
