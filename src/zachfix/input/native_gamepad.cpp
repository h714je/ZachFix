#include "zachfix/input/native_gamepad.h"

#include "zachfix/core/config.h"
#include "zachfix/input/gamepad_backend.h"
#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"
#include "zachfix/input/input_mode.h"

#include <Windows.h>
#include <intrin.h>
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
// int __cdecl EvaluateControllerBinding(legacy controller fields..., int binding)
//
// Every controller action reaches this evaluator. The production path keeps the hook
// as a narrow dispatch point: native GamepadState is evaluated directly while
// the original function remains authoritative outside the native record rebuild.
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
std::atomic_bool g_directInputRecordObserved{ false };
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

bool RemoveInputHookSafely(void* target, void** original, const char* name)
{
    const MH_STATUS removeStatus = MH_RemoveHook(target);
    if (removeStatus == MH_OK || removeStatus == MH_ERROR_NOT_CREATED)
    {
        if (original != nullptr)
            *original = nullptr;
        return true;
    }

    char text[256] = {};
    sprintf_s(
        text,
        "[Input] ERROR: rollback could not remove %s hook (%d); trampoline retained for safety.\n",
        name,
        static_cast<int>(removeStatus));
    AppendLog(text);
    return false;
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
        RemoveInputHookSafely(
            target,
            reinterpret_cast<void**>(&g_originalStickFloatGetter),
            "stick float getter");
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

std::atomic<float> g_xboxLeftStickX[kMaxGamepads]{};
std::atomic<float> g_xboxLeftStickY[kMaxGamepads]{};
std::atomic<float> g_xboxRightStickX[kMaxGamepads]{};
std::atomic<float> g_xboxRightStickY[kMaxGamepads]{};
std::atomic_bool g_xboxLeftStickValid[kMaxGamepads]{};

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

// DP evaluator binding values 0x29..0x38. The configJ binding IDs retain
// Director's Cut's legacy controller vocabulary, but native gamepad input no
// longer materializes a JOYINFOEX. The direct evaluator below maps those IDs
// straight onto canonical GamepadState controls.
constexpr int kAxisXLow = 0x2D;
constexpr int kAxisXHigh = 0x2E;
constexpr int kAxisYLow = 0x2F;
constexpr int kAxisYHigh = 0x30;
constexpr int kAxisZLow = 0x31;   // vanilla RT
constexpr int kAxisZHigh = 0x32;  // vanilla LT
constexpr int kAxisULow = 0x33;   // vanilla right-stick left
constexpr int kAxisUHigh = 0x34;  // vanilla right-stick right
constexpr int kAxisVLow = 0x35;
constexpr int kAxisVHigh = 0x36;
constexpr int kAxisRLow = 0x37;   // vanilla right-stick up
constexpr int kAxisRHigh = 0x38;  // vanilla right-stick down

DWORD LegacySignedAxisValue(SHORT value, bool invert)
{
    // Preserve the exact integer coordinates DP's WinMM evaluator historically
    // saw, but keep them as a local math convention only. No JOYINFOEX object
    // or WinMM acquisition participates in native gamepad input anymore.
    const int converted = invert
        ? 32767 - static_cast<int>(value)
        : 32768 + static_cast<int>(value);

    if (converted <= 0)
        return 0;
    if (converted >= 65535)
        return 65535;
    return static_cast<DWORD>(converted);
}

DWORD LegacyTriggerAxisValue(BYTE value, GamepadInputProfile profile)
{
    constexpr DWORD kNeutral = 32767;
    if (profile == GamepadInputProfile::Xbox360)
        return value > kXboxTriggerDeadzoneRaw ? 65535u : kNeutral;

    constexpr DWORD kRange = 65535 - kNeutral;
    return kNeutral + (static_cast<DWORD>(value) * kRange) / 255u;
}

DWORD LegacyPovValue(std::uint32_t buttons)
{
    const bool up = (buttons & GamepadButton_DpadUp) != 0;
    const bool down = (buttons & GamepadButton_DpadDown) != 0;
    const bool left = (buttons & GamepadButton_DpadLeft) != 0;
    const bool right = (buttons & GamepadButton_DpadRight) != 0;

    if (up && !down)
    {
        if (right && !left) return 4500u;
        if (left && !right) return 31500u;
        return 0u;
    }
    if (down && !up)
    {
        if (right && !left) return 13500u;
        if (left && !right) return 22500u;
        return 18000u;
    }
    if (right && !left)
        return 9000u;
    if (left && !right)
        return 27000u;
    return 0xFFFFu;
}

int LegacyAxisMagnitude(DWORD value)
{
    // FUN_006B1780 computes ((value / 65535) * 2 - 1) * 128 and then
    // truncates toward zero through the MSVC _ftol helper. Use the exact
    // equivalent rational form so this stays deterministic and does not
    // depend on the host x87/SSE precision mode.
    const std::int64_t numerator =
        static_cast<std::int64_t>(value) * 256 -
        static_cast<std::int64_t>(128) * 65535;
    return static_cast<int>(numerator / 65535);
}

int EvaluateCanonicalGamepadBinding(const GamepadState& pad, int binding)
{
    // This is the production replacement for the synthetic JOYINFOEX path.
    // configJ still supplies the same binding IDs; only the physical source is
    // different. Return values intentionally match FUN_006B1780, including
    // signed analog magnitudes for axis bindings.
    const auto axis = [](DWORD value, bool high) -> int
    {
        if (high)
            return value >= 45875u ? LegacyAxisMagnitude(value) : 0;
        return value <= 19661u ? LegacyAxisMagnitude(value) : 0;
    };

    const DWORD pov = LegacyPovValue(pad.buttons);

    switch (binding)
    {
    case 0x29:
        return (pov <= 4500u || (pov >= 31500u && pov <= 35900u)) ? 1 : 0;
    case 0x2A:
        return (pov >= 13500u && pov <= 22500u) ? 1 : 0;
    case 0x2B:
        return (pov >= 22500u && pov <= 31500u) ? 1 : 0;
    case 0x2C:
        return (pov >= 4500u && pov <= 13500u) ? 1 : 0;

    case kAxisXLow:
        return axis(LegacySignedAxisValue(pad.leftX, false), false);
    case kAxisXHigh:
        return axis(LegacySignedAxisValue(pad.leftX, false), true);
    case kAxisYLow:
        return axis(LegacySignedAxisValue(pad.leftY, true), false);
    case kAxisYHigh:
        return axis(LegacySignedAxisValue(pad.leftY, true), true);

    // DPLauncher's vanilla shared-Z meanings are preserved at the binding
    // level even though the canonical provider exposes independent triggers.
    case kAxisZLow:
        return axis(
            LegacyTriggerAxisValue(
                pad.rightTrigger,
                g_gamepadInputProfile.load(std::memory_order_acquire)),
            true);
    case kAxisZHigh:
        return axis(
            LegacyTriggerAxisValue(
                pad.leftTrigger,
                g_gamepadInputProfile.load(std::memory_order_acquire)),
            true);

    case kAxisULow:
        return axis(LegacySignedAxisValue(pad.rightX, false), false);
    case kAxisUHigh:
        return axis(LegacySignedAxisValue(pad.rightX, false), true);

    // The old synthetic layout placed RT on V-high. Preserve the otherwise
    // unused V-low/high compatibility meanings for manually edited configJ
    // files without routing gameplay through a JOYINFOEX.
    case kAxisVLow:
        return axis(
            LegacyTriggerAxisValue(
                pad.rightTrigger,
                g_gamepadInputProfile.load(std::memory_order_acquire)),
            false);
    case kAxisVHigh:
        return axis(
            LegacyTriggerAxisValue(
                pad.rightTrigger,
                g_gamepadInputProfile.load(std::memory_order_acquire)),
            true);

    case kAxisRLow:
        return axis(LegacySignedAxisValue(pad.rightY, true), false);
    case kAxisRHigh:
        return axis(LegacySignedAxisValue(pad.rightY, true), true);

    default:
        // Canonical bits 0..9 deliberately retain DP's WinMM button numbering.
        // D-pad lives above that range and remains a POV-only domain.
        constexpr std::uint32_t kLegacyButtonMask = (1u << 10) - 1u;
        return static_cast<int>(
            (1u << (static_cast<unsigned>(binding) & 0x1Fu)) &
            (pad.buttons & kLegacyButtonMask));
    }
}

// The native controller action helpers do not pass the input slot into
// FUN_006B1780. The direct path therefore scopes the canonical sample around one
// helper-record rebuild on the same input thread. Outside this scope the hook
// is a transparent pass-through, preserving keyboard/mouse and legacy WinMM.
thread_local const GamepadState* g_directBindingGamepad = nullptr;

using ControllerActionHelperFn = int (__thiscall*)(void* inputState, unsigned char slot);

struct ControllerActionField
{
    std::ptrdiff_t helperOffset;
    std::size_t outputOffset;
};

// FUN_00709C40 / FUN_00709BA0 writes one 0x6C logical-action record through
// these native helpers. Steam and GOG use the same relative helper layout;
// only the first-helper RVA differs by build. Keeping the game's own helpers
// means configJ, arrow-key fallbacks, mouse-derived auxiliary fields and any
// action-specific quirks stay native while the controller sample comes from
// GamepadState.
constexpr ControllerActionField kControllerActionFields[] = {
    {  0x000, 0x04 },
    {  0x1A0, 0x08 },
    {  0x270, 0x0C },
    {  0x0D0, 0x10 },
    { -0x0B0, 0x14 },
    { -0x230, 0x18 },
    {  0x340, 0x1C },
    {  0x3F0, 0x20 },
    { -0x190, 0x24 },
    { -0x120, 0x28 },
    {  0x4A0, 0x2C },
    {  0x550, 0x30 },
    {  0x620, 0x34 },
    {  0x6F0, 0x38 },
    {  0x900, 0x3C },
    {  0x9D0, 0x40 },
    {  0x7A0, 0x44 },
    {  0x850, 0x48 },
    {  0xAA0, 0x4C },
    {  0xAF0, 0x50 },
    {  0xB40, 0x54 },
    {  0xB40, 0x58 },
    {  0xB40, 0x5C },
    {  0xB40, 0x60 },
    {  0xBA0, 0x64 },
    {  0xE90, 0x68 }
};

constexpr unsigned char kExpectedControllerActionHelperBytes[] = {
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x56, 0x57,
    0x89, 0x4D, 0xF8, 0x0F, 0xB6, 0x45, 0x08, 0x6B,
    0xC0, 0x36, 0x03, 0x45, 0xF8
};

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
        inputSlot < 0 || inputSlot >= static_cast<int>(kMaxGamepads))
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
        RemoveInputHookSafely(
            target,
            reinterpret_cast<void**>(&g_originalStickAxisPostProcessor),
            "stick post-processor");
        AppendLog(
            "[Input] ERROR: failed to enable stick post-processor hook; "
            "Xbox 360 stick profile unavailable.\n");
        return false;
    }

    g_gamepadStickProfileHookInstalled.store(true, std::memory_order_release);

    return true;
}

