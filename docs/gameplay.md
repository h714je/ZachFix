# Gameplay and restoration fixes

## Native difficulty restoration

ZachFix restores Deadly Premonition's original New Game difficulty selector:

```text
Easy
Normal
Hard
```

The selection uses the game's native difficulty state and is stored in the normal `savedata\dp.sav` record. Continue reads the difficulty back from the save.

ZachFix does not implement custom difficulty damage/health multipliers and does not maintain separate Easy/Normal/Hard save directories.

F10 shows the current native difficulty as read-only status.

## Building day/night restoration

ZachFix repairs the Director's Cut `HOUSE_LIST.NOD` runtime conversion problem that breaks the game's native building/window day-night configuration.

With the fix active, affected `N_WINDOW` geometry is controlled again by the game's original day/night state instead of a shader or custom time-range workaround.

The production repair is automatic on supported Steam/GOG builds and has no user-facing INI switch.

## Interior visibility-volume fix

`World.FixInteriorOcclusionBugs = true` fixes a Director's Cut regression where visible props can be rejected by one interior visibility-volume path near walls/mirrors.

The fix only changes the confirmed affected callsite. Normal camera frustum culling and the rest of the world pipeline remain active.

## World distance controls

ZachFix exposes four independent controls:

- `HighDetailDistanceScale` - which existing streaming cells use high-detail content;
- `MainFrustumDistanceMode` - raises the native short-range CRdCamera visibility classes without changing object classification;
- `ObjectActivationDistanceScale` - how far world objects remain active;
- `ObjectLODDistanceScale` - extends native mesh LOD transition distances for type-1 render objects that contain native multi-LOD resource groups.

These settings solve different forms of visible pop-in and should not be treated as one global draw-distance multiplier.

See [rendering.md](rendering.md) for values and runtime behavior.
