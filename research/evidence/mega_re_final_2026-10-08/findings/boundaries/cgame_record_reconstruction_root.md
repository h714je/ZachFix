# CGame embedded record restore and typed CPlayer reconstruction

**Date:** 2026-10-03; sequence17. **State:** VERIFIED selected Steam root/type/copy/consumer/creation and numeric transition mechanics; UNKNOWN full disk-image grammar, load-to-creation temporal coordination, actual lifetime/cadence/GOG. C0125/C0126; BND-096–098. Normal Phase3 independent mandatory save/reconstruction root check, not a Phase2 reopen or complete serialization study.

## Contract / provenance

Declared `scratch/phase3_save_seq17_contract.md`. Sources: Steam0061A830 raw body/callers, root getter/constructor/RTTI, selected00647130 numeric transition use and00506F70/00509080 consumer, plus **only selector1** factory/callback/type proof needed to identify that consumer. Accepted selector/object registration/current handle/setter semantics reused at exact scope, not counted as discovery. New join is concrete record consumption in a typed actor-creation context. No generic callback/handle/offset census.

`python3 scripts/inspect_phase3_save_root.py`: **1,011 instructions / 4,088 bytes**, selected RTTI/base/call/table checks; output `scratch/phase3_save_primary.json`. Explicit ranges avoid export-minmax expansion and preserve orphan labels. Main-only analysis and verification this batch. Input/camera seq16 checker already independently checked the CPlayer state-setter body; this batch adds its missing receiver genealogy.

Lookup0061A830: no established homology. GOG has an exported entry at the same address with a different reported body scope; it is not a correspondence. H0002 is a **player CCT** seed, not a save root: the prior planned provenance watch does not authorize applying it here. No GOG/Xbox counterpart is promoted. C0007's broad dp.sav header/28-record image stays secondary/STRONG_INFERENCE; selected embedded size/offset proof is narrower.

## Organizing acquisition / object model — VERIFIED

`0040A320` reads **00BDA004**, allocates **0x838E30** bytes through00404390, sends actual ECX to **00451D90**, then writes **0076F67C** and publishes/returns the original pointer. Decompiler's wide error-string-looking allocator argument and void return are misleading: raw PUSH immediate / EAX / ECX establish the allocation and identity. This is the shared **CSingleton<CGame>**, not CMap and not a separately proven SaveManager.

Raw wrapper COL00875AB8 -> TD0088709C names CSingleton<CGame>; BCD00875B04 identifies CGame TD008870C0 with PMD(0,-1,0), so root is a zero-offset CGame receiver. Core ctor writes00772638 CGame table. Both own tables have one observed executable slot, wrapper0040A010 and base00455D50; their full deleting/free/caller semantics were not inspected/promoted here. Constructor has repeated record-sized stride0x45CC0 and embedded helper initialization, not proof of the on-disk image count/schema. Full initialization policy and acquisition timing remain UNKNOWN.

- `inputs/decompiler/steam/DP_full.asm:11249-11283` getter; `:93903-93972` ctor; raw RTTI and zero-offset descriptor in receipt.
- `inputs/decompiler/steam/DP_decompiled.c:9483-9509`, `:63426-63462` are checked against those instructions.
- The getter's high exported fan-in(6,387) is an inventory observation, not semantic evidence that every caller uses every field. Canonical role is shared application state; selected SAVE_GAMERECORD uses are the separate boundary.

Selected object-relative regions/fields:
- **+BE8** backup-compatible record; **+8C568** live-compatible record; copy size **0x45CC0**.
- **+8C5EC** live flags, record-relative **+84**, inside copied region.
- **+99040..+99043** signed bytes, record-relative **+CAD8..+CADB**, decoded to runtime domains +834FE4/+834FE0/+834FE8/+834FF8 respectively.
- **+99048..+99057** vector(record+CAE0); **+99058..+99067** vector(record+CAF0). Their selected copy-to-actor uses are proved; friendly position/orientation names reuse actor transform evidence at STRONG_INFERENCE unless separately named.

Do not reinterpret absolute-looking decompiler `DAT_00834FE4 + param_1` as a standalone global. Raw operands are receiver-relative **[ESI+0x834FE4]**. These lie within the observed large allocation. No global-ledger cells are fabricated at those numeric offsets.

## Backup restore versus snapshot round-trip — VERIFIED C0125

**0061A830** saves actual CGame ECX into ESI. It:
1. ORs live+8C5EC with0x80000000.
2. Writes current+CED68/+CED6C into **backup+433E8/+433EC**, record-relative +42800/+42804.
3. Calls memcpy at0061A868 with destination **this+8C568**, source **this+BE8**, count0x45CC0.
4. Sign-extends four live bytes+99040..43 and writes the selected runtime domain dwords described above.

**Important scope:** Step3 overwrites the flag field from step1 because +8C5EC is inside the copied destination. Therefore that pre-copy OR is **not a guaranteed resulting bit31 condition**. No engine defect is inferred; backup contents/caller policy can supply the flag.

**00647130** copies the same-size **live -> backup**, calls0061A830 on the same receiver, then ORs live flags with0x80000000 **after** the restore. This is a selected snapshot/restore round-trip with backup-field patching, not proof of a disk-read event or an identity/no-op operation. Another raw caller0061B369 acquires CGame for0061A830 under a bit0x40000000 condition and preserves selected counters around it; its fuller task/context owner is unvisited, not universally absent.

