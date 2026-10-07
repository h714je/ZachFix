# CThrowLure Retirement, Reset, and Stale-Handle Scope

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for reviewed static cleanup scope; runtime aftermath remains `UNKNOWN`.

## 1. Object-local cleanup

CThrowLure deleting teardown (`005F4E20/005F4F20`) rewrites the derived vtable and delegates to CRdObjectModel teardown (`00402DA0/00402D90`). That teardown cleans local model/interpolation/motion state and calls CThrowLure slot `+8`:

| Slot | Steam | GOG |
|---|---:|---:|
| CThrowLure vtable `+8` | `006BCB20` | `006BC630` |

The paired slot methods clean local fields around object `+0x390`; none reads object `+0x3C`, CRdHandleUtil, or active-list fields `+0x50/+0x54`.

**Result:** object-local CThrowLure cleanup cannot discover or clear H1 or H2.

## 2. Manager-local retirement

Generic retirement (`006C7070/006C6B70`) performs this order for an object selected from its retirement iterator:

1. rewrites neighbors using object `+0x50/+0x54`;
2. invokes vtable slot `+8`;
3. decrements manager object count;
4. calls CRdHandleUtil release (`006C7810/006C7310`); and
5. invokes vtable slot `+0` with deletion flag.

For the CThrowLure self-link created by duplicate registration, the neighbor rewrites assign the object’s links back to itself; retirement does not normalize them to a sentinel or null. The object is then deleted.

Release reads only current object `+0x3C`, so it clears H2/S2/G2. It has no input or historical-handle scan that could clear H1/S1/G1.

**Result:** manager-local retirement releases H2 only. It does not repair the self-link before deletion and cannot clear H1.

## 3. Manager reset and global scope

Manager reset (`006C58B0/006C53B0`) calls generic retirement, then destroys a manager-local auxiliary object. It has no direct reference to CRdHandleUtil global `01481358`.

The complete direct-reference census for `01481358` contains only:

- registration;
- lookup;
- retirement release;
- static constructor; and
- at-exit vtable teardown.

The constructor (`006C7AF0/006C75F0`) zeroes all table slots and generation flags, but is called from static initialization wrappers only. At-exit teardown resets the vtable and does not sweep the table.

**Result:** no reviewed post-startup manager/bulk reset clears H1. H1 remains an occupied pointer slot and generation flag in the static model until a table constructor clear, which is only directly evidenced at static initialization.

## 4. Retirement marker and iterator correction

The later bounded census establishes that `006C7070/006C6B70` does not consume an independent retirement queue. It iterates the active-object list with category selector `0xFF`, then selects only entries where `+0x29 & 0x80` is nonzero. Paired `006BAB20/006BAA70` is the sole direct exact `+0x29 = 0x80` writer found; CThrowLure vtable slot `+0x30` points to it.

Manager reset first calls `006C5DF0/006C58F0(0,0,0)`, whose selector accepts every iterated entry, before generic retirement. Its per-object dispatcher bypasses vtable `+0x30` if manager `+0x18E0 == 2`; otherwise an encountered CThrowLure is marked. Both broadcast and retirement compute next from `object + 0x54` before dispatch/deletion. Therefore the duplicate-registration self-link makes the next cursor equal to CThrowLure itself: under the enabled marker path, reset cannot statically progress cleanly past that entry; if separately marked before generic retirement, the latter retains the same pointer as post-delete next cursor.

This narrows the earlier manager-reset statement. It remains true that no reviewed direct path clears H1, but reset does have a conditional virtual marker path; it is not a clean, independently verified deletion enqueue for CThrowLure. See `cthrowlure_retirement_marker_iterator.md`.

## 5. Reuse and lookup bounds

Because H1 slot and generation flag remain occupied, CRdHandleUtil allocation skips both. The allocator cannot reuse H1’s index or generation entry while that stale state persists. This is a static allocation invariant, not a runtime-duration claim.

H1 lookup initially fails after H2 overwrites object `+0x3C`, because generation validation compares H1’s generation to the object’s current H2 generation. After object deletion, the table still contains the former object address; the behavior of an H1 lookup then depends on freed memory contents and is `UNKNOWN`. No known static consumer retains H1 in this CThrowLure path.

## CMustache cross-check

CMustachesAdmin’s child controller keeps current CMustache handles in parent slots and checks each via generation-validated lookup before creating a replacement. That controller has no duplicate-registration path like CThrowLure: it uses the current parent-held handle as an existence gate. It confirms the intended current-handle model but does not provide a cleanup path for CThrowLure H1.

## Strongest conclusion

The reviewed static code proves stale H1 is outside object-local cleanup, generic manager retirement, and manager reset. It does not prove a runtime memory leak, a persistent gameplay failure, or absence of every indirect cleanup path. The strongest supportable result is a **static stale mapping and self-link persistence invariant until an unreviewed cleanup or static table initialization occurs**.
