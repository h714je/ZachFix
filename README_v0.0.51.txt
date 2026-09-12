# DPFix-NG v0.0.51 UI Toggle Debounce

This checkpoint fixes the double-toggle behavior seen with F10 on native D3D9
after the EndScene UI fallback was introduced in v0.0.50.

The native v0.0.50 log showed the panel repeatedly opening and immediately
closing. Both input paths were valid, but they could observe the same physical
F10 press at different times:

- render-thread GetAsyncKeyState edge polling;
- WM_KEYDOWN / WM_SYSKEYDOWN from the subclassed game WndProc.

If polling consumed the press first and the Win32 message arrived slightly
later, the message queued a second toggle for the next EndScene callback. One
physical press could therefore become Open -> Close.

## Shared physical-press latch

v0.0.51 keeps both input paths for compatibility but makes them share one atomic
press latch. The first source that sees a new F10 press claims it. The other
source sees the latch and does not queue another toggle.

The latch is re-armed only after F10 is observed released, either through
WM_KEYUP / WM_SYSKEYUP or through render-thread polling. This also suppresses
keyboard autorepeat while the key is held.

No timing-based debounce window is used, so quick deliberate F10 presses are not
artificially delayed. The rule is simply one toggle per physical down/up cycle.

## Rendering path

The v0.0.50 EndScene compatibility path is unchanged:

- IDirect3DDevice9::EndScene remains the preferred UI callback once observed;
- Device::Present and primary SwapChain::Present remain fallbacks;
- Dear ImGui renders inside the game's active scene on the EndScene path.

The production hook surface is unchanged from v0.0.50: 13 device hooks plus the
optional primary swap-chain Present hook.

## Suggested native D3D9 test

1. Run without DXVK/ReShade D3D9 wrappers.
2. Confirm `[UI] EndScene UI path active.` appears once.
3. Tap F10 once. The panel should open once and stay open.
4. Tap F10 again. The panel should close once.
5. Hold F10 briefly. It must not chatter open/closed from autorepeat.
6. Repeat several quick taps and verify every physical press produces exactly one
   open/close transition in the log.

No game-rendering feature changed in this checkpoint.
