# Physics scene/controller dependency release and generation barriers

**2026-10-04; Phase5 batch5; STEAM_PC.** C0190–C0192. Accepted C0121 typed core/invoked session cleanup and C0149–C0151 context/vector/null mechanics remain anchors. New callee/ABI/retainer interfaces, not a subsystem or middleware reconstruction.

## Contract and outcome

Selected `P5_PHYSICS_SCENE_DEPENDENCY_RELEASE`: concrete core-slot/provider/removal/controller wrapper/external-release route. Fresh bounded branch was read, replayed into a separate primary receipt and personally verified at important assembly/PE joins.

**VERIFIED:** current core scene-like pointers are invalidated in typed context records, supplied as arguments to a current sharedSDK virtual14, then their physical slots are cleared. Earlier cleanup can publish a provider result into the same slot family. Final controller support is purged, reloaded, conditionally sent through an ordinary destruction route that invokes **NxReleaseControllerManager** on its stored external manager, conditionally invokes allocator deletion, and invokes a deallocation boundary on the saved local wrapper.

**STRONG_INFERENCE:** scene/controller release/purge meanings of unnamed numeric SDK/manager virtual operations. **UNKNOWN:** actual external success/destruction, same generation across opaque calls/reacquisition, admission closure, worker/actor last use, SDK last-user/free and exclusive ownership. These cannot be inferred from pointer clear or the named release import.

## Identity and reference vocabulary

C is the acquired CPhysicsCore passed to006F9040. S[i] is C+50+4*i, twenty retained pointer slots; G is current01493FA8, R is currentC+11C. Their accepted startup alias does not prove current equality.

W is the 0x18 producer-connected support wrapper retained atC+67CD4, **not** the external controller manager. M=W[0] is the result retained from namedNxCreateControllerManager; A=W[4] is its allocator dependency. Q[j] is a retained pointer atC+67CD8+4*j. W/M/A/Q/S/G/R are distinct domains; reacquisitions receive distinct suffixes.

## Scene-slot invalidation, opaque call and clear — VERIFIED

NonzeroG gates the entire006F9040 method; zeroG skips controller support cleanup too. After earlier dependent collection work and controller-slot dispatches, unsigned-byte i0..19 selects S[i]:

1. For a nonnull slot, load S1 and pass its **value** as one cdecl argument to0040B700 (006F92FF/9303/9304; caller pops at9309).
2. Exact accepted helper nulls the first matching record pointer in each typed context, without count decrement, pointee free, callee, wait or join. This helper does not replace the core slot; no opaque selected-code barrier is invented there.
3. Reload S2 from the same physical core slot (006F9313/17). Load currentG as table/ECX receiver and invoke **G.virtual14(S2)** at006F9328, not a call on C, R or S2.
4. Store0 to S[i] at006F9331 after the opaque SDK call, including originally null slots. No post-call slot reread, equality/generation or result-status test precedes that clear.

Physical-slot equality is positive; cleared-generation equality and external release success are not. Nulling a context entry does not end the lifetime of a worker's already loaded local receiver. Actual worker-start incoming and runtime debt/cadence sources remain at their original bounds.

## New provider/publication and selector breaks — VERIFIED interfaces

Concrete cleanup callees006F90B4/9158/920D reach006FA480 on C. The selected mapped-removal helper006F9500 also reaches that provider. For unsigned-byte k, the provider tests S[k]; null admits a currentG.virtual10 call with an address of a prepared stack descriptor, then **stores returnedEAX into S[k] at006FA537 without a success/null test or rechecking the original null observation**. Descriptor/configuration internals are not interpreted. Nonzero second argument separately publishes k via a freshly reacquired selector-address result.

The final twenty-slot cleanup therefore does not prove it only releases entry-retained generations: it reads after provider publication. Successful new scene creation is UNKNOWN.

The cleanup captures selectorbyte b0 once near its entry. Some later collection virtual28/20 calls use S[b0] despite intervening provider selector publications. The selected006F9500 additionally maps caller ordinal i throughC+37DC[i] before choosingC+12C[mapped], then reacquires its selector. Direct/mapped dependent identity and current selector/scene-generation equality are not supplied by adjacency. No full actor/scene policy or physical SDK method names are promoted from those offsets.

## Controller support: actual named release versus retainer clears — VERIFIED

The selected producer supplies allocatorA0 to **NxCreateControllerManager** (IAT0076E1AC), stores unvalidated returnMcreated atW+0 and allocator atW+4, and retainsW inC+67CD4. This is a narrow field-identity recheck, not a startup reconstruction.

