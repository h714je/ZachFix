# Vehicle subsystem — organizing roots and callback/control responsibilities

2026-10-04, Phase4 sequences39/54. These are separate scoped callback/control and model-state production mechanisms, not full vehicle/SDK/runtime understanding.

## Accepted acquisition and interfaces

**VERIFIED reused C0127/C0128:** staticCCar008C29F0 owns two embeddedCLayout members and uses a distinct native task; CObjectCar is a separate typed0x2008 actor with table00773F74 and selected own-event5 interface. CCar-to-actor ownership remains **UNKNOWN**. H0063 construction and H0007 selected physics-facing correspondence retain their export/build/semantic limits. See `findings/boundaries/vehicle_organizing_root.md`.

## Newly reconstructed selected mechanism

**VERIFIED conditional Steam C0160/C0161:**005ED660 successfultypedCObjectCar result/word30=**0x53(decimal83)** → same-actor actual005ED853 publication of005C92C0 at+44 → conditionalevent1/worldmask-clear/post-call0x53 dispatch →00543160 nonzero1FD4/state1 → actorcontrolwrites andnumeric1FD8=2. The producer is not the accepted CCar task. Initial setterevent0 is not actualevent1 activation.

**VERIFIED direct side-helper responsibility:**005C61B0 mutatesD8/DC(mask0x8/bit3), then conditionally reaches an opaque operation using literal0139368C. **UNKNOWN** memory/lifetime/state preservation across00712D50/first nested00712D80. Laterselector/state must be evaluated after return. State1's+11D8=-1000.0f andzeroedgroups are numeric/layout facts, not wheel/velocity/SDK semantic labels. Own+A4 event1 aftertheendpoint is **UNKNOWN**, so numericstate2store isnotprovenfinalcallbackstate.

Primary sources, byte checks, independentOpus/main verification andexactfirstmissingedges: `findings/boundaries/cobjectcar_callback_control_chain.md`.

## Remaining dependencies

**UNKNOWN:** actual producer/outerowner andsafeallocation/setup; event1/state1 activation andmode/frame placement; opaque-helper preservation andown-event1 post-effect; retainedactor lifetime/teardown/free; concreteCcar-to-actor ownership; remainingGOG/Xbox/fullprotocol. B0003 stillneedsreproducibleruntimecadence evidence. Do not pursue laterstates or SDK leaves merelybecause their implementations areavailable; the boundedarchitectural chain isrecorded andglobalfrontier comparison follows.

## Phase4 sequence54 — independent actor/model production boundary

**VERIFIED C0176/C0177:** originaltypedCar fromaccepted005ED660 constructor-result lineage, firststackargumentI>=0 andtwoopaquehelper localblocks ->00541760 modelinputcopies/434numericOR ->sameactorselectedbase98production andexplicitcurrentstateevaluation. This is notadditionalcallback/control-state/SDK depth. **UNKNOWN:** sourcecontents/outerrow/index, completealgebra/opaqueposttailpreservation, resource/outputsuccess, packetvalidity/query/latestpose/frame/lifetime/GOG. findings/boundaries/cobjectcar_model_base_cache_production.md. Globalfrontierreview follows ratherthan morevehicle detail.