- `inputs/decompiler/steam/DP_full.asm:603925-603948` restore, `:651851-651864` round-trip, `:604647-604687` second caller.
- `inputs/decompiler/steam/DP_decompiled.c:342554-342572` restore. NoMemcpy algorithm archaeology needed.

## Concrete lifecycle transition context — VERIFIED selected numeric branch

Event dispatcher **00642640**, event1 table target00642A42, reads private **014736D4**, and its selected numeric **6** maps to006438A7. After separate menu/control byte gates, +83500C bit2 clear and bit4 set reach00643A11. That branch changes live flags/control fields, acquires CGame and calls same-root00647130 at00643A74. Further flag masks follow. These are separate private-state and CGame domains; six is not CMap+10 state6 and no friendly loading/pause/title name is proven.

- Raw event/state tables00646D58/00646D80/00646E18 and exact mapping in receipt.
- `inputs/decompiler/steam/DP_full.asm:647213-647220`, `:648040-648050`, `:648080-648101`; supporting C`:361838-361914`.
- Higher incoming selection/whole frame nesting remains UNKNOWN. Existing B0009 raw-selector incoming bound is reused at its exact source limits, not rescanned or declared resolved. This supplies a mode-specific use context, not all-mode scheduling/cadence.

## Typed actor-creation -> reconstruction -> record-vector consumer — VERIFIED C0126

The important new type/data/control join:

1. Accepted factory **005E7620 selector1**: raw table005E9588 ->005E7668 allocates0xD50 and calls0052DF70 on that allocation. Its existing CPlayer type is also raw-vptr confirmed at0052DFB3(007767FC).
2. Successful case preserves object in ESI and selector1 in EDI/EBP; sets object selector word+30=1, registers through accepted006C5AE0, loads callback **008BF668=005348D0** and supplies the same ESI to **006BAB80** at005E956A.
3. Accepted setter writes object+44 and initially dispatches event0 through00402870. The seam has an optional global hook before reading object+44; the native installed-pointer arm is conditional, not a promise about runtime external callbacks.
4. **005348D0** is a raw five-byte jump to **0052EBC0**, not its expanded decompiler duplicate. Event0's table branch0052EBED passes the callback's first **CPlayer object argument**, not the opaque third argument, to006C5FB0; stores its handle in008A9BA4; resolves that handle through006C5FD0; tail-jumps **00506F70 with that actor in ECX**. This reuses accepted current-handle identity mechanics.
5. **00506F70** preserves actor in ESI, performs selected runtime resets, acquires CGame and tests **live bit0x80000000 set AND live+99042 !=0xFF**. On that arm00507349 calls **00509080 with CGame ECX and the typed actor as stack argument**.
6. **00509080** copies Game+99048..57 into actor+58..64 and duplicate+3B0..3BC; Game+99058..67 into actor+78..84 and duplicate+68..74. For live+99042 !=0xFF it sign-extends that byte into Game+834FEC. This is positive embedded-record -> transient actor reconstruction, not a second whole-record memcpy or a claim of complete world/resource restoration.
7. In a later selected mask branch of00506F70, raw005076DA restores same actor ESI to ECX and005076DC calls **00528F40(0x40)**. This establishes the missing **typed CPlayer state-setter receiver** for that specific positive chain; no offset-only inference. C0004 full handler table137-slot claim is still not proved.

Raw refs: `inputs/decompiler/steam/DP_full.asm:547384-547399`, `:549064-549074`, `:549172-549196`, `:340972-340982`, `:780947-780963`, `:1854-1879`, `:341672-341701`, `:301125-301130`, `:301338-301348`, `:303288-303326`, `:301586-301597`. Supporting C`:179224-179430`, `:180446-180468`. Callback/state tables and raw tails in receipt; no exported row is invented for0052EBED or0061B369.

This satisfies the seq16 input continuation's exact typed-player genealogy discriminator at the selected chain and adds a concrete native creation/reconstruction phase. It does **not** establish that all calls to00528F40 share that genealogy, all fields/action modes are known, or actor creation always occurs immediately after0061A830. The restore routine and actor consumer are joined through the same live embedded bytes and guard, not an invented immediate call stack.

## Residuals / reopen conditions

- **UNKNOWN/unvisited:** actual physical dp.sav image load/write/header/count/version grammar, staging-image owner/use and copy into backup; restore-call to later actor creation timing/transition coordination; all-slot selection; complete surrounding world/event/NPC reconstruction; CGame allocation-free/root-clear invocation; GOG primary matching.
- **UNKNOWN:** broader numeric transition naming/recurrence and rawselector incoming placement. Original B0009 bound and all accepted Phase2 generic-seam/format/handle/runtime limits stay intact.
- C0007 broad image schema and C0004 full actor state-table extent remain STRONG_INFERENCE; C0010 latency/frequency remains secondary. No runtime evidence acquired, no fixes/hooks/patches/assets altered.
- New work must follow a concrete typed disk-image/read/write/staging caller, transition-to-creation discriminator, invoked deleting/root-clear source, or structurally matched GOG boundary; do not repeat this getter/copy/factory/callback table source by renaming the frontier.
