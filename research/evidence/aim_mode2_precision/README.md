# Mode-2 aim camera, x87 precision, and repeated-entry evidence

**Research snapshot:** 2026-09-29
**Scope:** Deadly Premonition Director's Cut PC aim-range / camera-follow regression research.

This dossier separates two confirmed mechanisms that can look related but are not yet proven to share one historical root cause:

1. a **precision-sensitive exact-equality handoff** inside the live PC mode-2 aim handler;
2. a **PC-only repeated-mode2 initialization suppression** in the camera setter, absent from the original Xbox path.

The scoped PC24 guard corrects the affected behavior, but the trigger that places a normal session into the precision-sensitive bad state is still unidentified. Conclusions below therefore distinguish causal precision evidence from root-cause evidence.

---

## 1. State -> camera ownership

PC CPlayer state transition writes:

```text
CPlayer+0x654 current state
CPlayer+0x658 previous state
    -> 0x008A9980[state]
    -> CCamera mode setter
    -> CCamera+0x154 current mode
       CCamera+0x158 previous mode
```

Mode `2` is the aim/combat camera. Relevant state family:

```text
01 09 0A 0C 14 16 18 1A 1C 1E 20 22 24 26 28 2A 2B
```

Live mode-2 handler:

```text
GOG   0053B980
Steam FUN_0053B8B0
Xbox  sub_8233B3C0
```

---

## 2. Live PC target -> camera handoff

The PC handler is not simply "stick -> camera angle". It first moves a bounded aim target/reticle offset and only transfers motion into actual camera orientation at the edge.

Steam camera fields:

```text
+0x6C  actual/live pitch
+0x70  actual/live yaw
+0x8C  smoothed pitch offset
+0x90  smoothed yaw offset
+0x9C  pitch target accumulator
+0xA0  yaw target accumulator
+0x120 bit0  mode-2 persistence/follow-policy input
```

Horizontal edge:

```cpp
limit = weaponAimParams[5] * 3.2f * DEG_TO_RAD;
A0 = clamp(A0, -limit, +limit);

if (!(cameraFlags & 1) || A0 == -limit || A0 == +limit)
    yaw -= yawRate * input;
```

Vertical edge:

```cpp
limit = weaponAimParams[5] * DEG_TO_RAD;
A9C = clamp(A9C, -limit, +limit);

if (!(cameraFlags & 1) || abs(A9C) == limit)
    pitch -= pitchRate * input;
```

Representative Steam equality sites:

```text
horizontal: 0053C18C..0053C1A8 -> actual yaw update 0053C1ED
vertical:   0053C3A6..0053C3C3 -> actual pitch update 0053C3EC
```

This exactly matches the reported visual class: reticle reaches its screen-space bound, but the camera does not continue following.

---

## 3. x87 precision hazard

The clamp target is stored as float32, while the edge limit can remain live in x87 register precision. The equality gate therefore depends on x87 precision-control semantics.

Local A/B result:

```text
Native ambient PC24 = normal
Force53             = restricted edge-follow failure reproduced
Force24             = normal
```

The failure is mechanically straightforward: PC=53 can keep the computed limit at a value that is not bit-equivalent to the rounded float32 target loaded for comparison, so an exact equality test fails even though the clamp visually reached its edge.

### Evidence status

```text
PC=53 can cause the symptom                  CONFIRMED CAUSALLY
PC=24 removes that forced symptom             CONFIRMED CAUSALLY
normal affected session enters PC=53/64       NOT PROVEN
Alt+Tab changes local ambient x87 precision   NOT OBSERVED
```

A narrow PC24 guard around the native mode-2 handler is known to correct the affected behavior. It remains experimental because the normal-session trigger/root cause is still unidentified; it should not be generalized to the whole process or described as a proven root-cause repair.

---

## 4. PC/Xbox repeated-mode2 initialization policy

The original Xbox and PC both explicitly route Player state changes through the camera-mode setter. Their mode-2 initialization policy differs.

### Xbox

Every explicit `SetCameraMode(2)` performs fresh mode-2 anchor/transient setup.

### PC

PC added two `previousCameraMode != 2` gates.

```text
Gate A
    fresh orientation / anchor synchronization

Gate B
    clear +0x74/+0x78
    clear +0x8C..+0xA8 transient block
```

So an explicit repeated mode-2 state request behaves differently:

