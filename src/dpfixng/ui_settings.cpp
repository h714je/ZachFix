#include "ui_settings.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"
#include "runtime_resources.h"
#include "ssao.h"
#include "world_streaming.h"

#include <Windows.h>
#include <MinHook.h>
#include <intrin.h>
#include <algorithm>
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
bool g_toggleKeyWasDown = false;
DPFixNGConfig g_pending{};
char g_status[192] = "F10 opens this panel.";

using GetCursorPosFn = BOOL (WINAPI*)(LPPOINT);
using SetCursorPosFn = BOOL (WINAPI*)(int, int);
using GetAsyncKeyStateFn = SHORT (WINAPI*)(int);
using GetKeyboardStateFn = BOOL (WINAPI*)(PBYTE);

GetCursorPosFn g_originalGetCursorPos = nullptr;
SetCursorPosFn g_originalSetCursorPos = nullptr;
GetAsyncKeyStateFn g_originalGetAsyncKeyState = nullptr;
GetKeyboardStateFn g_originalGetKeyboardState = nullptr;

void* g_getCursorPosTarget = nullptr;
void* g_setCursorPosTarget = nullptr;
void* g_getAsyncKeyStateTarget = nullptr;
void* g_getKeyboardStateTarget = nullptr;

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
        void** targetOut;
    };

    HookSpec specs[] = {
        {"GetCursorPos", reinterpret_cast<void*>(&HookGetCursorPos),
         reinterpret_cast<void**>(&g_originalGetCursorPos), &g_getCursorPosTarget},
        {"SetCursorPos", reinterpret_cast<void*>(&HookSetCursorPos),
         reinterpret_cast<void**>(&g_originalSetCursorPos), &g_setCursorPosTarget},
        {"GetAsyncKeyState", reinterpret_cast<void*>(&HookGetAsyncKeyState),
         reinterpret_cast<void**>(&g_originalGetAsyncKeyState), &g_getAsyncKeyStateTarget},
        {"GetKeyboardState", reinterpret_cast<void*>(&HookGetKeyboardState),
         reinterpret_cast<void**>(&g_originalGetKeyboardState), &g_getKeyboardStateTarget},
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

        *spec.targetOut = reinterpret_cast<void*>(proc);

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
    }
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

void ReloadPendingFromIni()
{
    wchar_t path[MAX_PATH] = {};
    if (!GetConfigFilePath(path, MAX_PATH))
    {
        strcpy_s(g_status, "Could not locate DPFixNG.ini.");
        return;
    }

    DPFixNGConfig next = g_config;
    next.internalWidth = GetPrivateProfileIntW(L"Rendering", L"InternalWidth", next.internalWidth, path);
    next.internalHeight = GetPrivateProfileIntW(L"Rendering", L"InternalHeight", next.internalHeight, path);
    next.internalScale = std::clamp(ReadFloat(path, L"Rendering", L"InternalScale", next.internalScale), 0.25f, 4.0f);
    next.fixPixelOffset = ReadBool(path, L"Rendering", L"FixPixelOffset", next.fixPixelOffset);
    next.shadowScale = std::clamp<UINT>(GetPrivateProfileIntW(L"Shadows", L"Scale", next.shadowScale, path), 1, 8);
    next.reflectionScale = std::clamp<UINT>(GetPrivateProfileIntW(L"Reflections", L"Scale", next.reflectionScale, path), 1, 8);
    next.improveDofResolution = ReadBool(path, L"DepthOfField", L"ImproveResolution", next.improveDofResolution);
    next.ssaoEnabled = ReadBool(path, L"AmbientOcclusion", L"Enabled", next.ssaoEnabled);
    next.ssaoStrength = std::clamp(ReadFloat(path, L"AmbientOcclusion", L"Strength", next.ssaoStrength), 0.0f, 2.5f);
    next.ssaoRadius = std::clamp(ReadFloat(path, L"AmbientOcclusion", L"Radius", next.ssaoRadius), 0.25f, 8.0f);
    next.ssaoResolutionScale = GetPrivateProfileIntW(L"AmbientOcclusion", L"ResolutionScale", next.ssaoResolutionScale, path);
    if (next.ssaoResolutionScale != 1 && next.ssaoResolutionScale != 2 && next.ssaoResolutionScale != 4)
        next.ssaoResolutionScale = 2;
    next.ssaoDebugView = std::min<UINT>(GetPrivateProfileIntW(L"AmbientOcclusion", L"DebugView", next.ssaoDebugView, path), 13);
    next.highDetailDistanceScale = std::clamp<UINT>(GetPrivateProfileIntW(L"World", L"HighDetailDistanceScale", next.highDetailDistanceScale, path), 1, 2);

    g_pending = next;
    strcpy_s(g_status, "Reloaded editable settings from DPFixNG.ini.");
}

