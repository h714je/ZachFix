# SaveData/Sysutil deferred current-root finalization and callback lifetime

**2026-10-04; Phase5 batch6; STEAM_PC.** C0193–C0195. Accepted C0146–C0148 request/callback/buffer protocol is reused, not reconstructed. The closed save-gate prefix, staging/schema/player/file-temp sources remain excluded.

## Contract and result

Selected `P5_SAVE_CALLBACK_RETAINER_LIFETIME`: actual application-held code-carrier invocation, typed current-root finalizers, idle/dependency gates and callback/thread/member/storage teardown. Fresh bounded branch was read/replayed into a separate receipt; primary personally checked important joins and the newly reached carrier's raw type/constructor/deleting implementation.

**VERIFIED local mechanism:** application reaches a typedCSiHolder carrier with100 code slots; registered finalizers are called without captured object context, AL1 retires a code slot and other results leave it for another pass. SaveData finalizer gates currentholder70 before deleting; SaveData cleanup polls28, decrements a reacquiredSysutil220 and resets callback/lock state. Sysutil finalizer gates current220 before deleting; its explicit member teardown destroys locks/callback/thread storage without an acquired stop/join or C8-detachment edge.

**STRONG_INFERENCE:** conditional dependency-based deferred-root cleanup role, from construction-time increment, cleanup decrement, zero gate and repeat-pass behavior. **UNKNOWN:** retained registration/current-carrier/currentroot generations, balanced counts, admission closure, callback/buffer last use, actual thread quiescence and successful external operation/deallocation. This is not complete save shutdown or a Phase5 closeout.

## New typed carrier and invoked application seam — VERIFIED

`00401310@0040141D →00408D40`, then raw tailJMP00408D62/6E→006E19A0. This supplies an executable application→carrier dispatch route, not just `_atexit` registration or a destructor-table locator.

**Distinct-root guard:**00408D40 first touches accepted **CSingleton<CRdMovie> cache00BD9E48/table0076E838** if needed, then tail-jumps the dispatcher. That cache is not the code carrier, SaveData, Sysutil or a proved finalization owner.006E19A0 does not use the incomingECX as a movie/holder receiver. Do not merge roots because they share this wrapper.

Provider006E1AA0 allocates observed0x194 and sends that result to006E1870, then publishes/returns it through **0148CA40**. Constructor installs **00826D8C**, rawRTTI **CSiHolder**, and under externalguard0148CA44 zeros100 DWORD slots atthis+4+4*i. Own observed one-slot table points006E1B50. This named carrier is distinct from the templated **TSiHolder<CSaveData>/TSiHolder<CSysutil>** object wrappers.

Holder constructors pass literal root-finalizer code pointers **00408A70** (SaveData) and **00409240** (Sysutil) to006E1930. Registrar stores supplied code pointer into the first vacant one of100 slots; exhaustion may return without retention and constructor callers do not check a success result. **No constructed object pointer/context is captured.**

Dispatcher acquires its guard and savedcarrierRscan, scans100 slots, releases the guard before each call, reloads **current0148CA40 and currentslot function** and callsEAX at006E1A0D. It saves returned **AL**, reacquires the guard and clears the then-current physical slot only for **AL==1**. Other byte values retain it. Any observed nonzero slot keeps the outer pass flag set; another scan follows after opaque00408AF0(1000,0). This is a local retry/progress protocol, **not100 bounded retries or proved Sleep1000**. The helper's transitive behavior is unexpanded.

After a pass observes no records, driver invokes the currentcarrier's slot0 withflag1 and clears0148CA40. NamedCSiHolder slot006E1B50 saves its receiver, calls vptr-only006E18D0, and underflagbit0 calls006E1910→00404490→00403540. This is an available named deallocation route compatible with the current-carrier call, not unchanged-currenttable/generation/runtime-success proof.

**Opaque/generation breaks:** Rscan, currentRcall and currentRclear are separately observed around unlocks/callbacks; post-call slot clear does not compare the code pointer/record generation to the invoked one. Registeredliteral→retainedrecord→currentdispatch→currentobjectroot is not joined solely by equal addresses.

## SaveData holder gate and idle/dependency cleanup — VERIFIED

Orphan **00408A70** loads **Afinal=[00BD9FF8]**, readsDWORDAfinal70 and onlyzero permits currentvtable.slot0(Afinal,1). It unlocks around that call; after normal return clears the currentroot underguard and returnsAL1. Nonzero70 returnsAL0 without deleting/clear. It has no independentnull gate before the field read: the stated continuing route requires a readable/live root. Do not infer safe allocation failure or generation equality from registration.

RawTSiHolder<CSaveData>0076F604 slot0=00409C40. Conditional on currenttable being that holdertable, supplied1 selects same-this00409B20 followed by00403540 deallocation boundary. BaseCSaveData deleting slot00409C70 is distinct and not selected by this table.

Completeordinary00409B20:

1. writes baseCSaveData vptr;
2. underEnter/LeaveCriticalSection(Afinal+8), pollsDWORDAfinal28; nonzero sleeps1 and repeats; zero allows progress;
3. reacquiresSysutil through004098F0 as **Ucleanup**, then **DEC[Ucleanup+220]** underglobalguard;
4. replaces embeddedcallbackAfinal2C vptr withCCallBack0076F5B4, thenDeleteCriticalSection(Afinal+8);
5. returns to holder deallocation; rootfinalizerlater clears the root.

