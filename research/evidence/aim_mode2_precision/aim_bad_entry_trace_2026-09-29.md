# ZachFix — Aim Bug Bad-Entry Trace

**Date:** 2026-09-29
**Scope:** one bug only: identify the stock PC transition that can leave aim/camera in a bad mode-2 state before Alt+Tab repairs it.

## Starting constraints carried from the main worklog

- Alt+Tab repair mechanism is already established: focus loss zeroes the controller record, held AIM drops, common aim/weapon states exit camera mode 2, and the next aim entry performs a fresh PC mode-2 reset.
- Xbox `SetCameraMode(2)` always performs fresh mode-2 setup; PC skips two mode-2 initialization blocks when `previousCameraMode == 2`.
- x87 PC53 is a real phenocopy but does not explain the reporter's Alt+Tab recovery.
- Ordinary state-01 AIM ingress is `mode0 -> mode2` and is safe.
- Event opcode `0xA6`, subcommand 4, via `FUN_0044D520`, can explicitly reissue a weapon mode-2 state, but shipped-content occurrence is not yet proven.

## Current narrow task

Enumerate every `Player SetState` exit from the stock weapon/combat mode-2 handlers and determine whether ordinary gameplay itself contains a direct `mode2 -> mode2` transition.

### Steam dispatch anchors

```text
14 -> 0050D5B0
16 -> 0050F440
18 -> 00510F80
1A -> 00512FE0
1C -> 005143D0
1E -> 005155F0
20 -> 00517F70
22 -> 00516A90
24 -> 005194B0
26 -> 0051E5F0
28 -> 0051B270
2A -> 0051BC00
2B -> 0051CE50
```

State-to-camera mode: these states are all mode 2.


## Checkpoint 1 — ordinary weapon-handler mode2->mode2 transitions CLOSED

Raw Steam asm was scanned by dispatch interval for every direct `CALL 0x00528F40` from the weapon/combat handlers.

### Direct SetState exits

| Mode-2 state handler | Direct `Player SetState` result |
|---|---|
| `14/15` | `0` (two exits; one literal/CFG-resolved zero, one alternate jump supplies an explicit `push 0`) |
| `16/17` | `0` |
| `18/19` | `0` |
| `1A/1B` | `0` |
| `1C/1D` | `0` (two exits) |
| `1E/1F` | `0` |
| `20/21` | `0` (two exits) |
| `22/23` | `0` |
| `24/25` | `0` |
| `26/27` | `0` |
| `28/29` | `0` |
| `2A` | `0` (two exits) |
| `2B` | `0` at both apparent dynamic exits; `EBX` is zeroed before `0x0051D4E3`, `EBP` is zeroed before `0x0051E5BA` |

Representative common cleanup pattern:

```text
push 0                    ; target Player state
push DAT_008A9BA4
call FUN_006C5FD0         ; resolve Player
mov  ecx,eax
call FUN_00528F40         ; Player SetState(0)
```

The unusual second `14/15` call at `0x0050F405` initially looked register-valued because the fallthrough immediately before the common tail contains `push EBP`. Raw CFG inspection finds an alternate entry:

```text
0050E038  push 0
0050E03A  jmp 0050F3EA
...
0050F3EA  and [Player+638], ~0x10
...
0050F405  call FUN_00528F40
```

so this common tail is also a state-0 cleanup, not a hidden mode-2 target.

### One-level helper transitions from these handlers

The handler ranges also call a few helpers which themselves call `FUN_00528F40`:

```text
FUN_00522D50 -> states 4/6/8
FUN_0050C5F0 -> state 5
FUN_0050C200 -> states 10/11
```

All of those states map to non-mode-2 camera modes (`4/5/6/8/10/11` are camera mode 0 in this table region).

### Result

No stock ordinary weapon-state handler examined supplies a normal `mode2 -> mode2` Player transition. Their state exits either go directly to state 0 or through helpers that enter non-mode-2 states.

Therefore the bad state is **not produced by the normal per-frame weapon handler transition graph**. A repeated mode-2 setup request, if it is the cause, must come from an ingress/re-entry path outside the normal weapon-handler exits.

**Status:** `CLOSED — ordinary weapon-handler mode2->mode2 transition class eliminated`

## Next target

Trace the ordinary ingress into the primary weapon states `14/16/18/1A/1C/1E/20/22/24/26/28/2A/2B` and identify which producer is used during normal gameplay/control handoff. The key question is whether the same ingress can be invoked again while one of those states is already active.

## Checkpoint 2 — apparent generic same-state reissue at 0x004F8DDC is state 57, not aim

A previously unclassified raw block at `0x004F8700` contains:

