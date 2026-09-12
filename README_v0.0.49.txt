# DPFix-NG v0.0.49 Native D3D9 UI Compatibility

This checkpoint fixes the F10 settings overlay path discovered while testing
DPFix-NG on the native Windows Direct3D 9 runtime.

The rendering fixes themselves already worked on native D3D9. The v0.0.48 log
showed Direct3DCreate9/CreateDevice interception, all 12 device hooks, Internal
Scale, shadows, reflections, DoF and Texture Override working, but the F10 panel
never toggled. That result is consistent with native D3D9 presenting through the
primary IDirect3DSwapChain9 rather than the device Present entry point used by
the established DXVK test path. v0.0.49 adds coverage and diagnostics for both.

## Presentation compatibility

DPFix-NG now keeps both presentation entry points available for the UI:

- IDirect3DDevice9::Present, retained from previous releases.
- Primary IDirect3DSwapChain9::Present, added as a non-fatal compatibility
  fallback.

The swap-chain hook obtains the owning IDirect3DDevice9 and feeds the same
RenderSettingsUi path. A thread-local presentation-depth guard prevents nested
Device::Present -> SwapChain::Present implementations from rendering the UI
more than once for the same presentation call.

One-shot diagnostics identify the path that is actually active:

    [UI] Device Present path active.
    [UI] SwapChain Present path active.

The existing production device-hook set remains 12 methods. v0.0.49 adds one
primary swap-chain Present hook, for 13 D3D9 COM hook points when the fallback
is available.

Failure to obtain or hook the primary swap chain is intentionally non-fatal;
Device::Present and all non-UI DPFix-NG features remain available.

## F10 input fallback

Render-thread GetAsyncKeyState polling remains supported, including games that
consume keyboard input through DirectInput. The subclassed game WndProc now
also queues the initial WM_KEYDOWN/WM_SYSKEYDOWN for the configured UI toggle
key.

The Win32 path only queues an atomic request. The actual open/close transition
still occurs from the render thread. Polling and the Win32 request are ORed into
one edge, so receiving both for the same F10 press cannot toggle the panel twice.

The first observed Win32 fallback is logged as:

    [UI] Win32 toggle-key fallback observed.

## UI rendering diagnostic

The Dear ImGui DX9 rendering path is otherwise unchanged. If native D3D9 rejects
the extra BeginScene used by the overlay, DPFix-NG now logs the first HRESULT:

    [UI] WARNING: BeginScene failed while drawing settings UI (...)

This lets the next compatibility step be based on evidence rather than moving
the UI to EndScene pre-emptively.

## Rendering behavior

No game rendering algorithm, resource-scaling rule, texture ownership policy,
hot-reload behavior, filtering policy or DoF behavior changed in this release.
The v0.0.48 Settings / Diagnostics / About layout is unchanged.

## Native D3D9 smoke test

1. Run without a local DXVK d3d9.dll.
2. Open the game and press F10 once.
3. Confirm the panel opens and closes normally.
4. Check DPFixNG.log for which Present path became active.
5. If F10 is detected but the panel is invisible, look for the BeginScene
   warning and include that line in the test report.
6. Exercise one Apply operation and Additional DoF Blur Off/Soft/Stronger to
   confirm the UI remains interactive.