The selected construction slice separately obtains **Uconstruct** and incrementsUconstruct220. Sysutil's later finalizer gate reads **Ufinal220**. INC/DEC/zero gating is concrete; Uconstruct=Ucleanup=Ufinal and balanced reference/dependency responsibility are not proved across reacquisitions.

The completeSaveData cleanup does not explicitly call00707750/clearU+C8, clear embeddedcallbackreceiver/target, clearA40/44 or release its caller buffer. **Vptr reset is not detachment** from another object's retained pointer. Accepted state3 completion has a separate C8clear path, but polled28==0 alone does not prove that same U generation is detached/no callback in flight/no new admission.

## Sysutil counter gate and member/thread teardown — VERIFIED

Orphan **00409240** loadsUfinal=[00BD9E0C], readsDWORDUfinal220 and onzero calls currentholder.slot0(Ufinal,1), then clears currentroot and returnsAL1. It likewise requires a readable root; nonzero220 retains the code callback viaAL0. Holder0076F5EC slot0=00409890→00707310, thenflagbit0→00403540. Observedholderallocation0x228 is not a proved baseCSysutil sizeof.

The selected member genealogy is exact: **T=U+8** is rawtypedCCallBackThread008272C8; U+94 isTCallBackClass<CSysutil>; U+A4 is a guard shell whose actualcriticalsection isU+A8.

Complete00707310, on normal returns:

- obtains two other opaque provider results and decrements their +4/+1C50 fields; do not name those providers or assume currentgeneration/lastuser;
- deletes criticalsectionU+A8 through00403730(U+A4);
- rewrites callbackU+94 through00707A10→004091B0 without clearing receiver/target or C8;
- calls ordinaryCCallBackThread cleanup00702B80 on **U+8**. It deletes its own guard's criticalsection at **U+74**, then invokes acceptedCThfunc vptr-only00409E80 onT;
- returns to holder deallocation, then currentroot clear.

The explicit checked member expansion supplies no C8detach or stop/wait/join/handle-close before those operations. Opaque other-provider effects and unexamined upstream coordination remain UNKNOWN. This is not proof that a live callback thread exists, is never joined elsewhere, or races with destruction.

**Allocation identity:**00403540 reads the allocation prefix before the suppliedholder and calls accepted_free boundary with a decoded backingpointer. The interiorholder pointer must not casually be equated to that backingpointer. No CRT internals were reconstructed. AL1 root-finalizer result is local normal guarded-route progress, not successful external destruction/deallocation/quiescence.

## First missing edges / successor

A stronger lifetime claim must connect the same **U/T=U8/U+C8/A+2C/A callbackreceiver/A40 callerstorage** through admission closure and final use/stop-join before lock/member/storage retirement. It must also join registration/carrier/currentroot generations, construction/decrement/finalcounter observations and post-call currentroot/slot clear.

The new namedcarrier exposes a strong independent successor: accepted C0123 already supplies **CInput's registered00408A30**, BC8gate and holder/core cleanup004098C0→00708280. Its previously missing executionowner now has a concrete class-code carrier discriminator at conditional interface scope; a fresh ordinary primary batch must verify the input-specific currentroot/cleanup/lastuse relation, without reopening closedmode9 acquisition or replaying sample/commit algorithms.

Othernewcandidate: carrier's specific00408AF0 progress helper is unexpanded. It is not assumed to activate/close/join Sysutil or to supply an OS-success/timing contract. No registry-wide finalizer/constructor/free census is authorized by this one route.

## Primary verification

- Returnedchecker reviewed/replayed to new `scratch/phase5_save_callback_lifetime_main_replay_20261004.json`:**599 instructions/1948 bytes**,33 continuous boundedspans,22 directtargets,21 anchors,9rawtypes/5imports. Originalreturn preserved; sourcehashes retained.
- Main `scripts/inspect_phase5_save_lifetime_main.py` → `scratch/phase5_save_lifetime_main_20261004.json`:**366 instructions/1208 bytes**, importantAL/root/idle/counter/member/deallocation joins plus newCSiHolder typedconstructor/slot. Counts overlap; no additive novelty/runtime claims.
- SteamASM: app/wrapper300/9551–9563; rootfinalizers9312–9332/9848–9868; Savedatacleanup10576–10627; registration/dispatcher/provider830664–830811; newcarrierconstructor/deleting830594–830618/830839–830853; Sysutilcleanup878032–878071/thread871695–871722. ExactJSON VA/line/bytes govern shorthand.

No input/game/production/runtime/GOG change; Phase4 remains accepted/closed, noPhase6.

## Sequence82 sharedprogress qualification

C0201/C0202: formerlyunexpanded00408AF0 literal1000/0 requestsSleepDWORD1 afterunsigned/1000 conversion; notSleep1000/observed1ms. Explicitcarrierguardprotocol/noLeave anduncheckedreturn notcallbackdrain/workerjoin/currentgeneration closure. OriginalC0193-C0195 history/intents preserved. findings/boundaries/phase5_shared_progress_delay_retirement_limit.md
