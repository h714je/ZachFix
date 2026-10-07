# Structural baseline diff — 2026-10-06

Baseline: `/home/h7n14/DP_Engine_Archaeology_Census_100M/structural/repair_2026-10-06/baseline_v7`. Manifest SHA-256: `8b70425655d16b476b8a0d5adf0977c4a6f773041be25e2852876397459c8912`.

**VERIFIED:** source-bound structural accounting; **HYPOTHESIS:** score/rank/root/service/cluster relevance. No complete function/code/dispatch universe or semantic revalidation is claimed.

## Entry population and body geometry

| Build | Original recognized exports | All corrected entry records | New supported callable entries | New unproven entry candidates |
| --- | --- | --- | --- | --- |
| STEAM_PC | 10322 | 11046 | 71 | 653 |
| GOG_PC | 9930 | 10825 | 237 | 658 |

| Build | New entry channel/status | Count |
| --- | --- | --- |
| STEAM_PC | ADDRESS_TAKEN_CANDIDATE | 642 |
| STEAM_PC | CALLBACK_TARGET | 1 |
| STEAM_PC | DIRECT_CALL_TARGET_CANDIDATE | 2 |
| STEAM_PC | INITIALIZER_CALLBACK_TARGET | 70 |
| STEAM_PC | VTABLE_SLOT_CANDIDATE | 9 |
| GOG_PC | ADDRESS_TAKEN_CANDIDATE | 641 |
| GOG_PC | CALLBACK_TARGET | 1 |
| GOG_PC | DIRECT_CALL_TARGET | 166 |
| GOG_PC | DIRECT_CALL_TARGET_CANDIDATE | 8 |
| GOG_PC | INITIALIZER_CALLBACK_TARGET | 70 |
| GOG_PC | VTABLE_SLOT_CANDIDATE | 9 |

Units: build-local entry identities, not newly proved semantic functions. Original counts retain export lineage. New supported entries have one of the explicit supported statuses; address/vtable/candidate-rooted direct targets remain unproven. Exact disjoint export attribution and reconstructed CFG fragments replace hull-based accounting, not semantic ownership. Per-entry byte sums can overlap; accepted_unique_bytes is the physical-byte denominator. Original exporter/project/full FunctionBodyRanges remain unavailable.

| Build | Geometry metric | Count | Unit |
| --- | --- | --- | --- |
| GOG_PC | entries_with_conflicts | 1 | entry records |
| GOG_PC | entries_with_shared_bytes | 130 | entry records |
| GOG_PC | exact_export_attributed_bytes_sum | 2324329 | per-entry byte sum (hulls include gaps) |
| GOG_PC | original_body_metadata_bytes_sum | 2324329 | per-entry byte sum (hulls include gaps) |
| GOG_PC | original_hull_bytes_sum | 3315373 | per-entry byte sum (hulls include gaps) |
| GOG_PC | original_hull_records | 9930 | entry records |
| GOG_PC | original_hulls_larger_than_body_metadata | 844 | entry records |
| GOG_PC | reconstructed_reachable_bytes_membership_sum | 2538497 | per-entry byte sum (hulls include gaps) |
| STEAM_PC | entries_with_conflicts | 2 | entry records |
| STEAM_PC | entries_with_shared_bytes | 136 | entry records |
| STEAM_PC | exact_export_attributed_bytes_sum | 2981864 | per-entry byte sum (hulls include gaps) |
| STEAM_PC | original_body_metadata_bytes_sum | 2981864 | per-entry byte sum (hulls include gaps) |
| STEAM_PC | original_hull_bytes_sum | 3994582 | per-entry byte sum (hulls include gaps) |
| STEAM_PC | original_hull_records | 10322 | entry records |
| STEAM_PC | original_hulls_larger_than_body_metadata | 486 | entry records |
| STEAM_PC | reconstructed_reachable_bytes_membership_sum | 2645905 | per-entry byte sum (hulls include gaps) |

## Physical callsites versus adjacency

