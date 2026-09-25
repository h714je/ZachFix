#include "native_xinput.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"
#include "input_mode.h"

#include <Windows.h>
#include <intrin.h>
#include <mmsystem.h>
#include <Xinput.h>
#include <MinHook.h>

#include <atomic>
#include <bit>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <mutex>

namespace
{
// int __cdecl EvaluateControllerBinding(JOYINFOEX fields..., int binding)
//
// Every controller action reaches this evaluator. DP's binding enum assumes
// the original shared-trigger WinMM layout, while the XInput bridge below
// exposes independent sticks/triggers. Remapping here preserves DP's mutable
// controller-binding table and all vanilla action logic.
constexpr unsigned char kExpectedEvaluatorBytes[] = {
    0x55,                         // push ebp
    0x8B, 0xEC,                   // mov  ebp,esp
    0x81, 0xEC, 0xC4, 0x00, 0x00, 0x00, // sub esp,0C4h
    0x8B, 0x45, 0x3C,             // mov eax,[ebp+3Ch]
    0x89, 0x45, 0xFC              // mov [ebp-04h],eax
};

using ControllerBindingEvaluatorFn = int (__cdecl*)(
    DWORD size,
    DWORD flags,
    DWORD x,
    DWORD y,
    DWORD z,
    DWORD r,
    DWORD u,
    DWORD v,
    DWORD buttons,
    DWORD buttonNumber,
    DWORD pov,
    DWORD reserved1,
    DWORD reserved2,
    int binding);
ControllerBindingEvaluatorFn g_originalControllerBindingEvaluator = nullptr;

// PC CInput stick post-processor:
//   Steam DP.exe+0x00309400
//   GOG   DP.exe+0x003093B0
//
// It receives integer A+D/W+S and right-stick pairs, applies another +/-16
// deadzone, divides the surviving range by 109, then slews the final float by
// at most 0.5 per input update. The Xbox360 profile restores the exact Xbox
// normalized final stick floats after the PC routine has finished, leaving
// downstream locomotion/camera/aim routing untouched.
constexpr unsigned char kExpectedStickAxisPostProcessorBytes[] = {
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x14, 0x89, 0x4D,
    0xEC, 0x8B, 0x45, 0x0C, 0xDB, 0x40, 0x3C
};

using StickAxisPostProcessorFn = void (__thiscall*)(
    void* self, int inputSlot, void* actionState);
StickAxisPostProcessorFn g_originalStickAxisPostProcessor = nullptr;
std::atomic_bool g_gamepadStickProfileHookInstalled{ false };
std::atomic<GamepadInputProfile> g_gamepadInputProfile{
    GamepadInputProfile::Xbox360
};
std::atomic_bool g_analogVehicleTriggersEnabled{ true };
std::atomic<UINT> g_vehicleTriggerDeadzoneRaw{ 30 };

// Final float getter used by the proven live-aim consumers. The hook stays
// installed and switches between vanilla PC passthrough and exact Xbox 360
// aim shaping at runtime.
constexpr unsigned char kExpectedStickFloatGetterBytes[] = {
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x0C, 0x89, 0x4D,
    0xF4, 0xC6, 0x45, 0xF8, 0x00, 0xC6, 0x45, 0xF9
};

using StickFloatGetterFn = float (__thiscall*)(
    void* self, unsigned char inputSlot, unsigned char stick, unsigned char component);
StickFloatGetterFn g_originalStickFloatGetter = nullptr;
uintptr_t g_rightStickAimCallerRvas[4]{};

constexpr float kXboxCameraDeadzone =
    std::bit_cast<float>(std::uint32_t{ 0x3E800000u });
// Exact Xbox XEX constant 0x3FAAAAAB. The aim path subtracts the
// signed 0.25 camera deadzone, then scales the surviving 0.75 range
// back to 1.0.
constexpr float kXboxCameraPostDeadzoneScale =
    std::bit_cast<float>(std::uint32_t{ 0x3FAAAAABu });

bool IsRightStickAimCaller(uintptr_t callerRva)
{
    for (const uintptr_t candidate : g_rightStickAimCallerRvas)
    {
        if (candidate != 0 && callerRva == candidate)
            return true;
    }
    return false;
}

float ApplyXboxCameraDeadzone(float value)
{
    // Exact Xbox sub_8233B3C0 shaping for the live aiming path:
    //   abs(axis) <= 0.25 -> 0
    //   otherwise subtract signed 0.25 and multiply by 4/3.
    // This renormalizes the surviving 0.75 range back to 0..1.
    if (value > kXboxCameraDeadzone)
        return (value - kXboxCameraDeadzone) * kXboxCameraPostDeadzoneScale;
    if (value < -kXboxCameraDeadzone)
        return (value + kXboxCameraDeadzone) * kXboxCameraPostDeadzoneScale;
    return 0.0f;
}

float __fastcall HookStickFloatGetter(
    void* self,
    void*,
    unsigned char inputSlot,
    unsigned char stick,
    unsigned char component)
{
    const float value = g_originalStickFloatGetter(
        self, inputSlot, stick, component);

    if (stick == 1 && component <= 1 && g_mainExeBase != 0 &&
        g_gamepadStickProfileHookInstalled.load(std::memory_order_acquire) &&
        g_gamepadInputProfile.load(std::memory_order_acquire) ==
            GamepadInputProfile::Xbox360)
    {
        // CInput pair 1 is shared with mouse look while USEJOY == 0.
        // Never apply controller-only Xbox aim shaping to mouse deltas.
        bool controllerMode = false;
        if (!TryGetVanillaInputMode(controllerMode) || !controllerMode)
            return value;

        const uintptr_t caller =
            reinterpret_cast<uintptr_t>(_ReturnAddress());
        const uintptr_t callerRva =
            caller >= g_mainExeBase ? caller - g_mainExeBase : 0;

        if (IsRightStickAimCaller(callerRva))
            return ApplyXboxCameraDeadzone(value);
    }

    return value;
}

bool InstallRightStickAimProfileHook(const DpBuildProfile& build)
{
    for (size_t i = 0; i < 4; ++i)
        g_rightStickAimCallerRvas[i] = build.input.rightStickAimCallerRvas[i];

    auto* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.input.stickFloatGetterRva);

    if (std::memcmp(
            target,
            kExpectedStickFloatGetterBytes,
            sizeof(kExpectedStickFloatGetterBytes)) != 0)
    {
        AppendLog(
            "[Input] ERROR: stick float getter signature mismatch; "
            "Xbox 360 aim shaping unavailable.\n");
        return false;
    }

    MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookStickFloatGetter),
        reinterpret_cast<void**>(&g_originalStickFloatGetter));
    if (status != MH_OK)
    {
        AppendLog(
            "[Input] ERROR: failed to hook stick float getter; "
            "Xbox 360 aim shaping unavailable.\n");
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        MH_RemoveHook(target);
        AppendLog(
            "[Input] ERROR: failed to enable stick float getter hook; "
            "Xbox 360 aim shaping unavailable.\n");
        return false;
    }

    return true;
}

constexpr int kXboxStickDeadzoneRaw = 7864;
constexpr BYTE kXboxTriggerDeadzoneRaw = 30;
// Exact Xbox XEX constant at guest 0x8201DC8C: 0x38286CF7.
constexpr float kXboxStickScale =
    std::bit_cast<float>(std::uint32_t{ 0x38286CF7u });
// Xbox trigger normalization multiplies the surviving raw BYTE by the
// single-precision representation of 1/255.
constexpr float kXboxTriggerScale =
    std::bit_cast<float>(std::uint32_t{ 0x3B808081u });

std::atomic<float> g_xboxLeftStickX[XUSER_MAX_COUNT]{};
std::atomic<float> g_xboxLeftStickY[XUSER_MAX_COUNT]{};
std::atomic<float> g_xboxRightStickX[XUSER_MAX_COUNT]{};
std::atomic<float> g_xboxRightStickY[XUSER_MAX_COUNT]{};
std::atomic_bool g_xboxLeftStickValid[XUSER_MAX_COUNT]{};

float NormalizeXboxStickAxis(SHORT raw)
{
    const int value = static_cast<int>(raw);
    if (value > kXboxStickDeadzoneRaw)
    {
        return static_cast<float>(value - kXboxStickDeadzoneRaw) *
               kXboxStickScale;
    }
    if (value < -kXboxStickDeadzoneRaw)
    {
        // Original Xbox code converts (raw + 7864) to float, adds +1.0f,
        // then multiplies by the same scale. This keeps the negative endpoint
        // symmetric with the positive endpoint despite the signed-short range.
        return (static_cast<float>(value + kXboxStickDeadzoneRaw) + 1.0f) *
               kXboxStickScale;
    }
    return 0.0f;
}

