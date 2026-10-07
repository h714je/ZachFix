# Reverse-engineering methodology

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](README.md) · [Topics](INDEX.md) · [Open questions](unresolved.md)

> **Reading note:** Durable RE workflow rules; the 2026-10-04 snapshot remains useful even where the census has since advanced.

<details><summary><strong>On this page</strong> · 7 sections</summary>

- [Evidence identity](#evidence-identity)
- [Object and receiver reconstruction](#object-and-receiver-reconstruction)
- [Boundaries before leaf helpers](#boundaries-before-leaf-helpers)
- [Cross-build discipline](#cross-build-discipline)
- [Scalable census strategy](#scalable-census-strategy)
- [Production promotion rule](#production-promotion-rule)
- [MegaRE final methodological rules (2026-10-08)](#megare-final-methodological-rules-2026-10-08)

</details>
<!-- END AUTO RESEARCH NAV -->

**Research snapshot:** 2026-10-04.

This page records the durable working rules extracted from the Mega RE Census. They are
method rules, not semantic conclusions about unexamined code.

## Evidence identity

Keep these identities separate in every claim:

```text
build
address
original auto-name
canonical semantic name
cross-build homology group
```

A Steam address is not a GOG address, and a convincing semantic name does not create a
cross-build homology by itself. When a decompiler boundary affects control flow, confirm
it with assembly or raw bytes, especially around split functions, omitted blocks, and
`noreturn` interpretations.

Old reports are search hints. A claim becomes current only when the active executable
evidence supports it. When an old interpretation is disproven, preserve it as a negative
constraint in `disproven.md` instead of silently deleting history.

## Object and receiver reconstruction

Do not infer object identity from a nearby constructor, destructor, RTTI string, resource
name, or caller label. Track the actual receiver through ECX/register flow, stack locals,
returned pointers, vtable writes, and callback installation.

For resource branches, reconstruct a tuple rather than naming from an extension:

```text
extension/name discriminator
    -> helper / lookup
    -> allocation size
    -> vtable / RTTI
    -> parser or format marker
    -> callback / ownership handoff
    -> first discriminating consumer
```

Archive records, managers, child records, utility objects, and retained resource views
must remain separate candidate types until a concrete relationship joins them.

For manager-like objects, search construction arguments, fixed-stride storage, callback
fields, synchronization objects, registration writers, cleanup, and global/root
references as one lifecycle cluster. For child records, begin with allocation and field
initialization, then follow all helpers receiving the child base before assigning names
such as frame, tile, mip, layer, or slot.

## Boundaries before leaf helpers

Prioritize subsystem crossings because they carry more architecture per unit of work:

```text
resource -> object
object -> PhysX
animation -> actor
world -> resource
UI -> game state
save staging -> disk I/O
```

If a decompile is truncated, restore the raw continuation and inspect callers/callees
before interpreting the apparent endpoint. Centrality is useful for target selection,
but fan-out alone does not name a function.

## Cross-build discipline

Accept Steam/GOG homology only after comparing useful invariants such as normalized call
neighborhoods, constants, field offsets, table relationships, and vtable structure.
Whenever practical, a narrow branch slice across both PC builds is safer than a broad
single-build pass because it exposes false homology and decompiler artifacts early.

A Steam-only Phase 4 mechanism can still be useful for ZachFix research, but it stays
explicitly Steam-only until the second build is checked.

## Scalable census strategy

Classify obvious CRT/STL, imports, Win32, D3D9, PhysX, audio, and middleware code at a
coarse level once the evidence is sufficient. Spend detailed effort on game-owned roots,
state transitions, lifecycle boundaries, resource ownership, and repeated subsystem
signals.

Global read/write clusters are good discovery tools, but a shared address does not prove
ownership. Verify the root initializer and the concrete receiver before assigning a
semantic owner.

Each research batch should leave a durable receipt containing:

```text
source universe and build
exact addresses / files inspected
new claims and confidence limits
negative results / bounded scans
validation result
one exact successor question
```

If the same question is revisited repeatedly without a new discriminator, move it to a
blocked/unresolved state and record exactly what evidence would reopen it. This prevents
expensive circular archaeology.

## Production promotion rule

A static mechanism is not automatically a ZachFix patch. Production work additionally
needs the relevant call-site scope, supported builds, runtime cadence/reachability,
failure behavior, ownership/lifetime, and a fail-closed strategy. Conditional static
chains are valuable architecture even when those runtime properties remain unknown.

## MegaRE final methodological rules (2026-10-08)

The final foundational repair and adversarial phases add several durable rules for future ZachFix RE:

1. **Recognized functions are not the callable universe.** Check raw fallthrough, callbacks, initializer tables, code pointers and interior/direct targets before making negative claims.
2. **Diagnostic/error calls are not automatically `noreturn`.** The GOG dispatcher repair proved that a wrong termination assumption can hide substantial valid code.
3. **Function hulls are not ownership.** Do not treat `min(address)..max(address)` as a real body across disjoint fragments.
4. **Address range and thunk form are not ownership.** CRT-looking placement or forwarding shape does not prove non-game code.
5. **Published/admitted/completed/retired are different states.** A flag, row sentinel, request return, clear or reused slot does not prove lower-layer completion.
6. **Current is epoch-qualified.** Input, packet, shadow, resource, audio and save “current” values can come from different generations.
7. **Locks are not transaction identity.** A protected shared pending cell can still lack request/completion correlation.
8. **Failure is not rollback.** Some resource paths mutate/clear prior state or publish size before validation succeeds.
9. **Deep helper safety can live upstream.** A low-level table/index/resolver path may be correct only for admitted inputs.
10. **Numeric identity is generation-sensitive.** Reused tokens/handles/indices are not episode identity without generation/currentness evidence.
11. **Fixed-capacity pools are first-class long-session suspects.** High-water/exhaustion diagnostics can matter even with a stable heap.
12. **Cross-build homology is semantic, not arithmetic.** Never generalize one Steam/GOG delta or equal numeric address.

The imported [`RESEARCH_HEURISTICS.md`](evidence/mega_re_final_2026-10-08/maps/RESEARCH_HEURISTICS.md), [`ARCHITECTURAL_PATTERNS.md`](evidence/mega_re_final_2026-10-08/maps/ARCHITECTURAL_PATTERNS.md) and [`ENGINE_RECOGNITION_PATTERNS.md`](evidence/mega_re_final_2026-10-08/maps/ENGINE_RECOGNITION_PATTERNS.md) are the canonical expanded pattern registers.

The ZachFix-side heuristic synthesis derived from MegaRE is preserved as [`ZACHFIX_HEURISTIC_ANALYSIS_2026-10-07.md`](evidence/mega_re_final_2026-10-08/reports/ZACHFIX_HEURISTIC_ANALYSIS_2026-10-07.md); its bug candidates remain candidates until runtime validation.
