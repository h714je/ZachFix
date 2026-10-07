# Resource loading — scoped subsystem mechanism map

**2026-10-03; Phase4 active, not a subsystem/phase closeout.** Steam primary discovery; existing paired archive/root/callback scopes retained without same-address transfer to newly analyzed functions.

## Reused architecture

- **VERIFIED at accepted scope:** acquired CSingleton<CRdData> initialization/root and constructor-supplied callback (`C0035`); archive streams/extraction and descriptor population (`C0024/C0028/C0029`); selected installed typed callback branches (`C0030/C0031`); descriptor index/object/state fields (`C0036`). See `findings/formats/resource_manager_root.md`, `dpserial_archive_loader.md`, `resource_manager_population.md`, `typed_resource_dispatch.md`, and `resource_descriptor_fields.md`.
- **VERIFIED selected portfolio, not all dependencies:** named XPC → CRdPicture wrapper/record → CLevel retainer (`C0114`), and CMap numeric transitions/tag-counter pending release → acquired CRdData frame-tail processing (`C0119/C0120`). Retainer clearing/request processing is not safe or immediate resource unload. Original C0044 `+0x20`, family parser/consumer, CLevel retirement and raw world-selector bounds remain.

## New Phase4 selected request mechanism

**VERIFIED Steam:** static `01481130` CLoadThread is class-proven and separately acquired from CRdData. Its initializer-invoked start passes this exact object as OS argument to an adapter which invokes worker virtual4 `006B4DE0`. A mode0 public request can wait on a separate direct pending slot while the worker executes; the nonzero-mode branch admits a packed low16-index/control8 item to a contiguous collection. These are distinct mechanisms, not two descriptions of one queue.

The selected getter-derived `006B4F20→006B2780` call supplies acquired CRdData ECX. On callback-success registration, the exact control8 reaches descriptor `+0x2E`, status2 reaches `+0x2F`, and the twelve-dword descriptor is committed with word `+0x2C` reference increment. Normal worker return clears/decrements request control but does not certify a successful typed resource. Full-width identifier preservation, named control semantics and runtime safety are not established.

The focused canonical evidence/reasoning is `findings/boundaries/resource_worker_typed_handoff.md`. Primary helper receipts are subordinate to its scoped claims, not standalone semantic authorities.

## External boundaries and remaining obligations

The request route crosses game-owned resource state → Win32 thread/mutex operations → archive extraction and typed callback construction. Static invocation is positive; live OS success, resource-ready timing and callback cadence are **UNKNOWN**. Collection allocation/free ownership, shared manager/archive/payload lifetime, multi-producer direct-slot discipline, result propagation, concrete request-to-family/dependency matching and selected request-domain guarantees remain **UNKNOWN**.

Most typed-family grammars/consumers and matched world/actor/script retention/retirement are still unresolved at their individual original discriminators. No inherited runtime observation, a generic thread name, wrapper cardinality, or callback-compatible offset can fill those gaps. GOG worker homology and Xbox correspondence are not established by this batch.

After this coherent request reconstruction, compare independent sound/backend, animation freshness, input/control, save policy and world/UI mechanisms before more resource internals. Additional warm implementation detail is not itself a reason to stay in this subsystem. No Phase5 without later normal Phase4 closeout and independent fresh-context acceptance.