| Build | View | Nodes | Physical sites | (site,kind,target) edges | Unique singleton adjacency | Uncertain adjacency | Unresolved targets | Unowned edges |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| STEAM_PC | BRANCH | 11046 | 86116 | 86116 | 19 | 6553 | 0 | 4759 |
| STEAM_PC | CALLBACK_DISPATCH_CANDIDATE | 11046 | 1 | 116 | 0 | 116 | 0 | 0 |
| STEAM_PC | CALLBACK_POINTER | 11046 | 2998 | 3048 | 1118 | 56 | 0 | 207 |
| STEAM_PC | CANDIDATE_CONTROL_FLOW | 11046 | 92081 | 101205 | 32329 | 7064 | 0 | 10643 |
| STEAM_PC | DIRECT_CALL | 11046 | 87479 | 87479 | 31355 | 5758 | 0 | 10255 |
| STEAM_PC | EXPORTED_INDIRECT_TARGET_HINT | 11046 | 688 | 688 | 0 | 4 | 0 | 2 |
| STEAM_PC | INDIRECT_CALL_RESOLVED | 11046 | 577 | 577 | 0 | 0 | 0 | 1 |
| STEAM_PC | INDIRECT_CALL_STATIC_TARGET_CANDIDATE | 11046 | 3 | 3 | 0 | 3 | 0 | 0 |
| STEAM_PC | INDIRECT_CALL_UNRESOLVED | 11046 | 4681 | 4681 | 0 | 0 | 4681 | 462 |
| STEAM_PC | INDIRECT_JUMP_UNRESOLVED | 11046 | 1557 | 1557 | 0 | 0 | 1557 | 159 |
| STEAM_PC | LEGACY_ALL_CALL_ROWS | 10322 | 89516 | 89516 | 36588 | 0 | 0 | 13059 |
| STEAM_PC | RESOLVED_INVOCATION | 11046 | 88056 | 88056 | 31355 | 5758 | 0 | 10256 |
| STEAM_PC | SHARED_CODE_TRANSFER | 11046 | 1193 | 1193 | 0 | 358 | 0 | 3 |
| STEAM_PC | SWITCH_TARGET_CANDIDATE | 11046 | 1237 | 10360 | 0 | 567 | 0 | 305 |
| STEAM_PC | TAIL_TRANSFER_CANDIDATE | 11046 | 1596 | 1596 | 989 | 388 | 0 | 79 |
| GOG_PC | BRANCH | 10825 | 79431 | 79431 | 19 | 6457 | 0 | 4100 |
| GOG_PC | CALLBACK_DISPATCH_CANDIDATE | 10825 | 1 | 116 | 0 | 116 | 0 | 0 |
| GOG_PC | CALLBACK_POINTER | 10825 | 2767 | 2813 | 1095 | 48 | 0 | 243 |
| GOG_PC | CANDIDATE_CONTROL_FLOW | 10825 | 81894 | 90452 | 31154 | 5746 | 0 | 6024 |
| GOG_PC | DIRECT_CALL | 10825 | 75444 | 75444 | 30204 | 4436 | 0 | 5454 |
| GOG_PC | EXPORTED_INDIRECT_TARGET_HINT | 10825 | 677 | 677 | 0 | 3 | 0 | 4 |
| GOG_PC | INDIRECT_CALL_RESOLVED | 10825 | 569 | 569 | 0 | 0 | 0 | 1 |
| GOG_PC | INDIRECT_CALL_STATIC_TARGET_CANDIDATE | 10825 | 3 | 3 | 0 | 3 | 0 | 0 |
| GOG_PC | INDIRECT_CALL_UNRESOLVED | 10825 | 4092 | 4092 | 0 | 0 | 4092 | 268 |
| GOG_PC | INDIRECT_JUMP_UNRESOLVED | 10825 | 1448 | 1448 | 0 | 0 | 1448 | 196 |
| GOG_PC | LEGACY_ALL_CALL_ROWS | 9930 | 68506 | 68506 | 30506 | 0 | 0 | 10297 |
| GOG_PC | RESOLVED_INVOCATION | 10825 | 76013 | 76013 | 30204 | 4436 | 0 | 5455 |
| GOG_PC | SHARED_CODE_TRANSFER | 10825 | 3217 | 3217 | 0 | 431 | 0 | 3 |
| GOG_PC | SWITCH_TARGET_CANDIDATE | 10825 | 1124 | 9679 | 0 | 510 | 0 | 482 |
| GOG_PC | TAIL_TRANSFER_CANDIDATE | 10825 | 1543 | 1543 | 963 | 372 | 0 | 84 |

Legacy all-reference rows are not a direct-call universe. DIRECT_CALL includes inventoried E8 sites whether owners are qualified or not; singleton callable adjacency is a smaller, differently qualified set. Unresolved indirect sites, external imports, ambiguous/unowned sources, interior targets, pointer installation, switches, shared transfers and callback target candidates remain explicit separate channels. No callback/table pointer is silently promoted to invocation.