std::atomic<DWORD> g_activeGamepadUser{ 0 };
std::atomic_bool g_activeGamepadUserValid{ false };
std::atomic<DWORD> g_lastGamepadSequence[kMaxGamepads]{};
std::atomic_bool g_lastGamepadSequenceValid[kMaxGamepads]{};

// Continuous Xbox 360 trigger state consumed by the three restored vehicle
// control sites. The detours remain installed, but g_vehicleAnalogTriggersValid
// is cleared whenever the live Analog Vehicle Triggers option is disabled so
// each site falls back to Director's Cut's original digital path.
alignas(4) volatile LONG g_vehicleAnalogTriggersValid = 0;
alignas(4) volatile float g_vehicleLeftTrigger01 = 0.0f;
alignas(4) volatile float g_vehicleRightTrigger01 = 0.0f;
std::atomic_bool g_nativeGamepadBackendInstalled{ false };
std::mutex g_combatStrafeInputMutex;
bool g_combatStrafeInputInitialized = false;
DWORD g_combatStrafeInputUser = 0;
std::uint32_t g_combatStrafePreviousShoulders = 0;
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
DWORD g_lastMotorUser = 0;
bool g_lastMotorStateValid = false;
// Native lifecycle authority. The public owner atomics remain for existing
// read-only consumers, but they never authorize hardware output on their own.
// Begin revokes readiness; the post-update selected-slot commit restores it.
bool g_ownerReady = false;
std::uint64_t g_ownerEpoch = 0;
std::uint64_t g_inputReconciliationSerial = 0;
bool g_nativeModeKnown = false;
bool g_nativeControllerMode = false;
bool g_haveDiagnosticSample = false;
bool g_diagnosticConnected[kMaxGamepads] = {};
DWORD g_gameplayOutputPendingStopMask = 0;
std::atomic_bool g_vibrationTestActive{ false };
std::atomic<DWORD> g_vibrationTestUser{ 0 };
std::atomic<ULONGLONG> g_vibrationTestDeadlineMs{ 0 };
std::uint64_t g_combatStrafeInputRevision = 0;
ULONGLONG g_vibrationFailureLastLogMs[kMaxGamepads] = {};
unsigned int g_vibrationFailureSuppressed[kMaxGamepads] = {};

constexpr size_t kNativeOutputLogCapacity = 8;
constexpr size_t kNativeOutputLogLineSize = 288;

struct NativeOutputLogBatch
{
    char lines[kNativeOutputLogCapacity][kNativeOutputLogLineSize] = {};
    size_t count = 0;
    bool overflow = false;
};

void QueueNativeOutputLog(NativeOutputLogBatch& batch, const char* text)
{
    if (text == nullptr)
        return;
    if (batch.count >= kNativeOutputLogCapacity)
    {
        batch.overflow = true;
        return;
    }

    strncpy_s(
        batch.lines[batch.count],
        kNativeOutputLogLineSize,
        text,
        _TRUNCATE);
    ++batch.count;
}

void FlushNativeOutputLogs(const NativeOutputLogBatch& batch)
{
    for (size_t i = 0; i < batch.count; ++i)
        AppendLog(batch.lines[i]);
    if (batch.overflow)
    {
        AppendLog(
            "[Input][Vibration] WARNING: native output diagnostic batch overflowed; one or more log records were omitted.\n");
    }
}

void QueueVibrationStopLog(
    NativeOutputLogBatch& batch,
    DWORD userIndex,
    const char* reason,
    bool result,
    bool attempted)
{
    char text[224] = {};
    const char* resultText = result
        ? "ok"
        : (attempted ? "failed" : "unavailable/not attempted");
    sprintf_s(
        text,
        "[Input][Vibration] stopped user=%lu reason=%s result=%s.\n",
        static_cast<unsigned long>(userIndex),
        reason != nullptr ? reason : "unknown",
        resultText);
    QueueNativeOutputLog(batch, text);
}

