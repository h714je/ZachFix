# Controlled Subsystem Taxonomy

**Established:** 2026-10-02 audit reconciliation  
**Purpose:** keep function, subsystem, and boundary ledgers from creating incompatible names for the same architectural region.

## Canonical subsystem identifiers

`SUBSYSTEM_LEDGER.csv` is the controlled set of engine subsystem identifiers. Boundary endpoints must use one of its rows.

- `APPLICATION_LIFECYCLE`
- `OBJECT_DISPATCH`
- `OBJECT_HANDLE_SERVICE`
- `ACTOR_OBJECTS`
- `PLAYER_ACTOR`
- `RESOURCE_ARCHIVE`
- `RESOURCE_LOADING`
- `WORLD_STREAMING`
- `EVENT_SCRIPT`
- `SAVE_GAMERECORD`
- `INPUT_CAMERA`
- `ANIMATION`
- `PHYSICS_PHYSX`
- `VEHICLE`
- `EFFECTS`
- `FISHING`
- `RENDER_D3D9`
- `NATIVE_UI`
- `AUDIO`
- `PLATFORM_WIN32`

A controlled name does not imply that the corresponding subsystem is deeply reconstructed. Its ledger evidence state and notes define the actual confidence and scope.

## Reconciled aliases

| Former boundary endpoint | Canonical identifier | Rationale |
|---|---|---|
| `GAME_OBJECT_DISPATCH` | `OBJECT_DISPATCH` | Function ownership category was incorrectly used as a boundary subsystem name. |
| `GAME_EVENT` | `EVENT_SCRIPT` | Existing event-command boundary belongs to the event/script seed. |
| `PHYSX` | `PHYSICS_PHYSX` | Aligns direct-physics boundary labels with the subsystem ledger. |

`CRT_STL_RUNTIME` remains a permitted `FUNCTION_LEDGER.csv` coarse non-engine domain. It is intentionally not a subsystem-root row.

## Boundary kinds

`BOUNDARY_LEDGER.csv` records one of:

- `cross_subsystem` — producer and consumer use different canonical subsystem identifiers.
- `internal_lifecycle` — an architecturally useful handoff inside one canonical subsystem, such as CMessage code dispatch to its record resolver.

A boundary count is not a measure of independently reconstructed subsystems. Use the kind, endpoint confidence, and subsystem-root evidence together.

## Component boundaries

`RESOURCE_ARCHIVE` and `OBJECT_HANDLE_SERVICE` are represented separately because existing primary evidence establishes sufficiently distinct archive and packed-handle lifecycle mechanisms. They are not claims that all resource/object ownership is known. `ACTOR_OBJECTS` is a generic actor-interface seed, not a replacement for `PLAYER_ACTOR` or a completed actor hierarchy.

## Sequence52 primary scoped NPC component

`NPC_ACTOR` added for the selected primary CNpcEnemy event/camera-scalar/numeric-state/marker mechanism, separate from PLAYER_ACTOR and genericACTOR_OBJECTS. findings/subsystems/npc_actor.md and findings/boundaries/npc_enemy_camera_scalar_marker_policy.md. NofullAI/allNPC/liveownership/cadence/free claim; earlier21-family accepted milestone preserved.
