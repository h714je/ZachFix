# Typed light node detach and available cleanup dispatch limit

**2026-10-04; Phase5 batch12; STEAM_PC only.** C0208/C0209. Sources: exactly three new bodies`005F26C0/005F27E0/005F2890` and two raw typed tables. C0141 node collection and C0200 helper/current-vptr mechanics are reused at their existing confidence limits; old tree/query/packet/handle/free sources are not reopened.

## Typed detach request — VERIFIED, conditional interface

Raw tables`0077A5A4`→COL`0087816C`→TD`008C1A40` name **CRdObjectLight**, and`0077B1D4`→COL`00878780`→TD`008C1C64` name **CLight**. Both observed19-code-pointer tables have slot48=`005F27E0`; own slots are respectively`005F2890/005F2FC0`. Named table availability is not live object selection or an invoked lifetime generation.

`005F27E0` saves incoming O in ESI and reads its explicit query argument Q. For Q=0:

- `005F2861` loads N=`[O+14C]` into ECX;
- if N!=0, `005F286B` pushes original O and `005F286C` calls`006BC060(N,O)`;
- after normal return or N-null skip, `005F2871` zeros current`O+14C`; returns RET4.

Exact receiver/original pointer/request/clear are **VERIFIED**. Matching original-object-pointer removal from node+58 remains **STRONG_INFERENCE**, preserving C0141's unexpanded iterator-adapter qualification. The field clear is not O/N destruction, removal of all aliases/duplicates, free, exclusive ownership or final use. No new node/object generation or full nonzero selection policy is claimed.

## Available named own-slot cleanup — VERIFIED implementation

CRdObjectLight own slot`005F2890` saves O, calls ordinary`005F26C0` at`005F2893`, then tests original flagbit0. If set, it passes that same saved O to accepted outer-deallocation boundary`0074E82B` at`005F28A0`; returns saved O/RET4. Flag0 still invokes ordinary cleanup. This is available deleting implementation, not a class-discriminated actually retired instance or successful safe free.

Complete ordinary`005F26C0`:

1. saves original O; installs light table`0077A5A4` at`005F26E8`;
2. invokes accepted`006BA850(O)` at`005F26F6`;
3. forms O+194, installs embedded table`0076E69C`, invokes opaque`006B7F90` at`005F270C`;
4. restores ECX=O, installs base CRdObject table`0076E6B4` at`005F271B`, invokes`006BA850(O)` again at`005F2721`;
5. returns.

Two helper entries occur after different explicit named table writes on the same saved pointer value. They are **not two guaranteed same-target detach calls**. The first is not automatically the base target; the second is not automatically the light target. Embedded cleanup is opaque and not interpreted as whole-object destruction.

## First current-dispatch/generation break — UNKNOWN

Reused C0200 proves`006BA850` optionally performs an opaque backing operation, then reloads current vptr and invokes slot48(0). Therefore:

- if the first later current table is still light`0077A5A4`, the newly proved target is`005F27E0`;
- if the second later current table is still base`0076E6B4`, accepted target is`004029B0`.

**First missing edge:** `005F26F6→006BA850→later current slot48→005F27E0`, for the same actually retired O generation. Preceding table installation/helper-entry transport do not preserve current table/receiver generation across the inherited opaque boundary. Actual class-specific invocation of own slot`005F2890`, node/O alias generation, global last-use and successful safe retirement remain UNKNOWN. Detach is not original-object or submitted-packet last use. Stop without expanding old helper, query, packet, tree, free, registry or generic manager sources.

CLight own-slot`005F2FC0` is a raw table locator only; its body is not acquired. Existing H0056 maps **Steam constructor005F2EF0 ↔ GOG constructor005F2FC0** only. Equal cross-build address does not identify the Steam deleting candidate. No new GOG/Xbox parity or homology promotion.

## Primary verification and retained history

Independent branch`scratch/phase5_spatial_node_branch_20261004.md`/`.json` uses three of five permitted slices:92instructions/311bytes,9rel32 targets, two raw tables. Branch metadata-only entry/span association error was corrected and replayed before handback; byte/semantic findings unchanged. Main personally inspected every selected body/typed table/receiver/argument/vptr-order join; `scripts/inspect_phase5_spatial_light_main.py` → `scratch/phase5_spatial_light_main_20261004.json` and distinct`..._replay_seq0088.json`, each92instructions/311bytes. Overlap/reuse not additive coverage. ASM559423–559452,559501–559551,559557–559567.

All70+18 guards and sequence83 corrections conserved: C0047/C0086 DISPROVEN/H0209 invalid old basis, C0198 pointertransport notlateTread, Input+66811-byte no-op, `(1000,0)`→Sleep request1 notcompletion/drain/join. Available destructor≠invoked destructor; clear≠free; localteardown≠global lastuse; retention≠exclusiveowner; equaladdress≠equalgeneration. No input/binary/asset/production/runtime changes, patches/hooks/ASI, Phase4 audit, Phase5 closeout or Phase6.