void QueueOrdinaryVibrationFailureLocked(
    NativeOutputLogBatch& batch,
    DWORD userIndex,
    WORD leftMotor,
    WORD rightMotor,
    bool attempted)
{
    if (userIndex >= kMaxGamepads)
        return;

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous = g_vibrationFailureLastLogMs[userIndex];
    if (previous != 0 && now - previous < 1000ull)
    {
        ++g_vibrationFailureSuppressed[userIndex];
        return;
    }

    if (g_vibrationFailureSuppressed[userIndex] != 0)
    {
        char summary[224] = {};
        sprintf_s(
            summary,
            "[Input][Vibration] user=%lu suppressed %u repeated ordinary output failure log(s).\n",
            static_cast<unsigned long>(userIndex),
            g_vibrationFailureSuppressed[userIndex]);
        QueueNativeOutputLog(batch, summary);
        g_vibrationFailureSuppressed[userIndex] = 0;
    }

    char text[256] = {};
    sprintf_s(
        text,
        "[Input][Vibration] ordinary output user=%lu left=%u right=%u result=%s.\n",
        static_cast<unsigned long>(userIndex),
        static_cast<unsigned int>(leftMotor),
        static_cast<unsigned int>(rightMotor),
        attempted ? "failed" : "unavailable/not attempted");
    QueueNativeOutputLog(batch, text);
    g_vibrationFailureLastLogMs[userIndex] = now;
}

void AddZeroTarget(DWORD& targets, DWORD userIndex)
{
    if (userIndex < kMaxGamepads)
        targets |= (1u << userIndex);
}

void ClearSelectedVehicleTriggersLocked()
{
    // Consumers key off validity. Withdraw validity before touching the floats
    // so a concurrent detour can never observe a newly-disabled pair as valid.
    InterlockedExchange(&g_vehicleAnalogTriggersValid, 0);
    g_vehicleLeftTrigger01 = 0.0f;
    g_vehicleRightTrigger01 = 0.0f;
}

void PublishSelectedVehicleTriggersLocked(const GamepadState& pad, bool eligible)
{
    ClearSelectedVehicleTriggersLocked();

    if (!eligible ||
        !g_analogVehicleTriggersEnabled.load(std::memory_order_relaxed) ||
        !g_vehicleAnalogPatchInstalled.load(std::memory_order_acquire))
    {
        return;
    }

    const BYTE deadzone = static_cast<BYTE>(
        g_vehicleTriggerDeadzoneRaw.load(std::memory_order_relaxed));
    g_vehicleLeftTrigger01 = pad.leftTrigger > deadzone
        ? static_cast<float>(pad.leftTrigger) * kXboxTriggerScale
        : 0.0f;
    g_vehicleRightTrigger01 = pad.rightTrigger > deadzone
        ? static_cast<float>(pad.rightTrigger) * kXboxTriggerScale
        : 0.0f;
    InterlockedExchange(&g_vehicleAnalogTriggersValid, 1);
}

void ResetCombatBaselineFieldsLocked()
{
    g_combatStrafeInputInitialized = false;
    g_combatStrafeInputUser = 0;
    g_combatStrafePreviousShoulders = 0;
    ++g_combatStrafeInputRevision;
}

void ResetCombatBaselineForOwnerChangeLocked()
{
    // Lock ordering is always output -> combat. No provider/native call occurs
    // while the combat lock is held.
    std::lock_guard<std::mutex> combatLock(g_combatStrafeInputMutex);
    ResetCombatBaselineFieldsLocked();
}

void ClearPulseStateLocked()
{
    g_vibrationTestActive.store(false, std::memory_order_release);
    g_vibrationTestDeadlineMs.store(0, std::memory_order_release);
}

DWORD CaptureAndClearPulseTargetLocked()
{
    if (!g_vibrationTestActive.load(std::memory_order_acquire))
        return kMaxGamepads;

    const DWORD userIndex =
        g_vibrationTestUser.load(std::memory_order_relaxed);
    ClearPulseStateLocked();
    return userIndex;
}

void QueuePulseCancellationLocked(DWORD& zeroTargets)
{
    AddZeroTarget(zeroTargets, CaptureAndClearPulseTargetLocked());
}

void WithdrawGameplayOwnerLocked(DWORD& zeroTargets)
{
    if (!g_activeGamepadUserValid.load(std::memory_order_acquire))
        return;

    const DWORD previousOwner =
        g_activeGamepadUser.load(std::memory_order_relaxed);
    if (g_vibrationTestActive.load(std::memory_order_acquire) &&
        g_vibrationTestUser.load(std::memory_order_relaxed) == previousOwner)
    {
        QueuePulseCancellationLocked(zeroTargets);
    }
    AddZeroTarget(zeroTargets, previousOwner);
    g_activeGamepadUserValid.store(false, std::memory_order_release);
    ++g_ownerEpoch;
    ResetCombatBaselineForOwnerChangeLocked();
    g_lastMotorStateValid = false;
}

void SynchronizeNativeInputModeLocked(DWORD& zeroTargets)
{
    bool controller = false;
    if (!TryGetVanillaInputMode(controller))
    {
        // Unknown mode fails closed for gameplay, but it is not a fabricated
        // keyboard transition and says nothing about provider connectivity.
        if (g_activeGamepadUserValid.load(std::memory_order_acquire))
            WithdrawGameplayOwnerLocked(zeroTargets);
        g_nativeModeKnown = false;
        g_ownerReady = false;
        ClearSelectedVehicleTriggersLocked();
        return;
    }

    if (!g_nativeModeKnown)
    {
        g_nativeModeKnown = true;
        g_nativeControllerMode = controller;
        // First observation seeds mode authority; it is not a transition.
        // Controller mode still needs a fresh selected-slot reconciliation.
        if (controller)
        {
            g_ownerReady = false;
            ClearSelectedVehicleTriggersLocked();
            g_lastMotorStateValid = false;
        }
        else if (g_activeGamepadUserValid.load(std::memory_order_acquire))
        {
            WithdrawGameplayOwnerLocked(zeroTargets);
            g_ownerReady = false;
            ClearSelectedVehicleTriggersLocked();
        }
        return;
    }

    if (controller == g_nativeControllerMode)
    {
        // Steady keyboard mode must not cancel a diagnostic deliberately
        // started after the transition. An anomalous gameplay owner is still
        // withdrawn because keyboard mode cannot authorize gameplay output.
        if (!controller &&
            g_activeGamepadUserValid.load(std::memory_order_acquire))
        {
            WithdrawGameplayOwnerLocked(zeroTargets);
            g_ownerReady = false;
            ClearSelectedVehicleTriggersLocked();
        }
        return;
    }

    const bool wasController = g_nativeControllerMode;
    g_nativeControllerMode = controller;

    if (wasController && !controller)
    {
        // A real Controller -> Keyboard transition cancels the pulse that
        // existed before the transition, then withdraws gameplay authority.
        QueuePulseCancellationLocked(zeroTargets);
        zeroTargets |= g_gameplayOutputPendingStopMask;
        if (g_activeGamepadUserValid.load(std::memory_order_acquire))
            WithdrawGameplayOwnerLocked(zeroTargets);
        g_ownerReady = false;
        ClearSelectedVehicleTriggersLocked();
        g_lastMotorStateValid = false;
        return;
    }

    // Keyboard -> Controller does not elect an owner and never replays to an
    // old one. The post-update commit is the positive authorization boundary.
    g_ownerReady = false;
    ClearSelectedVehicleTriggersLocked();
    g_lastMotorStateValid = false;
}

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

