# Frame D3D9 Cooperative-Level Gate

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired cooperative-level/frame-permission behavior.

The idle application loop calls Steam `006CCEF0` / GOG `006CC990` immediately before timing and frame-root `00401A70`.

Each paired gate calls the D3D9 device's `TestCooperativeLevel` virtual method. On success it returns `1`, permitting the idle loop’s frame work. On a failure:

- for `D3DERR_DEVICELOST` (`0x88760868` as signed `-0x7789f798`), it sleeps 50ms and returns `0`;
- for failures other than `D3DERR_DEVICENOTRESET` (`0x88760869`), it likewise sleeps 50ms and returns `0`;
- for `D3DERR_DEVICENOTRESET`, it runs pre-reset helper, invokes device `Reset`, runs post-reset handler on success, then returns `1`; failed reset sleeps 50ms and returns `0`.

This verifies that device cooperative state gates eligible idle frame invocation. The semantics of reset helper internals remain separate.
