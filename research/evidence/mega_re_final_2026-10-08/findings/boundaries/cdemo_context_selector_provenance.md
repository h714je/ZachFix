# CDemo context-selector producer and selected exit

**Date:** 2026-10-03. **Frontier:** APPLICATION_MODE_RESIDUALS / MODE_CONTEXT_PROVENANCE. **Outcome:** ADVANCE, selected Steam primary scope. Claims C0137/C0138; boundaries BND-113–BND-115. Contract `scratch/phase3_mode_seq23_contract.md`.

## Novel source and scope

Exact008A6074 WRITE/READ_WRITE xrefs identify seven instruction sites:00424EB7,00424F62,00426831,00426A9F,00642FEF,00644480,0076ABAD. Their masks separate **bit30** OR/AND, three **bit31 clears**, one **bit31 set**, and static zero initialization. This is an exact provided-xref universe, not proof against all indirect/computed writes. Main investigates unique bit31 set and selected same-callback exit; bounded Opus thread verifies two independent reset clears. Accepted C0117 consumer split, H0202 initial-event infrastructure, H0217 XCA resource handoff, H0205 availability reset and C0123/C0125 helper types are reused only at their scopes. No old generic native seam, XCA grammar, CEvent/CMessage writer or raw00474830 incoming rescan.

## Typed roots and receiver split — VERIFIED

**008A6070 is an in-place CDemo object.** PE firstword is table0077140C; raw COL/TD names `.?AVCDemo@@`. Hence **008A6074 = CDemo+4**, not an independently allocated mode manager. Own one-slot deleting interface004275A0 uses this+7A0, count2, stride3C, then optional flag1 allocation free. This establishes **0x818 lower bound**, not a heap allocation size. Raw PE-initialized root and0076AB90 group initialize two3C records at008A6810 (=root+7A0), clear root+4, register0076DB20. The registered finalizer rewrites CDemo's vptr and performs the same embedded-vector cleanup; registration is not proof actual process-finalizer execution or heap freeing the static root.

**008A7218 is a separate in-place CDemoMovie**, PE table007717C4 -> `.?AVCDemoMovie@@`, ownslot00427A70. It is not CRdMovie cache00BD9E48 or the CDemo object. Selected default request path writes through+1F, giving0x20 observedlowerbound; full size/embedded layout/lifetime is unknown.

Raw **004288AF setsECX=008A6070** before004288B4->00427A90, although00428870's decompiler shape hides this receiver switch. Numeric request routing, not proximity, distinguishes scene/demo and movie roots. Selected request3 maps directly to0042889F; request5/12/17 pass a CGame field+8C5EC mask80000 guard before choosing that scene arm. Other numeric request arms are not assigned friendly content labels. A direct independent call at00445506/0044550B also uses CDemo as ECX; the larger00443060 caller's complete script/receiver semantics remain unvisited.

## Actual activation/task/event0 producer — VERIFIED mechanics

1. `00427A90` reads its actual CDemo **+4**, rejects when sign/bit31 is set, otherwise stores supplied numeric request at **+8**, writes **+C=1**, and selected bytes+770/+772/+773. These absolute fields correspond to former untyped008A6078/008A607C and byte roots008A67E0/2/3, not new independent managers.
2. It calls accepted rooted selector0 task factory **006C5930 at00427ADB**, passes returned EAX as ECX at00427AE0, then calls **006BAB80 at00427AE2**, with callback literal**004213D0** and auxiliary0. Reused H0202 setter emits initial event0 synchronously. The registered task is **not CDemo this**; it owns its native callback field while the callback accesses shared/static CDemo state.
3. Raw004213D0 prolog saves callback firststackargument (task) atstack+28; eventbyte0 branches **00421422->004268C0**. This is decisive primary flow, not a name-based match of two bodies. After selected CGame snapshots+834FE4/+834FE0/+834FE8 to absolute008A67B8/BC/C0, optional numericqueue handling and other setup, **00426A9F ORs CDemo+4 with80000000**.
4. Continuing setup resets private numeric state **008A670C (=CDemo+69C) to0 at00426BB7**, loads **ECX=CDemo at00426B6C**, and calls0041C2E0 at00426BC9. Its larger setup algorithm/weather/detail is not newly reopened. This gives an actual activation-to-state/setup continuation, not merely a getter called an initializer.

**STRONG_INFERENCE:** bit31 denotes a CDemo scene/demo activation/context condition: raw type, guarded request-to-task activation, resource/scene handoff and selected exit all reinforce that role. **UNKNOWN:** complete friendly menu/title/gameplay/pause/loading/cutscene policy; bit30 is separate and not the same flag; not every cutscene or media session is proven to set bit31.

## Selected event1 exit and retirement — VERIFIED conditional order

Raw00421435..00421443 dispatches event1 by numeric008A670C values0..6 through table00426DA4. **State6 ->0042644D**. On its selected continuing path, numeric requests5/25 first wait on0040B370; CGame receives the three earlier saved dwords at+834FE4/+834FE0/+834FE8. Additional object/world/presentation restoration is not named completely.