```text
004F8DC1  mov eax,[Player+0x654]   ; current Player state
...
004F8DD3  push eax
...
004F8DDC  call FUN_00528F40       ; SetState(currentState)
```

At first glance this is exactly the dangerous same-state reissue pattern.

Binary table lookup resolves the orphan block unambiguously:

```text
pointer 0x004F8700 occurs in DP_STEAM.exe at file offset 0x4A84B4
.data mapping -> VA 0x008A98B4
0x008A98B4 - 0x008A9758 = 0x15C
0x15C / 4 = 0x57
```

Therefore `0x004F8700` is the Player dispatch handler for **state 0x57**.

State `0x57` maps to camera mode `13`, not camera mode `2`.

### Result

The `SetState(currentState)` at `0x004F8DDC` is real, but it cannot expose the PC mode-2 previous-mode reset gate during ordinary execution of state `57`.

**Status:** `CLOSED FALSE POSITIVE — same-state reissue exists, but in camera mode 13`

## Checkpoint 3 — orphan/dynamic Player SetState sites do not hide a primary weapon ingress

The raw 159-call `FUN_00528F40` census contains several callsites which Ghidra did not assign cleanly to named functions. These were checked because a hidden dynamic target could have invalidated the earlier caller census.

Resolved relevant cases:

```text
004EB3E8 -> literal state 5
004EE187 -> state 0 (the call is reached only after the tested function returns EAX==0)
004F8DDC -> SetState(currentState), but belongs to state-57 handler (camera mode 13)
0052F86C -> state 0D
0052F942 -> state 0D
005D301B -> state 0 (value is OR-result tested equal to zero before the call)
005D431E -> state 0
0063FDF4 -> state 0
00655A3C -> state 0
0068871C -> state 0
```

`FUN_004EB6A0` also uses only `0`, `57`, or `59` in its Player SetState paths.

None of these hidden/dynamic callsites selects the primary weapon mode-2 family `14..2B`.

### Interim ingress result

After direct-call, orphan-call, and normal weapon-handler exit review, the only explicit selector currently found which maps `ITEM.PRM +0x6D` weapon class to primary states `14/16/18/1A/1E/20/24/28/2A/2B/26` is `FUN_0044D520`.

Quick Turn return (`FUN_00524BF0 -> Player+0xA11`) can restore a previously saved state and remains a special low-priority re-entry, but it is not the normal weapon-class selector.

Next: classify the exact producer/dispatch semantics of `FUN_0044D520` and its `A6/4` route. In particular, verify whether `A6` is truly XFE/script-only or a native object/event dispatch used by normal gameplay.

## Checkpoint 4 — `0xA6/4` is CEvent bytecode/interpreter ingress, not ordinary native Player event

**Date:** 2026-09-29

The remaining concrete primary-weapon selector `FUN_0044D520` is reached from `FUN_00436450`, which is dispatched by the central event-command decoder `FUN_00447B50` for command byte `0xA6` (signed `-0x5A`).

The surrounding code makes the ownership much clearer than the earlier generic "event" label:

```text
FUN_00447B50(interpreter, command)
    -> switch((char)command[2])
    -> case 0xA6 / -0x5A:
         FUN_00436450(...)
```

`FUN_00436450` then advances a command/operand stream through `param_1[6]`, repeatedly consumes `ushort` operands, and switches on the first decoded subcommand. Subcommand `4` calls `FUN_0044D520`. Later branches in the same routine explicitly manipulate `CSingleton<CEvent>` state.

This is interpreter/bytecode behavior, not an ordinary input-side native Player event callback.

For subcommand `4`:

```text
CEvent bytecode command 0xA6, subcommand 4
    -> FUN_00436450
    -> FUN_0044D520
    -> read ITEM.PRM class +0x6D
    -> choose primary weapon Player state 14/16/18/1A/1E/20/24/28/2A/2B/26
    -> if target is Player: FUN_00528F40(selectedState)
```

There is still no current-state guard in `FUN_0044D520`, so this route remains mechanically capable of an explicit same-mode2/mode2->mode2 SetState. It still exposes the exact PC/Xbox camera-reset divergence.

However, the causal ranking changes:

- `A6/4` is a **real sufficient producer**, but it requires an executed CEvent command/content path;
- it should not be treated as the ordinary held-AIM/weapon-update ingress without shipped-content evidence;
- the next target is the normal weapon/equipment boundary (`Player states 0E/0F`) and its transition into the primary weapon state family.

Status: `A6/4 MECHANICALLY SUFFICIENT, CONTENT-DEPENDENT; ordinary ingress still open`.

## Checkpoint 5 — Normal primary-weapon ingress is explicitly staged through camera mode 0

**Date:** 2026-09-29

