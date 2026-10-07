# Save/Event core-member value transport — not reference retirement

**Phase5 ACTIVE, Steam sequence124.** Selected `P5_SAVE_EVENT_CORE_REFERENCE_OPERATION`; no Phase6. Scope: complete `00430A10..00430AD3` and conditional complete `0072A470..0072A4DC` only. Actual contiguous staging before PE checking/display: caller **46 instructions/195 encoded bytes**, leaf **41/108**; distinct total **2 identities/2 windows/87 instructions/303 encoded bytes**, within persisted128/384 (main80/256, branch48/128). Raw-data/RTTI acquisition0. Personal replay adds no coverage.

## Receiver and operand prerequisite — VERIFIED

`00430A12` saves incoming ECX as **I**. Actual selected upstream `004524F2` receiver/argument→I is **UNKNOWN**; retained preparation report supplies only its already-proven scope and static route. I is not pre-named CEvent or equated to Game/core.

`00430A14` calls opaque `0040A320`; its EAX numeric result **G0** is saved as EDI. `00430A1B` calls cited opaque `00408960`; its EAX numeric result is **E0**. `00430A20/26` forms and pushes **P=G0+CDD80**; `00430A27` supplies **R=E0+7B4** as ECX to exact PE-decoded `00430A2D→0072A470`. This concrete in-body provenance passes review122's local prerequisite without assuming incoming I, getter return types, or shared generation.

**STRONG_INFERENCE naming only:** 00408960/00BD9FE8 is the retained CEvCore-associated seed (`xwp_dsb_branch_slice.md:26`; `typed_resource_dispatch.md:48`), not a newly proven current root/type/owned stream. 0040A320's Game association comes from retained Save/Game evidence. G/E denote getter-result occurrences, not proven stable live objects or generations. Getter/constructor bodies and raw root/type data were not reopened. No GOG mapping is established for either new entry.

## Selected leaf value mechanics — VERIFIED

The leaf saves receiver bits in its stack local `[EBP-4]`; the argument is `[EBP+8]`. Exact load/store records are in the personal and branch receipts; the following compact mapping assumes intact ABI frame/operand cells and the required memory validity, not a new alias/type proof:

- `0072A47F/482`, `A48A/48D`: current DWORDs **R+8/C→P/P+4**.
- `0072A49C..A4BB`: six interleaved DWORD load/stores **R+10/14/18/1C/20/24→P+8/C/10/14/18/1C**, in that order.
- `0072A4BE..A4D2`: separately reloads saved R/P; initializes **ESI=R+A0=E0+854**, **EDI=P+20=G0+CDDA0**, **ECX=14h**; `F3 A5` performs twenty repeated DWORD moves on normal completion.

Eight explicit plus twenty repeated stores are **112 byte-write operations**, not112 encoded code bytes, an atomic snapshot, or unconditional112-distinct-byte footprint. No CLD/STD occurs. **VERIFIED conditional DF=0:** source `[E0+7BC,E0+7DC)`→destination `[G0+CDD80,G0+CDDA0)` then `[E0+854,E0+8A4)`→`[G0+CDDA0,G0+CDDF0)`. **VERIFIED conditional DF=1:** REP byte-offset ranges `[R+54,R+A4)`→`[P-2C,P+24)`; pre-restoration ESI=R+50/EDI=P-30. Incoming DF/segment bases/valid extents/aliasing remain **UNKNOWN**.

Reads and writes are interleaved. Destination/source or stack-cell aliasing is not excluded; no pre-call source snapshot, receiver preservation, coherent field set, or non-overlap promise is established. Copied DWORD bits are not proven pointers, handles, borrowed references, ownership-bearing references or counts. The code does not follow a fetched payload pointer or explicitly adjust a reference counter.

The complete leaf has **no call, branch, explicit literal field clear, or pointee cleanup/deallocation boundary**. Payload overwrites replace destination bits, not proven object replacement/retirement. EAX retains the bits observed from R+24 atA4B8, ECX becomes0 after normal REP; neither is a constructed success result. EDI/ESI/EBP are restored and RET4 removes the local frame/argument space. Stack reclamation is not pointee cleanup.

## Continuing caller order — VERIFIED at numeric scope

After ABI-compatible leaf return, independent **E1** from00408960 is sampled at+4 mask2000. Nonzero arm reacquires **G1** and writes DWORD[G1+CDD7C]=1. No selected leaf result is tested for success; E0=E1/G0=G1 or same-generation coherence remains **UNKNOWN**.

Caller argument1==1 and WORD[I+3012]==01F5 jointly gate separately acquired **G2** +8C5EC bit1000 OR and x87 scalar transfers I+3000/3004→independent **G3/G4** +CFD80/CFD84. No full Save/Event algorithm or field meaning is inferred.

