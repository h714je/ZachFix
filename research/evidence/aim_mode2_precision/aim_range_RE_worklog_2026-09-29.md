# ZachFix — Aim Range Bug RE Worklog

**Date:** 2026-09-29
**Purpose:** Persistent scratch/work log for the five remaining static-RE branches of the long-standing Deadly Premonition: Director's Cut PC aiming-range bug.

This file is intentionally a running notebook, not a polished summary. Every branch below must be updated as soon as new evidence appears so the investigation can resume safely after interruption.

---

## Problem summary

Observed reporter symptom:

- aiming camera range becomes restricted in both horizontal and vertical directions;
- Alt+Tab out/in immediately restores normal aiming;
- issue is not reproduced locally;
- save file does not reproduce it locally;
- survives ZachFix native SDL3/direct-CInput gamepad path;
- therefore old XInput -> JOYINFOEX compatibility path is very unlikely to be the root cause.

Important known PC/Xbox architectural difference:

- PC Player state setter selects camera mode from the Player-state -> camera-mode table and calls the camera mode setter;
- Xbox does the equivalent;
- Xbox mode-2 entry re-anchors aim orientation and clears mode-2 transient state unconditionally;
- PC mode-2 entry performs that initialization only when `previousCameraMode != 2`.

PC confirmed functions / fields:

- Player state setter: `FUN_00528F40`
- Player current state: `Player+0x654`
- Player previous state: `Player+0x658`
- Player state -> camera mode table: `DAT_008A9980[state]`
- Camera mode setter: `FUN_00534C60`
- Camera current/previous mode field involved in gate: `CCamera+0x158`
- Aim orientation / base fields: `CCamera+0x6C/+0x70` and copies around `+0x5C/+0x60`
- Mode-2 transient block initialized on fresh entry: `CCamera+0x8C..+0xA8`
- Relevant flag word/bit: `CCamera+0x120 bit0`

Xbox confirmed equivalents found during this pass:

- Player state setter: `sub_8232A300`
- Player current state: `Player+0x724`
- Player previous state: `Player+0x728`
- Camera mode setter: `sub_82336130`

Known correction from this session:

- Earlier tentative identification of `0x004E0180` as Player state `0x0C` was wrong because the dispatch table base was offset.
- Correct dispatch base is `PTR_FUN_008A9758`.
- `0x004E0180` is state `0x0E`, camera mode 0, so its weapon ingress is a normal `0 -> 2` entry and not the bug producer.

---

# Branch 1 — Writers of `CCamera+0x120 bit0`

## Why this matters

Mode-2 aim code checks `CCamera+0x120 bit0` on PC. When the bit is clear, the mode-2 consumer explicitly zeroes the corresponding live transient pair (`PC +0x8C/+0x90`) instead of retaining it across frames.

## Result: writer census CLOSED

The earlier note that the writer was unknown is superseded. The writers are directly inside the weapon-state handlers and form a systematic per-state policy.

### PC Steam writer table

| Player state | PC writer | Operation |
|---|---:|---|
| `0x14` | `0x0050EC51` | CLEAR bit0 |
| `0x16` | `0x005106C9` | CLEAR bit0 |
| `0x18` | `0x00512561` | CLEAR bit0 |
| `0x1A` | `0x005134CC` | SET bit0 |
| `0x1C` | `0x005148B7` | SET bit0 |
| `0x1E` | `0x00515BE4` | SET bit0 |
| `0x20` | `0x00518501` | SET bit0 |
| `0x22` | `0x005170AA` | SET bit0 |
| `0x24` | `0x0051A998` | CLEAR bit0 |
| `0x26` | `0x0051E929` | CLEAR bit0 |
| `0x28` | `0x0051B635` | CLEAR bit0 |
| `0x2A` | `0x0051C2F8` | SET bit0 |
| `0x2B` | `0x0051CF9A` | SET bit0 |

State `0x2C` (camera mode 0) also clears the PC bit at `0x0051FBFF`.

### Xbox correspondence

The Xbox camera layout is shorter at this point. The directly corresponding flag is `Xbox CCamera+0x110` (decimal 272), not Xbox `+0x120`.

Evidence for the layout shift:

- Xbox camera mode setter stores the mode at `CCamera+0x144` (decimal 324), while PC uses `CCamera+0x154`;
- Xbox fresh mode-2 reset zeroes fields through `+0x98`;
- PC contains an additional 0x10-byte region after that point, shifting later equivalent fields by `+0x10`.

The Xbox weapon handlers reproduce the same bit policy exactly:

| Xbox Player state | Xbox operation on `CCamera+0x110 bit0` | PC match |
|---|---|---|
| `0x14` (20) | CLEAR | yes |
| `0x16` (22) | CLEAR | yes |
| `0x18` (24) | CLEAR | yes |
| `0x1A` (26) | SET | yes |
| `0x1C` (28) | SET | yes |
| `0x1E` (30) | SET | yes |
| `0x20` (32) | SET | yes |
| `0x22` (34) | SET | yes |
| `0x24` (36) | CLEAR | yes |
| `0x26` (38) | CLEAR | yes |
| `0x28` (40) | CLEAR | yes |
| `0x2A` (42) | SET | yes |
| `0x2B` (43) | SET | yes |

Representative Xbox functions found during the census include `sub_8230F228` (`0x14` clear), `sub_823194F8` (`0x1A` set), `sub_82315740` (`0x24` clear), `sub_8231F1B8` (`0x2A` set), and `sub_82320330` (`0x2B` set).

## Interpretation

`CCamera+0x120 bit0` is **not a PC-only writer-policy bug**. The per-weapon set/clear semantics are preserved from Xbox.

It remains relevant as a downstream persistence policy because it decides whether transient aim offsets survive a frame, but it should no longer be treated as the primary source of the regression.

This also strengthens the importance of the PC-only mode-2 reset gate: the same weapon policy is running on both versions, but Xbox begins a mode-2 state request from freshly reset camera transients while PC can skip that reset if mode 2 was already active.

## Status

`CLOSED — PC/Xbox writer policy matches; downstream amplifier only`

---

# Branch 2 — Other writers/preservation paths for `CCamera+0x8C..+0xA8`

## Why this matters

PC's fresh mode-2 initialization clears:

```text
+0x8C
+0x90
+0x94
+0x98
+0x9C
+0xA0
+0xA4
+0xA8
```

But PC skips that block when camera mode is already 2.

The transient block contains values involved in aim behavior on both axes. Therefore stale values here are one of the strongest mechanisms capable of explaining simultaneous X/Y range reduction.

## Current evidence

**Confirmed:**

- Fresh PC `non-2 -> 2` entry clears the full block.
- Xbox mode-2 setter performs the equivalent reset unconditionally.
- PC does not reset it when `previousCameraMode == 2`.
- The live PC mode-2 handler is `FUN_0053B8B0`.
- Only four members of the eight-field reset block are active in the live aim integrator:
  - `CCamera+0x9C` = vertical/pitch target accumulator;
  - `CCamera+0x8C` = smoothed vertical/pitch offset;
  - `CCamera+0xA0` = horizontal/yaw target accumulator;
  - `CCamera+0x90` = smoothed horizontal/yaw offset.
- The vertical accumulator feeds `CCamera+0x6C` (the pitch/orientation value subsequently clamped to `-35 deg / +55 deg`).
- The horizontal accumulator feeds `CCamera+0x70` / yaw-side orientation handling.
- `CCamera+0x94/+0x98/+0xA4/+0xA8` are cleared by fresh mode-2 initialization but are not consumed by `FUN_0053B8B0`; they are therefore lower priority for the reported simultaneous X/Y restriction.
- When `CCamera+0x120 bit0` is clear, `FUN_0053B8B0` zeroes the smoothed pair `+0x8C/+0x90` at the end of the frame.
- `FUN_004FF460`, called by the common Player update for the weapon/aim state family, implements target-selection/lock behavior. When its target-cycle flag (`Game/global +0x8C5C8 & 0x100`) is active, it explicitly clears the entire `+0x8C..+0xA8` block. This is a second real reset path independent of the camera setter.
- A second PC writer has now been isolated: `FUN_004E8790`, called only from `FUN_004E8BF0`. It directly updates the same active quartet while AIM is held:
  - `+0xA0` yaw target accumulator;
  - `+0x90` smoothed yaw offset;
  - `+0x9C` pitch target accumulator;
  - `+0x8C` smoothed pitch offset.
  It reads controller pair-1 axes through `FUN_00708A30(...,1,0/1)`, uses a small approximately `±0.09816 rad` target window, and then clamps/wraps the shared pitch/yaw orientation. This is not merely a reset site; it is a genuine alternate producer of the same aim transient state.
- `FUN_004E8790` is reached from `FUN_004E8BF0`, which is itself selected by the `FUN_006049A0` sub-state machine at its internal state `8`. The enclosing path is weapon/aim related and tests the AIM action `0x800000`, but its exact Player-state relationship is still being classified.
- Xbox field correspondence is now explicit from the mode-2 setter/layout: PC `+0x8C/+0x90/+0x9C/+0xA0` correspond to Xbox `+0x7C/+0x80/+0x8C/+0x90` (a `-0x10` layout shift in this region).
- A targeted Xbox census of functions that construct the known camera singleton base and write that quartet found a small finite set of writers. Crucially, `sub_8233B3C0` is the Xbox live mode-2 aim handler: it reads/writes the quartet and tests Xbox `CCamera+0x110 bit0`, matching PC `FUN_0053B8B0` / `+0x120 bit0`. The remaining Xbox quartet writers are being classified against PC reset/alternate-writer paths instead of searching the whole binary blindly.

**Interpretation:**

The likely stale-data mechanism is now narrower: if the bug is in this block, the important quartet is `+0x8C/+0x90/+0x9C/+0xA0`, not all eight fields. A stale pair can alter both axes at once. The remaining four fields are still tracked for completeness but no longer lead this branch.

## Writer/reset census closure

The active-quartet census is now classified far enough to close this branch as an independent PC-only regression source. The initially suspicious Xbox-only-looking cleanup path was a census artifact: PC implements the same operation through a helper rather than direct stores.

Confirmed PC/Xbox pairs:

