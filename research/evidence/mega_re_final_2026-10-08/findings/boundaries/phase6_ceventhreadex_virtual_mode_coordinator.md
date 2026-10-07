# CEvThreadEx virtual mode coordinator — sequence 145

**STEAM_PC only.** This report covers exactly `0043D400`, selected after the sequence-143 independent rank-7 scout found an RTTI-typed virtual seam distinct from the CEvent cache it accesses. Primary receipt: `scratch/phase6_evthreadex_personal_seq0145.json`; scout card: `scratch/phase6_seq0143_alternate_0043D400.{md,json}`. Main personally checked the complete `[0043D400,0043E68B)` body: 1,240 instructions / 4,747 bytes, exact PE/ASM agreement, with no adjacent body or callee acquisition.

## VERIFIED identity and receiver continuity

The function saves incoming `ECX` into `ESI` at `0043D404` and uses that retained receiver throughout. Its vtable is `007721DC`, observed 18 consecutive executable pointers; slot 3 is `0043D400` and slot 4 is the separately known `00447B50` Event entry. The complete-object locator/type metadata names `CEvThreadEx`; direct hierarchy descriptors identify `CEvThreadEx` at offset 0, `CEvThread` at offset 0, and `CEvFlag` at offset 4. This is a table/type relationship, not a universal interface extent or proof of every dispatch instance.

At `0043D420` the body reads cached `00BD9E58`; on null it requests `0x3B9C`, calls opaque `0042D5B0` on the allocation, writes `0076E858`, and publishes the result. RTTI for that table names `CSingleton<CEvent>`. The incoming receiver and the lazily accessed CEvent cache therefore have distinct tables and type descriptors. Repeated cache construction is not evidence that the incoming `ECX` is the CEvent object.

## VERIFIED local control and state geometry

When receiver bit `+78` bit `0x40` is set, the body calls opaque `006C5FD0` with `ECX=[00BD7670]` and `[ESI+10]`; its result gates the cache/flag path. It then tests receiver `+78` bit `0x100000`, sign-extends byte `+A0`, bounds it to `0..15`, and dispatches through the 16-entry table at `0043E68C`. The table has 12 distinct destinations due to case sharing. Three further computed dispatch sites occur at `0043DD9F`, `0043E0F1`, and `0043E4FF`, with 8, 5 and 5 recovered destinations respectively. This is local multi-mode control flow, not a global opcode dispatcher claim.

The body repeatedly tests/clears receiver `+78` bits and updates receiver `+9C`, `+A0`, `+A2/+A3`, `+A4/+A5` and `+AE`. It has 170 direct call sites to 65 distinct target addresses. It reads cached `00BD9E58` repeatedly and has selected writes/publications involving `00BDA000`, `00BDA0F8`, `0146A86C` and `013936F8`; ownership and high-level meanings remain unresolved.

## Architectural interpretation and limits

**STRONG_INFERENCE:** a `CEvThreadEx` virtual state/mode coordinator that mutates receiver-local flags/counters, performs nested mode dispatch, and consults a distinct cached CEvent object. This removes the prior alternative that the rank-7 body was merely repeated CEvent auxiliary construction.

This does **not** establish mode names, event grammar, frame/thread cadence, thread identity, construction/destruction, owner, lifetime, current generation, scheduling, success, runtime effects, GOG homology, or a new subsystem root. There are no direct incoming call rows because the entry is reached through a virtual table; absence of direct callers does not disprove central use. The first opaque outgoing semantic edge is `0043D417 -> 006C5FD0`; the first missing architecture-changing edge is an exact class-anchored construction/installation or virtual dispatch provenance for slot 3.

The accepted CEvent cache/root and earlier `00447B50` Event relay are not rewritten. Slot adjacency does not prove they share ownership or lifecycle. No expansion into the 65 callees is justified until the incoming dispatch/constructor edge is available.

## Acquisition boundaries and history

The scout's decompile read overshot the indexed end and displayed an adjacent `0043E790` prefix; that neighbor was explicitly excluded, not interpreted, counted, or used. Main acquisition remained within the exact selected body and permitted table/RTTI metadata. No raw input, binary, production code, runtime trace, GOG body or canonical file was modified by acquisition.

All Phase 5 closure, 88 inherited obligations, sequence-142 Camera mismatch, excluded neighboring ASM and no-Phase-7 controls remain authoritative.
