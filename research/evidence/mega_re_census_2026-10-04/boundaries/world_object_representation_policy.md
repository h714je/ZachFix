# World object representation / desired residency policy

**Date:** 2026-10-03. **Phase 4 sequence 45. Build:** Steam PC. **Scope:** bounded conditional static reconstruction. **Confidence:** `VERIFIED` for listed primary bytes, calls, receiver continuity and field transitions; `UNKNOWN` for incoming context semantics, descriptor/value meaning, resource identity/ownership, success, lifecycle, cadence and build/runtime limits.

## Contract and evidence

The post-seq43 global review selected `WORLD_RESOURCE_RESIDENCY_CHAIN` because it offered a new typed selector-46 same-object desired-state writer and consumer, distinct from accepted CMap state/tag and generic resource scans. Existing C0035/C0045/C0050/C0085, C0114/C0119/C0120, and accepted `006B2C70`/`006BE6E0` scopes are reused only at their proven interfaces. This report does not re-audit CMap state 7, tag requests, 75 pair identities, parsers, generic `+0x444` scans, CLevel ownership, GOG or runtime.

Main primary replay `scripts/inspect_phase4_world_representation_main.py` → `scratch/seq45_world_main_receipt.json` passed 316 instructions / 1082 bytes across sixteen aligned Steam windows. Independent consumer replay `scratch/seq45_world_consumer_branch.py` → `scratch/seq45_world_consumer_branch.json` passed 21 exact instruction checks / 16 selected calls; its detailed branch finding is `scratch/seq45_world_consumer_branch.md`. Factory branch `scratch/seq45_world_factory_branch.py` → `scratch/seq45_world_factory_branch.json` passed 12 windows / 715 PE-backed bytes / 15 call rows. Main personally read the decisive factory/descriptor/consumer ASM, replayed both branch checkers locally, and checked 316/1082 bytes against the Steam PE before promotion. The supplied Steam export spells the consumer `005C8720`; historical `005C87F0` is an interior conditional-jump label and is not transferred as a function identity.

## 1. Incoming context and typed conditional factory output

`0040D0D0` saves its incoming ECX as ESI and, under its pending-value gate, loads context-relative `+0x1003C/+10040/+10044`, pushes those values, sets `ECX=0143C078`, and calls `005F0A50`. The context’s class and the full producer meaning remain unknown.

`005F0A50` has two selected calls to `005EEC20`. At `005F0B8D..005F0B9D`, the pushed explicit values include the third control scalar `0`; at `005F0C25..005F0C42`, the corresponding selected scalar is `1`. The latter is the selected desired-mode arm. The generated C view lists the branches in a different textual order, but its call-local values agree once matched to the correct labels; there is no C/ASM argument-order contradiction. A provisional branch note claiming one was withdrawn before canonical promotion. No broader `005F0A50` algorithm is claimed.

Inside `005EEC20`, after its conditional existing-object/record paths, the selected fallback invokes opaque `0074EE20` with the exact literal pointer `0077A4A4`, whose raw bytes are `OBL\0`; only its zero/nonzero branch is used here, not a guessed string/CRT semantic. On its zero arm, `005EEE4C` pushes selector `0x46` and `005EEE51` calls `005EB600`. Selector dispatch in `005E7620` maps the raw selector-46 table entry at `005E969C` to `005E7E86`; that construction arm allocates the observed `0x458` size and calls `005479F0`, whose constructor writes the named `CObject` vtable `00773EC4` and returns the same receiver. The selector/class identity is reused from C0050 at this exact scope; no concrete subtype or lifecycle is inferred.

When the selected desired-mode scalar is nonzero and the factory returns a nonnull ESI, `005EEE6B` writes the single dword `dword [ESI+0x444]=1` (the decompiler expression `piVar7[0x111]` is the same `0x111*4` offset, not a byte `+0x111` field), then `005EEE75` calls exact Steam `005C8720` with `ECX=ESI`. Thus the object returned by selector construction is conserved through the desired-state write and consumer receiver setup. Fallback reachability, incoming context type, and successful object construction remain conditional.

