# CPlayer object-action taxonomy

**Pass:** 2026-09-25 continuation  
**Primary evidence:** GOG raw PE / vtable ownership, with GOG decompiler used only to expose callback argument structure.  
**Cross-check:** shared action selector and action-packet reconstruction in `action_protocol.md` / `action_selector.md`.

This document classifies the broad object-action domain by **two-sided protocol evidence**:

```text
native event emitted toward Player
        ↓
action selector
        ↓
candidate code (Player+0x660)
        ↓
optional ingress/gating
        ↓
committed gameplay state (Player+0x654)
        ↓
Player choreography
        ↓
object callback / packet acknowledgement
```

A state is assigned an object family only when the native event ID is identified in the object's callback/event-handler domain or another independent class-specific anchor exists. Merely reading `Game+0x8C57C` does not identify the object type.

---

## 1. Numeric domains must remain separate

The following values can have the same numeric spelling while meaning different things:

1. **native object event ID** — callback argument / selector input;
2. **candidate action code** — selector result, stored at `Player+0x660`;
3. **committed gameplay state** — `Player+0x654`;
4. **packet phase/status/subcommand** — fields inside the 0x48-byte action packet, especially `packet+0x3C` and object-specific payload words.

This distinction is mandatory. Several attractive false classifications came from reading a packet subcommand as if it were a native event ID.

---

## 2. Confirmed two-sided object mappings

| Native event | Selector candidate | Current gameplay attribution | Receiver-side evidence | Status |
|---:|---:|---|---|---|
| `2D` | `2E` | Door-family action | `CObjectDoor` family native-event handler accepts `2D` | **CONFIRMED family** |
| `45` | `2E` | Door-family alternate ingress | same Door-family handler accepts `45` | **CONFIRMED family** |
| `46` | `30` | Door/SlideDoor action | Door-family and `CObjectSlideDoor` accept native `46` | **CONFIRMED family** |
| `47` | `2F` | Door-family action | Door-family handler accepts `47` | **CONFIRMED family** |
| `48` | `31` | Door-family action | Door-family handler accepts `48` | **CONFIRMED family** |
| `2E` | `32` | shared object-manipulation action | accepted by `CObjectFreight`, `CObjectChest`, `CObjectBronzeStatue`, `CObjectBrianStoneConstruction`, `CObjectPushWardrobe` | **CONFIRMED shared protocol** |
| `35` | `38` | vehicle entry/exit hub | `CObjectCar` Event35 consumer plus seat/car-specific evidence | **CONFIRMED vehicle** |
| `36` | `39` | HideBox / conceal action | `CObjectHideBox` accepts native `36` | **CONFIRMED family** |
| `37` | `3A` | HideBox / conceal action | `CObjectHideBox` accepts native `37` | **CONFIRMED family** |
| `2F` | `37` | Phone interaction choreography | `CObjectPhone` / type `0x63` producer builds the 0x48-byte packet with the Phone object at `packet+0` and sends native `2F`; selector commits `37`. GOG/Steam agreement. | **CONFIRMED family** |
| `3A` | `4F` | item-object action | `CObjectItem` / `CObjectCoffee` handler accepts native `3A` | **CONFIRMED family** |
| `4F` | `56` | Autoslide action | `CObjectAutoslide` accepts native `4F` | **CONFIRMED family** |
| `51` | `55` | Jukebox action variant A | `CObjectJukebox` accepts native `51` | **CONFIRMED family** |
| `52` | `67` | Jukebox action variant B | `CObjectJukebox` accepts native `52` | **CONFIRMED family** |
| `68` | `65` | Telescope interaction choreography | `CObjectTelescope` / type `0xA2` producer builds the action packet with the Telescope object at `packet+0` and sends native `68`; selector commits `65`. GOG/Steam agreement. | **CONFIRMED family** |

The exact player-facing labels within each family (open/close, enter/leave, activate/deactivate, etc.) remain open unless separately established by motion/resource/runtime evidence.


## 2.1 Strong producer-side mappings added in the late continuation

These mappings have real sender ownership and correct numeric-domain separation, but the most conservative friendly action names remain intentionally broad until the motion/resource meaning is recovered.

| Native event / other code | Gameplay state | Owner / role | Status |
|---:|---:|---|---|
| native `5D` | `51` | `CBossKaysen3`-side action choreography; packet producer stores the participating object in `packet+0` | **STRONGLY_SUPPORTED owner** |
| native `65` | `57` | `CBossKaysen2`-side action choreography; Steam preserves the producer path that GOG decompilation truncates | **STRONGLY_SUPPORTED owner** |
| native `66` | `58` | `CNpcEnemy` / `CNpcThrow`-family action choreography; at least two live sender contours use native Event `66` | **STRONGLY_SUPPORTED family** |
| internal/global subprotocol code `66` | `59` | companion/alternate state sharing handler `004F8A70`; **not** selected by native Event `66` | **CONFIRMED numeric-domain split / PARTIAL semantics** |