```text
Xbox mode2 -> explicit SetState(mode2)
    -> SetCameraMode(2)
    -> fresh setup

PC mode2 -> explicit SetState(mode2)
    -> SetCameraMode(2)
    -> previous mode == 2
    -> Gate A and Gate B skipped
```

The surrounding camera writers/resetters were compared and largely match across platforms. Confirmed matched families include:

```text
alternate small-window aim producer
    PC FUN_004E8790
    Xbox sub_822EA858

target-lock/cycle full transient reset
    PC FUN_004FF460
    Xbox sub_82302EC8

live mode-2 integrator
    PC FUN_0053B8B0
    Xbox sub_8233B3C0

0x2000 decay/neutralization
    PC FUN_005354D0
    Xbox sub_823367B0

broad camera restore resets
    PC FUN_00606B50 / FUN_00608D20
    Xbox sub_823DF940 / sub_823E1BF0
```

The repeated-mode2 setter suppression remains the main confirmed policy divergence in this block.

---

## 5. Ordinary stock weapon ingress is normally safe

Primary weapon mode-2 states were censused for direct Player SetState exits. The stock handlers do not form a common primary-mode2 -> primary-mode2 loop; they exit to state 0 or non-mode2 families.

Normal class-based weapon entry passes through boundary states `0E/0F`, both camera mode 0:

```text
0E/0F (mode0)
    -> ITEM.PRM class selector
    -> primary weapon state 14/16/.../2B (mode2)
```

This forces a genuine mode0 -> mode2 transition and therefore runs both PC fresh-init gates.

State `0C` is mode 2 but its known AIM-release exit returns to state 0, and the full reviewed setter/writer census did not recover a normal stock ingress into `0C`.

---

## 6. Preserved 09/0A combat-strafe path is a real repeated-mode2 producer

Original Xbox shoulder ingress selects states `09/0A`, both camera mode 2. The PC consumer remnants survive.

Their completion route is special:

```text
09/0A (mode2)
    -> 0E
    -> camera setter special-case preserves mode2
    -> weapon selector requests primary mode2 state
```

Therefore the final request is explicitly mode2 -> mode2 on PC.

This is architecturally important because it exercises the PC/Xbox reset-policy divergence without CEvent scripting. However, Director's Cut removed the original high-level 09/0A ingress. The path is not a historical stock-PC producer unless ZachFix's optional Combat Strafe restoration is enabled.

---

## 7. Alt+Tab repair mechanism

PC input production contains a foreground/active-window gate equivalent to:

```text
GetForegroundWindow() == GetActiveWindow()
```

On focus loss the logical controller record becomes neutral. Common aim handlers then observe held AIM as false.

Confirmed examples:

```text
state 01 handler
    AIM false -> Player SetState(0)

common firearm 1A/1B handler
    held AIM false -> cleanup -> Player SetState(0)
```

This yields:

```text
focus loss
    -> neutral input
    -> AIM false
    -> aim/weapon state exits
    -> camera mode2 -> mode0

focus regain + re-aim
    -> mode0 -> mode2
    -> both PC fresh-init gates run
```

So Alt+Tab can repair stale/bad mode-2 state through ordinary state-machine re-entry. This explanation does not require an x87 control-word change, and local testing did not observe one.

---

## 8. CEvent A6/4: architectural proof, rejected gameplay cause

PC CEvent opcode `0xA6`, subcommand `4` reaches the weapon/equip helper:

```text
FUN_00436450
    -> FUN_0044D520
    -> read ITEM.PRM class at record+0x6D, stride 0xDC
    -> map class to primary weapon Player state
    -> Player SetState(selectedState)
```

Primary class mapping:

```text
1->14  2->16  3->18  4->1A  5->1E  6->20
7->24  8->28  9->2A  10->2B  11->26
```

Xbox has the exact semantic counterpart:

```text
central CEvent dispatcher sub_82230A18
    -> sub_822215A0
    -> sub_82237148
    -> same ITEM.PRM +0x6D / 0xDC mapping
    -> sub_8232A300(selectedState)
```

Neither platform adds a current-state guard before the explicit Player SetState request. This proves that explicit same-mode state requests are legitimate original engine semantics and explains why Xbox's unconditional mode-2 reset is robust.

### Full shipped content census