bool WriteVibrationLocked(
    DWORD userIndex,
    WORD leftMotor,
    WORD rightMotor,
    bool force,
    bool* attempted = nullptr)
{
    if (attempted != nullptr)
        *attempted = false;

    if (!GamepadBackendSupportsVibration() || userIndex >= kMaxGamepads)
    {
        g_lastMotorStateValid = false;
        return false;
    }

    if (!force && g_lastMotorStateValid &&
        g_lastMotorUser == userIndex &&
        g_lastMotorLeft == leftMotor &&
        g_lastMotorRight == rightMotor)
    {
        return true;
    }

    // Cache validity means the provider accepted this exact hardware write.
    // Invalidate before every attempt so a rejected send can never suppress a
    // later retry of the same values.
    g_lastMotorStateValid = false;
    if (attempted != nullptr)
        *attempted = true;
    const bool result = SetGamepadVibration(userIndex, leftMotor, rightMotor);
    if (result)
    {
        g_lastMotorUser = userIndex;
        g_lastMotorLeft = leftMotor;
        g_lastMotorRight = rightMotor;
        g_lastMotorStateValid = true;
    }
    return result;
}

bool WriteGameplayVibrationLocked(
    DWORD userIndex,
    WORD leftMotor,
    WORD rightMotor,
    bool force,
    NativeOutputLogBatch* logs,
    bool* attemptedOut = nullptr)
{
    if (userIndex >= kMaxGamepads)
        return false;

    const DWORD bit = 1u << userIndex;
    if (leftMotor != 0 || rightMotor != 0)
        g_gameplayOutputPendingStopMask |= bit;

    bool attempted = false;
    const bool result = WriteVibrationLocked(
        userIndex, leftMotor, rightMotor, force, &attempted);
    if (attemptedOut != nullptr)
        *attemptedOut = attempted;
    if (result && leftMotor == 0 && rightMotor == 0)
        g_gameplayOutputPendingStopMask &= ~bit;

    if (!result && logs != nullptr)
    {
        QueueOrdinaryVibrationFailureLocked(
            *logs, userIndex, leftMotor, rightMotor, attempted);
    }
    return result;
}

void FlushExplicitZerosLocked(
    DWORD zeroTargets,
    const char* reason,
    NativeOutputLogBatch& logs)
{
    for (DWORD userIndex = 0; userIndex < kMaxGamepads; ++userIndex)
    {
        const DWORD bit = 1u << userIndex;
        if ((zeroTargets & bit) == 0)
            continue;

        bool attempted = false;
        const bool stopped = WriteVibrationLocked(
            userIndex, 0, 0, true, &attempted);
        if (stopped)
            g_gameplayOutputPendingStopMask &= ~bit;
        QueueVibrationStopLog(
            logs, userIndex, reason, stopped, attempted);
    }
}

bool GetGameplayRestorePairLocked(
    DWORD userIndex,
    WORD& leftMotor,
    WORD& rightMotor)
{
    leftMotor = 0;
    rightMotor = 0;

    if (!g_ownerReady || !g_nativeModeKnown || !g_nativeControllerMode ||
        !g_activeGamepadUserValid.load(std::memory_order_acquire) ||
        g_activeGamepadUser.load(std::memory_order_relaxed) != userIndex ||
        !g_vibrationEnabled.load(std::memory_order_relaxed))
    {
        return false;
    }

    const float strength =
        g_vibrationStrength.load(std::memory_order_relaxed);
    const uint32_t motorState =
        g_currentNativeMotorState.load(std::memory_order_acquire);
    leftMotor = ApplyVibrationStrength(
        static_cast<WORD>(motorState & 0xFFFFu), strength);
    rightMotor = ApplyVibrationStrength(
        static_cast<WORD>(motorState >> 16), strength);
    return true;
}

bool RestoreOrStopCapturedTargetLocked(
    DWORD userIndex,
    WORD& leftMotor,
    WORD& rightMotor,
    NativeOutputLogBatch* logs = nullptr,
    bool* attemptedOut = nullptr)
{
    const bool gameplayRestore =
        GetGameplayRestorePairLocked(userIndex, leftMotor, rightMotor);
    if (gameplayRestore)
    {
        return WriteGameplayVibrationLocked(
            userIndex, leftMotor, rightMotor, true, logs, attemptedOut);
    }

    leftMotor = 0;
    rightMotor = 0;
    bool attempted = false;
    const bool result = WriteVibrationLocked(
        userIndex, 0, 0, true, &attempted);
    if (attemptedOut != nullptr)
        *attemptedOut = attempted;
    if (result && userIndex < kMaxGamepads)
        g_gameplayOutputPendingStopMask &= ~(1u << userIndex);
    return result;
}

void RefreshGamepadVibrationFromNativeStateLocked(NativeOutputLogBatch* logs)
{
    if (g_vibrationTestActive.load(std::memory_order_acquire) || !g_ownerReady ||
        !g_nativeModeKnown || !g_nativeControllerMode ||
        !g_activeGamepadUserValid.load(std::memory_order_acquire))
    {
        return;
    }

    const DWORD userIndex =
        g_activeGamepadUser.load(std::memory_order_relaxed);
    if (userIndex >= kMaxGamepads)
        return;

    WORD leftMotor = 0;
    WORD rightMotor = 0;
    GetGameplayRestorePairLocked(userIndex, leftMotor, rightMotor);
    WriteGameplayVibrationLocked(
        userIndex, leftMotor, rightMotor, false, logs);
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

void RefreshGamepadVibrationFromNativeState()
{
    NativeOutputLogBatch logs;
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);
        if (zeroTargets != 0)
            FlushExplicitZerosLocked(zeroTargets, "input mode changed", logs);
        RefreshGamepadVibrationFromNativeStateLocked(&logs);
    }
    FlushNativeOutputLogs(logs);
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
    RefreshGamepadVibrationFromNativeState();
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
    RefreshGamepadVibrationFromNativeState();
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

    // Replay the complete 12-byte sequence replaced by the detour. The
    // original code leaves the x87 stack balanced and stores abs([esi+13E4])
    // to [esp+14h]; reproducing only the initial fld would leak one x87 value
    // per invocation and skip the native store. popfd above has already
    // restored the original stack pointer, so the original displacement is
    // valid here.
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x86);
    EmitVehicle32(cursor, 0x000013E4u); // fld  dword ptr [esi+13E4h]
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0xE1); // fabs
    EmitVehicle8(cursor, 0xD9); EmitVehicle8(cursor, 0x5C);
    EmitVehicle8(cursor, 0x24); EmitVehicle8(cursor, 0x14); // fstp [esp+14h]

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

bool PollIntegratedGamepadState(UINT joyId, GamepadState& pad)
{
    pad = {};
    if (joyId >= kMaxGamepads)
        return false;
    if (!PollGamepadState(joyId, pad))
    {
        g_xboxLeftStickX[joyId].store(0.0f, std::memory_order_relaxed);
        g_xboxLeftStickY[joyId].store(0.0f, std::memory_order_relaxed);
        g_xboxRightStickX[joyId].store(0.0f, std::memory_order_relaxed);
        g_xboxRightStickY[joyId].store(0.0f, std::memory_order_relaxed);
        g_xboxLeftStickValid[joyId].store(false, std::memory_order_release);
        g_lastGamepadSequenceValid[joyId].store(false, std::memory_order_release);
        return false;
    }

    // Generic/all-pad polling is observation-only. It feeds AutoSwitch and the
    // per-user stick/sequence caches, but it must never elect gameplay
    // ownership, publish shared vehicle triggers, or stop another controller's
    // motors. The post-update selected-slot reconciliation owns those actions.
    g_lastGamepadSequence[joyId].store(
        pad.stateSequence, std::memory_order_relaxed);
    g_lastGamepadSequenceValid[joyId].store(true, std::memory_order_release);

    g_xboxLeftStickX[joyId].store(
        NormalizeXboxStickAxis(pad.leftX),
        std::memory_order_relaxed);
    g_xboxLeftStickY[joyId].store(
        NormalizeXboxStickAxis(pad.leftY),
        std::memory_order_relaxed);
    g_xboxRightStickX[joyId].store(
        NormalizeXboxStickAxis(pad.rightX),
        std::memory_order_relaxed);
    g_xboxRightStickY[joyId].store(
        NormalizeXboxStickAxis(pad.rightY),
        std::memory_order_relaxed);
    g_xboxLeftStickValid[joyId].store(true, std::memory_order_release);
    return true;
}

