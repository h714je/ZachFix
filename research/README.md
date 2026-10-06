# ZachFix reverse-engineering research

This directory contains the reverse-engineering record behind ZachFix.
It is intentionally separate from `docs/`: product documentation describes what the
current production build does, while `research/` records engine architecture,
restoration candidates, disproven interpretations, and unresolved questions.

## Evidence policy

Research claims use the following conservative evidence ordering:

```text
raw machine code / executable data for the build being claimed
    > decompile or recompilation output
    > same-platform cross-build agreement
    > exact cross-version homologs
    > cross-checked research notes
    > indirect platform clues
    > inference
```

Cross-version claims must name the platform and evidence level explicitly. An exact Xbox
homolog recovered from the original XEX/recomp is stronger evidence than an Xbox-derived
analogy or asset-name guess; the latter stays labeled as inference.

Semantic names are used only when mechanics support them. Attractive old labels that
were later disproven are retained in [disproven.md](disproven.md) instead of being
silently erased.

## Current Mega RE Census integration

The latest imported Census snapshot is 2026-10-04, still `PHASE_4_ACTIVE` at seq55.
New Phase 4 findings are integrated only at their written evidence scope. Most are
Steam-only conditional static mechanisms until GOG homology and/or runtime behavior is
explicitly established. Census progress counters are not treated as semantic closure.

## Current architecture index

- [engine/overview.md](engine/overview.md) - top-level scheduler, object dispatcher,
  Player spine, physics island, and major engine domains.
- [engine/phase4-mechanisms.md](engine/phase4-mechanisms.md) - 2026-10-04 Mega RE Census
  integration: resource worker, typed PhysX contexts, save staging/write, retail UI/Fade,
  world representation, vehicle/model, animation, effects/presentation, audio, item, and NPC mechanisms.
- [methodology.md](methodology.md) - durable evidence, receiver, homology, lifecycle, and
  bounded-research heuristics distilled from the Mega RE Census.
- [engine/timing.md](engine/timing.md) - main PC timing domains and known cadence
  boundaries.
- [evidence/game_time_precision/README.md](evidence/game_time_precision/README.md) -
  runtime-confirmed long-system-uptime x87 precision failure in DP's absolute-QPC
  clocks and the scoped PC53 repair boundary.