| Build | Raw relationship change | Count |
| --- | --- | --- |
| GOG_PC | added_physical_sites:ADDRESS_TAKEN_CODE | 2651 |
| GOG_PC | added_physical_sites:BRANCH | 79431 |
| GOG_PC | added_physical_sites:CALLBACK_DISPATCH_TARGET | 1 |
| GOG_PC | added_physical_sites:CALLBACK_INSTALLATION | 117 |
| GOG_PC | added_physical_sites:DIRECT_CALL | 9016 |
| GOG_PC | added_physical_sites:INDIRECT_CALL_RESOLVED | 3 |
| GOG_PC | added_physical_sites:INDIRECT_CALL_STATIC_TARGET_CANDIDATE | 1 |
| GOG_PC | added_physical_sites:INDIRECT_CALL_UNRESOLVED | 3981 |
| GOG_PC | added_physical_sites:INDIRECT_JUMP_UNRESOLVED | 1448 |
| GOG_PC | added_physical_sites:SHARED_CODE_TRANSFER | 3217 |
| GOG_PC | added_physical_sites:SWITCH_TARGET_CANDIDATE | 1124 |
| GOG_PC | added_physical_sites:SYSTEM_TRANSFER_UNRESOLVED | 20 |
| GOG_PC | added_physical_sites:TAIL_TRANSFER_CANDIDATE | 142 |
| GOG_PC | added_typed_physical_sites | 95674 |
| GOG_PC | adjacency_added | 3801 |
| GOG_PC | adjacency_after | 30204 |
| GOG_PC | adjacency_before | 30506 |
| GOG_PC | adjacency_removed | 4103 |
| GOG_PC | adjacency_retained | 26403 |
| GOG_PC | legacy_physical_sites | 68506 |
| GOG_PC | legacy_reference_rows | 68506 |
| GOG_PC | legacy_reference_type:COMPUTED_CALL | 675 |
| GOG_PC | legacy_reference_type:COMPUTED_CALL_TERMINATOR | 2 |
| GOG_PC | legacy_reference_type:UNCONDITIONAL_CALL | 67829 |
| GOG_PC | reclassified_or_not_definite_direct_physical_sites | 20167 |
| GOG_PC | removed_sites_without_inventoried_typed_edge | 0 |
| STEAM_PC | added_physical_sites:ADDRESS_TAKEN_CODE | 2882 |
| STEAM_PC | added_physical_sites:BRANCH | 86116 |
| STEAM_PC | added_physical_sites:CALLBACK_DISPATCH_TARGET | 1 |
| STEAM_PC | added_physical_sites:CALLBACK_INSTALLATION | 117 |
| STEAM_PC | added_physical_sites:DIRECT_CALL | 146 |
| STEAM_PC | added_physical_sites:INDIRECT_CALL_RESOLVED | 1 |
| STEAM_PC | added_physical_sites:INDIRECT_CALL_UNRESOLVED | 4569 |
| STEAM_PC | added_physical_sites:INDIRECT_JUMP_UNRESOLVED | 1557 |
| STEAM_PC | added_physical_sites:SHARED_CODE_TRANSFER | 1193 |
| STEAM_PC | added_physical_sites:SWITCH_TARGET_CANDIDATE | 1237 |
| STEAM_PC | added_physical_sites:SYSTEM_TRANSFER_UNRESOLVED | 16 |
| STEAM_PC | added_physical_sites:TAIL_TRANSFER_CANDIDATE | 101 |
| STEAM_PC | added_typed_physical_sites | 94232 |
| STEAM_PC | adjacency_added | 143 |
| STEAM_PC | adjacency_after | 31355 |
| STEAM_PC | adjacency_before | 36588 |
| STEAM_PC | adjacency_removed | 5376 |
| STEAM_PC | adjacency_retained | 31212 |
| STEAM_PC | legacy_physical_sites | 89516 |
| STEAM_PC | legacy_reference_rows | 89516 |
| STEAM_PC | legacy_reference_type:COMPUTED_CALL | 686 |
| STEAM_PC | legacy_reference_type:COMPUTED_CALL_TERMINATOR | 2 |
| STEAM_PC | legacy_reference_type:UNCONDITIONAL_CALL | 88828 |
| STEAM_PC | reclassified_or_not_definite_direct_physical_sites | 29459 |
| STEAM_PC | removed_sites_without_inventoried_typed_edge | 0 |

Added typed physical sites include branches/pointers/candidates; per-kind sites can overlap. Removed sites means no inventoried typed edge, not removed executable bytes or an accepted-instruction-boundary check. Existing typed inventory may retain unaccepted or uncertain boundaries. Reclassified/not-definite-direct means typed kinds are not exactly DIRECT_CALL or qualified definite edges are absent. Adjacency added/removed/retained compares original recognized-reference pairs with qualified singleton invocation pairs, not equal semantic universes.

## Globals, address-only contacts and vtables

| Build | Historical canonical global records (different denominator) | Mapped non-executable targets | Address-only targets (no READ/WRITE) | Named RTTI tables |
| --- | --- | --- | --- | --- |
| STEAM_PC | 145 | 15117 | 6940 | 512 |
| GOG_PC | 80 | 14574 | 6682 | 512 |

| Build | Channel | Metric | Count | Denominator |
| --- | --- | --- | --- | --- |
| STEAM_PC | globals | global_targets | 15117 | 15117 global-summary addresses |
| STEAM_PC | globals | address_only_targets | 6940 | 15117 global-summary addresses |
| STEAM_PC | globals | raw_export_refs | 72345 | global-summary target addresses |
| STEAM_PC | globals | primary_operand_refs | 48428 | global-summary target addresses |
| STEAM_PC | globals | physical_refs | 73931 | global-summary target addresses |
| STEAM_PC | globals | read_refs | 48908 | global-summary target addresses |
| STEAM_PC | globals | write_refs | 7470 | global-summary target addresses |
| STEAM_PC | globals | address_refs | 14192 | global-summary target addresses |
| STEAM_PC | globals | data_pointer_refs | 0 | global-summary target addresses |
| STEAM_PC | globals | unknown_refs | 3676 | global-summary target addresses |
| STEAM_PC | vtables | observed_slot_count | 8283 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | raw_export_refs | 2995 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | primary_operand_refs | 2286 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | physical_refs | 3002 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | vptr_literal_write_refs | 2250 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | generic_address_xrefs | 752 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | data_pointer_refs | 0 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | unknown_refs | 0 | 512 named RTTI table identities; non-RTTI tables not certified |
| STEAM_PC | vtables | complete_extent_assertions | 0 | 512 named RTTI table identities |
| GOG_PC | globals | global_targets | 14574 | 14574 global-summary addresses |
| GOG_PC | globals | address_only_targets | 6682 | 14574 global-summary addresses |
| GOG_PC | globals | raw_export_refs | 50880 | global-summary target addresses |
| GOG_PC | globals | primary_operand_refs | 45628 | global-summary target addresses |
| GOG_PC | globals | physical_refs | 60936 | global-summary target addresses |
| GOG_PC | globals | read_refs | 37528 | global-summary target addresses |
| GOG_PC | globals | write_refs | 6922 | global-summary target addresses |
| GOG_PC | globals | address_refs | 13245 | global-summary target addresses |
| GOG_PC | globals | data_pointer_refs | 0 | global-summary target addresses |
| GOG_PC | globals | unknown_refs | 3514 | global-summary target addresses |
| GOG_PC | vtables | observed_slot_count | 8283 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | raw_export_refs | 2497 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | primary_operand_refs | 2220 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | physical_refs | 2695 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | vptr_literal_write_refs | 2185 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | generic_address_xrefs | 510 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | data_pointer_refs | 0 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | unknown_refs | 0 | 512 named RTTI table identities; non-RTTI tables not certified |
| GOG_PC | vtables | complete_extent_assertions | 0 | 512 named RTTI table identities |