// DP evaluator binding values 0x29..0x38. Axis pairs are LOW then HIGH:
//   X: 0x2D / 0x2E
//   Y: 0x2F / 0x30
//   Z: 0x31 / 0x32
//   U: 0x33 / 0x34
//   V: 0x35 / 0x36
//   R: 0x37 / 0x38
//
// DPLauncher confirms the legacy shared-Z trigger semantics:
//   0x31 = RT, 0x32 = LT.
constexpr int kAxisZLow = 0x31;
constexpr int kAxisZHigh = 0x32;
constexpr int kAxisULow = 0x33;
constexpr int kAxisUHigh = 0x34;
constexpr int kAxisVHigh = 0x36;

void WriteFloatField(void* object, size_t offset, float value)
{
    if (object == nullptr)
        return;
    auto* base = static_cast<unsigned char*>(object);
    std::memcpy(base + offset, &value, sizeof(value));
}

void __fastcall HookStickAxisPostProcessor(
    void* self, void*, int inputSlot, void* actionState)
{
    g_originalStickAxisPostProcessor(self, inputSlot, actionState);

    if (!g_gamepadStickProfileHookInstalled.load(std::memory_order_acquire) ||
        self == nullptr || actionState == nullptr ||
        inputSlot < 0 || inputSlot >= static_cast<int>(XUSER_MAX_COUNT))
    {
        return;
    }

    if (g_gamepadInputProfile.load(std::memory_order_acquire) !=
        GamepadInputProfile::Xbox360)
    {
        return;
    }

    // 0x709C40 marks only the currently active input slot with state[0] = 1;
    // all other 0x6C-byte records are zeroed. Do not populate inactive slots.
    DWORD active = 0;
    std::memcpy(&active, actionState, sizeof(active));
    if (active != 1u)
        return;

    bool controllerMode = false;
    if (!TryGetVanillaInputMode(controllerMode) || !controllerMode)
        return;

    if (!g_xboxLeftStickValid[inputSlot].load(std::memory_order_acquire))
        return;

    const float xboxX =
        g_xboxLeftStickX[inputSlot].load(std::memory_order_relaxed);
    const float xboxY =
        g_xboxLeftStickY[inputSlot].load(std::memory_order_relaxed);
    const float xboxRightX =
        g_xboxRightStickX[inputSlot].load(std::memory_order_relaxed);
    const float xboxRightY =
        g_xboxRightStickY[inputSlot].load(std::memory_order_relaxed);

    constexpr size_t kPerSlotStride = 0x4C;
    const size_t slotBase = static_cast<size_t>(inputSlot) * kPerSlotStride;

    // PC stores its internal Y as down-positive; 0x708A30 negates component Y
    // before gameplay sees it. Store -XboxY so that the getter returns the
    // original Xbox convention: positive Y = stick up.
    WriteFloatField(self, 0x9B8 + slotBase, xboxX);
    WriteFloatField(self, 0x9BC + slotBase, -xboxY);

    // Right stick uses the adjacent final-float pair and the same getter-side
    // Y inversion. Xbox camera/aim consumers read RX/RY directly from the
    // normalized controller state, so bypass only PC's extra input filtering;
    // do not alter any downstream camera sensitivity/deadzone math here.
    WriteFloatField(self, 0x9C0 + slotBase, xboxRightX);
    WriteFloatField(self, 0x9C4 + slotBase, -xboxRightY);
}

bool InstallGamepadStickProfileHook(const DpBuildProfile& build)
{
    auto* target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.input.stickAxisPostProcessorRva);

    if (std::memcmp(
            target,
            kExpectedStickAxisPostProcessorBytes,
            sizeof(kExpectedStickAxisPostProcessorBytes)) != 0)
    {
        AppendLog(
            "[Input] ERROR: stick post-processor signature mismatch; "
            "Xbox 360 stick profile unavailable.\n");
        return false;
    }

    MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookStickAxisPostProcessor),
        reinterpret_cast<void**>(&g_originalStickAxisPostProcessor));
    if (status != MH_OK)
    {
        AppendLog(
            "[Input] ERROR: failed to hook stick post-processor; "
            "Xbox 360 stick profile unavailable.\n");
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        MH_RemoveHook(target);
        AppendLog(
            "[Input] ERROR: failed to enable stick post-processor hook; "
            "Xbox 360 stick profile unavailable.\n");
        return false;
    }

    g_gamepadStickProfileHookInstalled.store(true, std::memory_order_release);

    return true;
}

using JoyGetPosExFn = MMRESULT (WINAPI*)(UINT, LPJOYINFOEX);
JoyGetPosExFn g_originalJoyGetPosEx = nullptr;

using XInputGetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
using XInputSetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_VIBRATION*);
HMODULE g_xinputModule = nullptr;
XInputGetStateFn g_xinputGetState = nullptr;
XInputSetStateFn g_xinputSetState = nullptr;

std::atomic<DWORD> g_activeXInputUser{ 0 };
std::atomic_bool g_activeXInputUserValid{ false };

// Continuous Xbox 360 trigger state consumed by the three restored vehicle
// control sites. The detours remain installed, but g_vehicleAnalogTriggersValid
// is cleared whenever the live Analog Vehicle Triggers option is disabled so
// each site falls back to Director's Cut's original digital path.
alignas(4) volatile LONG g_vehicleAnalogTriggersValid = 0;
alignas(4) volatile float g_vehicleLeftTrigger01 = 0.0f;
alignas(4) volatile float g_vehicleRightTrigger01 = 0.0f;
std::atomic_bool g_nativeXInputBackendInstalled{ false };
std::mutex g_combatStrafeInputMutex;
bool g_combatStrafeInputInitialized = false;
DWORD g_combatStrafeInputUser = 0;
WORD g_combatStrafePreviousShoulders = 0;
std::atomic_bool g_vehicleAnalogPatchInstalled{ false };

std::atomic_bool g_vibrationEnabled{ true };
std::atomic<float> g_vibrationStrength{ 1.0f };
// Native motor pair currently owned by DP, packed as left in bits 0..15 and
// right in bits 16..31. CRdInput::SetActuator receives both channels at full
// 16-bit precision before the PC port truncates the second channel with >> 6.
// Publish the pair atomically so hot-apply/controller-switch refreshes can
// replay one coherent native state without reconstructing either channel.
std::atomic<uint32_t> g_currentNativeMotorState{ 0 };
std::atomic_bool g_nativeVibrationInstalled{ false };


// CRdInput::SetActuator synchronously calls CInput_Actuator::SetSecond after
// truncating actuator B. While that call is in flight, suppress the inner
// SetSecond hook's output; the outer hook publishes the exact A/B pair once
// the original function has completed. thread_local keeps concurrent input
// threads independent and also makes nested dispatch safe.
thread_local unsigned int g_rdInputSetActuatorDepth = 0;
std::mutex g_vibrationOutputMutex;
WORD g_lastMotorLeft = 0;
WORD g_lastMotorRight = 0;
bool g_lastMotorStateValid = false;

// Deadly Premonition still generates its original two-channel rumble commands
// and owns their lifetime/countdown. The PC port leaves CRdInput's actuator
// gate disabled and never forwards the final CInput_Actuator state to hardware.
// ZachFix restores only those two missing steps.
constexpr unsigned char kExpectedRdInputSetActuatorBytes[] = {
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x89, 0x4D,
    0xF8, 0x8B, 0x45, 0xF8, 0x83, 0x78, 0x04, 0x00,
    0x74, 0x43, 0x8B, 0x4D, 0x10, 0xC1, 0xE9, 0x06
};

constexpr unsigned char kExpectedInputActuatorSetSecondBytes[] = {
    0x55, 0x8B, 0xEC, 0x51, 0x89, 0x4D, 0xFC,
    0x8B, 0x4D, 0xFC, 0x83, 0xC1, 0x04
};

using RdInputSetActuatorFn = void (__thiscall*)(
    void* self, int slot, DWORD actuatorA, DWORD actuatorB, float duration);
RdInputSetActuatorFn g_originalRdInputSetActuator = nullptr;

using InputActuatorSetSecondFn = void (__thiscall*)(
    void* self, int slot, DWORD value);
InputActuatorSetSecondFn g_originalInputActuatorSetSecond = nullptr;

DWORD ReadDwordField(const void* object, size_t offset)
{
    DWORD value = 0;
    if (object != nullptr)
    {
        const auto* base = static_cast<const unsigned char*>(object);
        std::memcpy(&value, base + offset, sizeof(value));
    }
    return value;
}

WORD ScaleStoredSecondActuatorToMotor(DWORD stored)
{
    // DP stores the second actuator in 10 bits. Expand the full 0..1023 range
    // back over XInput's 0..65535 WORD range so both endpoints remain exact.
    if (stored >= 1023u)
        return 65535u;
    return static_cast<WORD>((stored * 65535u + 511u) / 1023u);
}

WORD ApplyVibrationStrength(WORD value, float strength)
{
    if (strength <= 0.0f || value == 0)
        return 0;
    if (strength >= 1.0f)
        return value;

    const float scaled = static_cast<float>(value) * strength;
    const DWORD rounded = static_cast<DWORD>(scaled + 0.5f);
    return static_cast<WORD>(rounded > 65535u ? 65535u : rounded);
}

