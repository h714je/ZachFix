# Long-uptime game-time precision evidence

**Builds:** Steam 1.01b and GOG 1.01b
**Runtime confirmation:** Steam, 2026-10-06
**Production boundary:** scoped x87 PC53 only inside two native absolute-QPC conversion helpers

## Prior discovery and ZachFix confirmation

Cesario67's `DeadlyPremonitionFix` had already identified and published this
absolute-QPC/x87 precision failure before the ZachFix investigation reached the
same cause. Its public analysis identifies Steam `0x401F50` and `0x701040`, the
missing `D3DCREATE_FPU_PRESERVE` flag, and the loss of timing resolution as
system uptime grows.

ZachFix independently reached the same mechanism on 2026-10-06 while tracing a
long-running Windows session. The implementation here is independently written,
but discovery priority for the QPC/x87 bug belongs to Cesario67. ZachFix's
contribution is an independent runtime reproduction, its own paired Steam/GOG
signature gating, and a deliberately narrow scoped-PC53 implementation that
coexists with the separately proven PC24-sensitive aim path.

Upstream research:

- https://github.com/Cesario67/DeadlyPremonitionFix
- `docs/analyse-dp-exe.md`, section `Temps et cadence`

## Runtime symptom

At roughly 91.7 hours of Windows uptime, ZachFix captured:

```text
real QPF      = 10,000,000
DP stored QPF = 10,000,000
wallTickHz    ~= 60
rawTickHz     ~= 60
finalTickHz   ~= 64
```

The raw DP timer was dominated by `0` and approximately `1.875`. At the observed
~330,000-second uptime, PC24 spacing for the absolute seconds value is 31.25 ms:

```text
31.25 ms * 60 = 1.875 raw ticks
native mode-0 transform: 1.875 -> 2.0
~32 nonzero samples/s * 2 ~= 64 final ticks/s
```

QPC and QPF themselves were correct. Precision was lost when DP converted the
large absolute counter value before subtracting timestamps.

## Affected helpers

Steam 1.01b:

```text
00401F50 / RVA 00001F50  absolute QPC -> integer microseconds
00701040 / RVA 00301040  absolute QPC / QPF -> seconds
```

GOG 1.01b:

```text
00401F50 / RVA 00001F50  absolute QPC -> integer microseconds
00700FA0 / RVA 00300FA0  absolute QPC / QPF -> seconds
```

DP creates its D3D9 device with behavior flags `0x44`, without
`D3DCREATE_FPU_PRESERVE`. The gameplay thread can therefore remain in x87 PC24.

## Production repair contract

The fix intentionally does not change global precision:

```text
enter native QPC conversion helper
    save caller x87 precision-control field
    set only precision-control bits to PC53
    execute the unmodified native helper
    restore only the caller's original precision-control bits
return
```

This is required because the native mode-2 aim path contains a separate
precision-sensitive exact-equality boundary that is known to behave correctly
in PC24 and can fail under PC53.

## Runtime acceptance

With the fix enabled on a long-uptime session:

- the 31.25 ms staircase must disappear;
- `dpRaw` must track wall time continuously again;
- the old `0 / ~1.875 -> 0 / 2` pattern must disappear;
- caller precision-control bits must be restored after each hook;
- aim behavior must remain unchanged.
