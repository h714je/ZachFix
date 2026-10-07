# Event-associated numeric transition/reset organizer — Steam Phase6

## Primary scope and role

00446AF0 own1524byte export only, plus own literal10-entry switch and three installedtable RTTI chains. Independent412instructions/1524bytes/72directsites/26targets reproduced personally. No incoming caller/supportcallee/slot bodies or runtime/GOG evidence.

**VERIFIED:** saves incomingECX=R inESI; one DWORD selector S, unsignedS>9 bypasses table; ten numeric entries at004470E4;RET4. All visible returns clear R+4 bit20000. Own R-addressed storage reachesDWORD+3B94, so write extent≥3B98, not sizeof/allocation. No own R+0 vptr access or deliberately constructed-R return. It is not the primary constructor/factory of incomingR.

**STRONG_INFERENCE:** architecture-sensitive event-associated numeric transition/reset organizer. It links receiver resets, cached event/camera/core paths, accepted event-core/game/fade interfaces and object lookup. **HYPOTHESIS incomingR=CEvent-compatible**, because extent matches separate3B9Crequest and eventRTTI hasCEvManage+4; this is not receiver provenance. R may still be a distinct compatible organizer. Numeric mode/lifecycle names and actual invocation UNKNOWN.

## Selected own architecture interfaces — VERIFIED local control/data

Selectors0/3 reset R+34/+40;3 zeros/sets numerous fields through+3B94 and resets game-derived/core/camera/event values. Selectors2/4/8 common-exit only. Selectors5/7/9 acquire cache00BD9E40, supply its selected pointer witharg1 to006CCD00, edit event-core/receiverflags;7skips finalfade request. Selector6 combines registry lookup/predicate, event/gameflags/parameters, receiverrequests and conditionalfade/forwarding calls. Full gameplay policy/opaque calleeeffects remainUNKNOWN.

Own cold cached objects are distinct fromR: CEvent00BD9E58 uses3B9Crequest/opaque0042D5B0/table0076E858; CCamera00BE1EA4 uses1ACrequest/table007721BC; CRdCore00BD9E40 uses13Crequest/table0076E828. Tables are installed on acquired pointers, notR. Their rawRTTI confirms named singleton wrappers and declared bases: CEvent+0/CEvManage+4;CCamera+0;CRdCore+0. This is static construction/table data, not non-null cachecurrenttype/ownership/generation proof.

Acceptedgetter outputs are reused only at their previous scopes:00408960 pointer's+4flags/+52C/+424/+7B4 interfaces;0040A320 CGame member accesses;00427780 fade-manager retained+4/+8 pointers→00449A00 requests; active-manager current00BD7670 and identifier008A9BA4→006C5FD0 lookup. Four-or-more reacquisitions do not imply same generation. Stackargument partition across lookup/predicate and helper calls remains explicitly unresolved.

## Raw/decompiler corrections

**VERIFIED:**00446B8B/00446B98 access `[EAX+834FE0]`, not absolute00834FE0. The decompiler/xref PTR_LAB label is an offset-shaped locator, not global-table proof.00446DA7–DB4 prepares **one** float1.0 before00449480 then uses returnedECX for00470420; the decompiler's two apparent floatarguments are not accepted. Which callee consumes the stackslot isUNKNOWN. Neither correction silently edits old supplied evidence.

## Missing join and finite successor

First map-changing edge: actual incomingECX→this organizer, particularly whether it comes from availableCEvent00BD9E58. Metadata gives11incoming sites/sevencallers but no byte provenance. Next finite discriminator is **incoming00452928 in unclassified00452780**, backward ECX/selector slice only, maximum128bytes immediately preceding thecall pluscall instruction; no fullcaller/callee acquisition. If getter/definition is outside that bound, retain exact unqualified edge rather than widening.

A successful static typed join could assign class ownership and major game→event transition boundary. It would still not establish live mode cadence, receiver generation, unique owner/free or runtime chronology. All88priorguards persist. Primary callees remain independent mechanisms, not automatically renamed asRmethods.

Evidence: scratch/phase6_00446af0_qualification.md/.json; scratch/phase6_00446af0_personal_verification_seq0135.json; inputs/decompiler/steam/DP_decompiled.c:53799-54092. Main personally verified owninstructions/tables/switch/calltargets and raw correction sites before any canonical promotion.