For selected nonnull Q[j], cleanup passes currentW and Q's value through006FE8D0 to **M.virtual0C(Q)**, then clears Q[j] without comparing post-call contents/status. The complete forwarder has no local result gate. Its inherited Concurrency/FID label does not establish semantics or exclusive library origin for this reached use.

Final support order:

1. Nonnull **W0=[C+67CD4]** reaches006FE900→**M0.virtual10()**, zero explicit arguments.
2. After the opaque purge, reload **W1=[C+67CD4]** at006F94B6. Test nonnull, **not W1==W0**.
3. Call006FB020(W1,flag1) at006F94D0. It saves W1, calls006FE800, then conditionally passes that same savedW1 to accepted deallocation endpoint0074E82B; returnsW1, not a success flag.
4. In006FE800, first reset the local container end atW1+8; then load **M2=W1[0]** and invoke **NxCharacter.dll!NxReleaseControllerManager(M2)** at006FE840/IAT0076E1A8. Argument is the stored manager pointer value, notW1/A/C/G/R or a slot address. No local external-success test or W1[0] zero is supplied.
5. **After** named release, reload **A1=W1[4]**; if nonnull invoke its currentvirtual1C withflag1. Then localW1+8 storage teardown/reset runs; its range helper has no pointee accesses/calls and is not additional controller destruction.
6. After normal wrapper destruction, flag1 selects the same-W1 deallocation boundary at006FB03B.
7. Core cleanup finally zerosC+67CD4 at006F94E4, without a post-call current-occupant equality test.

This establishes **actual static external-release and local-deallocator invocation**, stronger than mere available deleting code or pointer clear. It does not prove W0=W1, M0=M2, A0=A1, successful operations, absence of borrowers/callbacks, or safe last use. The wrapper's nominal C++ type remains UNKNOWN.

## Allocator identity and SDK lifetime limit

**VERIFIED:** allocator constructor006FE940 installs00827034; rawRTTI names **ControllerManagerAllocator**, with zero-offsetNxUserAllocator base. Installedtable+1C is006FF2C0, whose available flag1 path reaches006FEAA0 and0074E82B. This is an installed-table target, **not a proof that the post-import A1 still has that table or equalsA0**. Do not use allocatorRTTI to name W.

After all scene-slot clears, later conditional record operations still invoke **currentR=C+11C virtual24(pointer)**. The scene loop is not the last available SDK use in this method. Full006F9040 has only loads/comparison forG/R, no explicit zero store to01493FA8/C+11C or established SDK-self-release. Opaque side effects and other paths remain UNKNOWN; do not claim SDK remains alive, is never freed or is exclusively core-owned. The named release above is for M, not SDKG/R.

## First missing edges and global successor

- Entry S generation→provider publication→opaque SDK operation→cleared occupant; actual actor/scene/selector mapping and preserved receiver generations.
- Context-record null→completion of already loaded worker local receiver beforeG.virtual14; no join/last-use discriminator acquired.
- Q/W external last-user completion; **W0 purge→W1 reload/delete**; M/A reloads and current installed allocator table; post-delete core slot equality.
- G/R current equality, dependency cleanup→sharedSDK last user/self-release.

Move globally to another strong typed lifetime seam after validation, rather than widen middleware timing or generic thread/free/global searches. A concrete typed generation/last-use/self-release edge or correlated trace is the reopen discriminator. No Phase5 closeout orPhase6 implied.

## Primary verification

- Returned checker reviewed and replayed to new `scratch/phase5_physics_dependency_release_main_replay_20261004.json`:**123 explicit checks/802 instructions/2703 bytes**, six jump-skipped padding bytes separately conserved. Original return unchanged. Named imports/raw allocatorRTTI decoded from PE; no runtime API tests.
- Main `scripts/inspect_phase5_physics_release_main.py` → `scratch/phase5_physics_release_main_20261004.json`:**262 instructions/859 bytes**, selected receiver/reload/import/provider/lateSDK-use joins personally inspected. Counts overlap accepted/replayed evidence; no additive novelty claim.
- SteamASM: cleanup858484–858806; S loop858667–858691; W final858783–858806; provider860025–860076/860380–860408; wrapper/destructor866063–866225; directdeleting861030–861046. RawIAT0076E1A8/0076E1AC and allocator00827034 are build-specific; exact receipt VA/line/bytes control shorthand.

No supplied evidence, production, runtime, GOG or game modifications.
