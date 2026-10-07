# CLevel Virtual +0x4C Mapping

**Date:** 2026-10-01
**Evidence state:** `VERIFIED`.

Raw `.rdata` vtable bytes resolve the CLevel slot at byte offset `+0x4C`:

- Steam CLevel vtable `007798AC + 0x4C` -> `006C0620`.
- GOG CLevel vtable `0077989C + 0x4C` -> `006C04D0`.

The paired bodies are build-specific but CLevel-owned through this vtable slot. Steam `006C0620` handles CLevel resource/attachment update branches, including manager lookups, `006B9130`, `006BA680`, and `006C07D0`; GOG `006C04D0` performs paired attachment allocation/update paths and calls `006B8F70`, `006B9150`, and `006C02E0`.

This resolves the `006BE6E0` owner virtual handoff when the owner is selector-0x28 CLevel. It does not prove that every `006BE6E0` caller owns CLevel; selector-specific class mapping remains required for other cases.

Evidence: Steam/GOG raw `.rdata` dumps around `007798AC/0077989C`; Steam `DP_decompiled.c:437820-437880`; GOG `DP_decompiled.c:335482-335532`; selector factory and CLevel class findings.