Next, **G5+CFE1C,0,100h** are passed to opaque0074E760; normal return cleans0C stack bytes. Availability/name analogy does not prove the callee effect; its body is excluded. Later independent **E2/G6** supply ESI=E2+428, EDI=G6+CFE1C, ECX=40h to REP MOVSD. **Conditional DF=0 and valid intact ABI memory:**256-byte inline transfer `[E2+428,E2+528)`→`[G6+CFE1C,G6+CFF1C)`. No E0=E2 or G0=G6/current-generation/staging coherence is established. Original EDI/ESI are restored and caller RET4 follows. Neither complete selected body invokes a proved pointee-retirement boundary.

## Relationship distinctions and first missing edge

| Join | Evidence state / precise limit |
|---|---|
| Local numerical receiver/argument transformation | **VERIFIED:** E0→R=E0+7B4; G0→P=G0+CDD80 at selected call |
| Current getter-result or reference identity | **UNKNOWN:** opaque results are separate occurrences; same numeric address would not prove generation |
| Actual pointee/core object or current interface type | **UNKNOWN:** CEvCore/Game seeds guide naming only; no new type/root proof |
| Retained/shared/borrowed reference transport | **UNKNOWN:** observed bit copies do not establish pointer semantics or a lifetime obligation |
| Ownership | **UNKNOWN:** no retention→ownership inference |
| Load/copy/order | **VERIFIED:** local interleaved transfers and separate later getter/copy order, under explicit ABI/memory conditions |
| Operation/save success | **UNKNOWN:** reaching RET or residual EAX/field writes is not success |
| Cleanup/deallocation | **UNKNOWN outside source; none observed locally:** destination overwrite/stack teardown is not free; no available-cleanup→invoked-cleanup inference |
| Owner/pointee generation, last borrower/use, safe retirement | **UNKNOWN:** no continuity, admission closure, final-use or quiescence proof |

**First genuinely unproved stronger edge:** interpreting the exact untyped addresses/bit transfers as a current typed state/resource/reference/lifetime operation, including any copied pointer's obligations. Stop here; no getter/upstream caller/helper/constructor/sibling/registry/handle/allocator/old resource consumer/HookChain/GOG/runtime rescue. Selected stronger retirement frontier is NOT_READY, not all-corpus exhaustion. No canonical DISPROVEN claim is warranted merely because the candidate name included “reference.”

## Proof and preservation

- Entry `scratch/phase5_save_event_entry_seq0124.json`:119 handoff hashes exact, four structural checks EXIT0, selectedREADY/lastADVANCE/0-0-0; no121semantic re-audit. Original57 history and recent97 files/prefixes separately prechecked.
- Caller `scratch/phase5_save_event_caller_stage_seq0124.json`, `scratch/phase5_save_event_caller_personal_seq0124.json`; local prerequisite `scratch/phase5_save_event_transport_gate_seq0124.json`.
- Fresh single-assignment Opus branch `scratch/phase5_save_event_leaf_stage_seq0124.json`, `scratch/phase5_save_event_leaf_branch_seq0124.json`/.md. Branch acquired only the selected leaf.
- Main `scratch/phase5_save_event_leaf_personal_seq0124.json`, `scratch/phase5_save_event_comparison_seq0124.json`: exact41 ordered records, spans/counts, direct-target list, relative source keys/hashes, build and schema agree before promotion. Main directly checked all87 distinct instruction bytes/ASM and target transport; branch authority alone was not promoted.
- PE identity remains Steam7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029. `DP_full.asm` SHA9ea0d1b89a891c8cf66f0482db71cc9abda65c64459f9c3ab35f66df9827b060; leaf lines921788–921828.
- Entry Read rejects, oversized queue request, wrong ledger-column lookup and guessed phase-plan path were corrected without acquiring evidence from failed calls, preserved in setup history. Prospective comparison adopted the branch's documented list schema before first run; no comparison failure/receipt rewrite. Preservation baseline's original-history pointer was corrected before first check, with preflight note; no failed check hidden.
- All70+18, original failed receipts/corrections/NOTPASS/disconnect histories retained. No supplied evidence, binaries, assets, production code or runtime changes. Postbatch global comparison is required; repeated generation/last-use endpoints also warrant an honest Phase5 readiness-signal review, not invented deeper static work or Phase5 acceptance.

## Actual batch validation and preservation category correction

Actual124 first and Attempt2 ledger/state/heuristic/progress each EXIT0. Conservation PASS14739files/2682497921bytes/frozen8/70+18. First separate preservationcheck FAILED a mislabeled UNKNOWN_QUEUE append-only category; original baseline/failed console and exact reconstructed-original/forward-configured queue diff preserved. Corrected-category preservationPASS13updates/5proofs/7unaffectedcanonical/97recent/57oldhistory; no semantic/canonical/validator/checker correction or research/globalreview during repair. scratch/phase5_save_event_preservation_scope_correction_seq0124.json

Canonical projection: C0235/C0236 VERIFIED numerical mechanisms; BND-231/232 STRONG_INFERENCE static subsystem-name associations, not currentreceiver type/ownership/generation. 8809-byte preaddendum report retained in scratch/phase5_save_event_report_before_preservation_addendum_seq0124.md (SHA256bf66ca4c391fb3bdf81530a9109313e1dbcfb12f014808842171a1689c0c8ef8).
