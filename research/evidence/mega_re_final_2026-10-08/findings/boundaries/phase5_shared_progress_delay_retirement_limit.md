# Shared delay progress versus finalizer/callback retirement completion

**2026-10-04; STEAM_PC; Phase5 batch9 / sequence82; C0201/C0202.** Contract `scratch/phase5_progress_seq0082_contract.md`. C0193 driver/AL1/rootprotocol and C0198 callback/member joins reusedonlyatdeclaredscope. Newsharedhelperbody/ABI andcallerheldguard/unusedthis/return relationship; nogenericthread/clock/API/registry/oldmode9 expansion.

## Result — request, not completion

**VERIFIED:** complete00408AF0 converts anunsignedtwo-dwordvalue to **low32(floor(U64(high:low)/1000))**, supplies thatDWORD toPE-import-linkedKERNEL32.dll!Sleep, thenreturnswithouttesting/definingcompletion status. CSiHolder(1000,0) requestsDWORD1, **not1000**. Callerlocalguardprotocol hasnoLeave beforethiscall; threadwrapperforwardssuppliedarguments butdoesnotdereferenceitsreceiver. **UNKNOWN:** inputclock/provenance forallarguments, runtimeIATbinding/elapsedtime, sameparticipant generations/admissionclosure/callbackdrain/lastuse/stopjoin/safe retirement. This closes the finitehelper question, not any subsystemquiescence.

## Complete helper and arithmetic ABI — VERIFIED

00408AF0–00408B0D is30bytes/10instructions. It loads entryESP+8/high and+4/low, pushesdivisorhigh0/low1000 andnumeratorhigh/low, calls0074F9F0 at00408B01, pushesonlyEAX at00408B06 andcallsIAT0076E028 at00408B07. FinalplainRET leavesoriginal8argumentbytes tocaller. IncomingECX isoverwritten, notdereferenced; notypedreceiver orcapturedparticipant exists.

Selectedreachable__aulldiv path hasdivisorhigh=0, soitsJNZ0074FA12 arm isnotselected. AftercalleeEBX/ESI pushes, numeratorlow/high resideatESP+0C/+10 anddivisorlow/high at+14/+18. UnsignedDIV firstdivides high by1000, thenremainder:low by1000; highquotient savedinEBX/returnedEDX, lowquotientEAX. RET10 popsits4internalDWORDarguments. Onlyfixedreachable34-bytepath and5-byteepiloguechecked; no CRTdivideinventory. Helperignores quotientEDX; no signed-/overflow-/range-/INFINITE guard suppliesa universalboundedreturn. Mathematical examples arederivedsanitychecks, notemulation/runtime.

PEimportdescriptor008833BC, lookup008834FC/IAT0076E028, hint/name008838D2 andDLL00883A06 establish on-disk **KERNEL32.dll!Sleep**. Runtimebinding isunobserved. The separatelyfetchedMicrosoftAPIcontract documentsDWORD milliseconds andvoidreturn; itdoesnotprove exactelapsedtime orgameclock source. `scratch/phase5_sleep_api_contract_20261004.md` isdocumentationonly, notprimarygamebehavior.

## CSiHolder actual call and local guard — VERIFIED conditional control

Accepteddriver acquires shell0148CA44 via00403750 at006E19AB. Nonzeroslots releaseit at006E19FB beforecurrentcodecall, thenreacquireit at006E1A17; zero-slotpaths keep thecurrentlocalguardprotocol. Atloopend itpushes(1000,0), calls00408AF0 at006E1A3E, adjustsESP andjumpsdirectly006E19BC. **No localLeaveCriticalSection isissuedbetweenthatacquire/reacquire anddelaycall.** Adapter00403750/60 actualcritical-section address is0148CA48. This isitslocalnormal-returnguard protocol, notproof ofallopaque callbacklock-depth/thread/context effects orglobaladmissionquiescence.

