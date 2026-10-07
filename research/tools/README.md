# Navigation maintenance tools

[← Research atlas](../README.md) · [Style guide](../STYLE_GUIDE.md)

The scripts use **only Python's standard library** and intentionally leave the primary MegaRE findings, maps, reports, ledgers, and checkpoints untouched.

## Update after adding or editing research pages

From the repository root:

```bash
python research/tools/build_indexes.py
python research/tools/build_indexes.py --check
python research/tools/check_links.py --anchors
```

`build_indexes.py` rebuilds the reader-facing topic index, evidence indexes, and the collapsible section navigation on curated pages. Existing auto-generated navigation is safely replaced rather than stacked. To add a new curated page, extend `AREAS` (or `GUIDES`) in the script and give it a one-line human-readable description. The original technical body stays in place.

`check_links.py` checks internal links on the reader-facing layer and verifies in-page Markdown anchors when `--anchors` is selected. The imported MegaRE sources were intentionally copied without the giant underlying workspace, so the checker **does not treat original internal references in those primary documents as self-contained links**.

Do not edit imported primary documents to satisfy cosmetics or link lints. Add a wrapper index or consult the original workspace instead.