void SendXInputVibration(WORD leftMotor, WORD rightMotor)
{
    if (g_xinputSetState == nullptr)
        return;

    std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);

    if (!g_activeXInputUserValid.load(std::memory_order_acquire))
        return;

    if (g_lastMotorStateValid &&
        g_lastMotorLeft == leftMotor &&
        g_lastMotorRight == rightMotor)
    {
        return;
    }

    const DWORD userIndex = g_activeXInputUser.load(std::memory_order_relaxed);
    if (userIndex >= XUSER_MAX_COUNT)
        return;

    XINPUT_VIBRATION vibration = {};
    vibration.wLeftMotorSpeed = leftMotor;
    vibration.wRightMotorSpeed = rightMotor;
    const DWORD result = g_xinputSetState(userIndex, &vibration);

    if (result == ERROR_SUCCESS)
    {
        g_lastMotorLeft = leftMotor;
        g_lastMotorRight = rightMotor;
        g_lastMotorStateValid = true;
    }
}

void StopXInputVibration(DWORD userIndex, const char* reason)
{
    if (g_xinputSetState == nullptr || userIndex >= XUSER_MAX_COUNT)
        return;

    std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
    XINPUT_VIBRATION stop = {};
    const DWORD result = g_xinputSetState(userIndex, &stop);
    g_lastMotorLeft = 0;
    g_lastMotorRight = 0;
    g_lastMotorStateValid = result == ERROR_SUCCESS;

    char text[224] = {};
    sprintf_s(
        text,
        "[Input][Vibration] stopped user=%lu reason=%s result=%lu.\n",
        static_cast<unsigned long>(userIndex),
        reason != nullptr ? reason : "unknown",
        static_cast<unsigned long>(result));
    AppendLog(text);
}

uint32_t PackNativeMotorState(WORD leftMotor, WORD rightMotor)
{
    return static_cast<uint32_t>(leftMotor) |
           (static_cast<uint32_t>(rightMotor) << 16);
}

void PublishNativeMotorState(WORD leftMotor, WORD rightMotor)
{
    g_currentNativeMotorState.store(
        PackNativeMotorState(leftMotor, rightMotor),
        std::memory_order_release);
}

void RefreshXInputVibrationFromNativeState()
{
    // DP continues to generate native actuator commands even while USEJOY is
    // selecting the keyboard/mouse path. Treat those commands as game state,
    // not permission to drive XInput hardware. This also makes direct actuator
    // updates fail closed if the mode changes outside ZachFix's auto-switcher.
    bool controllerMode = false;
    if (!TryGetVanillaInputMode(controllerMode) || !controllerMode)
    {
        SendXInputVibration(0, 0);
        return;
    }

    if (!g_vibrationEnabled.load(std::memory_order_acquire))
    {
        SendXInputVibration(0, 0);
        return;
    }

    const float strength = g_vibrationStrength.load(std::memory_order_acquire);
    const uint32_t motorState =
        g_currentNativeMotorState.load(std::memory_order_acquire);
    const WORD nativeLeft = static_cast<WORD>(motorState & 0xFFFFu);
    const WORD nativeRight = static_cast<WORD>(motorState >> 16);

    SendXInputVibration(
        ApplyVibrationStrength(nativeLeft, strength),
        ApplyVibrationStrength(nativeRight, strength));
}

void __fastcall HookRdInputSetActuator(
    void* self, void*, int slot, DWORD actuatorA, DWORD actuatorB, float duration)
{
    // Preserve DP's own actuator/countdown path regardless of ZachFix's output
    // toggle. This makes hot-enable seamless and leaves effect timing native.
    if (self != nullptr && ReadDwordField(self, 0x04) == 0)
    {
        const DWORD enabled = 1;
        auto* bytes = static_cast<unsigned char*>(self);
        std::memcpy(bytes + 0x04, &enabled, sizeof(enabled));
    }

    // PC CRdInput::SetActuator performs, synchronously:
    //   SetFirst(slot, actuatorA)
    //   SetSecond(slot, actuatorB >> 6)
    // Keep the inner SetSecond hook from publishing that truncated transient
    // state. Once the original call returns, publish the exact 16-bit pair that
    // DP calculated at this command boundary.
    ++g_rdInputSetActuatorDepth;
    g_originalRdInputSetActuator(self, slot, actuatorA, actuatorB, duration);
    --g_rdInputSetActuatorDepth;

    if (slot != 0)
        return;

    const WORD exactLeft = static_cast<WORD>(
        actuatorA > 65535u ? 65535u : actuatorA);
    const WORD exactRight = static_cast<WORD>(
        actuatorB > 65535u ? 65535u : actuatorB);
    PublishNativeMotorState(exactLeft, exactRight);
    RefreshXInputVibrationFromNativeState();
}

void __fastcall HookInputActuatorSetSecond(
    void* self, void*, int slot, DWORD value)
{
    g_originalInputActuatorSetSecond(self, slot, value);

    if (self == nullptr || slot != 0)
        return;

    // The SetSecond reached from CRdInput::SetActuator contains B >> 6. The
    // outer hook owns that command and will publish the original 16-bit A/B
    // pair after CRdInput returns, so do not emit an intermediate quantized
    // XInput update here.
    if (g_rdInputSetActuatorDepth != 0)
        return;

    // The other native caller is DP's actuator countdown/expiry path. By the
    // time it reaches SetSecond it has already updated channel A too, so read
    // the coherent final state. Nonzero direct SetSecond values retain the old
    // conservative 10-bit expansion as a defensive fallback, while the normal
    // expiry case is exactly zero.
    const DWORD rawA = ReadDwordField(self, 0x28);
    const DWORD rawBStored = value;
    const WORD nativeLeft = static_cast<WORD>(
        rawA > 65535u ? 65535u : rawA);
    const WORD nativeRight = ScaleStoredSecondActuatorToMotor(rawBStored);
    PublishNativeMotorState(nativeLeft, nativeRight);
    RefreshXInputVibrationFromNativeState();
}

DWORD SignedAxisToWinMM(SHORT value, bool invert)
{
    // WinMM joystick axes use 0..65535. XInput Y is positive-up while DP's
    // legacy convention is low=up/high=down, hence the Y-axis inversion.
    const int converted = invert
        ? 32767 - static_cast<int>(value)
        : 32768 + static_cast<int>(value);

    if (converted <= 0)
        return 0;
    if (converted >= 65535)
        return 65535;
    return static_cast<DWORD>(converted);
}

DWORD TriggerToPositiveAxis(BYTE value, GamepadInputProfile profile)
{
    // Xbox 360 exposes LT/RT twice: as continuous 0..1 floats and as digital
    // trigger bits set when raw > 30. The restored vehicle path consumes the
    // continuous floats directly. For ordinary DP actions in the Xbox360
    // profile, present a digital HIGH axis at that exact threshold so the PC
    // evaluator sees the same pressed/released transition instead of its
    // native ~40% axis threshold.
    constexpr DWORD kNeutral = 32767;
    if (profile == GamepadInputProfile::Xbox360)
    {
        return value > kXboxTriggerDeadzoneRaw ? 65535u : kNeutral;
    }

    // PC profile keeps the continuous compatibility view used by the
    // Director's Cut evaluator.
    constexpr DWORD kRange = 65535 - kNeutral;
    return kNeutral + (static_cast<DWORD>(value) * kRange) / 255u;
}

DWORD MapXInputButtons(WORD buttons)
{
    DWORD mapped = 0;
    auto setButton = [&](WORD xinputMask, unsigned int zeroBasedButton)
    {
        if ((buttons & xinputMask) != 0)
            mapped |= (1u << zeroBasedButton);
    };

    // Preserve DP's WinMM button numbering.
    setButton(XINPUT_GAMEPAD_A, 0);
    setButton(XINPUT_GAMEPAD_B, 1);
    setButton(XINPUT_GAMEPAD_X, 2);
    setButton(XINPUT_GAMEPAD_Y, 3);
    setButton(XINPUT_GAMEPAD_LEFT_SHOULDER, 4);
    setButton(XINPUT_GAMEPAD_RIGHT_SHOULDER, 5);
    setButton(XINPUT_GAMEPAD_BACK, 6);
    setButton(XINPUT_GAMEPAD_START, 7);
    setButton(XINPUT_GAMEPAD_LEFT_THUMB, 8);
    setButton(XINPUT_GAMEPAD_RIGHT_THUMB, 9);
    return mapped;
}

DWORD FirstPressedButtonNumber(DWORD buttons)
{
    for (DWORD index = 0; index < 32; ++index)
    {
        if ((buttons & (1u << index)) != 0)
            return index + 1;
    }
    return 0;
}