## 2. Descriptor construction and representation-index value

`005EB600` builds a zeroed local `0x60` descriptor. Its selected `005EA200` call returns an untyped value which, under subtype/record gates and a signed `> -1` condition, sets local descriptor bit `0x100` and stores the result into local descriptor `+0x50`. With the function’s saved-register stack layout, the same local descriptor’s `+0x58` is the bitfield tested by the later factory, and `005EB8F7` passes the descriptor base (`LEA EDX,[ESP+8]`) to `005E7620` at `005EB8FF`.

The actual object-field copy is in `005E7620`, not directly in `005EB600`: `005E94C4` tests descriptor `+0x58` bit `0x100`; `005E94CD` reads descriptor byte `+0x50`; `005E94D0` writes that byte to returned object byte `+0x416`. The descriptor’s incoming/value provenance is therefore only conditionally traced through `005EA200`; no resource name, pair identity or valid index range is assigned. This corrects the locator-only source card’s overcompressed description while preserving its exact bounded question.

## 3. Same-object representation consumer

Exact exported Steam `005C8720` captures its incoming ECX as ESI. The initial gates are:

- `004607A0` result must be nonnegative;
- object byte `+0x416` must not equal `0xFF`;
- signed current byte `+0x12` must differ from desired dword `+0x444`.

For the selected desired-nonzero arm, the byte at `+0x416` is sign-extended as an index into paired tables at `008AA6B0` and `008AA6B4. Two separately acquired resource-manager getter results supply ECX receivers for `006B2C70` at `005C8778` and `005C8791`; their returned values are then passed to `006BE6E0` after `005C8797` restores `ECX=ESI`, preserving the same object as the handoff receiver. The manager/resource accessor and XMD/XPC setup interfaces are reused at their accepted scope; this batch does not type the individual pair resources.

The common tail later reads the low byte of desired dword `+0x444` at `005C8898` and stores it to object byte `+0x12` at `005C88B0`. This is a concrete desired-to-current field commit after the selected call sequence. It is not a successful setup result: `006BE6E0` is not treated as a boolean success return, null resource values have a return path, and no result test gates the `+0x12` write.

## 4. Architectural chain and limits

**Verified conditional chain:** untyped context `0040D0D0` → `005F0A50` selected desired-mode control → `005EEC20` fallback/selector branch → selector `0x46` `CObject` construction → same-object dword `+0x444=1` → exact `005C8720` representation consumer → descriptor-gated `+0x416` table index → paired `006B2C70` resource lookups → same-object `006BE6E0` handoff → low-byte desired `+0x444` to current `+0x12`.

The first unresolved joins are incoming context/class, `0074EE20` predicate meaning/reachability, `005EA200` result/value semantics, descriptor-to-pair index validity and individual resource identities. Ownership, last-use ordering, retirement/unload safety, final draw, CLevel equivalence, all 75 pairs, parser grammar, runtime cadence, GOG/Xbox correspondence and Phase5 are not claimed. The corrected raw details do not establish a general world residency policy; they establish one bounded producer → state/value → consumer path.

## Primary anchors

- Incoming context/calls: `inputs/decompiler/steam/DP_full.asm:14695-14721`, `:557396-557557`; selected C label correspondence `inputs/decompiler/steam/DP_decompiled.c:316882-316900`.
- Factory fallback/store: `inputs/decompiler/steam/DP_full.asm:555435-555469`; selector46 construction/descriptor copy `:547859-547868`, `:549151-549154`; local descriptor `:551393-551401`, `:551429-551464`, `:551511-551529`.
- Consumer: `inputs/decompiler/steam/DP_full.asm:512165-512212`, `:512285-512295`; accessor and same-object pair stores `:769590-769625`, `:785957-785963`.
- Raw map/case/type checks and aligned window VAs are preserved in main/branch receipts; computed xrefs/decompiled C are not independent raw-byte proof.
