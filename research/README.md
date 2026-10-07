# ZachFix | Research atlas

**Reverse-engineering notes for Deadly Premonition: The Director's Cut**  
Architecture, evidence, corrected interpretations, and the questions still worth investigating.

[**Visual engine map**](ARCHITECTURE_MAP.md) · [**Quick reading paths**](READING_PATHS.md)

> [!IMPORTANT]
> **Research is not a shipped feature.** This directory documents what has been demonstrated, inferred, contradicted, or left unresolved. A static call edge never proves runtime safety, reachability, timing, or cross-build compatibility.

| Current research snapshot | Scope | How to read it |
| :-- | :-- | :-- |
| **MegaRE, 2026-10-08** · sequence **260** | Phase 8 synthesis plus later static investigations | [Current status](STATUS.md) |
| **Steam / GOG / Xbox** | Evidence is build-specific unless explicitly paired | [Evidence rules](EVIDENCE_GUIDE.md) |
| **ZachFix 0.2.x / 0.3.x** | Fix history and future engineering questions, **not** an implementation specification | [Reading paths](READING_PATHS.md) |

## Start here

| I want to… | Open | Why |
| :-- | :-- | :-- |
| **Understand the engine in 10 minutes** | [Visual map](ARCHITECTURE_MAP.md) · [Engine overview](engine/overview.md) | Scheduler, objects, Player, UI, world, timing, resources. |
| **See what the final MegaRE established** | [MegaRE synthesis](engine/mega-re-final.md) | A readable technical survey, with strict confidence boundaries. |
| **Find evidence behind a claim** | [Evidence library](evidence/INDEX.md) | Browse primary notes, maps, findings, reports and ledgers. |
| **Explore a particular ZachFix problem** | [Topic-based reading paths](READING_PATHS.md) | Follow the shortest chain from question to primary evidence. |
| **See what remains unknown** | [Open questions](unresolved.md) | Research targets, not automatically implementation tasks. |
| **Avoid already rejected conclusions** | [Corrections and disproven ideas](disproven.md) | Negative constraints that protect future work. |

## Explore by subsystem

| Engine & runtime | Gameplay & control | Presentation & persistence |
| :-- | :-- | :-- |
| [Architecture](engine/overview.md) | [Input and gamepads](input/README.md) | [Rendering](render/README.md) |
| [Frame time](engine/timing.md) | [Player and actions](player/README.md) | [World, frusta and LOD](world/README.md) |
| [PhysX](physx/README.md) | [Camera modes](input/camera-modes.md) | [Native UI](ui/README.md) |
| [Resource loading](engine/resource-loading.md) | [Vehicles](player/vehicle.md) | [Saves / GameRecord](save/README.md) |
| [Audio](engine/audio.md) | [Xbox controls](input/xbox-controls.md) | [Effects / XWP](engine/effects.md) |
| [Animation](engine/animation.md) | [CCT bridge](player/cct-bridge.md) | [CRdDebug](engine/crddebug.md) |

[**All readable research pages →**](INDEX.md) · [**Terms and notation →**](GLOSSARY.md)

## How the archive fits together

```text
research/
  README.md             <- this front page
  STATUS.md             <- which conclusions are current
  INDEX.md              <- browse the readable research pages
  READING_PATHS.md      <- start from a concrete technical question
  EVIDENCE_GUIDE.md     <- confidence, builds, proof and promotion rules
  engine/, input/, ...  <- interpreted subsystem summaries
  unresolved.md         <- unanswered questions
  disproven.md          <- interpretations explicitly rejected or corrected
  evidence/             <- underlying reports and raw source slices
    INDEX.md            <- evidence collection guide
    mega_re_final_2026-10-08/
       ...               <- imported knowledge layer: preserve provenance
```

The **readable layer** helps you navigate and integrate findings. The **evidence layer** keeps the original claim wording, scope, and provenance. When they disagree, the original primary finding and its latest applicable correction control the interpretation.

## Evidence in one glance

| Label | Read as | **Do not** read as |
| :-- | :-- | :-- |
| `VERIFIED` | Demonstrated at the exact stated build, address and mechanism scope | Every downstream effect or runtime behavior is proven |
| `STRONG_INFERENCE` | Well-supported, but not directly closed | Equivalent to a verified identity |
| `UNKNOWN` | A link/identity/behavior remains open | An invitation to fill a gap from names |
| `DISPROVEN` | Specific former claim contradicted | Every nearby or weaker claim is false |
| `RUNTIME-TESTED` | Observed under the documented test conditions | Universal compatibility or a safe patch |

See [evidence and publication rules](EVIDENCE_GUIDE.md) before promoting research into code.

## About the final MegaRE import

The 2026-10-08 integration supersedes the older 2026-10-04 **Phase-4-only snapshot as the current static reference**, but preserves the earlier snapshot as history. The supplied state reaches sequence **260**; later campaigns extend the static maps without adding runtime observations.

Two different counting universes must stay separate: the **historical** ledger has 20,252 function rows (5,180 classified; 3,481 named; 15,072 `UNKNOWN`), while the repaired structural universe contains **21,871 entry records**. Neither denominator measures how much of the complete engine is understood.

The imported knowledge layer contains **281 primary finding pages** (plus five family README files), **44 map files**, **11 semantic ledgers**, and checkpoints **238–260**, plus reports and retained supporting materials. Start at the [MegaRE evidence index](evidence/mega_re_final_2026-10-08/INDEX.md), not by opening hundreds of files at random.

---

**Maintaining this atlas:** [Research writing conventions](STYLE_GUIDE.md) · [Link/index checks](tools/README.md) · [Methodology](methodology.md)