DWORD MapXInputPov(WORD buttons)
{
    const bool up = (buttons & XINPUT_GAMEPAD_DPAD_UP) != 0;
    const bool down = (buttons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
    const bool left = (buttons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
    const bool right = (buttons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;

    if (up && !down)
    {
        if (right && !left) return 4500;
        if (left && !right) return 31500;
        return 0;
    }
    if (down && !up)
    {
        if (right && !left) return 13500;
        if (left && !right) return 22500;
        return 18000;
    }
    if (right && !left)
        return 9000;
    if (left && !right)
        return 27000;
    return JOY_POVCENTERED;
}

constexpr unsigned char kExpectedVehicleAnalogInjectBytes[] = {
    0xD9, 0x86, 0xE4, 0x13, 0x00, 0x00, // fld  dword ptr [esi+13E4h]
    0xD9, 0xE1,                         // fabs
    0xD9, 0x5C, 0x24, 0x14              // fstp dword ptr [esp+14h]
};

// Xbox 360 reads the same continuous LT/RT controller-state floats in three
// independent car-control layers. Director's Cut replaced each read with its
// action evaluator followed by a 0/1 collapse. These offsets identify the two
// downstream PC copies relative to the primary MovePlyCar consumer so all
// three proven cuts can share one runtime On/Off state.
constexpr uintptr_t kVehicleLowLevelAnalogCutOffsetFromInject = 0x9D5Eu;
constexpr uintptr_t kVehicleHighLevelAnalogCutOffsetFromInject = 0xD273u;
constexpr uintptr_t kVehicleLowLevelAnalogResumeOffset = 0xE4u;
constexpr uintptr_t kVehicleHighLevelAnalogResumeOffset = 0xD9u;

constexpr unsigned char kExpectedVehicleControllerModeCmpBytes[] = {
    0x80, 0x3D, 0xF0, 0x10, 0x48, 0x01, 0x00 // cmp byte ptr [USEJOY],0
};

void EmitVehicle8(unsigned char*& cursor, unsigned char value)
{
    *cursor++ = value;
}

void EmitVehicle32(unsigned char*& cursor, std::uint32_t value)
{
    std::memcpy(cursor, &value, sizeof(value));
    cursor += sizeof(value);
}

void EmitVehicleRel32(
    unsigned char*& cursor,
    unsigned char opcode,
    uintptr_t destination)
{
    EmitVehicle8(cursor, opcode);
    const uintptr_t after =
        reinterpret_cast<uintptr_t>(cursor + sizeof(std::uint32_t));
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(destination - after));
}

bool BuildVehicleLowLevelAnalogCutStub(
    unsigned char* stub,
    size_t stubCapacity,
    uintptr_t useJoyAddress,
    uintptr_t fallbackReturnAddress,
    uintptr_t analogReturnAddress)
{
    if (stub == nullptr || stubCapacity < 128)
        return false;

    unsigned char* cursor = stub;

    // Use Xbox trigger floats only while DP is in controller mode and a live
    // XInput sample exists. Otherwise replay the original USEJOY comparison
    // and fall straight back into Director's Cut's native digital branch.
    EmitVehicle8(cursor, 0x80); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(cursor, static_cast<std::uint32_t>(useJoyAddress));
    EmitVehicle8(cursor, 0x00);
    EmitVehicle8(cursor, 0x74);
    unsigned char* jeFallbackUseJoy = cursor++;

    EmitVehicle8(cursor, 0x83); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleAnalogTriggersValid)));
    EmitVehicle8(cursor, 0x00);
    EmitVehicle8(cursor, 0x74);
    unsigned char* jeFallbackValid = cursor++;

    // Original Xbox sub_82354198 keeps LT in f22 and RT in f21 here. The PC
    // equivalents after its action/bool branch are LT [esp+44h] and
    // RT [esp+18h]. Preserve the two zero locals initialized by the PC code.
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0xEE); // fldz
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x54);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x1C); // fst [esp+1Ch]
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x24); // fstp [esp+24h]

    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x05);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleLeftTrigger01)));
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x44); // LT

    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x05);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleRightTrigger01)));
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x18); // RT

    // Reproduce the convergence instruction skipped with the digital branch.
    EmitVehicle8(cursor, 0xC6); EmitVehicle8(cursor, 0x86);
    EmitVehicle32(cursor, 0x00000580u); EmitVehicle8(cursor, 0x00);
    EmitVehicleRel32(cursor, 0xE9, analogReturnAddress);

    unsigned char* fallback = cursor;
    EmitVehicle8(cursor, 0x80); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(cursor, static_cast<std::uint32_t>(useJoyAddress));
    EmitVehicle8(cursor, 0x00);
    EmitVehicleRel32(cursor, 0xE9, fallbackReturnAddress);

    const std::intptr_t useJoyRel = fallback - (jeFallbackUseJoy + 1);
    const std::intptr_t validRel = fallback - (jeFallbackValid + 1);
    if (useJoyRel < -128 || useJoyRel > 127 ||
        validRel < -128 || validRel > 127)
    {
        return false;
    }
    *jeFallbackUseJoy =
        static_cast<unsigned char>(static_cast<std::int8_t>(useJoyRel));
    *jeFallbackValid =
        static_cast<unsigned char>(static_cast<std::int8_t>(validRel));
    return static_cast<size_t>(cursor - stub) <= stubCapacity;
}

bool BuildVehicleHighLevelAnalogCutStub(
    unsigned char* stub,
    size_t stubCapacity,
    uintptr_t useJoyAddress,
    uintptr_t fallbackReturnAddress,
    uintptr_t analogReturnAddress)
{
    if (stub == nullptr || stubCapacity < 96)
        return false;

    unsigned char* cursor = stub;

    EmitVehicle8(cursor, 0x80); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(cursor, static_cast<std::uint32_t>(useJoyAddress));
    EmitVehicle8(cursor, 0x00);
    EmitVehicle8(cursor, 0x74);
    unsigned char* jeFallbackUseJoy = cursor++;

    EmitVehicle8(cursor, 0x83); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleAnalogTriggersValid)));
    EmitVehicle8(cursor, 0x00);
    EmitVehicle8(cursor, 0x74);
    unsigned char* jeFallbackValid = cursor++;

    // Xbox sub_82356948 loads LT then RT into f30/f13. The matching PC locals
    // are [esp+10h] and [esp+14h]. Keep the fldz that the controller path
    // leaves under those two values at the common continuation.
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x05);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleLeftTrigger01)));
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x10);

    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x05);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleRightTrigger01)));
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x14);
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0xEE); // fldz
    EmitVehicleRel32(cursor, 0xE9, analogReturnAddress);

    unsigned char* fallback = cursor;
    EmitVehicle8(cursor, 0x80); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(cursor, static_cast<std::uint32_t>(useJoyAddress));
    EmitVehicle8(cursor, 0x00);
    EmitVehicleRel32(cursor, 0xE9, fallbackReturnAddress);

    const std::intptr_t useJoyRel = fallback - (jeFallbackUseJoy + 1);
    const std::intptr_t validRel = fallback - (jeFallbackValid + 1);
    if (useJoyRel < -128 || useJoyRel > 127 ||
        validRel < -128 || validRel > 127)
    {
        return false;
    }
    *jeFallbackUseJoy =
        static_cast<unsigned char>(static_cast<std::int8_t>(useJoyRel));
    *jeFallbackValid =
        static_cast<unsigned char>(static_cast<std::int8_t>(validRel));
    return static_cast<size_t>(cursor - stub) <= stubCapacity;
}

using VehicleAnalogCutStubBuilder = bool (*)(
    unsigned char*, size_t, uintptr_t, uintptr_t, uintptr_t);

bool BuildVehicleAnalogStub(
    unsigned char* stub,
    size_t stubCapacity,
    uintptr_t useJoyAddress,
    uintptr_t returnAddress);

struct VehicleAnalogPatchSite
{
    unsigned char* target = nullptr;
    size_t patchSize = 0;
    unsigned char originalBytes[12]{};
    unsigned char patchBytes[12]{};
    unsigned char* stub = nullptr;
    size_t stubCapacity = 0;
    const char* label = nullptr;
};

void ReleaseVehicleAnalogPatchSite(VehicleAnalogPatchSite& site)
{
    if (site.stub != nullptr)
    {
        VirtualFree(site.stub, 0, MEM_RELEASE);
        site.stub = nullptr;
    }
}