- [engine/resource-loading.md](engine/resource-loading.md) - typed Steam CLoadThread queue/direct request routes and CRdData descriptor handoff.
- [engine/audio.md](engine/audio.md) - selected CSound/PRM named-node to CSdMain/CSdCore request/status chain.
- [engine/animation.md](engine/animation.md) - typed CRdObjectModel state/matrix production and conditional same-instance packet submission.
- [engine/effects.md](engine/effects.md) - CEffect/CRdObjectEffect class layering, CEffectAdmin, XWP resources, callback/gameplay bridges, render/world integration, distance domains, and the Xbox-to-PC fixed-delta timing contract.
- [engine/crddebug.md](engine/crddebug.md) - recovered CRdDebug/developer-mode surfaces, surviving debug views, and remaining restoration boundaries.
- [ui/README.md](ui/README.md) - native CRdObject task/callback UI architecture, Pause parent integration, native input/text rendering, deferred removal, and the development-only ZachFix native-menu PoC contract.
- [save/README.md](save/README.md) - native `dp.sav`/GameRecord layout, runtime-to-persistent synchronization, save modes, resume anchors/adapters, inventory/NPC/world persistence, specialized tail registries, and safe whole-image load boundaries.
- [evidence/ceffect_xbox_timing/README.md](evidence/ceffect_xbox_timing/README.md) - compact cross-version evidence for the original Xbox fixed-delta branch, matching setters, cadence assumptions, and the remaining runtime-validation boundary.
- [evidence/cinput_pipeline/README.md](evidence/cinput_pipeline/README.md) - PC CInput object layout, producer/aggregate/pending/commit contract, held/rising/repeat semantics, dormant async-handoff path, runtime producer census, and original Xbox same-update contrast.
- [evidence/aim_mode2_precision/README.md](evidence/aim_mode2_precision/README.md) - mode-2 aim target/camera handoff, x87 precision-sensitive exact-equality hazard, PC/Xbox mode-2 reset-policy divergence, Alt+Tab recovery mechanics, and full shipped CEvent A6/4 content census.
- [evidence/game_record/README.md](evidence/game_record/README.md) - canonical byte-level `dp.sav`/GameRecord evidence and the complete 2026-09-30 persistence map.
- [evidence/save_resume_contract/README.md](evidence/save_resume_contract/README.md) - native save/resume control-flow evidence, save-mode policy, positional/scripted resume, post-load reconstruction, and special one-shot resume adapters. Final disputed-claim audit: [SaveAnywhere_Corrective_Verification_RE.md](evidence/save_resume_contract/SaveAnywhere_Corrective_Verification_RE.md).
- [evidence/native_ui/README.md](evidence/native_ui/README.md) - exact Steam/GOG Native UI address map, generic selector-0 task ABI, retail menu reference, COption/CLayout corrections, Pause parent evidence, frame ordering, and PoC safety requirements.
- [evidence/physx_timing/README.md](evidence/physx_timing/README.md) - 2026-10-01 Steam/GOG PhysX timing closure: common scene transaction, special records, queue/worker/catch-up homologs, fixed-step debt semantics, CCT/vehicle/Event-6 boundaries, retired interpretations, and the three remaining runtime blockers.
- [evidence/mega_re_census_2026-10-04/README.md](evidence/mega_re_census_2026-10-04/README.md) - preserved source slice from the latest Mega RE Census used for the 2026-10-04 integration, including exact Phase 4 boundary reports and their scope limits.
- [input/README.md](input/README.md) - physical input -> CInput -> gameplay consumers,
  Native Gamepad bridge, CInput latency, camera modes, and Xbox-only control findings. Detailed
  maps: [camera-modes.md](input/camera-modes.md) and [xbox-controls.md](input/xbox-controls.md).
- [player/README.md](player/README.md) - CPlayer state families, action packet,
  selector/state-domain separation, CCT bridge, and vehicle handoff. Detailed maps:
  [state-families.md](player/state-families.md), [action-protocol.md](player/action-protocol.md),
  [action_selector.md](player/action_selector.md), [object-action-taxonomy.md](player/object-action-taxonomy.md),
  [vehicle.md](player/vehicle.md), and [cct-bridge.md](player/cct-bridge.md).
- [world/README.md](world/README.md) - streaming, six main-frustum classes,
  activation, mesh LOD, alternate low-detail 3D residency, and shadow distance. Detailed
  maps: [frustum.md](world/frustum.md), [lod.md](world/lod.md), [residency.md](world/residency.md),
  and [shadows.md](world/shadows.md).
- [render/README.md](render/README.md) - renderer/restoration findings and the current
  status of depth, water, terrain, trees, day/night, and interior visibility work. Detailed
  retained branches include [depth.md](render/depth.md), [color.md](render/color.md),
  [water.md](render/water.md), [daynight.md](render/daynight.md), and
  [interior-visibility.md](render/interior-visibility.md).
- [physx/README.md](physx/README.md) - final PhysX timing investigation, the 2026-10-01 closure pass, and why no production physics patch is shipped.
- [disproven.md](disproven.md) - corrected interpretations that should not be
  rediscovered.
- [unresolved.md](unresolved.md) - remaining research targets.

## Production boundary

Research findings are not automatically production features. A candidate moves into
ZachFix only after its owning subsystem, call-site scope, failure modes, supported
builds, and runtime behavior are understood well enough to fail closed.

The clearest example is PhysX timing: several real defects were confirmed, but local
repairs repeatedly exposed incompatible assumptions in other timing domains. The
production tree therefore leaves native physics timing untouched.
