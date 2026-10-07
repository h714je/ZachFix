# Input, camera, and original-control research

<!-- BEGIN AUTO RESEARCH NAV -->
[← Research atlas](../README.md) · [Topics](../INDEX.md) · [Open questions](../unresolved.md)

> **Reading note:** Physical providers, logical action processing and CInput are different layers.

<details><summary><strong>On this page</strong> · 11 sections</summary>

- [Native PC input pipeline](#native-pc-input-pipeline)
- [Controller binding contract](#controller-binding-contract)
- [CInput staging / latency: closed](#cinput-staging-latency-closed)
- [2026-10-04 typed root/lifecycle confirmation](#2026-10-04-typed-rootlifecycle-confirmation)
- [UI direction masks](#ui-direction-masks)
- [Camera state -> mode architecture](#camera-state---mode-architecture)
- [ZachFix Xbox aim shaping guard](#zachfix-xbox-aim-shaping-guard)
- [Original Xbox controls retained by the PC state machine](#original-xbox-controls-retained-by-the-pc-state-machine)
- [Production status](#production-status)
- [Remaining high-value tests](#remaining-high-value-tests)
- [MegaRE final input addendum (2026-10-08)](#megare-final-input-addendum-2026-10-08)

</details>
<!-- END AUTO RESEARCH NAV -->

**Research snapshot:** 2026-10-04.

## Native PC input pipeline

```text
Win32 keyboard/mouse + WinMM joystick
    -> FUN_00709BA0 physical acquisition (GOG)
    -> 0x6C logical action record per slot
    -> digital mask + movement/look/trigger channels
    -> FUN_007093B0 PC stick filtering
    -> FUN_00708AB0 aggregation + one-deep pending snapshot
    -> FUN_00708300 commit + held/rising/repeat derivation
    -> public CInput getters
       FUN_007088C0 digital masks
       FUN_007089B0 trigger-like floats
       FUN_007089E0 movement/look floats
    -> Player / camera / UI / vehicle consumers
```

`USEJOY` selects controller versus keyboard/mouse evaluation throughout the engine.

## Controller binding contract

Vanilla Director's Cut expects a legacy WinMM layout:

```text
left stick     X / Y
right stick X  U
right stick Y  R
LT / RT style  opposite directions of shared Z
```

`configJ.cnf`/`configJex.cnf` feed the logical action -> binding enum used by
`FUN_006B1780`.

ZachFix production no longer synthesizes a WinMM `JOYINFOEX` for native gamepad
input. SDL3 or XInput feeds a backend-neutral `GamepadState`; ZachFix evaluates the
same `configJ.cnf` binding IDs at DP's common controller evaluator and then re-runs
DP's own controller action helpers to rebuild only the active controller's `0x6C`
logical-action record. The rest of the native pipeline remains unchanged:

```text
SDL3 / XInput
    -> GamepadState
    -> configJ binding IDs / common evaluator hook
    -> native FUN_0070A* / FUN_0070B* action helpers
    -> active 0x6C controller record
    -> native PC filtering / CInput aggregate / pending / commit
```

The old synthetic `X/Y,Z/R,U/V` JOY bridge is retained only as historical research;
production native input does not depend on WinMM controller enumeration. `JOYINFOEX`
remains only in the legacy AutoSwitch fallback when `NativeGamepad=false`.

## CInput staging / latency: closed

The PC CInput core is now mapped through producer, staging, commit, public masks, the
dormant callback path, and runtime producer ownership. The key PC object fields are:

```text
+0x0C8  live input slot[0], 7 x 0xCC
+0x7E4  one pending snapshot, 7 x 0x40 = 0x1C0
+0x9A8  pending count, normal values 0/1
+0x9AC  producer aggregate slot[0], 7 x 0x4C
+0xBC0  main-poll / async-handoff state
```

`FUN_00708AB0` (GOG) / `FUN_00708B00` (Steam) evaluates physical input, merges it
into the aggregate records, and publishes one pending snapshot when the pending slot
is empty. If the slot is already occupied, later physical activity continues merging
into the aggregate state rather than creating a second queued snapshot.

`FUN_00708300` / `FUN_00708350` consumes the pending snapshot and derives the four
public digital-mask selectors:

```text
selector 0  current / held
selector 1  rising edge
selector 2  rising | repeat
selector 3  previous
```

The native PC main tick calls commit at `DP.exe+0x00001AB0`, then poll at
`DP.exe+0x00001AF0`. A sample acquired on tick N is therefore normally committed on
tick N+1. Runtime producer census confirmed that normal gameplay uses the main-thread
producer only: a clean Steam run recorded 2361 main / 0 background producer calls in
vanilla and 3629 main / 0 background calls with the same-frame reorder.

This also closes the patch-safety question. A single `poll -> commit` reorder preserves
one native commit and one native edge/repeat derivation per tick, so the fresh sample
becomes live in the same tick. An *extra* commit is not equivalent and can erase a
newly created rising edge because commit begins by copying current -> previous.

`CInput+0xBC0` is now understood as a three-state main-poll/async-handoff protocol:
`0` normal main polling, `1` handoff requested with one final poll, `2` main polling
suppressed. The setter/query APIs have no shipped xrefs in either PC build and runtime
telemetry remained in state 0. The 33.333 ms callback worker implementation also
exists, but the normal shipped CInput lifecycle does not launch it; the runtime census
observed no background producer thread. It is dormant infrastructure, not a 30 Hz
gameplay-input cap.

The original Xbox 360 path is different: `sub_82523238` calls `XamInputGetState` and
builds current/rising/repeat state directly in the same update. The PC one-deep pending
snapshot boundary is therefore a port-side architecture difference rather than an
original Xbox requirement.

See [`../evidence/cinput_pipeline/README.md`](../evidence/cinput_pipeline/README.md) for
the cross-build addresses, layout, runtime census, and Xbox comparison.

## 2026-10-04 typed root/lifecycle confirmation

The latest Census independently types the Steam cached roots used by the already mapped
pipeline. `00BD9E10` is a zero-offset `TSiHolder<CInput>` / CInput receiver; it is one
object containing all seven logical channels, not seven CInput objects. The selected
field map agrees with the existing pending/aggregate/live layout (`+0x7E4`, `+0x9A8`,
`+0x9AC`, `+0x0C8`, `+0xBC0`).

Steam `00BE1EA4` is a typed `CSingleton<CCamera>` root. The selected frame root reads the
same CInput cache, commits pending input and then conditionally polls before object
dispatch. A separate selected manager phase reaches the camera dispatcher under its own
guards. These static placements strengthen receiver identity but do not replace the
existing runtime latency census or create a once-per-frame guarantee in every game mode.

The Census also confirms the state-to-camera table and camera-mode table as distinct from
input staging storage. Its positive typed CPlayer receiver proof is still scoped to the
selected chain; do not promote every offset-compatible state setter by analogy.

Exact source report:
`../evidence/mega_re_census_2026-10-04/boundaries/input_camera_primary_roots.md`.

## UI direction masks

Confirmed combined digital/analog direction composites:

```text
Up    0x4001
Down  0x8002
Left  0x10004
Right 0x20008
```

Friendly names for other low logical bits should be derived from concrete consumers,
not assigned globally by resemblance.

## Camera state -> mode architecture

The CPlayer state-to-camera table at `0x008A9980` selects `CCamera+0x154`. The normal
camera dispatcher uses `0x008A9BC8[mode]`.

Confirmed anchors:

```text
mode 2  aim/combat
mode 9  player vehicle camera; states 87/88
mode 10 context-target path; state 02
mode 11 context-target path; state 46
mode 16 Telescope; state 65
```

Important corrections:

- mode 2 aim uses a bounded target/reticle accumulator and exact edge equality to hand motion into real camera yaw/pitch; forcing x87 PC=53 reproduces the restricted edge-follow failure locally while PC=24 restores it;
- PC and Xbox differ on repeated explicit mode-2 entry: Xbox reinitializes mode 2 every time, while PC suppresses two fresh-init blocks when previous camera mode is already 2;
- Alt+Tab repair is independently explained by the PC foreground gate neutralizing input, dropping held AIM, exiting common aim/weapon states to camera mode 0, then re-entering mode 2 fresh after focus returns;
- ordinary mode-0 free-look re-anchors the camera target before the common look pass;
  the local `lookX * 2 degrees` instruction does not by itself prove an FPS bug;
- vehicle mode 9 already has a signed 0.25 deadzone, 4/3 renormalization,
  data-driven sensitivity, and anchor-relative target semantics;
- modes 10/11 remain the strongest static incremental-camera timing candidate and
  require runtime A/B before any patch;
- the old suspected second 0.25 live-aim deadzone was a false read; the compared PC
  constant is zero.

## ZachFix Xbox aim shaping guard

The caller-scoped Xbox aim-shaping hook is valid only in controller mode. The same
CInput pair carries mouse look while `USEJOY == 0`. Production now requires the
vanilla controller mode before applying the Xbox curve, so AutoSwitch cannot route
mouse deltas through controller-only aim shaping.

## Original Xbox controls retained by the PC state machine

### Combat Strafe

Static Xbox -> PC comparison recovers:

```text
Xbox LB edge, RB not held -> state 09
Xbox RB edge, LB not held -> state 0A
```

The recovered gate also uses capability `0x2000` and a Player flag corresponding to
`+0x638 & 0x10` on the PC layout.

GOG/Steam retain the `09/0A` consumers, mirrored motions `0x242C..0x2431`, and return
behavior, but the identified high-level PC Player update omits the homologous Xbox
ingress call.

### Quick Turn

Original Xbox behavior uses a stick-back condition plus the original Run/X action to
request a target yaw of current yaw + pi and temporarily enter state `0x0B`.

Director's Cut retains the full state-`0x0B` heading interpolation consumer, but its
input gating was changed. This is a strong restoration candidate, not a shipped
feature.

## Production status

Shipped today:

- Native Gamepad bridge: SDL3-first `Auto` backend with XInput fallback, direct `GamepadState -> DP action helpers -> 0x6C` integration;
- PC/Xbox360 stick and aim profiles;
- analog vehicle LT/RT restoration at the three confirmed binary consumers;
- vibration bridge;
- automatic keyboard/controller switching;
- dynamic glyph themes;
- Combat Strafe `09/0A` ingress restoration behind `Gamepad.RestoreCombatStrafe`; Native Gamepad + Xbox360 profile only, disabled by default. Steam 1.01b runtime testing confirmed one physical shoulder press produces one matching state request in the expected order.

Validated CInput implementation:

- the reversible same-frame `poll -> commit` order is structurally and runtime validated; the research-only producer/GPU-queue probes are not required by the final CInput mechanism.

Research-only / experimental:

- mode-2 x87 PC24 guard: causally validated against the forced precision failure and known to correct the affected behavior when it occurs; it remains experimental because the normal-session trigger/root cause is still unidentified;
- camera modes 10/11 timing patch;
- Quick Turn `0x0B` trigger restoration. The first runtime trigger experiment was removed; the recovered architecture/evidence is retained only for future research.

## Remaining high-value tests

CInput core itself no longer has an open timing/concurrency target. Remaining work belongs to consumers or original-control restoration:

1. Keep the narrowly scoped mode-2 PC24 precision guard experimental until reproducible evidence identifies the normal-session trigger/root cause; the successful workaround does not by itself prove why the bad state arises.
2. Runtime A/B for camera states 02 and 46 / modes 10 and 11.
3. Runtime validation of the `09/0A` Combat Strafe bridge on GOG and broader combat/FPS coverage; Steam 1.01b ingress and edge behavior are validated. State `0B` remains research-only.
4. Finish confirm/cancel/menu action semantics from concrete UI consumers.

## MegaRE final input addendum (2026-10-08)

The final static synthesis separates five controller state domains:

```text
P raw physical -> L logical 0x6C -> A aggregate -> Q pending -> I live
```

Seven WinMM-shaped raw rows (stride `0x36`) sit above the native binding interpreter. Initialization explicitly selects raw row 0 by setting its selected byte to exactly `1`, and the selected-row consumer compares against `== 1`. The logical/action and aggregate/pending/live layers are downstream of the OS API boundary and are therefore the natural place to preserve native game semantics while replacing only physical acquisition.

The pending/live relationship is approximately one update of staging, not a guaranteed one-rendered-frame delay. Fixed per-producer axis/trigger filtering makes provider invocation cadence part of input behavior.

Checkpoint 260 identifies a CGame-relative 8-DWORD control-mask bank used by captured-input queries and custom message/glyph-token formatting. Composite/unmatched masks can preserve partial token policy and some special paths bypass the generic mapping. This is directly relevant to SDL3 provider work and native glyph/remapping work.

Actuator storage/access is better mapped, but the native hardware executor remains UNKNOWN.

See [`FLOW_CONTROLLER_INPUT.md`](../evidence/mega_re_final_2026-10-08/maps/FLOW_CONTROLLER_INPUT.md).