Henceeachcompletedscan requestsSleepDWORD1 (documented1ms request) beforecheckingpassflagagain, evenanemptyobservedscan onthisnormalroute. Thehelperreturn isnotinspected; repeat/retire logic stilldepends onslots/AL1/currentroots, notSleepacknowledgement. Thedocumentedinterval isnotmeasuredcadence,100boundedretries, callbackservicing orthejoinof anyinput/resource/physics/save worker.

## Thread-wrapper and callback completion — VERIFIED / qualified history

Complete007032C0 is29bytes/13instructions. It savesincomingECX onlyinto anunusedstacklocal. It forwards itsstack+8/+C pair to00408AF0, adjustsstack andRET8, withno receiverdereference, stopflagread, callbackdispatch, statusvalidation orjoinhandle argument. Itis argumenttransport, not a participant-lifetime endpoint.

SelectedC0198 callback00709840, afterT25nonzero clearsnext-passflag, stillcalls007032C0 withcomputedT=I8, jumpslooptop andexecuteslocalcleanupbeforeRET. **Qualification of earlier“remainingTuse”:** thislaterpointerargument isnot a demonstratedTmemoryaccess; thecompletewrapper doesnotconsume itsstoredthis. The evidence is thatcallbackexecution hasnotyetreturned andthewaitingboundary remains, notthat itnecessarilydereferencesT afterthestopmarker. OriginalC0198 literalreachability/fieldbinding/order remainsVERIFIED; noentireclaim regrade orinventedrace. Externalworker invocations/after-callbackreturn/Tstoragecompletion remainUNKNOWN.

Sleepnormalreturn, T25marker andAL1rootprogress cannotjoin theheldcallback/currentI/U/T/worker generations tolock/member/storage retirement. No broaderno-join/no-callbackservice claim ismade aboutunexaminedupstream code orOS internals; the completehelper itselfsupplies onlyarithmetic/importcall/RET.

## First missing edge and source cap

Thefinitehelper/body/import/selectedarithmetic/callerABI question isresolvedatlocalstatic scope. Strongerretirementneeds **participant-specific same-generation admissionclose/detachment andlast-access/completion/join evidence** beforethealreadyacquiredretirementpaths. No participantidentity ispassedtoSleep here. Newtypedparticipantclosure source orcorrelatedtrace mustreopen it; nogenericwait/Sleep/API/thread/registry expansion.

OriginalInput/mode9, modelinvalidation/scene-vectorlastuse, Savecallbackdetachment, workercompletion, physicsSDKlastuser andall70+18guards stayopenunderoriginalcaps. K0022/C0047/C0086/H0209 disproof andK0023historicalqualifications unchanged.

## Primary verification / actual outcomes

- Freshsingle-assignmentOpus `scratch/phase5_progress_helper_branch_seq0082_*`:3code+5importdataspans, **27instructions/69codebytes+49importbytes**; no-writecheck-onlyreplayPASS, originalreceiptpreserved; NEWreplay `scratch/phase5_progress_helper_main_replay_seq0082.json` PASS.
- Personal `scripts/inspect_phase5_progress_helper_main.py`→`scratch/phase5_progress_helper_main_20261004.json`: **27/69**, arithmetic/ABI/PEimportindependentlychecked. Initialdescriptorassertioncomparedtuplewithlist andfailedbeforeoutput/promotion; correctedexpectedcontainertypeonly, successfulre-runPASS. No byte/semanticconflict orsourcechange.
- Personal `scripts/inspect_phase5_progress_callers_main.py`→`scratch/phase5_progress_callers_main_20261004.json`: **86/290**, carrierguard/call/uncheckedreturn, completeunusedthiswrapper andcallbacklatewait/returnsequence. Counts overlap/reuse, notadditivecoverage.

ASMhelper9359–9368/arithmetic950760–950773 and950800–950802; carrier830700–830749; wrapper872417–872429; callback880846–880865; guards3087–3099. ExactVA/line/bytes/sourcehashes governshortreferences. APIreference: https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-sleep (fetched2026-10-04; documentationnotruntime).

NoGOGhomologyestablishedfor00408AF0; no newGOG/Xbox/runtimepromotion. No suppliedevidence/binary/asset/production/runtimechanges, Phase4re-audit, Phase5closeout orPhase6.
