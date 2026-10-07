# Independent rank51: ItemManager adapter result to opaque receiver request

Main qualification; canonical adoption separately recorded. This is raw/static transport, not a new typed actor/item class.

## Exact primary scope

STEAM_PC FUN_00585820[00585820,00586137),672 instructions/2327 bytes. Contract scratch/phase6_seq0197_rank51_qualification_input.json; source scratch/phase6_seq0197_rank51_owned_primary.json. Fresh main scratch/phase6_main_retained_replay_seq0200.json matches every selected instruction against PE/exact ASM line and all previously selected data spans against PE. No0058D0E0 body, factory-selector bodies, current0148132C target source or caller body selected. Accepted C0311 adapter scope is reused from findings/subsystems/phase6_item_cache_factory_callback_adapter.md without reopening its84/242 body.

## VERIFIED local mechanics

00585827 saves ownECX asEBP.00585837 loads the first incoming DWORD stack operand intoEBX under ordinary stack ABI after theA8 local reservation/EBP/EBX saves. Locally that value is used as pointerP, while currentEBP isR. Neither receives a primary class name from these mechanics.

Own footprint:79 actualCALLs=66 directE8/31 targets+13 indirectCALLs andone indexed JMP. The five-entry table[00586138,0058614C) is independently selected by signedWORD→ESI, ESI+5, unsigned<=4 gating at00585D36..41. Table selection is numeric, not enum/type proof.

00585AE8 MOV AX,P44 and00585AEC SUB word R4FA,AX affect16bits only;00585AF3 next loads fullEAX.00585D7E sign-extends returnedAX intoESI.00585F51/58 compares signed16-bit R4FA/R4FE andJG selects a later edge. UpperEAX residue is not treated as preserved full input/HRESULT/value.

Three current00BDA0F8 guarded recipes request590 through00404390, write literal007702C4 to a nonzero allocation result, otherwise form0, then publish. Reused available CSingleton<CItemManager> table identity applies to the auxiliary operand, not incomingR/P, exclusive ownership or complete service role.

### Selected result-consumer interface

00585E20..50 independently obtains the current auxiliary receiverP-or0 and loads currentR3C intoESI.00585E50..65 pushes, in chronological order:

`-1, currentR3C, 822, &currentR78, &currentR58, 2DE`

Then it calls accepted00457E70. At that call, first through sixth raw stack words are `{2DE,&R58,&R78,822,R3C,-1}` under ordinary stack ABI. C0311 admits numeric2DE and supplies the known raw5A factory/callback request route; that is not a primary concrete5A output-class mapping or success/ownership proof.

00585E6A..6C tests returnedQ for zero. Nonzero path pushes0,0,1,0,ESI, setsECX=Q, andcalls0058D0E0 at00585E79. ESI was loaded by LEA at00585E5C, so the operand is **addressR58**, not a load of[R58]; continuity across00457E70 is explicitly ordinary nonvolatile/stack-ABI conditional unless independently proven. The selected first through fifth consumer operands are `{&R58,0,1,0,0}` under that qualification.

A separate branch transports00457D30-derived rawN into00457E70 and joins the consumer with lastflag1 rather than0. It must not be silently collapsed into the literal2DE case. Other selected006B2C70 results are pushed to006BE6E0 on currentR, retaining positive resource-request transport without proving completed actor/render effect.

## STRONG_INFERENCE architecture and disposition

Anonymous receiver/argument control invokes the existing ItemManager numeric shared-factory adapter and conditionally supplies its returned bits as an opaque request receiver. **B REFINE / C known-local COLLAPSE** of the selected independent new-factory/manager counterfactual. It positively places a consumer after the existing factory/callback seam, without a new typed actor/resource boundary, named incomingR/P/Q or ownership topology.

## First missing edge and readiness

IncomingR/P producer/type and0058D0E0 receiver/operand meaning remain UNKNOWN. A specific typed-output, capture, integration or lifetime map dependency could justify a bounded nextbody; none is established by this anonymous static interface. These optional static details are not classified runtime-only, and no observed genuine finite readiness E remains at this selected scope. Current0148132C targets, actualcohort/generation, synchronization, success, safe retirement, lastuse, cadence and GOG/Xbox correspondence remain carried limits.

Canonical synthesis alone may credit convergence. NOT_READY168 is unchanged by this report. No unselected198metadata/184rows/allocator-envelope bytes used.