bool InstallNativeVibrationBridge(const DpBuildProfile& build)
{
    if (!GamepadBackendSupportsVibration())
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
        RemoveInputHookSafely(
            commandTarget,
            reinterpret_cast<void**>(&g_originalRdInputSetActuator),
            "CRdInput::SetActuator");
        AppendLog(
            "[Input][Vibration] ERROR: Failed to hook CInput_Actuator output.\n");
        return false;
    }

    status = MH_EnableHook(commandTarget);
    if (status != MH_OK)
    {
        RemoveInputHookSafely(
            stateTarget,
            reinterpret_cast<void**>(&g_originalInputActuatorSetSecond),
            "CInput_Actuator output");
        RemoveInputHookSafely(
            commandTarget,
            reinterpret_cast<void**>(&g_originalRdInputSetActuator),
            "CRdInput::SetActuator");
        AppendLog(
            "[Input][Vibration] ERROR: Failed to enable CRdInput actuator hook.\n");
        return false;
    }

    status = MH_EnableHook(stateTarget);
    if (status != MH_OK)
    {
        const MH_STATUS disableStatus = MH_DisableHook(commandTarget);
        if (disableStatus != MH_OK && disableStatus != MH_ERROR_DISABLED)
        {
            AppendLog(
                "[Input][Vibration] ERROR: rollback could not disable CRdInput::SetActuator before removal.\n");
        }
        RemoveInputHookSafely(
            stateTarget,
            reinterpret_cast<void**>(&g_originalInputActuatorSetSecond),
            "CInput_Actuator output");
        RemoveInputHookSafely(
            commandTarget,
            reinterpret_cast<void**>(&g_originalRdInputSetActuator),
            "CRdInput::SetActuator");
        AppendLog(
            "[Input][Vibration] ERROR: Failed to enable CInput_Actuator output hook.\n");
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        g_vibrationEnabled.store(g_config.vibrationEnabled, std::memory_order_release);
        g_vibrationStrength.store(g_config.vibrationStrength, std::memory_order_release);
    }
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
    if (g_directBindingGamepad != nullptr)
        return EvaluateCanonicalGamepadBinding(*g_directBindingGamepad, binding);

    // Outside the short native-record rebuild scope this hook is transparent.
    // Keyboard/mouse and legacy WinMM therefore retain DP's exact evaluator.
    return g_originalControllerBindingEvaluator(
        size, flags, x, y, z, r, u, v, buttons, buttonNumber, pov,
        reserved1, reserved2, binding);
}

void CommitSelectedGameplayOwner(
    bool haveSelectionContext,
    int selectedSlot,
    const GamepadState* selectedPad,
    bool selectedConnected,
    const bool* freshConnected,
    NativeOutputLogBatch& logs)
{
    std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);

    DWORD zeroTargets = 0;
    SynchronizeNativeInputModeLocked(zeroTargets);

    if (freshConnected != nullptr)
    {
        g_haveDiagnosticSample = true;
        for (std::uint32_t user = 0; user < kMaxGamepads; ++user)
            g_diagnosticConnected[user] = freshConnected[user];

        if (g_vibrationTestActive.load(std::memory_order_acquire))
        {
            const DWORD testUser =
                g_vibrationTestUser.load(std::memory_order_relaxed);
            if (testUser >= kMaxGamepads || !freshConnected[testUser])
                QueuePulseCancellationLocked(zeroTargets);
        }
    }

    const bool previousValid =
        g_activeGamepadUserValid.load(std::memory_order_acquire);
    const DWORD previousOwner =
        g_activeGamepadUser.load(std::memory_order_relaxed);

    const bool selectedSupported =
        selectedSlot >= 0 && selectedSlot < static_cast<int>(kMaxGamepads);
    const bool nextValid =
        haveSelectionContext && freshConnected != nullptr &&
        g_nativeModeKnown && g_nativeControllerMode &&
        selectedSupported && selectedConnected;
    const DWORD nextOwner = nextValid
        ? static_cast<DWORD>(selectedSlot)
        : 0;

    // Diagnostic compatibility is independent of whether a previous gameplay
    // owner existed. This covers both owner-to-owner handoff and None -> owner.
    if (g_vibrationTestActive.load(std::memory_order_acquire))
    {
        const DWORD testUser =
            g_vibrationTestUser.load(std::memory_order_relaxed);
        const bool incompatibleWithNext =
            nextValid ? testUser != nextOwner
                      : previousValid && testUser == previousOwner;
        if (incompatibleWithNext)
            QueuePulseCancellationLocked(zeroTargets);
    }

    const bool identityChanged =
        previousValid != nextValid ||
        (previousValid && nextValid && previousOwner != nextOwner);

    if (identityChanged)
    {
        if (previousValid)
            AddZeroTarget(zeroTargets, previousOwner);

        // Failed cleanup from older gameplay destinations remains independent
        // from the one-entry hardware cache. Do not opportunistically stop a
        // diagnostic that is intentionally continuing, or the newly selected
        // owner that is about to become authorized.
        DWORD opportunistic = g_gameplayOutputPendingStopMask;
        if (g_vibrationTestActive.load(std::memory_order_acquire))
        {
            const DWORD testUser =
                g_vibrationTestUser.load(std::memory_order_relaxed);
            if (testUser < kMaxGamepads)
                opportunistic &= ~(1u << testUser);
        }
        if (nextValid)
            opportunistic &= ~(1u << nextOwner);
        zeroTargets |= opportunistic;

        g_activeGamepadUserValid.store(false, std::memory_order_release);
        ++g_ownerEpoch;
        ResetCombatBaselineForOwnerChangeLocked();
        g_lastMotorStateValid = false;
    }

    if (nextValid)
    {
        g_activeGamepadUser.store(nextOwner, std::memory_order_relaxed);
        g_activeGamepadUserValid.store(true, std::memory_order_release);
    }
    else
    {
        g_activeGamepadUserValid.store(false, std::memory_order_release);
    }

    // Cleanup is explicit and may run while readiness is still false. Only
    // after owner/diagnostic cleanup and baseline reset are complete do we
    // publish reconciled readiness and the selected trigger sample.
    if (zeroTargets != 0)
    {
        FlushExplicitZerosLocked(
            zeroTargets,
            identityChanged ? "gameplay controller ownership changed"
                            : "input mode or diagnostic target changed",
            logs);
    }

    g_ownerReady = true;
    if (nextValid && selectedPad != nullptr)
        PublishSelectedVehicleTriggersLocked(*selectedPad, true);
    else
        ClearSelectedVehicleTriggersLocked();

    RefreshGamepadVibrationFromNativeStateLocked(&logs);
}

} // namespace

