# Selector Object Factory Boundary

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for selector-to-constructor dispatch; complete class mapping remains open.

Steam `005E7620` / GOG `005E76F0` are paired selector factories. They switch on a selector byte, allocate through the build allocator, and route cases to concrete constructors such as `0052DF70`, `004B9440`, `0048D5C0`, `004B7720`, `00484D90`, `0048A140`, `00486CC0`, `005F2930`, `004893F0`, `004A1940`, and many additional game-owned constructors. Several cases allocate directly through specialized helpers.

The factory is called from `0041D3E0`, scene/resource setup, actor/effect paths, and many gameplay modules. Selector `0x28` is used immediately before `006BE6E0` in the Phase 1 actor/resource setup path and is resolved separately as CLevel; only its CLevel-specific `+0x4C` slot mapping applies to that class.

## Verified selector slice `0x34`–`0x37`

| Selector | Steam allocation → constructor → vtable | GOG allocation → constructor → vtable | Identity | State |
|---|---|---|---|---|
| `0x34` | `0x20A8` → `0062E0E0` → `00780FBC` | `0x20A8` → `0062E030` → `00780FAC` | CShop | `VERIFIED` |
| `0x35` | `0x898` → `0062E250` → `0078100C` | `0x898` → `0062E1A0` → `00780FFC` | CHelp | `VERIFIED` |
| `0x36` | `0x178` → `0062BBB0` → `00780C94` | `0x178` → `0062BB30` → `00780C84` | CPassword | `VERIFIED` |
| `0x37` | `0x198` → `0062BC10` → `00780CE4` | `0x198` → `0062BB90` → `00780CD4` | CChess | `VERIFIED` |

Each constructor performs a direct named-vtable write. CShop destructors (`0062E1B0` Steam; `0062E100` GOG) and CHelp destructors (`0062E2D0` Steam; `0062E220` GOG) rewrite their class vtables before base teardown. Constructor names and `PDATA/MENU/CHESS` string evidence support placing this slice in the native-menu/UI cluster, but no complete menu lifecycle, virtual-slot map, or render path is established here.

## Verified selector `0x38`

Selector `0x38` allocates `0x170` bytes and dispatches to CFishing constructors `0060EED0` (Steam) and `0060EFE0` (GOG), which directly write vtables `00780234` and `00780224`. Paired teardown candidates `0060D630/0060D850` rewrite the CFishing vtable, conditionally invoke an object virtual slot, restore `CRdObject::vftable`, and call base cleanup. CFishing task lifecycle, virtual-slot semantics, and rendering remain open.

This boundary supplies the correct route to resolve remaining object ownership: map each selector to its constructor/vtable and inspect the relevant class's slot table, rather than inferring a target from the generic handoff.
