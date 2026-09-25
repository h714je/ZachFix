# Building day/night / HOUSE_LIST.NOD

## Production status

The visible Director's Cut building/window day-night regression is **production-fixed and visually verified**.

The production fix restores native game authority over affected `N_WINDOW` geometry. It is not a shader-hash suppression and does not implement a replacement time-of-day system.

## Resource identity

Xbox resource ID:

```text
0x39DF = UPDATA/PRM/HOUSE_LIST.NOD
```

Structure:

```text
81 records × 0x50 bytes = 6480 bytes
```

Normalized Xbox and PC payloads are byte-for-byte identical.

Therefore the old framing:

```text
"PC ships a corrupt/swapped HOUSE_LIST asset"
```

is wrong.

## Runtime endian failure

The loaded PC runtime table shows:

```text
17 / 81 keys converted to normal host little-endian
64 / 81 keys still retaining Xbox byte order
```

The converted records are:

```text
0, 2, 4, ... 32
```

The native CLevel consumer then performs an ordinary unswapped 16-bit lookup. Records whose keys were not converted miss the lookup, breaking the configured day/night behavior.

## Production repair

The current safe repair preserves native direct matches and adds a unique byte-swapped-key fallback for the broken records.

This is intentionally narrower than altering the whole generic resource preprocessor.

## Still open

The exact generic preprocessing operation that creates the peculiar 17/64 pattern remains unresolved.

A plausible `0xA0`-stride/every-other-record converter bug was discussed, but it is **NOT PROVEN** and must not be promoted to fact.

Future root-cause work should trace the resource-registration/preprocess path around the retained `FUN_006B1310` trail and prove the exact conversion callback on both Steam and GOG before replacing the production repair.

## Evidence

- `ZachFix-HOUSE_LIST-endian-research-handoff-2026-09-19.md`
- production `house_list_fix.*`