void BeginNativeGamepadInputUpdate()
{
    if (!g_nativeGamepadBackendInstalled.load(std::memory_order_acquire))
        return;

    std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
    g_ownerReady = false;
    ++g_inputReconciliationSerial;
    ClearSelectedVehicleTriggersLocked();
}

void ApplyNativeGamepadInputRecord(void* inputState, void* actionState)
{
    if (!g_nativeGamepadBackendInstalled.load(std::memory_order_acquire))
        return;

    NativeOutputLogBatch logs;

    if (inputState == nullptr)
    {
        // Missing selection context is not a provider observation. Withdraw
        // gameplay ownership but preserve the last real diagnostic connectivity
        // sample and any independent diagnostic target.
        CommitSelectedGameplayOwner(
            false, -1, nullptr, false, nullptr, logs);
        FlushNativeOutputLogs(logs);
        return;
    }

    auto* inputBytes = static_cast<unsigned char*>(inputState);

    int selectedSlot = -1;
    for (int slot = 0; slot < 7; ++slot)
    {
        if (inputBytes[static_cast<std::size_t>(slot) * 0x36] == 1u)
        {
            selectedSlot = slot;
            break;
        }
    }

    GamepadState pads[kMaxGamepads] = {};
    bool connected[kMaxGamepads] = {};
    for (std::uint32_t user = 0; user < kMaxGamepads; ++user)
        connected[user] = PollIntegratedGamepadState(user, pads[user]);

    bool controllerMode = false;
    const bool haveMode = TryGetVanillaInputMode(controllerMode);
    if (!haveMode)
        controllerMode = false;

    const bool selectedSupported =
        selectedSlot >= 0 && selectedSlot < static_cast<int>(kMaxGamepads);
    const bool selectedConnected =
        selectedSupported && connected[static_cast<std::uint32_t>(selectedSlot)];
    const GamepadState* selectedSample = selectedSupported
        ? &pads[static_cast<std::uint32_t>(selectedSlot)]
        : nullptr;

    CommitSelectedGameplayOwner(
        true,
        selectedSlot,
        selectedSample,
        selectedConnected,
        connected,
        logs);
    FlushNativeOutputLogs(logs);

    // Lifecycle/ownership reconciliation above is required even while DP is in
    // keyboard mode, has no selected slot, or suppresses action records. Those
    // gates affect only controller action rebuilding below.
    if (!controllerMode || selectedSlot < 0 || actionState == nullptr)
        return;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
        return;

    auto* actionBytes = static_cast<unsigned char*>(actionState);
    auto* record = actionBytes + static_cast<std::size_t>(selectedSlot) * 0x6C;
    DWORD active = 0;
    std::memcpy(&active, record, sizeof(active));

    // The original input update owns focus/suppression behavior. If it zeroed
    // this slot, do not resurrect controller input while the game is inactive.
    if (active != 1u)
        return;

    GamepadState selectedPad = {};
    if (selectedSupported)
        selectedPad = pads[static_cast<std::uint32_t>(selectedSlot)];

    const std::size_t slotBase = static_cast<std::size_t>(selectedSlot) * 0x36;
    unsigned char& connectedByte = inputBytes[slotBase + 1];
    const unsigned char originalConnectedByte = connectedByte;
    connectedByte = selectedConnected ? 1u : 0u;

    g_directBindingGamepad = selectedConnected ? &selectedPad : nullptr;

    auto* helperBase = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->input.controllerActionHelperBaseRva);
    for (const ControllerActionField& field : kControllerActionFields)
    {
        const auto helper = reinterpret_cast<ControllerActionHelperFn>(
            helperBase + field.helperOffset);
        const int value = helper(inputState, static_cast<unsigned char>(selectedSlot));
        std::memcpy(record + field.outputOffset, &value, sizeof(value));
    }

    g_directBindingGamepad = nullptr;
    connectedByte = originalConnectedByte;

    if (!g_directInputRecordObserved.exchange(true, std::memory_order_acq_rel))
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Input][NativeGamepad] Native controller 0x6C record rebuild active "
            "(DP slot=%d, provider=%s).\n",
            selectedSlot,
            GetActiveGamepadBackendName());
        AppendLog(text);
    }
}

bool PollNativeGamepadState(std::uint32_t index, GamepadState& state)
{
    return PollIntegratedGamepadState(static_cast<UINT>(index), state);
}

bool IsAnalogVehicleTriggerPatchAvailable()
{
    return g_vehicleAnalogPatchInstalled.load(std::memory_order_acquire);
}

bool IsNativeVibrationAvailable()
{
    return g_nativeVibrationInstalled.load(std::memory_order_acquire) &&
           GamepadBackendSupportsVibration();
}

bool RunNativeVibrationTestPulse()
{
    if (!IsNativeVibrationAvailable())
    {
        AppendLog("[Input][VibrationTest] Test pulse unavailable: vibration backend unavailable.\n");
        return false;
    }

    DWORD userIndex = kMaxGamepads;
    bool onResult = false;
    bool onHardwareAttempted = false;
    bool recoveryAttempted = false;
    bool recoveryHardwareAttempted = false;
    bool recoveryResult = false;
    WORD recoveryLeft = 0;
    WORD recoveryRight = 0;
    bool alreadyActive = false;
    bool noSample = false;
    bool noTarget = false;
    NativeOutputLogBatch logs;

    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);
        if (zeroTargets != 0)
            FlushExplicitZerosLocked(zeroTargets, "input mode changed", logs);

        if (g_vibrationTestActive.load(std::memory_order_acquire))
        {
            alreadyActive = true;
        }
        else if (!g_haveDiagnosticSample)
        {
            noSample = true;
        }
        else
        {
            if (g_ownerReady &&
                g_activeGamepadUserValid.load(std::memory_order_acquire))
            {
                const DWORD owner =
                    g_activeGamepadUser.load(std::memory_order_relaxed);
                if (owner < kMaxGamepads && g_diagnosticConnected[owner])
                    userIndex = owner;
            }

            if (userIndex >= kMaxGamepads)
            {
                for (DWORD user = 0; user < kMaxGamepads; ++user)
                {
                    if (g_diagnosticConnected[user])
                    {
                        userIndex = user;
                        break;
                    }
                }
            }

            if (userIndex >= kMaxGamepads)
            {
                noTarget = true;
            }
            else
            {
                g_vibrationTestUser.store(userIndex, std::memory_order_relaxed);
                onResult = WriteVibrationLocked(
                    userIndex, 65535, 65535, true, &onHardwareAttempted);
                if (onResult)
                {
                    g_vibrationTestDeadlineMs.store(
                        GetTickCount64() + 750ull,
                        std::memory_order_release);
                    g_vibrationTestActive.store(true, std::memory_order_release);
                }
                else
                {
                    recoveryAttempted = true;
                    recoveryResult = RestoreOrStopCapturedTargetLocked(
                        userIndex,
                        recoveryLeft,
                        recoveryRight,
                        nullptr,
                        &recoveryHardwareAttempted);
                    ClearPulseStateLocked();
                }
            }
        }
    }

    FlushNativeOutputLogs(logs);

    if (alreadyActive)
    {
        AppendLog("[Input][VibrationTest] Test pulse already active.\n");
        return false;
    }
    if (noSample)
    {
        AppendLog("[Input][VibrationTest] Test pulse unavailable until a gamepad input sample is observed.\n");
        return false;
    }
    if (noTarget)
    {
        AppendLog("[Input][VibrationTest] Test pulse unavailable: no sampled connected gamepad.\n");
        return false;
    }

    if (!onResult)
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[Input][VibrationTest] ON user=%lu left=65535 right=65535 result=%s.\n",
            static_cast<unsigned long>(userIndex),
            onHardwareAttempted ? "failed" : "unavailable/not attempted");
        AppendLog(text);

        if (recoveryAttempted)
        {
            char recovery[288] = {};
            sprintf_s(
                recovery,
                "[Input][VibrationTest] RECOVERY user=%lu left=%u right=%u result=%s.\n",
                static_cast<unsigned long>(userIndex),
                static_cast<unsigned int>(recoveryLeft),
                static_cast<unsigned int>(recoveryRight),
                recoveryResult
                    ? "ok"
                    : (recoveryHardwareAttempted
                        ? "failed"
                        : "unavailable/not attempted"));
            AppendLog(recovery);
        }
        return false;
    }

    char text[256] = {};
    sprintf_s(
        text,
        "[Input][VibrationTest] ON user=%lu left=65535 right=65535 result=ok; non-blocking 750 ms pulse started.\n",
        static_cast<unsigned long>(userIndex));
    AppendLog(text);
    return true;
}

