#include "ui_settings.h"

#include "config.h"
#include "dof_blur.h"
#include "difficulty.h"
#include "logging.h"
#include "main_exe.h"
#include "native_xinput.h"
#include "runtime_resources.h"
#include "gameplay_pause.h"
#include "world_streaming.h"
#include "texture_override.h"
#include "shader_probe.h"
#include "postfx.h"
#include "postfx_ao.h"
#include "postfx_bloom.h"
#include "postfx_dof.h"
#include "postfx_exposure.h"
#include "version.h"

#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>
#include <intrin.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <cstring>
#include <cstdio>

#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_dx9.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace
{
HWND g_window = nullptr;
WNDPROC g_originalWndProc = nullptr;
bool g_initialized = false;
bool g_open = false;
// Snapshot of PauseGameWhileOpen for the lifetime of one F10 panel session.
// Apply/Reload may change the configured value, but must never start or stop
// the gameplay timer freeze while the panel is already open.
bool g_pauseThisUiSession = false;
// A physical toggle-key press can be observed through both Win32 messages and
// render-thread polling. Keep a shared press latch so the two input paths claim
// the same press instead of toggling the panel twice. The latch is released only
// after the key is observed up again.
std::atomic_bool g_togglePressLatched{ false };
std::atomic_bool g_toggleRequested{ false };
std::atomic_bool g_loggedWin32ToggleFallback{ false };
std::atomic_bool g_loggedBeginSceneFailure{ false };
ZachFixConfig g_pending{};
char g_status[192] = "F10 opens this panel.";

using GetCursorPosFn = BOOL (WINAPI*)(LPPOINT);
using SetCursorPosFn = BOOL (WINAPI*)(int, int);
using GetAsyncKeyStateFn = SHORT (WINAPI*)(int);
using GetKeyboardStateFn = BOOL (WINAPI*)(PBYTE);

GetCursorPosFn g_originalGetCursorPos = nullptr;
SetCursorPosFn g_originalSetCursorPos = nullptr;
GetAsyncKeyStateFn g_originalGetAsyncKeyState = nullptr;
GetKeyboardStateFn g_originalGetKeyboardState = nullptr;


bool IsCallFromGame(void* returnAddress)
{
    if (!g_mainExeInfoValid && !InitializeMainExeInfo())
        return false;

    const uintptr_t address = reinterpret_cast<uintptr_t>(returnAddress);
    return address >= g_mainExeBase &&
           address < g_mainExeBase + g_mainExeSize;
}

bool GetGameClientCenterInScreen(POINT* point)
{
    if (!point || !g_window)
        return false;

    RECT client = {};
    if (!GetClientRect(g_window, &client))
        return false;

    POINT center = {
        (client.left + client.right) / 2,
        (client.top + client.bottom) / 2
    };

    if (!ClientToScreen(g_window, &center))
        return false;

    *point = center;
    return true;
}

BOOL WINAPI HookGetCursorPos(LPPOINT point)
{
    if (g_open && IsCallFromGame(_ReturnAddress()))
    {
        if (!point)
            return FALSE;

        if (GetGameClientCenterInScreen(point))
            return TRUE;
    }

    return g_originalGetCursorPos
        ? g_originalGetCursorPos(point)
        : FALSE;
}

BOOL WINAPI HookSetCursorPos(int x, int y)
{
    if (g_open && IsCallFromGame(_ReturnAddress()))
        return TRUE;

    return g_originalSetCursorPos
        ? g_originalSetCursorPos(x, y)
        : FALSE;
}

SHORT WINAPI HookGetAsyncKeyState(int key)
{
    if (g_open && IsCallFromGame(_ReturnAddress()))
        return 0;

    return g_originalGetAsyncKeyState
        ? g_originalGetAsyncKeyState(key)
        : 0;
}

BOOL WINAPI HookGetKeyboardState(PBYTE keyState)
{
    if (g_open && IsCallFromGame(_ReturnAddress()))
    {
        if (!keyState)
            return FALSE;

        ZeroMemory(keyState, 256);
        return TRUE;
    }

    return g_originalGetKeyboardState
        ? g_originalGetKeyboardState(keyState)
        : FALSE;
}

bool InstallUiInputIsolationHooks()
{
    if (!InitializeMainExeInfo())
    {
        AppendLog("[UI] WARNING: DP.exe info unavailable; input isolation disabled.\n");
        return false;
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32)
        user32 = LoadLibraryW(L"user32.dll");

    if (!user32)
    {
        AppendLog("[UI] WARNING: user32.dll unavailable; input isolation disabled.\n");
        return false;
    }

    struct HookSpec
    {
        const char* name;
        void* detour;
        void** original;
    };

    HookSpec specs[] = {
        {"GetCursorPos", reinterpret_cast<void*>(&HookGetCursorPos),
         reinterpret_cast<void**>(&g_originalGetCursorPos)},
        {"SetCursorPos", reinterpret_cast<void*>(&HookSetCursorPos),
         reinterpret_cast<void**>(&g_originalSetCursorPos)},
        {"GetAsyncKeyState", reinterpret_cast<void*>(&HookGetAsyncKeyState),
         reinterpret_cast<void**>(&g_originalGetAsyncKeyState)},
        {"GetKeyboardState", reinterpret_cast<void*>(&HookGetKeyboardState),
         reinterpret_cast<void**>(&g_originalGetKeyboardState)},
    };

    for (const HookSpec& spec : specs)
    {
        FARPROC proc = GetProcAddress(user32, spec.name);
        if (!proc)
        {
            char text[192] = {};
            sprintf_s(text, "[UI] WARNING: %s not found; input isolation incomplete.\n", spec.name);
            AppendLog(text);
            continue;
        }

        const MH_STATUS createStatus = MH_CreateHook(
            reinterpret_cast<void*>(proc), spec.detour, spec.original);

        if (createStatus != MH_OK &&
            createStatus != MH_ERROR_ALREADY_CREATED)
        {
            char text[192] = {};
            sprintf_s(text, "[UI] WARNING: MH_CreateHook(%s) failed: %d.\n",
                      spec.name, static_cast<int>(createStatus));
            AppendLog(text);
            continue;
        }

        const MH_STATUS enableStatus = MH_EnableHook(
            reinterpret_cast<void*>(proc));

        if (enableStatus != MH_OK &&
            enableStatus != MH_ERROR_ENABLED)
        {
            char text[192] = {};
            sprintf_s(text, "[UI] WARNING: MH_EnableHook(%s) failed: %d.\n",
                      spec.name, static_cast<int>(enableStatus));
            AppendLog(text);
        }
    }

    AppendLog("[UI] Game mouse/keyboard isolation hooks installed.\n");
    return true;
}

void OnUiOpenStateChanged(bool open)
{
    if (open)
    {
        // DP continuously recenters its cursor. The SetCursorPos hook suppresses
        // that while the panel is open; releasing capture/clip makes the actual
        // Windows cursor free for the ImGui backend.
        ReleaseCapture();
        ClipCursor(nullptr);

        // Latch the configured policy once for this panel session. This keeps
        // graphics Apply/Reload from unexpectedly enabling the gameplay timer
        // freeze in the middle of a cutscene just because the INI/editor value
        // changed while F10 was already open.
        g_pauseThisUiSession = g_config.pauseGameWhileUiOpen;
        SetGameplayPauseActive(g_pauseThisUiSession);
        return;
    }

    // Always release a pause owned by the closing panel. The configured value
    // may have changed since opening; that new value is intentionally deferred
    // until the next F10 session.
    SetGameplayPauseActive(false);
    g_pauseThisUiSession = false;
}

bool IsKeyboardMessage(UINT msg)
{
    return msg == WM_KEYDOWN || msg == WM_KEYUP ||
           msg == WM_SYSKEYDOWN || msg == WM_SYSKEYUP ||
           msg == WM_CHAR;
}

bool IsMouseMessage(UINT msg)
{
    return (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) ||
           msg == WM_NCMOUSEMOVE;
}

bool ParseBoolValue(const wchar_t* value, bool fallback)
{
    if (!value || !value[0]) return fallback;
    if (_wcsicmp(value, L"true") == 0 || wcscmp(value, L"1") == 0 ||
        _wcsicmp(value, L"yes") == 0 || _wcsicmp(value, L"on") == 0) return true;
    if (_wcsicmp(value, L"false") == 0 || wcscmp(value, L"0") == 0 ||
        _wcsicmp(value, L"no") == 0 || _wcsicmp(value, L"off") == 0) return false;
    return fallback;
}

float ReadFloat(const wchar_t* path, const wchar_t* section, const wchar_t* key, float fallback)
{
    wchar_t def[64] = {};
    wchar_t value[64] = {};
    swprintf_s(def, L"%.2f", static_cast<double>(fallback));
    GetPrivateProfileStringW(section, key, def, value, 64, path);
    wchar_t* end = nullptr;
    const double parsed = wcstod(value, &end);
    if (end == value || *end != L'\0' || !std::isfinite(parsed)) return fallback;
    return static_cast<float>(parsed);
}

bool ReadBool(const wchar_t* path, const wchar_t* section, const wchar_t* key, bool fallback)
{
    wchar_t value[32] = {};
    GetPrivateProfileStringW(section, key, fallback ? L"true" : L"false", value, 32, path);
    return ParseBoolValue(value, fallback);
}

TextureDimensionMode ReadTextureDimensionMode(
    const wchar_t* path,
    TextureDimensionMode fallback)
{
    const wchar_t* fallbackText =
        fallback == TextureDimensionMode::Preserve ? L"Preserve" : L"DPFix";
    wchar_t value[32] = {};
    GetPrivateProfileStringW(
        L"Textures", L"DimensionMode", fallbackText, value, 32, path);

    if (_wcsicmp(value, L"Preserve") == 0 ||
        _wcsicmp(value, L"NPOT") == 0 ||
        wcscmp(value, L"1") == 0)
    {
        return TextureDimensionMode::Preserve;
    }

    if (_wcsicmp(value, L"DPFix") == 0 ||
        _wcsicmp(value, L"Compatible") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return TextureDimensionMode::DPFix;
    }

    return fallback;
}

GamepadInputProfile ReadGamepadInputProfile(
    const wchar_t* path,
    GamepadInputProfile fallback)
{
    const wchar_t* fallbackText =
        fallback == GamepadInputProfile::Xbox360 ? L"Xbox360" : L"PC";
    wchar_t value[32] = {};
    GetPrivateProfileStringW(
        L"Gamepad", L"InputProfile", fallbackText, value, 32, path);

    if (_wcsicmp(value, L"Xbox360") == 0 ||
        _wcsicmp(value, L"Xbox 360") == 0 ||
        _wcsicmp(value, L"Xbox") == 0 ||
        wcscmp(value, L"1") == 0)
    {
        return GamepadInputProfile::Xbox360;
    }

    if (_wcsicmp(value, L"PC") == 0 ||
        _wcsicmp(value, L"Vanilla") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return GamepadInputProfile::PC;
    }

    return fallback;
}

TextureFilteringMode ReadTextureFilteringMode(
    const wchar_t* path,
    TextureFilteringMode fallback)
{
    const wchar_t* fallbackText = L"Original";
    if (fallback == TextureFilteringMode::Bilinear)
        fallbackText = L"Bilinear";
    else if (fallback == TextureFilteringMode::Anisotropic)
        fallbackText = L"Anisotropic";

    wchar_t value[32] = {};
    GetPrivateProfileStringW(
        L"Filtering", L"Mode", fallbackText, value, 32, path);

    if (_wcsicmp(value, L"Bilinear") == 0 ||
        _wcsicmp(value, L"Linear") == 0 ||
        wcscmp(value, L"1") == 0)
    {
        return TextureFilteringMode::Bilinear;
    }

    if (_wcsicmp(value, L"Anisotropic") == 0 ||
        _wcsicmp(value, L"AF") == 0 ||
        wcscmp(value, L"2") == 0)
    {
        return TextureFilteringMode::Anisotropic;
    }

    if (_wcsicmp(value, L"Original") == 0 ||
        _wcsicmp(value, L"Off") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return TextureFilteringMode::Original;
    }

    return fallback;
}

void ReloadPendingFromIni()
{
    wchar_t path[MAX_PATH] = {};
    if (!GetConfigFilePath(path, MAX_PATH))
    {
        strcpy_s(g_status, "Could not locate ZachFix.ini.");
        return;
    }

    ZachFixConfig next = g_config;
    next.internalWidth = GetPrivateProfileIntW(L"Rendering", L"InternalWidth", next.internalWidth, path);
    next.internalHeight = GetPrivateProfileIntW(L"Rendering", L"InternalHeight", next.internalHeight, path);
    next.internalScale = std::clamp(ReadFloat(path, L"Rendering", L"InternalScale", next.internalScale), 0.25f, 4.0f);
    next.fixPixelOffset = ReadBool(path, L"Rendering", L"FixPixelOffset", next.fixPixelOffset);
    next.shadowScale = std::clamp<UINT>(GetPrivateProfileIntW(L"Shadows", L"Scale", next.shadowScale, path), 1, 8);
    next.improveShadowPrecision = ReadBool(
        path, L"Shadows", L"ImprovePrecision", next.improveShadowPrecision);
    next.reflectionScale = std::clamp<UINT>(GetPrivateProfileIntW(L"Reflections", L"Scale", next.reflectionScale, path), 1, 8);
    next.improveDofResolution = ReadBool(path, L"DepthOfField", L"ImproveResolution", next.improveDofResolution);
    next.additionalDofBlur = std::clamp<UINT>(GetPrivateProfileIntW(L"DepthOfField", L"AdditionalBlur", next.additionalDofBlur, path), 0, 2);
    next.highDetailDistanceScale = std::clamp<UINT>(GetPrivateProfileIntW(L"World", L"HighDetailDistanceScale", next.highDetailDistanceScale, path), 1, 2);
    next.objectActivationDistanceScale = std::clamp<UINT>(GetPrivateProfileIntW(L"World", L"ObjectActivationDistanceScale", next.objectActivationDistanceScale, path), 1, 2);
    next.fixInteriorOcclusionBugs = ReadBool(
        path, L"World", L"FixInteriorOcclusionBugs", next.fixInteriorOcclusionBugs);
    next.enableTextureOverride = ReadBool(path, L"Textures", L"EnableOverride", next.enableTextureOverride);
    next.textureDeveloperMode = ReadBool(path, L"Textures", L"DeveloperMode", next.textureDeveloperMode);
    next.dumpTextures = ReadBool(path, L"Textures", L"DumpTextures", next.dumpTextures);
    next.textureDimensionMode = ReadTextureDimensionMode(path, next.textureDimensionMode);
    next.textureFilteringMode = ReadTextureFilteringMode(path, next.textureFilteringMode);
    next.maxAnisotropy = std::clamp<UINT>(
        GetPrivateProfileIntW(L"Filtering", L"MaxAnisotropy", next.maxAnisotropy, path),
        2,
        16);
    next.pauseGameWhileUiOpen = ReadBool(
        path, L"UI", L"PauseGameWhileOpen", next.pauseGameWhileUiOpen);
    next.gamepadInputProfile = ReadGamepadInputProfile(
        path, next.gamepadInputProfile);
    next.analogVehicleTriggers = ReadBool(
        path,
        L"Gamepad",
        L"AnalogVehicleTriggers",
        next.analogVehicleTriggers);
    next.vehicleTriggerDeadzone = std::clamp<UINT>(
        GetPrivateProfileIntW(
            L"Gamepad",
            L"VehicleTriggerDeadzone",
            next.vehicleTriggerDeadzone,
            path),
        0,
        254);
    next.vibrationEnabled = ReadBool(
        path, L"Gamepad", L"Vibration", next.vibrationEnabled);
    next.vibrationStrength = std::clamp(
        ReadFloat(path, L"Gamepad", L"VibrationStrength", next.vibrationStrength),
        0.0f,
        1.0f);
    next.dynamicGlyphAtlas = ReadBool(
        path, L"Glyphs", L"DynamicAtlas", next.dynamicGlyphAtlas);
    next.glyphHotReload = ReadBool(
        path, L"Glyphs", L"HotReload", next.glyphHotReload);
    wchar_t keyboardGlyphSet[64] = {};
    wcscpy_s(keyboardGlyphSet, next.keyboardGlyphSet);
    GetPrivateProfileStringW(
        L"Glyphs",
        L"KeyboardSet",
        keyboardGlyphSet,
        next.keyboardGlyphSet,
        static_cast<DWORD>(sizeof(next.keyboardGlyphSet) / sizeof(next.keyboardGlyphSet[0])),
        path);
    wchar_t gamepadGlyphSet[64] = {};
    wcscpy_s(gamepadGlyphSet, next.gamepadGlyphSet);
    GetPrivateProfileStringW(
        L"Glyphs",
        L"GamepadSet",
        gamepadGlyphSet,
        next.gamepadGlyphSet,
        static_cast<DWORD>(sizeof(next.gamepadGlyphSet) / sizeof(next.gamepadGlyphSet[0])),
        path);

    g_pending = next;
    ReloadPostFxConfigFromIni();
    strcpy_s(g_status, "Reloaded editable + PostFX settings from ZachFix.ini.");
}

void ApplyLiveSettings(IDirect3DDevice9* device)
{
    // Shadow depth format is selected when the game's shadow textures are
    // created. Preserve the editor value across live Apply so it can still be
    // saved to INI for the next launch.
    const bool pendingShadowPrecision = g_pending.improveShadowPrecision;
    const bool pendingPauseWhileOpen = g_pending.pauseGameWhileUiOpen;
    const GamepadInputProfile pendingGamepadInputProfile =
        g_pending.gamepadInputProfile;
    const bool pendingAnalogVehicleTriggers =
        g_pending.analogVehicleTriggers;
    const UINT pendingVehicleTriggerDeadzone =
        std::clamp<UINT>(g_pending.vehicleTriggerDeadzone, 0, 254);
    const bool pendingVibrationEnabled = g_pending.vibrationEnabled;
    const float pendingVibrationStrength =
        std::clamp(g_pending.vibrationStrength, 0.0f, 1.0f);
    const bool pendingDynamicGlyphAtlas = g_pending.dynamicGlyphAtlas;
    const bool pendingGlyphHotReload = g_pending.glyphHotReload;
    wchar_t pendingKeyboardGlyphSet[64] = {};
    wchar_t pendingGamepadGlyphSet[64] = {};
    wcscpy_s(pendingKeyboardGlyphSet, g_pending.keyboardGlyphSet);
    wcscpy_s(pendingGamepadGlyphSet, g_pending.gamepadGlyphSet);
    const bool shadowPrecisionNeedsRestart =
        pendingShadowPrecision != g_config.improveShadowPrecision;
    const bool dynamicGlyphAtlasNeedsRestart =
        pendingDynamicGlyphAtlas != g_config.dynamicGlyphAtlas;

    if (!ApplyRuntimeRenderSettings(
            device,
            g_pending,
            g_status,
            sizeof(g_status)))
    {
        return;
    }

    if (!ApplyGlyphThemeSettings(
            g_config.dynamicGlyphAtlas,
            pendingGlyphHotReload,
            pendingKeyboardGlyphSet,
            pendingGamepadGlyphSet))
    {
        strcpy_s(g_status, "Render settings applied, but glyph theme settings were rejected.");
    }

    ApplyGamepadInputProfile(pendingGamepadInputProfile);
    ApplyAnalogVehicleTriggers(pendingAnalogVehicleTriggers);
    ApplyVehicleTriggerDeadzone(pendingVehicleTriggerDeadzone);

    ApplyNativeVibrationSettings(
        pendingVibrationEnabled,
        pendingVibrationStrength);

    // Keep the editor synchronized with the values that were actually committed.
    g_config.pauseGameWhileUiOpen = pendingPauseWhileOpen;
    g_pending = g_config;
    g_pending.improveShadowPrecision = pendingShadowPrecision;
    g_pending.dynamicGlyphAtlas = pendingDynamicGlyphAtlas;

    // PauseGameWhileOpen is a panel-session policy, not a graphics hot-apply
    // setting. If F10 is already open, keep the current session exactly as it
    // started. The new value takes effect on the next open. This avoids a
    // mid-cutscene Apply/Reload from suddenly enabling the timer freeze.
    if (g_open && g_pauseThisUiSession != g_config.pauseGameWhileUiOpen)
    {
        strcpy_s(g_status,
                 "Live settings applied. Gameplay pause change will take effect next time F10 is opened.");
    }
    if (dynamicGlyphAtlasNeedsRestart)
    {
        strcpy_s(g_status,
                 "Live settings applied. Dynamic Glyph Atlas enable/disable requires restart; Save to INI to persist it.");
    }

    if (shadowPrecisionNeedsRestart)
        strcpy_s(g_status, "Live settings applied. Shadow precision change requires restart.");
}

bool IsPowerOfTwo(UINT value)
{
    return value != 0 && (value & (value - 1u)) == 0;
}

const char* ImageFileFormatName(UINT format)
{
    switch (format)
    {
    case 0: return "BMP";
    case 1: return "JPG";
    case 2: return "TGA";
    case 3: return "PNG";
    case 4: return "DDS";
    case 5: return "PPM";
    case 6: return "DIB";
    case 7: return "HDR";
    case 8: return "PFM";
    default: return "?";
    }
}

const char* D3DFormatName(UINT format)
{
    switch (static_cast<D3DFORMAT>(format))
    {
    case D3DFMT_UNKNOWN: return "UNKNOWN";
    case D3DFMT_R8G8B8: return "R8G8B8";
    case D3DFMT_A8R8G8B8: return "A8R8G8B8";
    case D3DFMT_X8R8G8B8: return "X8R8G8B8";
    case D3DFMT_R5G6B5: return "R5G6B5";
    case D3DFMT_A1R5G5B5: return "A1R5G5B5";
    case D3DFMT_A4R4G4B4: return "A4R4G4B4";
    case D3DFMT_A8: return "A8";
    case D3DFMT_A8L8: return "A8L8";
    case D3DFMT_DXT1: return "DXT1";
    case D3DFMT_DXT2: return "DXT2";
    case D3DFMT_DXT3: return "DXT3";
    case D3DFMT_DXT4: return "DXT4";
    case D3DFMT_DXT5: return "DXT5";
    default: return nullptr;
    }
}

void FormatDimensionRequest(UINT value, char* text, size_t textSize)
{
    if (value == 0xffffffffu)
        strcpy_s(text, textSize, "DEFAULT");
    else if (value == 0xfffffffeu)
        strcpy_s(text, textSize, "DEFAULT_NONPOW2");
    else
        sprintf_s(text, textSize, "%u", value);
}

void FormatMipRequest(UINT value, char* text, size_t textSize)
{
    if (value == 0xffffffffu)
        strcpy_s(text, textSize, "DEFAULT");
    else
        sprintf_s(text, textSize, "%u", value);
}

void DrawImageInfoLine(const char* label, const TextureImageInfo& info)
{
    if (!info.valid)
    {
        ImGui::Text("%s: unavailable", label);
        return;
    }

    const char* formatName = D3DFormatName(info.format);
    if (formatName)
    {
        ImGui::Text("%s: %u x %u  mips=%u  %s  file=%s",
                    label, info.width, info.height, info.mipLevels,
                    formatName, ImageFileFormatName(info.fileFormat));
    }
    else
    {
        ImGui::Text("%s: %u x %u  mips=%u  format=0x%08X  file=%s",
                    label, info.width, info.height, info.mipLevels,
                    info.format, ImageFileFormatName(info.fileFormat));
    }

    ImGui::SameLine();
    ImGui::TextDisabled("[%s]", IsPowerOfTwo(info.width) && IsPowerOfTwo(info.height) ? "POT" : "NPOT");
}

void DrawGpuInfoLine(const TextureGpuInfo& info)
{
    if (!info.valid)
    {
        ImGui::Text("Loaded GPU: unavailable");
        return;
    }

    const char* formatName = D3DFormatName(info.format);
    if (formatName)
    {
        ImGui::Text("Loaded GPU: %u x %u  mips=%u  %s",
                    info.width, info.height, info.mipLevels, formatName);
    }
    else
    {
        ImGui::Text("Loaded GPU: %u x %u  mips=%u  format=0x%08X",
                    info.width, info.height, info.mipLevels, info.format);
    }

    ImGui::SameLine();
    ImGui::TextDisabled("[%s]", IsPowerOfTwo(info.width) && IsPowerOfTwo(info.height) ? "POT" : "NPOT");
}

void DrawTextureInspectionRecord(const char* title, const TextureInspectionRecord& record)
{
    if (!record.valid)
    {
        ImGui::TextDisabled("%s: no texture observed yet.", title);
        return;
    }

    const char* entryPoint =
        record.entryPoint == TextureLoadEntryPoint::InMemoryEx ? "InMemoryEx" :
        record.entryPoint == TextureLoadEntryPoint::InMemory ? "InMemory" : "unknown";

    ImGui::Text("%s: %08X  [%s]", title, record.hash, entryPoint);
    DrawImageInfoLine("Original source", record.sourceImage);

    char gameWidth[32] = {};
    char gameHeight[32] = {};
    char gameMips[32] = {};
    FormatDimensionRequest(record.gameRequestedWidth, gameWidth, sizeof(gameWidth));
    FormatDimensionRequest(record.gameRequestedHeight, gameHeight, sizeof(gameHeight));
    FormatMipRequest(record.gameRequestedMipLevels, gameMips, sizeof(gameMips));

    const char* gameFormatName = D3DFormatName(record.gameRequestedFormat);
    if (gameFormatName)
    {
        ImGui::TextDisabled("Game request: %s x %s  mips=%s  %s",
                            gameWidth, gameHeight, gameMips, gameFormatName);
    }
    else
    {
        ImGui::TextDisabled("Game request: %s x %s  mips=%s  format=0x%08X",
                            gameWidth, gameHeight, gameMips, record.gameRequestedFormat);
    }

    if (record.usedOverride)
    {
        char loadWidth[32] = {};
        char loadHeight[32] = {};
        FormatDimensionRequest(record.loadRequestedWidth, loadWidth, sizeof(loadWidth));
        FormatDimensionRequest(record.loadRequestedHeight, loadHeight, sizeof(loadHeight));
        const char* dimensionMode =
            record.loadRequestedWidth == 0xfffffffeu &&
            record.loadRequestedHeight == 0xfffffffeu
                ? "Preserve"
                : "DPFix-compatible";
        ImGui::TextDisabled(
            "Override load request: %s x %s (%s)",
            loadWidth, loadHeight, dimensionMode);
    }

    if (record.usedOverride)
    {
        const char* pathName =
            record.overridePath == TextureOverridePath::LegacyDPFix ? "legacy dpfix\\tex_override" :
            record.overridePath == TextureOverridePath::PreReleaseDPFixNG ? "DPFixNG\\textures\\override (pre-release)" :
            record.overridePath == TextureOverridePath::ZachFix ? "ZachFix\\textures\\override" : "unknown";
        ImGui::Text("Override: active (%s)", pathName);
        DrawImageInfoLine("Override file", record.overrideImage);

        if (record.sourceImage.valid && record.overrideImage.valid &&
            record.sourceImage.width != 0 && record.sourceImage.height != 0)
        {
            const double scaleX = static_cast<double>(record.overrideImage.width) /
                                  static_cast<double>(record.sourceImage.width);
            const double scaleY = static_cast<double>(record.overrideImage.height) /
                                  static_cast<double>(record.sourceImage.height);
            ImGui::TextDisabled("Override/source scale: %.3fx x %.3fx", scaleX, scaleY);
        }
    }
    else
    {
        ImGui::TextDisabled("Override: not used for this load");
    }

    DrawGpuInfoLine(record.loadedTexture);

    const TextureImageInfo& loadedSource =
        record.usedOverride && record.overrideImage.valid ? record.overrideImage : record.sourceImage;
    if (loadedSource.valid && record.loadedTexture.valid &&
        loadedSource.width != 0 && loadedSource.height != 0)
    {
        const double scaleX = static_cast<double>(record.loadedTexture.width) /
                              static_cast<double>(loadedSource.width);
        const double scaleY = static_cast<double>(record.loadedTexture.height) /
                              static_cast<double>(loadedSource.height);
        ImGui::TextDisabled("File -> GPU scale: %.3fx x %.3fx", scaleX, scaleY);
    }

    if (record.usedOverride && record.overrideImage.valid && record.loadedTexture.valid)
    {
        const bool dimensionsChanged =
            record.overrideImage.width != record.loadedTexture.width ||
            record.overrideImage.height != record.loadedTexture.height;
        const bool preserveRequested =
            record.loadRequestedWidth == 0xfffffffeu &&
            record.loadRequestedHeight == 0xfffffffeu;

        if (dimensionsChanged)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.72f, 0.20f, 1.0f),
                "D3DX resized override: %u x %u -> %u x %u",
                record.overrideImage.width,
                record.overrideImage.height,
                record.loadedTexture.width,
                record.loadedTexture.height);
            ImGui::TextDisabled(
                preserveRequested
                    ? "Preserve was requested, but the device/D3DX did not keep the exact dimensions."
                    : "DPFix mode permits POT rounding. Try Dimension Mode = Preserve for true 2x NPOT assets.");
        }
        else if (preserveRequested &&
                 (!IsPowerOfTwo(record.overrideImage.width) ||
                  !IsPowerOfTwo(record.overrideImage.height)))
        {
            ImGui::TextDisabled("Preserve confirmed: NPOT override reached the GPU unchanged.");
        }
    }
}

