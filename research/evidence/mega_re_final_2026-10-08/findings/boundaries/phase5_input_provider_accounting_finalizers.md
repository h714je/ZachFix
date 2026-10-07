# Input provider arithmetic and separately registered current-root finalizers

**2026-10-04; Phase5 batch11; STEAM_PC.** C0205–C0207. New provider scope, not Input sampling, mode9, callback/thread/member, or registry census. C0196–C0198 and C0201/C0202 retain their accepted scopes.

## Source contract and outcome

Personal exact transport/RTTI/code-literal/helper rechecks plus corrected raw StopWatch`004035E0`, available Actuator`0070BA50`, named holder deleting/member interfaces and one directly invoked thin backing adapter. The first missing generation/accounting/admission/last-user edge terminates expansion. Independent branch's six-slice limit and rejected preselection remain history; main's separately declared interface continuation is not a silent seventh branch slice.

**VERIFIED:** Input constructor transports exact acquired provider results into guarded increments; ordinary Input cleanup transports newly sampled current provider roots into guarded decrements. Each provider holder registers a code-only current-root zero-gated finalizer. Available finalizers leave the guard between zero observation and current deleting invocation, clear the then-current root after return, and return AL1. Their exact named implementations expose member/backing operations only conditionally on actual current-table identity. **STRONG_INFERENCE:** paired arithmetic serves provider dependency accounting. **UNKNOWN:** balanced responsibility, same generation, actual callback/deleting occurrence, closed admission, final user, completion and successful safe free.

## Provider identity and offsets — VERIFIED

- StopWatch: root`00BD9E1C`, requested holder allocation`1C58`, constructor`00407B60`, raw table`0076F098` / COL`00875550` / TD`00886548` names`TSiHolder<CStopWatch>`; named CStopWatch base has raw zero-offset PMD`(0,-1,0)`. Holder initializes provider`+1C50=0`, registers literal`004035E0` at`00407B8D→00407BA0/006E1930`, returns saved receiver.
- Actuator: root`0148CA34`, requested holder allocation`64`, constructor`0070B990`, raw table`00827664` / COL`0087D344` / TD`00BD59A4` names`TSiHolder<CInput_Actuator>`; raw named base PMD`(0,-1,0)`. Holder initializes provider`+60=0`, registers literal`0070BA50` at`0070B9CE→0070B9D3/006E1930`, returns saved receiver.

**Offsets`1C50/60` belong to providers, not fields in Input retaining provider pointers.** The exact selected transport windows contain no Input-field pointer store, and the actual cleanup uses fresh current-root accessors regardless of any unrelated storage elsewhere. No whole-object/global absence or standalone core sizeof is inferred. Roots name selected typed acquisitions, not every possible runtime occupant.

## Invoked arithmetic transports — VERIFIED; dependency interpretation SI

Use distinct `S_ctor/A_ctor`, `S_cleanup/A_cleanup`, `S_final/A_final` symbols.

| Selected Input call sequence | Exact supplied receiver and operation |
|---|---|
|`00707DF7→00407F40→00407CF0`; `00707DFC ECX=EAX`; `00707DFE→00703B70`|Exact selected StopWatch getter result; guarded DWORD`[S_ctor+1C50]+1`|
|`00707E0F→0070A330→0070B780`; `00707E14 ECX=EAX`; `00707E16→00706080`|Exact selected Actuator getter result; guarded DWORD`[A_ctor+60]+1`|
|`007082E0→00703BD0→007041C0`; `007082E5 ECX=EAX`; `007082E7→00703BA0`|New current`00BD9E1C` sample; guarded DWORD`[S_cleanup+1C50]-1`|
|`007082F8→006E1840→006E1850`; `007082FD ECX=EAX`; `007082FF→0070A340`|New current`0148CA34` sample; guarded DWORD`[A_cleanup+60]-1`|

Complete arithmetic helpers preserve the supplied receiver, use accepted global guard entry/leave, and perform unconditional DWORD arithmetic at that offset. They have no local underflow guard, zero-finalizer dispatch, root clear or generation token. Wrapping arithmetic is not an established balanced reference-count invariant. Input cleanup does **not** invoke either provider finalizer in these selected sequences.

## Corrected StopWatch registered code — VERIFIED conditional interface

Complete raw`004035E0–0040361C` / SteamASM2959–2979:

1. EBX=0; enter accepted global guard.
2. ESI=`S_final=[00BD9E1C]` at`004035E9`.
3. compare`[S_final+1C50]` with0 at`004035EF`; nonzero skips deleting/clear, leaves guard, returns AL0.
4. zero path leaves guard at`004035F7`; loads saved receiver's current table/slot0, pushes1, ECX=`S_final`, calls at`00403604`.
5. after normal return re-enters guard at`00403606`, clears **current** root`00BD9E1C` at`0040360B` without comparing its occupant to`S_final`; sets BL1, leaves guard, returns AL1.

