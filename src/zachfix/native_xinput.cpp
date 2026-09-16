#include "native_xinput.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <mmsystem.h>
#include <Xinput.h>
#include <MinHook.h>

#include <atomic>
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

using JoyGetPosExFn = MMRESULT (WINAPI*)(UINT, LPJOYINFOEX);
JoyGetPosExFn g_originalJoyGetPosEx = nullptr;

using XInputGetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_STATE*);
using XInputSetStateFn = DWORD (WINAPI*)(DWORD, XINPUT_VIBRATION*);
HMODULE g_xinputModule = nullptr;
XInputGetStateFn g_xinputGetState = nullptr;
XInputSetStateFn g_xinputSetState = nullptr;

std::atomic<DWORD> g_activeXInputUser{ 0 };
std::atomic_bool g_activeXInputUserValid{ false };
std::atomic_bool g_vibrationEnabled{ true };
std::atomic<float> g_vibrationStrength{ 1.0f };
std::atomic<uint64_t> g_currentActuatorState{ 0 };
std::atomic_bool g_nativeVibrationInstalled{ false };
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
    0xF8, 0x8B, 0x45, 0xF8, 0x83, 0x78, 0x04, 0x00
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
    if (g_xinputSetState(userIndex, &vibration) == ERROR_SUCCESS)
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

void RefreshXInputVibrationFromNativeState()
{
    if (!g_vibrationEnabled.load(std::memory_order_acquire))
    {
        SendXInputVibration(0, 0);
        return;
    }

    const float strength = g_vibrationStrength.load(std::memory_order_acquire);
    const uint64_t actuatorState =
        g_currentActuatorState.load(std::memory_order_acquire);
    const DWORD rawA = static_cast<DWORD>(actuatorState & 0xFFFFFFFFull);
    const DWORD rawBStored = static_cast<DWORD>(actuatorState >> 32);

    const WORD nativeLeft =
        static_cast<WORD>(rawA > 65535u ? 65535u : rawA);
    const WORD nativeRight = ScaleStoredSecondActuatorToMotor(rawBStored);

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

    g_originalRdInputSetActuator(self, slot, actuatorA, actuatorB, duration);
}

void __fastcall HookInputActuatorSetSecond(
    void* self, void*, int slot, DWORD value)
{
    g_originalInputActuatorSetSecond(self, slot, value);

    if (self == nullptr || slot != 0)
        return;

    const uint64_t actuatorState =
        static_cast<uint64_t>(ReadDwordField(self, 0x28)) |
        (static_cast<uint64_t>(ReadDwordField(self, 0x2C)) << 32);
    g_currentActuatorState.store(actuatorState, std::memory_order_release);
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

DWORD TriggerToPositiveAxis(BYTE value)
{
    // Released remains at DP's neutral center; fully pressed reaches 65535 so
    // DP's existing HIGH threshold can evaluate each trigger independently.
    constexpr DWORD kNeutral = 32767;
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
        if (g_activeXInputUserValid.load(std::memory_order_acquire) &&
            g_activeXInputUser.load(std::memory_order_relaxed) == joyId)
        {
            g_activeXInputUserValid.store(false, std::memory_order_release);
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
    const DWORD mappedButtons = MapXInputButtons(pad.wButtons);

    // Synthetic compatibility view consumed by DP:
    //   X/Y = left stick, Z/R = right stick, U = LT, V = RT.
    info->dwXpos = SignedAxisToWinMM(pad.sThumbLX, false);
    info->dwYpos = SignedAxisToWinMM(pad.sThumbLY, true);
    info->dwZpos = SignedAxisToWinMM(pad.sThumbRX, false);
    info->dwRpos = SignedAxisToWinMM(pad.sThumbRY, true);
    info->dwUpos = TriggerToPositiveAxis(pad.bLeftTrigger);
    info->dwVpos = TriggerToPositiveAxis(pad.bRightTrigger);
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

    if (createStatus != MH_OK && createStatus != MH_ERROR_ALREADY_CREATED)
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
        g_mainExeBase + build.rdInputSetActuatorRva);
    auto* stateTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + build.inputActuatorSetSecondRva);

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
        "(CRdInput=DP.exe+0x%08lX, CInput_Actuator=DP.exe+0x%08lX).\n",
        g_config.vibrationEnabled ? "true" : "false",
        static_cast<double>(g_config.vibrationStrength),
        static_cast<unsigned long>(build.rdInputSetActuatorRva),
        static_cast<unsigned long>(build.inputActuatorSetSecondRva));
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

bool IsNativeVibrationAvailable()
{
    return g_nativeVibrationInstalled.load(std::memory_order_acquire) &&
           g_xinputSetState != nullptr;
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

bool InstallNativeXInputBackend()
{
    if (!g_config.nativeXInputEnabled)
        return true;

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
        g_mainExeBase + build->controllerBindingEvaluatorRva);

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
        AppendLog("[Input][XInput] ERROR: MH_EnableHook failed for controller binding evaluator.\n");
        return false;
    }

    if (!InstallJoyGetPosExBridge())
    {
        MH_DisableHook(evaluatorTarget);
        AppendLog(
            "[Input][XInput] ERROR: joyGetPosEx bridge failed; "
            "controller evaluator hook disabled.\n");
        return false;
    }

    // Rumble restoration is non-fatal for controller input. Unsupported or
    // mismatched actuator code leaves input working normally without motors.
    InstallNativeVibrationBridge(*build);

    char readyText[192] = {};
    sprintf_s(
        readyText,
        "[Input][XInput] Native backend ready at DP controller evaluator "
        "(DP.exe+0x%08lX).\n",
        static_cast<unsigned long>(build->controllerBindingEvaluatorRva));
    AppendLog(readyText);
    return true;
}