void PollNativeVibrationTestPulse()
{
    DWORD userIndex = kMaxGamepads;
    WORD restoreLeft = 0;
    WORD restoreRight = 0;
    bool restoreResult = false;
    bool expired = false;
    NativeOutputLogBatch logs;

    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);
        if (zeroTargets != 0)
            FlushExplicitZerosLocked(zeroTargets, "input mode changed", logs);

        if (!g_vibrationTestActive.load(std::memory_order_acquire))
        {
            // A real mode transition may have canceled the pulse above. Its
            // captured zero/result log is still emitted after releasing O.
        }
        else
        {
            const ULONGLONG deadline =
                g_vibrationTestDeadlineMs.load(std::memory_order_acquire);
            if (GetTickCount64() >= deadline)
            {
                userIndex =
                    g_vibrationTestUser.load(std::memory_order_relaxed);
                ClearPulseStateLocked();
                expired = true;
                if (userIndex < kMaxGamepads)
                {
                    restoreResult = RestoreOrStopCapturedTargetLocked(
                        userIndex, restoreLeft, restoreRight, nullptr);
                }
                else
                {
                    g_lastMotorStateValid = false;
                }
            }
        }
    }

    FlushNativeOutputLogs(logs);

    if (!expired)
        return;

    char text[256] = {};
    sprintf_s(
        text,
        "[Input][VibrationTest] RESTORE user=%lu left=%u right=%u result=%s.\n",
        static_cast<unsigned long>(userIndex),
        static_cast<unsigned int>(restoreLeft),
        static_cast<unsigned int>(restoreRight),
        restoreResult ? "ok" : "failed");
    AppendLog(text);
}

void NotifyNativeVibrationInputModeChanged(bool controller)
{
    (void)controller;
    if (!g_nativeGamepadBackendInstalled.load(std::memory_order_acquire))
        return;

    NativeOutputLogBatch logs;
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);
        if (zeroTargets != 0)
            FlushExplicitZerosLocked(zeroTargets, "keyboard/mouse mode", logs);
        // A mode notification never grants positive output. A fresh selected
        // post-update commit is the only replay boundary.
    }
    FlushNativeOutputLogs(logs);
}

bool ApplyNativeVibrationSettings(bool enabled, float strength)
{
    if (strength < 0.0f)
        strength = 0.0f;
    else if (strength > 1.0f)
        strength = 1.0f;

    const bool availableBefore = IsNativeVibrationAvailable();
    NativeOutputLogBatch logs;
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);

        const bool wasEnabled =
            g_vibrationEnabled.load(std::memory_order_relaxed);
        const bool disabling = wasEnabled && !enabled;
        g_vibrationEnabled.store(enabled, std::memory_order_release);
        g_vibrationStrength.store(strength, std::memory_order_release);
        g_config.vibrationEnabled = enabled;
        g_config.vibrationStrength = strength;

        if (disabling)
        {
            QueuePulseCancellationLocked(zeroTargets);
            zeroTargets |= g_gameplayOutputPendingStopMask;
            if (g_activeGamepadUserValid.load(std::memory_order_acquire))
            {
                AddZeroTarget(
                    zeroTargets,
                    g_activeGamepadUser.load(std::memory_order_relaxed));
            }
        }

        if (zeroTargets != 0)
        {
            FlushExplicitZerosLocked(
                zeroTargets,
                disabling
                    ? "vibration disabled"
                    : "input mode changed",
                logs);
        }

        // Strength-only edits while already disabled leave an active diagnostic
        // untouched. Ordinary output remains suppressed while any pulse is active.
        if (availableBefore && !disabling)
            RefreshGamepadVibrationFromNativeStateLocked(&logs);
    }
    FlushNativeOutputLogs(logs);

    return IsNativeVibrationAvailable();
}

void ApplyGamepadInputProfile(GamepadInputProfile profile)
{
    g_config.gamepadInputProfile = profile;
    g_gamepadInputProfile.store(profile, std::memory_order_release);
}

void ApplyAnalogVehicleTriggers(bool enabled)
{
    std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
    const bool wasEnabled =
        g_analogVehicleTriggersEnabled.load(std::memory_order_relaxed);
    g_config.analogVehicleTriggers = enabled;
    g_analogVehicleTriggersEnabled.store(enabled, std::memory_order_release);

    // Disable always withdraws validity. Re-enable also waits for the next
    // authoritative post-update selected sample instead of reviving stale axes.
    if (!enabled || wasEnabled != enabled)
        ClearSelectedVehicleTriggersLocked();
}

void ApplyVehicleTriggerDeadzone(UINT deadzone)
{
    if (deadzone > 254u)
        deadzone = 254u;

    std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
    g_config.vehicleTriggerDeadzone = deadzone;
    g_vehicleTriggerDeadzoneRaw.store(deadzone, std::memory_order_release);
}

