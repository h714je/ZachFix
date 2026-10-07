# CMessage Constructor and Table-Base Source Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired CMessage initialization; `BOUNDED_STATIC` for the inspected table-installation source universe.

## Batch contract

- **Frontier family:** CMessage record-table provenance.
- **Question:** does a class-discriminated CMessage constructor, vtable entry, or direct singleton-local writer install the record table at `+0x44`?
- **Source universe:** paired raw CMessage constructor bodies Steam `0040A270` and GOG `0040A240`; their named CMessage vtable anchors `0076F674/0076F664`; and every raw-assembly function that both accesses exact global `00BDA000` and writes an object-register `+0x44` field. A candidate counted only when local register provenance derived the write base from `00BDA000`; stack slots, other `00BDA0xx` globals, dispatcher-body rereads, and broad xref enumeration were excluded.
- **Novel discriminator:** raw instruction-level CMessage class identity plus register-local object provenance, rather than the previously exhausted dispatcher/global-reference sources.
- **Success condition:** a nonzero CMessage `+0x44` store or loader edge with class-discriminated receiver provenance.
- **Negative condition:** constructor clears the field and no exact singleton-derived local store survives provenance, bounding this source universe without claiming there is no later indirect producer.

## Paired primary evidence

Steam `0040A270` writes the named CMessage vtable `0076F674`, then zeroes `this+0x34`, `this+0x38`, `this+0x44`, and `this+0x5C`; the relevant store is `0040A2A1: MOV dword ptr [EAX + 0x44], ECX` immediately after `XOR ECX, ECX`. GOG `0040A240` is structurally paired: it writes `0076F664` and executes `0040A271: MOV dword ptr [EAX + 0x44], ECX` after the same zeroing setup.

The paired raw source therefore directly identifies these routines as CMessage initialization and proves their `+0x44` effect is a null initialization, not record-table installation. It also corrects the decompiler’s array-index rendering, which did not expose the `+0x44` store as a named CMessage field.

The bounded raw-assembly census found no nonconstructor object-register `+0x44` store whose live base register locally derives from exact `00BDA000` in either PC build. Same-offset writes that merely occur in a function which also touches `00BDA000` were rejected because CMessage identity was not established by the declared local register-provenance test. Lack of singleton-local provenance is not itself proof that a receiver is a different class. The curator's existing dispatcher-call sample also distinguishes the resolver's ECX/EBX receiver from the singleton result in EAX; see `cmessage_high_centrality_dispatcher.md`. Neither that receiver alias nor the later loader is resolved by this bounded census.

## Bounded conclusion

`VERIFIED`: paired `0040A270/0040A240` are CMessage initializers; they install the paired CMessage vtable values and clear CMessage `+0x44`.

`VERIFIED`: the inspected exact-singleton local data-flow source contains no nonzero table-base installation store.

`UNKNOWN`: the later loader/producer that makes the resolver’s table access valid, the table format, caller-family taxonomy, code semantics, and runtime timing.

The only valid reopen trigger for this bounded source is a direct nonconstructor CMessage receiver path, a loader/constructor whose object identity is independently anchored to CMessage, a raw-assembly boundary that invalidates the local provenance limit, or reproducible runtime evidence. Repeating generic `00BDA000` references, dispatcher bodies, or untyped `+0x44` scans is not a trigger.

## Durable links

- `C0108` records the paired constructor and source limit.
- `H0243` records the paired constructor homology.
- `OBJ-CMESSAGE` and `CMessage::vftable` records retain the object anchors.
- The remaining CMessage table-provenance queue item is `BOUNDED_STATIC` under the reopen condition above.
