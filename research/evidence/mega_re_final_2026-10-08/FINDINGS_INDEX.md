# MegaRE primary findings | Complete catalogue

[← MegaRE index](INDEX.md) · [Readable synthesis](../../engine/mega-re-final.md) · [Maps & reports](MAPS_REPORTS_INDEX.md)

**281 finding pages**, plus five category README files (286 Markdown files total). Entries link directly to the untouched source documents. Use the title to locate a mechanism; the proof status must be read from the document, not guessed from a filename.

## Boundaries · 225

| Primary finding | Source file |
| :-- | :-- |
| [Actor / Effect State Cleanup Boundary](findings/boundaries/actor_effect_cleanup.md) | `actor_effect_cleanup` |
| [Typed model animation state to same-instance submission](findings/boundaries/animation_model_state_submission.md) | `animation_model_state_submission` |
| [Application Idle Frame Loop](findings/boundaries/application_idle_frame_loop.md) | `application_idle_frame_loop` |
| [Application post-dispatch media phase placement](findings/boundaries/application_media_phase_placement.md) | `application_media_phase_placement` |
| [Application to Object-Dispatch Boundary](findings/boundaries/application_object_dispatch.md) | `application_object_dispatch` |
| [Steam audio descriptor → named inline record → retained request chain](findings/boundaries/audio_descriptor_request_record_chain.md) | `audio_descriptor_request_record_chain` |
| [Static CSdCore -> shared backend/storage cleanup](findings/boundaries/audio_static_core_backend_cleanup.md) | `audio_static_core_backend_cleanup` |
| [C0098 storage-model dependency reconciliation — Phases 1–7](findings/boundaries/c0098_storage_dependency_reconciliation.md) | `c0098_storage_dependency_reconciliation` |
| [CDemo context-selector producer and selected exit](findings/boundaries/cdemo_context_selector_provenance.md) | `cdemo_context_selector_provenance` |
| [CEvent Stream Initial-Producer Census](findings/boundaries/cevent_stream_initial_producer_census.md) | `cevent_stream_initial_producer_census` |
| [CEvent Stream Writer `00435110/00435190` Negative](findings/boundaries/cevent_stream_writer_00435110_negative.md) | `cevent_stream_writer_00435110_negative` |
| [CEvent Stream Writer `00435AE0/00435B60` Negative](findings/boundaries/cevent_stream_writer_00435ae0_negative.md) | `cevent_stream_writer_00435ae0_negative` |
| [CGame embedded record restore and typed CPlayer reconstruction](findings/boundaries/cgame_record_reconstruction_root.md) | `cgame_record_reconstruction_root` |
| [CItem factory payload, numeric field and resource-slot binding](findings/boundaries/citem_factory_value_resource_binding.md) | `citem_factory_value_resource_binding` |
| [CLevel Selected-Attachment Lifecycle and Transform Update](findings/boundaries/clevel_selected_attachment_lifecycle.md) | `clevel_selected_attachment_lifecycle` |
| [CLevel Virtual +0x4C Mapping](findings/boundaries/clevel_virtual_4c.md) | `clevel_virtual_4c` |
| [CLevel Virtual Attachment-Selection Boundary](findings/boundaries/clevel_virtual_attachment_selection.md) | `clevel_virtual_attachment_selection` |
| [CMap numeric state, retainer reset and resource-phase placement](findings/boundaries/cmap_state_resource_phase_placement.md) | `cmap_state_resource_phase_placement` |
| [CMap static world/level organizing root](findings/boundaries/cmap_world_level_owner_root.md) | `cmap_world_level_owner_root` |
| [Native UI → CFade predicate → camera policy chain](findings/boundaries/cmenu_fade_predicate_camera_policy.md) | `cmenu_fade_predicate_camera_policy` |
| [CMenu numeric policy → selected value → CGame indexed mutation](findings/boundaries/cmenu_numeric_game_slot_commit.md) | `cmenu_numeric_game_slot_commit` |
| [CMenu organizing receiver, concrete model task and selected input/camera use](findings/boundaries/cmenu_organizing_task_use.md) | `cmenu_organizing_task_use` |
| [CMessage Constructor and Table-Base Source Limit](findings/boundaries/cmessage_constructor_table_base_limit.md) | `cmessage_constructor_table_base_limit` |
| [CMessage High-Centrality Dispatcher](findings/boundaries/cmessage_high_centrality_dispatcher.md) | `cmessage_high_centrality_dispatcher` |
| [GOG CMustachesAdmin Creator Raw Recovery](findings/boundaries/cmustache_gog_creator_raw_recovery.md) | `cmustache_gog_creator_raw_recovery` |
| [CMustachesAdmin Parent Handle Access Limit](findings/boundaries/cmustache_parent_handle_access_limit.md) | `cmustache_parent_handle_access_limit` |
| [CMustachesAdmin-Owned CMustache Active Objects](findings/boundaries/cmustachesadmin_cmustache_children.md) | `cmustachesadmin_cmustache_children` |
| [CNpcKiller Factory Request Stack Lifetime](findings/boundaries/cnpc_factory_request_stack_lifetime.md) | `cnpc_factory_request_stack_lifetime` |
| [CNpcKiller Factory-to-Registry Registration](findings/boundaries/cnpc_killer_factory_registration.md) | `cnpc_killer_factory_registration` |
| [CObjectCar producer → installed callback → selected numeric control endpoint](findings/boundaries/cobjectcar_callback_control_chain.md) | `cobjectcar_callback_control_chain` |
| [CObjectCar inputs → model base-state preparation → explicit evaluation](findings/boundaries/cobjectcar_model_base_cache_production.md) | `cobjectcar_model_base_cache_production` |
| [CObjectSpecies XAM Receiver Transition](findings/boundaries/cobjectspecies_xam_receiver_transition.md) | `cobjectspecies_xam_receiver_transition` |
| [Corrected Phase 6 residual sufficiency — primary necessity review](findings/boundaries/corrected_phase6_residual_sufficiency_2026-10-07.md) | `corrected_phase6_residual_sufficiency_2026-10-07` |
| [CPhysicsCore acquired root and selected lifecycle placement](findings/boundaries/cphysicscore_lifecycle_root.md) | `cphysicscore_lifecycle_root` |
| [CRdMovie organizing root above DirectShow](findings/boundaries/crdmovie_directshow_owner.md) | `crdmovie_directshow_owner` |
| [CRdPicture / CLevel Resource Attachment](findings/boundaries/crdpicture_clevel_resource_attachment.md) | `crdpicture_clevel_resource_attachment` |
| [CRdSceneDraw active-manager and CRdObject interface integration](findings/boundaries/crdscenedraw_active_manager_interface.md) | `crdscenedraw_active_manager_interface` |
| [CScenedemoPostEffect Singleton Lifecycle Limit](findings/boundaries/cscenedemo_post_effect_lifecycle_limit.md) | `cscenedemo_post_effect_lifecycle_limit` |
| [CShop positive callback-to-retirement chain](findings/boundaries/cshop_callback_retirement.md) | `cshop_callback_retirement` |
| [CShop factory-result retention and deletion interface](findings/boundaries/cshop_external_handles.md) | `cshop_external_handles` |
| [CThrowLure Dispatcher Direct-Caller Limit and CScenedemoPostEffect Ownership](findings/boundaries/cthrowlure_dispatcher_direct_caller_limit.md) | `cthrowlure_dispatcher_direct_caller_limit` |
| [CThrowLure Double Registration and Handle Invariant](findings/boundaries/cthrowlure_registration_invariant.md) | `cthrowlure_registration_invariant` |
| [CThrowLure Retirement Marker and Iterator Census](findings/boundaries/cthrowlure_retirement_marker_iterator.md) | `cthrowlure_retirement_marker_iterator` |
| [CThrowLure Retirement, Reset, and Stale-Handle Scope](findings/boundaries/cthrowlure_retirement_reset_scope.md) | `cthrowlure_retirement_reset_scope` |
| [D3D9 Device Resource Reset Lifecycle](findings/boundaries/d3d9_device_resource_reset_lifecycle.md) | `d3d9_device_resource_reset_lifecycle` |
| [D3D9 2D Texture Registry Layout](findings/boundaries/d3d9_texture_registry_layout.md) | `d3d9_texture_registry_layout` |
| [D3D9 Texture Registry Provenance Limit](findings/boundaries/d3d9_texture_registry_provenance_limit.md) | `d3d9_texture_registry_provenance_limit` |
| [Direct Active-Object Registration Producer Census](findings/boundaries/direct_active_registration_producer_census.md) | `direct_active_registration_producer_census` |
| [DSB Event-Core Dispatch Boundary](findings/boundaries/dsb_event_core_dispatch.md) | `dsb_event_core_dispatch` |
| [Independent effects organizing roots and selected use](findings/boundaries/effects_organizing_roots_and_use.md) | `effects_organizing_roots_and_use` |
| [Embedded Object Lifecycle Pattern and Selector-Factory Handle Production](findings/boundaries/embedded_object_lifecycle_pattern.md) | `embedded_object_lifecycle_pattern` |
| [CGame-relative control bank → captured Input query and custom-register message tokens](findings/boundaries/final_static_control_bank_message_tokens.md) | `final_static_control_bank_message_tokens` |
| [Private XLY/layout text banks → conditional draw-state override → save message gate](findings/boundaries/final_static_layout_text_save_presentation.md) | `final_static_layout_text_save_presentation` |
| [Optional record admission, deferred delivery and typed slot98 producer](findings/boundaries/final_static_optional_record_admission.md) | `final_static_optional_record_admission` |
| [Parent/index attachment carrier → child transform](findings/boundaries/final_static_parent_matrix_attachment.md) | `final_static_parent_matrix_attachment` |
| [Numeric player-resource carriers → available CShot → hybrid scene query → native hit packet](findings/boundaries/final_static_player_shot_hybrid_query.md) | `final_static_player_shot_hybrid_query` |
| [Fishing typed roots, conditional update and new line retainer](findings/boundaries/fishing_primary_owner_update.md) | `fishing_primary_owner_update` |
| [Frame D3D9 Cooperative-Level Gate](findings/boundaries/frame_d3d9_cooperative_gate.md) | `frame_d3d9_cooperative_gate` |
| [Independent gameplay-audio organizing roots and conditional use](findings/boundaries/gameplay_audio_organizing_roots.md) | `gameplay_audio_organizing_roots` |
| [Steam CInput / CCamera roots and sample-to-camera boundary](findings/boundaries/input_camera_primary_roots.md) | `input_camera_primary_roots` |
| [Message / Overlay Dispatch Boundary](findings/boundaries/message_overlay_dispatch.md) | `message_overlay_dispatch` |
| [Native-Task Availability Reader Scope](findings/boundaries/native_task_availability_reader_scope.md) | `native_task_availability_reader_scope` |
| [Native Task Availability Reset Boundary](findings/boundaries/native_task_availability_reset_boundary.md) | `native_task_availability_reset_boundary` |
| [Native Task Callback Lifecycle — `00647730 / 00647680`](findings/boundaries/native_task_callback_lifecycle.md) | `native_task_callback_lifecycle` |
| [Native Task Connective Architecture](findings/boundaries/native_task_connective_architecture.md) | `native_task_connective_architecture` |
| [Native Task Non-Initial Event Producer Limit](findings/boundaries/native_task_noninitial_event_producer_limit.md) | `native_task_noninitial_event_producer_limit` |
| [Native Task Private State Region — Initial Field Map](findings/boundaries/native_task_state_region.md) | `native_task_state_region` |
| [Native UI Task Factory to Object Manager Boundary](findings/boundaries/native_ui_object_manager.md) | `native_ui_object_manager` |
| [CNpcEnemy camera scalar, numeric-bit policy and marker boundary](findings/boundaries/npc_enemy_camera_scalar_marker_policy.md) | `npc_enemy_camera_scalar_marker_policy` |
| [CNpcRecord Embedded Lifecycle and Factory Descriptor Boundary](findings/boundaries/npc_record_embedded_lifecycle.md) | `npc_record_embedded_lifecycle` |
| [Active-Object / CRdHandleUtil Handle Lifecycle](findings/boundaries/object_handle_lifecycle.md) | `object_handle_lifecycle` |
| [Object Dispatcher to PhysX Boundary](findings/boundaries/object_physx.md) | `object_physx` |
| [Object Registry Handle Lookup Boundary](findings/boundaries/object_registry_handle_lookup.md) | `object_registry_handle_lookup` |
| [Generic Object Virtual-Phase Boundary](findings/boundaries/object_virtual_phases.md) | `object_virtual_phases` |
| [Client request operand to typed current Main and selected record operation](findings/boundaries/phase5_audio_client_selected_record_operation.md) | `phase5_audio_client_selected_record_operation` |
| [Audio singleton cleanup before optional receiver deallocation](findings/boundaries/phase5_audio_singleton_cleanup_interface_limit.md) | `phase5_audio_singleton_cleanup_interface_limit` |
| [CCar retained result: selected CGame interior address, not returned vehicle actor](findings/boundaries/phase5_ccar_retained_game_interior_address.md) | `phase5_ccar_retained_game_interior_address` |
| [CGame saved-receiver inline cleanup composition and available outer boundary](findings/boundaries/phase5_cgame_inline_layout_composition.md) | `phase5_cgame_inline_layout_composition` |
| [CMap post-mark reset, Lensflare shell publication and generic pointer-vector seam](findings/boundaries/phase5_cmap_reset_lensflare_cache_publication.md) | `phase5_cmap_reset_lensflare_cache_publication` |
| [CMenu task retirement, distinct Fade retainer and inline-layout reset](findings/boundaries/phase5_cmenu_task_layout_retirement.md) | `phase5_cmenu_task_layout_retirement` |
| [Phase5 CRdMovie → constructed CRdTexture → setup descriptor interfaces](findings/boundaries/phase5_crdmovie_constructed_texture_setup_descriptor.md) | `phase5_crdmovie_constructed_texture_setup_descriptor` |
| [CFishingPerson cold 7F0 shell retirement and first descendant break](findings/boundaries/phase5_fishing_cold_7f0_shell_retirement.md) | `phase5_fishing_cold_7f0_shell_retirement` |
| [Fishing parent event2, line deletion interface and unresolved lure alias](findings/boundaries/phase5_fishing_parent_line_aux_retirement.md) | `phase5_fishing_parent_line_aux_retirement` |
| [Phase5 HookChain inline-loop operand / available-retirement limit](findings/boundaries/phase5_hookchain_csoundloop_operand_retirement_limit.md) | `phase5_hookchain_csoundloop_operand_retirement_limit` |
| [Input provider arithmetic and separately registered current-root finalizers](findings/boundaries/phase5_input_provider_accounting_finalizers.md) | `phase5_input_provider_accounting_finalizers` |
| [CInput registered finalizer, dependency and callback/member lifetime](findings/boundaries/phase5_input_registered_finalizer_lifetime.md) | `phase5_input_registered_finalizer_lifetime` |
| [Typed model buffer/packet retirement versus replacement and scene borrowing](findings/boundaries/phase5_model_buffer_packet_retirement.md) | `phase5_model_buffer_packet_retirement` |
| [Phase5 — Model/CRdInterp resource-link clear and backing-boundary attempts](findings/boundaries/phase5_model_crdinterp_backing_reset_boundary.md) | `phase5_model_crdinterp_backing_reset_boundary` |
| [Movie task event2 retirement candidate — finite local limit](findings/boundaries/phase5_movie_event2_retirement_candidate_limit.md) | `phase5_movie_event2_retirement_candidate_limit` |
| [Phase5 NPC event2 — selected current virtual90 interface, nominated direct route rejected](findings/boundaries/phase5_npc_event2_selected_virtual90_routing_limit.md) | `phase5_npc_event2_selected_virtual90_routing_limit` |
| [Physics scene/controller dependency release and generation barriers](findings/boundaries/phase5_physics_scene_controller_release.md) | `phase5_physics_scene_controller_release` |
| [Player teardown: two independent physics-root/operand requests](findings/boundaries/phase5_player_physics_dependency_teardown.md) | `phase5_player_physics_dependency_teardown` |
| [CRdData shutdown versus CLoadThread retirement](findings/boundaries/phase5_resource_worker_shutdown_coordination.md) | `phase5_resource_worker_shutdown_coordination` |
| [Save/Event core-member value transport — not reference retirement](findings/boundaries/phase5_save_event_core_member_value_transport.md) | `phase5_save_event_core_member_value_transport` |
| [Save preparation: live Game/world/Player value transport, not stable snapshot](findings/boundaries/phase5_save_preparation_world_player_values.md) | `phase5_save_preparation_world_player_values` |
| [SaveData/Sysutil deferred current-root finalization and callback lifetime](findings/boundaries/phase5_save_sysutil_deferred_root_lifetime.md) | `phase5_save_sysutil_deferred_root_lifetime` |
| [Shared delay progress versus finalizer/callback retirement completion](findings/boundaries/phase5_shared_progress_delay_retirement_limit.md) | `phase5_shared_progress_delay_retirement_limit` |
| [Typed light node detach and available cleanup dispatch limit](findings/boundaries/phase5_spatial_light_detach_cleanup.md) | `phase5_spatial_light_detach_cleanup` |
| [Phase5 composite descriptor consumer — two-arm opaque-request limit](findings/boundaries/phase5_texture_descriptor_consumer_request_limit.md) | `phase5_texture_descriptor_consumer_request_limit` |
| [CMap Rain retainer: local request/clear versus separate cleanup availability](findings/boundaries/phase5_weather_retainer_grid_retirement.md) | `phase5_weather_retainer_grid_retirement` |
| [World/resource retirement: marks, retained references and missing generation join](findings/boundaries/phase5_world_resource_retirement.md) | `phase5_world_resource_retirement` |
| [Camera candidate source geometry residual — sequence 145](findings/boundaries/phase6_camera_source_geometry_residual.md) | `phase6_camera_source_geometry_residual` |
| [CEvThreadEx virtual mode coordinator — sequence 145](findings/boundaries/phase6_ceventhreadex_virtual_mode_coordinator.md) | `phase6_ceventhreadex_virtual_mode_coordinator` |
| [CFunc bulk control and selected CEvent receiver join — Phase6 Steam](findings/boundaries/phase6_cfunc_bulk_control_and_event_receiver_join.md) | `phase6_cfunc_bulk_control_and_event_receiver_join` |
| [CGame mask-pair query and CRdData pending tuple wrapper — sequence 146 acquisition](findings/boundaries/phase6_cgame_mask_pair_and_resource_tuple_wrapper.md) | `phase6_cgame_mask_pair_and_resource_tuple_wrapper` |
| [Cold receiver control and CEvThreadEx initializer prefix — sequence 147](findings/boundaries/phase6_cold_receiver_control_and_thread_initializer_prefix.md) | `phase6_cold_receiver_control_and_thread_initializer_prefix` |
| [Core-compatible current banks and fixed receiver recipes — sequence160](findings/boundaries/phase6_core_current_bank_and_fixed_recipes_seq0160.md) | `phase6_core_current_bank_and_fixed_recipes_seq0160` |
| [Core member binding and follow-up request surface — sequence155](findings/boundaries/phase6_core_member_binding_and_followup_requests.md) | `phase6_core_member_binding_and_followup_requests` |
| [Reviewed Core-compatible member request surface — sequence158](findings/boundaries/phase6_core_member_requests_reviewed_seq0158.md) | `phase6_core_member_requests_reviewed_seq0158` |
| [Typed CEvCore acquisition, configuration descriptor and independent static cohorts — sequence153](findings/boundaries/phase6_core_typed_root_descriptor_and_static_cohorts.md) | `phase6_core_typed_root_descriptor_and_static_cohorts` |
| [Available CEffectAdmin: selected resource/setup/active-registration placement](findings/boundaries/phase6_effectadmin_selected_resource_registration.md) | `phase6_effectadmin_selected_resource_registration` |
| [Event-associated numeric transition/reset organizer — Steam Phase6](findings/boundaries/phase6_event_associated_numeric_transition_organizer.md) | `phase6_event_associated_numeric_transition_organizer` |
| [Event incoming transport and fixed-word mask wrapper — sequence141](findings/boundaries/phase6_event_incoming_and_fixed_word_mask_wrapper.md) | `phase6_event_incoming_and_fixed_word_mask_wrapper` |
| [Event mode coordinator and qualified CMessage auxiliary references — sequence140](findings/boundaries/phase6_event_mode_coordinator_and_cmessage_auxiliary.md) | `phase6_event_mode_coordinator_and_cmessage_auxiliary` |
| [CEvThreadEx raw available reset path — sequence147 acquisition](findings/boundaries/phase6_evthreadex_raw_available_reset_path.md) | `phase6_evthreadex_raw_available_reset_path` |
| [Fixed-word mutex hook qualification — fresh sequence 143](findings/boundaries/phase6_fixed_word_mutex_hook_qualification.md) | `phase6_fixed_word_mutex_hook_qualification` |
| [GOG rank27 available allocation/request surface — source qualified159, promotion pending160](findings/boundaries/phase6_gog_allocation_request_qualification_seq0160.md) | `phase6_gog_allocation_request_qualification_seq0160` |
| [Phase 6 sequence 143 — GOG `0040A2F0` accessor qualification card](findings/boundaries/phase6_gog_cgame_accessor_qualification.md) | `phase6_gog_cgame_accessor_qualification` |
| [GOG rank63 shared-global candidate — source159, promotion pending160](findings/boundaries/phase6_gog_rank63_local_string_interface_seq0160.md) | `phase6_gog_rank63_local_string_interface_seq0160` |
| [Independent rank36 input/selection control — bounded result](findings/boundaries/phase6_independent_input_selection_control_limit.md) | `phase6_independent_input_selection_control_limit` |
| [Shared in-place static pointer-tracking root — Steam Phase6](findings/boundaries/phase6_inplace_static_pointer_tracking_root.md) | `phase6_inplace_static_pointer_tracking_root` |
| [Menu-facing shared-record/preservation phase bridge — sequence153](findings/boundaries/phase6_menu_shared_record_phase_bridge.md) | `phase6_menu_shared_record_phase_bridge` |
| [Named resource request and retained cold source — sequence152](findings/boundaries/phase6_named_resource_request_and_cold_source_qualification.md) | `phase6_named_resource_request_and_cold_source_qualification` |
| [Native instruction source → available texture → D3DX import attempt](findings/boundaries/phase6_native_instruction_texture_import.md) | `phase6_native_instruction_texture_import` |
| [Selected profile-name resource request/result publication — sequence196 primary qualification](findings/boundaries/phase6_native_profile_resource_result_publication.md) | `phase6_native_profile_resource_result_publication` |
| [Object/Core/camera selected local control — sequence160](findings/boundaries/phase6_object_core_camera_local_control_seq0160.md) | `phase6_object_core_camera_local_control_seq0160` |
| [Optional callback word and second record delivery context — sequence 145](findings/boundaries/phase6_optional_callback_record_delivery.md) | `phase6_optional_callback_record_delivery` |
| [Rank14 local control qualification — sequence155](findings/boundaries/phase6_rank14_local_control_qualification.md) | `phase6_rank14_local_control_qualification` |
| [Rank15 event-adjacent candidate: bounded qualification](findings/boundaries/phase6_rank15_event_candidate_qualification.md) | `phase6_rank15_event_candidate_qualification` |
| [Phase6B shared high-centrality service qualification](findings/boundaries/phase6_shared_service_qualification.md) | `phase6_shared_service_qualification` |
| [Short gate and record scan qualification — sequence 143 acquisition](findings/boundaries/phase6_short_gate_and_record_scan_qualification.md) | `phase6_short_gate_and_record_scan_qualification` |
| [Static thread-vector request and Demo-selected Level configuration — sequence149](findings/boundaries/phase6_static_thread_vector_and_demo_level_configuration.md) | `phase6_static_thread_vector_and_demo_level_configuration` |
| [Static tracker shell teardown and separate CRdModel cache root](findings/boundaries/phase6_static_tracker_shell_teardown_and_crdmodel_cache.md) | `phase6_static_tracker_shell_teardown_and_crdmodel_cache` |
| [Static-vector address publication and constructor ABI — sequence152](findings/boundaries/phase6_static_vector_address_publication_and_abi.md) | `phase6_static_vector_address_publication_and_abi` |
| [Timer cache/root and CFunc shared-word interfaces — sequence140](findings/boundaries/phase6_timer_root_and_bulk_shared_word_interfaces.md) | `phase6_timer_root_and_bulk_shared_word_interfaces` |
| [In-place tracker: separate application raw-pointer deallocation boundary](findings/boundaries/phase6_tracker_application_raw_pointer_retirement.md) | `phase6_tracker_application_raw_pointer_retirement` |
| [Phase7 A01 — MEDIA_COMPLETION_ATTRIBUTION](findings/boundaries/phase7_architecture_a01_falsification.md) | `phase7_architecture_a01_falsification` |
| [Phase7 A02 — SCENE_MEMBERSHIP_COHERENCE](findings/boundaries/phase7_architecture_a02_falsification.md) | `phase7_architecture_a02_falsification` |
| [Phase7 A03 — ANIMATION_RESOURCE_PUBLICATION](findings/boundaries/phase7_architecture_a03_falsification.md) | `phase7_architecture_a03_falsification` |
| [Phase7 A04 — DEFERRED_RESOURCE_CALLBACK_RETIREMENT](findings/boundaries/phase7_architecture_a04_falsification.md) | `phase7_architecture_a04_falsification` |
| [Phase7 A05 — AUDIO_TOKEN_EPOCH_AND_INSTANCE_IDENTITY](findings/boundaries/phase7_architecture_a05_falsification.md) | `phase7_architecture_a05_falsification` |
| [Phase7 A06 — CAMERA_MODE9_NON_GENERIC_SCHEDULING](findings/boundaries/phase7_architecture_a06_falsification.md) | `phase7_architecture_a06_falsification` |
| [Phase7 A07 — SYNCHRONOUS_PHYSICS_RESULT_AUTHORITY](findings/boundaries/phase7_architecture_a07_falsification.md) | `phase7_architecture_a07_falsification` |
| [Phase7 A08 — NATIVE_UI_RECORD_SELECTOR_AND_CACHE_DOMAINS](findings/boundaries/phase7_architecture_a08_falsification.md) | `phase7_architecture_a08_falsification` |
| [Phase7 A09 — PHYSICS_CONTROLLER_SLOT_CREATION_PROVIDER](findings/boundaries/phase7_architecture_a09_falsification.md) | `phase7_architecture_a09_falsification` |
| [Phase7 A10 — XCA_INTERPOLATOR_INITIALIZATION_GUARANTEES](findings/boundaries/phase7_architecture_a10_falsification.md) | `phase7_architecture_a10_falsification` |
| [Phase7 A11 — XAM_BINDING_RESOURCE_IDENTITY_AND_STORAGE_OWNERSHIP](findings/boundaries/phase7_architecture_a11_falsification.md) | `phase7_architecture_a11_falsification` |
| [Phase7 A12 — ACTOR_STATE_POPULATION_AND_HOMOLOGY_COMPLETENESS](findings/boundaries/phase7_architecture_a12_falsification.md) | `phase7_architecture_a12_falsification` |
| [Phase7 A13 — NPC_EVENT2_CONDITIONAL_TYPED_IMPLEMENTATION](findings/boundaries/phase7_architecture_a13_falsification.md) | `phase7_architecture_a13_falsification` |
| [Phase7 A14 — MODEL_CLEANUP_PACKET_COMPOSITION_AND_FRESHNESS](findings/boundaries/phase7_architecture_a14_falsification.md) | `phase7_architecture_a14_falsification` |
| [Phase7 A15 — ACTUATOR_FORMAL_REQUEST_CONSUMPTION_AND_PAIR_ATOMICITY](findings/boundaries/phase7_architecture_a15_falsification.md) | `phase7_architecture_a15_falsification` |
| [Phase7 A16 — EFFECT_ADMIN_SERVICE_USE_DELETE_AND_SHARED_STAGING](findings/boundaries/phase7_architecture_a16_falsification.md) | `phase7_architecture_a16_falsification` |
| [Phase7 A17 — ITEM_MANAGER_SELECTOR_ABI_RESOURCE_TUPLE_AND_READINESS](findings/boundaries/phase7_architecture_a17_falsification.md) | `phase7_architecture_a17_falsification` |
| [C0117 / BND-085 — exact-fragment Phase 7 falsification](findings/boundaries/phase7_c0117_exact_fragment_falsification.md) | `phase7_c0117_exact_fragment_falsification` |
| [Phase7 closeout P0 — typed candidates, numeric admission, and the remaining live join](findings/boundaries/phase7_closeout_p0_lineage_admission.md) | `phase7_closeout_p0_lineage_admission` |
| [Phase 7 extended Core-composition falsification — four finite dispositions](findings/boundaries/phase7_core_composition_extended_falsification.md) | `phase7_core_composition_extended_falsification` |
| [Phase 7 — CEvent/numeric-organizer association falsification](findings/boundaries/phase7_event_organizer_falsification.md) | `phase7_event_organizer_falsification` |
| [Phase7 P7X03 — factory transport, registration and concrete callback dependencies](findings/boundaries/phase7_factory_transport_registration_falsification.md) | `phase7_factory_transport_registration_falsification` |
| [Phase7 P7X06 — renderer descriptor contradiction and reset limits](findings/boundaries/phase7_renderer_descriptor_reset_falsification.md) | `phase7_renderer_descriptor_reset_falsification` |
| [Phase7 P7X05 — resource request identity, completion and conditional commit](findings/boundaries/phase7_resource_request_completion_falsification.md) | `phase7_resource_request_completion_falsification` |
| [Phase7 P7X04 — save admission, staging and physical-result separation](findings/boundaries/phase7_save_transaction_falsification.md) | `phase7_save_transaction_falsification` |
| [Phase7 P7X02 — Timer gate versus fixed-word authority](findings/boundaries/phase7_timer_fixed_word_falsification.md) | `phase7_timer_fixed_word_falsification` |
| [Phase7 P7X01 — tracker retirement versus shell cleanup](findings/boundaries/phase7_tracker_retirement_falsification.md) | `phase7_tracker_retirement_falsification` |
| [Physics fixed-context record vector and available activation mechanism](findings/boundaries/physics_context_vector_activation.md) | `physics_context_vector_activation` |
| [PGC005 — available Actuator record-access surface; executor still unjoined](findings/boundaries/post_phase8_actuator_record_access_limit.md) | `post_phase8_actuator_record_access_limit` |
| [PGC007 — typed audio FILEITEM population and selected metadata boundary](findings/boundaries/post_phase8_audio_fileitem_population.md) | `post_phase8_audio_fileitem_population` |
| [PGC004 — CPut resource binding and activation bridge](findings/boundaries/post_phase8_cput_resource_binding_activation.md) | `post_phase8_cput_resource_binding_activation` |
| [PGC002/PGC003 — Effect and Item table producer/admission bridges](findings/boundaries/post_phase8_effect_item_table_admission.md) | `post_phase8_effect_item_table_admission` |
| [PGC001 — CMessage resource-result binder → directory/category bridge](findings/boundaries/post_phase8_native_ui_resource_directory_installation.md) | `post_phase8_native_ui_resource_directory_installation` |
| [PGC006 — selected reflection constant → varying → projected texture sample](findings/boundaries/post_phase8_reflection_selected_shader_sample.md) | `post_phase8_reflection_selected_shader_sample` |
| [Typed movie request → task/private state → CFade data and presentation latch](findings/boundaries/presentation_movie_fade_latch_chain.md) | `presentation_movie_fade_latch_chain` |
| [CRdData Resource Handle to Object Dispatch](findings/boundaries/resource_handle_object_dispatch.md) | `resource_handle_object_dispatch` |
| [CRdData +0x1C XMD/XPC Lifecycle Boundary](findings/boundaries/resource_tail_xmd_xpc_lifecycle.md) | `resource_tail_xmd_xpc_lifecycle` |
| [Steam resource worker request-to-record handoff](findings/boundaries/resource_worker_typed_handoff.md) | `resource_worker_typed_handoff` |
| [Save disk → staged image → selected CGame record mechanism](findings/boundaries/save_disk_staging_record_chain.md) | `save_disk_staging_record_chain` |
| [Save writer / live staging / operation-6 physical write boundary](findings/boundaries/save_writer_staging_write_policy.md) | `save_writer_staging_write_policy` |
| [Scene query member: COctTree lifecycle and object membership protocol](findings/boundaries/scene_query_member_lifecycle.md) | `scene_query_member_lifecycle` |
| [Scene submission pointer pipeline and typed model-packet interfaces](findings/boundaries/scene_submission_pointer_pipeline.md) | `scene_submission_pointer_pipeline` |
| [Selector 0x1A / 0x1B Effect Classes](findings/boundaries/selector_effect_classes.md) | `selector_effect_classes` |
| [Selector Object Factory Boundary](findings/boundaries/selector_object_factory.md) | `selector_object_factory` |
| [Shared Actor Attachment Interface — Scope Correction](findings/boundaries/shared_actor_attachment_interface_correction.md) | `shared_actor_attachment_interface_correction` |
| [Shared Actor Virtual `+0x50` — Generic Dispatch Negative](findings/boundaries/shared_actor_vslot50_generic_dispatch_negative.md) | `shared_actor_vslot50_generic_dispatch_negative` |
| [TBC009 — CSound coordination ID to row to Main-operand transport](findings/boundaries/targeted_bridge_audio_coordination_id_transport.md) | `targeted_bridge_audio_coordination_id_transport` |
| [TBC006 — CEffect numeric distance flag to inherited part-update gate](findings/boundaries/targeted_bridge_effect_distance_part_update_gate.md) | `targeted_bridge_effect_distance_part_update_gate` |
| [TBC002 — numeric transition state to extra-presentation gate](findings/boundaries/targeted_bridge_extra_presentation_producer.md) | `targeted_bridge_extra_presentation_producer` |
| [TBC011 — GOG installed callback to conditional typed audio continuation](findings/boundaries/targeted_bridge_gog_installed_audio_task.md) | `targeted_bridge_gog_installed_audio_task` |
| [TBC003 — selected CHelp registration to native-import receiver](findings/boundaries/targeted_bridge_help_callback_import_receiver.md) | `targeted_bridge_help_callback_import_receiver` |
| [TBC010 — initial raw selected-slot flag to known controller consumer](findings/boundaries/targeted_bridge_input_initial_selected_slot.md) | `targeted_bridge_input_initial_selected_slot` |
| [TBC001 — CMap polling bracket to frame-repeat state](findings/boundaries/targeted_bridge_loading_repeat_bracket.md) | `targeted_bridge_loading_repeat_bracket` |
| [TBC012 — initial loading-thread callback to flag-controlled presentation requests](findings/boundaries/targeted_bridge_loading_worker_presentation_loop.md) | `targeted_bridge_loading_worker_presentation_loop` |
| [TBC005 — animation matrix to packet plane metadata to reflection](findings/boundaries/targeted_bridge_model_reflection_plane_record.md) | `targeted_bridge_model_reflection_plane_record` |
| [TBC007 — XAM-compatible retained-block production to known cleanup](findings/boundaries/targeted_bridge_xam_retained_block_producer.md) | `targeted_bridge_xam_retained_block_producer` |
| [TBC004 — XCA-compatible arrays to same-model scalar evaluation](findings/boundaries/targeted_bridge_xca_model_scalar_evaluation.md) | `targeted_bridge_xca_model_scalar_evaluation` |
| [Targeted Phase 1/6 revalidation — dispatcher, CRT roots, changed candidates](findings/boundaries/targeted_phase1_phase6_revalidation_2026-10-06.md) | `targeted_phase1_phase6_revalidation_2026-10-06` |
| [Vehicle organizing roots and selected PC correspondence](findings/boundaries/vehicle_organizing_root.md) | `vehicle_organizing_root` |
| [Win32 Activation Helper No-Op](findings/boundaries/win32_activation_helper_noop.md) | `win32_activation_helper_noop` |
| [Win32 DirectShow Media-Event Boundary](findings/boundaries/win32_directshow_media_event_boundary.md) | `win32_directshow_media_event_boundary` |
| [Win32 Dropfiles Helper No-Op](findings/boundaries/win32_dropfiles_helper_noop.md) | `win32_dropfiles_helper_noop` |
| [Win32 Message to Engine Input/Application Boundary](findings/boundaries/win32_message_input_boundary.md) | `win32_message_input_boundary` |
| [Win32 Mouse Coordinate Reader Limit](findings/boundaries/win32_mouse_coordinate_reader_limit.md) | `win32_mouse_coordinate_reader_limit` |
| [Win32 Mouse Translator No-Op](findings/boundaries/win32_mouse_translator_noop.md) | `win32_mouse_translator_noop` |
| [Win32 Resize Helper No-Op](findings/boundaries/win32_resize_helper_noop.md) | `win32_resize_helper_noop` |
| [World object representation / desired residency policy](findings/boundaries/world_object_representation_policy.md) | `world_object_representation_policy` |
| [XAM CObjectSpecies Interface Boundary](findings/boundaries/xam_cobjectspecies_interface.md) | `xam_cobjectspecies_interface` |
| [XAM Resource Binding State Transition](findings/boundaries/xam_resource_binding_state.md) | `xam_resource_binding_state` |
| [XAM `SEEDG` Resource to Object Dispatch](findings/boundaries/xam_seedg_object_dispatch.md) | `xam_seedg_object_dispatch` |
| [XCA CRdInterp Scene-State Boundary](findings/boundaries/xca_crdinterp_scene_state.md) | `xca_crdinterp_scene_state` |
| [XCA Scene-Transition Resource Handoff](findings/boundaries/xca_scene_transition_handoff.md) | `xca_scene_transition_handoff` |
| [XMD Animation to Actor Update](findings/boundaries/xmd_actor_animation.md) | `xmd_actor_animation` |
| [XMD Resource to Actor Lifecycle Boundary](findings/boundaries/xmd_actor_lifecycle.md) | `xmd_actor_lifecycle` |
| [XMD to Animation / CRdMesh Boundary](findings/boundaries/xmd_animation.md) | `xmd_animation` |
| [XMD Bounds / Visibility Preparation](findings/boundaries/xmd_bounds_visibility_prep.md) | `xmd_bounds_visibility_prep` |
| [XMD / CRdMesh Consumer](findings/boundaries/xmd_crdmesh_consumer.md) | `xmd_crdmesh_consumer` |
| [XMD / CRdMesh Descriptor-Commit Reconciliation](findings/boundaries/xmd_crdmesh_descriptor_commit_reconciliation.md) | `xmd_crdmesh_descriptor_commit_reconciliation` |
| [XMD Animation to Object Render Handoff](findings/boundaries/xmd_object_render_handoff.md) | `xmd_object_render_handoff` |
| [XMD Transform Blending Consumer](findings/boundaries/xmd_transform_blending.md) | `xmd_transform_blending` |
| [XMD Virtual +0x4C Target Constraint](findings/boundaries/xmd_virtual_4c_negative.md) | `xmd_virtual_4c_negative` |
| [XNV Hook/Tackle Chain Object Families](findings/boundaries/xnv_hook_tackle_chain_classes.md) | `xnv_hook_tackle_chain_classes` |
| [XNV Selector Object-Family Boundary](findings/boundaries/xnv_selector_object_family.md) | `xnv_selector_object_family` |
| [XNV Shared Setup Receiver Limit](findings/boundaries/xnv_shared_setup_receiver_limit.md) | `xnv_shared_setup_receiver_limit` |
| [XPC2 D3D9 Registry Cleanup Bridge](findings/boundaries/xpc2_d3d9_registry_cleanup.md) | `xpc2_d3d9_registry_cleanup` |
| [XPC2 to D3D9 Texture Boundary](findings/boundaries/xpc2_d3d9_texture.md) | `xpc2_d3d9_texture` |
| [XPC positive resource portfolio and ownership split](findings/boundaries/xpc_resource_positive_portfolio.md) | `xpc_resource_positive_portfolio` |
| [XWP CEffect Callback Lifecycle](findings/boundaries/xwp_ceffect_callback_lifecycle.md) | `xwp_ceffect_callback_lifecycle` |
| [XWP CEffect Package Boundary](findings/boundaries/xwp_ceffect_package_boundary.md) | `xwp_ceffect_package_boundary` |

