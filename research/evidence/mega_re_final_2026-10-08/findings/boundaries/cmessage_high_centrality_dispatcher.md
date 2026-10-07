# CMessage High-Centrality Dispatcher

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired CMessage code-dispatch and record-resolution mechanics; broader `NATIVE_UI` ownership remains `STRONG_INFERENCE`.

## Paired primary evidence

Steam `0045C9F0` begins at `inputs/decompiler/steam/DP_decompiled.c:72222`; GOG `0045CA20` begins at `inputs/decompiler/gog/DP_decompiled.c:64953`. The paired signatures take the same code parameter in `param_6`, contain matching early code-specific comparisons, require the same context fields at `this+0x34/+0x38/+0x3C`, and lazily allocate/write `CSingleton<CMessage>::vftable` through global `00BDA000`.

Both paired bodies then pass `param_6` to record resolvers Steam `0045A590` / GOG `0045A5C0`. Those resolvers directly index the table rooted at their receiver's `+0x44`, cache the selected entry at `+0x48`, read its leading category, and return category values or the explicit `-1/-2/-3` sentinels. See Steam `DP_decompiled.c:70580-70617` and GOG `DP_decompiled.c:63325-63362`.

**Curator scope clarification, 2026-10-02:** raw Steam `DP_full.asm:106948-106985` and GOG `DP_full.asm:99322-99359` corroborate the lazy root and resolver calls, but the resolver callsites use `MOV ECX,EBX`, whereas the lazy singleton result is in EAX. Resolver field access is independently corroborated at Steam `DP_full.asm:104412-104443` and GOG `DP_full.asm:96794-96825`. Lazy creation alone does not prove that the resolver receiver aliases singleton `00BDA000`; retain that receiver/alias distinction as UNKNOWN unless independently class-anchored. 'CMessage dispatcher' here names the verified CMessage-linked code path, not a proof that every incoming receiver is the singleton.

## Bounded conclusion

The pair is a high-centrality CMessage code dispatcher with an internal record-resolution boundary. The direct facts are the CMessage root, parameterized code dispatch, `+0x44` table access, and paired resolver mechanics. The prior `message_presentation_helper` wording remains a valid broader structural grouping because the bodies also reach presentation/resource helper families, but it does not replace the narrower verified dispatcher fact.

## Explicit limits and next discriminator

- Paired CMessage construction is now separately bounded: raw `0040A270/0040A240` clears `+0x44`, and exact singleton-register provenance finds no nonconstructor direct table-base writer. See `cmessage_constructor_table_base_limit.md` and `C0108`.
- The producer/loader that later installs the CMessage `+0x44` table remains unknown.
- Individual message-code semantics, caller-family taxonomy, final UI/render ownership, and runtime cadence/order remain unknown.
- Reopen only with an independently CMessage-anchored nonconstructor receiver/loader path, a raw-assembly boundary that invalidates the provenance limit, or runtime evidence; do not repeat the dispatcher body, generic global references, or untyped `+0x44` scans.

The former eight-line report is superseded in scope by this evidence-expanded version; no raw evidence was changed.