The path passes **ECX=013936F0 ->005DAA90 at00426818/1D**, then clears bit31 of both **CDemoMovie+4 (008A721C) and CDemo+4 (008A6074)** at0042682B/31. It reloads the original task pointer fromstack+28 and invokes **that task's vtable+30 at0042683C**. Accepted selector0 CRdObject interface supplies the retirement-marker implementation. **Clear-before-mark is proven; mark is not delete or free.** If008A6730 remains nonzero, the path conditionally creates a replacement selector0 task and installs the same004213D0 callback at00426882/89, which can synchronously enter event0 and set bit31 again. Therefore **a clear is not proof the final flag remains zero for the rest of the frame**.

## Existing consumer split / export limits

C0117's accepted frame/object/media spine reads this exact field through0041C270 and selects dispatcher/companion ordering plus later context selection. New producer/root identity connects that existing mechanical split to CDemo activation/selected exit; no accepted consumer-body rediscovery or GOG field transfer. Static callbacks can change the field; separate reads are not one atomic frame-wide mode snapshot, nor a cadence/first-observation measurement. Both selected returning context arms still reach the previously proved movie continuation; CDemo activation is not proof movie completion or a universal skip policy.

Exports split the selected callback's raw coverage:004213D0 prolog's event0 target004268C0 and the bit31 writer/selectedstate6 exit are reported as orphan instructions; exported00424F4D has disconnected recognized coverage including the common epilogue. `body_bytes=4387` is not a contiguous function length or full-CFG proof. Its entry00424F4D follows the returning CPhysicsCore getter call00424F48; this is a selected continuation fragment, not primary evidence of a standalone renderer function. New raw prolog/control edges justify this local export-scope annotation. No invented FUNCTION_LEDGER rows for raw004268C0/00426A9F, no overwritten supplied exports or accepted H0217 resource match; GOG beyond H0217 remains unvisited.

## Alternate numeric reset clears

Bounded thread `scratch/seq23_mode_clear_agent_findings.md` / `scratch/seq23_mode_clear_primary.json` verifies selected00642640 event1/private-state branches. **Main independently verifies**298 instructions/1,184 bytes and table/call/mask operands through `scratch/verify_seq23_mode_clear.py` / `scratch/seq23_mode_clear_main_verification.json`:

- Event1 compressed map/table reaches00642A42. Private014736D4 value1 maps00642EE8; local gates include004492C0 AL==0 and signed01474CEC>=5. The00469000 AL==0 arm writes committedD4=125; the other arm writes123. Both continuing arms reach00642FEF and clear both demo-root flags.
- PrivateD4 value103 maps006443D3. After004492C0 AL==0, acquired **CInput00404400 resultEAX->ECX at00644402 ->00708910** must returnnonzero, and01473730 must be nonzero for the selected00644480 clear arm. The query/channel/button algorithm remains unopened. It writes **pending014736D8=2**, not a direct committedD4=2 write. No later commit policy is inferred.
- Selected branches also invoke an acquired CGame helper; this does not type the central dispatcher's original firststackargument. Entry gates/preludes are not a complete sufficient-condition proof for reachability.

These are independent **clears of the same CDemo field**, not proof their untyped incoming receiver is CDemo or a universally named menu manager. Existing raw00474830/00474857 incoming provenance and H0205 availability source bounds remain unchanged; task availability is not reinterpreted as this context flag.

## Primary verification / first missing edges

Main `scripts/inspect_phase3_cdemo_context.py` / `scratch/phase3_cdemo_context_primary.json`: **626 instruction records /2,647 bytes**, two raw types/eight selectedE8 targets, primary root words/eventtable/bitmasks/receiver operands and orphan/export boundaries. These checks are not fullcallback semantics or a completeCFG. Exact main spans are retained in the JSON.

- Actual CDemo request/root switch: `inputs/decompiler/steam/DP_full.asm:44582-44605`, `:45509-45536`, `:79925-79934`; supporting `inputs/decompiler/steam/DP_decompiled.c:30180-30206`, `:30744-30819`. Decompiler alone misses the static root switch.
- Callback/event split: `inputs/decompiler/steam/DP_full.asm:38049-38080`; new raw event0 set and selectedexit at004268C0..00426BCE,0042644D..004268C0, with exactVA/lines in receipt. Eventstate table426DA4 readfromPE; seven providedxref writers independently distinguished.
- Static init/finalizer and owninterface: `inputs/decompiler/steam/DP_full.asm:991356-991365`, `:994038-994055`, `:44181-44197`; supporting `inputs/decompiler/steam/DP_decompiled.c:29825-29840`, `:30165-30179`. Rawtype table addresses/root words are primaryPE data.

**UNKNOWN:** fuller native-scene-demo state machine/asset/scene owner and loading gates; full event/script-loader/CEvent buffer provenance; complete movie request placement; CDemoMovie higher policies; exact00642640 incoming owner; runtime activation/cadence/clear-reentry observation; GOG newproducer scope and actual finalizer invocation. Newtyped CDemo acquisition/use reduces event/script organizing-root breadth but does not resolve the independently bounded CEvent stream-loader question. Keep unvisited continuation sources live rather than calling them globally bounded. No Phase3closeout/Phase4.
