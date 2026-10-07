# TBC002 — numeric transition state to extra-presentation gate

**Result: CONNECTED; evidence state: VERIFIED at paired selected static scope.** Known `004011B0` post-tick consumers previously lacked a selected producer relation. This result supplies two numeric producer arms, not a complete live trigger policy or a named UI mode.

## Exact producer/caller relation

Steam `0061F660` captures incoming ECX in ESI. On its selected returning transition arm it calls `0061F170`, then dispatches using saved numeric state. The arm at `0061F70B–0061F71E` passes fields S+260 and S+25C to `006227A0`. It does **not** explicitly reload ECX. New finite proof: `0061F170` has no ECX writes or calls; its eight raw branch-table targets remain inside local field-store/RET4 arms. Thus this exact call preserves ECX and the selected producer receives the same S. S's dynamic class remains UNKNOWN; do not name it CMenu.

Own-PE four-target table `00622824` maps selectors0/2→006227B3,1→006227D6,3→00622805. `006227A0` rejects selector>3. On selector1/secondarg0 it writes `00BD77BC = 2 + (S+22C != 0)` at `006227ED`; on selector3/secondarg0 it writes2 at `0062280C`. Other arms are not silently extra-presentation producers. Selected field-state helper calls/stores follow these writes.

GOG independently matches caller `0061F5E0`, preservation helper `0061F0F0`, producer `00622720`, helper table `0061F194`, and producer table `006227A4`. This is a selected field/branch/argument match, not an address-delta theorem.

## Connected right anchor and limits

Known `004011B0` gates additional work on nonzero `00BD77BC` and clears it at `0040129C`. Its two local **three-iteration** loops are independent of the counter's magnitude. Producer values2/3 do not mean two/three iterations. The optional reset arm tests value1, so these writes alone do not admit that reset arm. Actual calls, success, latency, device/content generations, callback mutations and display cadence remain UNKNOWN. No all-producer census, input-friendly names, dynamic type or coherent transaction is claimed.

## Primary

`audit/targeted_bridges_2026-10-07/PRESENT_PRIMARY.json` and `SCHEDULER_QUALIFIERS_PRIMARY.json` replay paired own-PE/original ASM/reachable selected functions and branch-table bytes. Existing Phase8 timing consumer and sequence153/158 menu-facing caller/adapter qualifications are reused. Their earlier body exclusion/type ceiling is preserved; receiver continuity is established locally, not assumed from calling-convention shorthand.