bool WideGlyphNameToUtf8(const wchar_t* value, char* output, size_t outputCount)
{
    if (output == nullptr || outputCount == 0)
        return false;

    output[0] = '\0';
    if (value == nullptr || value[0] == L'\0')
        return true;

    const int written = WideCharToMultiByte(
        CP_UTF8,
        0,
        value,
        -1,
        output,
        static_cast<int>(outputCount),
        nullptr,
        nullptr);
    if (written <= 0)
    {
        strcpy_s(output, outputCount, "<invalid name>");
        return false;
    }

    return true;
}

bool DrawGlyphThemeCombo(
    const char* label,
    bool gamepad,
    wchar_t* selectedSet,
    size_t selectedSetCount)
{
    if (label == nullptr || selectedSet == nullptr || selectedSetCount == 0)
        return false;

    char preview[256] = {};
    WideGlyphNameToUtf8(selectedSet, preview, sizeof(preview));

    bool changed = false;
    if (!ImGui::BeginCombo(label, preview))
        return false;

    const GlyphThemeList themes = GetGlyphThemeList(gamepad);
    bool currentFound = false;
    for (UINT i = 0; i < themes.count; ++i)
    {
        if (_wcsicmp(themes.names[i], selectedSet) == 0)
        {
            currentFound = true;
            break;
        }
    }

    if (!currentFound)
    {
        ImGui::TextDisabled("Configured set '%s' is not present; fallback is active.", preview);
        ImGui::Separator();
    }

    for (UINT i = 0; i < themes.count; ++i)
    {
        char item[256] = {};
        WideGlyphNameToUtf8(themes.names[i], item, sizeof(item));
        const bool selected = _wcsicmp(themes.names[i], selectedSet) == 0;
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::Selectable(item, selected))
        {
            wcscpy_s(selectedSet, selectedSetCount, themes.names[i]);
            changed = true;
        }
        if (selected)
            ImGui::SetItemDefaultFocus();
        ImGui::PopID();
    }

    ImGui::EndCombo();
    return changed;
}


