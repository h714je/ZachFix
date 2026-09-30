# SaveAnywhere corrective verification

**Pass:** 2026-10-01
**Scope:** Deadly Premonition: The Director's Cut PC, Steam primary with retained GOG/vehicle cross-checks.
**Status:** final corrective static pass for the save-anywhere feasibility research.

This pass exists because an intermediate investigation over-interpreted two facts:

1. `record+0x14` can contain a live runtime object pointer and is physically inside the
   serialized GameRecord image;
2. state `0x38` can consume the action-object pointer during live vehicle choreography.

Those facts are individually real, but normal load does **not** restore the pre-save
`CPlayer+0x654` value. The missing reachability edge invalidates the proposed
`save in 0x38 -> reload in 0x38 -> stale-pointer dereference` chain.

The same pass also rejected a second false ownership match at `object+0x434 bit 0x8000`.
As elsewhere in this engine, a matching field offset is not class ownership evidence.

## 1. Final disputed-claim audit

| Claim | Final status | Primary evidence / correction |
|---|---|---|
| Pre-save CPlayer state `0x38` is restored by normal load | **DISPROVEN** | `CPlayer+0x654` is not part of GameRecord; the reviewed load/reconstruction path chooses state through reconstructed defaults and explicit resume adapters instead. |
| Default reviewed post-load Player path selects state `0x00` | **CONFIRMED** | In the Player initialization stage, `FUN_00507BA0` tests `FUN_004FD860(0,4)` and calls Steam `FUN_00528F40(0)` when that special high-mask token is absent. |
| `record+0x64 bit 0x4` can override default reconstruction with state `0x40` | **CONFIRMED** | Steam `FUN_00506F70` tests the same high-mask token and calls `FUN_00528F40(0x40)` on the special-resume branch. |
| `record+0x14` can physically contain a serialized transient pointer | **CONFIRMED** | `Game+0x8C568` is the live GameRecord base and `Game+0x8C57C == record+0x14`; whole-record copies include the field without a universal pointer sanitizer. |
| Serialized `record+0x14` guarantees a post-load state-`0x38` crash | **DISPROVEN** | Normal/default reconstruction does not restore state `0x38` merely because it was active before save, so the claimed post-load path into `FUN_004DDDB0` is absent. |
| No post-load code anywhere can ever consume `record+0x14` as a pointer | **OPEN** | The disproven vehicle path is not a global reader census. Other explicit resume adapters must be judged by their own consumers. |
| Steam `0x00577B1E` is the normal `CAutomobile+0x434 & ~0x8000` dismount clear | **DISPROVEN ownership attribution** | The site belongs to a different object-family path; the retained GOG/Steam vehicle RE already warns that the homologous `object+0x434` clear is not owned by `CAutomobile`. |
| Exact normal-dismount lifetime/transfer of `CAutomobile+0x434 bit 0x8000` is known | **OPEN** | Real car-mode reconfiguration helpers can clear/transfer the bit, but the ordinary `87 -> 88 -> 38 -> 00` choreography has not been proven to invoke that ownership transfer. |
| Mode 3 is selected while top-level/system state `0x46` is active | **CONFIRMED** | Steam `0x006427D5` compares the state to `0x46`; `0x006427E0` sets `Game+0x8C5EC bit 0x4000` on equality and `0x006427F1` clears it otherwise. Save builders map that flag to mode 3. |
| Friendly semantic name of state `0x46` is known | **OPEN** | The numeric state-to-mode relationship is proven; names such as "loading" or "transition" remain descriptive hypotheses unless independently closed. |
| `FUN_00506F70` performs the whole-record memcpy restore | **DISPROVEN** | Whole-record backup-to-live copy is owned by `FUN_0061A830`; `FUN_00506F70` performs post-load Player/world reconstruction and resume application. |

## 2. Post-load CPlayer state selection

The important correction is that GameRecord is not a serialized `CPlayer` object.
`CPlayer+0x654` is the live gameplay-state selector, but it is not a GameRecord field.
The load path reconstructs Player runtime and then applies persistent resume policy.

Two branches make the default/special split concrete in the reviewed Steam path.

### Special high-mask resume

Inside `FUN_00506F70`:

```text
FUN_004FD860(0, 4)
    != 0
        -> FUN_00528F40(0x40)
        -> special reconstruction continues
```

This is the already established `record+0x64 bit 0x4` one-shot resume adapter.

### Default Player initialization

The staged initialization path in Steam `FUN_005D4DE0` obtains the live Player and
invokes `FUN_00507BA0`. That Player initialization function contains:

```text
FUN_004FD860(0, 4)
    == 0
        -> FUN_00528F40(0x00)
```

Therefore the strongest supported wording is:

> In the reviewed normal/default load path, Player reconstructs into state `0x00`
> unless an explicit persistent resume adapter selects another state.

No reviewed persistent selector chooses state `0x38`. This is intentionally narrower
than claiming that no unknown adapter can exist anywhere in the executable.

## 3. Raw action packet and pointer risk

The structural relationship remains:

