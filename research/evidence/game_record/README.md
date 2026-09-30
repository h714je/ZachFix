# GameRecord / dp.sav evidence

**Research snapshot:** 2026-09-30

The canonical byte-level persistence map is:

- [`ZachFix_GameRecord_RE_2026-09-30.md`](ZachFix_GameRecord_RE_2026-09-30.md)

It contains the complete `0x7A2620` save-image layout, the `0x45CC0` GameRecord offset
map, owner traces, Steam/GOG cross-checks, save-corpus evidence, CEvent history
semantics, specialized world registries, vehicle availability namespaces, and the
remaining fine-grained open names.

Use [`../../save/README.md`](../../save/README.md) for the compact engine architecture
view. Treat the full evidence document as the authoritative source when a field-level
or serializer claim needs justification.

Focused save/resume control-flow evidence is summarized in [`../save_resume_contract/README.md`](../save_resume_contract/README.md).