A major correction to the earlier ingress picture: `FUN_0044D520` / CEvent `0xA6/4` is **not** the only ITEM-class-to-primary-state selector.

The normal Player state handlers `0x0E` (`0x004E0180`) and `0x0F` (`0x004E0680`) both contain the full native selector themselves.

### State `0E`

Raw Steam code reads the active item, loads `ITEM.PRM[item].class` at record offset `+0x6D`, and dispatches to primary weapon states:

```text
class 1  -> 14
class 2  -> 16
class 3  -> 18
class 4  -> 1A
class 5  -> 1E
class 6  -> 20
class 7  -> 24
class 8  -> 28
class 9  -> 2A
class 10 -> 2B
class 11 -> 26
class 12 -> 22
class 13 -> 1C
class 99 -> 2C
```

The selected constant is passed directly to `FUN_00528F40` at `0x004E0569`.

### State `0F`

`0x004E0680` repeats the same ITEM-class selector and calls `FUN_00528F40(selectedPrimaryState)` at `0x004E0929`.

### Why this matters for the aim bug

The CPlayer state->camera-mode table gives:

```text
state 0E -> camera mode 0
state 0F -> camera mode 0
primary weapon states 14..2C -> mostly camera mode 2 according to the known weapon family
```

Therefore the stock ingress is intentionally staged:

```text
weapon/equipment boundary 0E/0F
    -> camera mode 0
    -> select active ITEM class
    -> SetState(primary weapon state)
    -> camera mode 0 -> 2
    -> PC previousMode != 2
    -> full mode-2 re-anchor/reset runs
```

This is structurally safe against the PC repeated-mode2 reset gate.

The original Xbox-only combat strafe remnants `09/0A` also complete through state `0E`; this again forces mode2 -> mode0 before returning to a primary weapon mode2 state.

Several ordinary action/submode paths enter `0F`; because `0F` is mode0, they likewise cannot directly create the suspected mode2->mode2 reset skip.

### Consequence

The ordinary weapon state machine is now substantially cleared as the bad-entry producer:

- weapon primary handlers do not transition directly to another mode2 primary state;
- their normal re-selection boundary is `0E/0F`, both mode0;
- normal primary selection therefore produces a true non2->2 camera entry on PC.

This raises a different remaining possibility: a **camera-mode/Player-state desynchronization** could make a logically safe Player ingress look like `previousCameraMode == 2`. The next census is direct writers/restores of `CCamera+0x154` and any path that can leave camera current mode at 2 while Player has already returned to a non-mode2 state.

Status: `NORMAL 0E/0F -> PRIMARY WEAPON INGRESS CLOSED SAFE`.

## Checkpoint 6 — IMPORTANT CORRECTION: `09/0A -> 0E -> primary` is an intentional mode2-preserving re-entry

**Date:** 2026-09-29

The previous checkpoint's statement that every `0E/0F -> primary weapon` path necessarily passes through camera mode 0 is too broad. `FUN_00534C60` contains a Player-state special case that changes the semantics specifically after states `09/0A`.

At the start of the PC camera-mode setter:

```cpp
CCamera+0x158 = CCamera+0x154;        // previous camera mode

if (FUN_0052DF20() == 0 &&
    (Player+0x658 == 9 || Player+0x658 == 10))
{
    requestedCameraMode = 2;
}

CCamera+0x154 = requestedCameraMode;
```

Player SetState updates `Player+0x658` to the state being left before it calls the camera setter. Therefore a transition:

```text
Player 09 or 0A
    -> SetState(0E)
```

normally looks up camera mode 0 for state `0E`, but `FUN_00534C60` sees previous Player state `09/0A` and **forces the camera request back to mode 2**.

State `0E` then performs the normal active-item class selection and calls `FUN_00528F40(primaryWeaponState)`, whose camera mode is again 2.

The effective camera sequence is therefore:

```text
Player 09/0A (camera mode 2)
    -> Player 0E
       nominal state-table mode = 0
       camera setter special-case forces mode = 2
    -> Player primary weapon state 14..2C
       camera setter request = 2 again
```

This produces an explicit repeated camera-mode-2 request by stock code.

### Why this is exactly the PC/Xbox divergence shape

On the second mode-2 request:

```text
Xbox:
    SetCameraMode(2)
    -> unconditional fresh mode-2 re-anchor/transient reset

PC:
    SetCameraMode(2)
    -> previousCameraMode == 2
    -> Gate A skips orientation/anchor synchronization
    -> Gate B skips +0x8C..+0xA8 reset
```

So `09/0A -> 0E -> primary weapon` is the first **ordinary state-machine sequence** found that mechanically exercises the exact PC-only reset suppression without requiring CEvent `0xA6/4`.

