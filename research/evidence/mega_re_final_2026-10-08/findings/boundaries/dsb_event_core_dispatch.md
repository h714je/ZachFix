# DSB Event-Core Dispatch Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for DSB payload-to-event-core slot dispatch; DSB grammar remains `UNKNOWN`.

The generic typed resource callback’s `.DSB` branch initializes event core then calls paired `0070DD40/0070DCE0` with the payload/state pointer and descriptor byte.

The paired wrapper:

1. resolves a signed slot through `0070E820/0070E7C0` from the payload;
2. returns failure if the resolver yields `-1`;
3. otherwise activates/updates the resolved slot through `00710730/00710690`;
4. forwards the descriptor byte to paired command/state helper `0072A5A0/0072A2B0`.

This establishes DSB resource -> event-core slot -> command/state dispatch. It does not establish DSB binary grammar, slot meaning, or event script semantics.
