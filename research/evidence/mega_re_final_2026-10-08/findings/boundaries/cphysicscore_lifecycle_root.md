# CPhysicsCore acquired root and selected lifecycle placement

**Date:** 2026-10-03. **Frontier:** PHYSX_ROOT_PRIMARY. **Outcome:** ADVANCE. C0121; BND-092; narrow C0122/BND-002 attribution correction. Steam-only new scope; runtime PhysX conclusions are not imported.

## Source contract and legitimate narrow correction

Follow accepted `006EB3C0` object-phase anchor to its actual getter/ECX, raw type/construction, startup SDK initialization, selected completion/ordinary record boundaries, phase-11/9 root consumers and shutdown session cleanup. Reuse accepted object phase order, not generic actor slots or manager acquisition. Exclude PhysX internals, full timing/queue algorithms and B0001–B0003 debt/cadence. Consult ledgers/homology first; no existing root correspondence is forced into GOG. The new raw observation of the phase-7 marker between the two synchronous helpers is the concrete contradictory-primary discriminator for correcting **only that older attribution**. Accepted Phase 2 object/root/interface milestone and its audit remain unchanged.

## Root/type/acquisition — VERIFIED

- Actual getter **0040E470** lazily allocates **0x67EDC** and retains allocator EAX in ESI; ECX=ESI -> **0040E340**; installs singleton table **0076FE3C** and publishes/returns saved ESI through EAX to **00BDA0C8**.
- **0040E4F0 is only a five-byte JMP to 0040E470.** Its decompiler output expands the target's getter body and declares void; that output is not its actual body or return contract. The exported thunk identity is preserved; no duplicated allocation body is invented.
- Raw base table **0076FE04**, COL **00875F34**, TD **008A5BF8** decodes `.?AVCPhysicsCore@@`; singleton table **0076FE3C**, COL **00876060**, TD **008A5C8C** decodes `.?AV?$CSingleton@VCPhysicsCore@@@@`. Each observed table has one code slot: **0040E300 / 0040E440**.
- Constructor installs the base table and embedded interface table pointers at +0x30/+0x34/+0x38/+0x40, creates named **PhysicsEvent** into **+0x48**, and a private HeapCreate handle into **+0x67ED8**. Embedded callback names from symbols are leads, not complete slot/SDK semantics here.
- Selected important storage: **20 dword pointer slots +0x50..+0x9C**, shared SDK alias **+0x11C**, scalar gate **+0x67CC8**, controller-support pointer **+0x67CD4**, event +0x48 and heap +0x67ED8. Pointer-slot contents/ownership are not established solely by constructor shape; the SDK init/use/cleanup evidence below supplies the selected relationships.

## Startup initialization and SDK dependency — VERIFIED mechanics

Startup **004017C0@00401829 -> 006CC290**, after application publication of receiver 00BD7678, calls **006EAAD0@006CC2E5**. That wrapper calls the physics getter thunk, **ECX=EAX at006EAADA -> 006EC450@006EAADC**. The enclosing receiver is a separate graphics/platform context; it is not assigned CPhysicsCore ownership from adjacency.

`006EC450` conditionally calls imported **NxCreatePhysicsSDK** with literal **0x02080100**, retains its result in shared global **01493FA8**, stores the same pointer at acquired core **+0x11C**, and initializes/clears the 20 slots at +0x50. It creates a separate 0x18 controller-support wrapper and stores its result at +0x67CD4. Slot-operation names and full initialization/error policy are not derived from decompiler names. Successful live initialization, actual loaded DLL version and exclusive global SDK ownership remain UNKNOWN.

The direct import and pointer flow prove a game-owned organizing service above PhysX, not just miscellaneous library calls. Scene-like use of the 20 slots is **STRONG_INFERENCE** from the shared SDK interface, selected cleanup, common producer and consistent virtual operation sequence; exact SDK virtual method names rely on the documented ABI and are not literal executable symbols.

## Selected use in the object/frame spine — VERIFIED

| Dispatcher marker / phase | Concrete selected edge | Scope |
|---|---|---|
| **14 / 0x0E** at006C6C70 | getter thunk006C6CA0; ECX=returned core at006C6CA5; **006C6CA7 -> 006EB3C0 -> 006EAB60**, preserving the same root ECX and supplied 014AFFE0 scalar | Rooted common producer, not proof of seconds, scene-zero-only work or measured cadence. |
| Still **14**, before marker7 | **006C6CB4 -> 0040B750**, which fans into **0040B850** over four fixed **0x2C contexts at00BDA010..00BDA0BF** | Selected synchronous first-half transaction; raw virtual calls +0x14C/+0x148/+0x230/+0x144 under flags. |
| **7** at006C6CC2 | **006C6CD4 -> 0040B780 -> 0040B940** over the same context range | Second-half calls +0x238 and optional timing restore +0x148, then clears context count +0x28 and flag +0x19. |
| **8** at006C6CF4 | Accepted post-completion active-object event6 phase | Reused interface/order scope, not fresh all-object dispatch census. |
| **11 / 0x0B** at006C6DCB | getter thunk006C6DD5; ECX=EAX -> **006EB400@006C6DDC** | Selected same-root input; profiling label PhysicsResist and actor-iteration body are seeds for mechanism work, not fetchResults. |
| **9** at006C6EC7 | getter006C6ED1; ECX=EAX -> **006F9800@006C6ED8** | Selected core refresh/reset branch; +0x118 can select cleanup/reinitialize006EC5F0. Full state meaning unassigned. |