There is no local null test before count/vptr dereferences and no return-status test before root clear/AL1. This raw code is absent as an exported function entry; track its literal, complete bytes, class/interface and boundary independently rather than manufacturing exported-inventory coverage. Registration supplies code, not a saved provider object. Accepted C0193 carrier mechanics apply only conditionally if current code selects this literal; actual registration/selection/invocation is not observed.

The independent branch initially checked`00407C50` as a candidate and rejected it; that complete body lacks the proposed root/count gate. This was a **DISPROVEN preselection**, not a canonical disproven claim. The corrected literal`004035E0` is now personally checked under the new main contract.

## Actuator registered code — VERIFIED conditional interface

Complete`0070BA50–0070BABF` / ASM883714–883747:

- current root→saved`A_final` at`0070BA5F/64`; test`[A_final+60]==0` at`0070BA6A`;
- zero path leaves guard at`0070BA70`, copies saved receiver, performs a **late** null test; if nonnull calls its current slot0 with1 at`0070BA93`;
- indirect result is stored at`0070BA95` but not tested;
- re-enter guard at`0070BAA1`, clear **current**`0148CA34` at`0070BAA6`, set local flag1, leave guard, return AL1;
- nonzero count skips invocation/clear and returns AL0.

Late null handling does not protect the earlier count dereference. Guard release between test and call and unconditional-current-cell clear do not establish admission closure, enduring zero count, occupant preservation, last user, race occurrence or safe free.

## Exact named deleting/member/backing interfaces — VERIFIED availability, conditional dispatch

**StopWatch table slot0=`00407BD0`:** complete wrapper preserves incoming S, rewrites table, passes`S+1C18` to the accepted DeleteCriticalSection import at`00407BEA`; passes`S+8` to opaque member cleanup`00407890` at`00407BF3`; only afterward tests original flag bit0 and passes that same S to accepted backing operation`00403540` at`00407C00`. Return saved S/RET4. Member helper and backing operation are **not** proof of full successful object destruction or last use and are not widened here.

**Actuator table slot0=`0070BA00`:** complete wrapper preserves A, calls`0070BA30(A)` before flag test, then bit0 conditionally passes same A to`007068A0` and returns saved A/RET4. Complete`0070BA30` rewrites holder table and forwards same A to`00735B60`; complete latter writes CInput_Actuator table`00828F54`, invokes accepted lock cleanup`00403730` on`A+4`, returns. The thin complete backing adapter`007068A0` forwards its pointer stack argument to opaque`00404490`; no external-success status is established or checked by the finalizer.

If current finalizer receivers retain their named holder tables, literal1 selects these implementations and backing operations. That is **not** proof these targets were actually selected or that same-generation storage/locks can safely retire. Member cleanup precedes any backing operation; it does not demonstrate all consumers detached.

## First missing edges / no widening

- `S_ctor→S_cleanup→S_final` and `A_ctor→A_cleanup→A_final` generations/accounting responsibility. Same root-cell address and paired arithmetic do not prove equality or balanced count.
- Code registration→current carrier code→actual finalizer/deleting occurrence; actual current vptr must be matched independently.
- Zero test→same-generation admission closure and completion of every user before lock/member/backing operations. Finalizer AL1/root clear is local progress, not complete safe release.
- Saved finalizer receiver→post-call cleared root occupant; possible replacement is not asserted, but there is no local occupant comparison.
- Transitive opaque member/backing outcomes, last use, runtime cadence, GOG/Xbox equivalence remain UNKNOWN. Do not force closure through generic user, registry, timing, Win32/free or Input algorithm inventories.

## Verification and history

Independent branch`scratch/phase5_input_provider_branch_20261004.md` / `_primary.json`: six local slices,248instructions/880bytes including reused transports and rejected preselection. Main`scripts/inspect_phase5_provider_main.py` → `scratch/phase5_provider_main_20261004.json` plus distinct`..._replay_seq0086.json`: each recheck256instructions/835bytes and new interface120instructions/355bytes. Counts overlap/reuse and are not additive discovery credit. Main personally checked raw RTTI/base PMDs, code literals, exact getter-result/counter arithmetic/current-root/slot/clear/member/backing joins. Initial main Actuator registrar-call address transcription failed before output, then corrected from`0070B9E1` to actual`0070B9D3`; no semantic/input or validator change. One read-only CSV query omitted BOM handling and failed; rerun used utf-8-sig, no state edits.

All70+18 guards, C0047/C0086 DISPROVEN/H0209 invalid old basis, C0198 pointertransport-notTread, Input+668 eleven-byte no-op, `(1000,0)`→Sleep DWORD1 request-notcompletion remain intact. No input/binary/asset/production/runtime modifications, patches/hooks/ASI or Phase6.