bool PrepareVehicleAnalogCut(
    const DpBuildProfile& build,
    uintptr_t offsetFromInject,
    uintptr_t analogResumeOffset,
    VehicleAnalogCutStubBuilder builder,
    const char* label,
    VehicleAnalogPatchSite& site)
{
    site.target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.input.vehicleAnalogInputInjectRva + offsetFromInject);
    site.patchSize = sizeof(kExpectedVehicleControllerModeCmpBytes);
    site.stubCapacity = 128;
    site.label = label;

    if (std::memcmp(
            site.target,
            kExpectedVehicleControllerModeCmpBytes,
            sizeof(kExpectedVehicleControllerModeCmpBytes)) != 0)
    {
        char text[224] = {};
        sprintf_s(
            text,
            "[Input][AnalogVehicle] %s signature mismatch; Xbox analog "
            "consumer transaction aborted.\n",
            label);
        AppendLog(text);
        return false;
    }

    std::memcpy(site.originalBytes, site.target, site.patchSize);

    site.stub = static_cast<unsigned char*>(VirtualAlloc(
        nullptr,
        site.stubCapacity,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE));
    if (site.stub == nullptr)
        return false;

    const uintptr_t targetAddress = reinterpret_cast<uintptr_t>(site.target);
    if (!builder(
            site.stub,
            site.stubCapacity,
            g_mainExeBase + build.input.useJoyModeRva,
            targetAddress + sizeof(kExpectedVehicleControllerModeCmpBytes),
            targetAddress + analogResumeOffset))
    {
        ReleaseVehicleAnalogPatchSite(site);
        return false;
    }

    DWORD stubOldProtect = 0;
    if (!VirtualProtect(
            site.stub,
            site.stubCapacity,
            PAGE_EXECUTE_READ,
            &stubOldProtect))
    {
        ReleaseVehicleAnalogPatchSite(site);
        return false;
    }
    FlushInstructionCache(GetCurrentProcess(), site.stub, site.stubCapacity);

    site.patchBytes[0] = 0xE9;
    site.patchBytes[5] = 0x90;
    site.patchBytes[6] = 0x90;
    const uintptr_t afterJump = reinterpret_cast<uintptr_t>(site.target + 5);
    const std::uint32_t rel = static_cast<std::uint32_t>(
        reinterpret_cast<uintptr_t>(site.stub) - afterJump);
    std::memcpy(site.patchBytes + 1, &rel, sizeof(rel));
    return true;
}

bool PreparePrimaryVehicleAnalogPatch(
    const DpBuildProfile& build,
    VehicleAnalogPatchSite& site)
{
    site.target = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.input.vehicleAnalogInputInjectRva);
    site.patchSize = sizeof(kExpectedVehicleAnalogInjectBytes);
    site.stubCapacity = 96;
    site.label = "player-car";

    if (std::memcmp(
            site.target,
            kExpectedVehicleAnalogInjectBytes,
            sizeof(kExpectedVehicleAnalogInjectBytes)) != 0)
    {
        AppendLog(
            "[Input][AnalogVehicle] Signature mismatch; analog car trigger "
            "transaction aborted.\n");
        return false;
    }

    std::memcpy(site.originalBytes, site.target, site.patchSize);

    site.stub = static_cast<unsigned char*>(VirtualAlloc(
        nullptr,
        site.stubCapacity,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE));
    if (site.stub == nullptr)
    {
        AppendLog(
            "[Input][AnalogVehicle] VirtualAlloc failed; analog car trigger "
            "transaction aborted.\n");
        return false;
    }

    const uintptr_t returnAddress =
        reinterpret_cast<uintptr_t>(site.target) + site.patchSize;
    if (!BuildVehicleAnalogStub(
            site.stub,
            site.stubCapacity,
            g_mainExeBase + build.input.useJoyModeRva,
            returnAddress))
    {
        ReleaseVehicleAnalogPatchSite(site);
        AppendLog(
            "[Input][AnalogVehicle] Failed to build player-car trigger stub; "
            "transaction aborted.\n");
        return false;
    }

    DWORD stubOldProtect = 0;
    if (!VirtualProtect(
            site.stub,
            site.stubCapacity,
            PAGE_EXECUTE_READ,
            &stubOldProtect))
    {
        ReleaseVehicleAnalogPatchSite(site);
        AppendLog(
            "[Input][AnalogVehicle] Failed to protect player-car trigger stub; "
            "transaction aborted.\n");
        return false;
    }
    FlushInstructionCache(GetCurrentProcess(), site.stub, site.stubCapacity);

    site.patchBytes[0] = 0xE9;
    site.patchBytes[5] = 0x90;
    const uintptr_t afterJump = reinterpret_cast<uintptr_t>(site.target + 5);
    const std::uint32_t rel = static_cast<std::uint32_t>(
        reinterpret_cast<uintptr_t>(site.stub) - afterJump);
    std::memcpy(site.patchBytes + 1, &rel, sizeof(rel));
    return true;
}

bool WriteVehicleAnalogPatchSite(
    VehicleAnalogPatchSite& site,
    bool& wroteBytes)
{
    wroteBytes = false;
    DWORD oldProtect = 0;
    if (!VirtualProtect(
            site.target,
            site.patchSize,
            PAGE_EXECUTE_READWRITE,
            &oldProtect))
    {
        return false;
    }

    std::memcpy(site.target, site.patchBytes, site.patchSize);
    FlushInstructionCache(GetCurrentProcess(), site.target, site.patchSize);
    wroteBytes = true;

    DWORD ignored = 0;
    if (!VirtualProtect(site.target, site.patchSize, oldProtect, &ignored))
        return false;
    return true;
}

bool RestoreVehicleAnalogPatchSite(VehicleAnalogPatchSite& site)
{
    DWORD oldProtect = 0;
    if (!VirtualProtect(
            site.target,
            site.patchSize,
            PAGE_EXECUTE_READWRITE,
            &oldProtect))
    {
        return false;
    }

    std::memcpy(site.target, site.originalBytes, site.patchSize);
    FlushInstructionCache(GetCurrentProcess(), site.target, site.patchSize);

    DWORD ignored = 0;
    return VirtualProtect(
        site.target,
        site.patchSize,
        oldProtect,
        &ignored) != FALSE;
}

bool BuildVehicleAnalogStub(
    unsigned char* stub,
    size_t stubCapacity,
    uintptr_t useJoyAddress,
    uintptr_t returnAddress)
{
    if (stub == nullptr || stubCapacity < 96)
        return false;

    unsigned char* cursor = stub;

    // The replaced instruction is x87-only. Preserve EFLAGS around our two
    // gate checks so the surrounding original code observes identical flags.
    // pushfd shifts DP's locals: LT [esp+18h] -> [esp+1Ch],
    // RT [esp+10h] -> [esp+14h].
    EmitVehicle8(cursor, 0x9C); // pushfd

    // cmp byte ptr [USEJOY],0
    EmitVehicle8(cursor, 0x80); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(cursor, static_cast<std::uint32_t>(useJoyAddress));
    EmitVehicle8(cursor, 0x00);
    EmitVehicle8(cursor, 0x74); // je original
    unsigned char* jeUseJoy = cursor++;

    // cmp dword ptr [g_vehicleAnalogTriggersValid],0
    EmitVehicle8(cursor, 0x83); EmitVehicle8(cursor, 0x3D);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleAnalogTriggersValid)));
    EmitVehicle8(cursor, 0x00);
    EmitVehicle8(cursor, 0x74); // je original
    unsigned char* jeValid = cursor++;

    // fld [LT01] / fstp [original esp+18h]
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x05);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleLeftTrigger01)));
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x1C);

    // fld [RT01] / fstp [original esp+10h]
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x05);
    EmitVehicle32(
        cursor,
        static_cast<std::uint32_t>(
            reinterpret_cast<uintptr_t>(&g_vehicleRightTrigger01)));
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x14);

    unsigned char* originalPath = cursor;
    EmitVehicle8(cursor, 0x9D); // popfd

    // Original instruction replaced by the detour.
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x86);
    EmitVehicle32(cursor, 0x000013E4u); // fld dword ptr [esi+13E4h]

    EmitVehicleRel32(cursor, 0xE9, returnAddress);

    const std::intptr_t useJoyRel = originalPath - (jeUseJoy + 1);
    const std::intptr_t validRel = originalPath - (jeValid + 1);
    if (useJoyRel < -128 || useJoyRel > 127 ||
        validRel < -128 || validRel > 127)
    {
        return false;
    }

    *jeUseJoy = static_cast<unsigned char>(static_cast<std::int8_t>(useJoyRel));
    *jeValid = static_cast<unsigned char>(static_cast<std::int8_t>(validRel));
    return static_cast<size_t>(cursor - stub) <= stubCapacity;
}

