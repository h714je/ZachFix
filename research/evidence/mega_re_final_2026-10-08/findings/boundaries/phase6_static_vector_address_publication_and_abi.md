# Static-vector address publication and constructor ABI — sequence152

STEAM_PC only. Main sources: scratch/phase6_vector_consumer_personal_replay_seq0151.json (175/599), scratch/phase6_vector_abi_personal_replay_seq0151.json (22/77), scratch/phase6_entry_retained_personal_replay_seq0151.json. Four sequence151 scout results qualified independently; all completed, no errors. Supplied evidence is unchanged.

## VERIFIED three explicit address-publication loops

In own00446890, three distinct loops reacquire00408960 on every iteration, then store a source address value into a DWORD cell of that immediate EAX result. Let E_i be the particular returned value for that iteration; it is not silently joined to another iteration's result.

| Source literal and increment | Destination relative offsets | Nominal loop count |
|---|---|---|
| 00BDBEE0, +54 | E_i+10EC+4*i through1168 | 32 |
| 00BDC960, +C4 | E_i+116C+4*i through12A8 | 80 |
| 00BE06A0, +10 | E_i+AEC+4*i through10E8 | 384 |

The second loop's literal/store/steps/limit are00446913/0044691D/00446920/00446923/00446929. This **publishes address bits**, without reading, resetting or invoking a source element. Counts and endpoints are static nominal geometry conditioned on normal calls and preserved nonvolatile loop registers, not observed completion, valid storage, stable E_i or initialized contents. The source one-past arithmetic addresses are00BDC960,00BE06A0,00BE1EA0 respectively. Neighboring arithmetic continuity does not prove a shared record format, allocation or owner.

Own0044695B also writes code-address00434320 to a separately reacquired getter-result+24, without invoking it there. Other own setup operations retain opaque/resource-view results in the original receiver and configure known generic callback infrastructure. These do not identify the original receiver as CEvCore or CFunc. Own0076FEE8 writes target allocation-derived auxiliary receivers before00BDA0CC publication; runtime aliasing remains UNKNOWN.

Exact source:175 defined instructions/599 bytes in602-byte envelope. Unlisted0044693D..00446940 is skipped by a listed jump and remains uninterpreted. Exact Ghidra FunctionBody address-set remains unavailable. No caller00454C60, callee, GOG, neighboring or generic event algorithm body opened. Normal EAX1 is not setup/load success; own allocation-failure backedges also preclude a universal completion guarantee.

## VERIFIED iterator machine slots; SI intended ABI join

Own0074E836 has22 defined instructions/77 PE-matched bytes, exactly [0074E836,0074E883). It initializes a local counter, compares it signed against[EBP+10], loadsESI from[EBP+8], putsESI inECX and transfers through[EBP+14]. It supplies no explicit callback stack word between that load andCALL. After normal return it adds[EBP+C] to the **returned** ESI and retains that value. The fifth frame slot[EBP+18] is not read in this extent. Support/SEH helper effects and the out-of-extent0074E883 path are not acquired.

The known five-word static request and named/decorated CRT interface support an **intended** mapping of initial address, stride, count and callback to these frame slots: `(00BDC960,C4,50,0042D3B0,0042BA30)`. This ABI join is SI; unseen SEH frame support and runtime execution are not promoted. In particular, the callback's cleanup/destructor argument is not proved by an ordinary-path read. A callback-preserving/normal-return condition is explicit, not assumed for arbitrary targets.

Together with accepted C0262/C0263 available71-export+15raw-tail initializer/table/write extent and C0264 static request, the exact00BDC960/C4/80 publication matches the intended CEvThreadEx vector. **SI:** CEvCore-associated address-table configuration exposes intended CEvThreadEx element addresses. CEvCore naming reuses existing00408960 inference; this batch does not turn its recovered name into primary type proof.

## Architecture and missing edges

This is a genuine explicit shared-address publication mechanism, not merely formatting or transporting constructor pointers. It extends the selected static-vector architecture without establishing a new root, manager, service or scheduler. The three cohorts' element types and record formats are independent except for the independently supported intended CEvThreadEx cohort.

UNKNOWN first edges: published cell -> consumer -> live element/current interface; exact typed getter result/root and current generation; entry receiver/caller/lifecycle invocation; actual virtual mode/reset route; format semantics of the prior stack tuple passed to0070D3E0; synchronization, ownership, success, lastuser and safe retirement. No published-address-to-mode-coordinator join is proved by vtable membership alone.

Next bounded discriminators: own115-byte00408960 acquisition/type source (existing recovered inference, new primary discriminator); own128-byte0070D3E0 descriptor consumer; own27-byte0070D8A0 follow-up, no support-body chain. Published-cell consumers require a separately selected actual source, not a generic offset scan or automatic grammar descent.

## Rejected scout inflation

The named58 scout's DISPROVEN label applies to a hypothetical unconditional single-operation interpretation, not the actual historical release label. Main does **not** promote a historical contradiction: two-stage request composition may still have higher-level release semantics, which remain UNKNOWN. The cold retained source is complete at the finite cap, but old workflow completion and semantic conclusions remain uncredited. No class assignment from its auxiliary CMessage table write or83-member collapse is accepted.