XboxCombatStrafeInput PollXboxCombatStrafeInput()
{
    constexpr std::uint32_t kShoulderMask =
        GamepadButton_LeftShoulder | GamepadButton_RightShoulder;

    NativeOutputLogBatch logs;
    DWORD user = 0;
    std::uint64_t ownerEpoch = 0;
    std::uint64_t reconciliationSerial = 0;
    std::uint64_t baselineRevision = 0;
    bool authorized = false;

    {
        std::lock_guard<std::mutex> outputLock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);
        if (zeroTargets != 0)
            FlushExplicitZerosLocked(zeroTargets, "input mode changed", logs);

        if (g_nativeGamepadBackendInstalled.load(std::memory_order_acquire) &&
            IsGamepadBackendAvailable() && g_ownerReady &&
            g_nativeModeKnown && g_nativeControllerMode &&
            g_activeGamepadUserValid.load(std::memory_order_acquire))
        {
            user = g_activeGamepadUser.load(std::memory_order_relaxed);
            ownerEpoch = g_ownerEpoch;
            reconciliationSerial = g_inputReconciliationSerial;
            std::lock_guard<std::mutex> combatLock(g_combatStrafeInputMutex);
            baselineRevision = g_combatStrafeInputRevision;
            authorized = user < kMaxGamepads;
        }
    }

    FlushNativeOutputLogs(logs);
    if (!authorized)
        return XboxCombatStrafeInput::None;

    GamepadState state = {};
    const bool providerOk = PollGamepadState(user, state);
    const std::uint32_t current = state.buttons & kShoulderMask;
    std::uint32_t rising = 0;
    bool accepted = false;
    bool firstSample = false;
    NativeOutputLogBatch postLogs;

    {
        std::lock_guard<std::mutex> outputLock(g_vibrationOutputMutex);
        DWORD zeroTargets = 0;
        SynchronizeNativeInputModeLocked(zeroTargets);
        if (zeroTargets != 0)
            FlushExplicitZerosLocked(zeroTargets, "input mode changed", postLogs);

        const bool stillMatching =
            g_ownerReady && g_nativeModeKnown && g_nativeControllerMode &&
            g_activeGamepadUserValid.load(std::memory_order_acquire) &&
            g_activeGamepadUser.load(std::memory_order_relaxed) == user &&
            g_ownerEpoch == ownerEpoch &&
            g_inputReconciliationSerial == reconciliationSerial;
        if (stillMatching)
        {
            std::lock_guard<std::mutex> combatLock(g_combatStrafeInputMutex);
            if (g_combatStrafeInputRevision == baselineRevision)
            {
                if (!providerOk)
                {
                    ResetCombatBaselineFieldsLocked();
                }
                else if (!g_combatStrafeInputInitialized ||
                         g_combatStrafeInputUser != user)
                {
                    g_combatStrafeInputInitialized = true;
                    g_combatStrafeInputUser = user;
                    g_combatStrafePreviousShoulders = current;
                    ++g_combatStrafeInputRevision;
                    firstSample = true;
                    accepted = true;
                }
                else
                {
                    rising = current & ~g_combatStrafePreviousShoulders;
                    g_combatStrafePreviousShoulders = current;
                    ++g_combatStrafeInputRevision;
                    accepted = true;
                }
            }
        }
    }

    FlushNativeOutputLogs(postLogs);
    if (!providerOk || !accepted || firstSample)
        return XboxCombatStrafeInput::None;

    // Baseline advancement intentionally precedes profile eligibility. Held
    // shoulders therefore never become synthetic edges when the profile flips.
    if (g_gamepadInputProfile.load(std::memory_order_acquire) !=
        GamepadInputProfile::Xbox360)
    {
        return XboxCombatStrafeInput::None;
    }

    const bool leftHeld =
        (current & GamepadButton_LeftShoulder) != 0;
    const bool rightHeld =
        (current & GamepadButton_RightShoulder) != 0;

    if ((rising & GamepadButton_LeftShoulder) != 0 && !rightHeld)
        return XboxCombatStrafeInput::Left;
    if ((rising & GamepadButton_RightShoulder) != 0 && !leftHeld)
        return XboxCombatStrafeInput::Right;

    return XboxCombatStrafeInput::None;
}

void ResetXboxCombatStrafeInput()
{
    std::lock_guard<std::mutex> lock(g_combatStrafeInputMutex);
    ResetCombatBaselineFieldsLocked();
}

bool InstallNativeGamepadBackend()
{
    g_nativeGamepadBackendInstalled.store(false, std::memory_order_release);
    g_directInputRecordObserved.store(false, std::memory_order_release);
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        g_ownerReady = false;
        g_activeGamepadUserValid.store(false, std::memory_order_release);
        ++g_ownerEpoch;
        ++g_inputReconciliationSerial;
        g_nativeModeKnown = false;
        g_nativeControllerMode = false;
        g_haveDiagnosticSample = false;
        for (bool& connected : g_diagnosticConnected)
            connected = false;
        g_gameplayOutputPendingStopMask = 0;
        for (DWORD user = 0; user < kMaxGamepads; ++user)
        {
            g_vibrationFailureLastLogMs[user] = 0;
            g_vibrationFailureSuppressed[user] = 0;
        }
        ClearPulseStateLocked();
        g_lastMotorStateValid = false;
        ClearSelectedVehicleTriggersLocked();
        std::lock_guard<std::mutex> combatLock(g_combatStrafeInputMutex);
        ResetCombatBaselineFieldsLocked();
    }

    if (!g_config.nativeGamepadEnabled)
        return true;

    g_gamepadInputProfile.store(
        g_config.gamepadInputProfile,
        std::memory_order_release);
    {
        std::lock_guard<std::mutex> lock(g_vibrationOutputMutex);
        g_analogVehicleTriggersEnabled.store(
            g_config.analogVehicleTriggers,
            std::memory_order_release);
        g_vehicleTriggerDeadzoneRaw.store(
            g_config.vehicleTriggerDeadzone,
            std::memory_order_release);
        ClearSelectedVehicleTriggersLocked();
    }

    if (!InitializeGamepadBackend(g_config.gamepadBackend))
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[Input][NativeGamepad] ERROR: requested provider %s failed to initialize.\n",
            GamepadBackendName(g_config.gamepadBackend));
        AppendLog(text);
        return false;
    }

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Input][NativeGamepad] ERROR: DP.exe info unavailable; native backend disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[Input][NativeGamepad] ERROR: Unsupported DP.exe build; native backend disabled.\n");
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
            "[Input][NativeGamepad] ERROR: Controller binding evaluator signature mismatch; "
            "native backend disabled.\n");
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        evaluatorTarget,
        reinterpret_cast<void*>(&HookControllerBindingEvaluator),
        reinterpret_cast<void**>(&g_originalControllerBindingEvaluator));

    if (createStatus != MH_OK)
    {
        AppendLog("[Input][NativeGamepad] ERROR: MH_CreateHook failed for controller binding evaluator.\n");
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(evaluatorTarget);
    if (enableStatus != MH_OK)
    {
        RemoveInputHookSafely(
            evaluatorTarget,
            reinterpret_cast<void**>(&g_originalControllerBindingEvaluator),
            "controller binding evaluator");
        AppendLog("[Input][NativeGamepad] ERROR: MH_EnableHook failed for controller binding evaluator.\n");
        return false;
    }

    auto* helperBase = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build->input.controllerActionHelperBaseRva);
    if (std::memcmp(
            helperBase,
            kExpectedControllerActionHelperBytes,
            sizeof(kExpectedControllerActionHelperBytes)) != 0)
    {
        const MH_STATUS disableStatus = MH_DisableHook(evaluatorTarget);
        if (disableStatus != MH_OK && disableStatus != MH_ERROR_DISABLED)
        {
            AppendLog(
                "[Input][NativeGamepad] ERROR: rollback could not disable controller evaluator before removal.\n");
        }
        RemoveInputHookSafely(
            evaluatorTarget,
            reinterpret_cast<void**>(&g_originalControllerBindingEvaluator),
            "controller binding evaluator");
        AppendLog(
            "[Input][NativeGamepad] ERROR: controller action-helper signature mismatch; "
            "direct native record path disabled.\n");
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

    char readyText[384] = {};
    sprintf_s(
        readyText,
        "[Input] NativeGamepad=true Backend=%s Profile=%s AnalogVehicleTriggers=%s "
        "VehicleTriggerDeadzone=%u Vibration=%s "
        "(binding evaluator DP.exe+0x%08lX, action helpers DP.exe+0x%08lX).\n",
        GetActiveGamepadBackendName(),
        g_config.gamepadInputProfile == GamepadInputProfile::Xbox360
            ? "Xbox360"
            : "PC",
        g_config.analogVehicleTriggers ? "true" : "false",
        g_config.vehicleTriggerDeadzone,
        g_config.vibrationEnabled ? "true" : "false",
        static_cast<unsigned long>(build->input.controllerBindingEvaluatorRva),
        static_cast<unsigned long>(build->input.controllerActionHelperBaseRva));
    AppendLog(readyText);
    g_nativeGamepadBackendInstalled.store(true, std::memory_order_release);
    return true;
}

bool IsNativeGamepadBackendAvailable()
{
    return g_nativeGamepadBackendInstalled.load(std::memory_order_acquire);
}