## Builds · 0

| Primary finding | Source file |
| :-- | :-- |

## Classes · 0

| Primary finding | Source file |
| :-- | :-- |

## Formats · 13

| Primary finding | Source file |
| :-- | :-- |
| [DPserial Archive Stream Loader](findings/formats/dpserial_archive_loader.md) | `dpserial_archive_loader` |
| [DSB Event-Core Slot Selection Limit](findings/formats/dsb_event_core_slot_selection_limit.md) | `dsb_event_core_slot_selection_limit` |
| [Resource Descriptor Field Map](findings/formats/resource_descriptor_fields.md) | `resource_descriptor_fields` |
| [DPserial Resource Extension Census](findings/formats/resource_extension_census.md) | `resource_extension_census` |
| [Resource-Path Anchors: XPC/XMD/XAM Families](findings/formats/resource_loader_anchors.md) | `resource_loader_anchors` |
| [Outer Resource Manager Population](findings/formats/resource_manager_population.md) | `resource_manager_population` |
| [Outer Resource Manager Root](findings/formats/resource_manager_root.md) | `resource_manager_root` |
| [Typed Resource Extension Dispatcher](findings/formats/typed_resource_dispatch.md) | `typed_resource_dispatch` |
| [XAM / XCA / XNV Targeted Slice](findings/formats/xam_xca_xnv_slice.md) | `xam_xca_xnv_slice` |
| [XCA CRdInterp Parser Boundary](findings/formats/xca_crdinterp_parser.md) | `xca_crdinterp_parser` |
| [XCA CRdInterp Wrapper Limit](findings/formats/xca_crdinterp_wrapper_limit.md) | `xca_crdinterp_wrapper_limit` |
| [XPC2 Child-Record Accessors](findings/formats/xpc2_child_accessors.md) | `xpc2_child_accessors` |
| [XWP / DSB Branch Slice](findings/formats/xwp_dsb_branch_slice.md) | `xwp_dsb_branch_slice` |

