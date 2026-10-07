# CInput registered finalizer, dependency and callback/member lifetime

**2026-10-04; STEAM_PC; Phase5 batch7 / sequence78.** C0196–C0198. Accepted C0123/C0124 acquisition/sample/camera and C0193 named code carrier are reused only at exact scope. Mode9, sample/getter/commit algorithms, Win32 API and generic registry/free lanes stay excluded.

## Contract and result

`scratch/phase5_input_seq0078_contract.md`. New discriminator: actual application→CSiHolder current-code invocation narrows the former available/registered Input finalizer gap. Fresh single-assignment Opus branches independently examine guard/dependency and embedded/member cleanup. Important joins are personally checked against the Steam PE/ASM in separate main receipts.

**VERIFIED conditional local mechanism:** code-only registration, currentI BC8 gate→slot0(flag1)→named holder cleanup/deallocation boundary→currentroot clear/AL1; separate Sysutil220 increment/decrement; retained Input callback receiver/target; member cleanup order and no-op668 target; callback byte25 observation does not eliminate remaining T use. **STRONG_INFERENCE:** cross-root dependency-based retirement intent. **UNKNOWN:** registration success/carrier-slot/currentI generations, balanced same-U counts, admission closure, callback detachment/last use, thread quiescence and successful safe deallocation. This is not a complete input shutdown or observed race.

## Registered code versus selected current-root invocation — VERIFIED

00409370 preserves constructedI; calls accepted CInput ctor00707D90, installs rawTSiHolder<CInput>0076F5DC and supplies literal00408A30 to006E1930 at0040939D/B0; it then storesI+BC8=0 at004093B8. Registrar retains only a code pointer in its first vacant100-slot carrier record. No object context is captured and exhaustion is not checked. Reused C0193 supplies application0040141D→00408D40→006E19A0, currentcarrier/currentcode call006E1A0D and AL1 current-record retirement.

**Conditional, not asserted occurrence:** if a continuing dispatcher call selects00408A30, that code obtains currentI=[00BD9E10] at00408A39 and comparesI+BC8 with0 at00408A3F. Nonzero returnsAL0 without deletion/rootclear. Zero releases the guard, loads the currentI table/slot0, calls it withECX=the savedI andflag1 at00408A54, reacquires the guard and clears **current**00BD9E10 at00408A5B beforeAL1. There is no independentnull gate beforeI+BC8. Readable/liveI and ordinary returning calls are prerequisites; post-call root clear does not compare the cleared occupant/generation withI.

Raw holder0076F5DC slot0=004098C0. Conditional on that still being the loaded currenttable, its savedI is passed to00708280 at004098C9; flagbit0 supplies sameI to00403540 at004098D6. Accepted allocation-prefix decoder makes this a backing-allocation deallocation boundary, not proof that interiorI is the backingpointer or that release succeeds. BaseCInput table00827604 deleting00709AC0 is distinct. Availability/type-compatible invocation is not every registered callback running or actual lifetime closure.

## Separate Sysutil dependency and retained callback — VERIFIED

Selected construction00707E03 calls00409A40, a raw tailJMP to accepted Sysutil lazygetter004098F0. Its result **Uconstruct** is immediately supplied asECX to004091E0 at00707E08/A. Adapter preservesU and under globalguard executesINC[U+220] at004091E8.

Selected cleanup007082EC instead calls004043E0. This guarded accessor returns the pointer sampled from **00BD9E0C**, not a saved constructionU. Returned **Ucleanup** is immediately supplied to00409200 at007082F1/3; that adapter executesDEC[U+220] at00409208. Reused C0195 finalizer separately gates **Ufinal220==0**. Each local result→adapter receiver is concrete; **Uconstruct=Ucleanup=Ufinal and balanced responsibility remainUNKNOWN**. These are not writes to InputBC8.

Input ctor creates rawTCallBackClass<CInput>00827624 at **C=I+94**. Setter007037D0 storesI atC+4=I+98; setter00709B70 storesliteral00709840 atC+8=I+9C. Selected ctor calls currentC.virtual4(0) at00708218; rawnamedcallbacktable+4=007037F0, not00709840. This proves callback configuration and a constructor call to its virtual interface, not actualtarget execution. Incoming thread activation/targetdispatch, 007037F0 argument-field writer (C+0C=argument), not a target call and unchanged currentC table remain separate obligations.

