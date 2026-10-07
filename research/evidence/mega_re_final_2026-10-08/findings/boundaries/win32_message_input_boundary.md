# Win32 Message to Engine Input/Application Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired message routing and direct global/helper handoff.

Paired application window procedures Steam `00700C90` / GOG `00700BF0`, registered by the outer application loop, route Win32 messages into engine state/helpers:

- `WM_KEYDOWN` / `WM_KEYUP`: update key-state byte `014AFFB4`;
- `WM_ACTIVATE`: forwards activation parameters to `004012D0`;
- `WM_SIZE`: forwards dimensions to `004012E0`;
- `WM_ACTIVATEAPP`: stores low-word activation state at `014AFFB0`;
- `WM_MOUSEWHEEL`: stores signed wheel delta at `014AFFC0`;
- mouse move/button messages (`0x200`, `0x201`, `0x202`, `0x204`, `0x205`, `0x207`, `0x208`): store mouse coordinates at `014AFFC8/C4`, obtain keyboard state, then call shared translator `004012B0`;
- `WM_DROPFILES`: enumerate/uppercase files and forward drag lifecycle/files to `004012C0`;
- `WM_DESTROY`: post quit and invoke optional shutdown callback `014AFFBC`;
- custom `0x8001`: route to paired engine helper.

This verifies OS message -> input/application global/helper boundary. It does not establish downstream gameplay/input mapping after `004012B0`.