This also explains why the camera setter contains both pieces of unusual code in the same subsystem:

1. a special rule that intentionally keeps camera mode 2 across leaving Player states `09/0A`;
2. a PC-only optimization that suppresses reinitialization when mode 2 is requested while already in mode 2.

Those two policies interact.

### Critical remaining reachability question

Reverse engineering identifies Player `09/0A` as the original Xbox combat-strafe left/right states. PC retains their handlers/consumers, but the known original Xbox shoulder-button ingress was not previously established as live on shipped PC.

Therefore this is now the strongest architectural bad-entry candidate, but reporter causality depends on PC reachability:

```text
Can stock/shipped PC enter Player state 09 or 0A in ordinary gameplay,
without our restored strafe work?
```

If YES, this is a direct stock producer with an excellent structural match.
If NO, it remains a preserved-but-orphaned original sequence and cannot explain the historical PC report.

Status: `STRONGEST MODE2 RE-ENTRY FOUND; PC 09/0A INGRESS REACHABILITY NOW CRITICAL`.

## Checkpoint 7 — `09/0A` re-entry is real but stock PC ingress is cut

**Date:** 2026-09-29

The critical reachability check for the `09/0A -> 0E -> primary` re-entry is now closed using the existing Xbox-vs-PC input research.

Recovered original Xbox ingress:

```text
capability 0x2000 passes
Player combat flag bit passes
LB logical pressed-edge, RB not held -> Player state 09
RB logical pressed-edge, LB not held -> Player state 0A
```

Xbox executes this through `sub_82302EC8` from the high-level Player update.

The Director's Cut PC homolog retains:

- Player states `09/0A` in the dispatch table;
- their mirrored `0x242C..0x2431` motion consumers;
- camera mode 2 for both states;
- completion to state `0E`;
- the camera-setter special case that preserves mode 2 while leaving `09/0A`.

But the high-level shoulder-button producer call itself is absent from shipped PC. This was already classified in the input RE as **producer/ingress cut with consumer state machine retained**. ZachFix's optional restored Combat Strafe explicitly reintroduces this missing ingress.

Therefore:

```text
09/0A -> 0E -> primary mode2 re-entry
```

is a genuine engine sequence and exposes the PC reset-policy flaw, but it is **not reachable in stock Director's Cut through the original ordinary input path**.

Consequences for this aim-bug investigation:

- it cannot by itself explain historical vanilla-PC reports unless another producer of `09/0A` is found;
- it remains important for ZachFix's optional restored Combat Strafe, because that restoration makes the preserved re-entry live again;
- for the reporter's original stock-compatible bug, continue looking for a mode-2 state/re-entry that remains reachable on PC.

Next narrow target: Player state `0C`, which itself maps to camera mode 2 and is retained on PC. Its exit path must be checked for a direct handoff into another mode-2 weapon state.

Status: `09/0A ARCHITECTURAL MATCH, STOCK-PC CAUSALITY REJECTED VIA MISSING INGRESS`.

## Checkpoint 8 — State `0C` does not supply a stock mode2 re-entry

**Date:** 2026-09-29

Player state `0C` maps to camera mode 2, so it was checked as another possible stock bridge between two mode-2 states.

Steam state `0C` handler is `0x00524DA0`. Its directly observed Player-state transition on AIM loss is:

```text
00524F7F  query held action 0x800000
...
AIM == 0:
00524FB2  push 0
00524FC2  call FUN_00528F40
```

So the clear exit is `0C -> 00`, i.e. mode2 -> mode0, not another mode2 state.

A full direct `FUN_00528F40` call census contains no literal state-`0C` request. A secondary scan of decompiled dynamic-setter functions found only two functions that even contain both a `0x0C` temporary constant and a Player SetState call (`FUN_00504510` and `FUN_00522D50`); in both, the `0x0C` temporary belongs to unrelated helper arguments while their actual Player-state calls select other states (`0x85` in the relevant `FUN_00504510` branch; fixed `4/6/8` in `FUN_00522D50`).

The alternate raw state writer `FUN_00528EF0` has only one callsite and is invoked with literal state `2`, so it also does not provide `0C` ingress.

Result:

- state `0C` is a real PC consumer / camera-mode-2 handler;
- no ordinary shipped-PC producer into `0C` has been found in the complete direct setter/writer census;
- its known exit returns to mode0.

It therefore does not currently explain the historical stock-PC aim bug.

Status: `0C DEPRIORITIZED / NO STOCK INGRESS FOUND`.

## Checkpoint 9 — No independent direct writer found for `CCamera+0x154`; D01 containers do not contain XFE bytecode

**Date:** 2026-09-29

