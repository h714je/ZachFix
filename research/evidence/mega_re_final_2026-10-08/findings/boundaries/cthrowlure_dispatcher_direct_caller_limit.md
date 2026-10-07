# CThrowLure Dispatcher Direct-Caller Limit and CScenedemoPostEffect Ownership

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the paired direct caller matrix and CScenedemoPostEffect ownership; `BOUNDED_STATIC` for CThrowLure-specific provenance within that matrix.

## Batch contract

- **Frontier family:** CThrowLure indirect lifecycle dispatch.
- **Question:** do direct caller contexts of active-object dispatcher Steam `006C5DA0` / GOG `006C58A0`, excluding the reviewed selector-0 reset walker, establish a class-discriminated route to CThrowLure vtable slot `+0x30`?
- **Source universe:** all paired direct dispatcher calls recorded in `calls.csv`/`xrefs.csv`: application-region callsites `0042617E/0042619E`, paired `005B2140/005B2210`, and paired `0066D280/0066D1D0`; reviewed walker `006C5DF0/006C58F0` was excluded. Raw assembly, paired decompilation, exact `.rdata` code-pointer locations, named vtables, and the `004277F0/00427810` owner-creation chain were inspected only as needed to classify receiver provenance.
- **Novel discriminator:** complete paired direct caller matrix plus raw vtable/pointer provenance, distinct from the previous marker-writer, factory, registration, reset-walker, and generic-retirement censuses.
- **Success condition:** a direct caller presents a CThrowLure-anchored receiver to the dispatcher.
- **Negative condition:** every non-reset direct caller remains generic, anonymous, or independently class-anchored to another owner; retain a CThrowLure-specific static bound without asserting no indirect runtime route exists.

## Paired direct caller matrix

| Source family | Steam | GOG | Receiver provenance | Result |
|---|---:|---:|---|---|
| Application-region pointer sweep | `0042617E` | `0042619E` | Entries from global pointer array `008A63E4[index]`; no CThrowLure vtable or object field anchor | Generic/untyped |
| Single owner field | `005B2140` | `005B2210` | Conditional owner `+0x58C`, cleared after dispatch; its raw code-pointer table is anonymous and distinct from CThrowLure | Generic/untyped |
| CScenedemoPostEffect cleanup | `0066D280` | `0066D1D0` | Non-null entries in `+0x350` 0x32-record array and fields `+0x2C8/+0x2CC`, each cleared after dispatch | Independently class-anchored generic owner |

The application-region apparent Steam `.rdata` pointer hit at `007750A0` is bytes for the literal `MOB`, not an object vtable. The `005B2140/005B2210` pointer locations occur in an unnamed data table immediately before the separately named CObjectFan vtable; they do not identify a CThrowLure virtual method or CThrowLure receiver.

## CScenedemoPostEffect connection

Raw paired constructors `004276C0/004276A0` first install `CScenedemoPostEffect::vftable` (`0077142C/0077141C`), initialize a 0x32-element `0x4C` record vector at object `+0x34C`, and then install `CSingleton<CScenedemoPostEffect>::vftable` (`00771434/00771424`). Paired singleton accessors `004277F0/00427810` lazily allocate `0x1224`, call those constructors, and store the root at `00BDBCC8`.

Paired cleanup `0066D280/0066D1D0` dispatches each non-null entry at object `+0x350 + 0x4C * index` through `006C5DA0/006C58A0`, then clears it; it similarly dispatches and clears object `+0x2C8` and `+0x2CC`. The direct facts establish a CScenedemoPostEffect-owned collection of generic active-object references. They do not establish any member is CThrowLure, nor that CThrowLure’s `+0x30` slot is reached through this owner.

## Bounded conclusion

`VERIFIED`: the declared non-reset direct-caller matrix is exhausted in both PC builds, and no source presents a CThrowLure-class-discriminated receiver to the active-object dispatcher.

`VERIFIED`: CScenedemoPostEffect is a paired lazy singleton with an independently evidenced `0x1224` allocation and generic active-object cleanup fields.

`UNKNOWN`: whether any CScenedemoPostEffect-held field ever contains CThrowLure at runtime; any indirect CThrowLure invocation beyond the bounded direct caller matrix; and all runtime aftermath of CThrowLure self-link traversal.

Reopen the CThrowLure source only with a direct CThrowLure storage/retainer field, a new class-discriminated receiver/virtual dispatch edge, a raw boundary that invalidates this caller matrix, or runtime trace. Repeating generic active-object dispatcher callers, marker-writer calls, factory/registration, reset, or generic retirement is not a trigger.

## Durable links

- `C0109` records the CScenedemoPostEffect generic ownership edge and CThrowLure-specific limit.
- `H0244`–`H0246` record paired constructor, singleton accessor, and cleanup homology.
- `OBJ-CSCENEDEMOPOSTEFFECT` records the class and allocation root.
