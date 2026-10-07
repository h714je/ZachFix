# TBC007 — XAM-compatible retained-block production to known cleanup

**Result: CONNECTED; VERIFIED selected numeric producer/replacement/accounting relation.** This supplies A11's missing allocation origin, not full XAM parsing, successful decode, coherent resources or owning lifetime.

## Same-state and source qualification

`XAM_PRIMARY.json` captures entry S and then S14 into two caller locals. The selected call at `006B84E3` reloads those locals and offers them to `006B92D0`. Reproducible `XAM_LOCAL_SCREEN_V2.json` checks all1054 owned instruction rows against original ASM/PE and indexes89 exact references: only `006B8089/006B8098` directly write the two normalized locals, with no direct LEA offer. This supports the bounded direct-local transport, **not** absence of indirect/overlapping aliases, exceptional/callee mutation, unchanged S14/resource bytes or current-generation continuity.

New raw getter `004D4710` returns S14. `006B9E30(1)` obtains its offset at resource+30, then reloads S14 and adds the offset (zero offset→NULL). The explicit producer argument and this separately reacquired source are not certified coherent.

## Producer to retained fields

`006B92D0` tests offered resource WORD+C bit2000:

- Clear: lazily calls `006B9450` only when S4 is null, storing its return at S4 and passing &S8 as size output.
- Set: unsigned-divides the offered second argument by resource DWORD4C. Nonnull S4 with matching S+C can be reused; otherwise a length-linked record walk precedes subtraction of old S8 from01481328, old-pointer free request/clear, new allocation/decode-shaped request, S4 publication and S+C quotient store. Accounting adds S8 only for nonnull S4.
- `006B9450` requests record DWORD+C bytes; null allocation writes output size0. Nonnull writes the **requested amount** before three decode/inflate-shaped calls whose statuses are not checked. Return is the allocated pointer; the caller ultimately returns S4, not an attempted-refresh flag or decode success.

Known setter `006B9020` conditionally subtracts/free-requests/clears these same fields on a **nonnull changed resource argument**; NULL or same-resource calls do not supply unconditional cleanup.

## Primary and remaining qualification

`XAM_PRIMARY.json`, `RESOURCE_QUALIFIERS_PRIMARY.json`, `XAM_LOCAL_SCREEN_V2.json` under `audit/targeted_bridges_2026-10-07/`; protected A11 setter/Species qualifications reused. Independent PE/ASM/direct-local review concurs.

UNKNOWN: divisor/record/input/output extent admission, full grammar, allocation/decode completion, initialized output extent, current source/block generations, aliasing/reentrancy, thread-safe accounting, exclusive ownership and safe last borrower. No observed fault or all-instance replacement/lifetime theorem. Steam only.