Ordinary callback cleanup00709B10→004091B0 rewrites C's vptr (endingCCallBack0076F5B4), without clearing C+4/+8 or detaching any external retained pointer. Vptr reset is not receiver/target invalidation or callback cancellation.

## Exact ordinary Input member order — VERIFIED

00708280 savesoriginalI and rewritesCInput00827604. On normal returning calls:

1. acquiresI+A4 guard; calls00709C30 on **I+668**; releasesguard;
2. calls00703BD0→00703BA0 withthe returnedprovider (opaque integration effect here);
3. decrements separately acquiredSysutilUcleanup220;
4. calls006E1840→0070A340 withthat provider (opaque integration effect here);
5. deletesI+A4 guard via00403730 (criticalsectionI+A8);
6. rewritesI+94 callback via00709B10;
7. callsordinaryCCallBackThread00702B80 on **T=I+8**, beforeholderstorage boundary.

**668 target:** complete00709C30–00709C3A is11 bytes/7 instructions: ordinary stack-frame prologue/local receiver save/epilogue/RET, with no callees or non-stack writes. It establishes no handle/device/resource release or field reset. Embeddedregion class/ownership remainsUNKNOWN; prior acquisition-region label is retained at sample-receiver scope, not promoted to an owning device class.

**T genealogy:** ctor00707DC4/C7 suppliesI+8 totypedCCallBackThread00702AB0/raw008272C8. Reused ordinary00702B80 deletesT+68 guard's criticalsectionT+6C (=I+74) and invokesvptr-onlyCThfuncbase00409E80. These selected explicit member operations do not themselves establish callback stop/detach/wait/join/handle-close. Opaque integration calls and upstream coordination remainunknown; no claim that all external coordination is absent or a live thread races.

## Callback stop observation is not last use — VERIFIED selected interface

Installed00709840 preservesECX=I. Selectedconditional sample arm locksI+A4 around opaque accepted00708B00 (algorithm not reopened). At007098F5/F8 it suppliesT=I+8 to00703290, which returns sampledbyteT+25 underT+68 guard. Nonzero writes onlystacklocal next-pass flag0 at00709904. On the continuing normal-return path it **still reaches00709993→007032C0 withECX=T**, thenjumps007098B5 tocheck thatlocalflag beforeexit. Tail004-thread operations are not assumed join/timing or success. A stop-marker observation therefore cannot supply the finalT use orsafe member-lock retirement.

## First missing edges and finite cap

A stronger claim needs registeredcarrier/slot→currentI generation, typed admission closure/detachment forC=I94/receiverI/target00709840 and T=I8, completion afterthe finalcallbackT use, and a same-generation relation tolock/member/backing-storage teardown. Uconstruct/Ucleanup/Ufinal also need exactgeneration/balance. Rootclear, BC8zero, T25 observation and AL1 cannot substitute.

No globalabsence is claimed. BC8 literal-use inventory and root-export omissions are a finite source-cap qualification, not proof against indirect writes/aliases or unexported consumers. Stronger input lifetime is NOT_READY without a newtyped closure/completion source orcorrelatedtrace; do not widen into sample/commit/mode9/worker/registry/API inventories. Independentmodel buffer/packet retirement is the successor candidate.

## Primary verification

- `scripts/inspect_phase5_input_registration_main.py` → `scratch/phase5_input_registration_main_20261004.json`: **83 instructions/303 bytes**, code registration/current-code seam andrawholder.
- `scripts/inspect_phase5_input_lifetime_main.py` → `scratch/phase5_input_lifetime_main_20261004.json`: **181/617**, finalizer/member/U220/callback joins, fourrawtables.
- `scripts/inspect_phase5_input_members_main.py` → `scratch/phase5_input_members_main_20261004.json`: **130/446**, exactno-op/member/provider/thread genealogy.
- `scripts/inspect_phase5_input_callback_main.py` → `scratch/phase5_input_callback_main_20261004.json`: **62/216**, callbackstop-observation/remainingTuse.

