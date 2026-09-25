# Original Xbox-only player controls

**Status:** active Xbox360 -> Director's Cut PC comparison.

## Combat strafe: CONFIRMED Xbox path, consumer preserved on PC

Original Xbox input/gameplay path:

```text
LB logical mask 0x0100 edge, RB not held -> CPlayer state 09
RB logical mask 0x0200 edge, LB not held -> CPlayer state 0A
```

Xbox ingress is inside `sub_82302EC8`. It uses the game's logical input-query helpers and enters states through `sub_8232A300`.

The paired state handler chooses mirrored motion-resource triplets and returns to state `0E` after completion.

Director's Cut PC preserves the consumers:

- GOG state table: `09 -> 005249E0`, `0A -> 005249E0`;
- the handler still references motion IDs `0x242C..0x2431` (decimal 9260..9265);
- state `09` selects the `0x242F/2430/2431` side, state `0A` the `0x242C/242D/242E` side;
- completion requests state `0E`.

The high-level Director's Cut Player update retains surrounding gameplay logic but the Xbox `sub_82302EC8` strafe-ingress call is absent from the identified PC homolog. This is therefore best described as **producer/ingress cut with consumer state machine retained**.

**Classification:**

```text
09 = Xbox combat strafe via LB side (Strafe Left by original control mapping)
0A = Xbox combat strafe via RB side (Strafe Right by original control mapping)
CONFIRMED Xbox semantics / CONFIRMED PC consumer remnants
```

Do not confuse the physical shoulder mask with the later PC `configJ.cnf` action masks; they are separate layers.

### Exact Xbox strafe gate

The final Xbox ingress block is fully recovered:

```text
capability 0x2000 must pass
Player+0x708 bit 0x10 must be set

logical 0x0100 pressed edge
    + logical 0x0200 not held
    -> state 09

logical 0x0200 pressed edge
    + logical 0x0100 not held
    -> state 0A
```

`sub_82300068(...,0x2000)` is structurally equivalent to PC `FUN_004FEF30(...,0x2000)`. Xbox current-state storage is at `Player+0x724` versus PC `Player+0x654`, a `0xD0` layout shift, so Xbox `Player+0x708 & 0x10` corresponds to PC `Player+0x638 & 0x10`.

The exact original semantics can therefore be recreated with surviving PC primitives:

```text
FUN_004FEF30(player, 0x2000) != 0
&& (Player+0x638 & 0x10) != 0
&& shoulder edge/held test
-> FUN_00529010(player, 09/0A)     [GOG]
-> FUN_00528F40(player, 09/0A)     [Steam]
```

### Missing PC call site / restoration hook candidate

Xbox high-level Player update `sub_82304FF0` calls the strafe ingress helper `sub_82302EC8` at the common tail immediately before final state bookkeeping. The identified Director's Cut homologs retain that tail but omit the call.

Static mid-hook candidates at the corresponding point are:

```text
GOG   00504567   CMP [ESI+654],0
Steam 00504497   CMP [ESI+654],0
```

A trampoline immediately before those instructions can run the recovered gate and request `09/0A`. This placement is preferred over a generic post-function hook because Xbox early-return paths bypass the strafe check as well.

**Patch status:** opt-in runtime bridge implemented with the recovered native combat gate and physical LB/RB edge semantics. Steam 1.01b runtime validation confirmed correct one-press/one-request edge behavior for both `09` and `0A`; GOG remains statically mapped but not yet runtime-tested.

## Quick Turn: Xbox mechanism confirmed; PC remnants strong

Xbox quick turn is a combo path, not an independent binding:

```text
left-stick-back condition
+ X edge
-> target yaw = current yaw + pi (normalized)
-> transition to temporary state 0B
```

State `0B` is a heading-interpolation state. The transition machinery preserves the previous gameplay state in the Player object; state `0B` slews current yaw toward target yaw and returns to that saved state once aligned.

Director's Cut PC preserves:

