# CMustachesAdmin Parent Handle Access Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for the paired direct controller behavior and bounded absence of direct retirement in the reviewed owner/controller source universe.

Paired controller Steam `00527660` / GOG raw continuation rooted at `00527730` iterates CMustachesAdmin packed child handles `+0x2C`, `+0x30`, and `+0x34`. It resolves each through the active-object lookup (`006C5FD0` / `006C5AD0`) and manipulates state on the resolved child, including child `+0x61C`; it does not overwrite any parent child-handle field and does not directly call generic retirement (`006C7070` / `006C6B70`) or a CRdHandleUtil release helper.

Steam `00527A60` separately resolves the same three handles and clears each resolved child `+0x61C`; it likewise leaves parent handle fields intact and makes no retirement/release call. GOG has compiler-displaced/orphaned controller continuation in the same local code region, so its generated function metrics are not treated as complete.

Direct-call evidence independently bounds generic active-object retirement to the frame root and manager-internal traversal helpers, not CMustachesAdmin controller/owner functions. The paired CMustache deleting destructors `00528020` / `005280F0` have no direct caller in the supplied call manifests. Previously reviewed CPlayer teardown only resets embedded CMustachesAdmin vtable state, and CFishingPerson teardown neither reads its `+0x7F0` CMustachesAdmin pointer nor the child handles; it takes the distinct control-bit/event-13 broadcast path.

Therefore, no class-discriminated direct parent-handle path establishes child deletion, generic retirement, packed-handle release, or parent-slot clearing. The static direct-access question is bounded. A future class-discriminated indirect destructor/retirement call source or runtime trace would be required to characterize actual child lifetime.

## Cold7F0 separate source supplement — sequence84

**VERIFIED STEAM C0203/C0204:** exact3C allocation/value/CMustachesAdmin type retainedP7F0; cold event2 direct00526BA0 thenindependentlyreloadedcurrent-slot0flag1/fieldclear. Helperthreehandles/twoseparatelookups/currentvirtual30, namedshellavailabledeallocation notdescendantdestruction. **UNKNOWN:** construction/currentA/X generations, currenttable/typedchild/lastuse/free safety. Originalordinaryactor/controller/CMustache/CThrow/handlebounds andallseq83corrections retained. findings/boundaries/phase5_fishing_cold_7f0_shell_retirement.md
