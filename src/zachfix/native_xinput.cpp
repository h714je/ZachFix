#include "native_xinput.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <mmsystem.h>
#include <Xinput.h>
#include <MinHook.h>

#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstring>

namespace
{
constexpr size_t kSupportedImageSize = 0x010B5000;
constexpr DWORD kSupportedTimeDateStamp = 0x529721DC;

// DP.exe 0x00400000 image:
//   VA  0x006B1780
//   RVA 0x002B1780
//
// int __cdecl EvaluateControllerBinding(JOYINFOEX fields..., int binding)
//
// Every controller action reaches this evaluator. DP's binding enum assumes
// the original shared-trigger WinMM layout, while the XInput bridge below
// exposes independent sticks/triggers. Remapping here preserves DP's mutable
// controller-binding table and all vanilla action logic.
constexpr uintptr_t kControllerBindingEvaluatorRva = 0x002B1780;

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
HMODULE g_xinputModule = nullptr;
XInputGetStateFn g_xinputGetState = nullptr;

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
        return JOYERR_UNPLUGGED;

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

    if (g_mainExeSize != kSupportedImageSize ||
        g_mainExeTimeDateStamp != kSupportedTimeDateStamp)
    {
        AppendLog("[Input][XInput] ERROR: Unsupported DP.exe build; native backend disabled.\n");
        return false;
    }

    auto* evaluatorTarget = reinterpret_cast<unsigned char*>(
        g_mainExeBase + kControllerBindingEvaluatorRva);

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

    AppendLog(
        "[Input][XInput] Native backend ready at DP controller evaluator "
        "(DP.exe+0x2B1780).\n");
    return true;
}
