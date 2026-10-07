# CCar retained result: selected CGame interior address, not returned vehicle actor

**Phase5 Steam sequence102 compactproducerqualification.** AcceptedC0127/H0269 typedCCar005F5DD0 use is retainedonly; new00452270/0070E0B0/005E6E10 returnlineage. This isnotnewsubstantialfullvehicle-lifetimeportfolio, actorownership, lastuse/free orall-buildclosure.

## Personal producer and retained caller — VERIFIED

Retained005F5DD0 savesCCar, acquires acceptedCGameG, passesECX=G/stack0 to00452270, stores returnedEAX into sameCar+73C. H0269 onlypairedretained-use scope; no newGOGbody/type/fieldsemantic transfer.

Complete00452270 savesincomingG inEDI, begins n=0 inESI. Each candidateiteration independently calls00405360 then00408960; only the latterreturn is explicitly supplied as ECX to0070E0B0 withnumericarguments48/n. The formeropaquegetter/call mayhavesideeffects; it isnot itself thereturnedretainedvalue. Getter/currenttype/table/generation remain **UNKNOWN** atthisnewcaller; no sourcewidening.

Nonzero firstpredicate result admits005E6E10 withfixedECX=0143C078 andn; nonzero secondresult admits success. Failureincrementsn untilsignedn>=128, thenreturns0. Humanmeaning of48/indices/predicates/selection/fullreachability unknown.

On success, optional nonzerooutput-pointerarg gets n at004522B5;004522B7 returns **LEA EAX=[savedG+4*n+CD368]**, not a getterresult, currentvirtualreturn, loadedpointee, handle ornewallocation. AcceptedCar caller's0arg doesnotrequestthatindexwrite. Thus **VERIFIED returnedaddress provenance** is an interiorGamecell selectedby n0..127 undernormal ABI-preservedregister return. The producer doesnotdereference itsreturnedcell oridentifythestoredDWORDtype. Aninline128DWORDaddressedregion isaddressgeometry, notfullrecord/schema/classownershipproof.

CGameGgeneration/currentcellcontents afteropaque predicates, futureCar+73Cuse/clear/free/borrower equality/lastuse remain **UNKNOWN**. Theacceptedtaskclear ofCar73C clearsaretainer, notGame/actor destruction. No CCar→CObjectCar owningalias is supplied by thisproducer. This isnot a claimthereisno suchrelationshipelsewhere.

## Two leaf predicate interfaces

**VERIFIED complete0070E0B0 mechanics:** saves incomingreceiver U locally. Forwards bothfullDWORD arguments to opaque0070DFE0 with ECX=U, saves returned Q, then restores the savedreceiver argument for opaque0070FE90(Q,0). The finalreturn forwards that secondcall's EAX unmodified throughRET8. Q/finalreturntype, implementationmeaning, currentU/table/generation and successfulpredicate semantics are **UNKNOWN**; no helperbody was opened.

**VERIFIED complete005E6E10 mechanics:** overwritesECX with its first stackDWORD n, zeroesEAX, compares byte[008BF560+n] with1, SETZ AL, RET4. IncomingECX—including the producer's apparent0143C078 receiver—is **unused** in this completeinterface. Thus the secondqualification is a receiverless global-byte equalitypredicate, not a newlyproved contextobject operation. Outputis32-bit0/1 fromthelocalcompare; no indexbounds guard exists inthisleaf. Currenttablebyte semantics/generation andhowthey relate tophysicalactor remain **UNKNOWN**.

Personalnewthreebodies total56records/150bytes, within80/256cap. Freshleafbranch finalrawPE/ASM receipt andpersonal24/67 checks agree. **PERSONAL_CCAR_RESULT_VERIFICATION_COMPLETE:** importantleafreceiver/argument/return edges personallycheckedagainstPE/ASMbeforecanonicalpromotion. Aninitialgenericcross-receipt walker incorrectlyassumedidenticalJSONschema andfailed; the shellcontinuedrecording despiteintendedset-e, so cross-receipt agreement wasnotPASSatpromotion. Minimalexplicitschema normalization afterward nowconfirms exact24record/67byteagreement in `scratch/phase5_ccar_receipt_schema_comparison_repair_seq0102.json`. Originalreceipts/bytes unchanged; no newsemanticcredit. This chronology supersedes the earlier premature agreement wording andisretained as acontrol-order failure. No newgetter/helper/context-table body orunusedprefix.

## First missing edges / stop

**UNKNOWN:** current00408960receiver/type/vptr/generation andopaque0070DFE0/0070FE90 returntype/meaning; byte-table008BF560currentgeneration/semantics; inputindex validity beyondproducer'slocal0..127; Gsaved generation acrossopaque calls; returnedGamecell contents/class/lastuser; exactGame/CCarborrowing lifetime andsafe root/memberretirement. Apparent0143C078 contextidentity is not a prerequisite for this secondpredicate because complete005E6E10 ignores incomingthis; do not reintroduce it as an actualreceiver or tablebase. InternalGameaddress retention isnotexclusiveownership/keepalive/referencecounting orphysicalactorproof.

Stopfirsttype/currenttable/generation/lastuse edge, no getters/eventparser/nativecallbacks/vehiclealgorithms/physics/factory/handle/free/registry widening. AnewtypedGameactualcleanup/borrowertransport source isneeded forstrongerlifetime.

## Primary checks

Main `scripts/inspect_phase5_ccar_result_main_seq0102.py` -> `scratch/phase5_ccar_result_main_seq0102.json`:32records/83bytes completeproducer; acceptedCarcaller9/25 retainedfrom `scratch/phase3_vehicle_primary.json`, notnewcoverage. Exactcalls/VA/ASM/sourcehashes inreceipt. No establishedhomologyfornewthreeproducerentries; H0269reusedonlyexistingoutercaller scope.

Allseq83–101corrections/70+18guards survive; noinputs/production/runtime changes orPhase6.

Finalseq102classificationcorrection:005E6E10 incomingthisunused doesnotjustifyGAME_WORLD_STREAMING/subsystemassignment; nowDATA_OR_INIT_HELPER/UNKNOWNdomain, exactmechanicsV. BND-218internalVEHICLEselection notWORLDcross-boundary; BND-217event-facingdomainSI whiledirecttransportV. No newcanonicalDISPROVENclaim, no newprimaryacquisition; taxonomyoverclaim corrected ratherthanvalidatedaway.
