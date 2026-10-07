# CThrowLure Double Registration and Handle Invariant

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for both registration calls and their static state effects.

## Resolution

`K0007` is resolved statically. Both PC builds execute two active-object registrations on the same CThrowLure pointer in one straight-line successful path:

| Step | Steam | GOG |
|---|---:|---:|
| factory call | `005E7620(0x44, 0)` | `005E76F0(0x44, 0)` |
| CThrowLure constructor | `005F4E00` | `005F4F00` |
| factory registration tail | `005E9526` | `005E95F6` |
| second registration | `0062772B` | `006276AB` |

The selector `0x44` case sets factory table index `0x24`. The table’s registration-category byte at `008BF786` is zero in both binaries. The subsequent caller passes registration category zero directly. With null factory descriptor input, both calls also use the same registration mode. There is no branch between the factory return and the second registration: the caller writes CThrowLure fields `+0x2C = 0x18` and `+0x30 = 0x44`, then invokes registration.

## No bypass or deduplication

- CThrowLure factory allocation succeeds on the concrete path because the caller immediately dereferences the returned pointer.
- The CThrowLure vtable `+0x04` slot called by registration is `006BA7C0/006BA710`; it initializes fields around `+0xF0` and does not inspect, clear, or release the handle at `+0x3C`.
- `006C5AE0/006C55E0` does not inspect object `+0x3C` or membership state before linking the object, allocating a handle, and incrementing the manager count.
- `CRdHandleUtil::allocate` likewise checks only its next free table slot and generation flag, not whether the object already appears in a slot.

## Static state transformation

Let the factory registration return `H1 = (G1 << 16) | S1`.

1. First registration stores object `O` in table slot `S1`, marks generation `G1`, and stores `H1` at `O + 0x3C`.
2. Second registration inserts `O` into the same manager bucket while it is already head. Its list links at `O + 0x50/+0x54` become self-referential.
3. Second allocation chooses a fresh slot/generation pair `H2 = (G2 << 16) | S2`; immediate cursor advancement guarantees `G2 != G1` for the consecutive allocation.
4. Second registration stores `O` at `S2` and overwrites `O + 0x3C` with `H2`.

`H1` still names a table slot containing `O`, but the generation-validated lookup rejects it because lookup compares handle generation to the current value at `O + 0x3C`, which is now `H2`.

## Release side

Generic active-object retirement invokes CRdHandleUtil release after object cleanup. Release reads only the object’s current `+0x3C` handle, so it clears `S2/G2` and cannot directly clear the overwritten `S1/G1` mapping. No intervening release appears between the two registrations.

This proves a non-idempotent registration invariant and a stale-first-mapping condition in the static path. It does **not** establish runtime frequency, whether a later unreviewed manager reset clears the stale mapping, or all behavior resulting from the self-linked list.

## Model correction

The selector-factory outer-handle rule remains valid: factory success does register CThrowLure. CThrowLure is now the verified exception where caller code deliberately or accidentally invokes registration again. Any producer taxonomy must treat active-object registration as non-idempotent unless a caller has direct evidence of prior removal/reset.