```text
Game + 0x8C568 == record + 0x0000
Game + 0x8C57C == record + 0x0014
```

The generic `0x48` action packet starts at `record+0x14`. Its first word can hold a live
runtime action-object reference during gameplay, while other packet fields are reused by
native persistent resume protocols such as the `0x75C` / state-`0x40` adapter.

Whole GameRecord capture therefore can preserve pointer-shaped session data. That does
**not** make the packet a valid serialized object graph.

The live vehicle state-`0x38` path still provides a real unsafe consumer if entered with
an invalid packet pointer: Steam `FUN_004DDDB0` reads `Game+0x8C57C` and later
dereferences fields from the resulting object. What is disproven is the post-load
reachability premise: normal loading does not restore state `0x38` from the pre-save
transient FSM state.

Current classification:

```text
serialized transient pointer bytes exist                       CONFIRMED
state-0x38 guaranteed post-load dereference                    DISPROVEN
all possible post-load readers of record+0x14+0x00 exhausted  OPEN
```

## 4. Whole-record backup/restore ownership

Keep the whole-record copies separate from reconstruction:

```text
FUN_00647130
    memcpy(Game+0xBE8, Game+0x8C568, 0x45CC0)
    ...

FUN_0061A830
    memcpy(Game+0x8C568, Game+0xBE8, 0x45CC0)
    ...

FUN_00506F70
    post-load Player/world reconstruction and resume policy
```

The exact higher-level use of `Game+0xBE8` depends on the caller, but the memcpy
ownership itself is direct binary evidence. Do not attribute the whole-record restore to
`FUN_00506F70`.

## 5. Mode 3 and top-level state `0x46`

The Steam writer is exact:

```text
006427D5  cmp [EBP], 0x46
006427E0  or  [Game+0x8C5EC], 0x00004000   ; equal branch
006427F1  and [Game+0x8C5EC], 0xFFFFBFFF   ; non-equal branch
```

The save builders test this bit and select mode 3. `FUN_004524C0` skips the normal
pre-save synchronization cluster for modes 3 and 4.

Canonical wording:

> Mode 3 is the native save context selected while top-level/system state `0x46` is
> active. The friendly semantic identity of state `0x46` remains OPEN.

The skip itself is proven. A specific historical rationale such as "coordinates are in
the void" should not be promoted to fact without separate evidence.

## 6. Vehicle scheduler bit `0x8000`

The retained vehicle RE remains authoritative:

```text
car+0x434 & 0x8000
    -> live player-car scheduler/ownership classification
```

Known car reconfiguration paths can clear `0x8000` and transfer the car into other mode
bits. However the normal York dismount spine:

```text
0x87 -> 0x88 -> 0x38 -> cleanup -> 0x00
```

still has no ownership-proven direct clear/transfer call.

The candidate clear at Steam `0x00577B1E` must not be used. It is another example of the
engine-wide `object+0x434` false-lead problem: matching offset and mask do not prove the
object is `CAutomobile`.

## 7. `FUN_0050B240` is not a generic action abort

A separate normalization pass tested `FUN_0050B240` as a possible generic way to make
in-flight interactions safe before saving. Raw control flow does not support that role.
The function clears `Game+0x8C57C` only as part of setup for a bounded map-target subtype
family (`0x759..0x766`) and writes coordinate-anchor data into the rest of the action
packet. It does not perform a general CPlayer state reset, camera teardown, or proven
object-side ownership cleanup.

Therefore:

```text
FUN_0050B240 == universal AbortAction / SaveAnywhere normalizer  DISPROVEN
known interaction cleanup remains family/protocol specific       CONFIRMED architecture
absence of every possible generic abort helper in the executable OPEN
```

This is another reason a first SaveAnywhere implementation should fail closed rather
than force `CPlayer+0x654 = 0` and zero the packet manually.

## 8. SaveAnywhere research consequence

The final static result is narrower than the discarded crash hypothesis, but more useful:

- arbitrary positions are not fundamentally blocked by hardcoded save-point coordinates;
- native load reconstructs transient Player runtime instead of restoring the previous
  live FSM byte-for-byte;
- explicit persistent adapters override the normal/default reconstruction when required;
- mixed live/persistent packet storage means unknown multi-phase protocols still need
  caller-specific proof before being declared universally resumable;
- state `0x00` is a sensible conservative ZachFix v1 whitelist, but it is a **mod policy**,
  not a native save prerequisite (phone save already proves native state `0x37` saving);
- modes 3/4, pending scripted-resume policy, and active world/fade transitions remain
  explicit fail-closed boundaries for a first experiment.

A conservative research envelope is therefore:

```text
DENY mode 3 / mode 4 contexts
DENY when pending/active scripted-resume policy would be disturbed
DENY active world/fade transition
require valid York
for a v1 general-purpose path, whitelist CPlayer state 0x00
preserve native persistent resume markers
invoke native runtime-to-persistent synchronization
capture/write the native GameRecord/save image rather than synthesizing fields
```

This does not claim that every non-zero Player state is inherently unsafe. It only keeps
unknown multi-phase resume contracts out of the first general-purpose SaveAnywhere path.