| Role | PC Steam | Xbox 360 | Result |
|---|---|---|---|
| alternate small-window aim producer | `FUN_004E8790` | `sub_822EA858` | MATCH |
| target-lock / cycle reset of full transient block | `FUN_004FF460` | `sub_82302EC8` | MATCH |
| live camera mode-2 aim integrator | `FUN_0053B8B0` | `sub_8233B3C0` | MATCH |
| `0x2000` decay/cleanup toward neutral | `FUN_005354D0` | `sub_823367B0` | MATCH |
| broad camera/minigame restore reset | `FUN_00606B50` | `sub_823DF940` | MATCH |
| broad camera/world restore reset | `FUN_00608D20` | `sub_823E1BF0` | MATCH |
| fresh camera-mode-2 initialization | `FUN_00534C60` | `sub_82336130` | **DIFFERS**: PC gates reset on previous mode != 2; Xbox resets unconditionally |

### Important cleanup-path correction

Xbox `sub_823367B0` initially looked like a potentially missing PC cleanup. It is not missing. One matched caller sequence proves the correspondence directly:

```text
Xbox sub_82330458:
    clear Player flag bits
    set camera low flags
    -> sub_823367B0
    -> sub_82338270

PC FUN_0052EBC0:
    clear the same Player flag family
    set camera low flags
    -> FUN_005354D0
    -> FUN_00537270
```

`FUN_005354D0` checks PC `CCamera+0x11C & 0x2000`, calls `FUN_00534900` to move base/aim fields including `+0x8C/+0x90/+0x9C/+0xA0` toward their neutral targets using step `0.1`, and clears `0x2000` only when all six tracked values have converged. Xbox `sub_823367B0` performs the same policy explicitly with its `-0x10` camera-layout offsets and clears Xbox `CCamera+0x10C bit 0x2000` when convergence completes.

### Giant Player/event handler direct resets also match

The broad Xbox writer census showed direct quartet zeroing inside `sub_82330458`, which initially appeared absent from the PC direct-DAT census. PC `FUN_0052EBC0` does contain the corresponding resets, but obtains `CCamera` through `FUN_00449550()` and writes through the returned pointer, so the earlier simplistic `DAT_00BE1EA4[...]` scan missed them. Multiple branches explicitly zero PC `+0x8C/+0x90/+0x9C/+0xA0`, matching the Xbox large-handler reset behavior.

### Census false positives / reads

Several PC hits from the first lexical scan were not camera-transient writers at all (`FUN_005577D0`, `FUN_00579870`, `FUN_0062F9B0`) or only consumed the fields (`FUN_004FFF40`, `FUN_0052EBC0` in other branches, `FUN_00605CD0`). Likewise several Xbox lexical hits were stack/other-structure accesses. They are not evidence of an unmatched aim writer.

## Interpretation

The active quartet can absolutely persist and influence both axes, but the **producer/reset policy outside the camera mode setter is preserved between PC and Xbox**. The surviving platform-specific difference remains the mode-2 initialization policy itself:

```text
Xbox state request -> camera setter(2) -> fresh quartet reset every time
PC state request   -> camera setter(2) -> reset only when previous camera mode != 2
```

Therefore stale `+0x8C/+0x90/+0x9C/+0xA0` remains a plausible mechanism, but Branch 2 no longer supplies a second independent PC-only bug. It instead strengthens Branch 5: if a reachable repeated mode-2 request exists, the setter gate is the unique reset-policy divergence found in this subsystem.

## Remaining note

It is still useful to understand whether normal AIM release naturally drives the quartet to zero or merely leaves mode 2 and relies on the next fresh entry. That question affects the Alt+Tab narrative, but it is no longer required to classify the writer/reset mismatch.

## Status

`CLOSED — all meaningful quartet producer/reset paths matched; only camera mode-2 setter policy differs`

---

# Branch 3 — External ingress that requests Player state `0x01`

## Why this matters

Internal mode-2 handlers mostly leave through mode-0 states. Therefore ordinary `14 -> 16 -> 18` style direct mode2->mode2 transitions are not common.

External logic can still request Player state changes independently of the current handler. Re-requesting state `0x01` while camera mode is already 2 is exactly where PC and Xbox camera reset semantics diverge.

Two direct PC producers of state `0x01` are now identified. One is proven safe with respect to the reset-skip bug; one scripted/event producer remains open.

## Producer A — ordinary gameplay/input path: CLOSED SAFE

`FUN_004FF100` contains a normal gameplay-looking state-01 request:

```text
input/current-action test 0x800000
    -> FUN_004FEE60(1)
    -> FUN_00520860(...)
    -> CCamera+0x11C |= 0x10
    -> FUN_00528F40(1)
```

Xbox has the corresponding path in `sub_82300398`, including the equivalent mask/gate helper and `sub_8232A300(Player, 1)`.

At first this looked like a strong candidate because the producer is in ordinary Player/input update logic. However its caller at PC `0x00503C60` dispatches on the **current Player state** before entering this routine. The dispatch byte table selects the `FUN_004FF100` call at `0x00503C84` only for current state `0x00`.

Relevant dispatch result:

```text
current Player state 00
    -> FUN_004FF100
    -> may request state 01

current Player state 01 / weapon mode-2 family
    -> different dispatch target
    -> FUN_004FF100 is not entered
```

Therefore this producer can only perform:

```text
state 00 / camera mode 0
    -> state 01 / camera mode 2
```

PC necessarily sees `previousCameraMode != 2`, so its full mode-2 re-anchor/transient reset runs. This path cannot produce the suspected mode2->mode2 reset skip.

**Status:** `CLOSED SAFE — ordinary gameplay producer is gated to current state 00`

## Producer B — scripted/event opcode `0x16`: OPEN

`FUN_004FFB80` is a direct external producer:

```text
CCamera+0x11C |= 0x10
FUN_00528F40(1)
Game/global +0x8C5C8 |= 0x800000
```

Its paired inverse `FUN_004FFBE0` does:

```text
CCamera+0x11C &= ~0x10
FUN_00528F40(0)
Game/global +0x8C5C8 &= ~0x800000
```

The direct caller pair currently identified is event/interpreter opcode `0x16`:

```text
opcode 0x16, argument != 0
    -> FUN_004FFB80
    -> request state 01 / mode 2

opcode 0x16, argument == 0
    -> FUN_004FFBE0
    -> request state 00 / mode 0
```

Unlike Producer A, this route is outside the ordinary Player-state dispatch and no previous-state restriction has yet been found at the helper itself. It therefore remains capable in principle of requesting state 01 while camera mode is already 2.

## Xbox status

Xbox ordinary Producer A has been found and is behaviorally equivalent at the producer level; the important difference remains the Xbox camera setter's unconditional mode-2 reset versus PC's `previousCameraMode != 2` gate.