A direct-write census was used to test an alternative mechanism: Player could already have left a mode-2 state while `CCamera+0x154` silently remained `2`, causing the next otherwise-normal aim entry to be mistaken for a repeated mode-2 request.

For the actual CCamera mode field, the live camera subsystem's direct write remains the assignment inside `FUN_00534C60` (`0x00534CE4` in Steam). Other raw writes to an unrelated object's `+0x154` belong to other constructors/structures and do not reference the camera singleton. No direct `DAT_00BE1EA4[0x55] = ...` writer and no obvious whole-object `memcpy` into the camera singleton was found.

Together with the existing finding that `FUN_00534C60` has only the Player SetState callsite, current static evidence does not support an independent mode-field desynchronization producer.

### Asset/content check

The available Prologue scene files were inspected as raw containers:

```text
D01.XPF   ~186 KiB manifest/prefetch content
D01.XCA   ~6.7 KiB, signature XCA1
```

`D01.XPF` contains the reference:

```text
V:/updata/scene/01/cpl01.xfe
```

but not the XFE command stream itself. `D01.XCA` contains no `cpl01`, `XFE`, event-path strings, or embedded obvious XFE payload. Therefore the current D01 files cannot prove whether shipped Prologue content executes CEvent opcode `0xA6`, subcommand `4` while Player is already in a primary weapon mode-2 state.

Status:

- independent `CCamera+0x154` writer/desync: `NO STATIC EVIDENCE`;
- `D01.XPF/XCA` content proof for `A6/4`: `BLOCKED BY MISSING raw CPL01.XFE`.

## Checkpoint 10 — Existing runtime logs prove `CPL01.XFE` is a live resource, but do not contain its payload

**Date:** 2026-09-29

Older Day/Night diagnostic logs in the project repeatedly resolve:

```text
result=5569 path="CPL01.XFE"
```

from the common resource lookup caller `DP.exe+0x002B2BBB`.

This strengthens one point: `CPL01.XFE` is not merely a stale manifest string in `D01.XPF`; the running game actually resolves it as live resource id `5569`.

However, the available logs record only path -> resource-id lookup. They do not include the raw resource bytes or decoded CEvent command stream, so they still cannot answer whether command `0xA6`, subcommand `4` occurs in `CPL01.XFE`.

Current evidence boundary:

```text
CONFIRMED: CPL01.XFE is live and looked up at runtime (resource 5569).
NOT AVAILABLE: resource 5569 payload / decoded command stream.
```

## 2026-09-29 checkpoint — normal ingress mostly closed; Xbox CEvent branch located

### Normal PC weapon ingress

The ordinary PC weapon boundary has now been traced through states `0E/0F`.

- `0E` and `0F` are camera mode 0.
- Their ITEM-class selectors enter the primary weapon family `14..2C`.
- Therefore normal weapon entry is structurally `mode0 -> mode2`, which forces the PC mode-2 initialization path.
- Direct SetState exits found inside the ordinary mode-2 weapon handlers go to state 0 or other non-mode-2 families; no common every-frame `mode2 -> mode2` reissue was found.

### Preserved strafe path is a real but non-stock mode2->mode2 producer

States `09/0A` use camera mode 2 and complete through `0E`. `FUN_00534C60` special-cases the `09/0A -> 0E` transition so the requested camera mode 0 is kept as mode 2. The next `0E` weapon selection therefore requests mode 2 while camera mode is already 2.

This is a genuine programmed `mode2 -> mode2` re-entry and exactly exposes the PC-vs-Xbox reset-policy difference. However, the Director's Cut PC removed the stock producer for `09/0A`; only the consumer states remain. Therefore this path is not a viable root cause for the long-standing shipped-PC aim bug unless the ZachFix opt-in Combat Strafe restoration is enabled.

### State 0C

State `0C` is also camera mode 2, but its handler exits to state 0 on AIM release, and no stock PC producer for entering `0C` was found in the current full setter census. Treat as another preserved/unresolved consumer, not a confirmed reporter path.

### Hidden/dynamic SetState callers

Previously unclassified dynamic callsites were rechecked. They resolve to non-mode2 states in the examined paths, including state `0D`, state `5`, and state `0`; the apparent `SetState(currentState)` at `0x004F8DDC` belongs to state `57` (camera mode 13), not aim mode 2.

### Xbox CEvent dispatcher counterpart

The exact Xbox counterpart of PC central CEvent dispatcher `FUN_00447B50` has been identified as `sub_82230A18`:

- both save the current VM/event cursor from `+0x18` into `+0x1C`;
- both clear `+0x68/+0x6C`;
- both dispatch on the opcode byte at command offset `+4`.

Xbox normalizes the opcode by subtracting 2 before indexing its jump table. Therefore PC opcode `0xA6` corresponds to Xbox switch index `164`, which dispatches to `sub_822215A0`.

