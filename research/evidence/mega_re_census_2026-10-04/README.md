# Mega RE Census integration snapshot - 2026-10-04

This directory preserves the subset of the current Mega RE Census that was used to
update ZachFix's durable `research/` tree on 2026-10-04.

## Source

Source archive: `Новая папка (2)(2).zip`.

The source Census was at:

```text
phase:      PHASE_4_ACTIVE
checkpoint: checkpoints/2026-10-04_seq0055_phase4_post-car-model-global-review.md
```

The exact source-archive SHA-256 is stored in `source/source_zip.sha256`. The copied
files are listed in `source/manifest.txt`.

## Evidence rules

The copied Phase 4 reports are evidence snapshots, not a declaration that Phase 4 is
complete. In particular:

- most of the newly integrated Phase 4 mechanisms are Steam-PC-only static results;
- they must not be silently transferred to GOG until a homology or independent GOG
  check is established;
- a conditional producer-to-consumer chain does not prove runtime cadence, reachability,
  success, ownership, or safe destruction unless the source finding says so;
- exact numeric state, field, event, and selector domains stay separate even when they
  happen to use the same number;
- Census `VERIFIED` means verified at the scope written in the finding, not whole-system
  semantic closure.

## Included source slices

`source/` contains the overall status, Phase 4 mechanism frontier, and research
heuristics. `subsystems/` contains the relevant subsystem summaries. `boundaries/`
contains the concrete mechanism reports that were promoted into ZachFix's architecture
pages.

The integrated, ZachFix-facing synthesis is
[`../../engine/phase4-mechanisms.md`](../../engine/phase4-mechanisms.md). The reusable
methodology distilled from the Census is
[`../../methodology.md`](../../methodology.md).
