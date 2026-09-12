# DPFix-NG v0.0.46 Additional DoF Blur

This checkpoint adds the last useful Depth-of-Field tuning knob from original DPFix as an optional live setting.

## What it does

Deadly Premonition already performs its own DoF blur. DPFix-NG does not replace the game's focus/depth logic.

`DepthOfField.AdditionalBlur` softens the already-generated DoF buffer immediately before the final tone-map/resolve path:

    [DepthOfField]
    ImproveResolution = true
    AdditionalBlur = 0

Values:

- `0` = off (default).
- `1` = soft additional blur.
- `2` = stronger additional blur.

The option is mainly useful with `ImproveResolution=true`: raising the original 448x252 DoF target removes pixelation but can also make the effect look perceptually sharper than vanilla.

## Implementation

Original DPFix used its GAUSS effect at this point in the rendering pipeline. DPFix-NG keeps the same high-level injection point but deliberately avoids importing the old effect framework.

When the floating-point DoF top-level target is observed twice in direct succession, DPFix-NG applies a lightweight low-pass pass to that target:

    DoF target -> 3/4-size FP16 scratch RT (linear) -> DoF target (linear)

`AdditionalBlur=2` repeats the round trip twice.

The scratch RT is created lazily with the same format as the game's DoF target and is recreated automatically if the effective DoF dimensions change. Disabling Additional Blur through Hot Apply releases the scratch resource.

This path:

- does not modify focus distance, depth selection or circle-of-confusion logic;
- does not replace the game's own DoF blur;
- uses the already-existing `SetRenderTarget` hook;
- adds no new D3D9 hooks;
- stays separate from the Runtime Resource Audit replacement manager;
- is live through the F10 Apply button.

## Testing

A useful test scene has a clearly focused foreground subject and visibly defocused background.

1. Keep `Improve DoF Resolution` enabled.
2. Compare `Additional DoF Blur`: Off -> Soft -> Stronger.
3. Verify that focus boundaries/selection remain identical and only blur softness changes.
4. Toggle back to Off and confirm the normal high-resolution DoF is restored on the next DoF pass.
5. Exercise Internal Scale Hot Apply; the scratch RT should be recreated for the new DoF size without affecting Runtime Resource Audit counters.

Expected first-use log lines include:

    [DoF] Additional blur scratch ready: ...
    [DoF] Additional blur active: amount=1, target=..., scratch=...

If the driver's/DXVK StretchRect path rejects the FP16 filtered copy, DPFix-NG logs a warning and leaves the game's DoF buffer otherwise intact.

## Existing features

Unchanged from v0.0.45:

- internal-resolution scaling and Hot Apply;
- shadow/reflection/DoF resolution scaling;
- pixel-offset corrections;
- world-detail extension;
- smart texture filtering;
- DPFix-compatible texture override;
- Texture Developer Mode with live add/edit/remove reload;
- Texture Inspector/dumping;
- Runtime Resource Audit;
- F10 UI and input isolation.
