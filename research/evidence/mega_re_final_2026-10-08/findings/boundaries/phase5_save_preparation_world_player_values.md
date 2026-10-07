# Save preparation: live Game/world/Player value transport, not stable snapshot

**Phase5 Steam, sequence100.** Cold actual preparation hook previously opaque at C0169/C0170. New004524C0/00451450 only; accepted builder/current-G/currenthandle/type records reused. No save gate/schema/worker/finalizer/event/registry/free/handle-generation body or source reopening.

## Personal preparation entry qualification — VERIFIED

Accepted builder acquires G1 via0040A320 at006ACB0E, setsECX=G1 and calls004524C0 at006ACB15. After return the builder independently reacquires G2 at006ACB1A for the live-record memcpy; G1=G2/samegeneration andstagingcontinuity are **UNKNOWN**.

Complete004524C0 loads full-DWORD first stackargument K inESI. K=3 or4 skips its body toRET0C. Otherwise it pushes original third/second argument values in order at004524D7/D8 and calls00451450 at004524D9 with incomingECX **unchanged into that call**. It does not save G across that call, and later calls independently acquired receivers via00449480/00405360/004AD980. Those bodies/identities are excluded here; availability/call order is not synchronization or commonreceiver/lifetimeproof. K humanmeaning andfullactivation remain **UNKNOWN**.

The already-retained builder guard samples Game live flags in separate acquired occurrences: bit4000 selectsK=3; otherwise a fresh bit80000 sample selectsK=4 or the zero EBX operand. Both nonzero selected modes3/4 therefore bypass this newly qualified first subcallee. The zero-mode preparation and the later record copy are conditional; no friendly mode naming or cross-sample Game equality is implied.

This positively types the selected firstsubcallee receiver as the builder-supplied CGame atentry, but later root/call/generation stability andguards remain independent. The wrapper is not a mutex/admission/transaction barrier merely because save staging follows it.

## World/Player value preparation

**VERIFIED complete00451450 mechanics (personalcheck after independent preliminaryhandback; finalbranchreceipt nowchecked):** captures incomingG inESI at00451451. If liveflags+8C5EC mask04000000 isclear, copies lowbyte values from runtimeG+834FE4/+834FE0/+834FEC/+834FF8 into inline live-record99040..99043. Otherwise clears thatmask and writes99042=FF; the other three bytes are not written in that arm. These operations are not proved to be the full inverse of accepted record reconstruction or complete schema.

First optional pointerargument X1 is used directly when nonzero. If zero, fresh global008A9BA4 handle/current00BD7670 receiver are supplied to accepted006C5FD0 at004514BA; returnedR1 is adjusted+58 without a local null guard. Four DWORDs are copied from selected X1/R1+58 into inlineG+99048..99054. Second pointerargument X2 is independent: zero invokes a **new** handle/root lookup at004514F9; R2+78 supplies another four DWORDs to inlineG+99058..99064. R1=R2, freshhandle equality/currentmanager generation and currentclass/field freshness are **UNKNOWN**; acceptedPlayer publication/type does not establish each later lookup's actualgeneration. Nonzeroarguments bypass lookup entirely. These are inline values, not retained dynamically owned pointer containers or new owning references.

00451524 zeros DWORDG+99068. The call-free final loop inspects64 current pointer-valued CMapretainedslots01437688..01437784, four periteration, and ORs a rotating ECX one-bit into **one32-bit DWORD** for each nonzero observedslot. ROL wraps every32bits, so positions i and i+32 contribute the same bit; this is **not**64 independently represented membershipbits. Bit schema/meaning, coherentall-slot sampling and level/resourcetype generation/lastuse remain **UNKNOWN**. No slot/pointee is cleared/freed/accessed beyond nonzero-slot tests in this loop.

On ABI-conforming normal completion EAX is the final traversaladdress0143778C andECX=1; these are not snapshot/write/release success. LeafRET8 consumes its twoarguments. The wrapper's later independentcallee calls are stillopaque; preparation does not seal subsequentstaging generation oradmission.

Personalnew sources total96records/372bytes overtwo completebodies, within128/512cap; independentleaf73/301 overlaps ratherthanaddingcoverage. No additionalidentity/body orprefix overread.

## First missing edges

**UNKNOWN:** K admission/actual selected occurrence; original G1→currentG2/staginglastwriter; sourceworldslot/handle/currentreturnedclass/valuegenerations; cross-helper/currentdestination continuity; stable all-field snapshot; successful activation/write; Player/world lifetime/lastuse. IncomingECX/sourcecopy/localreturn do not supply synchronization/quiescence orretention/free.

## Primary checks / preservation

Main `scripts/inspect_phase5_save_preparation_main_seq0100.py` -> `scratch/phase5_save_preparation_main_seq0100.json`:23records/71bytes, completewrapper; selectedbuilderrecords retained from `scratch/seq47_save_main_receipt.json` only, notnewcoverage. Freshsingle-assignmentleafbranch separate; no establishedSteam/GOGhomologyfornewentries, no transfer.

Allseq83–99corrections/70+18guards preserved. No inputs/production/runtime modifications orPhase6.

**PERSONAL_SAVE_WORLD_VERIFICATION_COMPLETE:** finalindependentbranch/personal73exactrecords agree; wrapper23/71 entry/argument/order andleaf73/301 field/copy/ROL/return edges personallychecked. Newunique96/372/twoidentities, no additivebranch/objdumpcredit. Branchfirstreplay blankcaller_entry metadatahex-parse failure repairedbyexacttargetstringfilter; finalcheckerEXIT0, no evidence/semanticrepair. ExactleafASM93339–93411; thirdruntimebyte source834FEC not834FE8. Finalbranch scratch/phase5_save_world_branch_seq0100.{py,json,md}; personal scratch/phase5_save_world_personal_seq0100.json; originalcaller/zero seed retainedonly.
