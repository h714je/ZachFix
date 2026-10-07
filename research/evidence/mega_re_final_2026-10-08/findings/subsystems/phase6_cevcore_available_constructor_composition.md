# CEvCore available constructor and inline-init boundary

## Scope / evidence

**VERIFIED** Steam0070D150 [0070D150,0070D304),106instructions/436bytes, PE/ASM exact and metadata geometryMATCH: scratch/phase6_main_core_ctor_personal_seq0165.json. Direct installedtableRTTI/basegraph: scratch/phase6_main_core_ctor_rtti_seq0165.json. No caller/callee/helper bodies, exception handlers or runtime trace acquired. Accepted00408960 wrapper/getter relationship is reused at its existing scope.

## Local mechanics

**VERIFIED** savedincomingECX=R. First R+4->opaque0040D380, then [R]=008276A4; rawinstalledtable declaresCEvCore0/CEvManage4. Normalpath subsequently submits receiverinteriors:

| Offset | Opaque target |
|---|---|
|44|00710360|
|424|007103D0|
|528|00710440|
|7B4|007105B0|
|A08/A48/A5C/A6C/A7C/A8C|004DB2F0|
|12BC/12C0/12C4|00402100|

**VERIFIED** directnormalpathstores: bytesAA8/AA9=0, floatAAC=0, DWORD AA4/12B8/24=0, AC0=-1. Then R->0070D6F0 beforeR->0070E4C0. On normalreturn, fourDWORDs atA6C..A78 andfour atA7C..A88 arezeroed, EAX=R returned. Own directwriteendpoint12BC lowerbound is notsizeof/allocation extent; opaquehelper offsets do not establish their pointee types or complete regions. Exception/unwind/free effects UNKNOWN.

**STRONG_INFERENCE** availableCEvCore constructor/inline-composition surface: savedR, tableinstallation, repeatedinterior initialization interfaces andreturnR. CSingleton<CEvCore>'s accepted getter calls thisinitializer beforeitswrappertableinstall. Neither actualcurrentcache identity/type nor ownedcomposition follows.

## Architectural effect

REFINE existingCore class/root ratherthannewmanager/service/factory/dispatcher. Rawbasegraph isdirectly corroborated atbaseinitializer ratherthanonlywrapperRTTI. Aftertheinitial+4base request, theinterface exposes13distinctinterior callstartsoffsets; it doesnotname13classes or infer their sizes. Accepted32/80/384 publication and247/199 recipes remainqualifiedconditionalinterfaces. Thisconstructor supplies no own directassignment to+3C/+12B4 and no currentgeneration/typedcohort-content join.

## First missing edges

Typedmeaning of44/424/528/7B4 regions requires a direct qualifiedinitializer/table/consumer edge; firstopaque00710360 is a possible futurefinitequestion, notautomaticdepthselection. Firstsourcefor+3C/+12B4 state is alsoopaque; later0070D6F0/0070E4C0 may alterstate butareunopened. Actualconfiguration-to-query receiveridentity/preservation remainsUNKNOWN and mayrequire runtimeevidence. Do notfollowthisinitializer chain simplytoavoidconvergence.

All88guards,155/158history andBND244unpromoted preserved. NoPhase7.