This branch is now the active comparison target. The remaining task is to determine whether `sub_822215A0` is the Xbox semantic counterpart of PC `A6/4 -> FUN_0044D520` weapon/equip re-entry, or whether the PC command was restructured during the port.

## Final checkpoint — Xbox counterpart of PC `A6/4` confirmed

**Stop point requested by user: do not continue beyond this comparison.**

The exact Xbox counterpart of the PC event/interpreter path has now been identified and compared.

### Dispatcher correspondence

PC central CEvent dispatcher:

```text
FUN_00447B50
  opcode 0xA6 / signed -0x5A
    -> FUN_00436450
```

Xbox counterpart:

```text
sub_82230A18
  opcode normalization: opcode - 2
  PC opcode 0xA6 therefore lands at Xbox switch index 164
    -> sub_822215A0
```

`sub_822215A0` is structurally the Xbox counterpart of PC `FUN_00436450`: it consumes the same event-stream fields, validates the same object type family, stores the same event object bookkeeping, and dispatches on the first 16-bit subcommand.

For subcommand `4`:

```text
PC FUN_00436450 case 4
    -> FUN_00414E50(...)
    -> FUN_0044D520(...)

Xbox sub_822215A0, original subcommand 4
    -> switch index 2 after `subcommand - 2`
    -> loc_82221844
    -> sub_821FEEA8(...)
    -> sub_82237148(...)
```

`sub_821FEEA8` matches the role of PC `FUN_00414E50`; the important downstream helper is `sub_82237148`.

### `sub_82237148` is the exact Xbox weapon/equip helper counterpart

The Xbox helper reproduces the distinctive PC `FUN_0044D520` structure:

```text
ITEM.PRM stride = 220 / 0xDC
weapon class byte = record + 109 / 0x6D
```

It then verifies that the target object is the Player and maps weapon class to the same primary Player states.

Xbox decimal mapping recovered directly from `sub_82237148`:

```text
class 1  -> 20  = 0x14
class 2  -> 22  = 0x16
class 3  -> 24  = 0x18
class 4  -> 26  = 0x1A
class 5  -> 30  = 0x1E
class 6  -> 32  = 0x20
class 7  -> 36  = 0x24
class 8  -> 40  = 0x28
class 9  -> 42  = 0x2A
class 10 -> 43  = 0x2B
class 11 -> 38  = 0x26
```

This is exactly the PC primary weapon-state mapping previously recovered from `FUN_0044D520`.

After selecting the state, Xbox calls:

```text
sub_8232A300(selectedState)   // Player SetState
sub_82323C28()                // downstream companion setup
```

This directly corresponds to PC:

```text
FUN_00528F40(selectedState)
FUN_00522230()
```

### Critical result: no current-state guard on Xbox either

Immediately before `sub_8232A300(selectedState)`, the Xbox helper checks that the target object is the Player and derives the selected state from the weapon class. It does **not** check whether the Player is already in that state or whether the current camera mode is already mode 2.

Therefore the explicit event re-entry semantics are original engine behavior on both builds:

```text
A6/4 event command
    -> weapon-class selector
    -> explicit Player SetState(primaryWeaponState)
```

and this request may occur even when the Player is already in the corresponding mode-2 weapon family.

The important PC/Xbox divergence is downstream in the camera setter:

```text
Xbox explicit SetState(mode-2 state)
    -> SetCameraMode(2)
    -> mode-2 anchor/transient initialization runs unconditionally

PC explicit SetState(mode-2 state)
    -> SetCameraMode(2)
    -> if previousCameraMode == 2,
       PC skips the mode-2 re-anchor/transient reset gates
```

### Conclusion at requested stop point

**CONFIRMED:** PC `A6/4 -> FUN_0044D520` is not a PC-only oddity. The original Xbox contains the same event-command path and the same unguarded weapon-class-selected explicit Player state request.

This substantially strengthens the port-regression model: the event semantics survived, while the PC camera setter introduced `previousMode == 2` suppression that the Xbox setter does not have.

Still intentionally unresolved here:

```text
Does reporter-relevant shipped event content actually execute A6/4 at the moment that produces the bug?
```

Per user instruction, investigation stops at this comparison and does not proceed into event-content/runtime reachability.

## Checkpoint 11 — Content reachability blocked offline; minimal runtime A6/4 probe prepared

**Date:** 2026-09-29

The shipped-content question was pursued as far as the currently available Library permits.

### Offline content result

The older D01 collectors contain a working parser/extractor for the PC `DPSerial.*` containers. Therefore `CPL01.XFE` can in principle be extracted directly from the installed game and decoded/scanned offline.

