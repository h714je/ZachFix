# CFishingPerson cold 7F0 shell retirement and first descendant break

**2026-10-04; Phase5 batch10; STEAM_PC only.** C0203/C0204. Sequence83 selected a cold continuation, not a generic CMustache/handle reopening. C0188's typed installed-parent event2 → `0060E800` is reused at its conditional-static scope.

## Contract

Finite sources: cold `0060E912–0060E94C`, exact parent `+7F0` writer in typed `00613960`, directly invoked complete `00526BA0`, and exact named shell slot0 `004274E0`. Parent/slot/type prefixes are identity checks, not duplicate architectural credit. No generic CMustache, CThrow, handle, creator/controller, dispatcher, or minigame census. The newly acquired direct helper call is the discriminator for examining that helper alone, not for reopening its transitive targets.

## Typed writer versus current occupants — VERIFIED local transport

The independent writer branch identifies `00613C26` (Steam ASM596625) as an exact store into the constructor's preserved CFishingPerson receiver `P+7F0`. Primary assembly `00613BFB–00613C26` shows:

- push allocation size `3C`, call `0074EEEB` at `00613C07`;
- successful EAX is unchanged through inline vptr store `007713F4` at `00613C13` and `+2C/+30/+34 = FFFFFFFF` stores;
- failed allocation converges with EAX=0;
- store that exact EAX into `P+7F0`. There is **no nested constructor call** in this block.

Raw `007713F4` → COL/type descriptor names **CMustachesAdmin**, with observed slot0 `004274E0`. This proves the successful constructor-retained shell's type; it does not name the shell CMustache. The named class and exact pointer transport, not a nearby constructor or an equal offset, support this identity.

Use separate symbols: `A_construct` for the stored allocation, `A_helper=[P+7F0]` at `0060E92A`, `A_delete=[P+7F0]` at `0060E935`, and the post-interface parent field at `0060E947`. **UNKNOWN:** preserved construction→current occupant generation and equality across intervening calls. Retention is not exclusive ownership.

## Cold parent continuation — VERIFIED invoked local interfaces

After the earlier C0188 line clear and a separate current `P+690` slot0/flag1/clear, the continuation:

1. loads ECX=`A_helper` at `0060E92A`;
2. directly calls `00526BA0` at `0060E930`, **without a local null guard** on `A_helper`;
3. reloads ECX=`A_delete` at `0060E935`;
4. if nonzero, loads that receiver's current vptr and slot0 at `0060E93F/941`;
5. pushes literal1 at `0060E943`, calls the slot at `0060E945`;
6. on normal return stores zero to `P+7F0` at `0060E947`.

ESI is the saved P, EBX was explicitly zeroed at `0060E817`. The current `A_delete` is locally conserved for vptr/call. The earlier helper can transitively alter state; neither the pre-helper load nor constructor type proves the post-helper current vptr. The clear is a local field store, not proof of deallocation, complete shell ownership, absence of reentrant replacement, or global last use. This event2 cleanup is not the separately available whole actor destructor `0060F290`.

## Directly invoked shell helper — VERIFIED mechanics, descendant identity UNKNOWN

Complete `00526BA0–00526BE3` (ASM333414–333438) starts ESI=`A_helper+2C`, EDI=3, then for each of `+2C/+30/+34`:

- load the current slot value and current application manager `00BD7670`, call accepted handle lookup `006C5FD0` at `00526BB9` → `X_test`;
- only if `X_test!=0`, **reload both slot and manager**, call the same lookup again at `00526BCB` → `X_call`;
- without a separate null test on `X_call`, load its vptr, set ECX=`X_call`, invoke its current virtual `+30` at `00526BD7`;
- advance slot by4, decrement the three-iteration counter.

The complete local helper has no slot clear, direct release, or direct child deleting interface. **Invoked virtual30 is cleanup/control of an unresolved current object, not proven destruction.** The two lookups are separate observations; even equal numerical slot/manager addresses do not establish `X_test=X_call`, generation preservation, second-result nonnullness, or a CMustache current table.

