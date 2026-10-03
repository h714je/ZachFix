# Actor/resource mechanism: selected CItem

**2026-10-04; Phase4 sequence50; Steam; C0171–C0172.** This adds one conditional producer/state/consumer slice to ACTOR_OBJECTS and RESOURCE_LOADING. It does not solve the general Player action gap or regrade accepted Phase1-3 roots.

## Responsibilities and object model

**VERIFIED:** selected creator004DEDA0 builds a three-dword transient payload with numeric argument2 at word0. Wrapper/factory selector0x14(decimal20) creates observed4B8 CItem, installs native004DEE70 and supplies the same payload to event0 on the original pointer. Event0 writes+4A8; event0/22 field consumption leads to row-address/format-buffer/manager-return/resource-slot transport. Direct constructor/RTTI proof is primary, not a presumed CWeapon alias.

Selected fields: +4A8 numeric input/readback; +160/+164 retained lookup-return values through shared model setup. Constructor/model hierarchy/type and registration are reused at exact scope. Parent/helper+420, exclusive owner, retirement and free are not newly established. All prior actor+50/scene/query/animation/root bounds remain.

## Lifecycle/data path

`typed CGame indexed dword input → creator transient payload → descriptor4C → same CItem native0 → numeric4A8 → later field reads / CGame835000 DC-row addresses → literal%s.XMD/%s.XPC formatter-interface buffers → distinct typedCRdData lookups (XPC first, XMD second) → sameCItem006BE6E0( R_M,R_P,0,0 ) →160/164 stores before null gates`.

**VERIFIED:** argument/pointer/field mechanics. **UNKNOWN:** N0=N_M=N_P across opaque helpers; actual formatted names; valid row/count/range; actual same-root generations; concrete returned resource classes or successful load/setup/use. Numeric semantics beyond resource-selection role are not forced.

## External boundaries and first missing edges

CGame state input connects application state to typed actor construction. Actor field consumption connects to resource manager lookup; actual return values connect back to typed CItem slots. No parser or SDK internals are reconstructed. First missing row population/content/type and value-invariance edges are carried in `CITEM_RESOURCE_VALUE_IDENTITY_AND_REBIND_POLICY`; Player emitter/current-handle/ownership gaps remain separate.

## Confidence, evidence and next research

Conditional-static scope only: normal returns, valid pointers and native callback conditions. No runtime cadence, lifetime safety, Player/Event44, GOG or Phase5 claim. Full reasoning/reproduction and exact primary refs: `findings/boundaries/citem_factory_value_resource_binding.md`. Main34/114 input plus506/1625 mechanism replays and two fresh independent branches passed; counts overlap. Global frontier reassessment follows rather than warm all-CItem/resource deepening.
