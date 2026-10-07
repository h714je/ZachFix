# CItem factory payload, numeric field and resource-slot binding

**2026-10-04; Phase4 sequence50. Steam only. C0171–C0172 / BND-154–157.** VERIFIED selected conditional mechanics; UNKNOWN value invariance, valid name content, concrete resource types, success and lifetime. This is an independent actor/resource mechanism, **not** a promoted Player action or Player-owned weapon chain.

## Selection, scope and reuse

Sequence49 compared five independent Player candidates and rejected every strict Player discriminator. The global reassessment selected this separately class-proven CItem source: `reports/PHASE4_POST_PLAYER_ACQUISITION_FRONTIER_REASSESSMENT_2026-10-03.md`. Contract: `scratch/phase4_seq50_citem_contract.md`. Two fresh Opus branches each answered one question, without children or writes; main independently checked decisive bytes, tables and stack joins before promotion.

Accepted CItem selector/type evidence is not rediscovered as new architecture. H0042 remains its prior paired constructor scope; its legacy-reference caveat is not used as foundational proof here, because the Steam constructor/RTTI is freshly checked. C0125 supplies only the typed CGame getter; established CRdData and callback/model interfaces supply their exact prior scopes. No GOG correspondence, Player alias, parser, physics, full animation or generic callback/state/item census.

## Numeric input and actual creator payload — VERIFIED

`004FD920` ignores incoming ECX, calls the accepted CGame getter and returns dword `[G+98E48+4*index]`. The selected `004DFF2F` caller supplies index0. This is a numeric input source, not proof of a friendly equipped-item meaning, valid range, Player identity or unchanged CGame generation. The creator `004DEDA0` also accepts other callers; no all-caller claim is made.

At creator entry ESP=E, define arguments A1..A4 and local **P=E−C**:

- P[0]=**A2**, P[4]=A3, P[8]=0.
- The two stores written as `[ESP+8]` address different words after pushes; do not collapse them.
- `006C5FB0` consumes A1 only, returning separate K. The already-pushed P/A4/−1 survive.
- `005EA8D0` receives `(0x14,0,0,0,K,P,A4,−1)`.

The wrapper builds descriptor D and stores **P at D+4C**, not +58: the literal `[ESP+58]` store at005EA9B2 follows two argument pushes. D+58 is the flags word. Factory invocation is synchronous; P remains on the creator stack through initial dispatch, but this does not prove retained payload lifetime after return.

## Same CItem construction and event0 value store — VERIFIED conditional

Selector **0x14=decimal20** indexes005E95D4→005E78A7, allocates observed **4B8**, calls005F2DE0 and preserves its returned pointer O. Constructor writes0077AFEC; raw COL00878638/TD008C1BF8 names `.?AVCItem@@` with zero COL offset. This is CItem, not CWeapon or CObjectItem.

Callback row008BF700 is `{004DEE70,000F}`; 0F metadata is not A2. Factory extracts D+4C contents P, registers O and invokes006BAB80 with O/callback/P. Setter writes O+44 and calls00402870 `(O,0,P)`; the native arm reloads O+44 and supplies O as callback argument1. The optional global hook0148132C executes first when nonnull.

Raw event0 mapping selects004DEEAD. Callback preserves O in EBP, recovers P from argument3 and writes **O+4A8=P[0]** at004DEEC3. Positive continuity is constructor/allocation → the same original pointer → installed callback → numeric-field store. It does not depend on later Player/global-handle re-resolution.

**Condition / first diversion:** allocation and intermediate calls must return normally with valid O/P; optional hook and0045FC70 must not invalidate/change relevant payload, callback or object state. Their full contracts are not established. No unconditional live delivery or allocation success is claimed.

## Event0/22 field consumption and name-buffer transport — VERIFIED

Event0 continues to004DEF43; event **0x22** enters there directly and does not perform a new payload/4A8 store. A zero field branches **into** binding004DEF88, not out of it. A nonzero/equality arm may invoke opaque00528090 before convergence.

Define **G_B** as the actual later CGame getter return and **B=[G_B+835000]**. Separate field loads **N_M** and **N_P** produce 32-bit row addresses `B+DC*N`. They are not automatically equal to each other or the earlier stored N0:006C2880/00528090 and formatting intervene. This is a field-mediated mechanism, not an immutable-value assertion.

With S=callback-entry ESP−26C, the call-boundary tuples are:

