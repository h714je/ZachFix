# CMustachesAdmin-Owned CMustache Active Objects

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for paired Steam/GOG child creation/registration. GOG raw executable recovery is recorded in `cmustache_gog_creator_raw_recovery.md`.

Steam `00527170` is a CMustachesAdmin child-controller, not an unrelated batch producer. It is called with CPlayer `+0x80C`, CFishingPerson’s CMustachesAdmin pointer, and CMenu `+0x9478`.

For each empty parent slot at CMustachesAdmin `+0x2C`, `+0x30`, or `+0x34`, the function:

1. allocates `0x628` bytes;
2. initializes base state and writes the named `CMustache` vtable `00776504`;
3. registers the new CMustache through `006C5AE0` / active-object manager `00BD7670`;
4. configures task/resource state; and
5. stores the resulting packed handle in the parent slot.

The children are therefore independent active objects with their own CRdHandleUtil handle at child `+0x3C`. Their parent stores those handles, not direct pointers, and later resolves them through the high-fanin handle lookup wrapper.

`00528020` / `005280F0` are paired CMustache deleting-destructor helpers. Each rewrites the vtable, releases owned state at child `+0x618` when present, then conditionally deletes. Raw GOG `0052728E`–`005276A6` supplies the matched three-child creator continuation that the generated decompiler record truncates.

## Consequence for embedded-object analysis

CMustachesAdmin is still not directly registered as an active object merely because it is embedded or separately allocated. However, an unregistered embedded/parent-held helper can own and schedule independently registered child active objects. The earlier embedded-helper rule is thus limited to the helper object’s own handle, not its descendants.

## Parent teardown boundary

Paired CFishingPerson teardown (`0060F290/0060F3A0`) calls `005E6CF0/005E6DC0(1)`. That selects a manager-wide broadcast (`006C5ED0/006C59D0`) with predicate kind `2`, criterion `0x17`. For every matched active object it invokes `006BB180/006BB0D0`, which ORs caller mask `1` into that object's `+0x2B`, recomputes `+0x38`, and dispatches event `13`.

This path does not invoke generic retirement, CRdHandleUtil release, or the `+0x29` retirement marker. The reviewed CFishingPerson teardown body does not dereference its separately allocated CMustachesAdmin pointer at `+0x7F0` or any of the three CMustache handle slots. CMustache creation configures separate child fields (`+0x2C = 0x11`, `+0x30 = 0xBF`), but no evidence identifies selector-2 criterion `0x17` as a CMustache child.

**Result:** parent-triggered child retirement/release has not been directly recovered. The observed CFishingPerson teardown broadcast is a distinct non-handle control path, not evidence of CMustache retirement; generic active-object retirement remains the verified release mechanism. The paired direct parent-handle controller census is now bounded in `cmustache_parent_handle_access_limit.md`.