void ApplyPendingVehicleTriggerDeadzoneImmediate()
{
    g_pending.vehicleTriggerDeadzone =
        std::clamp<UINT>(g_pending.vehicleTriggerDeadzone, 0, 254);
    ApplyVehicleTriggerDeadzone(g_pending.vehicleTriggerDeadzone);

    sprintf_s(
        g_status,
        sizeof(g_status),
        "Vehicle trigger deadzone %u raw counts (live). Xbox 360 default is 30. Save to INI to persist.",
        g_config.vehicleTriggerDeadzone);
}

void ApplyPendingGamepadInputSettingsImmediate()
{
    ApplyGamepadInputProfile(g_pending.gamepadInputProfile);
    ApplyAnalogVehicleTriggers(g_pending.analogVehicleTriggers);

    // Keep Apply from later replacing these immediate values with stale ones.
    g_pending.gamepadInputProfile = g_config.gamepadInputProfile;
    g_pending.analogVehicleTriggers = g_config.analogVehicleTriggers;

    sprintf_s(
        g_status,
        sizeof(g_status),
        g_config.nativeXInputEnabled
            ? "Gamepad profile %s; analog vehicle triggers %s (live). Save to INI to persist."
            : "Gamepad profile %s; analog vehicle triggers %s. Native XInput is disabled this session; save to INI to persist.",
        g_config.gamepadInputProfile == GamepadInputProfile::Xbox360
            ? "Xbox 360"
            : "PC",
        g_config.analogVehicleTriggers ? "enabled" : "disabled");
}

void ApplyPendingVibrationSettingsImmediate()
{
    g_pending.vibrationStrength =
        std::clamp(g_pending.vibrationStrength, 0.0f, 1.0f);

    const bool available = IsNativeVibrationAvailable();
    ApplyNativeVibrationSettings(
        g_pending.vibrationEnabled,
        g_pending.vibrationStrength);

    // Keep Apply from later replacing these immediate values with stale ones.
    g_pending.vibrationEnabled = g_config.vibrationEnabled;
    g_pending.vibrationStrength = g_config.vibrationStrength;

    if (available)
    {
        sprintf_s(
            g_status,
            sizeof(g_status),
            "Vibration %s, strength %.2fx (live). Save to INI to persist.",
            g_config.vibrationEnabled ? "enabled" : "disabled",
            static_cast<double>(g_config.vibrationStrength));
    }
    else
    {
        strcpy_s(
            g_status,
            "Vibration preference updated, but native XInput rumble is inactive this session. Save to INI to persist.");
    }
}

bool ApplyPendingGlyphSettingsImmediate()
{
    // DynamicAtlas itself is restart-only because it decides texture ownership
    // during the original D3DX loads. Preserve that pending editor value while
    // applying only the live-safe theme/hot-reload settings.
    const bool pendingDynamicGlyphAtlas = g_pending.dynamicGlyphAtlas;

    if (!ApplyGlyphThemeSettings(
            g_config.dynamicGlyphAtlas,
            g_pending.glyphHotReload,
            g_pending.keyboardGlyphSet,
            g_pending.gamepadGlyphSet))
    {
        g_pending.glyphHotReload = g_config.glyphHotReload;
        wcscpy_s(g_pending.keyboardGlyphSet, g_config.keyboardGlyphSet);
        wcscpy_s(g_pending.gamepadGlyphSet, g_config.gamepadGlyphSet);
        g_pending.dynamicGlyphAtlas = pendingDynamicGlyphAtlas;
        strcpy_s(g_status, "Glyph theme settings were rejected; previous live settings kept.");
        return false;
    }

    g_pending.glyphHotReload = g_config.glyphHotReload;
    wcscpy_s(g_pending.keyboardGlyphSet, g_config.keyboardGlyphSet);
    wcscpy_s(g_pending.gamepadGlyphSet, g_config.gamepadGlyphSet);
    g_pending.dynamicGlyphAtlas = pendingDynamicGlyphAtlas;
    strcpy_s(
        g_status,
        g_config.dynamicGlyphAtlas
            ? "Glyph theme settings applied live. Save to INI to persist them."
            : "Glyph theme preferences updated. Dynamic Glyph Atlas is inactive this session; Save to INI for the next start.");
    return true;
}