| Call | Ordered arguments |
|---|---|
| 004DEFAF→0074FDC5 | `(S+248, "%s.XMD", B+DC*N_M)` |
| 004DEFD0→0074FDC5 | `(S+258, "%s.XPC", B+DC*N_P)` |

The row address itself is passed as string argument, not a dereferenced row-pointer member. **VERIFIED:** literals, row arithmetic, buffer addresses and call interface. **STRONG_INFERENCE:** intended resource-name formatting. **UNKNOWN:** actual formatted contents, row population/schema/count/range/NUL termination and string/buffer safety. ANALYSIS-sourced `_sprintf` naming alone is not proof; the small raw interface forwards destination/format/varargs to opaque00756540. Formatter returns are untested; buffer starts are16 bytes apart, without a bounded-size argument. No observed bug or safe formatting claim.

## Typed manager return values to same-object slots — VERIFIED

Keep independently reacquired typed CRdData receivers **D_A** and **D_B** distinct. Formatting order and lookup order differ:

1. 004DEFF6 uses D_A with `(S+258,1)` → **R_P**, the XPC-named-buffer lookup.
2. 004DF008 uses D_B with `(S+248,1)` → **R_M**, the XMD-named-buffer lookup.
3. 004DF010 restores ECX=the same CItem O and calls006BE6E0 **`(R_M,R_P,0,0)`**.

006B2C30 calls006B2AD0(name,1,0), which forwards to006AFAD0 then opaque006B2B20. Its actual returned k, not an assumed original resolver index, is tested for−1;006B2C70 applies signed upper-bound/−1/record-word2C tests and returns record+1C. This is not a general nonnegative-index or load-success contract. Accepted accessor scope is reused without turning suffixes into pointee classes.

Setup saves O at006BE705. After opaque pre-store calls, it reloads that same pointer and writes **R_M→O+160**, **R_P→O+164** at006BE72B/737. These stores precede the null gates, so null results can still be published. The callback does not test lookup results before setup; handoff/return is not successful loading, valid animation or final rendering. CRdMesh/CRdPicture identity in an accepted CLevel route cannot be transferred to these CItem results.

## First missing edges and durable consequence

- Strict Player/current-root ownership and native Event44 producer/target remain the sequence49 missing questions; this mechanism does not resolve them.
- Stored N0 versus later N_M/N_P and hook/helper behavior remain conditional; no immutable ID or last-writer claim.
- From consumed4A8, first semantic gap is **B→valid named DC-stride row**; population/range/content and actual output names are unknown.
- Resolver/control/helper-return k, concrete R_M/R_P type/content/load state, root-null safety, successful setup, reuse/retirement/free and cadence remain unproved.

The positive contribution is an actual typed CItem initialization/field mechanism plus precisely ordered CGame row-address/format-buffer/typed-manager-return/same-object slot transport. New residual `CITEM_RESOURCE_VALUE_IDENTITY_AND_REBIND_POLICY` preserves those missing edges. No whole actor/Player/resource subsystem completion or Phase5.

## Reproduction / primary references

- Main numeric input: `python3 scripts/inspect_phase4_citem_input_main.py` —34instructions/114bytes/4alignedwindows; `scratch/phase4_seq50_citem_input_main.json`.
- Main mechanism: `python3 scripts/inspect_phase4_citem_mechanism_main.py` —506/1625/16alignedwindows,65opcode and23relative-call assertions, raw maps/RTTI/literals and independent ESP arithmetic. Receipt `scratch/phase4_seq50_citem_main.json`.
- Fresh producer branch299/993; consumer353/1150, with the formatter-entry checker’s initial non-thunk assumption rejected and corrected before final evidence. Drafts `scratch/phase4_seq50_citem_producer_branch.md` / `scratch/phase4_seq50_citem_consumer_branch.md`. Counts overlap and are not additive or full-function semantics.
- `inputs/decompiler/steam/DP_full.asm:258532-258558`,`:550350-550419`,`:547517-547526`,`:560002-560011`,`:549064-549198`,`:780947-780963`,`:1854-1879`: payload/class/dispatch.
- Same source`:258599-258699`,`:290980-290984`,`:259563-259579`,`:769418-769447`,`:769561-769620`,`:785934-785972`,`:786132-786137`,`:951060-951111`: field/arguments/accessors/slots/formatter interface. Receipt exact per-instruction lines govern the selected windows.
- PE `inputs/binaries/steam/DP_STEAM.exe`, unchanged SHA2567a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029. All supplied inputs and production source remain untouched.