Mapped targets are non-executable data addresses, including .rdata/.data, not typed game globals. Historical canonical global-record counts use a different denominator and are not subtracted from address inventories. Raw export rows, recognized users, primary operands and physical references are distinct denominators. READ_WRITE participates in both modes. Address-only roots and data pointers do not prove typed memory dereferences. Named RTTI identity does not certify full vtable extent or non-RTTI population. Literal vptr writers do not prove constructors/destructors; observed shared-method membership is not duplicated direct-call degree.

## Roots, services and subsystem adjacency

| Build | Family | Metric | Count |
| --- | --- | --- | --- |
| STEAM_PC | roots | old_unique_seed_addresses | 174 |
| STEAM_PC | roots | new_qualified_seed_addresses | 50 |
| STEAM_PC | roots | removed_seed_addresses | 129 |
| STEAM_PC | roots | added_seed_addresses | 5 |
| GOG_PC | roots | old_unique_seed_addresses | 79 |
| GOG_PC | roots | new_qualified_seed_addresses | 10 |
| GOG_PC | roots | removed_seed_addresses | 71 |
| GOG_PC | roots | added_seed_addresses | 2 |
| GOG_PC | services | added | 136 |
| GOG_PC | services | after | 2796 |
| GOG_PC | services | before | 3182 |
| GOG_PC | services | changed_common | 2660 |
| GOG_PC | services | rank_changed_common | 2660 |
| GOG_PC | services | removed | 522 |
| STEAM_PC | services | added | 51 |
| STEAM_PC | services | after | 3507 |
| STEAM_PC | services | before | 3883 |
| STEAM_PC | services | changed_common | 3456 |
| STEAM_PC | services | rank_changed_common | 3450 |
| STEAM_PC | services | removed | 427 |
| GOG_PC | subsystems | added_nonzero_pairs | 1 |
| GOG_PC | subsystems | after_nonzero_pairs | 34 |
| GOG_PC | subsystems | before_nonzero_pairs | 56 |
| GOG_PC | subsystems | changed_pair_counts | 45 |
| GOG_PC | subsystems | removed_nonzero_pairs | 23 |
| STEAM_PC | subsystems | added_nonzero_pairs | 26 |
| STEAM_PC | subsystems | after_nonzero_pairs | 136 |
| STEAM_PC | subsystems | before_nonzero_pairs | 140 |
| STEAM_PC | subsystems | changed_pair_counts | 124 |
| STEAM_PC | subsystems | removed_nonzero_pairs | 30 |

Qualified lower-bound seeds use build-local clauses/proven explicit mappings. Inherited selected-root assertions remain a distinct candidate-union view. Rejected foreign/unqualified seeds remain graph nodes if locally present. Service/root/subsystem contact labels are selection hypotheses, not new semantic functions or managers.

| Isolated historical F07 replay metric | Actual source value |
| --- | --- |
| algorithm_result | PASS |
| distance_changes | 11 |
| status | PASS |
| top50_membership_changes | 0 |
| unknown_score_changes | 9 |

F07 isolates the audited six foreign root seeds. Its distance/score/top50 effect is not the combined corrected-feature ranking or current relevance-model change. Replay algorithm PASS, if present, is not baseline acceptance or semantic validation.

## Centrality/relevance/rank comparisons and top 50

Common captured semantic view: Reconstructed legacy algorithm vs corrected substrate with identical captured-current semantics/locators; explicit PHASE6_WEIGHT_COMPARATOR score/rank channel isolates substrate/features from current relevance model change.

Frozen SEQ0132 view: Separate SEQ0132 published rows versus corrected PHASE6_WEIGHT_COMPARATOR, not corrected current priority; historical semantic contents unavailable, input hashes alone cannot rerun historical neighbor/locator features.

Changed historical semantic input hashes: `ledgers/BOUNDARY_LEDGER.csv`, `ledgers/CLAIM_LEDGER.csv`, `ledgers/CLASS_LEDGER.csv`, `ledgers/EVIDENCE_LEDGER.csv`, `ledgers/FUNCTION_LEDGER.csv`, `ledgers/GLOBAL_LEDGER.csv`, `ledgers/HOMOLOGY_LEDGER.csv`, `ledgers/SUBSYSTEM_LEDGER.csv`, `ledgers/VTABLE_LEDGER.csv`. Hash lists cannot restore unavailable historical semantic contents; frozen ordinal changes are not purely structural causation.