Critical consequence for `58/59`:

```text
native Event 66 -> selector -> state 58
internal/global code 66 -> state 59
```

The repeated numeric value `66` belongs to two different domains. State `59` must not be described as a second native-Event66 state.

---

## 3. Protocol candidates that are **not** gameplay states

The action selector is wider than the live `CPlayer+0x654` state table. Some object-native events select candidate values whose gameplay-state slots are null or otherwise intercepted before state commit.

Examples recovered in this pass:

| Native event | Candidate | Receiver class | Meaning for taxonomy |
|---:|---:|---|---|
| `41` | `6C` | `CObjectShaft` | selector/protocol candidate; **not** evidence for gameplay state `58/59` |
| `50` | `71` | `CObjectExclusive` | protocol-only candidate in the null-state region |
| `62` | `73` | `CObjectVendor` | protocol-only candidate; Vendor handler additionally uses packet phase/status `33` |

This is direct architectural evidence that:

```text
native event → candidate action
```

is **not equivalent to**:

```text
native event → CPlayer gameplay state
```

The common ingress contains additional gating/interception logic between these domains.

---

## 4. Shared completion states remain protocol-level

`43/44/5A..63/66/68` share `FUN_004F56F0` (Steam `004F5620`). Their fixed phase-2 mappings are:

```text
43 → event 3D
44 → event 3E
5A → event 53
5B → event 54
5C → event 55
5D → event 56
5E → event 57
5F → event 58
60 → event 59
61 → event 5A
62 → event 5B
63 → event 4B
68 → event 5C
66 → anomalous default path
```

Current receiver-side census did **not** establish a unique class-specific `CObject* +0xA4` owner for these completion event IDs. They therefore remain a reusable object-action acknowledgement layer, not a newly named Door/Car/Vendor family.

---

## 5. State `66` hazard

Selector Event `4E` can produce candidate/state `66` behind capability `0x400`. Unlike protocol-only candidates such as `6C/71/73`, the ordinary action ingress has a structurally valid path to call the full transition hub with candidate `66`.

The shared completion handler's `66` default branch then reads both outgoing event and packet status from an uninitialized/stale local on the reviewed phase-2 path.

Current status:

```text
state 66 static stale-stack hazard: STRONGLY_SUPPORTED
selector/commit path for 66: structurally live
native Event 4E producer: UNKNOWN
runtime reachability: UNKNOWN
observable vanilla bug: NOT PROVEN
```

The highest-value next test is to identify or runtime-trace a producer of native Event `4E`.

---

## 6. Corrected / withdrawn intermediate labels

These labels must not be revived:

- `state 57 = BoxBase/Toolbox/Suitcase` — **wrong**. The observed `0x65` is a packet subcommand under native Event `0x19`, not native Event `0x65`.
- `states 58/59 = Shaft` — **wrong**. The observed `0x66/0x67` are packet payload/subcommands under native Event `0x19`; Shaft's actual native action event found here is `0x41`, selecting candidate `0x6C`.
- `state 32 = Freight` — too narrow. Native Event `0x2E` is shared by at least five object classes.
- mutable callback `0x005F5EF0` proves Event `0x2F → state37` — **wrong**. Its large switch reads the callback data/argument domain rather than the native event-ID argument.
- `0D/57 = lever family` — **withdrawn as a pair**. `0D` still has an independent `LEVER.PRM` ingress clue; that does not prove `57` is a Lever object state.

---

## 7. Remaining live-state identities

High-value unresolved candidates/states include:

```text
2F event producer is known Door-family → state 37 is NOT implied;
state 37 ← native Event 2F = CObjectPhone family (closed)
state 65 ← native Event 68 = CObjectTelescope family (closed)
41  ← native Event 38 (owner still open)
3B  ← native Event 39 (owner still open)
42  ← native Event 4D (owner still open)
51  ← native Event 5D = CBossKaysen3-side sender strongly supported
52  ← native Event 5E (owner still open)
57  ← native Event 65 = CBossKaysen2-side sender strongly supported
58  ← native Event 66 = CNpcEnemy/CNpcThrow-family sender strongly supported
59  ← internal/global code 66 companion state; not native Event66 ingress
66  ← native Event 4E (producer/open reachability)
```

For these, use producer/receiver ownership or runtime traces before assigning friendly object names.
