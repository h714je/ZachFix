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

## Original Xbox Combat Strafe

The PC executable retains the original Xbox Player states `09/0A`, their mirrored motion sets, mode-2 combat camera use, and completion back to the weapon/combat boundary. Director's Cut removed the corresponding high-level shoulder-button ingress.

With `Gamepad.RestoreCombatStrafe = true`, ZachFix restores that ingress only while the recovered native combat gates pass. Under Native XInput with the Xbox 360 profile, LB pressed-edge requests state `09` (left) and RB pressed-edge requests state `0A` (right); holding the opposite shoulder suppresses the request, matching the recovered Xbox semantics. No custom strafe animation, camera motion, or locomotion is introduced.

The option is disabled by default and changes immediately from F10. The separate Xbox Quick Turn `0x0B` path remains research-only and is not patched by the production runtime.

## Building day/night restoration

ZachFix repairs the Director's Cut `HOUSE_LIST.NOD` runtime conversion problem that breaks the game's native building/window day-night configuration.

With the fix active, affected `N_WINDOW` geometry is controlled again by the game's original day/night state instead of a shader or custom time-range workaround.

The production repair is automatic on supported Steam/GOG builds and has no user-facing INI switch.

## Interior visibility-volume fix

`World.FixInteriorOcclusionBugs = true` fixes a Director's Cut regression where visible props can be rejected by one interior visibility-volume path near walls/mirrors.

The fix only changes the confirmed affected callsite. Normal camera frustum culling and the rest of the world pipeline remain active.

## World distance controls

ZachFix exposes five independent controls:

- `HighDetailDistanceScale` - which existing streaming cells use high-detail content;
- `MainFrustumDistanceMode` - raises the native short-range CRdCamera visibility classes without changing object classification;
- `ObjectActivationDistanceScale` - how far world objects remain active;
- `ObjectLODDistanceScale` - extends native mesh LOD transition distances for type-1 render objects that contain native multi-LOD resource groups.
- `Alternate3DDistanceScale` - extends the native near/full range for objects that use alternate low-detail 3D residency packages.

These settings solve different forms of visible pop-in and should not be treated as one global draw-distance multiplier. Directional shadow relevance is another independent distance path and is not controlled by the main-frustum setting.

See [rendering.md](rendering.md) for values and runtime behavior.
