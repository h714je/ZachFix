# Optional record admission, deferred delivery and typed slot98 producer

**Campaign FSC001; Steam/GOG selected static scope.** This is a new producer/comparator/type connection to the accepted Phase6 record drain, not a new scheduler or a runtime-success finding. Prior report: `findings/boundaries/phase6_optional_callback_record_delivery.md`. Exact new acquisitions: `audit/final_static_campaign_2026-10-07/OPTIONAL_RECORD_PRODUCER_PRIMARY.json` and `OPTIONAL_RECORD_CONNECTION_PRIMARY.json`, with their pre-analysis contracts.

## Prior scope and why this is new

The prior drain knew fixed record base `00BD7690`, count `00BD7798`, busy byte `00BD7674`, comparator address `00401020`, low-key optional-hook/current-payload+44 route and high-key current-vtable+98 route. It explicitly did not acquire the producer, comparator, selected caller contexts or GOG bodies. That earlier decision against deeper descent was a scheduling recommendation, not a static absence proof. The new enqueuer/comparator bodies and independently typed slot98 producer provide finite discriminators; existing hook/lifetime/cadence questions are not reopened.

## VERIFIED admission and comparator mechanics

In **both own builds**, `00401050` tests `00BD7674`:

- Zero: load index from `00BD7798`, write second stack DWORD to `00BD7690 + index*8` and first stack DWORD to the next DWORD; increment/publish the count; return `AL=1`.
- Nonzero: no selected record/count write; return `AL=0`.

This is an **admission/insertion result**, not callback or draw success. The selected body contains no capacity check, duplicate check, atomic locking or wait. These are local code facts, not proof of an invalid runtime count or actual overflow/race.

`00401020` reads the first DWORD of each comparator argument. It returns **+1 when signed left key < right key; -1 otherwise, including equal keys**. It never returns zero. Thus it is descending-oriented for unequal keys, but equal-key behavior does **not** satisfy an equality-return contract. Do not describe the resulting queue as a valid total/stable/FIFO ordering, or infer a deterministic equal-key order. No observed sorting fault is established.

The selected producer/comparator scope per own-build pair is 21 instructions / 70 bytes, all qualified singleton reachable instructions and own-PE/original-ASM matched. The GOG connection acquisition independently checks these bodies plus its drain; same numeric addresses are not the basis of correspondence.

## VERIFIED selected low-key caller paths

Selected Steam event-dispatch prefixes at `00633BBB–00633BFB`, `006348DE–00634915` and `0063ED3E–0063ED7D` isolate numeric **byte 0x12 (decimal18)**. Each offers its receiver-like operand and key0 to `00401050`; `AL!=0` selects an exit branch, while zero permits later local continuation. The second and third receivers remain untyped; auxiliary CMessage allocation never types their incoming receiver.

The first prefix belongs to the already-qualified native Instruction source/import endpoint `00633B80`. The established selector35 CHelp factory→installed `006347D0` adapter path supplies a class-proven receiver on that selected route only (`findings/boundaries/targeted_bridge_help_callback_import_receiver.md`); it does not type every endpoint caller. The current queue prefix adds a delivery/admission edge, not another texture-parser or CHelp-construction discovery.

The accepted Steam drain `00401080`, and newly acquired own GOG `00401080`, set the busy byte before their sort/selected delivery loop. For a signed key below10, they offer `(current payload,0x12,incoming DWORD)` first to the optional current `0148132C` hook, then reload current payload+44 and conditionally call it. This composes with the selected caller's insertion-versus-continuation branch as a **STRONG_INFERENCE deferred local presentation/control mechanism**.

The composition is conditional: a hook can change carriers before the second callback is reloaded, and callback/receiver/backing epochs are unproved. When the same busy storage remains nonzero at a nested admission call, the enqueuer refuses insertion and returns zero; this is not a global serialization or unconditional draw guarantee.

## VERIFIED independently typed virtual producer

Own RTTI identifies Steam table `0077D854` and GOG `0077D844` as **CObjectTarget**, with the CObject→CRdObjectModelGame→CRdObjectModel→CRdObject base lineage. Their observed slot `+98` cells `0077D8EC` / `0077D8DC` contain `005B19A0` / `005B1A70` respectively.

These independently acquired methods have matching selected mechanics:

1. Require receiver byte `+3B !=0`.
2. Offer the receiver and **wrapping32-bit** key `[receiver+460]+0xA` to the selected enqueuer.
3. Nonzero admission return selects the local exit.
4. Zero admission return reaches an opaque game predicate and a receiver-interior `+458` fallback request, choosing one of two float literals before calling `005B1120` / `005B11F0`.

The unproved range of `+460` means adding0xA does **not** guarantee a signed key>=10. Do not automatically classify every such record into the drain's virtual arm. The available named-table method is typed; an arbitrary current drain payload is not thereby typed. No fallback helper's full meaning, current method installation, successful rendering or owner/free relationship is asserted.

## Scoped correspondence and architecture consequence

**VERIFIED selected structural correspondence:** Steam/GOG comparator `00401020`, enqueuer `00401050`, drain `00401080`, and independently typed methods `005B19A0` / `005B1A70`. Evidence is own-build field/branch/argument/call/table lineage, not numeric-address equality or a blanket relocation. GOG drain calls its own qsort-shaped endpoint `0074ED20`; Steam's accepted endpoint is `0074F010`. Native low-key caller equivalence outside the selected Steam windows is not claimed.

New current-state connection: selected native/event0x12 control paths and available CObjectTarget virtual presentation paths can publish key/payload records into the application-side drain. Insertion and drain/fallback are separate stages, gated locally by the same fixed busy carrier; sort equality semantics and reentry/currentness ceilings remain explicit. This narrows a formerly unexplained small producer family and connects known object/UI/request regions to a known frame-phase consumer.

## Remaining UNKNOWNs / reopening triggers

- All producers and actual current payload types/keys: need independently typed additional incoming contexts, not the direct-call list alone.
- `+460` admissible range and meaning, fallback+458 helper semantics: need a class-proven producer/consumer discriminator.
- Comparator's actual equal-key runtime behavior, scheduling/cadence and hook mutation: need concrete trace or a distinct finite executable discriminator; no fault is inferred.
- Capacity invariant, duplicate/retirement policy and safe borrower lifetime: need admitted count/backing and typed lifetime evidence, not adjacency or this unchecked insertion body.
- Actual callback/vptr/current generations and successful rendering remain unproved.

No canonical semantic ledger, old evidence, binary, asset, runtime or production source was modified. Original guards/all88/renderer236/Phase6DO_NOT_RENEW remain active.
