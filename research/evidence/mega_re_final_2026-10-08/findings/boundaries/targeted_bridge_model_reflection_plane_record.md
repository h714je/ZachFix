# TBC005 — animation matrix to packet plane metadata to reflection

**Result: CONNECTED. VERIFIED selected Steam field/math/consumer relation; animated-matrix-defined reflection plane is STRONG_INFERENCE.** This is distinct from PGC006's conditional shader sample and does not explain the reported offset bug.

## Missing relation and primary path

Known model evaluation writes M1E8 matrices; known reflection code consumes packet P1AC plane operands. A14 already byte-covered the packet population arm but did not establish this field genealogy. The new decisive evidence is the selected matrix/offset math and cross-receipt consumer join, not whole-model coverage.

`PLANE_PRIMARY.json` and `BRIDGE_QUALIFIERS_PRIMARY.json` under `audit/targeted_bridges_2026-10-07/` establish:

- P1AC defaults0 at `006BD7AD`; selected populated arm at `006BDE8D` publishes the current packet-interior cursor. Its **0x1C-byte** record has normal XYZ at0/4/8, offset XYZ atC/10/14, WORD18 and byte1A. It is not a newly named separately allocated type or universal packet schema.
- Signed WORD M2F4 selects `M1E8 + i×0x40` only on its nonnegative arm. Translation XYZ is read from matrix30/34/38. Negative selector retains the default construction.
- `004AAE80` copies0x40 bytes and zeros translation30/34/38. `00448570` transforms XYZ with computed W/division and a near-zero-W alternative. This is **not** a proved orthonormal rotation, inverse-transpose normal transform or normalization.
- The selected offset is **M(E0/E4/E8)−matrix(30/34/38)** through four-component subtraction; only XYZ is copied into the record. A length-shaped native comparison against double0.009999999776482582 can replace the computed normal with a default. Independent mask800 can overwrite both normal/offset using global01481334; do not silently exclude it.
- Existing reflection consumer `006D3B08..006D3BB1` forms point XYZ as **P(position0/4/8)−metadata(C/10/14)** and takes normal from metadata0/4/8. It retains the selected P at scene+6440.

## Qualification

For the selected-index path with the independent override excluded, the relation supports an animated selected-node plane rather than a necessarily object-origin plane. Algebraic cancellation is not an exact native-floating equality or contemporaneous pose proof.

UNKNOWN: admitted matrix/index/record extent, current P/M/resource/camera generations, latest pose after packet bypass, normal validity, physical surface/material meaning, actual shader/target/GPU result, owner/lifetime/final borrower and bug causality. GOG's existing population locator is not new plane parity evidence. Source selectors/node-friendly names remain outside this bridge. Prior A03/A14 bypass/invalidation/currentness and PGC006 guards are preserved.
