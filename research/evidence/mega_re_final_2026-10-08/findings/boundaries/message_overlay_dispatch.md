# Message / Overlay Dispatch Boundary

**Date:** 2026-10-01
**Evidence state:** `STRONG_INFERENCE` for CMessage/UI ownership.

Steam `0045C9F0` and GOG `0045CA20` are paired 9.6KB, high-centrality functions called from many UI/gameplay paths. Their primary structure shows:

- Lazy initialization of a `0x124D0` `CSingleton<CMessage>` object and vtable write.
- Message-ID/state handling through `0045A590`, `0045A630`, and scene/state globals.
- Resource/UI helper calls including XPC2 child accessors `006B5590` and texture/UI state helpers.
- Overlay/text geometry emission through `0045A310`, `0045B710`, `0045F6E0`, and `0045F7B0`.
- Coordinate, scale, color, and message-ID parameters at the public call boundary; callsites include menu, event, and gameplay overlays.

This is a high-centrality message/overlay boundary connecting resource-backed UI data to native UI/render output. Exact message taxonomy, CMessage ownership, and final D3D9 draw ownership remain open.

Evidence: `inputs/decompiler/steam/DP_decompiled.c:72222-73340`; paired GOG `0045CA20`; `inputs/decompiler/steam/calls.csv:9891-9920`; `findings/boundaries/xpc2_d3d9_texture.md`.
