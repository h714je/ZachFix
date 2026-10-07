# Research status | What is actually established?

[← Research atlas](README.md) · [MegaRE synthesis](engine/mega-re-final.md) · [Open questions](unresolved.md)

> [!NOTE]
> **As of the imported 2026-10-08 snapshot.** This is a guide to the preserved research, not a claim that the complete engine has been recovered or that the game has been tested at every entry point.

## At a glance

| Track | State in supplied material | Authority |
| :-- | :-- | :-- |
| Phase 7 scientific closeout | checkpoint **238** | [Final synthesis](engine/mega-re-final.md) |
| Phase 8 synthesis | complete through checkpoint **243** | [Phase 8 index](evidence/mega_re_final_2026-10-08/maps/PHASE8_FLOW_INDEX.md) |
| Post-Phase 8 gap closure | checkpoints **244–245** | [Gap closure index](evidence/mega_re_final_2026-10-08/maps/POST_PHASE8_GAP_CLOSURE_INDEX.md) |
| Targeted bridge campaign | checkpoints **246–250** | [Bridge index](evidence/mega_re_final_2026-10-08/maps/POST_PHASE8_TARGETED_BRIDGE_INDEX.md) |
| Final broad static campaign | checkpoints **251–260** | [Final strategic review](evidence/mega_re_final_2026-10-08/reports/FINAL_STATIC_STRATEGIC_REVIEW_SEQ0260.md) |
| Latest imported control | `PHASE_8_COMPLETE` / `READY_FOR_AUTONOMOUS_STATIC_RESEARCH` | [Evidence import](evidence/mega_re_final_2026-10-08/README.md) |
| Runtime observations added by those static campaigns | **0** | [MegaRE synthesis](engine/mega-re-final.md) |

The control label does **not** mean every phase's every question is answered, that all functions are classified, or that subsequent experiments can be skipped.

## Accounting without false percentages

| Measure | Value | Denominator / interpretation |
| :-- | --: | :-- |
| Historical function rows | 20,252 | Original/export-recognized population |
| Historical classified | 5,180 | Subset of 20,252 |
| Historical named | 3,481 | Subset of 20,252 |
| Historical `UNKNOWN` | 15,072 | Subset of 20,252 |
| Corrected structural entry records | 21,871 | **Different** repaired population |
| New-entry candidates | 1,619 | Relative to original recognized entries |

**Never calculate a whole-engine semantic-coverage percentage from these mixed sets.** Exact basis and structural corrections: [architecture synthesis](engine/mega-re-final.md) and [structural repair](evidence/mega_re_final_2026-10-08/reports/STRUCTURAL_FOUNDATION_REPAIR_2026-10-06.md).

## Engineering status is a separate question

| Area | Current research/engineering boundary |
| :-- | :-- |
| **Input / SDL3** | PC input path and logical-action contract are substantially mapped. Full provider replacement is still an engineering/runtime-validation task. [Input](input/README.md) |
| **Native UI** | A development-only native-task architecture and callback contract are mapped; this is **not** proof of a shipped stable settings menu. [Native UI](ui/README.md) |
| **PhysX** | Strong PC timing findings exist, but production physics-timing modifications remain retired pending precise characterization. [PhysX](physx/README.md) |
| **World distances / NPC / shadows** | Multiple independent distance domains; do not equate known six frustum values with every NPC, lighting or shadow distance. [World](world/README.md) |
| **Save / resume** | Byte-level mapping and several adapters are known; arbitrary save/load in transitional states is not generally certified safe. [Save](save/README.md) |
| **Renderer / audio** | Some regressions have confirmed boundaries; other producer/consumer or runtime paths remain unresolved. [Renderer](render/README.md) · [Audio](engine/audio.md) |

## Before calling a finding “ready”

Check the exact build and provenance, the bounded evidence, the latest correction, the live entry-point and ownership conditions, the rollback/fail-closed path, and the runtime test or A/B comparison. More detail: [evidence guide](EVIDENCE_GUIDE.md).
