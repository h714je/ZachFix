# Xbox-only control evidence

Raw/static extracts supporting `input/xbox_only_controls.md` and `input/xbox_only_controls_patch_plan_2026-09-26.md`.

## Combat Strafe

- `xbox_strafe_ingress_excerpt.cpp.txt` — Xbox final capability/flag/input gate and `09/0A` transitions from `sub_82302EC8`.
- `gog_strafe_states_09_0a.asm` — retained Director's Cut GOG shared handler.
- `steam_strafe_states_09_0a.asm` — retained Steam shared handler.
- `gog_player_update_tail.asm` / `steam_player_update_tail.asm` — homologous PC common tail where Xbox calls the missing strafe ingress before final state bookkeeping.

## Quick Turn

- `xbox_quickturn_candidates.cpp.txt` — extracted Xbox high-level input/transition contexts.
- `gog_quickturn_normal.asm` / `steam_quickturn_normal.asm` — preserved normal `yaw + pi -> state 0B` branch with altered input trigger.
- `gog_quickturn_combat.asm` / `steam_quickturn_combat.asm` — Director's Cut combat-context branch showing added HoldBreath dependency and LIGHTONOFF trigger.
- `gog_quickturn_state_0b.asm` / `steam_quickturn_state_0b.asm` — retained heading-interpolation state consumer.

All PC addresses are static VAs for the inspected supported GOG/Steam executables. Patch bytes in the patch-plan note are candidates until runtime-tested.
