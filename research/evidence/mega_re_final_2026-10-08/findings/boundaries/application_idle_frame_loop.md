# Application Idle Frame Loop

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired message-loop/frame-root ordering; real-time cadence remains bounded by observed code only.

Paired outer application functions Steam `00700650` / GOG `00700670` establish the top-level Windows loop after startup initializer `004017C0` succeeds:

1. loop until `WM_QUIT` (`message == 0x12`);
2. `PeekMessageA`; when a message exists, `TranslateMessage` and `DispatchMessageA`;
3. when idle, run a frame-execution gate (`006CCEF0` Steam / `006CC990` GOG);
4. obtain a timing value, derive frame delta using target scalar `60.0`, clamp to `8.0`, and update shared frame/timing globals;
5. invoke `00401A70` exactly once per eligible idle iteration;
6. run post-frame helper and foreground-window cursor visibility handling.

Steam additionally performs Steam initialization/shutdown around the same Windows loop; GOG has the structurally matched non-Steam loop.

This verifies application lifecycle -> timing -> frame-root ordering. It does not prove real-world frequency, wall-clock unit semantics beyond the direct arithmetic, or behavior when the frame gate denies execution.