## Subsystems · 43

| Primary finding | Source file |
| :-- | :-- |
| [Phase 4 architectural mechanism portfolio — closeout candidate](findings/subsystems/PHASE4_ARCHITECTURAL_PORTFOLIO.md) | `PHASE4_ARCHITECTURAL_PORTFOLIO` |
| [Phase 5 ongoing boundary/lifetime portfolio](findings/subsystems/PHASE5_BOUNDARY_PORTFOLIO.md) | `PHASE5_BOUNDARY_PORTFOLIO` |
| [Actor/resource mechanism: selected CItem](findings/subsystems/actor_resource_binding.md) | `actor_resource_binding` |
| [Animation — organizing/temporal slice and selected actor base-state reconstruction](findings/subsystems/animation.md) | `animation` |
| [Gameplay audio — selected Phase4 mechanism](findings/subsystems/audio.md) | `audio` |
| [CPut static cohort / selected provider addition](findings/subsystems/cput_static_cohort_provider_2026-10-07.md) | `cput_static_cohort_provider_2026-10-07` |
| [Typed Darts driver / trajectory / board classification / score family](findings/subsystems/final_static_darts_board_driver_score.md) | `final_static_darts_board_driver_score` |
| [CMap time/environment callbacks → render packet → thunder-associated pulse](findings/subsystems/final_static_environment_packet_thunder_family.md) | `final_static_environment_packet_thunder_family` |
| [Typed Password / Chess puzzle family and conditional result-bank writes](findings/subsystems/final_static_password_chess_puzzle_family.md) | `final_static_password_chess_puzzle_family` |
| [XPM executable envelopes → geometry caches → typed shape preparation](findings/subsystems/final_static_xpm_model_physics_family.md) | `final_static_xpm_model_physics_family` |
| [CFishing Selector Root](findings/subsystems/fishing.md) | `fishing` |
| [High-Centrality Dispatcher Pair `00647730` / `00647680`](findings/subsystems/high_centrality_00647730.md) | `high_centrality_00647730` |
| [Native UI — selected Phase4 numeric commit mechanism](findings/subsystems/native_ui.md) | `native_ui` |
| [Native UI / Fade predicate / camera policy](findings/subsystems/native_ui_fade_camera.md) | `native_ui_fade_camera` |
| [Subsystem — NPC_ACTOR: selected CNpcEnemy numeric policy](findings/subsystems/npc_actor.md) | `npc_actor` |
| [Anonymous nested-record control/factory requests — sequence196 primary qualification](findings/subsystems/phase6_anonymous_nested_control_factory_requests.md) | `phase6_anonymous_nested_control_factory_requests` |
| [Independent shared adapter: byte150-gated four requests](findings/subsystems/phase6_byte150_four_request_adapter.md) | `phase6_byte150_four_request_adapter` |
| [CEvCore available constructor and inline-init boundary](findings/subsystems/phase6_cevcore_available_constructor_composition.md) | `phase6_cevcore_available_constructor_composition` |
| [CRdInput available base initialization and singleton publication](findings/subsystems/phase6_crdinput_available_construction.md) | `phase6_crdinput_available_construction` |
| [CRdInput request pair joins the established Actuator provider](findings/subsystems/phase6_crdinput_existing_actuator_join.md) | `phase6_crdinput_existing_actuator_join` |
| [CRdInput available gated request-pair interface](findings/subsystems/phase6_crdinput_gated_provider_request.md) | `phase6_crdinput_gated_provider_request` |
| [CRdModel fixed shader/state/texture composition — Phase6 Steam](findings/subsystems/phase6_crdmodel_fixed_shader_composition.md) | `phase6_crdmodel_fixed_shader_composition` |
| [GOG CRdModel fixed aggregate — sequence153](findings/subsystems/phase6_gog_crdmodel_fixed_aggregate_qualification.md) | `phase6_gog_crdmodel_fixed_aggregate_qualification` |
| [Selected item-cache operation: numeric factory/callback adapter](findings/subsystems/phase6_item_cache_factory_callback_adapter.md) | `phase6_item_cache_factory_callback_adapter` |
| [Independent packed-sequence native-control interface](findings/subsystems/phase6_native_packed_sequence_control.md) | `phase6_native_packed_sequence_control` |
| [Receiver-local numeric control, audio and callback interfaces](findings/subsystems/phase6_numeric_control_audio_callback_layer.md) | `phase6_numeric_control_audio_callback_layer` |
| [Independent rank33 Game/registry/auxiliary Message control — sequence197](findings/subsystems/phase6_rank33_game_registry_message_aux_control.md) | `phase6_rank33_game_registry_message_aux_control` |
| [Independent rank37: local Game/Message auxiliary and resource comparisons](findings/subsystems/phase6_rank37_local_game_message_resource_control.md) | `phase6_rank37_local_game_message_resource_control` |
| [Independent rank45: anonymous nested dispatch and known request interfaces](findings/subsystems/phase6_rank45_anonymous_nested_dispatch_requests.md) | `phase6_rank45_anonymous_nested_dispatch_requests` |
| [Independent rank50: anonymous byte control and known input/resource requests](findings/subsystems/phase6_rank50_anonymous_byte_control_input_resource_requests.md) | `phase6_rank50_anonymous_byte_control_input_resource_requests` |
| [Independent rank51: ItemManager adapter result to opaque receiver request](findings/subsystems/phase6_rank51_item_adapter_result_receiver_request.md) | `phase6_rank51_item_adapter_result_receiver_request` |
| [Available ThreadEx-associated cursor/cache control — sequence196 primary qualification](findings/subsystems/phase6_threadex_available_cursor_item_event_cache_control.md) | `phase6_threadex_available_cursor_item_event_cache_control` |
| [Independent World47 candidate: two-tier raw control and auxiliary cache](findings/subsystems/phase6_two_tier_receiver_control_item_cache.md) | `phase6_two_tier_receiver_control_item_cache` |
| [Root/service adversarial retained-wave review](findings/subsystems/phase7_root_service_adversarial_review.md) | `phase7_root_service_adversarial_review` |
| [PHYSICS_PHYSX — Phase4 selected context/record mechanism](findings/subsystems/physics_physx.md) | `physics_physx` |
| [Presentation/media effects subsystem — selected request/fade/latch mechanism](findings/subsystems/presentation_effects.md) | `presentation_effects` |
| [Resource loading — scoped subsystem mechanism map](findings/subsystems/resource_loading.md) | `resource_loading` |
| [SAVE_GAMERECORD — Phase4 selected persistence/staging reconstruction](findings/subsystems/save_gamerecord.md) | `save_gamerecord` |
| [Save / GameRecord — typed writer and staging policy](findings/subsystems/save_writer_staging_policy.md) | `save_writer_staging_policy` |
| [Selector Range `0x48`–`0x4E` Object Census](findings/subsystems/selector_48_4e_object_range.md) | `selector_48_4e_object_range` |
| [Paired Selector Factory Direct-Vtable Census](findings/subsystems/selector_factory_direct_vtable_census.md) | `selector_factory_direct_vtable_census` |
| [Vehicle subsystem — organizing roots and callback/control responsibilities](findings/subsystems/vehicle.md) | `vehicle` |
| [World resource residency — typed representation policy](findings/subsystems/world_resource_residency.md) | `world_resource_residency` |
