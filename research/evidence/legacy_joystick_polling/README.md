# Legacy WinMM / DirectInput pathological polling evidence

**Build:** runtime-confirmed on Steam 1.01b, 2026-10-06
**Production boundary:** application-side `joyGetPosEx` retry suppression only
**Do not modify:** registry mappings, winmm.dll, dinput.dll, foreign HKEYs

## Symptom

A degraded runtime state reduced uncapped gameplay from roughly 150-180+ FPS to
about 50-55 FPS. Passive profiling showed most of the missing frame time in
kernel CPU rather than rendering or PhysX.

The process handle count also grew deterministically. The leaked handles were
registry keys below DirectInput calibration paths, dominated by devices seen
during HID enumeration. Logitech hardware was incidental to enumeration, not
the root cause.

## Standalone reproduction

A 32-bit standalone caller using only `winmm!joyGetPosEx` reproduced the problem:

```text
joyId 0 -> rc=165 (JOYERR_PARMS), +8 handles/call, ~4.62 ms/call
joyId 1 -> rc=165 (JOYERR_PARMS), +8 handles/call, ~4.58 ms/call
```

Slots 2-7 could also return 165 but did not leak in the captured state.

The bad state can be forced after controller removal by calling:

```text
joyConfigChanged(0)
```

Healthy disconnected state is different:

```text
JOYERR_UNPLUGGED = 167
```

That path is cheap and does not leak. It must not be suppressed.

## Vanilla DP polling cost

The native input update loops over joystick IDs 0-6 once per update, and the
main frame contains an additional `joyGetPosEx(0)` call. On the affected slots,
the observed per-frame sequence is therefore effectively:

```text
id0
id1
id0
```

At eight leaked handles per bad call:

```text
3 * 8 = 24 handles/frame
```

At the measured call costs:

```text
4.62 + 4.58 + 4.62 ~= 13.82 ms/frame
```

This accounts for the observed collapse to roughly 50 FPS when ordinary frame
work is about 6 ms.

## Windows-side evidence

ProcMon and static mapping place the leaking route below WinMM's legacy
joystick-slot resolution and inside DirectInput device creation/enumeration.
`dinput.dll` repeatedly opens calibration keys, and the pathological invalid
slot path leaves part of that open/close set unbalanced.

The exact internal DirectInput branch is useful RE evidence but is not required
for the ZachFix repair.

## Production repair contract

ZachFix keeps WinMM and DP's controller architecture intact. The fix caches only
the confirmed pathological result:

```text
first JOYERR_PARMS for a slot
    call real joyGetPosEx
    return the real 165 result
    mark slot suppressed

later calls for the same slot
    return JOYERR_PARMS directly
    do not re-enter WinMM
```

The cache is invalidated only by `WM_DEVICECHANGE`. Each previously suppressed
slot is then allowed one real call:

- success: slot recovered;
- `JOYERR_UNPLUGGED`: normal cheap disconnected polling remains live;
- `JOYERR_PARMS`: suppress the slot again.

The same guard is shared by DP.exe's IAT calls and ZachFix's legacy AutoSwitch
fallback, preventing ZachFix-owned activity probes from reintroducing the same
pathological retries.

## Excluded approaches

Do not:

- delete or rewrite joystick registry mappings;
- call `joyConfigChanged` automatically;
- patch `winmm.dll` or `dinput.dll`;
- intercept registry APIs;
- close foreign registry handles from ZachFix;
- periodically retry suppressed slots.

Event-driven revalidation avoids turning recovery itself into a slower handle
leak.