| Comparison | Build | Metric | Count |
| --- | --- | --- | --- |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | added | 895 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | after_present | 10825 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | before_present | 9930 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | centrality_changed_common | 3692 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | common_changed | 9930 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | comparison_records | 10825 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | rank_changed_common | 7613 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | relevance_changed_common | 7624 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | removed | 0 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | score_changed_common | 7333 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | added | 724 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | after_present | 11046 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | before_present | 10322 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | centrality_changed_common | 3437 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | common_changed | 10322 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | comparison_records | 11046 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | rank_changed_common | 7743 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | relevance_changed_common | 8546 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | removed | 0 |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | score_changed_common | 7541 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | added | 2887 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | after_present | 10526 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | before_present | 7640 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | centrality_changed_common | 2665 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | common_changed | 7639 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | comparison_records | 10527 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | rank_changed_common | 7638 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | relevance_changed_common | 6236 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | removed | 1 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | GOG_PC | score_changed_common | 5670 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | added | 2828 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | after_present | 10592 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | before_present | 7769 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | centrality_changed_common | 2389 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | common_changed | 7764 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | comparison_records | 10597 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | rank_changed_common | 7761 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | relevance_changed_common | 6773 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | removed | 5 |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | STEAM_PC | score_changed_common | 5815 |

| Comparison | Candidate-set metric | Count | Denominator |
| --- | --- | --- | --- |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:before | 0 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:after | 50 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:retained | 0 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:added | 50 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:removed | 0 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:before | 50 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:after | 50 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:retained | 37 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:added | 13 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:removed | 13 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:before | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:after | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:retained | 0 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:added | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_common_view_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:removed | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:before | 0 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:after | 50 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:retained | 0 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:added | 50 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | NEW_ENTRY:removed | 0 | NEW_ENTRY joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:before | 50 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:after | 50 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:retained | 25 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:added | 25 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | PARTIAL:removed | 25 | PARTIAL joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:before | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:after | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:retained | 0 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:added | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |
| metrics_frozen_phase6_diff.csv.gz:PHASE6_WEIGHT_COMPARATOR | UNKNOWN:removed | 50 | UNKNOWN joint-PC candidate-set ordinal <=50; separate sets never merged |

The common/frozen tables above use the explicitly named PHASE6_WEIGHT_COMPARATOR score/rank channel, not final current priority. Structural feature changes and historical semantic drift are separate from the following same-corrected-facts model revision.

## Current relevance versus preserved comparator — same corrected facts

| Model contract | Final declaration |
| --- | --- |
| comparator_eligibility_field | phase6_weight_comparator_candidate_set |
| comparator_rank_field | phase6_weight_comparator_rank |
| comparator_role | Common-view/frozen structural comparison only; pre-overlay candidate lanes preserved, newly admitted review records have no invented comparator ordinals; not corrected current priority |
| comparator_score_field | phase6_weight_comparator_score |
| comparator_universe_field | phase6_weight_comparator_rank_universe |
| comparator_version | PHASE6_WEIGHT_COMPARATOR |
| current_eligibility_field | candidate_set |
| current_priority_version | CURRENT_RELEVANCE_V2_BOUNDED_MEMBERSHIP |
| current_rank_field | candidate_rank |
| current_score_field | priority_score |
| current_universe_field | rank_universe |
| eligibility_version | CURRENT_ELIGIBILITY_V2_UNQUALIFIED_COARSE_OWNERSHIP_REVIEW |
| evidence_state | HYPOTHESIS |
| feature_version | STRUCTURAL_FEATURES_V3_COARSE_ELIGIBILITY_REVIEW |
| pre_overlay_relevance_rank_field | pre_overlay_candidate_rank |
| pre_overlay_relevance_universe_field | pre_overlay_rank_universe |

Current formula: `CURRENT_RELEVANCE_V2_BOUNDED_MEMBERSHIP:12*direct_subsystems + 2*min(twohop_subsystems,8) + 5*primary_vtable_xref_tables + 5*bool(slot_membership_tables-primary_vtable_xref_tables) + 6*has_class + 2*log2(1+callers) + log2(1+callees) + 5*min(global_bridges,4) + 8*has_carry + min(report_locators,4) + root_cue(5-distance, distance<=3)`.

Preserved comparator: Original Phase6 weights with corrected-feature linear vtable term5*len(primary_vtable_xref_tables union slot_membership_tables); all other terms match current; explicit comparator score/rank never substituted for current priority.

An unactivated diagnostic predecessor exposed a unit mismatch: repeated slot membership was given the old uncapped xref coefficient. Shared methods could therefore dominate top50 without primary vtable xrefs. Final current relevance bounds membership-only breadth to one extra5-point presence cue, while retaining every membership/count in structural sidecars. The old linear union survives only as PHASE6_WEIGHT_COMPARATOR. All other weights and corrected facts/context are unchanged. Score deltas isolate this weight-unit revision, but current ordinal/top50 changes also include the explicit coarse-review eligibility expansion; newly admitted rows have no comparator ordinal. Neither ranking proves architecture or a semantic role.