However, the current Library does **not** contain the source `DPSerial.*` containers, and the previously generated Prologue/D01 asset bundles contain `D01.XPF`, `D01.XCA`, XAM/XMD/XPC dependencies, etc., but do not contain `CPL01.XFE` itself. Existing runtime logs only prove the live lookup:

```text
CPL01.XFE -> resource id 5569
```

They do not contain the payload or command execution trace.

So current offline evidence cannot prove or disprove occurrence of `A6/4` in shipped XFE content.

### Minimal direct runtime test

A diagnostic patch was prepared against `DPFIX-NG(20260929-092815)`.

It does **not** modify Player state, camera state, input, or event behavior. It installs one MinHook on the already-known native Player SetState routine and immediately forwards every call to the original trampoline.

The detour logs only when `_ReturnAddress()` falls inside the exact `A6/4` weapon selector helper range:

```text
Steam:  DP.exe+0x0004D520 .. +0x0004D6C3  (FUN_0044D520)
GOG:    DP.exe+0x0004D550 .. +0x0004D6F3  (FUN_0044D550 counterpart)
```

Player SetState target comes from the existing build profile:

```text
Steam: DP.exe+0x00128F40
GOG:   DP.exe+0x00129010
```

For each real Player-target invocation it records:

```text
[AimProbe][A6/4]
caller=...
old=...
oldPrev=...
requested=...
committed=...
committedPrev=...
mode2Reentry=YES/no
sameState=YES/no
```

`mode2Reentry=YES` means the A6/4 selector explicitly requested a camera-mode-2 Player state while the old Player state was already in the camera-mode-2 family. This is the exact situation where Xbox performs a fresh mode-2 camera reset and PC suppresses it through the `previousMode == 2` gates.

Interpretation of a reproduction run:

```text
A6/4 line + mode2Reentry=YES before/at bad aim
    -> shipped runtime reachability CONFIRMED for the suspected producer

A6/4 lines occur but only mode2Reentry=no
    -> command is live, but this run does not exercise the dangerous re-entry

No A6/4 lines during a successful aim-bug reproduction
    -> A6/4 is ruled out as the producer for that reproduction
```

Patch artifact:

```text
ZachFix_A6_aim_reentry_probe_2026-09-29.patch
```

`git apply --check` was run successfully against the supplied `DPFIX-NG(20260929-092815)` source snapshot.

Status: `READY FOR ONE RUNTIME REPRODUCTION PASS`.

## Checkpoint 11 — shipped-content verification route reduced to one narrow runtime probe

**Date:** 2026-09-29

The remaining content question is whether live shipped event content actually executes the already-confirmed `A6/4` weapon helper while the Player is already in a camera-mode-2 state.

### Offline extraction status

Older D01 asset collectors contain a working `DPSerial.*` extractor and can recover XFE payloads by relative path. However, the Library currently does not contain the game's `DPSerial.*` archives themselves. The available `D01.XPF/XCA` files only reference `CPL01.XFE`; they do not embed its event bytecode. Therefore offline `CPL01.XFE` decoding is blocked specifically by missing source archives, not by lack of an extractor.

### Minimal runtime proof design

A lower-risk probe than hooking the whole CEvent helper was selected:

```text
hook Player SetState
    ↓
inspect _ReturnAddress()
    ↓
only log calls whose caller lies inside the A6/4 weapon helper
    ↓
record old Player state + requested Player state
```

This leaves the CEvent helper ABI untouched and filters out every unrelated Player state transition.

Build-local helper ranges established from raw assembly:

```text
Steam 1.01b: FUN_0044D520  RVA 0x0004D520..0x0004D6C3
  exact SetState call: 0x0044D6B3, return 0x0044D6B8

GOG 1.01b:   FUN_0044D550 + Ghidra-split orphan tail
              RVA 0x0004D550..0x0004D6F3
```

The probe classifies the known mode-2 Player states and emits:

```text
[AimBadEntry][A6/4] oldState=.. requestedState=..
oldMode2=.. newMode2=.. mode2Reentry=YES/no caller=DP.exe+...
```

A `mode2Reentry=YES` line is direct runtime proof that shipped event content reached the exact PC reset-policy hazard identified statically.

### Implementation status

A temporary research-only ZachFix source patch has been prepared against the current 2026-09-29 source snapshot. It hooks the existing build-profile `stateTransitionRva`, supports Steam/GOG helper ranges, and changes no Player/camera behavior beyond logging.

Status: `PROBE PREPARED; RUNTIME RESULT NOT YET AVAILABLE`.

## Checkpoint 11 — CPL01.XFE corrected: facial-expression asset, not CEvent bytecode

**Date:** 2026-09-29

