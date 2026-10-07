# Reading paths | Start with a real question

[← Research atlas](README.md) · [Full page index](INDEX.md) · [Open questions](unresolved.md)

**Short routes through a large archive.** The first link gives the approachable explanation; subsequent links lead to specific mechanisms or evidence. Read the scoped primary finding before editing a hook or elevating confidence.

| If you are investigating… | Start with | Then follow the evidence |
| :-- | :-- | :-- |
| **Frame time, stalls and FPS** | [Timing](engine/timing.md) | [Engine execution spine](engine/overview.md) · [QPC precision](evidence/game_time_precision/README.md) · [Legacy joystick poll](evidence/legacy_joystick_polling/README.md) |
| **SDL3, triggers and gamepad redesign** | [Input](input/README.md) | [CInput pipeline](evidence/cinput_pipeline/README.md) · [Xbox controls](input/xbox-controls.md) · [MegaRE input flow](evidence/mega_re_final_2026-10-08/maps/FLOW_CONTROLLER_INPUT.md) |
| **Native settings inside Pause / Options** | [Native UI](ui/README.md) | [Native UI evidence](evidence/native_ui/README.md) · [UI tasks in engine](engine/overview.md) |
| **NPC, culling and missing draw-distance controls** | [World distances](world/README.md) | [Frusta](world/frustum.md) · [LOD](world/lod.md) · [Shadows](world/shadows.md) · [Visibility flow](evidence/mega_re_final_2026-10-08/maps/FLOW_VISIBILITY_FRUSTUM_LOD.md) |
| **Player states and car transitions** | [Player](player/README.md) | [State families](player/state-families.md) · [Action protocol](player/action-protocol.md) · [Vehicle](player/vehicle.md) |
| **Crashes after loading / save-anywhere** | [Save map](save/README.md) | [Save/resume contract](evidence/save_resume_contract/README.md) · [Corrective verification](evidence/save_resume_contract/SaveAnywhere_Corrective_Verification_RE.md) |
| **PhysX speed, debt or vehicle cadence** | [PhysX status](physx/README.md) | [PhysX timing evidence](evidence/physx_timing/README.md) · [Frame-dependence flow](evidence/mega_re_final_2026-10-08/maps/FLOW_PHYSICS_AND_FRAME_DEPENDENCE.md) |
| **Depth, shaders, water, color** | [Rendering](render/README.md) | [Depth](render/depth.md) · [Water](render/water.md) · [Color](render/color.md) · [D3D9 resources](evidence/mega_re_final_2026-10-08/maps/FLOW_D3D9_DEVICE_RESOURCES.md) |
| **Audio service and looping playback** | [Audio](engine/audio.md) | [Audio requests](evidence/mega_re_final_2026-10-08/maps/FLOW_AUDIO.md) · [Audio architecture](engine/mega-re-final.md) |
| **Effects, particles and XWP** | [Effects](engine/effects.md) | [Animation](engine/animation.md) · [Engine overview](engine/overview.md) |
| **Resource lifetimes / crashes on unload** | [Resource loading](engine/resource-loading.md) | [Resource system flow](evidence/mega_re_final_2026-10-08/maps/FLOW_RESOURCE_SYSTEM.md) · [Corrections](evidence/mega_re_final_2026-10-08/reports/DP_RE_CORRECTIONS_FINAL.md) |
| **An unfamiliar address, class or offset** | [MegaRE synthesis](engine/mega-re-final.md) | [Address map](evidence/mega_re_final_2026-10-08/reports/DP_ENGINE_ADDRESS_MAP_FINAL.md) · [Findings catalogue](evidence/mega_re_final_2026-10-08/FINDINGS_INDEX.md) |

## A recommended research session

1. **Choose one question** from the routes above, not a loosely named “engine bug”.
2. Read the **subsystem summary**, then note the actual build and confidence boundary.
3. Open the **primary evidence** using the catalogue or the linked source.
4. Check [corrections](disproven.md) and [unresolved work](unresolved.md) before drafting a patch.
5. Keep runtime validation and the shipped ZachFix configuration separate from static conclusions.

[Browse every curated research page →](INDEX.md)
