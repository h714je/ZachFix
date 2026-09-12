# DPFix-NG v0.0.50 EndScene UI Fallback

This checkpoint fixes the native-D3D9 F10 failure that remained in v0.0.49.

The v0.0.49 native log proved three useful facts at once:

- IDirect3DDevice9::Present is reached at least during startup;
- the subclassed Win32 WndProc sees F10 and queues the toggle request;
- no later RenderSettingsUi call consumes that request, so the panel never opens.

That means the failure is not keyboard input. The UI update/render callback itself
was tied too closely to Present behavior on the native D3D9 runtime.

## EndScene UI path

v0.0.50 adds IDirect3DDevice9::EndScene to the production hook surface and makes
it the preferred UI callback once observed.

The hook runs before the game's original EndScene, while the D3D9 scene is still
open. Therefore Dear ImGui is rendered directly into the current scene and
DPFix-NG does not call a second BeginScene/EndScene pair on this path.

The first observed call is logged as:

    [UI] EndScene UI path active.

The existing Device::Present and primary SwapChain::Present paths remain as
fallbacks. Once EndScene has been observed, those presentation hooks stop
calling the UI renderer, preventing the same ImGui frame from being submitted
again during Present.

## F10 input

The v0.0.49 dual input path remains unchanged:

- render-thread GetAsyncKeyState edge polling;
- WM_KEYDOWN / WM_SYSKEYDOWN fallback queued by the subclassed game WndProc.

The native v0.0.49 log already confirmed the Win32 fallback sees F10, so this
release intentionally changes the render callback rather than adding another
keyboard workaround.

## Hook surface

The production device-hook set changes from 12 to 13 methods by adding:

- IDirect3DDevice9::EndScene

The optional primary IDirect3DSwapChain9::Present compatibility hook remains,
for 14 D3D9 COM hook points when available.

## Rendering behavior

No game rendering fix changed. Internal resolution, shadows, reflections, DoF,
Additional DoF Blur, pixel offset, world detail, texture override, hot reload
and smart texture filtering are unchanged.

## Native D3D9 test

1. Run without DXVK/ReShade D3D9 wrappers.
2. Confirm the log contains `[UI] EndScene UI path active.`.
3. Press F10 once.
4. Confirm the log contains `[UI] Settings panel opened; game input suppressed.`.
5. Confirm the panel is visible and interactive.
6. Close it with F10 and verify game input returns normally.

If step 2 is missing, the next investigation should identify which scene boundary
DP uses on the native runtime rather than changing F10 handling again.