The shipped `CPL01.XFE` payload was finally inspected directly. It begins with:

```text
XAM2
```

and contains facial rig/node names such as:

```text
N_EYEL
N_EYER
NT_JAW
NT_MOUTHL
NT_MOUTHR
NT_BROWL1
NT_BROWR1
...
```

Therefore the earlier working assumption that `CPL01.XFE` carries CEvent command bytecode was wrong. `.XFE` here is a facial/expression animation resource.

Static executable strings identify the actual event content namespace as:

```text
UPDATA/EVENT
UPDATA/EVENT/PROJECT.DPB
UPDATA/EVENT/00/0_0011.DSB
UPDATA/EVENT/01/0_0005.DSB
...
```

Existing DPSerial research independently counted **656 DSB records** in the PC asset set.

Revised content-level target for the `A6/4` reachability question:

```text
UPDATA/EVENT/**/*.DSB
+ preferably UPDATA/EVENT/PROJECT.DPB for indexing/context
```

The safest offline verification is therefore to scan the complete shipped PC EVENT directory, not scene-local XFE/XCA/XPF assets.

Status:

- `CPL01.XFE as event bytecode`: DISPROVEN
- `.DSB as shipped CEvent content family`: STRONGLY SUPPORTED by executable path strings / event namespace
- next offline requirement: extracted `UPDATA/EVENT` tree or equivalent DSB payload set

## Checkpoint 12 — Full shipped EVENT census closes A6/4 as normal-gameplay root cause

**Date:** 2026-09-29

The complete extracted PC `UPDATA/EVENT` tree was supplied and parsed offline:

```text
656 DSB files
61,498 structurally decoded CEvent commands
0 parser errors
```

### DSB command layout confirmed

The command stream is self-delimiting:

```text
+0x00  u16 control/branch field
+0x02  u16 operand byte count
+0x04  u16 opcode word        // CEvent dispatch uses the low byte
+0x06  u16 flags
+0x08  operand bytes
```

Thus the next command is exactly:

```text
next = command + 8 + operandByteCount
```

For opcode `0xA6`, `FUN_00436450` consumes the first operand word as the subcommand, so `A6/4` can be identified structurally rather than by raw-byte pattern matching.

### Full census result

Across all 656 DSBs:

```text
A6 commands total:     55
files containing A6:   17
A6 subcommand 4:        2
files containing A6/4:  1
```

Both `A6/4` commands occur in exactly one file:

```text
UPDATA/EVENT/91/0_0455.DSB
routine: 銃撃テスト  ("Shooting Test")
```

The two commands are nearly adjacent:

```text
0x34CC  A6/4  operands: 04 00 00 00 FF FF FF FF FF FF
0x34EA  A6/4  operands: 04 00 01 00 FF FF FF FF FF FF
```

The immediate routine sequence is:

```text
A6/3
A6/4
opcode 33
A6/4
opcode 33
A6/5
A6/6
END
```

### `0_0455.DSB` is unambiguously a developer/test event collection

Representative routine names decoded from CP932:

```text
バグチェック                         Bug Check
体を向ける拡張テスト                 extended body-turn test
口パクテスト                         lip-sync test
SEテスト                             SE test
ＢＧＭ再生テスト                     BGM playback test
モーションテスト                     motion test
カメラコンストレイン                 camera constraint
ＬＤ読み込みテスト                   LD loading test
銃撃テスト                           Shooting Test
半透明化テスト                       transparency test
マップ移動テスト                     map-move test
スキップテスト                       skip test
強制終了テスト                       forced-exit test
クリエイトアイテムテスト             create-item test
```

All 62 routines in the file follow the same internal test/debug character.

### Conclusion

`A6/4 -> weapon-class-selected Player SetState` remains a real engine mechanism and remains an excellent proof of why the Xbox camera setter's unconditional mode-2 reset is semantically safer than the PC `previousMode == 2` suppression.

However, the complete shipped EVENT census shows that `A6/4` is **not used by ordinary shipped gameplay content**. Its only content occurrence is the developer `Shooting Test` inside the broad internal test event `0_0455.DSB`.

Therefore:

```text
A6/4 as reporter root cause: CLOSED / REJECTED
A6/4 as architectural proof of valid explicit mode2 re-entry: RETAINED
```

This removes the strongest previously suspected content-driven bad-entry producer from the reporter investigation.

### Correction to earlier asset assumption

`CPL01.XFE` was inspected directly and is an `XAM2` facial-expression asset containing facial node names (`N_EYEL`, `NT_JAW`, `NT_MOUTH*`, `NT_BROW*`, etc.). It is not CEvent bytecode. Actual CEvent scripts are the `.DSB` files under `UPDATA/EVENT`.
