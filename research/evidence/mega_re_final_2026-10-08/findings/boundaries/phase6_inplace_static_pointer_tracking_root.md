# Shared in-place static pointer-tracking root — Steam Phase6

## Qualified identity and initialization

**VERIFIED:**00403690 returns existing00BDA0C0 or fixed00BD9E20; bit0 of00BD9E38 gates initialization and atexit registration. Guard is set before opaque initializer0040CEE0; registered callback literal0076D9D0; cache publication/return uses fixedroot. No dynamic allocation occurs in this own getter.29instructions/117bytes personally agree with PE.

**VERIFIED:**0040CEE0 saves incomingECX inESI, invokes accepted0040D8B0 on it, reads receiver+10/+C/+0 with two unsigned begin/end consistency checks, supplies stack-output address and captured words to opaque0040D580, returns savedreceiver.32instructions/70bytes personally agree with PE. There is no own vtable installation or RTTI class name. Do not call this a named CFreeSingleton manager or invent a vtable.

**STRONG_INFERENCE:** in-place pointer-tracking collection compatible with the accepted0x18 pointer-vector shell (reference cell at+0, begin/end/capacity+C/10/14) and conditional pointer-reference append. The selected allocator/world/audio acquire the same fixed address under ordinary ABI assumptions, making this an organizing shared collection/root rather than one world-owned vector. This reuses earlier concrete constructor/append sources; it does not reopen their full growth/destruction algorithms or invent container type identity.

The initialized root/global cache is distinct from the guard and from separate scene+63A8/node+58 collections. Similar C/C+10/C+14 layouts do not prove aliasing, shared generation or shared owner responsibility.

## Registered cleanup route — VERIFIED at source boundary

Atexit literal0076D9D0 is a **non-exported label**. Its exactly two instructions/10bytes setECX=00BD9E20 and terminal-JMP0040CE20. This is positive available registration-to-target routing, not actual callback execution, global game shutdown chronology or cleanup effects.0040CE20 body remains unopened at this report's first reconstruction boundary. No function row/exported identity is fabricated for the label.

**UNKNOWN:** callback target effects, pointee deleting/free behavior, vector backing retirement, actual exit invocation/ordering, cache clear, current instance/generation, global last use, concurrent users and worker quiescence. Tracking/publication is not ownership; a vector destructor need not delete pointer entries. No88carry status or stronger guard changes.

## Architectural consequence and first missing edges

The allocator's previously opaque00403690 receiver is now linked to a separate in-place statically rooted collection and a registered CRT-exit tail route. This adds a shared construction/retention seam to the engine map. It does not turn accepted cached-object cleanup availability into actual destruction/free coordination.

Exact next static discriminator:0040CE20 own53byte target and direct0074E82B11byte runtime-boundary thunk, solely to distinguish shell/backing teardown from pointee retirement.0040D580's143byte range/move algorithm is a lower-information implementation edge; unvisited, not runtime-blocked or exhausted. No transitive cleanup matrix or caller-wide classification.

## Evidence

scratch/phase6_00403690_ownbody_check_seq0134.json; scratch/phase6_static_tracker_construction_callback_seq0135.json; inputs/decompiler/steam/DP_decompiled.c:2609-2636;12219-12245. Accepted shell source: findings/boundaries/scene_query_member_lifecycle.md (0040D8B0 only); accepted adapter/append and world source: findings/boundaries/phase5_cmap_reset_lensflare_cache_publication.md. The original world operand/generation/lastuse limits remain history and guard stronger reliance.
