# CShop positive callback-to-retirement chain

**Date:** 2026-10-02. **Outcome:** ADVANCE. This is a conditional static lifecycle, not measured execution frequency.

## Declared batch

`CSHOP_CALLBACK_RETIREMENT`: follow only the selector-0x34 factory's post-construction callback record, concrete CShop setup/control continuation, and receiver-proven event/retirement paths. Success is a positive class-specific callback-to-retirement connection; negative condition is the first untyped receiver or missing source. No repeated constructor/caller enumeration or generic virtual-offset census.

## Positive active-object portfolio

All mechanics below are `VERIFIED` for the displayed Steam/GOG scopes:

1. **Construction and registration:** selector `0x34` allocates `0x20A8`, constructs CShop, and takes the successful-result tail of `005E7620/005E76F0`. The tail assigns object type `+0x2C`, selector `+0x30`, then calls `006C5AE0/006C55E0` on the same object. Registration links the outer object and assigns its packed handle at `+0x3C`. The null request used by the three known external creation contexts leaves the record's bucket value unoverridden.
2. **Concrete callback installation and initial use:** raw selector record `008BF800` (`008BF660 + 0x34*8`) is Steam bytes `B0 47 63 00 11 00 00 00`, GOG `00 47 63 00 11 00 00 00`: callback `006347B0/00634700`, type `0x11`, bucket `0`. Factory tail `005E956A/005E963A` passes that callback and the concrete object's ECX to `006BAB80/006BAAD0`. The setter stores object `+0x44` and immediately emits event 0 via `00402870/00402860`. The callback adapter forwards the original object as ECX to `0062F9B0/0062F900`; event zero initializes the shop and binds `UPDATA/MENU/SHOP`, `IMG_ITEM`, and `SHARE` resources. Detailed resource ownership is not established here.
3. **Class-discriminated update:** the CShop table's `+0x0C` target is `006BA8A0/006BA7F0`, emitting event 1 through the stored callback. The known manager dispatcher calls slot `+0x0C` in its phase-2 pass. This resolves a concrete CShop target for that manager interface; it does not claim every CShop is selected in every frame. Handler event-one flow checks object state, then switches byte `+0x152C`. Raw branch `0062FA54–0062FA56` / `0062F9A4–0062F9A6` identifies event 1; switch tables `006336F8/00633648` map state 3 to `00633309/00633259`.
4. **Class-discriminated retirement:** in that state-3 arm, when lookup of CShop field `+0x1510` returns null, the handler reaches `00633355–0063335C` / raw GOG `006332A5–006332AC`: it invokes **CShop's** `+0x30` with ECX=ESI (the callback's original this). This is unlike the external native task's separate +0x30 receiver. The raw CShop slot resolves to `006BAB20/006BAA70`; if `00427320/00427340` returns zero, it writes retirement bit `+0x29=0x80`, clears `+0x2A`, and emits event 2.
5. **Teardown continuation:** the existing active-list retirement iterator selects marked objects, removes their manager links, invokes virtual `+0x08`, releases the current handle through `01481358`, and invokes slot zero with deletion flag 1. CShop slot zero is `00634830/00634780`, which calls its class destructor then frees the same object. Thus the manager's generic cleanup now has a concrete CShop-class endpoint. The conditional marker/use mechanics are proven, not runtime reachability or actual deletion timing.

**First missing edges:** reproducible runtime selection/state-3 reachability, exact meaning/producer of CShop's queried `+0x1510` handle, and live update/retirement timing. These do not erase the positive static chain. Other events and menu internals are outside this batch. No stale/non-current handle policy is inferred.

## Non-dispatcher interface closure

CShop's observed 19-slot table is not merely a symbol inventory: `+0x00` has the class deleting target; `+0x0C` has a callback update target and a manager call context; `+0x30` has a class-discriminated handler call context and marker target; `+0x08` has a manager teardown context. Creation/registration/callback/conditional deletion are connected. Remaining slots and actual mode selection remain UNKNOWN. See `cshop_external_handles.md` for raw table extent and external handle retainers.

## Cross-build/export red-team limit

GOG exports only 587 recognized bytes / 26 callees for `0062F900`, versus Steam's 15,308 bytes / 449 callees, because lookup `006C5AD0` is falsely nonreturning. GOG's decompilation is **not** sufficient to assert absence of CShop update or retirement. Raw GOG event branch, state-switch table, adapter, and retirement arm independently match Steam's mechanics; this match is scoped to those anchors, not a complete equivalence of every menu operation. No fabricated GOG function or callee count replaces the supplied export.

## Primary evidence

- Factory successful tail: Steam `inputs/decompiler/steam/DP_decompiled.c:312019-312102`; GOG `inputs/decompiler/gog/DP_decompiled.c:240965-241048`; raw Steam `inputs/decompiler/steam/DP_full.asm:549189-549196`; raw GOG callback installation reproduced below.
- Setter/dispatch: Steam `inputs/decompiler/steam/DP_decompiled.c:434799-434811`, `:1540-1556`; GOG `inputs/decompiler/gog/DP_decompiled.c:332556-332568`, `:1529-1545`.
- Adapter: Steam `inputs/decompiler/steam/DP_full.asm:632998-633004`; GOG binary VA `00634700–00634713`.
- Event-one wrapper: Steam `inputs/decompiler/steam/DP_decompiled.c:434629-434640`; GOG `inputs/decompiler/gog/DP_decompiled.c:332386-332397`. Class table bytes: the external-handles report.
- Steam handler: `inputs/decompiler/steam/DP_decompiled.c:354348-356532`, especially `:355691-355700`, `:356478-356514`; raw event branch `inputs/decompiler/steam/DP_full.asm:628299-628307`, state switch `:630547-630554`, retirement `:631936-631950`. GOG event branch `inputs/decompiler/gog/DP_full.asm:478568-478576`; remaining anchors use executable bytes, not the truncated export.
- Marker: Steam `inputs/decompiler/steam/DP_decompiled.c:434778-434798`; GOG `inputs/decompiler/gog/DP_decompiled.c:332535-332555`. Retirement/deletion: Steam `inputs/decompiler/steam/DP_decompiled.c:441308-441401`; GOG `inputs/decompiler/gog/DP_decompiled.c:338466-338559`; plus `object_handle_lifecycle.md`, `cthrowlure_retirement_marker_iterator.md`, and the paired CShop deletion report.

Reproduce raw GOG anchors with `objdump -d -Mintel` on `inputs/binaries/gog/DP_GOG.exe`, bounded ranges `0x634700..0x634714`, `0x631c50..0x631c68`, `0x633259..0x6332ae`, and `0x5e9628..0x5e963f`. Decode the state table at `00633648` as seven little-endian pointers; its third entry is `00633259`. Raw selector record is at `008BF800` in both **independently decoded** binaries.

## Successor selection card

Choose `AUDIO_DIRECTSHOW_GAME_OWNER`, family AUDIO_ROOT_LIFECYCLE. Payoff: a major underrepresented root and game-to-media ownership boundary rather than a third adjacent-menu batch. New source: direct game-owned caller/acquisition and shutdown contexts of the already known graph setup `00735D00/00735A10` and release `00736300/00736010`. Prior limit: BND-077 bounded notifications/cadence, not the higher-level acquiring caller. Cost: one paired direct-caller/owner chain. Useful negative: identify exactly where the chain stops before assuming all audio uses DirectShow. No DirectSound import was found while checking scheduling availability; no middleware-absence claim follows from that import subset.