Accepted C0076/H0241 establishes a creator's CMustache handle production at these shell offsets, but it is not re-acquired or promoted here. It does not bridge construction/current-slot/current-resolved-child generation to this call. **First descendant break:** the exact current slot/manager → `X_call` lifetime/type, including equality with `X_test` and any earlier created child. Stop before transitive virtual30 target semantics. No leak, double free, safe free, or child retirement conclusion follows.

## Named shell deleting interface — VERIFIED availability; dispatch conditional

Complete `004274E0–004274FE` (ASM44111–44121) tests incoming flag bit0, preserves ECX as ESI, writes shell table `007713F4`, conditionally passes that same ESI to deallocation boundary `0074E82B` at `004274F1`, returns saved receiver with RET4. There is no local descendant slot access or invoked ordinary child destructor.

**Conditional relationship:** if `A_delete`'s actual current table is the named shell table, parent literal1 selects this shell deallocation boundary. A concrete parent current-slot invocation is now primary-backed; the complete shell implementation is available. That is not proof that the table remained installed, that this occurrence selected it, that deallocation succeeded, or that shell/descendant borrowers had stopped. Do not join shell free with descendant deletion.

## First stronger edges and durable limits

- `A_construct` → `A_helper` → `A_delete` → actual selected named interface, same lifetime generation, unresolved at the first intervening/current-state boundary.
- Current shell handle value/manager generation → `X_test`/`X_call` and typed descendant identity; virtual30 effects and last use unresolved.
- Actual actor callback occurrence/admission, alias01470670 lifetime, parent/shell/descendant global last use remain UNKNOWN.
- C0081's immediate ordinary actor teardown/broadcast remains correct at its original source scope. C0105's selected controller/direct-caller bounds remain correct; this cold event2/helper continuation supplements them, not a blanket claim that no parent shell can ever be cleared.
- P7E0↔CFishingLine8 alias, old CThrow/H1/self-link, CMustache deleting availability versus actual child invocation, all build/runtime/source caps remain unchanged.

## Primary checks and clerical history

`scripts/inspect_phase5_fishing_7f0_main.py` → `scratch/phase5_fishing_7f0_main_20261004.json` and distinct `scratch/phase5_fishing_7f0_main_replay_seq0084.json`: each **64 instructions/186 bytes**, cold raw receiver/reload/flag/clear, complete helper and exact shell slot/RTTI/deleting availability. Counts overlap and are not additive. Writer branch checker and targeted personal writer replay are separately recorded in the batch receipt; branch output is not promoted without personal verification.

Initial main checker had two mistaken assumed prologue anchor addresses; it failed before writing output. Exact exported instruction starts `0060E803/0060E817` replaced those addresses; permitted span extended to the zero anchor only. No evidence/semantic change or validator alteration. Initial worktree agent launches failed before useful work because this is not a git repository; fresh ordinary background branches continued, without orchestration repair.

All sequence83 corrections remain binding: C0047/C0086 DISPROVEN, H0209 former same-slot basis invalid, C0198 late pointer transport not T memory access, Input+668 eleven-byte no-op, carrier `(1000,0)`→Sleep request1 not completion/drain/join. No supplied evidence, binary, production source, runtime modification or Phase6.

Independent writer report: scratch/phase5_fishing_7f0_writer_branch_20261004.md; checker receipt542instructions/2325bytes across finitector fieldcensus/continuation/origin, no additionalfactorybodies. Main personal exactwriterreceipt scratch/phase5_fishing_7f0_main_writer_verification_seq0084.json:25instructions/93bytes. Importanttype/transport/fieldclear joins personallyverified; no rawwriter census beyond declaredparentbody. Sequence84 actualledger/state/heuristic/progressallEXIT0: reports/PHASE5_COLD7F0_VALIDATION_2026-10-04_SEQ0084.txt.
