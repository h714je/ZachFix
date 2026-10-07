# CScenedemoPostEffect Singleton Lifecycle Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired allocation, virtual deleting destructor mechanics, and exact direct-root write scope; `BOUNDED_STATIC` for direct singleton teardown provenance.

## Batch contract

- **Frontier family:** CScenedemoPostEffect ownership and lifecycle.
- **Question:** can the paired singleton accessor, constructor, vtable, direct root writers, and deletion contexts establish CScenedemoPostEffect ownership and a static free path?
- **Source universe:** paired singleton accessors `004277F0/00427810`, constructors `004276C0/004276A0`, raw vtable bytes at `0077142C/00771434` and `0077141C/00771424`, paired deleting bodies `00427680/004276E0`, and all direct `00BDBCC8` write xrefs plus direct destructor xrefs. Generic active-object field consumers and broad singleton readers were excluded.
- **Novel discriminator:** named-class vtable contents plus exact root-writer and direct deletion-call provenance, rather than the prior generic active-object cleanup family.
- **Success condition:** a paired direct owner/free chain from root through deletion or a precise static bound on its missing edge.
- **Negative condition:** if the only root writes are construction/failure publication and deleting destructors have no direct static caller, retain the direct teardown route as bounded without claiming runtime persistence.

## Paired primary evidence

The paired CScenedemoPostEffect vtable and singleton-vtable entries both resolve to the vector deleting paths Steam `00427680` / GOG `004276E0`. Each writes the base CScenedemoPostEffect vtable, invokes the 0x32-element `0x4C` vector destructor at `this+0x34C`, and calls the allocator delete routine only when the deleting flag is set.

The exact singleton root `00BDBCC8` has two writes per build, all within `004277F0/00427810`: publish the allocated/constructed pointer or publish null on allocation failure. No direct writer clears the root after construction. The direct xref set contains no direct call to `00427680/004276E0`; their statically evidenced reachability is through the paired virtual table entry.

## Bounded conclusion

`VERIFIED`: CScenedemoPostEffect has a paired virtual deleting destructor implementation that tears down its 0x32 record vector and conditionally frees `this`.

`VERIFIED`: the declared singleton-root writer universe contains only allocation/failure publication, not a direct teardown/root-clear edge.

`UNKNOWN`: what owner invokes the virtual deleting destructor, whether the singleton root is reset elsewhere through an indirect/write-unresolved path, whether a live singleton is deleted, and runtime lifetime/cadence.

Reopen only with a direct caller of the CScenedemoPostEffect deleting path, an independently class-anchored root-clear/free writer, raw evidence that expands the exact write/call universe, or runtime trace. Repeating constructor/accessor readers, generic active-object fields, or vtable byte scans is not a trigger.

## Durable links

- `C0110` records the verified destructor mechanics and direct teardown limit.
- `H0247` records the paired deleting destructor homology.
- `OBJ-CSCENEDEMOPOSTEFFECT` and its four vtable records now retain the free-path limit.
