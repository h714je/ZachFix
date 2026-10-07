# TBC010 — initial raw selected-slot flag to known controller consumer

**Result: CONNECTED; VERIFIED paired initialization path only.** Later selection policy, acquisition/device success and complete action bindings remain UNKNOWN.

Known typed input constructors and the I668 raw-region/first-selected-byte consumer were missing their initial selected-flag producer. New exact Steam site `0070802B→00709BB0` and independent GOG `00707FDB→00709B20` explicitly offer **I+668**. Targets are nominated from each build's call bytes, not by a uniform relocation.

Both initializer bodies clear only bytes **+0 and +1** of seven rows with stride**0x36**, then set **row0 byte+0=1**. They do not clear every row byte or certify complete input reset. Both known selected-consumer windows compare each row's **byte+0 exactly to1**, not nonzero or bit0, selecting the first matching row. Thus row0-only initial selection is established.

Byte+1 is a separate location; its acquisition-result role is inherited from the prior raw poll chain, not newly proved by clearing it. Selecting row0 does not establish a physically present/successfully acquired device0 or later default policy.

The initializer also issues a cursor-position request, converts two stack DWORDs to float globals and clears four other globals. No API success test protects those conversions; report request/store mechanics, not successful coordinate acquisition.

Primary: `audit/targeted_bridges_2026-10-07/INPUT_INITIAL_PRIMARY.json`—50 selected instructions/168 bytes per build including exact constructor/consumer windows; initializer37/126 per build. Own-PE/original-ASM and independent paired review pass. Existing typed roots/raw/action/aggregate flow reused, not re-enumerated.

UNKNOWN: later flag writes, complete selected-slot/device topology, bindings/action-friendly names, physical acquisition validity, callback/root epochs, input-worker activity, cadence, complete reset/readiness, ownership and successful cursor values. No B0010 reopening or all-consumer claim.