void ApplyLiveSettings(IDirect3DDevice9* device)
{
    if (!ApplyRuntimeRenderSettings(
            device,
            g_pending,
            g_status,
            sizeof(g_status)))
    {
        return;
    }

    ApplySsaoSettings(g_pending);

    // Keep the editor synchronized with the values that were actually committed.
    g_pending = g_config;
}

void DrawSettingsWindow(IDirect3DDevice9* device)
{
    ImGui::SetNextWindowSize(ImVec2(500.0f, 0.0f), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("DPFix-NG Settings", &g_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::End();
        return;
    }

    ImGui::TextUnformatted("v0.0.36 Runtime Audit + SSAO exp1 fix10");
    ImGui::Separator();

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
    ImGui::TextDisabled("(live)");

    ImGui::Spacing();
    ImGui::TextUnformatted("Shadows / Reflections");
    int shadowScale = static_cast<int>(g_pending.shadowScale);
    if (ImGui::SliderInt("Shadow Scale", &shadowScale, 1, 8, "%dx"))
        g_pending.shadowScale = static_cast<UINT>(shadowScale);
    int reflectionScale = static_cast<int>(g_pending.reflectionScale);
    if (ImGui::SliderInt("Reflection Scale", &reflectionScale, 1, 8, "%dx"))
        g_pending.reflectionScale = static_cast<UINT>(reflectionScale);

    ImGui::Spacing();
    ImGui::TextUnformatted("Depth of Field");
    ImGui::Checkbox("Improve DoF Resolution", &g_pending.improveDofResolution);

    ImGui::Spacing();
    ImGui::TextUnformatted("Ambient Occlusion (experimental)");
    ImGui::Checkbox("Enable SSAO", &g_pending.ssaoEnabled);
    ImGui::SliderFloat("SSAO Strength", &g_pending.ssaoStrength, 0.0f, 2.5f, "%.2f");
    ImGui::SliderFloat("SSAO Radius", &g_pending.ssaoRadius, 0.25f, 8.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
    int ssaoResolution = g_pending.ssaoResolutionScale == 1 ? 0 :
                         g_pending.ssaoResolutionScale == 4 ? 2 : 1;
    const char* ssaoResolutionItems[] = { "Full", "Half", "Quarter" };
    if (ImGui::Combo("SSAO Resolution", &ssaoResolution, ssaoResolutionItems, 3))
    {
        const UINT scales[] = { 1, 2, 4 };
        g_pending.ssaoResolutionScale = scales[ssaoResolution];
    }
    int ssaoDebugView = static_cast<int>(std::min<UINT>(g_pending.ssaoDebugView, 13));
    const char* ssaoDebugItems[] =
    {
        "Combined",
        "AO Only",
        "DP Fixed-point Decoded Depth",
        "Solid Magenta",
        "Raw Packed RGB",
        "AO Contrast x32",
        "Depth channel R",
        "Depth channel G",
        "Depth channel B",
        "Depth channel A",
        "Packed RGB 0..1 candidate",
        "Packed RGB inverted candidate",
        "Packed RGB perspective-linearized candidate",
        "Legacy DSFix inverted decode"
    };
    if (ImGui::Combo("SSAO Debug View", &ssaoDebugView, ssaoDebugItems, 14))
        g_pending.ssaoDebugView = static_cast<UINT>(ssaoDebugView);
    ImGui::TextDisabled("Solid Magenta bypasses depth/AO math and tests the scene injection target itself.");
    ImGui::TextDisabled("Debug views are applied with the normal Apply button.");
    ImGui::TextDisabled("Radius is a DP view-space radius multiplier (1.0 ~= 2 view units).");

    ImGui::Spacing();
    ImGui::TextUnformatted("World");
    int worldMode = static_cast<int>(g_pending.highDetailDistanceScale - 1);
    const char* worldItems[] = { "Original 2x2 core", "Extended 4x4 ring" };
    if (ImGui::Combo("High Detail Distance", &worldMode, worldItems, 2))
        g_pending.highDetailDistanceScale = static_cast<UINT>(worldMode + 1);
    ImGui::SameLine();
    ImGui::TextDisabled("(live on cell transition)");

    ImGui::Separator();
    ImGui::TextDisabled("Hot Apply rebuilds DPFix-NG render targets between frames; no D3D9 Reset is used.");
    ImGui::TextDisabled("World Detail updates fully on subsequent streaming-cell transitions.");
    const RuntimeResourceStats runtimeStats = GetRuntimeResourceStats();
    const unsigned long long outstanding =
        runtimeStats.replacementCreates >= runtimeStats.replacementReleases
            ? runtimeStats.replacementCreates - runtimeStats.replacementReleases
            : 0;

    if (ImGui::TreeNodeEx("Runtime Resource Audit", ImGuiTreeNodeFlags_DefaultOpen))
    {
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

        const SsaoRuntimeStats ssaoStats = GetSsaoRuntimeStats();
        ImGui::Separator();
        ImGui::Text("SSAO: %s   Depth MRT this frame: %s",
                    ssaoStats.enabled ? "enabled" : "disabled",
                    ssaoStats.depthPairSeen ? "yes" : "no");
        if (ssaoStats.resourcesReady)
        {
            ImGui::Text("SSAO resources: frame %u x %u, AO %u x %u",
                        ssaoStats.frameWidth, ssaoStats.frameHeight,
                        ssaoStats.aoWidth, ssaoStats.aoHeight);
            ImGui::Text("Estimated SSAO target memory: %.1f MiB",
                        static_cast<double>(ssaoStats.estimatedBytes) / (1024.0 * 1024.0));
        }
        ImGui::Text("SSAO applied frames: %llu   failures: %llu",
                    ssaoStats.framesApplied, ssaoStats.failures);

        if (ImGui::Button("Probe SSAO buffers next frame"))
            RequestSsaoProbe();

        if (ssaoStats.probeValid)
        {
            ImGui::Text("Probe samples: %u", ssaoStats.probeSamples);
            ImGui::Text("Decoded depth min/p05/median/p95/max:");
            ImGui::Text("%.4f / %.4f / %.4f / %.4f / %.4f",
                        ssaoStats.depthMin, ssaoStats.depthP05, ssaoStats.depthMedian,
                        ssaoStats.depthP95, ssaoStats.depthMax);
            ImGui::Text("Approx view-Z median: %.1f units",
                        1.0f + ssaoStats.depthMedian * 5000.0f);
            ImGui::Text("Depth <0.075: %.1f%%   Depth >1: %.1f%%",
                        ssaoStats.depthBelow0075Percent, ssaoStats.depthAbove1Percent);
            ImGui::Text("Estimated median AO tap radius: %.3f px",
                        ssaoStats.estimatedTapRadiusMedianPx);
            ImGui::Text("AO mask min / mean / max: %.5f / %.5f / %.5f",
                        ssaoStats.aoMin, ssaoStats.aoMean, ssaoStats.aoMax);
            ImGui::Separator();
            ImGui::TextUnformatted("Raw channel min / p05 / median / p95 / max:");
            ImGui::Text("R %.4f / %.4f / %.4f / %.4f / %.4f",
                        ssaoStats.channelRMin, ssaoStats.channelRP05, ssaoStats.channelRMedian, ssaoStats.channelRP95, ssaoStats.channelRMax);
            ImGui::Text("G %.4f / %.4f / %.4f / %.4f / %.4f",
                        ssaoStats.channelGMin, ssaoStats.channelGP05, ssaoStats.channelGMedian, ssaoStats.channelGP95, ssaoStats.channelGMax);
            ImGui::Text("B %.4f / %.4f / %.4f / %.4f / %.4f",
                        ssaoStats.channelBMin, ssaoStats.channelBP05, ssaoStats.channelBMedian, ssaoStats.channelBP95, ssaoStats.channelBMax);
            ImGui::Text("A %.4f / %.4f / %.4f / %.4f / %.4f",
                        ssaoStats.channelAMin, ssaoStats.channelAP05, ssaoStats.channelAMedian, ssaoStats.channelAP95, ssaoStats.channelAMax);
            ImGui::Text("Packed RGB candidate %.5f / %.5f / %.5f / %.5f / %.5f",
                        ssaoStats.packedUnitMin, ssaoStats.packedUnitP05, ssaoStats.packedUnitMedian, ssaoStats.packedUnitP95, ssaoStats.packedUnitMax);
        }
        ImGui::TreePop();
    }

    if (ImGui::Button("Apply"))
        ApplyLiveSettings(device);
    ImGui::SameLine();
    if (ImGui::Button("Save to INI"))
    {
        if (SaveEditableConfig(g_pending))
            strcpy_s(g_status, "Saved current editor values to DPFixNG.ini.");
        else
            strcpy_s(g_status, "Save failed. Check DPFixNG.log.");
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload INI"))
        ReloadPendingFromIni();

    ImGui::TextWrapped("%s", g_status);
    ImGui::End();
}

LRESULT CALLBACK SettingsWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // The toggle key is intentionally polled from RenderSettingsUi() rather than
    // relying on WM_KEYUP. Deadly Premonition may consume keyboard input through
    // DirectInput, in which case the game window does not reliably receive the
    // corresponding Win32 key message.
    if (g_initialized)
        ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

    if (g_open && (IsKeyboardMessage(msg) || IsMouseMessage(msg)))
        return 1;

    if (g_originalWndProc)
        return CallWindowProcW(g_originalWndProc, hwnd, msg, wParam, lParam);

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
} // namespace

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
    g_initialized = true;
    AppendLog("[UI] In-game settings initialized. Toggle key: F10 by default.\n");
    return true;
}

void RenderSettingsUi(IDirect3DDevice9* device)
{
    if (!g_initialized || !device || !g_config.uiEnabled)
        return;

    // Poll the toggle key from the render thread. This works even when the game
    // obtains keyboard state through DirectInput and bypasses WM_KEYUP.
    const bool toggleKeyDown =
        (GetAsyncKeyState(static_cast<int>(g_config.uiToggleKey)) & 0x8000) != 0;

    if (toggleKeyDown && !g_toggleKeyWasDown)
    {
        g_open = !g_open;
        OnUiOpenStateChanged(g_open);
        AppendLog(g_open ? "[UI] Settings panel opened; game input suppressed.\n"
                         : "[UI] Settings panel closed; game input restored.\n");
    }

    g_toggleKeyWasDown = toggleKeyDown;

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

    if (SUCCEEDED(device->BeginScene()))
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }
}

void ShutdownSettingsUi()
{
    if (!g_initialized)
        return;

    if (g_window && g_originalWndProc)
        SetWindowLongPtrW(g_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(g_originalWndProc));

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    g_initialized = false;
    g_open = false;
    g_toggleKeyWasDown = false;
    g_window = nullptr;
    g_originalWndProc = nullptr;
}
