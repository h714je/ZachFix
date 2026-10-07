# Research style guide

[← Research atlas](README.md) · [Evidence guide](EVIDENCE_GUIDE.md) · [Maintenance tools](tools/README.md)

This directory has **two layers**: readable subsystem syntheses and primary historical evidence. **Do not automatically reformat imported evidence.** Its original scope, anchors, findings and corrective history matter more than typographic consistency.

## Preferred structure for a readable research page

```markdown
# Clear name of the mechanism or subsystem

[← Research atlas](../README.md) · [Topics](../INDEX.md)

> **Reader's note:** One sentence about what this page covers.

**Jump to:** [Mechanism](#mechanism) · [Evidence](#evidence) · [Open questions](#open-questions)

## Mechanism
...
## Evidence
...
## Open questions
...
```

Write for two readers: a developer looking for a bounded hook and a future research assistant tracing proof. Optimize the first screen for **scope and navigation**; preserve addresses, offsets and exact citations in the body.

## Conventions

- **One claim, one scope.** Explicitly distinguish Steam/GOG/Xbox and static/runtime sources.
- **Short headings and meaningful links.** Use links with descriptive titles, never orphan paths if the target is available in this tree.
- **Tables for comparisons.** Tables are ideal for build pairs, status decisions, field layouts and topic routing; prose is better for nuanced control flow.
- **Code blocks for exact artifacts.** Preserve assembler, offsets, patch bytes and execution diagrams as monospaced text.
- **Readable emphasis.** Bold a critical guard/limit once; do not bold entire technical paragraphs or invent colourful confidence labels.
- **Never erase negative evidence.** Point to [disproven.md](disproven.md) and to canonical MegaRE correction reports when needed.
- **Keep original evidence immutable.** Add indexes and synthesis instead of renaming or editing the 2026-10-08 import's primary findings.
- **Avoid semantic promotion while editing layout.** Cosmetic revision is not a revalidation campaign.

## Updating the archive

1. Update source evidence and scope **only** after a real investigation.
2. Update the readable subsystem page and the relevant [open question](unresolved.md).
3. Extend [reading paths](READING_PATHS.md) only if there's a useful route for a recurring question.
4. Regenerate indexes and navigation using [tools](tools/README.md), then check links and source preservation.
5. Record any superseded claim in [disproven](disproven.md) instead of silently removing it.
