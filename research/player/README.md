# Player state machine and object-action protocol

**Source:** engine map v10 Player/action reconciliation, 2026-09-26.

## CPlayer state domain

`CPlayer+0x654` is a 137-slot gameplay-state domain extending through at least
`0x88`. It is not the old 22-state model.

High-confidence families:

```text
00                      ordinary locomotion anchor
0D                      operated-object state; LEVER.PRM clue
0E..2C                  weapon/equipment/combat
2E/2F/30/31             Door-family actions
32                      shared Event2E object manipulation
33..36                  object-action states; identities partial
37                      CObjectPhone via native Event 2F
38                      vehicle entry/exit animation hub
39/3A                   HideBox/conceal
3C/3D/3E/3F/40/45/
4B/4C/4D/4E             generic world-object interaction
4F                      Item/Coffee action
51                      Kaysen3-side action; strong owner
52/53/54                special actor/scripted-control layer
55/67                   Jukebox variants
56                      Autoslide
57                      Kaysen2-side action; strong owner
58                      CNpcEnemy/CNpcThrow-family Event66 action
59                      internal-code-66 companion state, not native Event66 ingress
65                      Telescope via native Event68
43/44/5A..63/66/68      shared object-action completion
7B/7C                   item-use special actions
7D                      special external-object interaction
7E..85                  damage/reaction/fatal convergence
87                      active player car
88                      vehicle-exit prelude
```

## Four numeric domains must stay separate

A recurring source of bad labels was treating equal numeric values as one namespace.
Keep these distinct:

1. native object event ID;
2. candidate action/state code selected by the action selector and stored at
   `Player+0x660`;
3. committed gameplay state at `Player+0x654`;
4. action-packet phase/status at packet `+0x3C`.

The selector candidate domain is wider than the gameplay state machine. For example:

```text
CObjectShaft     native Event 41 -> candidate 6C
CObjectExclusive native Event 50 -> candidate 71
CObjectVendor    native Event 62 -> candidate 73
```

These do not prove live `CPlayer+0x654` states because ingress can intercept candidates
before commit.

## Generic object-action packet

`Game+0x8C57C` is the start of a generic 0x48-byte action packet, not a vehicle-only
pointer.

Recovered fields:

| Packet offset | Game offset | Observed use |
| ---: | ---: | --- |
| `+00` | `+8C57C` | action object pointer; selected car in vehicle paths |
| `+04..+10` | `+8C580..+8C58C` | position/transform vector copied into Player fields |
| `+24` | `+8C5A0` | heading/yaw for alignment/camera code |
| `+28` | `+8C5A4` | action-dependent argument; vehicle setup stores `car+424` |
| `+2C` | `+8C5A8` | world-resource target/index in index-based interactions |
| `+3C` | `+8C5B8` | protocol phase/status/command word |
| total | `+8C57C..+8C5C3` | 18 DWORDs / 0x48 bytes |

Player Event `0x67` refreshes the whole packet through an 18-DWORD copy.

## Shared action completion

GOG `004F56F0` / Steam `004F5620` serves states
`43/44/5A..63/66/68`.

The normal completion contract is:

```text
phase 0 -> align Player/camera to packet transform
phase 1 -> wait for packet+0x3C == 0x20
phase 2 -> write packet status 0x1D
           dispatch one native event
           restore Player flags
           request state 00
```

This corrected an older interpretation that treated packet status `0x1D` as a first
event in a two-event sequence.

State `66` falls into a default branch that statically reads an uninitialized/stale
stack local for event/status on the reviewed phase-2 path. Reachability is not yet
proven, so this is a latent-bug candidate, not a confirmed runtime bug.

## Player-specific CCT bridge

York's normal post-state controller reconciliation lives in:

```text
GOG   FUN_004E3350
Steam FUN_004E3280
```

The previously suspected `FUN_0048B0A0 -> FUN_004831C0 -> FUN_00481D70` path belongs
to NPC/common-character inheritance and includes a `CNpcDog` override.

## Vehicle choreography

Confirmed player-car path:

```text
state 38 entry animation hub
    -> commit vehicle entry
    -> state 87 active player car
    -> state 88 exit prelude
    -> state 38 exit animation hub
    -> cleanup -> state 00
```

State 87:

- York is attached to `PM_SEAT_ML`;
- selected car uses marker `M_CENTER`;
- `car+0x434 & 0x8000` selects the live player-car scheduler branch;
- steering target `car+0x4E0` is produced with a `gameDelta60`-scaled slew.

State 88 copies car yaw back to Player, clears `car+0xDC` bit 2, and requests state
38. It does not perform every final cleanup step itself.

The car-facing protocol uses native Event `0x35` and packet phase values `0x0F..0x15`.
Those phase values are not additional event IDs.

## Special actor/scripted-control states 52/53/54

A separate context at `Game+0xCEB20/+0xCEB5C` is populated by boss/actor code and is
not the generic 0x48-byte object-action packet.

CPlayer Event `0x6D` selects:

```text
payload mode 2 -> state 53
payload mode 3 -> state 54
payload mode 4 -> state 52
```

State 52 is also reachable from native object-action Event `0x5E`.

## Research rule

Do not name a state from a shared packet field or shared handler alone. Prefer:

```text
producer/receiver object type
+ native event
+ selector mapping
+ committed state
+ concrete animation/resource/field use
```

That rule is what separated the genuine vehicle subset `38/87/88` from the much
broader object-action family.