Counts overlap and earn no additive novelty. PerinstructionVA/ASMline/bytes andsourcehashes inreceipts govern navigation. MainASM locators: registration9954–9980; finalizer9290–9310; deleting10363–10376; corecleanup879267–879318; ctorcallback879232–879252; Uadapter9811–9827; memberleaf881108–881114; callback880784–880865; T25getter872395–872411; currentUaccessor4034–4040/thunk10504.

No runtime/GOG/Xbox confidence orhomology promotion. K0022/C0047/C0086/H0209 andK0023 histories unchanged; all70+18 obligations preserved. No game/input/asset/production/runtime modification, Phase4re-audit, Phase5closeout orPhase6.

## Independent returns/replays and finite source qualification

Both freshsingle-assignment branches completed. Originalscratch findings/checkers/receipts preserved. Cleanupreplay `scratch/phase5_input_cleanup_branch_main_replay_20261004.json`:50contiguousspans/761instructions/2504bytes/100rel32edges/10tables/5imports. Guardreplay `scratch/phase5_input_guard_main_replay_20261004.json`:33semanticspans/420instructions/1371bytes; nonadditive mechanicalunion1130instructions/5153bytes includes gettercalls checkedonlyasbytes, notalgorithms. Mainimportantjoins separatelypersonallycheckedabove. Initialcleanupreplay output-name assertion failedbeforeacquisition; correctedonly NEWfilename prefix, originalchecker/receiptunchanged; bothactualreplaysPASS. Noorchestrationrepair.

**VERIFIED finite literal scope:** twoBC8 operanduses (004093B8zero/00408A3Ftest), notproof againstcomputedalias/indirectwrites. Rootexports225references(223read/2write), raw.text234literaloccurrences include9omittedguardedloads at0040DB36,004B016D,004B113E,004B1165,004B11A9,004B11D0,004B1216,004B124F,004BEC61. Onlytheir3-instruction/16-bytewindowschecked; owner/consumerbodiesUNKNOWN. BC8raw4patternhitsinclude2relativebranch/calldisplacements, notextraaccesses. Exportliteralcaps arenotglobalconsumer/writerabsence.

**Unpromoted additionalbranchlocators:** cleanupbranchrawtypes/providerchains nominate CStopWatch00BD9E1C/+1C50 andCInput_Actuator0148CA34/+60 as the otherselectedcounter receivers. Their type/currentprovider genealogy ispreserved in `scratch/phase5_input_cleanup_branch_20261004_findings.md` andsuccessfulreplay butnotusedfor canonicalprovider ownership/identitypromotion inthisbatch; targetedpersonalverification ofallload-bearingnewproviderjoins remainsbeforeanysuchpromotion. Construction/currentcleanup generation/balance/lastuser stillmissing regardless. NarrowCCallBackThread007032C0→00408AF0 progresswrapper is a qualifiedsuccessorlocator, notassumedSleep/join.

## Sequence82 remainingTuse precision qualification

C0202: complete007032C0 savesbutneveruses/dereferencesthis. Earlier remainingTuse namespointertransportonly, notlateTmemoryaccess. Callbackstillhashelperwait/localcleanupbeforereturn; noactualactivation/participantlastuse/join orsafe retirement. C0198 literalreachability unchanged. findings/boundaries/phase5_shared_progress_delay_retirement_limit.md

## Additional-provider qualification — sequence86

**VERIFIED STEAM C0205-C0207:** exactconstructorproviderresult INC versuscleanupnewcurrentroot DEC, provideroffsets1C50/60 notInputpointerfields; typedholders registercode004035E0/0070BA50, availablezero-gate/unlock/currenttable0flag1/relock/currentrootclear/AL1. Namedholdermember/backinginterfaces availableconditionally. **SI:** dependencyaccounting interpretation. **UNKNOWN:** gen/balance/actualcarrier/currenttable/admissionclose/lastuser/success. OriginalInput/allseq83guards unchanged. findings/boundaries/phase5_input_provider_accounting_finalizers.md