- state `0B` handler (`00524CC0` GOG) with the same temporary heading-interpolation role;
- `yaw + pi` / normalized-angle branches entering state `0B` in the Player update;
- the surrounding input/gate architecture.

Director's Cut action conditions differ from the original Xbox combo, but the structural homologs are now isolated. Xbox uses capability `0x100` followed by logical action `0x4000` (the original Run/X action) before `yaw + pi -> state 0B`. PC keeps capability `0x100` but changed the trigger.

### Normal locomotion Quick Turn branch

GOG:

```text
00504095  PUSH 0x100       ; capability gate, keep unchanged
005040A2  CALL 004FEF30
005040B0  PUSH 0x100       ; altered Director's Cut input trigger
005040B7  CALL 004547E0    ; pressed-edge query
...
005040D5  add pi
...
00504109  PUSH 0x0B
00504110  CALL 00529010
```

Steam equivalent trigger is `00503FE0`; transition is `00504040 -> 00528F40`.

PC logical `RUN` is `0x400000` through `FUN_00454720`, while `0x100` is not one of the remapped `configJ.cnf` action masks and passes through as a raw mask. Therefore the minimal Xbox-semantics candidate is:

```text
GOG   005040B0 : PUSH 0x00000100 -> PUSH 0x00400000
Steam 00503FE0 : PUSH 0x00000100 -> PUSH 0x00400000
```

Byte form in both builds:

```text
68 00 01 00 00  ->  68 00 00 40 00
```

The capability `PUSH 0x100` at GOG `00504095` / Steam `00503FC5` must **not** be changed.

### Combat/weapon-context Quick Turn branch

Xbox performs the stick-back/angle test, sets the turn-ready bit, checks capability `0x100`, then queries the same original Run/X logical action and enters `0B`. Director's Cut added a `HOLDBREATH` dependency and changed the final trigger to `LIGHTONOFF`.

GOG altered region:

```text
00504459  PUSH 0x800000    ; HOLDBREATH
00504460  CALL 004547A0    ; held/current
00504467  JE   00504476    ; prevents setting turn-ready bit
...
005044A4  PUSH 0x100       ; capability, keep
...
005044BF  PUSH 0x2000      ; LIGHTONOFF trigger
005044C6  CALL 004547E0
...
00504518  PUSH 0x0B
```

Steam homologs: `00504389`, `00504397`, `005043D4`, `005043EF`, `00504448`.

Minimal Xbox-style static candidate:

```text
GOG   00504467 : 74 0D -> 90 90
Steam 00504397 : 74 0D -> 90 90

GOG   005044BF : 68 00 20 00 00 -> 68 00 00 40 00
Steam 005043EF : 68 00 20 00 00 -> 68 00 00 40 00
```

This keeps the surviving PC angle/yaw/state machinery, removes only the extra Director's Cut HoldBreath requirement, and routes the trigger through PC's remappable `RUN` logical action.

**Classification:** Xbox Quick Turn = **CONFIRMED**; PC state `0B` and both turn branches = **CONFIRMED preserved remnants**. The first runtime trigger experiment was removed after it proved awkward to validate and bind cleanly; Quick Turn is research-only again.

## Director's Cut PC action-mask mapping recovered from config loader

```text
0x000040 = AIM
0x000080 = ATTACK
0x000400 = OBSERVE
0x000800 = RELOAD
0x001000 = INTERACT
0x002000 = LIGHTONOFF
0x400000 = RUN
0x800000 = HOLDBREATH
```

These are PC logical action masks, not raw XInput button bits.

## Restoration implication

The preferred restoration strategy is to reactivate the surviving native PC Player states/branches rather than implement bespoke ZachFix movement. For Strafe in particular, the consumer state machine is already present in both PC builds; the missing piece is the original ingress/control semantics.

## Reproducible evidence

Raw Xbox/GOG/Steam excerpts are indexed in [evidence/xbox_only_controls/README.md](../evidence/xbox_only_controls/README.md).