The scripted path is now identified exactly as well. In the Xbox event interpreter the corresponding switch arm is the same logical opcode `0x16` (the recomp switch label is `case 21` after the interpreter's opcode-minus-one normalization). Its nonzero/zero branches call:

```text
Xbox opcode 0x16, arg != 0
    -> sub_82300728
    -> CCamera-equivalent flag |= 0x10
    -> sub_8232A300(Player, 1)
    -> global force-aim/event bit |= 0x800000

Xbox opcode 0x16, arg == 0
    -> sub_82300788
    -> CCamera-equivalent flag &= ~0x10
    -> sub_8232A300(Player, 0)
    -> global force-aim/event bit &= ~0x800000
```

This is a direct semantic match for PC `FUN_004FFB80/FUN_004FFBE0`.

### Important duplicate-call result

Neither PC nor Xbox helper checks the current Player state, current camera mode, existing `0x10` camera flag, or existing `0x800000` global bit before requesting state 1/0. Therefore a repeated scripted command:

```text
0x16, 1
0x16, 1
```

without an intervening `0x16,0` is mechanically allowed by both interpreters. On the second request the behavioral split is exactly the suspected one:

```text
Xbox: state 1 requested while camera already mode 2
      -> camera setter(2)
      -> fresh aim re-anchor/reset

PC:   state 1 requested while camera already mode 2
      -> camera setter(2)
      -> previousCameraMode == 2
      -> re-anchor/reset skipped
```

So scripted opcode `0x16` is now a **mechanically sufficient producer** of the PC/Xbox reset-policy difference. What is still not proven is whether shipped event content actually issues a repeated nonzero command in a context relevant to the reporter.

### Asset-path correction

`D01.XPF` is not itself the event opcode stream. Static strings show it is a scene/prefetch manifest and references:

```text
V:/updata/scene/01/cpl01.xfe
```

The actual `CPL01.XFE` resource is also observed in prior runtime resource logs (resource id 5569), making XFE the better content target. The raw XFE bytes are not currently present in the working container/Library materialized set, so content-level duplicate-opcode verification is not available from the current files. This is an evidence limitation, not evidence that the duplicate does not exist.

### Focus / neutral-input consistency test

The PC state-handler dispatch table at `0x008A9758` resolves Player state `0x01` to handler `0x00525120`. This gives a strong semantic check on the scripted hypothesis.

The start of `0x00525120` does:

```text
if (Game/global flags & 0x800000) != 0:
    skip normal AIM-release check
    continue forced-aim/state-01 handling
else:
    if AIM action 0x800000 is not held:
        Player+0x644 = 0
        FUN_00528F40(0)
        return
```

So the same `0x800000` global bit set by `opcode 0x16,1` is specifically used to keep state `0x01` alive even when the physical AIM action disappears. This is consistent with the bit being a scripted/forced-aim latch rather than ordinary input state.

A direct writer census of `Game/global +0x8C5C8` finds the literal `0x800000` set in `FUN_004FFB80` and cleared in paired `FUN_004FFBE0`; the broad Player/global reset block clears many neighboring flags but does **not** clear `0x800000`. No focus-loss-specific clear has been identified.

This matters for the reporter symptom:

```text
reported bad aim
    -> Alt+Tab
    -> controller record becomes neutral/suppressed
    -> problem immediately recovers
```

If the bad state were caused by active scripted `opcode 0x16,1`, neutral input alone should *not* force state `01 -> 00`, because the forced-aim bit deliberately bypasses that release. Therefore the scripted producer is mechanically capable of creating the PC/Xbox mode2-reset mismatch, but it is a **poor causal match for the reporter's Alt+Tab recovery** unless a separate device/focus path clears camera state independently. Branch-2 writer/reset census found no such unique PC focus reset in the aim-transient block.

**Interpretation:** scripted `0x16` is now downgraded from primary candidate to architectural/example producer. It demonstrates why the PC reset gate can be behaviorally different, but current static evidence does not make it the likely reporter trigger.

## Remaining questions

1. Determine whether shipped XFE/event content can execute `0x16,1` while Player/camera is already in mode 2.
2. If raw XFE becomes available, search actual command streams for repeated/non-balanced `0x16` usage instead of inferring content from interpreter capability.
3. Check whether Alt+Tab can interrupt/clear the forced-aim/event state in a way consistent with the reporter's immediate recovery.

## Decision rule

Producer A is eliminated. Producer B becomes a primary concrete producer only if static evidence shows a reachable `mode2-family -> state01(mode2)` request without an intervening state-00 reset.

## Status

`DEPRIORITIZED — mechanically sufficient mode2 re-request exists, but forced-aim latch conflicts with Alt+Tab/neutral-input recovery`

---

# Branch 4 — Player state `0x2B` dynamic-target exit

## Why this mattered

`0x2A` and `0x2B` both map to camera mode 2. The PC `0x2B` handler initially looked suspicious because two calls to `FUN_00528F40` pass a register rather than an immediate state constant. If that register could carry another mode-2 state, this would have been an ordinary mode2->mode2 producer.

## Result: dynamic-target hypothesis CLOSED

The apparent dynamic target is not actually dynamic at either exit. Both reachable register values are statically forced to zero and remain zero until the state-set call.

### PC first exit

At `0x0051D1D9`:

```asm
xor ebx, ebx       ; EBX = 0
...
0x0051D4DA: push ebx
...
0x0051D4E3: call FUN_00528F40
```

There is no write to `EBX` between `xor ebx,ebx` and the state-set call on this path. Therefore the target at `0x0051D4E3` is exactly Player state `0x00`, not an arbitrary state.

### PC second exit

On the later branch, `EBP` looks dynamic at the call site but is also a constant zero for the lifetime of that sub-path:

```asm
0x0051D6C6: push ebp       ; save caller EBP
0x0051D6C7: xor  ebp,ebp  ; EBP = 0
...
0x0051E5B1: push ebp
...
0x0051E5BA: call FUN_00528F40
```

A full instruction scan from `0x0051D6C7` through `0x0051E5BA` finds no assignment/modification of `EBP`. Therefore this target is also exactly state `0x00`.

### Xbox equivalent

Xbox state `0x2B` handler `sub_82320330` confirms the same semantics. Its exit directly loads the target constant before the state setter:

```text
0x82320938  li r4,0
0x82320940  sub_8232A300(Player, 0)
```

So the PC register form is merely a compiler/codegen difference, not a behavioral divergence.

## Interpretation

State `0x2B` does **not** provide a normal mode2->mode2 transition through the suspected exits. Both PC and Xbox exit to state 0, which forces camera mode out of 2 and guarantees a fresh PC mode-2 initialization on the next aim entry.

This branch is therefore non-causal for the reported persistent restricted aim range.

## Status

`CLOSED — both apparent dynamic exits resolve to state 0; Xbox matches`

---

# Branch 5 — Exact PC/Xbox comparison of the remaining candidate sites

## Why this matters

The broad PC/Xbox architectural mismatch is already confirmed, but a production restoration should be justified by one concrete gameplay path rather than by blindly removing the PC `previousCameraMode != 2` gate everywhere.

Need address-to-address comparison of only the candidate sites that survive Branches 1–4.

## Current evidence

**Confirmed architectural difference:**

```text
Xbox Player state request
    -> Xbox camera setter(mode 2)
    -> fresh aim anchor/reset

PC Player state request
    -> PC camera setter(mode 2)
    -> if previous camera mode == 2:
           skip fresh aim anchor/reset
```

This is the strongest static difference found so far.

**Important negative evidence:**

- most normal weapon-state pairs transition through mode 0 on their odd/even hand-off;
- the earlier apparent state-0C weapon ingress was a table-indexing mistake and is actually state 0x0E / mode 0;
- therefore the bug likely requires a narrower external or dynamic producer rather than every weapon change.

## Next static work

After Branches 1–4 identify surviving candidates:

1. locate Xbox equivalent producer;
2. compare Player state request semantics;
3. compare camera setter behavior;
4. compare flag/transient writes immediately before/after request;
5. classify the difference as:
   - PC-only regression;
   - harmless port refactor;
   - behaviorally equivalent despite code difference;
   - strong restoration candidate.

## Patch threshold

Do **not** patch yet.

A patch candidate is justified only when we have:

- one concrete reachable gameplay producer;
- a direct PC/Xbox reset-policy mismatch on that path;
- a plausible link to simultaneous X/Y restriction;
- a natural explanation for Alt+Tab recovery;
- no evidence that the PC gate is required for normal camera continuity elsewhere.

Potential patch styles, ordered from safest to broadest:

1. reset/re-anchor only on one confirmed bad Player transition;
2. reset/re-anchor on a small confirmed subset of Player mode2->mode2 transitions;
3. reproduce Xbox unconditional mode-2 setter semantics globally, only if all callsites are proven safe.

## Status

`OPEN — final synthesis branch`

---

# Current overall hypothesis ranking

## Strongest current model

```text
rare aim/combat/weapon state request
    -> Player state changes or is re-requested while camera remains mode 2
    -> Xbox performs fresh mode-2 initialization
    -> PC skips it because previousCameraMode == 2
    -> transient aim/basis state survives
    -> both horizontal and vertical movement become restricted
    -> Alt+Tab creates neutral input / aim-family exit
    -> camera leaves mode 2
    -> next aim is true non-2 -> 2
    -> PC full initialization finally runs
    -> normal range restored
```

## Confidence notes

- PC/Xbox reset-policy difference: **HIGH confidence**
- transient block can materially affect aim: **HIGH confidence**
- `CCamera+0x120 bit0` as part of preservation policy: **HIGH confidence as consumer, LOW/MEDIUM as cause**
- ordinary weapon handlers as generic producer: **LOW after current negative evidence**
- external state-01 scripted ingress: **OPEN / only surviving concrete re-request candidate**
- 0x2B dynamic-target exit: **CLOSED / both exits are state 0**
- exact reporter producer: **NOT YET IDENTIFIED**

---

# Research discipline / anti-zalip rules

1. Update this file immediately after every new confirmed finding.
2. Record corrections explicitly; never silently overwrite a disproven interpretation.
3. Do not broaden back into the entire Player state graph unless one of the five branches requires it.
4. No reporter probes until static RE produces one highly specific hypothesis.
5. No production patch until a concrete reachable bad transition/reset mismatch is identified.
6. Keep PC and Xbox evidence side by side whenever possible.


---

# Secondary path — shared mode-2 gates above both axes

## Why this branch was opened

Branches 1, 2, and 4 are closed, and Branch 3's only mechanically sufficient mode2 re-request (`opcode 0x16`) is a poor match for immediate Alt+Tab recovery because its forced-aim latch deliberately survives physical AIM release. Per the original handoff, the next static target is therefore any shared gate above both horizontal and vertical aim consumers that can remain latched across frames and clear on a neutral/focus transition.

## Preliminary PC/Xbox gate comparison

The first three shared mode-2 flag branches examined are the `0x80`, `0x100`, and `0x200` camera-flag paths. Their ordering and early-exit structure are preserved between PC and Xbox in the portions compared so far. No simple missing-PC/missing-Xbox shared gate has been found here.

### `0x100` branch

This branch is semantically notable even though it is not yet a platform difference. It is associated with target/lock-camera behavior. In the PC mode-2 path it clears the live aim-offset/transient state and diverts into the dedicated target/lock camera handler (`FUN_0053A5C0`) instead of continuing through the ordinary free-aim X/Y path.

This makes a stale target/lock state a plausible *mechanism class* for simultaneous X/Y restriction, but **not yet a PC-only cause**: the equivalent Xbox branch exists and current comparison shows the same high-level gate structure.

## Current question

Trace the producer/clear lifecycle of the target/lock flag feeding the `0x100` mode-2 branch and determine whether:

1. PC has a latch/clear mismatch versus Xbox;
2. focus loss / neutral input clears or bypasses it;
3. a stale value can persist while the physical AIM action remains held;
4. it can produce the reporter's restricted X/Y range rather than a normal lock-on camera state.

## Status

`OPEN — first shared gates match; target/lock 0x100 lifecycle is the current narrow target`

## Interim status — camera-follow / FPU hypothesis (2026-09-29)

### What is now closed
- `CCamera+0x120 bit0` writer policy matches Xbox (`+0x110 bit0` there) across weapon states; not a PC-only root cause.
- Active aim transient fields are `PC +0x8C/+0x90/+0x9C/+0xA0` (Xbox corresponding layout shifted by `-0x10`). Writer/reset census found PC equivalents for the Xbox cleanup paths, including Xbox `sub_823367B0` <-> PC `FUN_005354D0`; no missing PC cleanup found.
- `state 2B` dynamic-looking exits resolve to state `0`; not a hidden mode2->mode2 path.
- Normal gameplay ingress to Player state `01` is dispatched only from state `00`, so it is a safe `mode0 -> mode2` transition and receives the PC reset.
- Scripted/event opcode `0x16` can force Player state `01`; PC/Xbox event handlers are structurally equivalent before the common Player/camera setter. Static code alone has not yet proven a repeated invocation while already in mode 2.

### Strongest confirmed PC/Xbox semantic difference
Xbox camera mode-2 setter re-anchors and clears the aim transient state whenever mode 2 is set. PC `FUN_00534C60(2)` performs that initialization only if `previousCameraMode != 2`.

### New symptom refinement from historical PC reports
A 2013 Steam report describes the bug more precisely: aiming moves where York points the gun, but does not rotate/follow the camera; the crosshair reaches the edge of the screen and then stops in both horizontal and vertical directions. Alt+Tab is a long-standing workaround reported by multiple users. This suggests the bug may be a camera-follow/rotation handoff failure rather than merely a wrong pitch/yaw clamp.

### New candidate: exact threshold / x87 FPU-state sensitivity
The mode-2 firearm camera path appears to transition from internal aim-offset motion to camera rotation when an offset reaches an exact limit. PC code uses x87 floating-point operations/comparisons; Xbox uses PPC single-precision floating point. Direct3D 9 is documented to alter the x87 FPU control state to single-precision round-to-nearest unless `D3DCREATE_FPU_PRESERVE` is used. Because Alt+Tab/fullscreen reset activity is entangled with D3D9 device/focus transitions, FPU-state sensitivity is now a concrete hypothesis worth checking statically/runtime-locally before blaming the mode2 reset gate alone.

### Next narrow checks
1. Identify the exact PC branch where internal aim offset at its limit hands control to camera yaw/pitch rotation, and record the compared fields/constants/instructions.
2. Compare the corresponding Xbox branch instruction-for-instruction, especially precision and equality/ordering semantics.
3. Inspect PC D3D9 CreateDevice behavior flags and all `_controlfp`/`fldcw`/`fnstcw` writers, plus reset/focus paths, to see whether FPU precision/control can differ before vs after Alt+Tab.
4. If the branch can fail under a different x87 precision/control word, reproduce locally by forcing the relevant FPU control state instead of sending reporter probes.
5. Only after this, decide whether the root cause is (a) stale mode2 transient state, (b) FPU-sensitive camera-follow threshold, or an interaction of both.

---

# Interim checkpoint — user-requested summary (2026-09-29)

## Confirmed findings

1. **PC vs Xbox mode-2 reset policy differs.**
   - Xbox re-anchors mode-2 aim orientation and clears the transient aim block whenever mode 2 is set.
   - PC `FUN_00534C60(2)` does this only when `previousCameraMode != 2`.
   - This remains the only confirmed PC-only semantic difference in the examined aim transient/reset subsystem.

2. **The active transient fields affecting both axes are narrowed to four fields.**
   - PC `CCamera+0x8C/+0x90` are smoothed offsets.
   - PC `CCamera+0x9C/+0xA0` are the corresponding target accumulators.
   - These four fields can influence horizontal and vertical aim together.
   - The wider `+0x8C..+0xA8` block contains additional fields, but the live mode-2 handler does not use all of them for the reported behavior.

3. **`CCamera+0x120 bit0` is not a PC-only cause.**
   - Its weapon-state set/clear policy matches the Xbox equivalent (`+0x110 bit0`) state-for-state.
   - It is still part of the normal preservation/decay policy for aim offsets, but not the platform regression by itself.

4. **Writer/reset census for the four active aim fields found no missing PC cleanup.**
   - Xbox cleanup/helper paths have PC equivalents.
   - Xbox `sub_823367B0` has PC analogue `FUN_005354D0`.
   - Therefore there is no evidence that the PC port simply omitted a whole offset-decay/reset helper.

5. **`state 0x2B` dynamic-target suspicion is closed.**
   - The apparently dynamic exit values resolve to Player state `0` on the relevant paths.
   - No hidden normal `2B -> mode2` transition was established there.

6. **Normal gameplay entry to Player state `01` is safe.**
   - The normal input producer is dispatched from Player state `00` only.
   - Therefore it is a normal `mode0 -> mode2` entry and receives the PC mode-2 initialization.

7. **Scripted/event opcode `0x16` can force Player state `01`.**
   - PC and Xbox event-side handlers are structurally equivalent up to the common Player/camera state setter.
   - Static code has not yet proved that real game content invokes this again while already in mode 2.
   - This remains mechanically capable of exposing the PC/Xbox reset-policy difference, but causality is unproven.

## Important symptom refinement

Historical descriptions fit a more specific failure than "wrong aim limits":

- York/the weapon can still aim within the visible screen area;
- the reticle reaches the screen edge;
- the **camera itself fails to continue following/rotating**;
- both horizontal and vertical effective range therefore look restricted;
- Alt+Tab restores normal camera-follow behavior.

This suggests the failure may sit at the handoff between **internal screen-space/relative aim offset** and **actual camera yaw/pitch rotation**, not at the fixed `-35 deg / +55 deg` pitch clamp.

## Current leading models

### Model A — stale mode-2 state
A repeated mode-2 state request occurs. Xbox resets/re-anchors; PC sees `previousCameraMode == 2` and preserves old transient/basis state. That stale state prevents normal camera-follow until aim is broken by focus loss.

### Model B — camera-follow threshold / floating-point sensitivity
The mode-2 path may switch from local aim-offset motion to camera rotation only after a threshold/comparison is reached. PC uses x87 floating point while Xbox uses PPC single precision. A precision/control-word mismatch could make the handoff branch fail. Alt+Tab/D3D9 reset/focus activity is therefore relevant because D3D9 can alter x87 FPU control state unless created with FPU preservation.

### Model C — interaction of A and B
A stale mode-2 offset/basis survives because of the PC reset gate, and an FPU-sensitive threshold then prevents that stale value from crossing the camera-follow handoff correctly.

## What is NOT currently supported

- SDL3/XInput/JOYINFOEX as root cause.
- AutoSwitch or USEJOY alone.
- Exclusive fullscreen alone.
- The fixed pitch clamp alone.
- Quick Turn / stuck `0x100` action as direct cause.
- Missing PC writer/cleanup function for the four active transient fields.
- `state 2B` as a hidden generic mode2->mode2 producer.

## Immediate next static work

1. Locate the exact PC branch where aim-offset motion at/near the screen limit hands control to actual camera yaw/pitch rotation.
2. Identify every field and constant participating in that handoff.
3. Compare that branch instruction-for-instruction against Xbox.
4. Inspect PC x87 control-word behavior (`_controlfp`, `fldcw`, `fnstcw`, D3D9 CreateDevice flags, focus/reset path).
5. Only then decide whether the next local experiment should force FPU state or force a precise mode2 reinitialization. No reporter probe yet.


---

# Milestone: exact camera-follow handoff + x87 precision hazard (2026-09-29)

## Exact PC camera-follow handoff identified

Steam `FUN_0053B8B0` contains an explicit two-stage aim model:

1. input first moves an internal aim target/offset inside a bounded screen-space range;
2. once that target reaches the bound, the code starts rotating the actual camera pitch/yaw.

The key fields are:

```text
PC CCamera+0xA0 = horizontal/yaw target accumulator
PC CCamera+0x90 = smoothed horizontal offset
PC CCamera+0x70 = actual camera yaw/heading

PC CCamera+0x9C = vertical/pitch target accumulator
PC CCamera+0x8C = smoothed vertical offset
PC CCamera+0x6C = actual camera pitch/orientation
```

### Horizontal handoff

Decompiled semantics:

```cpp
limit = weaponAimParams[5] * 3.2f * DEG_TO_RAD;
A0 = clamp(A0, -limit, +limit);

if (!(CCamera+0x120 & 1) || A0 == -limit || A0 == +limit)
    CCamera+0x70 -= yawCameraRate * input;
```

Steam assembly of the equality gate is around:

```text
0053C183  test byte ptr [esi+0x120], 1
0053C18A  je   camera_follow
0053C18C  fld  dword ptr [esi+0xA0]
0053C192  fucompp                    ; compare against retained -limit
...
0053C199  jnp  camera_follow         ; equality
0053C19B  fld  dword ptr [esi+0xA0]
0053C1A1  fucompp                    ; compare against retained +limit
...
0053C1A8  jnp  camera_follow         ; equality
...
0053C1ED  fsubr dword ptr [esi+0x70] ; actual camera yaw update
0053C1F0  fstp  dword ptr [esi+0x70]
```

### Vertical handoff

Decompiled semantics:

```cpp
limit = weaponAimParams[5] * DEG_TO_RAD;
A9C = clamp(A9C, -limit, +limit);

if (!(CCamera+0x120 & 1) || abs(A9C) == limit)
    CCamera+0x6C -= pitchCameraRate * input;
```

Steam assembly:

```text
0053C39D  test byte ptr [esi+0x120], 1
0053C3A4  je   camera_follow
0053C3A6  fld  dword ptr [esi+0x9C]
0053C3AC  fabs
...
0053C3BA  fmul ...                   ; retained radians limit
0053C3BC  fucompp
...
0053C3C3  jp   no_follow             ; equality falls through
...
0053C3EC  fsubr dword ptr [esi+0x6C] ; actual camera pitch update
0053C3EF  fstp  dword ptr [esi+0x6C]
```

This exactly matches the refined historical symptom: the weapon/reticle can move to the edge of the visible range while the actual camera does not continue rotating.

## Xbox comparison: same logical equality, different precision semantics

Xbox mode-2 code has the same high-level handoff policy and also tests equality at the edge.

Horizontal, around `0x8233BB40`:

```text
load CCamera+0x110 bit0
if clear -> camera follow
fcmpu target, -limit
beq -> camera follow
fcmpu target, +limit
bne -> no camera follow
```

Vertical, around `0x8233BCB4`:

```text
load CCamera+0x110 bit0
if clear -> camera follow
fmuls limit = weaponLimit * radians
fabs target
fcmpu target, limit
bne -> no camera follow
```

Crucial difference: Xbox uses PPC single-precision operations (`fmuls`, `fadds`, `stfs`) throughout this path. The compared limit and the clamped target therefore share float32 rounding semantics.

## PC x87 precision hazard

The PC port uses x87 and mixes float32 storage with an extended-precision live value.

Relevant constants in Steam are stored as doubles containing exact promoted float values:

```text
0x007769A0 = 3.200000047683716
0x00770380 = 0.0010000000474974513
0x0076F6D0 = 0.01745329238474369  // DEG_TO_RAD float value promoted to double
```

The dangerous sequence is:

```text
compute radians limit in x87
retain that limit on x87 stack
clamp target by fst/fstp DWORD [CCamera+0xA0 or +0x9C]
    -> target is rounded to float32 in memory
reload target from float32 memory
compare reloaded float32 target against still-live x87 limit
```

With x87 precision control at 24-bit, the multiplication result is rounded to float precision and equality behaves like Xbox.

With x87 precision control at 53/64-bit, the live limit can retain more precision than the float32 value written by the clamp. Exact equality then fails even though the target is logically at the clamp boundary.

A simple arithmetic check with the exact Steam constants confirms the mismatch class. For example, with representative non-power-of-two aim limits, `float32(limit)` differs from the live double/53-bit product by one or more float ULP fractions; vertical limits are particularly easy to make unequal. This is sufficient to make the `== limit` handoff branch false indefinitely while bit0 is set.

## D3D9/FPU static facts

The Steam executable's CRT initializes default x87 precision through:

```text
__fpmath -> __setdefaultprecision
__controlfp_s(NULL, 0x10000, 0x30000)
```

MSVC semantics identify `0x10000` as the 53-bit precision setting.

Later, DP creates its D3D9 device with BehaviorFlags `0x44`:

```text
IDirect3D9::CreateDevice(..., BehaviorFlags=0x44, ...)
```

The game does NOT include `D3DCREATE_FPU_PRESERVE`. ZachFix currently forwards `behaviorFlags` unchanged.

Microsoft documents that when `D3DCREATE_FPU_PRESERVE` is not specified, Direct3D 9 switches the calling thread to single-precision round-to-nearest. Therefore the game's aim handoff appears to rely accidentally on D3D9 leaving x87 in 24-bit/single-precision mode after device initialization.

This creates a highly plausible failure mode:

```text
normal expected state:
    D3D9 has set x87 to 24-bit
    -> clamp target == retained limit
    -> camera-follow equality fires

bad state:
    some later code/module restores x87 to 53-bit (or otherwise changes precision)
    -> clamped float32 target != retained higher-precision limit
    -> equality never fires while CCamera+0x120 bit0 is set
    -> reticle/weapon reaches screen edge but camera does not follow
```

## Why this is currently stronger than stale mode2-reset alone

This mechanism directly explains all of the distinctive symptom geometry:

- both horizontal and vertical effective range stop at the on-screen aim bound;
- weapon/reticle movement still works inside that bound;
- actual camera yaw/pitch fails specifically at the boundary handoff;
- only weapon states with `CCamera+0x120 bit0` require the exact-equality gate, matching the weapon-state-specific camera policy;
- Xbox naturally avoids the x87 extended-precision mismatch because its relevant arithmetic is single precision.

The old PC/Xbox mode2-reset mismatch remains real, but it is no longer required to explain the core screen-edge symptom.

## Still OPEN before calling this root cause

1. Identify what can change the main game's x87 precision back away from D3D9's 24-bit mode after CreateDevice.
2. Determine whether the exclusive-fullscreen Alt+Tab/Reset path reliably re-establishes 24-bit precision, or whether another focus-related path does so.
3. Check whether the reporter/vanilla historical affected weapon states are among the bit0-set families (`1A/1C/1E/20/22/2A/2B`).
4. Prefer a local A/B that forces x87 precision to 53-bit vs 24-bit before `FUN_0053B8B0`; if 53-bit reproduces the restricted range and 24-bit restores it, this would be a near-conclusive reproduction without reporter probes.

## Current ranking

```text
LEADING: x87 precision-sensitive exact-equality camera-follow handoff
REAL BUT SECONDARY/UNPROVEN: PC mode2->mode2 reset-policy mismatch
LOWER: scripted repeated state-01 ingress as the primary cause
```

# Follow-up: FPU writer census + D3D9 reset path (2026-09-29)

## DP.exe x87 control-word census

A direct census of `fnstcw/fldcw` sites in the Steam executable was performed after the camera-follow precision hazard was identified.

Result: the ordinary game-side sites inspected do **not** look like long-lived precision-mode setters. The repeated pattern is compiler-generated float-to-int conversion support:

```text
fnstcw old
old | 0x0C00          ; temporary rounding-mode change
fldcw temp
fistp
fldcw old             ; immediate restore
```

Confirmed examples include `0x0041645C`, `0x004ACFC3`, `0x004B75D8`, `0x004D76E5`, `0x0060F65C`, `0x00612927`, and `0x0067941D`. They change rounding control temporarily and restore the incoming control word before continuing.

The CRT transcendental helpers around `0x00750568`, `0x00751208`, `0x00751E58`, and `0x00752038` also save the incoming x87 CW, temporarily load their preferred CW (`0x027F` path), and funnel through common cleanup (`0x0075987E` / `0x0075988B`) which restores the saved word when required. They are therefore not an obvious persistent 53-bit leak either.

The only direct call to the CRT `_controlfp_s` implementation (`0x0075FEB8`) found in the executable is the startup/default-precision initializer at `0x007532A8`:

```text
push 0x30000          ; mask precision-control field
push 0x10000          ; request MSVC _PC_53
push 0
call 0x75FEB8
```

No second ordinary game-side `_controlfp_s` / `_control87` caller was found that deliberately restores 53-bit after D3D initialization.

### Interpretation

Static evidence currently says:

```text
CRT startup -> 53-bit
D3D9 CreateDevice without FPU_PRESERVE -> expected 24-bit
ordinary DP gameplay -> preserves whatever CW it receives
```

Therefore a bad 53-bit state, if this is the reporter's trigger, is more likely to come from an external/runtime interaction or an uncommon control-flow leak than from a normal persistent setter in the main game logic.

## DP's actual D3D9 create/reset lifecycle

Steam `FUN_006CC290` creates the device with BehaviorFlags `0x44`:

```text
0x40 D3DCREATE_HARDWARE_VERTEXPROCESSING
0x04 D3DCREATE_MULTITHREADED
(no 0x02 D3DCREATE_FPU_PRESERVE)
```

The exclusive-fullscreen device-loss recovery is concrete in `FUN_006CCEF0`:

```cpp
hr = IDirect3DDevice9::TestCooperativeLevel();
if (hr == D3DERR_DEVICENOTRESET) {
    FUN_006CC030();
    hr = IDirect3DDevice9::Reset(&DAT_0148BB70);
    if (SUCCEEDED(hr))
        FUN_006CBD00(device);
}
```

So the reporter's Alt+Tab path definitely reaches a real `IDirect3DDevice9::Reset()` when exclusive fullscreen transitions through device loss/not-reset. This strengthens the temporal connection between the fix and the D3D9/FPU environment, even though static Microsoft documentation only guarantees the non-preserve precision behavior as part of Direct3D device initialization and does not explicitly document `Reset()` as reapplying the CW.

## External corroboration

Microsoft's Direct3D 9 documentation explicitly states that without `D3DCREATE_FPU_PRESERVE`, Direct3D defaults the calling thread to single-precision, round-to-nearest. Historical Wine implementation notes likewise describe D3D9 as setting the FPU control word on device creation. This matches the port's apparent hidden dependency on 24-bit x87 arithmetic.

## Narrow next test

The static RE has now produced a sufficiently specific A/B:

```text
force x87 PC=53 immediately before / during FUN_0053B8B0
    -> prediction: screen-edge camera-follow equality fails

force x87 PC=24
    -> prediction: equality behaves like Xbox and camera follow works
```

This test should be done locally first. No reporter diagnostic build is needed.

# Independent hardware x87 reproduction of the equality hazard (2026-09-29)

To validate the arithmetic mechanism independently of Python/double emulation, the DP-style x87 sequence was reproduced in a tiny native x86-64 assembly harness using the real x87 control word. The relevant sequence is architecture-independent x87 and matches the PC port's critical behavior:

```text
load float input
multiply by the promoted-float DEG_TO_RAD constant in x87
store the result to float32 without popping the live x87 result
reload that float32 value
FUCOMPP reloaded_float32 vs still-live x87 product
```

The harness explicitly switches raw x87 precision-control bits between:

```text
PC=53 : raw PC bits 10b (0x0200)
PC=24 : raw PC bits 00b (0x0000)
```

Observed real-hardware results:

```text
v=3.2  PC53=0 PC24=1
v=16   PC53=1 PC24=1
v=48   PC53=0 PC24=1
v=15   PC53=0 PC24=1
```

`1` means the exact equality survives the float32 spill/reload; `0` means it does not.

This confirms two important points:

1. the hypothesized failure is a real x87 precision-control effect, not a decompiler/Python artifact;
2. PC=24 consistently restores the intended equality for these representative values, while PC=53 breaks it for many ordinary non-special products.

The `v=16` control case is also useful: it remains equal in both modes because the particular multiplication is exactly representable enough for the retained product and the float32 spill to agree. Therefore the bug can naturally be **weapon/camera-profile dependent** rather than universally reproducible for every aiming configuration.

Harness source/result files:

```text
/mnt/data/aim_re/x87_edge_handoff_test64.s
/mnt/data/aim_re/x87_edge_handoff_test64
```

This materially strengthens the x87 camera-follow hypothesis, but one game-side A/B is still required to prove that the reporter's actual failing camera profile lands on one of the precision-sensitive values and that forcing PC=53/24 flips the observed in-game behavior.

# PLAYER_GUN profile fingerprint for the x87 bug (2026-09-29)

The actual PC runtime dump of resource `0x39FF` was recovered from Library:

```text
PLAYER_GUN.pc.bin
20 rows x 0x18 bytes
6 float32 fields per row
```

This is the exact table consumed by `FUN_0053B8B0`. Prior combat-resource research already established `0x39FF = PLAYER_GUN`, 20 rows x 24 bytes, and that the PC/Xbox payloads compared byte-for-byte equal. Therefore this is tuning shared with Xbox, not a PC asset regression.

## How the 20 rows map into mode-2 aim

The mode-2 code selects the table using the equipped ITEM id returned by `FUN_004FD920(0)`.

For normal firearm ITEM IDs `0x16..0x1E` (22..30):

```text
vertical profile = PLAYER_GUN[itemId - 0x16]     -> rows 0..8
horizontal profile = PLAYER_GUN[itemId - 0x0C]   -> rows 10..18
```

ITEM IDs `0x37..0x3F` use the same 0..8 / 10..18 profile indices, consistent with alternate/infinite variants. All other values fall back to rows 9 and 19.

The relevant edge-limit field is float #5 (`row + 0x14`). Actual values are:

```text
vertical rows 0..9:
  5, 5, 5, 5, 6.6240000725, 3, 1, 2, 5.6240000725, 0

horizontal rows 10..19 before the code's x3.2 conversion:
  5.6240000725, 6.6240000725, 6.6240000725, 5.6240000725,
  7, 3, 1, 2, 5.6240000725, 0
```

## Exact-equality classification under x87 PC=53

Because both operands originate as binary32 values, their product has at most 48 significant bits and is exactly representable in IEEE double. Thus comparing the exact 53-bit product against its float32 spill can be classified without approximation.

For normal ITEM IDs 22..30:

| ITEM | known weapon group | horizontal equality at PC=53 | vertical equality at PC=53 |
|---:|---|:---:|:---:|
| 22 | handgun family | FAIL | FAIL |
| 23 | handgun family | FAIL | FAIL |
| 24 | 10mm SMG | FAIL | FAIL |
| 25 | 5.56 Assault Rifle | FAIL | FAIL |
| 26 | .357 Magnum | FAIL | FAIL |
| 27 | Shotgun | FAIL | FAIL |
| 28 | Wesley Special / flamethrower | FAIL | PASS |
| 29 | RPG | FAIL | PASS |
| 30 | Dart Gun | FAIL | FAIL |

At PC=24, the relevant multiplication rounds to float precision before the comparison and all of these edge equalities behave as intended.

## Why rows 28/29 are special vertically

Their vertical field5 values are exactly `1.0` and `2.0`. The `DEG_TO_RAD` constant is itself an exact promoted binary32 value, so multiplying it by 1 or by the power-of-two factor 2 does not create extra mantissa bits. The retained x87 product therefore remains exactly equal to the float32 spill even at PC=53.

Horizontal still fails for those weapons because the code first converts the profile value through the separate `* 3.2` float32 step, yielding `3.2` and `6.4`, then multiplies that by `DEG_TO_RAD`; the final product is no longer float32-exact.

## Diagnostic fingerprint

If the local F9 (`PC=53`) A/B reproduces the suspected port bug, the x87 hypothesis predicts a very specific pattern:

```text
all ordinary firearms:
    horizontal camera-follow should fail at the reticle edge

handguns / SMG / AR / Magnum / Shotgun / Dart Gun:
    vertical camera-follow should fail too

Wesley Special / flamethrower and RPG:
    vertical camera-follow should continue to work
    while horizontal follow remains broken
```

A result matching this profile-dependent fingerprint would strongly discriminate the x87 cause from a generic stale-camera-state or controller-range bug.

## 2026-09-29 checkpoint: Alt+Tab device path does NOT recreate D3D9 device

Static Steam PC trace of the exclusive-fullscreen lost-device path closes one tempting explanation for the x87 hypothesis.

Confirmed path:

```text
FUN_006CCEF0
    -> IDirect3DDevice9::TestCooperativeLevel (vtable +0x0C)
    -> if D3DERR_DEVICELOST: sleep / retry
    -> if D3DERR_DEVICENOTRESET:
         FUN_006CC030()                // On Render Device Lost
         IDirect3DDevice9::Reset       // vtable +0x40
         FUN_006CBD00(device)          // On Render Device Reset / recreate resources
```

Initial device creation is separate:

```text
Direct3DCreate9(D3D_SDK_VERSION)
IDirect3D9::CreateDevice(..., behaviorFlags = 0x44, ...)
```

`0x44` does not include `D3DCREATE_FPU_PRESERVE`, so initial CreateDevice is expected to put the calling thread into Direct3D's default single-precision / round-to-nearest FPU mode.

Crucially, the Alt+Tab recovery path above does **not** release/recreate the IDirect3DDevice9 and does not call CreateDevice again. It only calls Reset on the existing device.

Therefore:

- the attractive model `Alt+Tab -> device recreated -> CreateDevice forces PC=24 -> aim fixed` is **disproven** for DP's normal lost-device path;
- current Microsoft Reset documentation also does not promise that Reset reapplies the FPU precision-control mode;
- x87 exact-equality remains a confirmed latent failure mechanism, but Alt+Tab's observed repair cannot yet be attributed to a proven D3D9 FPU reset;
- focus/input-driven Player/camera-state reset remains a separate viable explanation for why Alt+Tab repairs the reporter's already-bad session.

Interpretation discipline going forward:

```text
Confirmed:
    PC mode-2 edge handoff is FPU-precision fragile.
    Forcing PC=53 should be able to reproduce the edge-follow failure.

Not yet confirmed:
    reporter's spontaneous bug is caused by PC actually becoming 53-bit.
    Alt+Tab repairs x87 precision.
```

The next useful test is therefore stronger than just `PC53 breaks / PC24 works`: measure/record the live x87 CW when the bug is present and immediately after Alt+Tab, or reproduce locally with the forced-PC53 A/B and see whether Alt+Tab changes it. No reporter probe should be sent yet; this can first be done locally.

## 2026-09-29 checkpoint: DPfix/ZachFix do not inject FPU_PRESERVE

The D3D9 proxy layer itself does not explain the historical aim reports by preserving a 53-bit x87 mode at device creation.

### ZachFix

In the available ZachFix source snapshot, the `IDirect3D9::CreateDevice` hook forwards the game's original `behaviorFlags` unchanged to the real D3D9 CreateDevice call. No `D3DCREATE_FPU_PRESERVE` bit is injected.

### Original Durante DPfix

The original DPfix `hkIDirect3D9::CreateDevice` follows the same pattern: it adjusts presentation parameters, but forwards the original `BehaviorFlags` unchanged to the underlying D3D9 device creation call.

The game itself uses:

```text
behaviorFlags = 0x44
```

which does not contain `D3DCREATE_FPU_PRESERVE`.

Therefore:

```text
DP.exe -> CreateDevice(0x44)
       -> DPfix/ZachFix proxy
       -> real D3D9 CreateDevice(0x44)
```

The proxy does not change the FPU-preservation policy.

Implications:

- old reports where DPfix was installed are not explained by DPfix adding `D3DCREATE_FPU_PRESERVE`;
- the initial device creation should still request normal D3D9 FPU behavior;
- if the affected process later reaches x87 PC=53/64, the source must be elsewhere (runtime/appcompat/external module/rare unbalanced FPU state), or the reporter's Alt+Tab repair is primarily a Player/camera-state reset rather than an FPU reset.


## 2026-09-29 local A/B harness prepared: Native / Force53 / Force24

A local diagnostic patch has now been prepared for the exact PC mode-2 aim handler rather than changing the process-wide FPU state.

Hook targets:

```text
Steam 1.01b: DP.exe+0x0013B8B0 = FUN_0053B8B0
GOG   1.01b: DP.exe+0x0013B980 = FUN_0053B980
```

Both targets are signature-gated against the shared prologue:

```text
83 EC 24
56
8B 35 A4 1E BE 00
```

The test starts in `Native` and F9 cycles:

```text
Native -> Force53 -> Force24 -> Native
```

Implementation discipline:

- read/log the ambient x87 control word on entry to the native mode-2 handler;
- log only when that ambient CW changes;
- for `Force53`, set only `_MCW_PC` to `_PC_53` immediately before calling the original handler;
- for `Force24`, set only `_MCW_PC` to `_PC_24`;
- restore the caller's original precision-control bits immediately after the original handler returns;
- leave rounding, exception masks and unrelated control-word fields untouched;
- do not alter the rest of the game thread or D3D9 globally.

This makes the local test answer two separate questions in one run:

1. **Causality:** does `Force53` create the edge-follow/restricted-range symptom, and does `Force24` remove it?
2. **Alt+Tab mechanism:** does the ambient x87 control word observed on mode-2 entry actually change across Alt+Tab/focus regain?

Expected strongest result:

```text
Native   = normal locally
Force53  = reticle reaches screen edge but camera-follow fails/restricts
Force24  = camera-follow works again
```

If that happens, the x87 exact-equality mechanism is locally causally proven even if the reporter's spontaneous source of PC=53 remains unknown.

If Alt+Tab also produces a logged ambient CW transition from PC53/64 to PC24, that additionally explains the reporter's repair mechanism. If Alt+Tab does **not** change ambient CW, then the reporter's Alt+Tab recovery is more likely caused by the independently confirmed focus/input -> Player/camera-state reset, while x87 precision remains a separate latent way to reproduce the same visible failure.

Prepared patch:

```text
ZachFix_aim_FPU_AB_test.patch
```

This is diagnostic-only and should not be kept as a production feature after the A/B is complete.

### 2026-09-29 — FPU A/B patch rebased to current source layout

The first generated A/B patch targeted the pre-reorganization ZachFix source tree and therefore did not apply to the user's current `DPFIX-NG(20260929-092815)` snapshot. The current repository has subsystem directories (`core/`, `gameplay/`, `input/`, etc.) and initialization now lives at `src/zachfix/core/initialization.inl`.

The diagnostic was rebased to the current tree without changing its test semantics:

- new module: `src/zachfix/gameplay/aim_fpu_ab.cpp/.h`;
- source added to `ZACHFIX_GAMEPLAY_SOURCES`;
- include added to `src/asi_main.cpp`;
- install call added immediately after successful `MH_Initialize()` in `src/zachfix/core/initialization.inl`.

Current patch:

```text
ZachFix_aim_FPU_AB_test_20260929-current.patch
```

Validation against the exact uploaded source snapshot:

```text
git apply --check : PASS
git apply         : PASS
new files present : PASS
CMake/source/init wiring verified : PASS
```

## Runtime A/B refinement — Native vs Force53 only

**Date:** 2026-09-29

The first runtime A/B produced the expected causal result:

```text
Native   = normal
Force53  = restricted-aim bug reproduced
Force24  = normal
```

For the next Alt+Tab investigation the diagnostic harness is intentionally simplified to only two states:

```text
Native <-> Force53
```

### Native semantics

`Native` is now deliberately passive. The hook still reads/logs the ambient x87 control word on mode-2 handler entry, but it performs **no `_controlfp_s` write at all** before or after the native aim handler. Therefore any x87 precision change observed while in Native, including across Alt+Tab/focus/device-reset activity, belongs to DP/runtime/Direct3D rather than the A/B harness.

### Force53 semantics

`Force53` saves the precision-control bits observed on handler entry, sets only `_MCW_PC` to `_PC_53` for the duration of the native mode-2 aim handler, then restores the entry precision-control bits. Rounding/exception controls are not intentionally changed by the harness.

### Hotkey

`F9` toggles:

```text
Native -> Force53 -> Native -> ...
```

This replaces the earlier three-state `Native -> Force53 -> Force24` diagnostic build. The reduced harness is specifically for determining whether Alt+Tab changes the ambient x87 state independently of the forced reproduction mode.

## Runtime checkpoint — Native vs Force53 A/B (2026-09-29)

Test build: Native <-> Force53 only. Native path is passive and does not modify x87 precision; Force53 changes precision locally around the mode-2 aim handler and restores the incoming control word afterward.

Observed result on local machine:

```text
Native   = normal aim/camera-follow
Force53  = reported restricted-aim bug reproduced
Native   = normal again
```

Runtime log:

```text
[AimFpuAB] Ambient x87 CW changed: 0x000A001F (PC24, PCbits=0x20000).
```

No later `Ambient x87 CW changed` line appeared after Alt+Tab. Therefore on this machine:

- ambient x87 precision is already PC24 during normal gameplay;
- Alt+Tab does not change the observed x87 control word;
- Native remains normal before and after Alt+Tab;
- Force53 alone is sufficient to reproduce the aim-range/camera-follow failure.

Interpretation:

1. The precision-sensitive mode-2 edge handoff is now runtime-causally confirmed. PC=53 breaks the handoff; PC24 preserves normal behavior.
2. The reporter's Alt+Tab recovery is not reproduced locally as an x87 CW transition, because the local process is already in the correct PC24 state.
3. A remaining open problem is the producer of a bad ambient FPU state on the reporter's machine/startup path. This is now separate from the already-confirmed camera-follow bug mechanism.
4. D3D9 CreateDevice BehaviorFlags in the local log are `0x00000044` (no FPU_PRESERVE), consistent with Direct3D setting the calling thread to single precision at device creation. The observed runtime CW after initialization is PC24.

Status update:

```text
CONFIRMED: PC x87 PC53 can reproduce the bug exactly.
CONFIRMED: PC24 fixes/prevents it locally.
CONFIRMED: local Native runtime is PC24.
CONFIRMED: local Alt+Tab does not change ambient x87 CW.
OPEN: what leaves the reporter/startup path in PC53 before their first Alt+Tab.
```

---

## Runtime checkpoint: persistent PC53 poison / Alt+Tab test

**Date:** 2026-09-29

The prior Native-vs-Force53 runtime test established:

```text
Native (ambient PC24) = normal aim/camera follow
Force53 local override = restricted aim-range bug reproduced
Force24 local override = normal
```

The uploaded runtime log also showed the local machine entering the aim handler with ambient x87 PC24 (`CW=0x000A001F`) and showed no ambient-CW change after the tested Alt+Tab cycle.

### Next diagnostic

Replace the local per-call Force53 override with a persistent one-shot poison:

```text
normal operation:
    observe/log ambient x87 CW only

F9 while aiming:
    set current gameplay thread x87 precision to PC53
    DO NOT restore it
    call original aim handler normally

then:
    reproduce restricted aim
    Alt+Tab out/in
    aim again
    observe whether ambient CW changes back to PC24
```

Interpretation:

```text
PC53 -> bug -> Alt+Tab -> PC24 -> fixed
    strongly supports focus/D3D lifecycle repairing the bad ambient FPU state.

PC53 -> bug -> Alt+Tab -> still PC53 but fixed
    means x87 explains the range failure itself, while Alt+Tab repairs a second camera/player state.

PC53 -> bug -> Alt+Tab -> still PC53 and still bug
    means the reporter's Alt+Tab recovery depends on some external source first changing the FPU state, or on an environment-specific lifecycle path not reproduced locally.
```

Patch prepared against the already-applied `ZachFix_aim_FPU_Native_vs_53_test.patch` state:
`ZachFix_aim_FPU_persistent53_AltTab_test.patch`.

## Runtime checkpoint: persistent PC53 survives Alt+Tab

**Date:** 2026-09-29

A dedicated persistent-poison test was run in exclusive fullscreen. F9 changed the gameplay thread's ambient x87 precision from PC24 to PC53 and intentionally did not restore it.

Observed log:

```text
[AimFpuAB] Ambient x87 CW changed: 0x000A001F (PC24, PCbits=0x20000).
[AimFpuAB] F9: ambient x87 precision poisoned and left at PC53: before=0x000A001F (PC24), after=0x0009001F (PC53). No restore will be performed.
```

Runtime result:

```text
Native / PC24  -> normal aim
Persistent PC53 -> restricted aim reproduced
Alt+Tab in exclusive fullscreen
Persistent PC53 -> restricted aim still present
```

No post-Alt+Tab `Ambient x87 CW changed` line appeared, therefore the gameplay thread did not return to PC24 during the tested focus-loss/device-reset cycle.

### Conclusion

The x87 precision mismatch is a **real latent PC aim bug / phenocopy**: PC53 is sufficient to reproduce the restricted screen-space aim / failed camera-follow symptom, while PC24 behaves normally.

However it is **not the reporter's Alt+Tab recovery mechanism**. Alt+Tab does not repair a deliberately persistent PC53 environment on the local machine. Therefore the reporter's recovery must act through another state transition, most likely the focus/input suppression -> Player/camera transition path already identified earlier.

Status change:

- `FPU precision can cause restricted aim`: **RUNTIME CONFIRMED**.
- `Alt+Tab fixes reporter by restoring PC24`: **DISPROVEN locally**.
- Do not conflate the FPU phenocopy with the original report unless future evidence shows the reporter actually enters bad precision for some other reason.
- Return primary investigation priority to the focus-loss / aim-exit / camera-mode reset path.

The observed ~23 FPS after Alt+Tab is treated as the already-separate fullscreen/reset/first-run performance issue unless new evidence connects it to aiming.

## Static checkpoint: focus-loss forces a real neutral controller frame and state-01 aim exit

**Date:** 2026-09-29

The native PC input producer `FUN_00709C40` has now been traced through its foreground gate.

### Focus gate / controller record behavior

For the active controller slot, input is rebuilt only when:

```text
local_28 == activeControllerSlot
AND
FUN_006AF070() != 0
```

where:

```text
FUN_006AF070():
    GetForegroundWindow() == GetActiveWindow()
```

If that condition fails (Alt+Tab / focus loss), `FUN_00709C40` executes:

```text
memset(activeRecord, 0, 0x6C)
activeRecord->active = 0
```

Therefore focus loss produces a genuine native neutral/suppressed controller interval. It is not merely a skipped poll.

### State 01 handler proves mode-2 exit on AIM loss

Player dispatch table `PTR_FUN_008A9758[1]` points to `0x00525120`.

At the start of the state-01 handler:

```text
FUN_004FD860(... 0x800000 ...)
if not already in the related latched path:
    FUN_00454770(0x800000)   // current/held action
    if action == 0:
        Player+0x644 = native constant
        FUN_00528F40(0)      // state 01 -> state 00
        return
```

The same `0x800000` action is the action checked from state 00 by `FUN_004FF100` when entering state 01. Therefore it is the normal aim/action ingress/held condition for this state family.

This establishes a direct static Alt+Tab repair path for state 01:

```text
foreground lost
    -> controller 0x6C record zeroed
    -> AIM 0x800000 reads false
    -> state 01 handler calls state setter(0)
    -> camera mode 2 -> mode 0
    -> focus regained / AIM pressed again
    -> state 0 -> state 1
    -> camera mode 0 -> mode 2
    -> full mode-2 re-anchor + transient reset occurs on PC
```

So the earlier conceptual focus-loss reset hypothesis is now directly proven for Player state 01.

### Remaining question

The reporter is likely to hit weapon-specific mode-2 states (`14..2B`) during actual firearm aiming, so the next static task is to prove whether loss of action `0x800000` in those weapon handlers also reaches a non-mode-2 state or otherwise clears the same camera transient state.

Status:

- focus loss creates real neutral input: **CONFIRMED**
- state 01 exits mode 2 on focus-lost AIM release: **CONFIRMED**
- weapon mode-2 states on focus loss: **IN PROGRESS**

### Weapon-state focus-loss closure: handgun 1A/1B

The common handgun / Dart Gun handler pair is `0x00512FE0` for Player states `1A/1B`.

A late held-AIM check is explicit:

```text
0x00514250: FUN_00454770(0x800000)
0x00514262: test eax,eax
0x00514264: je 0x005142FF
```

The zero-AIM target `0x005142FF` is not an inert branch. It enters the state cleanup path:

```text
0x00514304: Player+0x644 = native neutral/exit constant
0x00514322: Player+0x62C = 0
...
0x00514392: push 0
0x0051439C: FUN_00528F40(0)
```

Therefore for the ordinary handgun mode-2 family:

```text
focus loss
 -> native controller record zeroed
 -> held AIM 0x800000 becomes false
 -> handgun handler branches directly to cleanup
 -> Player state -> 0
 -> camera mode 2 -> mode 0
```

On focus regain, the next aim ingress necessarily returns from non-2 to mode 2, so PC executes the full mode-2 re-anchor/transient clear.

This is a direct static explanation for why Alt+Tab can repair *any* stale mode-2 camera state during common firearm aiming, independent of the exact producer of that stale state.

Status:

- state 01 focus reset: **CONFIRMED**
- handgun 1A/1B focus reset: **CONFIRMED**
- Alt+Tab repair mechanism via real aim exit / camera re-entry: **STRONGLY CONFIRMED for common aiming paths**

## Checkpoint: focus-loss reset path + external weapon-state re-entry

### Focus loss is a real neutral input frame
PC foreground gate `FUN_006AF070()` compares `GetForegroundWindow()` with `GetActiveWindow()`. In the normal controller producer (`FUN_00709C40` family), a failed focus gate clears the active controller logical record (`0x6C` bytes) rather than merely skipping polling. Therefore Alt+Tab guarantees a frame where held AIM is false at the Player consumer.

### State 01: exact focus-loss exit
Steam Player state `01` handler is `0x00525120`. It queries held action `0x800000`; when absent it directly calls `FUN_00528F40(0)`. Thus:

`focus loss -> 0x6C neutral -> AIM false -> state 01 -> state 00 -> camera mode 2 -> 0`.

On return, normal aim ingress is state `00 -> 01`, so PC's `previousCameraMode != 2` condition becomes true and the complete mode-2 initialization runs.

### Handgun / firearm family: exact focus-loss exit
The common `1A/1B` handler (`0x00512FE0`) has the same held-AIM check. At `0x00514250` it queries `0x800000`; the false branch reaches cleanup and `FUN_00528F40(0)` at `0x0051439C`. This proves the Alt+Tab recovery mechanism for a common firearm aim path, not only unarmed/state-01 aim.

The other weapon handlers also contain held-AIM checks and non-mode2 cleanup paths; the handgun branch is the cleanest fully traced example.

### New concrete mode2->mode2 producer: event weapon/equip helper
PC `FUN_0044D520` is reached from event/interpreter command `FUN_00436450`, opcode `0xA6` (signed `-0x5A`), subcommand `4`.

`FUN_0044D520` reads the active item's weapon-class byte (`ITEM.PRM +0x6D`) and selects:

- class 1 -> state 14
- class 2 -> 16
- class 3 -> 18
- class 4 -> 1A
- class 5 -> 1E
- class 6 -> 20
- class 7 -> 24
- class 8 -> 28
- class 9 -> 2A
- class 10 -> 2B
- class 11 -> 26

If the target object is the Player, it then writes camera orientation seed fields and calls `FUN_00528F40(selectedState)` followed by `FUN_00522230()`.

Important: there is **no current Player-state guard** and `FUN_00528F40` itself does **not** early-return on `oldState == newState`. Therefore this event path can issue a real explicit mode2->mode2 or same-state->same-state SetState call if invoked while the Player is already aiming.

This is exactly the class of call for which Xbox and PC differ:

- Xbox Player SetState -> camera mode 2 -> unconditional mode-2 re-anchor/reset.
- PC Player SetState -> camera mode 2 -> Gate A/Gate B skip if previous camera mode was already 2.

The ordinary gameplay path is safer: normal aim/weapon processing is surrounded by state/input gates and does not simply reissue the primary weapon state every held frame. The event helper is therefore a stronger stale-state producer than normal held aim.

### Xbox primary selector comparison
Xbox `sub_822E2100` contains the original ITEM.PRM class-byte (`+109 / +0x6D`, stride 220 / 0xDC) weapon-state selector and calls `sub_8232A300`. Its mapping includes the same main primary weapon states (20/22/24/26/30/32/36/40/42/43/38 etc., plus surviving class-13/class-99 cases). This independently confirms that weapon-class-selected Player SetState is original engine behavior. The exact Xbox counterpart of PC event opcode `0xA6/4` is still being localized, but its downstream Player setter semantics are already known: mode-2 reset is unconditional.

### Current causal model (stronger)
A concrete mechanism now exists that satisfies both halves of the reporter symptom:

1. an external event/object path explicitly reissues a mode-2 weapon state while camera mode is already 2;
2. PC skips mode-2 re-anchor/transient reset, unlike Xbox;
3. stale mode-2 aim/camera state survives and restricts effective movement;
4. Alt+Tab zeros input;
5. held-AIM loss forces weapon state -> 0 / camera mode 2 -> 0;
6. re-aim gives 0 -> mode 2 and PC finally performs the full reset.

Still OPEN: prove that opcode `0xA6`, subcommand `4` is actually emitted in a reporter-relevant gameplay/event sequence, or find another external producer with equivalent semantics that is unquestionably common.

## Checkpoint: initial Player/CCamera state and first-aim hypothesis

### CCamera lifecycle

- `CCamera` singleton is lazily allocated as `0x1AC` bytes through `FUN_00404390(0x1AC)`.
- `FUN_00404390` zeroes the allocation before the vtable is installed, so the fresh camera object's mode fields begin at zero.
- Steam `FUN_00534C60` (camera-mode setter) has exactly **one** direct callsite in `DP.exe`: `0x00529568`, inside Player state setter `FUN_00528F40`.
- No second direct camera-mode setter caller was found. This strengthens the model that `CCamera+0x154/+0x158` normally follow Player-state changes rather than an independent camera-mode producer.

### Player initial/reset state

`FUN_00478DF0` explicitly initializes:

```text
Player+0x654 = 0   // current state
Player+0x658 = 0   // previous state
```

There is another broad Player reset path (`FUN_004C6540`) that also zeros the corresponding state slots in its object layout.

**Conclusion:** the hypothesis that the bug exists because Player/CCamera are born in mode/state 2 is not supported. Fresh Player and fresh CCamera both initialize to zero/non-aim state. If a first-usable-aim session is already bad, some post-init gameplay/event/equip transition must have placed the Player/camera into a mode-2 family before that aim, or another mode-2 transient must have been preserved later.

### Event weapon producer remains concrete

`FUN_0044D520` has only one direct PC caller: event interpreter `FUN_00436450`, subcommand `case 4` (the previously identified `0xA6/4` family).

For Player target it:

1. reads ITEM.PRM weapon class (`record stride 0xDC`, class byte `+0x6D`);
2. maps class to primary weapon Player state `14/16/18/1A/1E/20/24/28/2A/2B/26`;
3. seeds camera orientation fields;
4. calls `FUN_00528F40(selectedState)` **without checking current Player state**;
5. runs `FUN_00522230()`.

Because `FUN_00528F40` itself also has no same-state early-return, `0xA6/4` is a genuine external producer capable of explicit mode2->mode2 or same-mode2-state SetState requests. Xbox camera semantics reset on such an explicit request; PC's `previousCameraMode == 2` gates preserve mode-2 orientation/transients instead.

### Current interpretation

- Constructor/startup garbage: **rejected**.
- Alt+Tab recovery via AIM-neutral frame -> state0/mode0 -> clean re-aim: **statically confirmed**.
- Need for an actual bad-entry producer: still open.
- `0xA6/4 -> FUN_0044D520` remains the strongest concrete external mode2->mode2 producer found so far, but occurrence in reporter-relevant event content is not yet established.

### Stronger Xbox/PC structural difference: previous camera mode is PC-side behavior

Direct inspection of Xbox `sub_82336130` strengthens the reset-policy finding:

- Xbox writes the requested camera mode directly to `CCamera+0x144` (`stw r27,324(r31)`).
- Within the setter there is no companion write of the old mode to the adjacent `+0x148`, and no compare against a saved previous mode before mode-2 initialization.
- Xbox mode-2 case unconditionally computes fresh orientation, writes the live aim anchor, and zeros the whole corresponding transient block (`+0x7C..+0x98`; PC layout is shifted by +0x10, giving PC `+0x8C..+0xA8`).
- It then copies the fresh live orientation into the rendered camera state.

PC `FUN_00534C60` instead begins with:

```text
CCamera+0x158 = CCamera+0x154   // save previous camera mode
CCamera+0x154 = requested mode
```

and the mode-2 case contains two `previousMode != 2` gates around orientation synchronization and full mode-2 re-anchor/transient reset.

This is not merely a differing branch arrangement: the explicit previous-camera-mode bookkeeping used to suppress repeated mode-2 initialization is itself absent from the original Xbox setter path. The safest semantic description is therefore:

> PC introduced a camera-mode-change optimization that suppresses repeated mode-2 initialization; Xbox treats every explicit SetCameraMode(2), reached from an explicit Player SetState request, as a fresh mode-2 setup.

This materially strengthens the case for an Xbox-semantics restoration A/B that removes both PC `previousMode == 2` skip branches, provided no PC-only caller is shown to require preservation.

## Checkpoint: direct Player SetState caller census narrowed

A direct-call census of Steam `FUN_00528F40` was reviewed specifically for producers capable of selecting a camera-mode-2 Player state. Most dynamic-looking calls are false positives caused by decompiler variable reuse and actually select unrelated menu/cutscene/vehicle states.

### Confirmed mode-2-capable producers

1. `FUN_004FF100` -> state `01`.
   - Normal gameplay AIM ingress.
   - Caller/state dispatch constrains it to `00 -> 01`, so it is not a mode2->mode2 producer.

2. `FUN_004FFB80` -> state `01`.
   - Forced/scripted AIM ingress from event opcode `0x16`.
   - No equivalent normal-state guard was found here; repeated invocation while already in mode 2 remains mechanically possible.

3. `FUN_0044D520` -> primary weapon states `14/16/18/1A/1E/20/24/28/2A/2B/26`.
   - Single direct caller is event interpreter `FUN_00436450`, event opcode `0xA6` (signed `-0x5A`), subcommand `4`.
   - For Player target there is no current-state guard before `FUN_00528F40(selectedState)`.
   - This is the strongest concrete stock producer of an explicit mode2->mode2 / same-mode2-state SetState request.

4. `FUN_00524BF0` -> deferred state from `Player+0xA11`.
   - `+0xA11` has one writer: Player SetState case `0x0B` (Quick Turn), which saves the state that was active before entering `0x0B`.
   - `FUN_00524BF0` returns to that saved state after the turn completes.
   - Therefore it can theoretically return to a mode-2 state, but it belongs to the preserved Quick Turn path already considered non-normal/unvalidated on shipped PC. Low priority for the reporter issue.

### Non-mode-2 callers eliminated

- `FUN_0044F360` always forces state `0`.
- `FUN_004FFD00` forces state `0` under its relevant gate.
- `FUN_00509C20` Event `0x44` selects target-bound odd weapon states (`15/17/19/1B/21/25/29`), which are not in the camera-mode-2 primary-state set.
- `FUN_0050C5F0` reaches state `5`, not mode 2.
- `FUN_005254D0` selects `0x46`.
- Later camera/cutscene/vehicle helpers seen in the census select `0x80/0x84/0x85/...` or other non-mode2 families.

### Result

After narrowing the full direct-caller list, there is no evidence of a hidden common every-frame mode2 reissue in the ordinary Player update. The important stock external mode2 reissue paths are now reduced to:

- forced AIM event -> state `01`;
- event/equip weapon selector `0xA6/4` -> primary weapon mode-2 state;
- Quick Turn return -> prior state (low priority / not normal shipped ingress).

This makes a broad Xbox-semantics A/B more defensible: the PC camera setter has only one caller (Player SetState), and Xbox intentionally performs fresh mode-2 setup on every explicit SetState request. The PC previous-mode optimization suppresses that behavior only on explicit repeated mode-2 requests, not on ordinary held input frames.