bool InstallVehicleAnalogTriggerPatch(const DpBuildProfile& build)
{
    VehicleAnalogPatchSite sites[3]{};

    // Preparation phase: validate every signature, snapshot every original
    // byte sequence, allocate/build every stub, and make every stub executable.
    // Nothing in DP.exe is modified until all three consumers are ready.
    if (!PreparePrimaryVehicleAnalogPatch(build, sites[0]) ||
        !PrepareVehicleAnalogCut(
            build,
            kVehicleLowLevelAnalogCutOffsetFromInject,
            kVehicleLowLevelAnalogResumeOffset,
            BuildVehicleLowLevelAnalogCutStub,
            "vehicle low-level update",
            sites[1]) ||
        !PrepareVehicleAnalogCut(
            build,
            kVehicleHighLevelAnalogCutOffsetFromInject,
            kVehicleHighLevelAnalogResumeOffset,
            BuildVehicleHighLevelAnalogCutStub,
            "high-level car update",
            sites[2]))
    {
        for (auto& site : sites)
            ReleaseVehicleAnalogPatchSite(site);
        g_vehicleAnalogPatchInstalled.store(false, std::memory_order_release);
        AppendLog(
            "[Input][AnalogVehicle] Three-consumer LT/RT transaction not "
            "prepared; DP.exe left unmodified.\n");
        return false;
    }

    size_t committed = 0;
    bool failingSiteWasWritten = false;
    for (; committed < 3; ++committed)
    {
        bool wroteBytes = false;
        if (!WriteVehicleAnalogPatchSite(sites[committed], wroteBytes))
        {
            failingSiteWasWritten = wroteBytes;
            break;
        }
    }

    if (committed != 3)
    {
        bool rollbackOk = true;
        bool restored[3] = { false, false, false };
        const size_t rollbackCount =
            committed + (failingSiteWasWritten ? 1u : 0u);
        for (size_t i = rollbackCount; i > 0; --i)
        {
            restored[i - 1] = RestoreVehicleAnalogPatchSite(sites[i - 1]);
            rollbackOk &= restored[i - 1];
        }

        // Free only stubs that can no longer be reached from DP.exe. If a
        // rollback failed, deliberately retain that stub so any surviving JMP
        // still has a valid destination instead of becoming use-after-free.
        for (size_t i = 0; i < 3; ++i)
        {
            const bool siteWasWritten = i < rollbackCount;
            if (!siteWasWritten || restored[i])
                ReleaseVehicleAnalogPatchSite(sites[i]);
        }

        g_vehicleAnalogPatchInstalled.store(false, std::memory_order_release);
        AppendLog(
            rollbackOk
                ? "[Input][AnalogVehicle] Commit failed; all written LT/RT "
                  "consumer patches rolled back.\n"
                : "[Input][AnalogVehicle] ERROR: commit failed and rollback "
                  "could not fully restore all LT/RT consumer sites.\n");
        return false;
    }

    // Stubs are intentionally retained for the lifetime of the process; every
    // committed JMP targets them. Runtime On/Off is controlled by the shared
    // validity/enable state, not by rewriting executable code again.
    g_vehicleAnalogPatchInstalled.store(true, std::memory_order_release);
    AppendLog(
        "[Input][AnalogVehicle] Three-consumer LT/RT transaction committed.\n");
    return true;
}

bool InitializeXInput()
{
    if (g_xinputGetState != nullptr)
        return true;

    static const wchar_t* kCandidates[] = {
        L"xinput1_4.dll",
        L"xinput1_3.dll",
        L"xinput9_1_0.dll"
    };

    for (const wchar_t* candidate : kCandidates)
    {
        HMODULE module = LoadLibraryW(candidate);
        if (module == nullptr)
            continue;

        auto* proc = reinterpret_cast<XInputGetStateFn>(
            GetProcAddress(module, "XInputGetState"));
        if (proc == nullptr)
        {
            FreeLibrary(module);
            continue;
        }

        g_xinputModule = module;
        g_xinputGetState = proc;
        g_xinputSetState = reinterpret_cast<XInputSetStateFn>(
            GetProcAddress(module, "XInputSetState"));

        char modulePath[MAX_PATH] = {};
        if (GetModuleFileNameA(module, modulePath, static_cast<DWORD>(sizeof(modulePath))) != 0)
        {
            char text[MAX_PATH + 96] = {};
            sprintf_s(text, "[Input][XInput] XInputGetState provider: %s\n", modulePath);
            AppendLog(text);
        }
        else
        {
            AppendLog("[Input][XInput] XInputGetState loaded.\n");
        }

        if (g_xinputSetState == nullptr)
        {
            AppendLog(
                "[Input][Vibration] WARNING: XInputSetState is unavailable; "
                "native rumble restoration will stay disabled.\n");
        }
        return true;
    }

    AppendLog("[Input][XInput] ERROR: No usable XInputGetState provider found.\n");
    return false;
}

MMRESULT FillJoyInfoFromXInput(UINT joyId, LPJOYINFOEX info)
{
    if (info == nullptr || info->dwSize < sizeof(JOYINFOEX))
        return MMSYSERR_INVALPARAM;

    if (g_xinputGetState == nullptr || joyId >= XUSER_MAX_COUNT)
        return JOYERR_UNPLUGGED;

    XINPUT_STATE state = {};
    if (g_xinputGetState(joyId, &state) != ERROR_SUCCESS)
    {
        if (joyId < XUSER_MAX_COUNT)
        {
            g_xboxLeftStickX[joyId].store(0.0f, std::memory_order_relaxed);
            g_xboxLeftStickY[joyId].store(0.0f, std::memory_order_relaxed);
            g_xboxRightStickX[joyId].store(0.0f, std::memory_order_relaxed);
            g_xboxRightStickY[joyId].store(0.0f, std::memory_order_relaxed);
            g_xboxLeftStickValid[joyId].store(false, std::memory_order_release);
        }
        if (g_activeXInputUserValid.load(std::memory_order_acquire) &&
            g_activeXInputUser.load(std::memory_order_relaxed) == joyId)
        {
            g_activeXInputUserValid.store(false, std::memory_order_release);
            g_vehicleLeftTrigger01 = 0.0f;
            g_vehicleRightTrigger01 = 0.0f;
            InterlockedExchange(&g_vehicleAnalogTriggersValid, 0);
            StopXInputVibration(joyId, "controller disconnect");
        }
        return JOYERR_UNPLUGGED;
    }

    const bool hadActiveUser =
        g_activeXInputUserValid.load(std::memory_order_acquire);
    const DWORD previousUser =
        g_activeXInputUser.load(std::memory_order_relaxed);
    if (!hadActiveUser || previousUser != joyId)
    {
        if (hadActiveUser)
        {
            g_activeXInputUserValid.store(false, std::memory_order_release);
            StopXInputVibration(previousUser, "controller switch");
        }

        g_activeXInputUser.store(joyId, std::memory_order_relaxed);
        g_activeXInputUserValid.store(true, std::memory_order_release);
        {
            std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
            g_lastMotorStateValid = false;
        }
        RefreshXInputVibrationFromNativeState();
    }

    const XINPUT_GAMEPAD& pad = state.Gamepad;

    if (joyId < XUSER_MAX_COUNT)
    {
        g_xboxLeftStickX[joyId].store(
            NormalizeXboxStickAxis(pad.sThumbLX),
            std::memory_order_relaxed);
        g_xboxLeftStickY[joyId].store(
            NormalizeXboxStickAxis(pad.sThumbLY),
            std::memory_order_relaxed);
        g_xboxRightStickX[joyId].store(
            NormalizeXboxStickAxis(pad.sThumbRX),
            std::memory_order_relaxed);
        g_xboxRightStickY[joyId].store(
            NormalizeXboxStickAxis(pad.sThumbRY),
            std::memory_order_relaxed);
        g_xboxLeftStickValid[joyId].store(true, std::memory_order_release);
    }

    // Match the Xbox 360 input backend's semantics, with a user-tunable raw
    // threshold. The original Xbox build uses 30. Values above the threshold
    // remain raw/255; the surviving range is deliberately not renormalized.
    // These floats feed all three restored Xbox car-control consumers.
    const BYTE vehicleTriggerDeadzone = static_cast<BYTE>(
        g_vehicleTriggerDeadzoneRaw.load(std::memory_order_acquire));
    const float rawVehicleLt = pad.bLeftTrigger > vehicleTriggerDeadzone
        ? static_cast<float>(pad.bLeftTrigger) * kXboxTriggerScale
        : 0.0f;
    const float rawVehicleRt = pad.bRightTrigger > vehicleTriggerDeadzone
        ? static_cast<float>(pad.bRightTrigger) * kXboxTriggerScale
        : 0.0f;

    if (g_analogVehicleTriggersEnabled.load(std::memory_order_acquire) &&
        g_vehicleAnalogPatchInstalled.load(std::memory_order_acquire))
    {
        g_vehicleLeftTrigger01 = rawVehicleLt;
        g_vehicleRightTrigger01 = rawVehicleRt;
        InterlockedExchange(&g_vehicleAnalogTriggersValid, 1);
    }
    else
    {
        g_vehicleLeftTrigger01 = 0.0f;
        g_vehicleRightTrigger01 = 0.0f;
        InterlockedExchange(&g_vehicleAnalogTriggersValid, 0);
    }

    const DWORD mappedButtons = MapXInputButtons(pad.wButtons);

    // Synthetic compatibility view consumed by DP:
    //   X/Y = left stick, Z/R = right stick, U = LT, V = RT.
    const GamepadInputProfile inputProfile =
        g_gamepadInputProfile.load(std::memory_order_acquire);
    info->dwXpos = SignedAxisToWinMM(pad.sThumbLX, false);
    info->dwYpos = SignedAxisToWinMM(pad.sThumbLY, true);
    info->dwZpos = SignedAxisToWinMM(pad.sThumbRX, false);
    info->dwRpos = SignedAxisToWinMM(pad.sThumbRY, true);
    info->dwUpos = TriggerToPositiveAxis(pad.bLeftTrigger, inputProfile);
    info->dwVpos = TriggerToPositiveAxis(pad.bRightTrigger, inputProfile);
    info->dwButtons = mappedButtons;
    info->dwButtonNumber = FirstPressedButtonNumber(mappedButtons);
    info->dwPOV = MapXInputPov(pad.wButtons);
    info->dwReserved1 = 0;
    info->dwReserved2 = 0;
    return JOYERR_NOERROR;
}

