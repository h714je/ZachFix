# DPFix-NG v0.0.47 Live DoF Blur Controls

This checkpoint is intentionally small. It keeps the v0.0.46 Additional DoF Blur implementation and changes only its F10 interaction model.

## What changed

`DepthOfField.AdditionalBlur` is still the same three-state setting:

    0 = Off
    1 = Soft
    2 = Stronger

The F10 panel now shows the three states as radio buttons on one line instead of a combo box:

    Additional DoF Blur   ( ) Off   ( ) Soft   ( ) Stronger

Selecting a state applies it immediately. The general Apply button is not required for this setting. No DoF render-target rebuild occurs when switching Off / Soft / Stronger.

`Save to INI` remains explicit. A live selection changes the current session only until the user saves the editor values.

## What did not change

- No DoF Resolution Scale option was added.
- `ImproveResolution` remains the existing DPFix-compatible 35% internal-resolution path.
- The blur algorithm and injection point are unchanged from v0.0.46.
- 0 remains the default.
- 1 performs one 3/4-size linear downsample/upscale round trip.
- 2 performs the same round trip twice.
- The game's focus/depth/CoC behavior remains untouched.
- No new D3D9 hooks were added.

## Expected workflow

Find a visible DoF shot, open F10 and click Off / Soft / Stronger while the shot is still on screen. The next matching DoF pass uses the new amount immediately. Use Save to INI only if that choice should persist on the next launch.
