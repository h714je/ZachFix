# CEvThreadEx raw available reset path — sequence147 acquisition

**STEAM_PC only.** The explicit sequence147 contract permitted only the gap `[0042D3F7,0042D410)`, ending before the next declared function, and required stopping at the first RET. It was selected to discriminate the prefix/decompile extent conflict, not to read any excluded Camera neighbor or adjacent initializer body.

## VERIFIED raw-only continuation

The supplied ASM has no defined instruction at0042D3F7. A conservative raw decoder first stopped on unsupported operand-width/opcode forms; those partialattempts are retained. No source cap was expanded and no padding/nextfunction was interpreted. No decoder package was installed.

Exact PE encodings personally checked:

-0042D3F7: `66 89 90 b0 00 00 00` — WORD[EAX+B0]←DX; retainedprefix establishesDX=0.
-0042D3FE: `c6 80 98 00 00 00 01` — BYTE[EAX+98]←1.
-0042D405: `c3` — ordinaryRET.

This is3instructions/15bytes, ending0042D406. Reads stop at this RET; no bytes0042D406..0042D410 were acquired/interpreted. Exactreceipt: `scratch/phase6_evthreadex_exact_raw_tail_seq0147.json`; preservedpartialattempts are listed there.

## Available path and export reconciliation

Together with the primary14instruction/71byteprefix in `scratch/phase6_cold_and_initializer_personal_seq0147.json`, there is a verified available linear17instruction/86byte path from0042D3B0 throughRET: incomingECX copiedtoEAX; installedCEvThreadEx table007721DC; exactzero/resetstores; currentEAX returned unchanged. No localallocation, basecall or supportcall appears in this available path.

The newly verified operations agree with the ownindexed decompile but have exact WORD/BYTE widths rather than invented DWORD normalization. Highest observed receiverwrite remainsC0..C3, lowerboundC4; notallocation orsizeof.

The supplied functions.csv71bytes/end0042D3F6 and ASM omission are preserved, not repaired or replaced. ExactGhidraFunctionBody membership is still unavailable. The canonical name/classification describes this available entry-to-RET reset sequence with an explicit raw-only extension, not universal exportcompleteness or dynamicexecution.

## Architectural limit

**STRONG_INFERENCE:** CEvThreadEx-associated available initialization/reset interface. **VERIFIED:** literal tableinstallation andlocalstate/reset/returngeometry. **UNKNOWN:** invocation of constructorpointer, incomingproducer, owner, runtimegeneration, instantiatedarray/root, virtualdispatch, lastuser, scheduling/cadence orretirement. The separate pending0076AC65 source assignment must prove its own transport; this tail does not prove it.

All88 inheritedguards, Phase5acceptance, sourcefailures and noPhase7 remain. No supplied evidence, binary, asset, production code orruntime state changed.