| Candidate-set metric | Count | Same-facts comparison |
| --- | --- | --- |
| NEW_ENTRY:comparator_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:current_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:entered_current_top50 | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:left_current_top50 | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:new_admissions_without_comparator_rank | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:ordinal_rank_shifts | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:retained_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:rows | 1619 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:score_changes | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| NEW_ENTRY:top50_membership_symmetric_difference | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:comparator_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:current_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:entered_current_top50 | 22 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:left_current_top50 | 22 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:new_admissions_without_comparator_rank | 4109 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:ordinal_rank_shifts | 316 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:retained_top50 | 28 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:rows | 4427 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:score_changes | 33 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| PARTIAL:top50_membership_symmetric_difference | 44 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:comparator_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:current_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:entered_current_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:left_current_top50 | 50 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:new_admissions_without_comparator_rank | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:ordinal_rank_shifts | 11725 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:retained_top50 | 0 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:rows | 15072 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:score_changes | 345 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |
| UNKNOWN:top50_membership_symmetric_difference | 100 | SAME_CORRECTED_FEATURES_LINEAR_COMPARATOR_VS_CURRENT_RELEVANCE_V2 |

Score deltas isolate weights; ordinal/top50 deltas additionally include explicit current eligibility population; newly admitted records have no prior comparator ordinal; see separate same-score eligibility diff; not F07/frozen Phase6/semantic promotion.

## Current candidate eligibility — identical bounded scores

Separate metrics_eligibility_diff.csv.gz compares pre-overlay versus current review lanes/ranks using identical bounded relevance scores; no ownership/evidence/graph/score-weight change.

| Eligibility metric | Count |
| --- | --- |
| comparator_candidate_counts.CONTEXT_ONLY | 4862 |
| comparator_candidate_counts.NEW_ENTRY | 1619 |
| comparator_candidate_counts.PARTIAL | 318 |
| comparator_candidate_counts.UNKNOWN | 15072 |
| current_candidate_counts.CONTEXT_ONLY | 753 |
| current_candidate_counts.NEW_ENTRY | 1619 |
| current_candidate_counts.PARTIAL | 4427 |
| current_candidate_counts.UNKNOWN | 15072 |
| eligible_already_partial_rows | 15 |
| eligible_coarse_rows | 4124 |
| eligible_remaining_context_rows | 0 |
| newly_admitted_by_build.GOG_PC | 1994 |
| newly_admitted_by_build.STEAM_PC | 2115 |
| newly_admitted_partial_rows | 4109 |
| pre_overlay_candidate_counts.CONTEXT_ONLY | 4862 |
| pre_overlay_candidate_counts.NEW_ENTRY | 1619 |
| pre_overlay_candidate_counts.PARTIAL | 318 |
| pre_overlay_candidate_counts.UNKNOWN | 15072 |

| Eligibility basis | Rows |
| --- | --- |
| CANONICAL_OWNERSHIP_UNKNOWN | 15072 |
| EXISTING_PARTIAL_GAME_OR_CLASS_TEXT_LOCATOR | 303 |
| EXISTING_PARTIAL_GAME_OR_CLASS_TEXT_LOCATOR;UNQUALIFIED_COARSE_OWNERSHIP_REQUIRES_REVIEW | 15 |
| NEW_STRUCTURAL_ENTRY_NO_CANONICAL_SEMANTICS | 1619 |
| PRE_OVERLAY_CONTEXT_NOT_INDEPENDENT_NON_GAME_PROOF | 753 |
| PRE_OVERLAY_CONTEXT_NOT_INDEPENDENT_NON_GAME_PROOF;UNQUALIFIED_COARSE_OWNERSHIP_REQUIRES_REVIEW | 4109 |

PARTIAL is a review population, not synonymous with proven GAME ownership. UNKNOWN game ownership with a nonUNKNOWN coarse legacy label remains eligible even if an origin/forwarding form is supported; that form is not non-GAME proof. Admission changes neither canonical ownership/confidence nor graph/score weights. The source-bound guard rejects eligible CONTEXT_ONLY/unranked rows; this is not whole-executable or semantic completeness.

