# Direct Active-Object Registration Producer Census

**Date:** 2026-10-01  
**Scope:** all direct calls to Steam `006C5AE0` / GOG `006C55E0` in supplied call manifests.  
**Evidence state:** `VERIFIED` for direct-call taxonomy and outer-handle assignment; object names/ownership remain separately bounded.

## Census result

- **45** direct call sites total.
- **41** build-specific caller functions: 22 Steam, 19 GOG.
- **2** sites are the already-established paired selector factory; excluded from net-new producer counts.
- **43** non-factory sites remain: 24 Steam and 19 GOG.

Every direct call executes active-object registration, which passes the object to CRdHandleUtil allocation and writes the returned packed handle at object `+0x3C`. Direct registration alone does not establish a concrete class name.

## Complete taxonomy

| Taxonomy | Steam caller(s) | GOG caller(s) | Sites | Evidence limit / result |
|---|---|---|---:|---|
| Known selector-factory producer | `005E7620` | `005E76F0` | 2 | Already established; excluded from net-new work. |
| Known player producer | `005FB8C0` | `005FB9C0` | 2 | CPlayer construction and retained player-array ownership already established. |
| Known fishing producer | `0060EED0` | `0060EFE0` | 2 | CFishing constructor; pre-existing class identity retained. |
| Fixed-vtable producer | `00463400`, `006373F0`, `00674B20` | `00463430`, `00637340`, `00674A70` | 6 | Fresh allocated outer objects receive fixed unnamed vtables and callback setup; no reliable class name yet. |
| Callback-owned producer | `004DEE00`, `00528340` | `004DEED0`, `00528410` | 4 | Fresh outer object is initialized from caller state and receives a task callback; precise class identity unresolved. |
| Caller-retained producer | `005B9430`, `005BA540` | `005B9500`, `005BA610` | 6 | Fresh outer objects are returned/stored through caller-owned output pointers; `005BA540/005BA610` also registers a nested caller-held object. |
| Renderer-boundary producer | `005EC6C0`, `005ED660` | `005EC790` | 3 | Existing D3D9 classification preserved; direct registration gives an active-object handle but no safe class identity. |
| Fishing auxiliary producer | `0060EE40` | — | 1 | Steam-only direct site; GOG structure folds the corresponding setup into adjacent CFishing code. Exact auxiliary class remains unresolved. |
| Selector-result re-registration anomaly | `00627240` | `006271C0` | 2 | Selector `0x44` constructs CThrowLure and the caller directly registers the returned result again. Recorded as `K0007`. |
| Global-retained producer | `006374D0` | `00637420` | 2 | Newly created outer object is stored at `01472254`; name/type unresolved. |
| Batch producer | `00527170`, `00665CC0` | `00665C10` | 5 | Creates multiple outer objects in loops or batches; Steam `00527170` has three direct sites and no matched GOG direct boundary. |
| Pre-existing object registration wrapper | `00677960`, `00677CF0`, `00678600`, `00678BA0` | `006778B0`, `00677C40`, `00678550`, `00678AF0` | 8 | Registers objects already obtained/constructed elsewhere. `00677CF0/00677C40` retain their prior XWP loader classification. |
| Native-task factory forwarder | `006C5930` | `006C5430` | 2 | Forwards created native tasks into registration; prior NATIVE_UI classification retained. |

## Construction-to-registration-to-release bridges

- **Selector factories:** construction -> registration -> outer `+0x3C` handle is verified. Generic manager retirement (`006C7070/006C6B70`) unlinks the active object, calls virtual slot `+0x08`, and releases the handle through CRdHandleUtil.
- **Non-factory producers:** each row above has direct registration and therefore the same handle-storage rule. Their individual destruction owners and scheduling causes are mostly unresolved; the generic retirement path remains the currently verified release bridge.
- **Pre-existing-object wrappers:** registration proves a new active-object handle assignment, but the originating construction/lifetime is outside the wrapper and intentionally not inferred.

## CThrowLure contradiction

`00627240/006271C0` call selector `0x44` then directly call registration on the returned CThrowLure. Since the general factory tail independently registers successful selector results, this appears to be a double-registration path. `K0007` records the issue; no assumption is made about whether control-flow conditions, handle replacement, or list deduplication reconcile it.

## Generalized rule and limits

The selector factory is not the sole producer family. Direct non-factory callers show active-object registration is a cross-subsystem service used by renderer-boundary, fishing, callback, batch, global-retained, and native-task flows. At registration, the result is always an **outer object** from the caller’s perspective; this census provides no counterexample to the Phase 2 embedded-helper rule.

The census is complete for supplied direct call edges. Indirect function-pointer registration and runtime-only paths remain outside this bounded conclusion.