`006EAB60` tests a core scalar through `006FEF80` (+0x67CC8) and a world/mode predicate, stages up to 20 pointer/index records from **this+0x50**, waits on **this+0x48**, and enters selected common record helpers. No human pause/loading policy or previous-batch guarantee is promoted solely from this wait.

At the selected record boundary `006EACE0` reaches timing configuration, then `0040B7B0 -> 0040BCE0 -> 0040BC50`. The raw producer selects fixed context **00BDA010 +0x2C*ordinal** and copies six dwords into 0x18-stride queued records. Separately, **0040BAF0** is a loop body consuming context +0x20 records, calling virtual offsets **+0x230 -> +0x144 -> +0x238**, then resetting count and signaling a helper. **STRONG_INFERENCE:** these are ordinary simulate/flush/fetch versus the synchronous transaction above, consistent with the prior PhysX ABI seed. **UNKNOWN:** full worker entry/activation and queued-list -> active-list transfer; neither is established merely by matching record offsets. The loop body is not relabeled a proved OS thread entry. No copied +8 word is promoted to live timing authority.

## Shutdown/reset versus allocation free — VERIFIED distinction

Application **00401310@00401412 -> 006CC670@006CC677 -> 006EAAF0**. The wrapper obtains the same physics root and passes EAX as ECX to **006F9040**. Under nonzero shared SDK global, this method processes selected dependent collections and, for each nonnull +0x50 slot, calls a removal helper and SDK virtual +0x14 before clearing the slot. Controller support at +0x67CD4 is processed/cleared separately. Exact every-object/resource cleanup and safe callback/worker coordination remain unvisited.

This is a positive invoked **session/dependency cleanup**, not proof that the cached 0x67EDC allocation is freed. The separate destructor **0040E2A0** rewrites the base table, sets/closes event +0x48, destroys heap +0x67ED8 and resets embedded tables. Own slot targets **0040E300/0040E440** call it and conditionally free under flag1. Actual deleting invocation/cache clear, final global SDK release and full worker/controller/scene safety remain **UNKNOWN**, not a permanent-allocation claim.

## Narrow correction / preserved history

**DISPROVEN C0122:** both 0040B750 and0040B780 execute while manager+0x18E0 is7. Raw order is **phase14 -> producer -> 0040B750 -> write7 -> 0040B780 -> write8**. The earlier `object_physx.md` line assigning both helpers to state7 is superseded at that exact Steam scope. Broad C0006 ordering (14 before7 before8), State-5 and State-11 disprovals C0013/C0014, C0116 manager identity/interface and the accepted Phase2 closeout remain intact. GOG marker attribution is not casually transferred across the known export split.

## Primary references / verification

- Root getter/constructor/destructor: `inputs/decompiler/steam/DP_decompiled.c:13132-13317`; raw **0040E470/0040E4F0/0040E340** and table/COL/type bytes in primary `inputs/binaries/steam/DP_STEAM.exe`. Getter allocation/result anchors `inputs/decompiler/steam/DP_full.asm:15833-15866`.
- Startup/shutdown receiver and wrappers: `inputs/decompiler/steam/DP_full.asm:563-566`, `:295-299`, `:802219-802222`, `:802434-802437`, `:842140-842158`.
- Exact phase-marker/call distinction: `inputs/decompiler/steam/DP_full.asm:795564-795640`, `:795685-795691`. Selected initial/producer bodies `DP_decompiled.c:467167-467223`, `:466113-466226`, `:466496-466511`; cleanup `:473716-473819`.
- Selected record/worker and synchronous scope: `inputs/decompiler/steam/DP_decompiled.c:10662-10833`, `:10913-11057`; code/ABI labels from `inputs/knowledge/zachfix_research_2026-10-01/physx/README.md` are secondary, not runtime confirmation. No absent historical trace is treated as primary.
- `scripts/inspect_phase3_physics_root.py` corroborates **1,286 instructions / 4,346 bytes**, raw root RTTI/one-slot extents, thunk/result/root flow, startup/cleanup edges and phase14/7/8 markers against primary PE. Derived receipt: `reports/PHASE3_PHYSICS_ROOT_RAW_CHECKS_2026-10-03.json`. Whole-byte checks are not complete method-semantic proof.

## Architectural gain and remaining source boundary

Mandatory current-primary physics organizing acquisition/use is reduced at **Steam selected static scope**: typed root, startup SDK dependency, frame/object core use and invoked session cleanup. This does not complete PhysX scheduling or SDK lifetime. Keep `PHYSICS_LIFECYCLE_CONTINUATIONS` for worker activation/record transfer, concrete cache/SDK deletion and GOG foundational correspondence. Runtime B0001–B0003 remain trace-dependent, with no new solver/debt/vehicle/CCT conclusion. Independent input/camera, save/reconstruction and vehicle primary root checks remain viable higher-information breadth work.