| Feature | Final source definition |
| --- | --- |
| callback_dispatch_candidates | Audited CRT walker/table target sets stay CALLBACK_DISPATCH_CANDIDATE uncertain adjacency; not direct/definite callers, resolved invocation, or runtime dispatch proof |
| corrected_anchors | Independent primary-backed GAME/subsystem axes, including GAME_AUDIO and GAME_RENDER_D3D9; API/origin never prunes nodes |
| corrected_degree | Unique singleton reachable-owner callable-entry adjacency in DIRECT_CALL + INDIRECT_CALL_RESOLVED only |
| corrected_document_carry_locators | Build qualified within local clause; address-only locators visible separately and all88guard source rows preserved |
| corrected_frequency | Unique physical (site,kind,target) invocation references; physical sites are separately deduplicated |
| corrected_globals | Fresh PRIMARY_OPERAND singleton-source .data targets; all other physical/global channels remain in relationship sidecars and diffs |
| corrected_roots | Qualified lower-bound view uses only build-qualified local SUBSYSTEM_LEDGER clauses or unambiguous supported explicit homology mapping; root role remains HYPOTHESIS |
| corrected_vtables | Full separate primary-operand singleton xref-table and observed entry-target slot-membership counts/locators retained; only current ranking membership bonus is bounded, not graph/table/context evidence; ambiguous xrefs excluded from definite feature |
| current_eligibility | UNKNOWN game_ownership axis plus any nonUNKNOWN legacy coarse label requires uncertain PARTIAL review regardless of origin/API/thunk form; no canonical ownership/state rewrite, GAME promotion, or new score weight; explicit source axes/basis retained |
| current_vtable_bonus | 5*len(primary_vtable_xref_tables)+5*bool(slot_membership_tables-primary_vtable_xref_tables):1vs193membership tables without xrefs both add5, overlap not counted twice; complete membership cardinality remains visible |
| indirect_target_candidates | INDIRECT_CALL_STATIC_TARGET_CANDIDATE and EXPORTED_INDIRECT_TARGET_HINT retain constant-pointer/export-inferred target context in separate possible-only views; no definite caller degree, resolved invocation, or score input; original unresolved physical call sites remain separate |
| inherited_root_candidates | Exact selected build-qualified matrix/family-table assertion + subsystem + same-build supported primary-backed canonical function; distinct candidate-union reachability, never silently lower-bound root seeds or score inputs |
| legacy | Original all-reference adjacency, build-ambiguous root/lex/carry address joins, old audio/render anchor exclusions; fresh raw xref scan reproduces legacy feature selection, no compatibility metric preservation |
| phase6_weight_comparator | Original Phase6 weights with corrected-feature linear vtable term5*len(primary_vtable_xref_tables union slot_membership_tables); all other terms match current; explicit comparator score/rank never substituted for current priority |
| preserved_comparator_eligibility | phase6_weight_comparator_candidate_set/rank preserves pre-overlay lanes; context-only records newly admitted to current PARTIAL have blank comparator ordinals; legacy candidate universe unchanged |
| score_weights | CURRENT_RELEVANCE_V2_BOUNDED_MEMBERSHIP:12*direct_subsystems + 2*min(twohop_subsystems,8) + 5*primary_vtable_xref_tables + 5*bool(slot_membership_tables-primary_vtable_xref_tables) + 6*has_class + 2*log2(1+callers) + log2(1+callees) + 5*min(global_bridges,4) + 8*has_carry + min(report_locators,4) + root_cue(5-distance, distance<=3) |
| service_score | 2*log2(1+callers) + log2(1+callees) + 4*contact_subsystems + 3*min(global_bridges,4); a service hypothesis cue, not a semantic classification |

## architecture_sensitive_changes — exact producer-tag scope

| Reason | Tagged records | Precise meaning |
| --- | --- | --- |
| NEW_ENTRY_WITHOUT_LEGACY_ORDINAL | 1619 | New NEW_ENTRY record has no legacy ordinal; includes both supported entries and unproven candidates. |
| NEW_MULTISUBSYSTEM_BRIDGE_HYPOTHESIS | 91 | Tier changes into ARCHITECTURE_SENSITIVE_BRIDGE; a bridge hypothesis, not a semantic boundary proof. |
| PREVIOUS_LOWER_PRIORITY_NOW_CURRENT_RELEVANCE_TOP50_SCORE_MODELS_EXPLICIT | 25 | Old legacy candidate rank >50 and corrected CURRENT_RELEVANCE rank <=50; score models differ explicitly, so not a pure structural-score effect. |
| PREVIOUS_ORPHAN_TARGET_CONNECTIVITY_CHANGED | 1047 | Old unowned incoming sites exist and singleton caller degree changes. |
| PREVIOUS_UNASSIGNED_HAS_CORRECTED_SUBSYSTEM_CONTACT | 62 | Old METADATA_LIMITED_UNASSIGNED row acquires direct qualified subsystem contact. |
| UNQUALIFIED_COARSE_RECORD_ADMITTED_TO_UNCERTAIN_REVIEW_NOT_OWNERSHIP_PROMOTION | 4109 | UNKNOWN game ownership plus a nonempty/nonUNKNOWN legacy coarse label moves CONTEXT_ONLY into current PARTIAL review, irrespective of origin/thunk form; no ownership/confidence promotion. |

Reasons overlap. NEW_ENTRY_WITHOUT_LEGACY_ORDINAL tags new entry records, including unproven candidates; it is not a semantic-function discovery count. Examples below prefer a low final current rank within each available reason, then a large absolute comparator change, plus supported/candidate new-entry examples. Candidate sets are separate rank universes; context-only rows remain unranked. Current-top50 reason tags explicitly compare different score models; the displayed comparator ranks are not substituted for current ranks. This display is deterministic question selection, not completed architecture revalidation.

