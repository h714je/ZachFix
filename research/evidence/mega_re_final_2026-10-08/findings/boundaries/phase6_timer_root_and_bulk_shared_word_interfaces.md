# Timer cache/root and CFunc shared-word interfaces — sequence140

**STEAM_PC only.** Fresh primary reproduced retained51/159 from137 before new acquisition; new main41instructions/109bytes covers only00470880(42) and006EB0F0(67), plus own Timer RTTI. Independent cards: scratch/phase6_seq0138_bulk.md, phase6_seq0138_timer.md, phase6_seq0138_atomic.md. Main receipts: scratch/phase6_entry_retained_qualification_seq0138.json; scratch/phase6_timer_atomic_personal_seq0140.json; scratch/phase6_timer_atomic_comparison_seq0140.json. No caller/callee/slot/GOG/runtime expansion.

## VERIFIED: direct Timer construction/access root

Retained00449480 loads00BE1EA0. Cold path requests0x24 through00404390, supplies savedP inECX to00470880, installs00772140, publishes savedP (nullarm0) and returns. Direct RTTI namesCSingleton<CTimer>, declaresCTimer at0 andCEvFlag at4.

New00470880 saves incomingECX inEAX and returns that value. It installs00773280, whose own RTTI namesCTimer and declaresCEvFlag at4. It zeros DWORD4/8/C/10/14, stores floats0.0 at18 and1.0 at1C, zeros bytes20/21. Observed own write extent22hex; bytes22/23 untouched, request24 is not universal sizeof. Observed one-slot tables point to unopened004486A0(wrapper)/00470D70(base), not complete interface/current-type proof.

The root is shared/cached by construction, not a local automatic Timer. No timing API, clock read, elapsed-time calculation or updater appears in this initializer. Actual clock units, cadence, synchronization, owner, lifetime and update placement remain UNKNOWN. The two floats cannot name a clock policy. This is a newly explicit named cache/state shell, not a proved scheduling subsystem.

## VERIFIED: exact paired operand transport and fixed shared word

Accepted0044E560 pushes0 at0044E626, invokes getter0040E470, moves returnedEAX toECX and calls006EAB30. Opposite0044E640 pushes1 at0044E6CC and follows the same interface. Accepted getter has ordinary no-stack-argument return; the pending DWORD is delivered to006EAB30, which reads its low byte andRET4 consumes it. This is not an argument attributed to the getter.

Retained006EAB30 saves but never usesECX. Lowbyte0 supplies fixed014940F8 and0 to importedInterlockedExchange; nonzero supplies the same fixed address to importedInterlockedIncrement. Exact symbol/import metadata names pointer slots0076E044/0076E07C. EAX is left as the imported result, not normalized success/completion. It does not access an acquiredPhysicsCore-relative field. CFunc also clears/sets bit2 of separately acquiredTimer+4, compatible with declaredCEvFlag subobject; no common named policy is established.

New006EB0F0 has no ownECX receiver use. StackDWORD0 returnsAL0; equality with fixed[014940E4] returnsAL0; equality with fixed[0149406C] selectsAL1. Otherwise it pushes-1 and014940F8, calls opaque00712D50, testsEAX and returnsAL0/1. This proves the same fixed address is transported by two distinct functions, not that00712D50 reads/decrements/waits on it. AL-only returns do not establish normalized fullEAX. Word ownership and alias toPhysicsCore+67CC8 remain UNKNOWN.

## Architectural qualification and stop edges

**VERIFIED local structure:** CFunc-compatible local flag/control fanout connects selected objects, a separate typed Timer flag interface and a separate fixed-word atomic request. **STRONG_INFERENCE:** directionally opposed control sequences; the shared word is an architectural coordination candidate. **UNKNOWN:** dependency manager, retained/shared dependencies, node ownership, service/root ownership, generation continuity, traversal termination and common pause/resume/scheduling/readiness semantics. Do not force any of those labels.

The generic dependency-manager hypothesis receives no positive support at the qualified scope; deprioritize its per-element internals. Neither “no manager anywhere” nor “pure local accounting” is proven. The newly independent fixed-word operand pathway keeps a small coordination hypothesis alive. Exact next discriminating edge00712D50 has42 metadata bytes, still UNVISITED here. Timer actual state updater/clock consumer remains a separate static gap; no slotbody authority is implied by this report. Event/orphan branches remain independent and pending at this checkpoint.

C0246/BND-235 retain their syntactic acquiredCore-result relay; extend their limitation: final helper ignores that receiver and targets fixed storage. This is not a disproof of acceptedCore root/lifecycleC0121 and not an instance-field alias. All88 inherited guards/acceptedPhase5scope, failures and source caps remain intact; Phase7NOTSTARTED.

## Corrections and procedure history

Main first compared the Timer receipt as a whole object and failed because the branch adds metadata. Second comparison failed on representation (branch triples versus main instruction dictionaries). Explicit [va,bytes,assembly] normalization plus counts15/42 then matched; atomic receipt matched26/67. No executable-byte disagreement occurred. Preserve these two failed checks. Branch empty-pages failures, oversized metadata outputs and unused incidentalGOG row are disclosed in their original cards and supply no additional source/evidence credit.
