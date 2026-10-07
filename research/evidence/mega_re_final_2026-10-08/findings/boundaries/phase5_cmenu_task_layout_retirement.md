# CMenu task retirement, distinct Fade retainer and inline-layout reset

**2026-10-04; Phase5 batch3; STEAM_PC.** C0186/C0187. Accepted C0142/C0143/C0165/C0166 task/controller/Fade interfaces and C0180 world filtering are anchors, not reconstructed.

## Contract and result

Selected `P5_PRESENTATION_TASK_RETIREMENT`: actual world→CMenu lifecycle helper, installed task's local event2 arm and the specific controller's ordinary teardown. Exclude full UI/input/Fade algorithms, optional remap lane, generic child/free/offset and native-task seam scans.

**VERIFIED:** the world call resets an embedded CLayout, not the task or whole controller; the typed task-compatible marker can deliver event2 to an installed callback whose explicit local effect is a flag mask on a **separately acquired CFadeManager+8 retainer**. An available controller destructor invokes the same CLayout reset via its ordinary destructor. None of these operations is task deletion, controller allocation free or a release of the Fade pointee.

**UNKNOWN:** actual task/marker occurrence, current vptr/callback preservation, Fade retainer generation/lifetime, controller-active flag chronology and later manager deleting/last-use join. This result supplies positive local integration, not safe presentation shutdown.

## Actual world-to-embedded reset — VERIFIED

Selected world state6 sets ECX=CMenu01476978 at005D44C2 and calls005FF350 at005D44C7. The complete helper adds **6EE8 to ECX** and tail-jumps00458990. Thus reset receiver is **U+6EE8**, not originalU or the registered CRdObjectModel taskT.

Specific typed construction evidence:00654856 supplies ECX=U+6EE8 to004592B0. That constructor writes table0077294C (rawRTTI **CLayout**) and calls00458990. Specific available CMenu destruction:0065490B supplies the same inline address to004592C0, which writes the same class table and tail-jumps00458990. These are narrow receiver/type/lifetime discriminators, not a repeated full constructor reconstruction.

The entire00458990 body clears byte2C/2D, DWORDs34..90 and selected numeric fields; writes its explicit float defaults, including values computed from separate globals. It has **no CALL, deallocation, old-field load for release, vtable mutation or destructor dispatch**. Field kinds beyond the literal writes are UNKNOWN. In particular, zeroing34..90 is not proof those are owning pointers, that old pointees were released, or that all helpers/arrays/controller state were destroyed.

Available ordinary CMenu destructor006548C0 invokes numerous embedded teardown operations; this report interprets only the specific6EE8 CLayout call. Registered static finalizer dispatch and controller deleting-slot invocation remain at their accepted availability/UNKNOWN scope. The world reset call is not that full destructor.

## Task marker and local event2 consumer — VERIFIED interfaces, conditional composition

Accepted request006545B0 creates/registers a typed selector1 CRdObjectModel T and installs00654490. It explicitly writes **wordT+2C=8** (00654661/66). C0180's state6 filter includes8. This gives a concrete category-compatible created task, **not proof that the same current task still has8 and is traversed in a later state6 occurrence**.

Accepted event1 adapter passes taskT separately to CMenu006516E0. Its selected state101 arm loads T's **current** virtual30 and invokes it at00653194. The named CRdObjectModel table0076E704 has30=006BAB20. If that current table still supplies the marker, the marker updates retirement fields and on its first-marker arm calls00402870(T,2,0); it does not call task destructor/free itself.

**Opaque break retained:**00402870 invokes optional global callback0148132C before reloading T+44. If T+44 still equals the installed00654490 and T remains live, the local callback observes lowbyte event2. Raw byte map0065459A=2 and target word00654584=00654556 prove the event2 arm:

1. call00427780 (accepted typed CFadeManager acquisition);
2. load **F2=[returned_manager+8]**, not+4;
3. `AND DWORD[F2+168],FFFFFFFA` at0065455E;
4. return.

Under valid live pointers/normal call behavior this clears mask bits **1 and4** while retaining the other bits. It does not locally clear CMenu+A6, unlink T, release a handle, call either T/F2 deleting interface, null a manager pointer or free storage. It is a concrete **task-retirement notification→effects state** boundary; it is not proof of those stronger consequences.

## Retainer/generation distinctions

Accepted CFadeManager initialization stores typed CFade pointers in+4/+8/+C. The new event2 route uses+8; accepted menu request3/predicate7 and draw snapshot use+4. **+8≠+4 as storage locations** does not prove unequal pointer values, exclusive per-layer ownership or same-instance chronology. The getter result and F2 are new observations after the optional global hook/earlier opaque calls. No producer/consumer generation is merged by manager class/name/adjacency.

The accepted active-manager retirement method supplies conditional later cleanup/current-handle release/deleting interfaces, separate from006BAB20 marking. Static frame placement is retained, but this branch does not match its concrete selected task generation or prove task/Fade/controller last use before deletion. A normal event2 return is not successful whole presentation integration.

## First missing edges / global successor

- Concrete generation and preserved current vptr/callback from request-installed T to the marker and local event2 delivery, including the optional global-hook bridge.
- A matching admission/retirement/current-handle/manager deleting occurrence, plus controller-active flag and remaining callers' final use.
- Actual CFadeManager+8 population/current pointer lifetime and its relationship to+4 routes; no assumed F2=F99/F100 or opposite equality.
- Any stronger interpretation of CLayout cleared fields as owned resources needs independent acquire/release/consumer evidence, not a field-offset analogy.
- Runtime cadence, actual static finalizer dispatch and GOG/other task policies remain UNKNOWN.

Stop this finite lifecycle slice, preserve those first edges, and return globally to the independently qualified physics dependency-release frontier. Do not expand the full menu state machine or chase generic finalizer/child/free code to manufacture safe ownership.

## Primary verification

`scripts/inspect_phase5_menu_task_lifetime.py` → `scratch/phase5_menu_task_lifetime_main_20261004.json`:**131 instructions/429 bytes**,10 receiver/type/mask anchors, exact event2 map/target, namedCLayout RTTI and CRdObjectModel marker table. PE/ASM bytes agree; some marker/request instructions are exact accepted-scope reuse, not additive novel coverage.

SteamASM anchors: world typedcall525238–525239; helper573374–573375; reset102318–102362; constructor/destructor103120–103130; specificinline constructor665519–665521/destructor665558–665560; taskcategory665399–665400/callback665418–665420; state101marker664149–664152; event2localconsumer665344–665347; marker780914–780941; global/stored callback ordering1854–1879. Exact JSON VA/line/bytes govern locators if prose range shorthand differs.

No supplied evidence, production source, runtime operation, hook or patch changed. Phase4 accepted scope remains closed; noPhase6.