| Build/address | Selection reason | Entry status | Legacy→corrected comparator rank | Final current rank / score | Primary xref / slot tables | Comparator→current vtable bonus | All source tags |
| --- | --- | --- | --- | --- | --- | --- | --- |
| GOG_PC/00507C70 | NEW_ENTRY_WITHOUT_LEGACY_ORDINAL | DIRECT_CALL_TARGET | unranked → 1 | NEW_ENTRY 1 / 98.322 | 1 / 0 | 5 → 5 | NEW_ENTRY_WITHOUT_LEGACY_ORDINAL;NEW_MULTISUBSYSTEM_BRIDGE_HYPOTHESIS |
| GOG_PC/0044E760 | NEW_MULTISUBSYSTEM_BRIDGE_HYPOTHESIS | DIRECT_CALL_TARGET | unranked → 2 | NEW_ENTRY 2 / 74.492 | 0 / 0 | 0 → 0 | NEW_ENTRY_WITHOUT_LEGACY_ORDINAL;NEW_MULTISUBSYSTEM_BRIDGE_HYPOTHESIS |
| STEAM_PC/00537270 | PREVIOUS_LOWER_PRIORITY_NOW_CURRENT_RELEVANCE_TOP50_SCORE_MODELS_EXPLICIT | DIRECT_CALL_TARGET | 127 → 86 | UNKNOWN 7 / 110.693 | 1 / 0 | 5 → 5 | PREVIOUS_LOWER_PRIORITY_NOW_CURRENT_RELEVANCE_TOP50_SCORE_MODELS_EXPLICIT;PREVIOUS_ORPHAN_TARGET_CONNECTIVITY_CHANGED |
| STEAM_PC/00404390 | PREVIOUS_ORPHAN_TARGET_CONNECTIVITY_CHANGED | DIRECT_CALL_TARGET | 1 → 3 | PARTIAL 1 / 215.429 | 0 / 0 | 0 → 0 | PREVIOUS_ORPHAN_TARGET_CONNECTIVITY_CHANGED |
| STEAM_PC/0072C690 | PREVIOUS_UNASSIGNED_HAS_CORRECTED_SUBSYSTEM_CONTACT | DIRECT_CALL_TARGET | 312 → 265 | PARTIAL 865 / 26.401 | 0 / 0 | 0 → 0 | PREVIOUS_UNASSIGNED_HAS_CORRECTED_SUBSYSTEM_CONTACT |
| STEAM_PC/0074E760 | UNQUALIFIED_COARSE_RECORD_ADMITTED_TO_UNCERTAIN_REVIEW_NOT_OWNERSHIP_PROMOTION | DIRECT_CALL_TARGET | unranked → unranked | PARTIAL 2 / 205.955 | 0 / 0 | 0 → 0 | PREVIOUS_ORPHAN_TARGET_CONNECTIVITY_CHANGED;UNQUALIFIED_COARSE_RECORD_ADMITTED_TO_UNCERTAIN_REVIEW_NOT_OWNERSHIP_PROMOTION |
| GOG_PC/004FFDA0 | new supported callable entry | DIRECT_CALL_TARGET | unranked → 3 | NEW_ENTRY 3 / 54.755 | 0 / 0 | 0 → 0 | NEW_ENTRY_WITHOUT_LEGACY_ORDINAL;NEW_MULTISUBSYSTEM_BRIDGE_HYPOTHESIS |
| STEAM_PC/00409240 | new unproven entry candidate | ADDRESS_TAKEN_CANDIDATE | unranked → 125 | NEW_ENTRY 125 / 11.0 | 0 / 0 | 0 → 0 | NEW_ENTRY_WITHOUT_LEGACY_ORDINAL |

## Record-level references

| Sealed file | Rows | Unit / purpose |
| --- | --- | --- |
| structural/repair_2026-10-06/baseline_v7/metrics_entry_body_diff.csv.gz | 21871 | Recognized hull metadata versus exact attribution/reachability fragments |
| structural/repair_2026-10-06/baseline_v7/metrics_raw_call_typed_edge_diff.csv.gz | 347928 | Every raw call-reference row and additional typed physical sites |
| structural/repair_2026-10-06/baseline_v7/metrics_global_diff.csv.gz | 29691 | Fresh physical/global relationships and qualified users |
| structural/repair_2026-10-06/baseline_v7/metrics_vtable_diff.csv.gz | 1024 | Named identities, observed slots and separate xref channels |
| structural/repair_2026-10-06/baseline_v7/metrics_common_view_diff.csv.gz | 21871 | Legacy/corrected features under identical captured-current semantics |
| structural/repair_2026-10-06/baseline_v7/metrics_frozen_phase6_diff.csv.gz | 21124 | Published SEQ0132 versus corrected comparator, with semantic drift caveat |
| structural/repair_2026-10-06/baseline_v7/metrics_relevance_model_diff.csv.gz | 21871 | Same corrected score facts; comparator/current ordinals also carry eligibility-population versions |
| structural/repair_2026-10-06/baseline_v7/metrics_eligibility_diff.csv.gz | 21871 | Identical bounded score: pre-overlay versus current coarse-review candidate lanes/ranks |
| structural/repair_2026-10-06/baseline_v7/metrics_service_diff.csv | 7252 | Service-candidate presence, rank and feature differences |
| structural/repair_2026-10-06/baseline_v7/metrics_subsystem_diff.csv | 223 | Legacy all-reference versus corrected invocation subsystem adjacency |
| structural/repair_2026-10-06/baseline_v7/metrics_architecture_sensitive_changes.csv | 6791 | Producer-tagged architecture-sensitive subset of common-view changes |
| structural/repair_2026-10-06/baseline_v7/metrics_f07_distance_changes.csv | 11 | Isolated historical F07 root-distance changes |
| structural/repair_2026-10-06/baseline_v7/metrics_f07_score_changes.csv | 9 | Isolated historical F07 score/rank counterfactual |

The machine summary accompanying this report includes all per-build substrate numeric counts, relationship channels, graph views, entry-status/fragment populations, comparisons and candidate qualification counts. Backing records remain exclusively in the sealed baseline. BASELINE: bindings are baseline-local logical file names verified against output hashes, not workspace-root external paths. A repeat build compares logical output names/bytes, never destination-path text in these reports.
