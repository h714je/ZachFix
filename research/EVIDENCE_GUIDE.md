# Evidence guide | How much does this claim prove?

[← Research atlas](README.md) · [Methodology](methodology.md) · [Correction ledger](disproven.md)

This page is a reading contract for humans and assistants. **The most useful research note is not the most confident-sounding one. It is the one that tells you precisely where its confidence ends.**

## Evidence hierarchy

```text
For the exact build being discussed:
  machine code / raw executable data
    > decompile / recompilation output
    > agreement between Steam and GOG PC builds
    > exact, evidence-backed cross-version homolog
    > cross-checked research notes
    > indirect platform/asset clues
    > inference
```

A Steam-only proof is not automatically a GOG proof. An Xbox semantic analogue is not automatically a 1:1 PC owner or hook. Prefer exact mechanism language over attractive class names.

## Claim labels

| Status | Safe interpretation | Typical next step |
| :-- | :-- | :-- |
| `VERIFIED` | The recorded mechanic, bytes, call, field or flow were established **within their written limits** | Review scope and application to the target build |
| `STRONG_INFERENCE` | Concrete evidence supports the interpretation, but a decisive join is missing | Seek a discriminating caller, writer, owner or consumer |
| `UNKNOWN` | The available evidence does not settle the question | Keep multiple candidates open |
| `DISPROVEN` | One particular historical interpretation was contradicted | Follow the replacement explanation; do not resurrect the stronger claim |
| Runtime observation | A measured behavior was seen in particular conditions | Reproduce, instrument and test regressions separately |

A copied pointer does not prove ownership. A queued request does not prove completion. A matching numeric state does not prove the same semantic domain. An active call chain does not prove frame-by-frame cadence.

## Good claim anatomy

A useful entry should make the following easy to answer:

1. **Claim:** What exact relation or behavior is asserted?
2. **Build:** Steam PC, GOG PC, original Xbox or a specific pairing?
3. **Anchor:** Address, bytes, branch, offset, ledger row or explicit runtime trace?
4. **Confidence:** Verified at what *scope*? What is inferred?
5. **Negative space:** What is expressly **not** established?
6. **Dependency:** Which source finding or correction governs this conclusion?
7. **Next discriminating test:** What evidence would change the answer?

## From source to ZachFix code

```text
primary evidence
    -> scoped interpretation
    -> cross-build checks / corrections
    -> runtime hypotheses
    -> instrumented reproduction or A/B
    -> constrained fail-closed implementation
    -> regression tests
    -> production documentation
```

Never skip from **static architecture** directly to “safe to patch” because an address looks plausible.

## Which copy is authoritative?

- **Primary imported finding** and the latest relevant correction determine the factual scope.
- [MegaRE final synthesis](engine/mega-re-final.md) is the quickest *interpretation*, not a substitute for primary evidence.
- [MegaRE imported evidence](evidence/mega_re_final_2026-10-08/README.md) is intentionally retained without cosmetic rewriting.
- [Earlier Phase-4 snapshot](evidence/mega_re_census_2026-10-04/README.md) is historical and may contain limits or open claims since revised.
- [Disproven ideas](disproven.md) is a first-class research asset; a negative conclusion must remain visible.

See [research methodology](methodology.md) for receiver tracing, lifecycles, homology and boundary-first work.