MMRESULT WINAPI HookJoyGetPosEx(UINT joyId, LPJOYINFOEX info)
{
    return FillJoyInfoFromXInput(joyId, info);
}

bool InstallJoyGetPosExBridge()
{
    HMODULE winmm = GetModuleHandleW(L"winmm.dll");
    if (winmm == nullptr)
        winmm = LoadLibraryW(L"winmm.dll");

    if (winmm == nullptr)
    {
        AppendLog("[Input][XInput] ERROR: winmm.dll unavailable; native bridge disabled.\n");
        return false;
    }

    FARPROC proc = GetProcAddress(winmm, "joyGetPosEx");
    if (proc == nullptr)
    {
        AppendLog("[Input][XInput] ERROR: joyGetPosEx export not found; native bridge disabled.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        reinterpret_cast<void*>(proc),
        reinterpret_cast<void*>(&HookJoyGetPosEx),
        reinterpret_cast<void**>(&g_originalJoyGetPosEx));

    if (createStatus != MH_OK &&
        !(createStatus == MH_ERROR_ALREADY_CREATED &&
          g_originalJoyGetPosEx != nullptr))
    {
        char text[160] = {};
        sprintf_s(
            text,
            "[Input][XInput] ERROR: MH_CreateHook(joyGetPosEx) failed: %d.\n",
            static_cast<int>(createStatus));
        AppendLog(text);
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(reinterpret_cast<void*>(proc));
    if (enableStatus != MH_OK && enableStatus != MH_ERROR_ENABLED)
    {
        if (createStatus == MH_OK)
            MH_RemoveHook(reinterpret_cast<void*>(proc));
        char text[160] = {};
        sprintf_s(
            text,
            "[Input][XInput] ERROR: MH_EnableHook(joyGetPosEx) failed: %d.\n",
            static_cast<int>(enableStatus));
        AppendLog(text);
        return false;
    }

    AppendLog(
        "[Input][XInput] Native bridge active: "
        "LeftStick=X/Y, RightStick=Z/R, LT=U(high), RT=V(high).\n");
    return true;
}

bool InstallNativeVibrationBridge(const DpBuildProfile& build)
{
    if (g_xinputSetState == nullptr)
        return false;

    auto* commandTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.input.rdInputSetActuatorRva);
    auto* stateTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.input.inputActuatorSetSecondRva);

    if (std::memcmp(
            commandTarget,
            kExpectedRdInputSetActuatorBytes,
            sizeof(kExpectedRdInputSetActuatorBytes)) != 0 ||
        std::memcmp(
            stateTarget,
            kExpectedInputActuatorSetSecondBytes,
            sizeof(kExpectedInputActuatorSetSecondBytes)) != 0)
    {
        AppendLog(
            "[Input][Vibration] ERROR: Native actuator signature mismatch; "
            "rumble restoration disabled.\n");
        return false;
    }

    MH_STATUS status = MH_CreateHook(
        commandTarget,
        reinterpret_cast<void*>(&HookRdInputSetActuator),
        reinterpret_cast<void**>(&g_originalRdInputSetActuator));
    if (status != MH_OK)
    {
        AppendLog(
            "[Input][Vibration] ERROR: Failed to hook CRdInput::SetActuator.\n");
        return false;
    }

    status = MH_CreateHook(
        stateTarget,
        reinterpret_cast<void*>(&HookInputActuatorSetSecond),
        reinterpret_cast<void**>(&g_originalInputActuatorSetSecond));
    if (status != MH_OK)
    {
        MH_RemoveHook(commandTarget);
        AppendLog(
            "[Input][Vibration] ERROR: Failed to hook CInput_Actuator output.\n");
        return false;
    }

    status = MH_EnableHook(commandTarget);
    if (status != MH_OK)
    {
        MH_RemoveHook(stateTarget);
        MH_RemoveHook(commandTarget);
        AppendLog(
            "[Input][Vibration] ERROR: Failed to enable CRdInput actuator hook.\n");
        return false;
    }

    status = MH_EnableHook(stateTarget);
    if (status != MH_OK)
    {
        MH_DisableHook(commandTarget);
        MH_RemoveHook(stateTarget);
        MH_RemoveHook(commandTarget);
        AppendLog(
            "[Input][Vibration] ERROR: Failed to enable CInput_Actuator output hook.\n");
        return false;
    }

    g_vibrationEnabled.store(g_config.vibrationEnabled, std::memory_order_release);
    g_vibrationStrength.store(g_config.vibrationStrength, std::memory_order_release);
    g_nativeVibrationInstalled.store(true, std::memory_order_release);

    char text[320] = {};
    sprintf_s(
        text,
        "[Input][Vibration] Native rumble restored: enabled=%s strength=%.2f "
        "exact16=true (CRdInput=DP.exe+0x%08lX, "
        "CInput_Actuator=DP.exe+0x%08lX).\n",
        g_config.vibrationEnabled ? "true" : "false",
        static_cast<double>(g_config.vibrationStrength),
        static_cast<unsigned long>(build.input.rdInputSetActuatorRva),
        static_cast<unsigned long>(build.input.inputActuatorSetSecondRva));
    AppendLog(text);
    return true;
}

int __cdecl HookControllerBindingEvaluator(
    DWORD size,
    DWORD flags,
    DWORD x,
    DWORD y,
    DWORD z,
    DWORD r,
    DWORD u,
    DWORD v,
    DWORD buttons,
    DWORD buttonNumber,
    DWORD pov,
    DWORD reserved1,
    DWORD reserved2,
    int binding)
{
    // Translate DP's vanilla axis meanings onto the synthetic XInput-backed
    // JOYINFOEX layout. Buttons and POV remain untouched.
    switch (binding)
    {
    case kAxisZLow:  binding = kAxisVHigh; break; // vanilla RT -> physical RT
    case kAxisZHigh: binding = kAxisUHigh; break; // vanilla LT -> physical LT
    case kAxisULow:  binding = kAxisZLow;  break; // right stick left
    case kAxisUHigh: binding = kAxisZHigh; break; // right stick right
    default: break;
    }

    return g_originalControllerBindingEvaluator(
        size, flags, x, y, z, r, u, v, buttons, buttonNumber, pov,
        reserved1, reserved2, binding);
}

} // namespace

bool IsAnalogVehicleTriggerPatchAvailable()
{
    return g_vehicleAnalogPatchInstalled.load(std::memory_order_acquire);
}

bool IsNativeVibrationAvailable()
{
    return g_nativeVibrationInstalled.load(std::memory_order_acquire) &&
           g_xinputSetState != nullptr;
}

bool RunNativeVibrationTestPulse()
{
    if (!IsNativeVibrationAvailable() ||
        !g_activeXInputUserValid.load(std::memory_order_acquire))
    {
        AppendLog("[Input][VibrationTest] Test pulse unavailable: no active XInput user.\n");
        return false;
    }

    const DWORD userIndex =
        g_activeXInputUser.load(std::memory_order_relaxed);
    if (userIndex >= XUSER_MAX_COUNT)
        return false;

    XINPUT_VIBRATION on = {};
    on.wLeftMotorSpeed = 65535;
    on.wRightMotorSpeed = 65535;

    DWORD onResult = ERROR_DEVICE_NOT_CONNECTED;
    DWORD restoreResult = ERROR_DEVICE_NOT_CONNECTED;
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        onResult = g_xinputSetState(userIndex, &on);

        char text[256] = {};
        sprintf_s(
            text,
            "[Input][VibrationTest] ON user=%lu left=65535 right=65535 result=%lu; holding 750 ms.\n",
            static_cast<unsigned long>(userIndex),
            static_cast<unsigned long>(onResult));
        AppendLog(text);

        if (onResult == ERROR_SUCCESS)
            Sleep(750);

        WORD restoreLeft = 0;
        WORD restoreRight = 0;
        if (g_vibrationEnabled.load(std::memory_order_acquire))
        {
            const float strength =
                g_vibrationStrength.load(std::memory_order_acquire);
            const uint32_t motorState =
                g_currentNativeMotorState.load(std::memory_order_acquire);
            restoreLeft = ApplyVibrationStrength(
                static_cast<WORD>(motorState & 0xFFFFu), strength);
            restoreRight = ApplyVibrationStrength(
                static_cast<WORD>(motorState >> 16), strength);
        }

        XINPUT_VIBRATION restore = {};
        restore.wLeftMotorSpeed = restoreLeft;
        restore.wRightMotorSpeed = restoreRight;
        restoreResult = g_xinputSetState(userIndex, &restore);
        if (restoreResult == ERROR_SUCCESS)
        {
            g_lastMotorLeft = restoreLeft;
            g_lastMotorRight = restoreRight;
            g_lastMotorStateValid = true;
        }

        sprintf_s(
            text,
            "[Input][VibrationTest] RESTORE user=%lu left=%u right=%u result=%lu.\n",
            static_cast<unsigned long>(userIndex),
            static_cast<unsigned int>(restoreLeft),
            static_cast<unsigned int>(restoreRight),
            static_cast<unsigned long>(restoreResult));
        AppendLog(text);
    }

    return onResult == ERROR_SUCCESS && restoreResult == ERROR_SUCCESS;
}