void DrawSettingsTab()
{
    ImGui::TextUnformatted("Rendering");
    ImGui::SliderFloat("Internal Scale", &g_pending.internalScale, 0.50f, 4.00f, "%.2fx");

    const UINT previewWidth = static_cast<UINT>(static_cast<double>(g_displayWidth) * g_pending.internalScale + 0.5);
    const UINT previewHeight = static_cast<UINT>(static_cast<double>(g_displayHeight) * g_pending.internalScale + 0.5);
    if (g_pending.internalWidth != 0 || g_pending.internalHeight != 0)
        ImGui::TextDisabled("Explicit InternalWidth/Height is active; Internal Scale is not currently used.");
    else
        ImGui::TextDisabled("Live target: %u x %u", previewWidth, previewHeight);

    ImGui::Checkbox("Fix Pixel Offset", &g_pending.fixPixelOffset);
    ImGui::SameLine();
    ImGui::TextDisabled("(live on Apply)");

    ImGui::Spacing();
    ImGui::SeparatorText("Shadows / Reflections");
    int shadowScale = static_cast<int>(g_pending.shadowScale);
    if (ImGui::SliderInt("Shadow Scale", &shadowScale, 1, 8, "%dx"))
        g_pending.shadowScale = static_cast<UINT>(shadowScale);
    ImGui::Checkbox("Improve Shadow Depth Precision", &g_pending.improveShadowPrecision);
    ImGui::SameLine();
    ImGui::TextDisabled("(restart required)");
    if (ImGui::IsItemHovered())
    {
        ImGui::SetTooltip(
            "Original DPFix fix by Peter \"Durante\" Thoman.\n"
            "Uses D32F_LOCKABLE instead of D16 for recognized shadow depth maps.\n"
            "Reduces depth-precision stair-stepping along shadow edges.");
    }
    int reflectionScale = static_cast<int>(g_pending.reflectionScale);
    if (ImGui::SliderInt("Reflection Scale", &reflectionScale, 1, 8, "%dx"))
        g_pending.reflectionScale = static_cast<UINT>(reflectionScale);

    ImGui::Spacing();
    ImGui::SeparatorText("Depth of Field");
    ImGui::Checkbox("Improve DoF Resolution", &g_pending.improveDofResolution);
    ImGui::SameLine();
    ImGui::TextDisabled("(live on Apply)");

    ImGui::TextUnformatted("Additional DoF Blur");
    ImGui::SameLine();

    auto drawDofBlurChoice = [&](const char* label, UINT value)
    {
        const bool selected = g_pending.additionalDofBlur == value;
        if (!ImGui::RadioButton(label, selected))
            return;

        if (!SetAdditionalDofBlurLive(value))
        {
            strcpy_s(g_status, "Could not change Additional DoF Blur live.");
            return;
        }

        g_pending.additionalDofBlur = value;
        sprintf_s(g_status, sizeof(g_status), "Additional DoF Blur: %s (live).", label);
    };

    drawDofBlurChoice("Off", 0);
    ImGui::SameLine();
    drawDofBlurChoice("Soft", 1);
    ImGui::SameLine();
    drawDofBlurChoice("Stronger", 2);
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate)");
    ImGui::TextDisabled("Softens the game's existing DoF buffer; focus/depth logic is unchanged.");
    if (!g_pending.improveDofResolution && g_pending.additionalDofBlur > 0)
        ImGui::TextDisabled("Usually most useful together with Improve DoF Resolution.");

    ImGui::Spacing();
    ImGui::SeparatorText("World Detail");
    int worldMode = static_cast<int>(g_pending.highDetailDistanceScale - 1);
    const char* worldItems[] = { "Original 2x2 core", "Extended 4x4 ring" };
    if (ImGui::Combo("High Detail Distance", &worldMode, worldItems, 2))
        g_pending.highDetailDistanceScale = static_cast<UINT>(worldMode + 1);
    ImGui::SameLine();
    ImGui::TextDisabled("(live on cell transition after Apply)");

    int activationMode = static_cast<int>(g_pending.objectActivationDistanceScale - 1);
    const char* activationItems[] = { "Original 1000 units", "Extended 2000 units" };
    if (ImGui::Combo("Object Activation Distance", &activationMode, activationItems, 2))
        g_pending.objectActivationDistanceScale = static_cast<UINT>(activationMode + 1);
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate after Apply)");
    ImGui::TextDisabled("Extends DP's native per-object activation radius and reduces visible prop pop-in without expanding streaming cell arrays.");

    ImGui::Checkbox("Fix Interior Occlusion Bugs", &g_pending.fixInteriorOcclusionBugs);
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate after Apply)");
    ImGui::TextDisabled(
        "Bypasses only the confirmed outer-world visibility-volume callsite; normal frustum culling remains native.");

    ImGui::Spacing();
    ImGui::SeparatorText("Texture Filtering");
    int filteringMode = static_cast<int>(g_pending.textureFilteringMode);
    const char* filteringItems[] =
    {
        "Original (game settings)",
        "Bilinear compatibility",
        "Anisotropic (smart)"
    };
    if (ImGui::Combo("Filtering Mode", &filteringMode, filteringItems, 3))
    {
        g_pending.textureFilteringMode =
            filteringMode == 1
                ? TextureFilteringMode::Bilinear
                : filteringMode == 2
                    ? TextureFilteringMode::Anisotropic
                    : TextureFilteringMode::Original;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(live on Apply)");

    int maxAnisotropy = static_cast<int>(g_pending.maxAnisotropy);
    if (g_pending.textureFilteringMode != TextureFilteringMode::Anisotropic)
        ImGui::BeginDisabled();
    if (ImGui::SliderInt("Max Anisotropy", &maxAnisotropy, 2, 16, "%dx"))
        g_pending.maxAnisotropy = static_cast<UINT>(maxAnisotropy);
    if (g_pending.textureFilteringMode != TextureFilteringMode::Anisotropic)
        ImGui::EndDisabled();

    if (g_pending.textureFilteringMode == TextureFilteringMode::Anisotropic)
    {
        ImGui::TextDisabled("Smart AF: mipmapped non-RT 2D textures only; point-sampled assets stay untouched.");
        ImGui::TextDisabled("MINFILTER becomes anisotropic; MAGFILTER and the game's MIPFILTER policy are preserved.");
    }
    else if (g_pending.textureFilteringMode == TextureFilteringMode::Bilinear)
    {
        ImGui::TextDisabled("DPFix-style compatibility fix: POINT/NONE MIN/MIP filtering becomes LINEAR on ordinary 2D textures.");
        ImGui::TextDisabled("Render targets, depth resources and dynamic textures are excluded.");
    }
    else
    {
        ImGui::TextDisabled("No sampler filtering override. ZachFix passes the game's filtering states through unchanged.");
    }

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Textures"))
    {
        ImGui::Indent();
        ImGui::Checkbox("Enable Texture Override", &g_pending.enableTextureOverride);
        ImGui::SameLine();
        ImGui::TextDisabled("(production + developer modes)");

        const bool textureDeveloperModeActive = IsTextureDeveloperModeActive();
        ImGui::Checkbox("Texture Developer Mode", &g_pending.textureDeveloperMode);
        ImGui::SameLine();
        ImGui::TextDisabled("(restart required)");
        ImGui::TextDisabled(
            textureDeveloperModeActive
                ? "Current session: ACTIVE. Logical textures remain original; overrides are substituted at SetTexture."
                : "Current session: OFF. Production DPFix-compatible override path only.");
        if (g_pending.textureDeveloperMode != textureDeveloperModeActive)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.72f, 0.20f, 1.0f),
                "Developer Mode change requires a game restart to switch texture ownership safely.");
        }

        ImGui::Checkbox("Dump Textures", &g_pending.dumpTextures);
        ImGui::SameLine();
        ImGui::TextDisabled("(Developer Mode only; new texture loads)");

        int dimensionMode = static_cast<int>(g_pending.textureDimensionMode);
        const char* dimensionItems[] =
        {
            "DPFix-compatible (POT rounding)",
            "Preserve file dimensions (NPOT)"
        };
        if (ImGui::Combo("Dimension Mode", &dimensionMode, dimensionItems, 2))
        {
            g_pending.textureDimensionMode =
                dimensionMode == 1
                    ? TextureDimensionMode::Preserve
                    : TextureDimensionMode::DPFix;
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(new override loads; hot reload uses applied mode)");
        ImGui::TextDisabled(
            g_pending.textureDimensionMode == TextureDimensionMode::Preserve
                ? "Preserve keeps exact DDS/PNG width and height when the D3D9 device supports NPOT textures."
                : "DPFix mode matches original DPFix: D3DX_DEFAULT may round each dimension up to POT.");

        ImGui::TextDisabled("Hash: DPFix-compatible SuperFastHash over original D3DX source bytes.");
        ImGui::TextDisabled("Override: ZachFix\\textures\\override, then pre-release DPFixNG and legacy dpfix paths.");
        ImGui::TextDisabled("Dump: ZachFix\\textures\\dump\\<hash>.tga");

        const bool pendingTextureSettings =
            g_pending.enableTextureOverride != g_config.enableTextureOverride ||
            g_pending.textureDimensionMode != g_config.textureDimensionMode;
        if (!textureDeveloperModeActive)
            ImGui::BeginDisabled();
        if (ImGui::Button("Reload Overrides"))
        {
            const UINT generation = RequestTextureOverrideHotReload();
            if (generation != 0)
            {
                sprintf_s(
                    g_status,
                    g_config.enableTextureOverride
                        ? "Texture hot reload %u requested; tracked textures rescan on their next bind."
                        : "Texture hot reload %u requested; active replacements will revert to originals on their next bind.",
                    generation);
            }
        }
        if (!textureDeveloperModeActive)
            ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::TextDisabled(
            textureDeveloperModeActive
                ? "(add / edit / remove overrides live)"
                : "(available after restart with Texture Developer Mode enabled)");
        if (pendingTextureSettings)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.72f, 0.20f, 1.0f),
                "Texture settings have unapplied changes. Press Apply before Reload Overrides to use them.");
        }

        ImGui::Unindent();
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Gamepad Input");

    int gamepadProfile =
        g_pending.gamepadInputProfile == GamepadInputProfile::Xbox360 ? 1 : 0;
    const char* gamepadProfileItems[] = {
        "PC (Director's Cut)",
        "Xbox 360"
    };
    if (ImGui::Combo(
            "Gamepad Profile",
            &gamepadProfile,
            gamepadProfileItems,
            2))
    {
        g_pending.gamepadInputProfile = gamepadProfile == 1
            ? GamepadInputProfile::Xbox360
            : GamepadInputProfile::PC;
        ApplyPendingGamepadInputSettingsImmediate();
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate)");
    ImGui::TextDisabled(
        g_pending.gamepadInputProfile == GamepadInputProfile::Xbox360
            ? "Xbox 360 restores the proven stick normalization/filter bypass, aim shaping and LT/RT press threshold while keeping Director's Cut aiming on the right stick."
            : "PC keeps Director's Cut's original stick evaluator, secondary filtering and aim shaping.");

    if (ImGui::Checkbox(
            "Analog Vehicle Triggers",
            &g_pending.analogVehicleTriggers))
    {
        ApplyPendingGamepadInputSettingsImmediate();
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate)");
    ImGui::TextDisabled(
        "Independent of Gamepad Profile. On restores all three proven Xbox 360 LT/RT vehicle consumers; Off uses vanilla PC digital throttle/brake.");

    ImGui::Spacing();
    ImGui::SeparatorText("Vehicle Controls");

    int vehicleTriggerDeadzone =
        static_cast<int>(g_pending.vehicleTriggerDeadzone);
    if (ImGui::SliderInt(
            "Vehicle Trigger Deadzone",
            &vehicleTriggerDeadzone,
            0,
            254,
            "%d raw"))
    {
        g_pending.vehicleTriggerDeadzone =
            static_cast<UINT>(vehicleTriggerDeadzone);
        ApplyPendingVehicleTriggerDeadzoneImmediate();
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate)");
    ImGui::TextDisabled(
        "Xbox 360 default: 30. Raw values <= deadzone are zero; values above it keep raw/255 scaling.");

    ImGui::Spacing();
    ImGui::SeparatorText("Gamepad Vibration");

    bool vibrationChanged = false;
    if (ImGui::Checkbox("Vibration", &g_pending.vibrationEnabled))
        vibrationChanged = true;
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate)");

    if (ImGui::SliderFloat(
            "Vibration Strength",
            &g_pending.vibrationStrength,
            0.0f,
            1.0f,
            "%.2fx"))
    {
        vibrationChanged = true;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(immediate)");

    if (vibrationChanged)
        ApplyPendingVibrationSettingsImmediate();

    if (IsNativeVibrationAvailable())
    {
        ImGui::TextDisabled(
            "Restores DP's native two-channel rumble timing and amplitudes through XInput.");
    }
    else if (!g_config.nativeXInputEnabled)
    {
        ImGui::TextDisabled(
            "Native XInput is disabled for this session; vibration settings will take effect when it is enabled on restart.");
    }
    else
    {
        ImGui::TextDisabled(
            "Native rumble is unavailable for this executable/XInput provider; the preference can still be saved.");
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Glyph Themes");

    bool glyphSettingsChanged = false;
    ImGui::Checkbox("Dynamic Glyph Atlas", &g_pending.dynamicGlyphAtlas);
    ImGui::SameLine();
    ImGui::TextDisabled("(restart required to enable/disable)");
    if (g_pending.dynamicGlyphAtlas != g_config.dynamicGlyphAtlas)
    {
        ImGui::TextColored(
            ImVec4(1.0f, 0.72f, 0.20f, 1.0f),
            "Dynamic Glyph Atlas will change after restart. Save to INI to persist it.");
    }

    if (ImGui::Checkbox("Glyph Theme Hot Reload", &g_pending.glyphHotReload))
        glyphSettingsChanged = true;
    ImGui::SameLine();
    ImGui::TextDisabled("(checks active theme files about twice per second)");

    if (DrawGlyphThemeCombo(
            "Keyboard Theme",
            false,
            g_pending.keyboardGlyphSet,
            sizeof(g_pending.keyboardGlyphSet) / sizeof(g_pending.keyboardGlyphSet[0])))
    {
        glyphSettingsChanged = true;
    }

    if (DrawGlyphThemeCombo(
            "Gamepad Theme",
            true,
            g_pending.gamepadGlyphSet,
            sizeof(g_pending.gamepadGlyphSet) / sizeof(g_pending.gamepadGlyphSet[0])))
    {
        glyphSettingsChanged = true;
    }

    if (glyphSettingsChanged)
        ApplyPendingGlyphSettingsImmediate();

    ImGui::TextDisabled("Themes: ZachFix\\glyphs\\keyboard and ZachFix\\glyphs\\gamepad (.dds/.png/.tga).");
    ImGui::TextDisabled("Native prefers DP's captured atlas; native.* can supply a family DP never loaded.");
    ImGui::TextDisabled("Missing gamepad sets fall back to xbox, then a captured native atlas when available.");
    ImGui::TextDisabled("The lists are rescanned while their combo is open; no game restart is required.");

    ImGui::Spacing();
    ImGui::SeparatorText("Gameplay");
    ImGui::Text("Difficulty");
    ImGui::SameLine();
    ImGui::TextDisabled("%s (read-only, -zachfix-difficulty=%u)",
                        GetSessionDifficultyName(),
                        GetSessionDifficultyValue());
    ImGui::TextDisabled("Save profile: savedata\\%s\\dp.sav",
                        GetSessionDifficultyProfileName());
    ImGui::TextDisabled("Difficulty is fixed for the current process; restart with 0=Easy, 1=Normal, or 2=Hard.");

    ImGui::Spacing();
    ImGui::SeparatorText("Tuning Pause");
    ImGui::Checkbox("Pause gameplay when opening F10", &g_pending.pauseGameWhileUiOpen);
    ImGui::SameLine();
    ImGui::TextDisabled("(next F10 session)");
    const GameplayPauseStats pauseStats = GetGameplayPauseStats();
    const char* pauseState =
        !pauseStats.hooksInstalled ? "unavailable" :
        !pauseStats.active ? "ready" : "ACTIVE";
    ImGui::TextDisabled(
        "Original timer-freeze path: %s, hooks=%u.",
        pauseState, pauseStats.installedHooks);
    ImGui::TextDisabled(
        "Game timer calls: QPC=%llu Tick32=%llu Tick64=%llu timeGetTime=%llu.",
        pauseStats.qpcCalls, pauseStats.tick32Calls,
        pauseStats.tick64Calls, pauseStats.timeGetTimeCalls);
    ImGui::TextDisabled(
        "Apply/Reload never changes pause state while this F10 panel is already open.");
    ImGui::TextColored(
        ImVec4(1.0f, 0.72f, 0.20f, 1.0f),
        "Gameplay-only research option: known to hang some cutscenes. Use PostFX Preview Freeze there.");
}

void DrawPostFxRuntimeDiagnostics()
{
    const PostFxStats postFxStats = GetPostFxStats();

    ImGui::TextDisabled(
        "Shared runtime for native AO, exposure, bloom, DoF and tone mapping.");

    if (postFxStats.deviceReady)
    {
        ImGui::Text(
            "Device: ps_%u_%u   MRT %u   max texture %ux%u",
            postFxStats.pixelShaderMajor,
            postFxStats.pixelShaderMinor,
            postFxStats.maxSimultaneousRenderTargets,
            postFxStats.maxTextureWidth,
            postFxStats.maxTextureHeight);
    }
    else
    {
        ImGui::TextDisabled("Device capabilities not captured yet.");
    }

    const double allocatedMiB =
        static_cast<double>(postFxStats.estimatedBytes) / (1024.0 * 1024.0);
    ImGui::Text(
        "Frames: %llu   fullscreen passes: %llu   resets: %llu",
        postFxStats.frameIndex,
        postFxStats.fullscreenPasses,
        postFxStats.resetCount);
    ImGui::Text(
        "Lazy RT pool: %u targets   %.2f MiB   generation %llu",
        postFxStats.allocatedTargets,
        allocatedMiB,
        postFxStats.resourceGeneration);

    if (postFxStats.gbufferCaptured)
    {
        ImGui::Text(
            "G-buffer: %ux%u   frame %llu   %s",
            postFxStats.gbufferWidth,
            postFxStats.gbufferHeight,
            postFxStats.gbufferFrame,
            postFxStats.gbufferFresh ? "fresh" : "stale");
    }
    else
    {
        ImGui::TextDisabled("G-buffer pair not observed yet.");
    }

    ImGui::TextDisabled(
        "Render targets are allocated lazily and recreated per-slot when a hot-applied quality setting changes size/format.");
    ImGui::TextDisabled(
        "A single state-safe fullscreen-pass API will be shared by GTAO-lite, exposure, bloom and DoF.");
}

void DrawPostFxTab()
{
    ImGui::TextDisabled("Live final-composite tuning. Controls apply immediately.");
    ImGui::Spacing();
    ImGui::SeparatorText("Preview Freeze");
    const PostFxDofStats previewFreezeStats = GetPostFxDofStats();
    const char* previewButton = previewFreezeStats.frameFrozen
        ? "Unfreeze PostFX Preview"
        : previewFreezeStats.freezePending
            ? "Cancel Preview Capture"
            : "Freeze PostFX Preview";
    if (ImGui::Button(previewButton))
        TogglePostFxPreviewFreeze();
    ImGui::SameLine();
    if (previewFreezeStats.frameFrozen)
        ImGui::Text("Frozen frame %llu", previewFreezeStats.frozenFrame);
    else if (previewFreezeStats.freezePending)
        ImGui::TextDisabled("capture pending...");
    else
        ImGui::TextDisabled("scene live");
    ImGui::TextWrapped(
        "Captures the current PostFX inputs while the game continues running behind the preview. "
        "AO, DoF, Bloom, Exposure and tone mapping remain live for fine A/B tuning.");
    ImGui::TextDisabled(
        "Useful for cutscenes. Geometry-driven settings such as internal resolution, shadows, reflections and world detail are not part of the frozen preview.");

    ImGui::Spacing();
    if (ImGui::BeginTabBar("PostFxEffectTabs"))
    {
        if (ImGui::BeginTabItem("AO"))
        {
            ImGui::TextDisabled("GTAO-lite v1.2 thickness-aware");

            PostFxAoSettings aoSettings = GetPostFxAoSettings();
            const PostFxAoStats aoStats = GetPostFxAoStats();

            const char* aoModes[] =
            {
                "Off",
                "Show Raw AO",
                "Show Filtered AO",
                "Show AO Enhanced (diagnostic)",
                "Composite (HDR)"
            };
            int aoMode = static_cast<int>(aoSettings.mode);
            if (ImGui::Combo("AO mode", &aoMode, aoModes, 5))
            {
                SetPostFxAoMode(static_cast<PostFxAoMode>(aoMode));
                aoSettings.mode = static_cast<PostFxAoMode>(aoMode);
            }

            float aoRadius = aoSettings.radius;
            if (ImGui::SliderFloat("AO radius", &aoRadius, 0.25f, 32.0f, "%.2f"))
                SetPostFxAoRadius(aoRadius);

            float aoStrength = aoSettings.strength;
            if (ImGui::SliderFloat("AO strength", &aoStrength, 0.0f, 4.0f, "%.2f"))
                SetPostFxAoStrength(aoStrength);

            float aoBias = aoSettings.bias;
            if (ImGui::SliderFloat("AO bias", &aoBias, 0.0f, 0.35f, "%.3f"))
                SetPostFxAoBias(aoBias);

            float aoThickness = aoSettings.thickness;
            if (ImGui::SliderFloat("AO thickness", &aoThickness, 0.05f, 1.00f, "%.2f"))
                SetPostFxAoThickness(aoThickness);

            float aoPower = aoSettings.power;
            if (ImGui::SliderFloat("AO power", &aoPower, 0.25f, 3.0f, "%.2f"))
                SetPostFxAoPower(aoPower);

            const char* aoResolutions[] = { "Full", "Half", "Quarter" };
            int aoResolution =
                aoSettings.resolutionDivisor == 1 ? 0 :
                aoSettings.resolutionDivisor == 4 ? 2 : 1;
            if (ImGui::Combo("AO resolution", &aoResolution, aoResolutions, 3))
            {
                const UINT divisor = aoResolution == 0 ? 1u :
                                     aoResolution == 2 ? 4u : 2u;
                SetPostFxAoResolutionDivisor(divisor);
            }

            if (ImGui::Button("Reset AO##PostFxAo"))
                ResetPostFxAoSettings();

            if (ImGui::CollapsingHeader("Details / diagnostics##PostFxAoDetails"))
            {

                ImGui::TextDisabled(
                    "All AO controls hot-apply on the next final-composite draw. Changing resolution lazily recreates only the AO RTs.");
                ImGui::TextDisabled(
                    "v1.2 uses 4 horizon directions with near/far samples, thickness-aware foreground rejection and quartic distance falloff.");
                ImGui::TextDisabled(
                    "Thickness is a fraction of AO radius: lower values reject foreground silhouette halos more aggressively.");
                ImGui::TextDisabled(
                    "Enhanced is display-only (filtered AO^6) so subtle visibility differences are easier to inspect.");
                ImGui::TextDisabled(
                    "Composite (HDR) is sampled inside the PostFX final-composite replacement before DP grading/exposure. The old LDR multiply is now only a fallback if HDR binding fails.");

                if (aoStats.projectionReady)
                {
                    ImGui::Text(
                        "Projection scale: %.4f x %.4f   frame %llu",
                        aoStats.projectionScaleX,
                        aoStats.projectionScaleY,
                        aoStats.projectionFrame);
                }
                else
                {
                    ImGui::TextDisabled("Projection scale not captured yet.");
                }

                if (aoStats.width != 0 && aoStats.height != 0)
                {
                    ImGui::Text(
                        "AO working size: %ux%u   shader %s   prepared frame %llu",
                        aoStats.width,
                        aoStats.height,
                        aoStats.shaderReady ? "ready" : "not ready",
                        aoStats.preparedFrame);
                }

                if (aoStats.skippedDebugOverride)
                {
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.75f, 0.25f, 1.0f),
                        "AO paused: set Native composite debug view to Vanilla.");
                }
                else if (aoStats.skippedStaleGBuffer)
                {
                    ImGui::TextDisabled("AO paused: G-buffer is stale (expected for FMV/non-3D frames).");
                }
                else if (aoStats.skippedProjection)
                {
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.75f, 0.25f, 1.0f),
                        "AO paused: current-frame projection constants were not captured.");
                }
                else if (aoStats.activeThisFrame)
                {
                    ImGui::TextDisabled("AO prepared from fresh depth + view-space normals.");
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Bloom"))
        {
            ImGui::TextDisabled("Bloom NG v0 pyramid");

            PostFxBloomSettings bloomSettings = GetPostFxBloomSettings();
            const PostFxBloomStats bloomStats = GetPostFxBloomStats();

            const char* bloomModes[] =
            {
                "Legacy",
                "Bloom NG",
                "Show Bloom"
            };
            int bloomMode = static_cast<int>(bloomSettings.mode);
            if (ImGui::Combo("Bloom mode", &bloomMode, bloomModes, 3))
            {
                SetPostFxBloomMode(static_cast<PostFxBloomMode>(bloomMode));
                bloomSettings.mode = static_cast<PostFxBloomMode>(bloomMode);
            }

            float bloomThresholdEv = bloomSettings.thresholdEv;
            if (ImGui::SliderFloat(
                    "Bloom threshold", &bloomThresholdEv, -4.0f, 8.0f, "%+.2f EV"))
            {
                SetPostFxBloomThresholdEv(bloomThresholdEv);
            }

            float bloomSoftKnee = bloomSettings.softKnee;
            if (ImGui::SliderFloat(
                    "Bloom soft knee", &bloomSoftKnee, 0.01f, 1.0f, "%.2f"))
            {
                SetPostFxBloomSoftKnee(bloomSoftKnee);
            }

            float bloomIntensity = bloomSettings.intensity;
            if (ImGui::SliderFloat(
                    "Bloom intensity", &bloomIntensity, 0.0f, 3.0f, "%.2f"))
            {
                SetPostFxBloomIntensity(bloomIntensity);
            }

            float bloomScatter = bloomSettings.scatter;
            if (ImGui::SliderFloat(
                    "Bloom scatter", &bloomScatter, 0.0f, 1.0f, "%.2f"))
            {
                SetPostFxBloomScatter(bloomScatter);
            }

            int bloomLevels = static_cast<int>(bloomSettings.maxLevels);
            if (ImGui::SliderInt("Bloom levels", &bloomLevels, 2, 6))
                SetPostFxBloomMaxLevels(static_cast<UINT>(bloomLevels));

            if (ImGui::Button("Reset Bloom##PostFxBloom"))
                ResetPostFxBloomSettings();

            if (ImGui::CollapsingHeader("Details / diagnostics##PostFxBloomDetails"))
            {

                ImGui::TextDisabled(
                    "Bloom NG uses a soft-knee HDR prefilter, 13-tap progressive downsample and 9-tap tent upsample. All controls hot-apply.");
                ImGui::TextDisabled(
                    "The pyramid starts at half output resolution. Threshold is scene-linear EV, so auto exposure does not move the bright-pass cutoff.");
                ImGui::TextDisabled(
                    "GTAO Composite (HDR) is folded into the bloom prefilter when active; exposure metering intentionally excludes bloom to avoid feedback.");
                ImGui::TextDisabled(
                    "DP's authored g_fBloomForce remains the scene bloom key; Bloom intensity is an additional ZachFix multiplier.");

                if (bloomStats.baseWidth != 0 && bloomStats.baseHeight != 0)
                {
                    ImGui::Text(
                        "Bloom pyramid: %ux%u -> %u levels   source %ux%u   frame %llu",
                        bloomStats.baseWidth,
                        bloomStats.baseHeight,
                        bloomStats.levels,
                        bloomStats.sourceWidth,
                        bloomStats.sourceHeight,
                        bloomStats.preparedFrame);
                }
                ImGui::Text(
                    "Bloom shaders: %s   AO-aware: %s",
                    bloomStats.shaderReady ? "ready" : "lazy",
                    bloomStats.usedAo ? "yes" : "no");
                if (bloomStats.fallbackToLegacy && bloomSettings.mode != PostFxBloomMode::Legacy)
                {
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.75f, 0.25f, 1.0f),
                        "Bloom NG unavailable this frame; final composite fell back to legacy bloom.");
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("DoF"))
        {
            ImGui::TextDisabled("DoF NG v0.2 bokeh");

            PostFxDofSettings dofSettings = GetPostFxDofSettings();
            const PostFxDofStats dofStats = GetPostFxDofStats();

            const char* dofModes[] =
            {
                "Legacy",
                "DoF NG",
                "Show CoC",
                "Show Near",
                "Show Far"
            };
            int dofMode = static_cast<int>(dofSettings.mode);
            if (ImGui::Combo("DoF mode", &dofMode, dofModes, 5))
            {
                SetPostFxDofMode(static_cast<PostFxDofMode>(dofMode));
                dofSettings.mode = static_cast<PostFxDofMode>(dofMode);
            }

            float dofRadius = dofSettings.maxRadiusPixels;
            if (ImGui::SliderFloat("DoF max radius", &dofRadius, 2.0f, 32.0f, "%.1f px"))
                SetPostFxDofMaxRadiusPixels(dofRadius);

            float dofNearStrength = dofSettings.nearStrength;
            if (ImGui::SliderFloat("DoF near strength", &dofNearStrength, 0.0f, 2.0f, "%.2f"))
                SetPostFxDofNearStrength(dofNearStrength);

            float dofFarStrength = dofSettings.farStrength;
            if (ImGui::SliderFloat("DoF far strength", &dofFarStrength, 0.0f, 2.0f, "%.2f"))
                SetPostFxDofFarStrength(dofFarStrength);

            float dofDepthReject = dofSettings.depthReject;
            if (ImGui::SliderFloat("DoF depth rejection", &dofDepthReject, 0.10f, 6.0f, "%.2f"))
                SetPostFxDofDepthReject(dofDepthReject);

            float dofHighlightBoost = dofSettings.highlightBoost;
            if (ImGui::SliderFloat("DoF bokeh highlights", &dofHighlightBoost, 0.0f, 2.0f, "%.2f"))
                SetPostFxDofHighlightBoost(dofHighlightBoost);

            const char* dofResolutions[] = { "Half", "Quarter" };
            int dofResolution = dofSettings.resolutionDivisor >= 4 ? 1 : 0;
            if (ImGui::Combo("DoF resolution", &dofResolution, dofResolutions, 2))
                SetPostFxDofResolutionDivisor(dofResolution == 0 ? 2u : 4u);

            if (ImGui::Button("Reset DoF##PostFxDof"))
                ResetPostFxDofSettings();

            if (ImGui::CollapsingHeader("Details / diagnostics##PostFxDofDetails"))
            {

                ImGui::TextDisabled(
                    "DoF NG keeps DP's authored c15/c16 focus logic and uses separate near/far HDR gather layers. v0.2 uses a fully-unrolled 16-tap rotated disk for rounder bokeh.");
                ImGui::TextDisabled(
                    "Radius is measured in output pixels, so the look is independent of InternalScale. Depth rejection reduces foreground/background bleeding; Bokeh highlights preserves bright defocus discs.");
                ImGui::TextDisabled(
                    "For A/B tuning use PostFX Preview Freeze above; it freezes the whole PostFX input set and keeps these DoF controls live.");
                ImGui::TextDisabled(
                    "Show CoC: red=near, blue=far. Show Near/Far display the two HDR layers after a simple preview compression.");

                if (dofStats.width != 0 && dofStats.height != 0)
                {
                    ImGui::Text(
                        "DoF working size: %ux%u   source %ux%u   frame %llu",
                        dofStats.width, dofStats.height,
                        dofStats.sourceWidth, dofStats.sourceHeight,
                        dofStats.preparedFrame);
                }
                ImGui::Text(
                    "DoF shaders: %s   active: %s",
                    dofStats.shaderReady ? "ready" : "lazy",
                    dofStats.activeThisFrame ? "yes" : "no");
                if (dofStats.fallbackToLegacy && dofSettings.mode != PostFxDofMode::Legacy)
                {
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.75f, 0.25f, 1.0f),
                        "DoF NG unavailable this frame; final composite fell back to legacy DoF.");
                }
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Exposure"))
        {
            ImGui::TextDisabled("Exposure v1 + Filmic Shoulder");

            PostFxExposureSettings exposureSettings = GetPostFxExposureSettings();
            const PostFxExposureStats exposureStats = GetPostFxExposureStats();

            const char* exposureModes[] =
            {
                "Legacy",
                "Auto exposure only",
                "Filmic shoulder only",
                "Auto exposure + shoulder"
            };
            int exposureMode = static_cast<int>(exposureSettings.mode);
            if (ImGui::Combo("PostFX exposure mode", &exposureMode, exposureModes, 4))
            {
                SetPostFxExposureMode(static_cast<PostFxExposureMode>(exposureMode));
                exposureSettings.mode = static_cast<PostFxExposureMode>(exposureMode);
            }

            float compensationEv = exposureSettings.compensationEv;
            if (ImGui::SliderFloat(
                    "Exposure compensation", &compensationEv, -4.0f, 4.0f, "%+.2f EV"))
            {
                SetPostFxExposureCompensationEv(compensationEv);
            }

            float meterMinEv = exposureSettings.meterMinEv;
            if (ImGui::SliderFloat(
                    "Meter minimum luminance", &meterMinEv, -16.0f, 4.0f, "%+.1f EV"))
            {
                SetPostFxExposureMeterMinEv(meterMinEv);
            }

            float meterMaxEv = exposureSettings.meterMaxEv;
            if (ImGui::SliderFloat(
                    "Meter maximum luminance", &meterMaxEv, -4.0f, 16.0f, "%+.1f EV"))
            {
                SetPostFxExposureMeterMaxEv(meterMaxEv);
            }

            float minExposureEv = exposureSettings.minExposureEv;
            if (ImGui::SliderFloat(
                    "Minimum exposure", &minExposureEv, -12.0f, 4.0f, "%+.2f EV"))
            {
                SetPostFxExposureMinEv(minExposureEv);
            }

            float maxExposureEv = exposureSettings.maxExposureEv;
            if (ImGui::SliderFloat(
                    "Maximum exposure", &maxExposureEv, -4.0f, 12.0f, "%+.2f EV"))
            {
                SetPostFxExposureMaxEv(maxExposureEv);
            }

            float brightenSpeed = exposureSettings.brightenSpeed;
            if (ImGui::SliderFloat(
                    "Brighten speed", &brightenSpeed, 0.05f, 8.0f, "%.2f /s"))
            {
                SetPostFxExposureBrightenSpeed(brightenSpeed);
            }

            float darkenSpeed = exposureSettings.darkenSpeed;
            if (ImGui::SliderFloat(
                    "Darken speed", &darkenSpeed, 0.05f, 8.0f, "%.2f /s"))
            {
                SetPostFxExposureDarkenSpeed(darkenSpeed);
            }

            float shoulderStrength = exposureSettings.shoulderStrength;
            if (ImGui::SliderFloat(
                    "Shoulder strength", &shoulderStrength, 0.0f, 1.0f, "%.2f"))
            {
                SetPostFxExposureShoulderStrength(shoulderStrength);
            }

            float whitePoint = exposureSettings.whitePoint;
            if (ImGui::SliderFloat("White point", &whitePoint, 1.05f, 16.0f, "%.2f"))
                SetPostFxExposureWhitePoint(whitePoint);

            if (ImGui::Button("Reset Exposure##PostFxExposure"))
                ResetPostFxExposureSettings();
            ImGui::SameLine();
            if (ImGui::Button("Reset Adaptation##PostFxExposure"))
                RequestPostFxExposureAdaptationReset();

            if (ImGui::CollapsingHeader("Details / diagnostics##PostFxExposureDetails"))
            {

                ImGui::TextDisabled(
                    "Hot apply: v1 meters the FP16 HDR scene before DP's final-composite draw. With GTAO Composite (HDR), metering also sees the AO-modulated HDR scene. HUD/UI still render afterward.");
                ImGui::TextDisabled(
                    "The geometric-mean meter uses log luminance; Meter Min/Max clamp scene luminance before averaging.");
                ImGui::TextDisabled(
                    "DP's g_fExposure (c10) remains the authored exposure key; ZachFix replaces the old adapted-luminance denominator.");
                ImGui::TextDisabled(
                    "Brighten controls adaptation after entering darkness; Darken controls adaptation after entering a brighter scene.");
                ImGui::TextDisabled(
                    "The luminance-preserving shoulder runs after exposed scene + selected bloom. DP DoF/grading are unchanged; Bloom NG can replace the legacy bright-pass path.");

                ImGui::Text(
                    "Exposure shaders: final %s   meter %s   adaptation %s",
                    exposureStats.shaderReady ? "ready" : "lazy",
                    exposureStats.meterShadersReady ? "ready" : "lazy",
                    exposureStats.adaptationInitialized ? "active" : "waiting");

                if (exposureStats.meterWidth != 0 && exposureStats.meterHeight != 0)
                {
                    ImGui::Text(
                        "Meter: %ux%u   frame dt %.2f ms   game key c10 %.4f",
                        exposureStats.meterWidth,
                        exposureStats.meterHeight,
                        exposureStats.frameDeltaMs,
                        exposureStats.gameExposureKey);
                }

                if (exposureStats.telemetryAvailable)
                {
                    ImGui::Text(
                        "Scene log luminance: %+.2f EV   target: %+.2f EV   adapted: %+.2f EV",
                        exposureStats.averageLogLuminance,
                        exposureStats.targetEv,
                        exposureStats.adaptedEv);
                    ImGui::Text(
                        "Exposure gain: %.3fx   last applied frame: %llu",
                        exposureStats.exposureGain,
                        exposureStats.lastAppliedFrame);
                }
                else
                {
                    ImGui::TextDisabled(
                        "1x1 EV telemetry: %s   last applied frame: %llu",
                        exposureStats.telemetryReadbackFailed ? "readback unavailable" : "waiting",
                        exposureStats.lastAppliedFrame);
                }

                if (GetShaderProbeCompositeDebugMode() != ShaderProbeCompositeDebugMode::Vanilla &&
                    exposureSettings.mode != PostFxExposureMode::Legacy)
                {
                    ImGui::TextColored(
                        ImVec4(1.0f, 0.75f, 0.25f, 1.0f),
                        "Exposure replacement paused: Native composite debug view has priority.");
                }
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void DrawDiagnosticsTab()
{
    ImGui::TextDisabled("Runtime counters and developer diagnostics. These controls do not change gameplay settings.");
    ImGui::Spacing();

    if (ImGui::CollapsingHeader("Runtime Resource Audit"))
    {
        ImGui::Indent();
        ImGui::TextDisabled("Hot Apply rebuilds ZachFix render targets between frames; no D3D9 Reset is used.");
        ImGui::TextDisabled("World Detail updates fully on subsequent streaming-cell transitions.");

        const RuntimeResourceStats runtimeStats = GetRuntimeResourceStats();
        const unsigned long long outstanding =
            runtimeStats.replacementCreates >= runtimeStats.replacementReleases
                ? runtimeStats.replacementCreates - runtimeStats.replacementReleases
                : 0;

        ImGui::Text("Generation: %u   Last changed: %u",
                    runtimeStats.generation, runtimeStats.lastChangedResources);
        ImGui::Text("Managed logical: %u   Active replacements: %u",
                    runtimeStats.managedLogicalResources, runtimeStats.activeReplacementResources);
        ImGui::Text("Active texture refs: %u   surface refs: %u",
                    runtimeStats.activeTextureRefs, runtimeStats.activeSurfaceRefs);
        ImGui::Text("Created: %llu   Released: %llu   Outstanding: %llu",
                    runtimeStats.replacementCreates, runtimeStats.replacementReleases, outstanding);
        ImGui::Text("Estimated active replacement memory: %.1f MiB",
                    static_cast<double>(runtimeStats.estimatedActiveBytes) / (1024.0 * 1024.0));
        ImGui::Text("Apply success/failure: %llu / %llu",
                    runtimeStats.applySuccesses, runtimeStats.applyFailures);

        if (outstanding != runtimeStats.activeReplacementResources)
        {
            ImGui::TextColored(
                ImVec4(1.0f, 0.35f, 0.25f, 1.0f),
                "Lifetime mismatch: outstanding != active replacements");
        }
        else
        {
            ImGui::TextDisabled("Lifetime counters balanced for the active generation.");
        }

        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("PostFX Runtime"))
    {
        ImGui::Indent();
        DrawPostFxRuntimeDiagnostics();
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Shader Probe (research)"))
    {
        ImGui::Indent();
        ImGui::TextDisabled("Developer shader probe. It observes game shader binds; it does not replace shaders.");

        const ShaderProbeStats shaderStats = GetShaderProbeStats();
        ImGui::Text("Game shaders observed: VS %llu   PS %llu",
                    shaderStats.gameVertexShaders, shaderStats.gamePixelShaders);
        ImGui::Text("Registered bytecode blobs: VS %llu   PS %llu",
                    shaderStats.registeredVertexShaders, shaderStats.registeredPixelShaders);
        ImGui::Text("Game shader binds: %llu   unknown: %llu",
                    shaderStats.gameShaderBinds, shaderStats.unknownGameShaderBinds);
        ImGui::Text("Last game-bound VS: %016llX", shaderStats.currentVertexShaderHash);
        ImGui::Text("Last game-bound PS: %016llX", shaderStats.currentPixelShaderHash);

        ImGui::Spacing();
        float bloomMultiplier = GetShaderProbeBloomMultiplier();
        if (ImGui::SliderFloat(
                "Research bloom multiplier",
                &bloomMultiplier,
                0.0f,
                1.5f,
                "%.2fx"))
        {
            SetShaderProbeBloomMultiplier(bloomMultiplier);
        }
        ImGui::SameLine();
        if (ImGui::Button("Vanilla##BloomMultiplier"))
            SetShaderProbeBloomMultiplier(1.0f);
        ImGui::TextDisabled(
            "1.00x = vanilla g_fBloomForce. Applied only to the identified final-composite draw.");

        float exposureMultiplier = GetShaderProbeExposureMultiplier();
        if (ImGui::SliderFloat(
                "Research exposure multiplier",
                &exposureMultiplier,
                0.0f,
                1.5f,
                "%.2fx"))
        {
            SetShaderProbeExposureMultiplier(exposureMultiplier);
        }
        ImGui::SameLine();
        if (ImGui::Button("Vanilla##ExposureMultiplier"))
            SetShaderProbeExposureMultiplier(1.0f);
        ImGui::TextDisabled(
            "1.00x = vanilla g_fExposure (c10). Applied only to the identified final-composite draw.");

        ImGui::Spacing();
        const char* compositeDebugModes[] =
        {
            "Vanilla",
            "Show Depth",
            "Show Normals",
            "Show Normal Validity"
        };
        int compositeDebugMode = static_cast<int>(
            GetShaderProbeCompositeDebugMode());
        if (ImGui::Combo(
                "Native composite debug view",
                &compositeDebugMode,
                compositeDebugModes,
                4))
        {
            SetShaderProbeCompositeDebugMode(
                static_cast<ShaderProbeCompositeDebugMode>(compositeDebugMode));
        }
        ImGui::TextDisabled(
            "Hot apply: changes take effect on the next final-composite draw; no restart or D3D9 Reset.");
        ImGui::TextDisabled(
            "Normals uses the game's full-resolution MRT1 G-buffer captured alongside packed depth RT0.");

        if (ImGui::Button("Dump observed shader bytecode"))
            DumpShaderProbeShaders();
        ImGui::SameLine();
        if (ImGui::Button("Capture next game frame"))
            RequestShaderProbeFrameCapture();

        if (shaderStats.captureRequested)
            ImGui::TextDisabled("Capture requested; it will arm at the next Present.");
        else if (shaderStats.captureActive)
            ImGui::TextDisabled("Capturing this game frame...");
        else if (shaderStats.captureAvailable)
        {
            ImGui::TextDisabled(
                "Last capture: %llu draws, %u draw signatures, %u unique VS, %u unique PS",
                shaderStats.capturedDraws,
                shaderStats.capturedDrawSignatures,
                shaderStats.capturedUniqueVertexShaders,
                shaderStats.capturedUniquePixelShaders);
            ImGui::TextDisabled(
                "Target A90F VS pairs: %u   post-process snapshots: %u -> ZachFix\\shaders\\last_frame.txt",
                shaderStats.capturedTargetVertexShaderPairs,
                shaderStats.capturedTargetDraws);
        }

        ImGui::TextDisabled("Dumps: %llu written   %llu failed",
                            shaderStats.dumpSuccesses, shaderStats.dumpFailures);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("World Culling Research"))
    {
        ImGui::Indent();
        ImGui::TextDisabled(
            "Research-only runtime switch. It is deliberately not saved to ZachFix.ini.");

        bool disableFrustum = GetWorldFrustumCullDisabledResearch();
        if (ImGui::Checkbox(
                "Disable ALL hooked frustum culling (risky)",
                &disableFrustum))
        {
            SetWorldFrustumCullDisabledResearch(disableFrustum);
        }

        ImGui::TextDisabled(
            IsWorldFrustumCullResearchHookReady()
                ? "Global helper bypass: main/shadow/reflection callers using this helper can all be affected."
                : "Shared frustum helper is untouched until this switch is enabled for the first time.");
        ImGui::TextDisabled(
            "Bypassed native frustum rejects this session: %llu",
            GetWorldFrustumCullBypassedRejects());
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Texture Inspector"))
    {
        ImGui::Indent();
        const bool textureDeveloperModeActive = IsTextureDeveloperModeActive();
        const TextureOverrideStats textureStats = GetTextureOverrideStats();

        ImGui::Text("Observed: %llu   Unique: %llu   Override hits: %llu",
                    textureStats.sourceLoads, textureStats.uniqueHashes, textureStats.overrideHits);
        ImGui::Text("Dumped: %llu   Dump failures: %llu   Last hash: %08X",
                    textureStats.dumpedTextures, textureStats.dumpFailures, textureStats.lastHash);
        ImGui::Text(
            "Hot reload gen: %u   tracked loads: %llu   requests: %llu",
            textureStats.hotReloadGeneration,
            textureStats.hotReloadTrackedLoads,
            textureStats.hotReloadRequests);
        ImGui::Text(
            "Current rescan checked/loaded/reverted/fail: %llu / %llu / %llu / %llu",
            textureStats.hotReloadAttempts,
            textureStats.hotReloadSuccesses,
            textureStats.hotReloadReverts,
            textureStats.hotReloadFailures);

        ImGui::Spacing();
        if (!textureDeveloperModeActive)
        {
            ImGui::TextDisabled("Texture Developer Mode is off. Inspector/tracking is intentionally inactive this session.");
        }
        else
        {
            ImGui::TextDisabled("Source dimensions come from D3DX image metadata; GPU dimensions come from GetLevelDesc(0).");
            ImGui::TextDisabled("Inspector reports both source-file and actual GPU dimensions for the selected dimension mode.");

            const TextureInspectorSnapshot inspector = GetTextureInspectorSnapshot();
            DrawTextureInspectionRecord("Last observed", inspector.lastObserved);
            ImGui::Spacing();
            DrawTextureInspectionRecord("Last override hit", inspector.lastOverride);
        }
        ImGui::Unindent();
    }
}

void DrawAboutTab()
{
    ImGui::TextUnformatted("ZachFix");
    ImGui::TextDisabled("Modern rendering and compatibility fix for Deadly Premonition: The Director's Cut");
    ImGui::Text("Version: %s", kZachFixVersion);
    ImGui::TextDisabled("Target: Windows x86 / Direct3D 9");
    ImGui::TextDisabled("\"Zach, do you see this?\"");

    ImGui::Spacing();
    ImGui::SeparatorText("Acknowledgements");
    ImGui::BulletText("Peter Thoman (Durante) - original DPFix / DSFix and the rendering research this project builds on.");
    ImGui::BulletText("DXVK project - modern D3D9-to-Vulkan compatibility path used alongside ZachFix.");
    ImGui::BulletText("ReShade project - external post-processing and depth-based effects used alongside ZachFix.");
    ImGui::BulletText("dgVoodoo2 - validated D3D9-to-D3D11 compatibility path.");
    ImGui::BulletText("Ultimate ASI Loader / ThirteenAG - convenient ASI loading for the current deployment stack.");
    ImGui::BulletText("Deadly Premonition modding community and testers - compatibility findings, edge cases and validation.");

    ImGui::Spacing();
    if (ImGui::CollapsingHeader("Third-party software"))
    {
        ImGui::Indent();
        ImGui::TextUnformatted("MinHook v1.3.4");
        ImGui::TextDisabled("Tsuda Kageyu et al. - BSD-2-Clause - build dependency via CMake FetchContent.");
        ImGui::Spacing();

        ImGui::TextUnformatted("Dear ImGui v1.92.9b");
        ImGui::TextDisabled("Omar Cornut and contributors - MIT - in-game UI and Win32/DX9 backends.");
        ImGui::Spacing();

        ImGui::TextUnformatted("Paul Hsieh's SuperFastHash");
        ImGui::TextDisabled("BSD-style license - DPFix-compatible texture hashing, including historical signed-byte behavior.");
        ImGui::Spacing();

        ImGui::TextUnformatted("Original DPFix / DSFix lineage");
        ImGui::TextDisabled("Peter Thoman (Durante) - GPLv3 source and research reference. Derived code retains the applicable obligations.");
        ImGui::Spacing();

        ImGui::TextDisabled("Full attribution and license notes: THIRD_PARTY.md");
        ImGui::Unindent();
    }
}

void DrawSettingsWindow(IDirect3DDevice9* device)
{
    ImGui::SetNextWindowSize(ImVec2(620.0f, 0.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("ZachFix Settings", &g_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::TextUnformatted(kZachFixDisplayName);

    if (ImGui::Button("Save to INI"))
    {
        if (SaveEditableConfig(g_pending))
            strcpy_s(g_status, "Saved editor + PostFX values to ZachFix.ini.");
        else
            strcpy_s(g_status, "Save failed. Check ZachFix.log.");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload INI"))
        ReloadPendingFromIni();
    ImGui::SameLine();
    if (ImGui::Button("Apply"))
        ApplyLiveSettings(device);

    ImGui::SameLine();
    ImGui::TextDisabled("F10 closes");
    ImGui::TextWrapped("%s", g_status);
    ImGui::Separator();

    if (ImGui::BeginTabBar("ZachFixTabs"))
    {
        if (ImGui::BeginTabItem("Graphics"))
        {
            DrawSettingsTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("PostFX"))
        {
            DrawPostFxTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Diagnostics"))
        {
            DrawDiagnosticsTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("About"))
        {
            DrawAboutTab();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Keep render-thread polling as the primary path because Deadly Premonition
    // may read keyboard input through DirectInput. Native D3D9 can present via
    // IDirect3DSwapChain9::Present though, so also accept the initial Win32 key
    // message as a fallback. Only queue the request here; the actual UI state
    // change still happens on the render thread.
    const bool isToggleKey = static_cast<UINT>(wParam) == g_config.uiToggleKey;
    if ((msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN) &&
        isToggleKey &&
        (lParam & (1LL << 30)) == 0)
    {
        // Win32 and GetAsyncKeyState may report the same physical press in
        // different callbacks. Whichever source gets here first owns the press.
        // The other source sees the latch and must not queue a second toggle.
        bool expectedPress = false;
        if (g_togglePressLatched.compare_exchange_strong(
                expectedPress, true, std::memory_order_acq_rel))
        {
            g_toggleRequested.store(true, std::memory_order_release);
        }

        bool expectedLog = false;
        if (g_loggedWin32ToggleFallback.compare_exchange_strong(
                expectedLog, true, std::memory_order_relaxed))
        {
            AppendLog("[UI] Win32 toggle-key fallback observed.\n");
        }
    }
    else if ((msg == WM_KEYUP || msg == WM_SYSKEYUP) && isToggleKey)
    {
        // Re-arm only after the physical key is released. This also suppresses
        // autorepeat WM_KEYDOWN messages while F10 is held.
        g_togglePressLatched.store(false, std::memory_order_release);
    }

    if (g_initialized)
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

    if (g_open && (IsKeyboardMessage(msg) || IsMouseMessage(msg)))
        return 1;

    if (g_originalWndProc)
        return CallWindowProcW(g_originalWndProc, hwnd, msg, wParam, lParam);

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
} // namespace

void InvalidateSettingsUiDeviceObjects()
{
    if (!g_initialized || !g_config.uiEnabled)
        return;

    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void NotifySettingsUiResetResult(HRESULT resetResult)
{
    if (!g_initialized || !g_config.uiEnabled || FAILED(resetResult))
        return;

    if (!ImGui_ImplDX9_CreateDeviceObjects())
        AppendLog("[UI] ERROR: Dear ImGui DX9 device objects could not be recreated after Reset.\n");
}


bool InitializeSettingsUi(HWND window, IDirect3DDevice9* device)
{
    if (!g_config.uiEnabled)
    {
        AppendLog("[UI] Disabled by config.\n");
        return true;
    }

    if (g_initialized)
        return true;

    if (!window || !device)
        return false;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplWin32_Init(window))
    {
        AppendLog("[UI] ERROR: Dear ImGui Win32 backend initialization failed.\n");
        ImGui::DestroyContext();
        return false;
    }

    if (!ImGui_ImplDX9_Init(device))
    {
        AppendLog("[UI] ERROR: Dear ImGui DX9 backend initialization failed.\n");
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    SetLastError(0);
    g_originalWndProc = reinterpret_cast<WNDPROC>(
        SetWindowLongPtrW(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&SettingsWndProc)));
    if (!g_originalWndProc)
    {
        AppendLog("[UI] ERROR: Could not subclass game window.\n");
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        return false;
    }

    g_window = window;
    g_pending = g_config;
    InstallUiInputIsolationHooks();
    InitializeGameplayPauseHooks();
    g_initialized = true;
    AppendLog("[UI] In-game settings initialized. Toggle key: F10 by default.\n");
    return true;
}

namespace
{
void RenderSettingsUiInternal(IDirect3DDevice9* device, bool sceneAlreadyBegun)
{
    if (!g_initialized || !device || !g_config.uiEnabled)
        return;

    // Poll the toggle key from the render thread. This works even when the game
    // obtains keyboard state through DirectInput and bypasses WM_KEYUP.
    const bool toggleKeyDown =
        (GetAsyncKeyState(static_cast<int>(g_config.uiToggleKey)) & 0x8000) != 0;

    bool pollingToggle = false;
    if (toggleKeyDown)
    {
        bool expectedPress = false;
        pollingToggle = g_togglePressLatched.compare_exchange_strong(
            expectedPress, true, std::memory_order_acq_rel);
    }
    else
    {
        // Some games do not forward WM_KEYUP consistently. Polling therefore
        // also re-arms the latch once the key is physically up.
        g_togglePressLatched.store(false, std::memory_order_release);
    }

    const bool messageToggle =
        g_toggleRequested.exchange(false, std::memory_order_acq_rel);

    if (pollingToggle || messageToggle)
    {
        g_open = !g_open;
        OnUiOpenStateChanged(g_open);
        AppendLog(g_open ? "[UI] Settings panel opened; game input suppressed.\n"
                         : "[UI] Settings panel closed; game input restored.\n");
    }


    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = g_open;

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (g_open)
    {
        const bool wasOpen = g_open;
        DrawSettingsWindow(device);
        if (wasOpen && !g_open)
        {
            OnUiOpenStateChanged(false);
            AppendLog("[UI] Settings panel closed; game input restored.\n");
        }
    }

    ImGui::Render();
    if (ImGui::GetDrawData()->CmdListsCount == 0)
        return;

    if (sceneAlreadyBegun)
    {
        // EndScene fallback calls us while the game's D3D9 scene is still open.
        // Rendering directly here avoids opening a nested/second scene, which
        // native D3D9 is stricter about than translation layers such as DXVK.
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        return;
    }

    const HRESULT beginSceneResult = device->BeginScene();
    if (SUCCEEDED(beginSceneResult))
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
    else
    {
        bool expected = false;
        if (g_loggedBeginSceneFailure.compare_exchange_strong(
                expected, true, std::memory_order_relaxed))
        {
            char text[160] = {};
            sprintf_s(
                text,
                "[UI] WARNING: BeginScene failed while drawing settings UI (HRESULT=0x%08X).\n",
                static_cast<unsigned>(beginSceneResult));
            AppendLog(text);
        }
    }
}
} // namespace

void RenderSettingsUi(IDirect3DDevice9* device)
{
    RenderSettingsUiInternal(device, false);
}

void RenderSettingsUiInScene(IDirect3DDevice9* device)
{
    RenderSettingsUiInternal(device, true);
}