The event archive supplied from the PC install was parsed structurally, not searched as raw bytes.

DSB command layout:

```text
+0x00 u16 control/branch field
+0x02 u16 operand byte count
+0x04 u16 opcode word (CEvent uses low byte)
+0x06 u16 flags
+0x08 operand bytes
next = current + 8 + operandBytes
```

Census result:

```text
DSB files parsed:             656
CEvent commands decoded:      61,498
opcode A6 occurrences:        55 in 17 files
A6 / subcommand 4:            2
files containing A6/4:        1
```

Both A6/4 commands occur in:

```text
UPDATA/EVENT/91/0_0455.DSB
routine: 銃撃テスト ("Shooting Test")
```

Nearby commands form a developer shooting-test sequence. The same DSB contains many explicitly named internal test routines. No ordinary story/COMMON/SUBEVENT content uses A6/4.

Status:

```text
A6/4 as spontaneous-failure root cause      CLOSED / REJECTED
A6/4 as proof of valid explicit re-entry    RETAINED / CONFIRMED
```

---

## 9. XFE correction

`D01.XPF` references `CPL01.XFE`, and runtime logs resolve it as a live resource. That initially made it look like a candidate event bytecode container.

Direct inspection disproved that interpretation. `CPL01.XFE` begins with `XAM2` and contains facial-expression node names such as:

```text
N_EYEL
NT_JAW
NT_MOUTH*
NT_BROW*
```

It is a facial/expression animation resource, not CEvent bytecode. Actual event command streams are `.DSB` resources under `UPDATA/EVENT`.

---

## 10. Current causal boundary

What is established:

```text
CONFIRMED
- exact mode-2 target -> camera edge handoff
- x87 PC=53 can break that exact-equality handoff
- PC24 fixes the forced precision failure locally
- Xbox and PC differ on repeated explicit mode-2 initialization
- Alt+Tab can force a clean mode2 -> mode0 -> mode2 reset
- common stock weapon ingress is normally mode0 -> mode2
- A6/4 is not used by normal shipped gameplay
```

What remains open:

```text
OPEN
- what causes a normal session to enter the spontaneous bad state
- whether another as-yet-unidentified stock path creates stale mode-2 state that the
  PC repeated-mode suppression preserves
```

The precision hazard is the only mechanism found that produces a closely matching local
phenocopy, and PC24 fixes that forced case. The scoped guard is therefore retained as an
experimental best-effort workaround. The repeated-mode2 setter divergence remains a
separate confirmed architectural regression candidate; neither finding proves the exact
spontaneous trigger.

---

## 11. Retained raw research notes

For auditability, the repository keeps the two long-form research records used to produce this summary:

- [`aim_range_RE_worklog_2026-09-29.md`](aim_range_RE_worklog_2026-09-29.md) - full aim-range / camera-follow investigation, x87 A/B, PC/Xbox writer/reset census, and camera-state analysis.
- [`aim_bad_entry_trace_2026-09-29.md`](aim_bad_entry_trace_2026-09-29.md) - focused search for a stock repeated-mode2 producer, Xbox A6/4 comparison, XFE correction, and full DSB content-census closure.

The summary in this README is authoritative where those notes contain earlier intermediate hypotheses that were later corrected.

---

## 12. Experimental scoped PC24 workaround (2026-09-30)

The causal A/B result above has now been packaged as an opt-in runtime experiment rather
than a permanent production assumption.

```text
[Experimental]
AimFpuPrecisionFix = false
```

Implementation contract:

- the hook is build/signature gated and prepared only after MinHook initialization;
- when enabled, it forces x87 PC24 only for the native mode-2 aim handler;
- the caller's original precision-control bits are restored immediately afterward;
- the hook can be enabled or disabled live through the normal F10 **Apply** path;
- disabling it removes the interception with `MH_DisableHook`, returning the native path;
- the default remains `false` because the normal-session trigger/root cause is still unidentified and the workaround remains experimental.

This does **not** promote the FPU hypothesis to a proven root cause. Forced PC53
causally reproduces the failure, PC24 eliminates that forced case, and the scoped guard
is known to correct the affected behavior. The unresolved part is why a normal gameplay
session enters the precision-sensitive bad state. The guard therefore remains narrow,
reversible, and experimental. The repeated-mode2 initialization divergence remains a
separate confirmed architectural finding.