void NotifyNativeVibrationInputModeChanged(bool controller)
{
    if (!IsNativeVibrationAvailable())
        return;

    if (!controller)
    {
        if (!g_activeXInputUserValid.load(std::memory_order_acquire))
            return;

        const DWORD userIndex =
            g_activeXInputUser.load(std::memory_order_relaxed);
        StopXInputVibration(userIndex, "keyboard/mouse mode");
        return;
    }

    // The last hardware state may be a forced zero from keyboard/mouse mode.
    // Invalidate the cache so returning to controller mode can replay DP's
    // current native actuator state even when the values themselves did not
    // change while keyboard/mouse owned USEJOY.
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        g_lastMotorStateValid = false;
    }
    RefreshXInputVibrationFromNativeState();
}

bool ApplyNativeVibrationSettings(bool enabled, float strength)
{
    if (strength < 0.0f)
        strength = 0.0f;
    else if (strength > 1.0f)
        strength = 1.0f;

    g_config.vibrationEnabled = enabled;
    g_config.vibrationStrength = strength;
    g_vibrationEnabled.store(enabled, std::memory_order_release);
    g_vibrationStrength.store(strength, std::memory_order_release);

    if (!IsNativeVibrationAvailable())
        return false;

    RefreshXInputVibrationFromNativeState();
    return true;
}

void ApplyGamepadInputProfile(GamepadInputProfile profile)
{
    g_config.gamepadInputProfile = profile;
    g_gamepadInputProfile.store(profile, std::memory_order_release);
}

void ApplyAnalogVehicleTriggers(bool enabled)
{
    g_config.analogVehicleTriggers = enabled;
    g_analogVehicleTriggersEnabled.store(enabled, std::memory_order_release);

    if (!enabled)
    {
        g_vehicleLeftTrigger01 = 0.0f;
        g_vehicleRightTrigger01 = 0.0f;
        InterlockedExchange(&g_vehicleAnalogTriggersValid, 0);
    }
}

void ApplyVehicleTriggerDeadzone(UINT deadzone)
{
    if (deadzone > 254u)
        deadzone = 254u;

    g_config.vehicleTriggerDeadzone = deadzone;
    g_vehicleTriggerDeadzoneRaw.store(deadzone, std::memory_order_release);
}



XboxCombatStrafeInput PollXboxCombatStrafeInput()
{
    constexpr WORD kShoulderMask =
        XINPUT_GAMEPAD_LEFT_SHOULDER | XINPUT_GAMEPAD_RIGHT_SHOULDER;

    if (!g_nativeXInputBackendInstalled.load(std::memory_order_acquire) ||
        g_xinputGetState == nullptr ||
        !g_activeXInputUserValid.load(std::memory_order_acquire))
    {
        ResetXboxCombatStrafeInput();
        return XboxCombatStrafeInput::None;
    }

    const DWORD user = g_activeXInputUser.load(std::memory_order_relaxed);
    XINPUT_STATE state = {};
    if (g_xinputGetState(user, &state) != ERROR_SUCCESS)
    {
        ResetXboxCombatStrafeInput();
        return XboxCombatStrafeInput::None;
    }

    const WORD current = state.Gamepad.wButtons & kShoulderMask;
    WORD rising = 0;
    {
        std::lock_guard<std::mutex> lock(g_combatStrafeInputMutex);
        if (!g_combatStrafeInputInitialized || g_combatStrafeInputUser != user)
        {
            g_combatStrafeInputInitialized = true;
            g_combatStrafeInputUser = user;
            g_combatStrafePreviousShoulders = current;
            return XboxCombatStrafeInput::None;
        }

        rising = current & static_cast<WORD>(~g_combatStrafePreviousShoulders);
        g_combatStrafePreviousShoulders = current;
    }

    // Keep the baseline fresh even when controller-specific behavior is not
    // currently eligible. This prevents a held shoulder from becoming a
    // synthetic edge when AutoSwitch or the profile later changes.
    bool controllerMode = false;
    if (!TryGetVanillaInputMode(controllerMode) || !controllerMode ||
        g_gamepadInputProfile.load(std::memory_order_acquire) !=
            GamepadInputProfile::Xbox360)
    {
        return XboxCombatStrafeInput::None;
    }

    const bool leftHeld =
        (current & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
    const bool rightHeld =
        (current & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;

    if ((rising & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0 && !rightHeld)
        return XboxCombatStrafeInput::Left;
    if ((rising & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0 && !leftHeld)
        return XboxCombatStrafeInput::Right;

    return XboxCombatStrafeInput::None;
}

void ResetXboxCombatStrafeInput()
{
    std::lock_guard<std::mutex> lock(g_combatStrafeInputMutex);
    g_combatStrafeInputInitialized = false;
    g_combatStrafeInputUser = 0;
    g_combatStrafePreviousShoulders = 0;
}

bool InstallNativeXInputBackend()
{
    g_nativeXInputBackendInstalled.store(false, std::memory_order_release);

    if (!g_config.nativeXInputEnabled)
        return true;

    g_gamepadInputProfile.store(
        g_config.gamepadInputProfile,
        std::memory_order_release);
    g_analogVehicleTriggersEnabled.store(
        g_config.analogVehicleTriggers,
        std::memory_order_release);
    g_vehicleTriggerDeadzoneRaw.store(
        g_config.vehicleTriggerDeadzone,
        std::memory_order_release);

    if (!InitializeXInput())
    {
        AppendLog("[Input][XInput] ERROR: Native backend requested but XInput initialization failed.\n");
        return false;
    }

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Input][XInput] ERROR: DP.exe info unavailable; native backend disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[Input][XInput] ERROR: Unsupported DP.exe build; native backend disabled.\n");
        return false;
    }

    auto* evaluatorTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->input.controllerBindingEvaluatorRva);

    if (std::memcmp(
            evaluatorTarget,
            kExpectedEvaluatorBytes,
            sizeof(kExpectedEvaluatorBytes)) != 0)
    {
        AppendLog(
            "[Input][XInput] ERROR: Controller binding evaluator signature mismatch; "
            "native backend disabled.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        evaluatorTarget,
        reinterpret_cast<void*>(&HookControllerBindingEvaluator),
        reinterpret_cast<void**>(&g_originalControllerBindingEvaluator));

    if (createStatus != MH_OK)
    {
        AppendLog("[Input][XInput] ERROR: MH_CreateHook failed for controller binding evaluator.\n");
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(evaluatorTarget);
    if (enableStatus != MH_OK)
    {
        MH_RemoveHook(evaluatorTarget);
        g_originalControllerBindingEvaluator = nullptr;
        AppendLog("[Input][XInput] ERROR: MH_EnableHook failed for controller binding evaluator.\n");
        return false;
    }

    if (!InstallJoyGetPosExBridge())
    {
        MH_DisableHook(evaluatorTarget);
        MH_RemoveHook(evaluatorTarget);
        g_originalControllerBindingEvaluator = nullptr;
        AppendLog(
            "[Input][XInput] ERROR: joyGetPosEx bridge failed; "
            "controller evaluator hook disabled.\n");
        return false;
    }

    // Keep the profile hooks installed for the session. Runtime atomics choose
    // PC passthrough vs the proven Xbox 360 stick/aim behavior on each call.
    const bool stickProfileInstalled = InstallGamepadStickProfileHook(*build);
    if (stickProfileInstalled)
    {
        InstallRightStickAimProfileHook(*build);
    }
    else
    {
        AppendLog(
            "[Input] Xbox 360 aim shaping skipped because exact stick "
            "restoration is unavailable.\n");
    }

    // Keep the three proven Xbox vehicle trigger consumers installed. When
    // disabled, the shared valid flag is cleared and every detour falls back
    // to Director's Cut's original digital path.
    InstallVehicleAnalogTriggerPatch(*build);

    // Rumble restoration is non-fatal for controller input. Unsupported or
    // mismatched actuator code leaves input working normally without motors.
    InstallNativeVibrationBridge(*build);

    char readyText[320] = {};
    sprintf_s(
        readyText,
        "[Input] NativeXInput=true Profile=%s AnalogVehicleTriggers=%s "
        "VehicleTriggerDeadzone=%u Vibration=%s "
        "(controller evaluator DP.exe+0x%08lX).\n",
        g_config.gamepadInputProfile == GamepadInputProfile::Xbox360
            ? "Xbox360"
            : "PC",
        g_config.analogVehicleTriggers ? "true" : "false",
        g_config.vehicleTriggerDeadzone,
        g_config.vibrationEnabled ? "true" : "false",
        static_cast<unsigned long>(build->input.controllerBindingEvaluatorRva));
    AppendLog(readyText);
    g_nativeXInputBackendInstalled.store(true, std::memory_order_release);
    return true;
}

bool IsNativeXInputBackendAvailable()
{
    return g_nativeXInputBackendInstalled.load(std::memory_order_acquire);
}
