# Vehicle / object-action protocol evidence

Read-only static extraction from the supplied GOG and Steam 1.01b executables. No game execution or runtime test.

## Sources

| Build | Source | SHA-256 |
|---|---|---|
| GOG | `DP_GOG.exe` (`libfile_8b96c337b6908191bf4bae19ae0a2a41`) | `c954c2e3b205d444b0fc3649adf4bd8462a7dfe73d89599e24a7e17a129415c2` |
| STEAM | `DP_STEAM.exe` (`libfile_5feee799969481918448e54024a4cebd`) | `7a713886756bcde67bf276ce0e8bb898ee689673ff6027fc182d491bd242a029` |

## Reproduction and results

[extract_protocol.py](extract_protocol.py) requires Python 3 and GNU objdump. Invoke with GOG executable path, Steam executable path, and an output directory. No third-party Python modules or game execution are needed. Checked local build offset only; no global delta assumption.

[protocol.json](protocol.json) records all 137 gameplay handler slots, all 80 inspected native-event selector entries, capability masks, 14 shared completion cases, car phase targets, and the checked Event35/car and Event67/Player dispatcher targets.

Numeric candidate selectors are not proof of runtime state reachability. The script follows each short selector's successful capability-check path; guards may reject it and the common ingress can intercept it.

## Assembly extracts

Each pair covers the same bounded region in both builds. Code addresses are embedded in the text. Inline switch tables are separately read as PE data for JSON; linear disassembly is not used to interpret those table bytes.

- action_ingress: [GOG](gog_action_ingress.asm), [Steam](steam_action_ingress.asm)
- action_selector: [GOG](gog_action_selector.asm), [Steam](steam_action_selector.asm)
- active_car_exit_request: [GOG](gog_active_car_exit_request.asm), [Steam](steam_active_car_exit_request.asm)
- active_state87_steering: [GOG](gog_active_state87_steering.asm), [Steam](steam_active_state87_steering.asm)
- car_auxiliary_create: [GOG](gog_car_auxiliary_create.asm), [Steam](steam_car_auxiliary_create.asm)
- car_auxiliary_objects_release: [GOG](gog_car_auxiliary_objects_release.asm), [Steam](steam_car_auxiliary_objects_release.asm)
- car_auxiliary_release: [GOG](gog_car_auxiliary_release.asm), [Steam](steam_car_auxiliary_release.asm)
- car_event35_dispatch_case: [GOG](gog_car_event35_dispatch_case.asm), [Steam](steam_car_event35_dispatch_case.asm)
- car_event_dispatch_entry: [GOG](gog_car_event_dispatch_entry.asm), [Steam](steam_car_event_dispatch_entry.asm)
- car_mode_enable: [GOG](gog_car_mode_enable.asm), [Steam](steam_car_mode_enable.asm)
- car_packet_consumer: [GOG](gog_car_packet_consumer.asm), [Steam](steam_car_packet_consumer.asm)
- packet_refresh: [GOG](gog_packet_refresh.asm), [Steam](steam_packet_refresh.asm)
- player_event_dispatch_entry: [GOG](gog_player_event_dispatch_entry.asm), [Steam](steam_player_event_dispatch_entry.asm)
- player_packet_refresh_dispatch: [GOG](gog_player_packet_refresh_dispatch.asm), [Steam](steam_player_packet_refresh_dispatch.asm)
- shared_completion: [GOG](gog_shared_completion.asm), [Steam](steam_shared_completion.asm)
- vehicle_cleanup: [GOG](gog_vehicle_cleanup.asm), [Steam](steam_vehicle_cleanup.asm)
- vehicle_entry: [GOG](gog_vehicle_entry.asm), [Steam](steam_vehicle_entry.asm)
- vehicle_exit_prelude: [GOG](gog_vehicle_exit_prelude.asm), [Steam](steam_vehicle_exit_prelude.asm)
- vehicle_hub38: [GOG](gog_vehicle_hub38.asm), [Steam](steam_vehicle_hub38.asm)
- vehicle_setup: [GOG](gog_vehicle_setup.asm), [Steam](steam_vehicle_setup.asm)
