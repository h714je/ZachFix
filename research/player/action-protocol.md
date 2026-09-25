# CPlayer object-action protocol — correction to the vehicle taxonomy

**Pass:** 2026-09-25, vehicle continuation. **Evidence:** GOG PE machine code; Steam PE comparison. No runtime experiment in this pass.

## Architectural correction

`Game+0x8C57C` is the beginning of a **0x48-byte object-action packet**, not an exclusively vehicle-owned pointer. Its first word is the participating object; vehicle setup places the selected car there, while the common action ingress copies an arbitrary supplied packet there.

The decisive GOG instructions are `0050A421` (candidate action stored at `Player+0x660`), `0050A431..0050A452` (destination `Game+0x8C57C`, `ECX=0x12`, `REP MOVSD`). Candidates `57/58` have a separate packet destination at `Game+0x838DE8`; additional preservation rules follow at `0050A454..0050A4CC`. Steam has the corresponding code at `0050A351..0050A3FC`.

Consequences:

- Reading `+0x8C57C` proves object-action context, **not the object's vehicle type**.
- Withdraw the blanket vehicle labels for `2E..37`, `39/3A/3B/41/42/51/55/56/65/67`, and `43/44/5A..63/66/68`. Their per-object gameplay identities need producer/receiver evidence.
- `38/87/88` remain positively vehicle-related: seat marker, car fields, car event consumer, and explicit state transitions independently identify them.
- `+0x8C5A8` is **inside the same packet**, at offset `+0x2C`. Its use as a world-resource target/index remains distinct from the object pointer at packet `+0`, but these are not two disjoint storage structures.

The earlier generic-world setup clearing the first word did not prove vehicle exclusivity. It proved that an index-based interaction can leave the packet's object-pointer field empty.

## Recovered packet fields

Names below are descriptive, not recovered C++ member names.

| Packet offset | Game offset | Observed use |
|---|---|---|
| `+00` | `+8C57C` | action object pointer; selected car in vehicle paths |
| `+04..+10` | `+8C580..+8C58C` | position/transform vector copied into Player position fields |
| `+24` | `+8C5A0` | heading/yaw used by alignment and camera code |
| `+28` | `+8C5A4` | action-dependent argument; vehicle setup stores `car+424` here |
| `+2C` | `+8C5A8` | world-resource target/index in index-based interaction setup |
| `+3C` | `+8C5B8` | protocol phase/status/command word; interpretation depends on event |
| total | `+8C57C..+8C5C3` | `18 * 4 = 0x48` bytes copied by common ingress/refresh |

**Player Event `0x67` refreshes the whole packet.** GOG dispatcher table selects `00534173`; `00534190` calls `00509F90`, whose `00509F9B..00509FA6` copies 18 DWORDs. Steam: dispatcher `005340A3`, helper `00509EC0`. This establishes a concrete external refresh route; it does not identify every producer of every phase value.

## Keep four numeric domains separate

1. Native object event ID, passed as a callback argument.
2. Candidate action/state code selected by GOG `005092A0` / Steam `005091D0`, stored at `Player+660`.
3. Committed gameplay state at `Player+654`.
4. Packet phase/status at `packet+3C` / `Game+8C5B8`.

The selector is conditional on capability checks. A candidate is not an unconditional state transition: special handling in the ingress may intercept it. In particular, the selector can return codes `69..75` despite null gameplay-handler slots there. This pass does not establish those candidates as live `+654` states.

The complete extracted selector, capability masks, and both builds' case addresses are in [action_selector.md](action_selector.md) and [protocol.json](../evidence/vehicle_protocol/protocol.json).

## Shared completion states

GOG `004F56F0` / Steam `004F5620` serves `43/44/5A..63/66/68`. Its local phase counter is `013928A4` in the inspected builds.

- Phase 0 aligns Player/camera to the packet transform and advances the local counter.
- Phase 1 waits for **packet status `+3C == 0x20`**, then advances; it does not complete on the same invocation.
- Phase 2 selects the outgoing event, writes packet status `0x1D`, invokes the optional global callback and the object's `+44` callback, restores Player flags, requests gameplay state `00`, and resets the local counter.

GOG anchors: wait `004F58D9..004F58F5`; phase write `004F5809`; callback arguments `004F5839..004F5856`; state `00` transition `004F58B5..004F58C5`.

| Gameplay state | Outgoing native event | Packet `+3C` |
|---|---|---|
| `43` | `3D` | `1D` |
| `44` | `3E` | `1D` |
| `5A` | `53` | `1D` |
| `5B` | `54` | `1D` |
| `5C` | `55` | `1D` |
| `5D` | `56` | `1D` |
| `5E` | `57` | `1D` |
| `5F` | `58` | `1D` |
| `60` | `59` | `1D` |
| `61` | `5A` | `1D` |
| `62` | `5B` | `1D` |
| `63` | `4B` | `1D` |
| `68` | `5C` | `1D` |
| `66` | no fixed mapping | no fixed mapping |

This is **one event ID plus a packet field**, not the old “first event `1D`, second event …” interpretation. The same event/packet may be delivered to two callback layers.

For the 13 fixed cases, the outgoing event is the same event that the common selector maps to that candidate. The family is therefore a reusable action completion/acknowledgement mechanism. Concrete object types are not established by this mechanism alone.

### State `66` deserves separate investigation

The selector maps Event `4E` to candidate `66` behind capability `0x400`. The gameplay table maps `66` to the shared handler, but its completion jump table selects the default block at GOG `004F57FC` / Steam `004F572C`.

That block loads **both** event and status from the same stack local (`[ESP+0x0C]` after the saved-register pushes), rather than loading constants. Raw GOG stack reconstruction strengthens the finding: the function begins with `SUB ESP,8`, and on the reviewed phase-2 path neither of those fresh local DWORDs is initialized before the default block consumes the relevant slot. Thus, **if this default path is reached, the code statically reads stale/uninitialized stack data**. Reachability and runtime effect remain open, so this is a latent-bug candidate rather than an observed runtime bug. Do not call state `66` reserved/inert solely by analogy with the weapon aliases.

## Evidence and reproducibility

The included [extract_protocol.py](../evidence/vehicle_protocol/extract_protocol.py) reads supplied PE bytes, uses GNU objdump for bounded code ranges, and reads branch tables as data. It executes no game code.

```sh
python extract_protocol.py DP_GOG.exe DP_STEAM.exe output_directory
```

Verified: all 137 existing gameplay slots agree with the observed local handler offset; 119 non-null, 82 unique handlers. Both builds agree on all 80 inspected selector entries and the shared completion mappings. The `0xD0` offset is used only for the checked regions, not as a universal build conversion rule.

The `.asm` evidence files end before known inline branch tables where practical. JSON records the decoded table results. No runtime reachability, animation appearance, or friendly object name was inferred from the binary comparison alone.
