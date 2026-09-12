#include "config.h"
#include "logging.h"

#include <cmath>
#include <cstdio>
#include <cwchar>

DPFixNGConfig g_config{};

UINT g_displayWidth = 1280;
UINT g_displayHeight = 720;
UINT g_internalWidth = 1280;
UINT g_internalHeight = 720;

namespace
{
static constexpr UINT kMinResolutionWidth = 640;
static constexpr UINT kMinResolutionHeight = 360;

bool BuildConfigPath(wchar_t* path, size_t pathCount)
{
    if (path == nullptr || pathCount == 0)
        return false;

    const DWORD length = GetModuleFileNameW(
        nullptr,
        path,
        static_cast<DWORD>(pathCount)
    );

    if (length == 0 || length >= pathCount)
        return false;

    wchar_t* slash = wcsrchr(path, L'\\');

    if (slash == nullptr)
        return false;

    *(slash + 1) = L'\0';

    return wcscat_s(
        path,
        pathCount,
        L"DPFixNG.ini"
    ) == 0;
}


bool ParseBool(const wchar_t* value, bool defaultValue)
{
    if (value == nullptr || value[0] == L'\0')
        return defaultValue;

    if (_wcsicmp(value, L"true") == 0 ||
        _wcsicmp(value, L"yes") == 0 ||
        _wcsicmp(value, L"on") == 0 ||
        wcscmp(value, L"1") == 0)
    {
        return true;
    }

    if (_wcsicmp(value, L"false") == 0 ||
        _wcsicmp(value, L"no") == 0 ||
        _wcsicmp(value, L"off") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return false;
    }

    return defaultValue;
}


float ParseFloat(const wchar_t* value, float defaultValue)
{
    if (value == nullptr || value[0] == L'\0')
        return defaultValue;

    wchar_t* end = nullptr;
    const double parsed = wcstod(value, &end);

    if (end == value ||
        *end != L'\0' ||
        !std::isfinite(parsed))
    {
        return defaultValue;
    }

    return static_cast<float>(parsed);
}


bool IsReasonableResolution(UINT width, UINT height)
{
    return width >= kMinResolutionWidth &&
           height >= kMinResolutionHeight &&
           width <= kMaxResolutionWidth &&
           height <= kMaxResolutionHeight;
}


UINT ParseVirtualKey(
    const wchar_t* value,
    UINT defaultValue)
{
    if (value == nullptr || value[0] == L'\0')
        return defaultValue;

    if ((value[0] == L'F' || value[0] == L'f') &&
        value[1] != L'\0')
    {
        wchar_t* end = nullptr;
        const unsigned long number =
            wcstoul(value + 1, &end, 10);

        if (end != value + 1 &&
            *end == L'\0' &&
            number >= 1 &&
            number <= 12)
        {
            return VK_F1 +
                static_cast<UINT>(number - 1);
        }
    }

    struct NamedKey
    {
        const wchar_t* name;
        UINT vk;
    };

    const NamedKey keys[] =
    {
        { L"HOME", VK_HOME },
        { L"END", VK_END },
        { L"INSERT", VK_INSERT },
        { L"DELETE", VK_DELETE },
        { L"PAUSE", VK_PAUSE },
        { L"SCROLLLOCK", VK_SCROLL },
        { L"NUMLOCK", VK_NUMLOCK }
    };

    for (const NamedKey& key : keys)
    {
        if (_wcsicmp(value, key.name) == 0)
            return key.vk;
    }

    if (value[1] == L'\0')
    {
        wchar_t ch = value[0];

        if (ch >= L'a' && ch <= L'z')
            ch = static_cast<wchar_t>(ch - L'a' + L'A');

        if ((ch >= L'A' && ch <= L'Z') ||
            (ch >= L'0' && ch <= L'9'))
        {
            return static_cast<UINT>(ch);
        }
    }

    wchar_t* end = nullptr;
    const unsigned long numeric =
        wcstoul(value, &end, 0);

    if (end != value &&
        *end == L'\0' &&
        numeric <= 0xFF)
    {
        return static_cast<UINT>(numeric);
    }

    return defaultValue;
}



} // namespace

void LoadConfig()
{
    wchar_t path[MAX_PATH] = {};

    if (!BuildConfigPath(path, MAX_PATH))
    {
        AppendLog("[Config] WARNING: Could not build DPFixNG.ini path. Using defaults.\n");
        return;
    }

    g_config.displayWidth = GetPrivateProfileIntW(
        L"Display",
        L"Width",
        0,
        path
    );

    g_config.displayHeight = GetPrivateProfileIntW(
        L"Display",
        L"Height",
        0,
        path
    );

    wchar_t borderlessText[32] = L"true";

    GetPrivateProfileStringW(
        L"Display",
        L"Borderless",
        L"true",
        borderlessText,
        static_cast<DWORD>(sizeof(borderlessText) / sizeof(borderlessText[0])),
        path
    );

    g_config.borderless = ParseBool(borderlessText, true);

    g_config.internalWidth = GetPrivateProfileIntW(
        L"Rendering",
        L"InternalWidth",
        0,
        path
    );

    g_config.internalHeight = GetPrivateProfileIntW(
        L"Rendering",
        L"InternalHeight",
        0,
        path
    );

    wchar_t internalScaleText[64] = L"1.0";

    GetPrivateProfileStringW(
        L"Rendering",
        L"InternalScale",
        L"1.0",
        internalScaleText,
        static_cast<DWORD>(sizeof(internalScaleText) / sizeof(internalScaleText[0])),
        path
    );

    g_config.internalScale = ParseFloat(internalScaleText, 1.0f);

    if (g_config.internalScale < 0.25f ||
        g_config.internalScale > 4.0f)
    {
        AppendLog(
            "[Config] WARNING: Rendering.InternalScale must be between "
            "0.25 and 4.0. Falling back to 1.0.\n"
        );
        g_config.internalScale = 1.0f;
    }

    g_config.shadowScale = GetPrivateProfileIntW(
        L"Shadows",
        L"Scale",
        1,
        path
    );

    if (g_config.shadowScale < 1 || g_config.shadowScale > 8)
    {
        AppendLog(
            "[Config] WARNING: Shadows.Scale must be between 1 and 8. "
            "Falling back to 1.\n"
        );
        g_config.shadowScale = 1;
    }

    g_config.reflectionScale = GetPrivateProfileIntW(
        L"Reflections",
        L"Scale",
        1,
        path
    );

    if (g_config.reflectionScale < 1 || g_config.reflectionScale > 8)
    {
        AppendLog(
            "[Config] WARNING: Reflections.Scale must be between 1 and 8. "
            "Falling back to 1.\n"
        );
        g_config.reflectionScale = 1;
    }

    wchar_t improveDofResolutionText[32] = L"false";

    GetPrivateProfileStringW(
        L"DepthOfField",
        L"ImproveResolution",
        L"false",
        improveDofResolutionText,
        static_cast<DWORD>(
            sizeof(improveDofResolutionText) /
            sizeof(improveDofResolutionText[0])
        ),
        path
    );

    g_config.improveDofResolution =
        ParseBool(improveDofResolutionText, false);

    wchar_t fixPixelOffsetText[32] = L"true";

    GetPrivateProfileStringW(
        L"Rendering",
        L"FixPixelOffset",
        L"true",
        fixPixelOffsetText,
        static_cast<DWORD>(
            sizeof(fixPixelOffsetText) /
            sizeof(fixPixelOffsetText[0])
        ),
        path
    );

    g_config.fixPixelOffset =
        ParseBool(fixPixelOffsetText, true);

    g_config.highDetailDistanceScale =
        GetPrivateProfileIntW(
            L"World",
            L"HighDetailDistanceScale",
            1,
            path
        );

    if (g_config.highDetailDistanceScale < 1 ||
        g_config.highDetailDistanceScale > 2)
    {
        AppendLog(
            "[Config] WARNING: World.HighDetailDistanceScale currently supports "
            "only 1 (original) or 2 (extended outer ring). Falling back to 1.\n"
        );
        g_config.highDetailDistanceScale = 1;
    }

    wchar_t uiEnabledText[32] = L"true";

    GetPrivateProfileStringW(
        L"UI",
        L"Enabled",
        L"true",
        uiEnabledText,
        static_cast<DWORD>(sizeof(uiEnabledText) / sizeof(uiEnabledText[0])),
        path
    );

    g_config.uiEnabled = ParseBool(uiEnabledText, true);

    wchar_t uiToggleKeyText[32] = L"F10";

    GetPrivateProfileStringW(
        L"UI",
        L"ToggleKey",
        L"F10",
        uiToggleKeyText,
        static_cast<DWORD>(sizeof(uiToggleKeyText) / sizeof(uiToggleKeyText[0])),
        path
    );

    g_config.uiToggleKey = ParseVirtualKey(uiToggleKeyText, VK_F10);

    wchar_t profilerEnabledText[32] = L"true";

    GetPrivateProfileStringW(
        L"Profiler",
        L"Enabled",
        L"true",
        profilerEnabledText,
        static_cast<DWORD>(
            sizeof(profilerEnabledText) /
            sizeof(profilerEnabledText[0])
        ),
        path
    );

    g_config.profilerEnabled =
        ParseBool(profilerEnabledText, true);

    wchar_t profilerCaptureKeyText[32] = L"F11";

    GetPrivateProfileStringW(
        L"Profiler",
        L"CaptureKey",
        L"F11",
        profilerCaptureKeyText,
        static_cast<DWORD>(
            sizeof(profilerCaptureKeyText) /
            sizeof(profilerCaptureKeyText[0])
        ),
        path
    );

    g_config.profilerCaptureKey =
        ParseVirtualKey(
            profilerCaptureKeyText,
            VK_F11
        );

    wchar_t profilerDumpShadersText[32] = L"false";

    GetPrivateProfileStringW(
        L"Profiler",
        L"DumpShaders",
        L"false",
        profilerDumpShadersText,
        static_cast<DWORD>(
            sizeof(profilerDumpShadersText) /
            sizeof(profilerDumpShadersText[0])
        ),
        path
    );

    g_config.profilerDumpShaders =
        ParseBool(
            profilerDumpShadersText,
            false
        );

    wchar_t profilerGpuTimingsText[32] = L"true";

    GetPrivateProfileStringW(
        L"Profiler",
        L"GPUTimings",
        L"true",
        profilerGpuTimingsText,
        static_cast<DWORD>(
            sizeof(profilerGpuTimingsText) /
            sizeof(profilerGpuTimingsText[0])
        ),
        path
    );

    g_config.profilerGpuTimings =
        ParseBool(
            profilerGpuTimingsText,
            true
        );

    wchar_t profilerCallerTracingText[32] = L"true";

    GetPrivateProfileStringW(
        L"Profiler",
        L"CallerTracing",
        L"true",
        profilerCallerTracingText,
        static_cast<DWORD>(
            sizeof(profilerCallerTracingText) /
            sizeof(profilerCallerTracingText[0])
        ),
        path
    );

    g_config.profilerCallerTracing =
        ParseBool(
            profilerCallerTracingText,
            true
        );

    wchar_t profilerSceneObjectTracingText[32] = L"true";

    GetPrivateProfileStringW(
        L"Profiler",
        L"SceneObjectTracing",
        L"true",
        profilerSceneObjectTracingText,
        static_cast<DWORD>(
            sizeof(profilerSceneObjectTracingText) /
            sizeof(profilerSceneObjectTracingText[0])
        ),
        path
    );

    g_config.profilerSceneObjectTracing =
        ParseBool(
            profilerSceneObjectTracingText,
            true
        );

    wchar_t profilerContinuousTraceText[32] = L"true";

    GetPrivateProfileStringW(
        L"Profiler",
        L"ContinuousTrace",
        L"true",
        profilerContinuousTraceText,
        static_cast<DWORD>(
            sizeof(profilerContinuousTraceText) /
            sizeof(profilerContinuousTraceText[0])
        ),
        path
    );

    g_config.profilerContinuousTrace =
        ParseBool(profilerContinuousTraceText, true);

    g_config.profilerContinuousMaxFrames =
        GetPrivateProfileIntW(
            L"Profiler",
            L"ContinuousMaxFrames",
            600,
            path
        );

    if (g_config.profilerContinuousMaxFrames < 30 ||
        g_config.profilerContinuousMaxFrames > 2000)
    {
        AppendLog(
            "[Config] WARNING: Profiler.ContinuousMaxFrames must be between "
            "30 and 2000. Falling back to 600.\n"
        );
        g_config.profilerContinuousMaxFrames = 600;
    }

    g_config.profilerMaxEvents =
        GetPrivateProfileIntW(
            L"Profiler",
            L"MaxEvents",
            20000,
            path
        );

    if (g_config.profilerMaxEvents < 1000 ||
        g_config.profilerMaxEvents > 200000)
    {
        AppendLog(
            "[Config] WARNING: Profiler.MaxEvents must be between "
            "1000 and 200000. Falling back to 20000.\n"
        );
        g_config.profilerMaxEvents = 20000;
    }

    char text[768] = {};

    sprintf_s(
        text,
        "[Config] Requested Display=%u x %u, Borderless=%s, "
        "Internal=%u x %u, InternalScale=%.2f, ShadowScale=%u, ReflectionScale=%u, "
        "ImproveDOF=%s, FixPixelOffset=%s, HighDetailDistanceScale=%u, "
        "UI=%s UIKey=0x%02X, Profiler=%s Key=0x%02X DumpShaders=%s GPUTimings=%s CallerTracing=%s "
        "SceneObjectTracing=%s ContinuousTrace=%s ContinuousMaxFrames=%u MaxEvents=%u\n",
        g_config.displayWidth,
        g_config.displayHeight,
        g_config.borderless ? "true" : "false",
        g_config.internalWidth,
        g_config.internalHeight,
        g_config.internalScale,
        g_config.shadowScale,
        g_config.reflectionScale,
        g_config.improveDofResolution ? "true" : "false",
        g_config.fixPixelOffset ? "true" : "false",
        g_config.highDetailDistanceScale,
        g_config.uiEnabled ? "true" : "false",
        g_config.uiToggleKey,
        g_config.profilerEnabled ? "true" : "false",
        g_config.profilerCaptureKey,
        g_config.profilerDumpShaders ? "true" : "false",
        g_config.profilerGpuTimings ? "true" : "false",
        g_config.profilerCallerTracing ? "true" : "false",
        g_config.profilerSceneObjectTracing ? "true" : "false",
        g_config.profilerContinuousTrace ? "true" : "false",
        g_config.profilerContinuousMaxFrames,
        g_config.profilerMaxEvents
    );

    AppendLog(text);
}



bool GetConfigFilePath(wchar_t* path, size_t pathCount)
{
    return BuildConfigPath(path, pathCount);
}

bool SaveEditableConfig(const DPFixNGConfig& config)
{
    wchar_t path[MAX_PATH] = {};
    if (!BuildConfigPath(path, MAX_PATH))
    {
        AppendLog("[Config] ERROR: Could not build DPFixNG.ini path for save.\n");
        return false;
    }

    auto writeUInt = [&](const wchar_t* section, const wchar_t* key, UINT value)
    {
        wchar_t buffer[32] = {};
        swprintf_s(buffer, L"%u", value);
        return WritePrivateProfileStringW(section, key, buffer, path) != FALSE;
    };

    auto writeFloat = [&](const wchar_t* section, const wchar_t* key, float value)
    {
        wchar_t buffer[64] = {};
        swprintf_s(buffer, L"%.2f", static_cast<double>(value));
        return WritePrivateProfileStringW(section, key, buffer, path) != FALSE;
    };

    auto writeBool = [&](const wchar_t* section, const wchar_t* key, bool value)
    {
        return WritePrivateProfileStringW(section, key, value ? L"true" : L"false", path) != FALSE;
    };

    bool ok = true;
    ok &= writeUInt(L"Rendering", L"InternalWidth", config.internalWidth);
    ok &= writeUInt(L"Rendering", L"InternalHeight", config.internalHeight);
    ok &= writeFloat(L"Rendering", L"InternalScale", config.internalScale);
    ok &= writeBool(L"Rendering", L"FixPixelOffset", config.fixPixelOffset);
    ok &= writeUInt(L"Shadows", L"Scale", config.shadowScale);
    ok &= writeUInt(L"Reflections", L"Scale", config.reflectionScale);
    ok &= writeBool(L"DepthOfField", L"ImproveResolution", config.improveDofResolution);
    ok &= writeUInt(L"World", L"HighDetailDistanceScale", config.highDetailDistanceScale);
    ok &= writeBool(L"UI", L"Enabled", config.uiEnabled);

    wchar_t keyText[16] = L"F10";
    if (config.uiToggleKey >= VK_F1 && config.uiToggleKey <= VK_F12)
        swprintf_s(keyText, L"F%u", config.uiToggleKey - VK_F1 + 1);
    else
        swprintf_s(keyText, L"0x%02X", config.uiToggleKey);
    ok &= WritePrivateProfileStringW(L"UI", L"ToggleKey", keyText, path) != FALSE;

    if (ok)
        AppendLog("[Config] Editable settings saved to DPFixNG.ini.\n");
    else
        AppendLog("[Config] WARNING: One or more settings could not be saved.\n");

    return ok;
}

bool ResolveConfigForWindow(HWND window)
{
    if (window == nullptr)
    {
        AppendLog("[Config] ERROR: Cannot resolve display settings without a window.\n");
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(
        window,
        MONITOR_DEFAULTTONEAREST
    );

    if (monitor == nullptr)
    {
        AppendLog("[Config] ERROR: MonitorFromWindow failed while resolving settings.\n");
        return false;
    }

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfoW(monitor, &monitorInfo))
    {
        AppendLog("[Config] ERROR: GetMonitorInfoW failed while resolving settings.\n");
        return false;
    }

    const UINT monitorWidth =
        static_cast<UINT>(
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left
        );

    const UINT monitorHeight =
        static_cast<UINT>(
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top
        );

    const bool displayAuto =
        g_config.displayWidth == 0 &&
        g_config.displayHeight == 0;

    if (displayAuto)
    {
        g_displayWidth = monitorWidth;
        g_displayHeight = monitorHeight;
    }
    else if (g_config.displayWidth == 0 ||
             g_config.displayHeight == 0 ||
             !IsReasonableResolution(
                 g_config.displayWidth,
                 g_config.displayHeight))
    {
        AppendLog(
            "[Config] WARNING: Invalid Display resolution. "
            "Falling back to monitor native resolution.\n"
        );

        g_displayWidth = monitorWidth;
        g_displayHeight = monitorHeight;
    }
    else
    {
        g_displayWidth = g_config.displayWidth;
        g_displayHeight = g_config.displayHeight;
    }

    const bool hasExplicitInternal =
        g_config.internalWidth != 0 ||
        g_config.internalHeight != 0;

    bool useInternalScale = !hasExplicitInternal;

    if (hasExplicitInternal)
    {
        if (g_config.internalWidth != 0 &&
            g_config.internalHeight != 0 &&
            IsReasonableResolution(
                g_config.internalWidth,
                g_config.internalHeight))
        {
            g_internalWidth = g_config.internalWidth;
            g_internalHeight = g_config.internalHeight;
            useInternalScale = false;
        }
        else
        {
            AppendLog(
                "[Config] WARNING: Invalid explicit Internal resolution. "
                "Falling back to Rendering.InternalScale.\n"
            );
            useInternalScale = true;
        }
    }

    if (useInternalScale)
    {
        const double scaledWidth =
            static_cast<double>(g_displayWidth) *
            static_cast<double>(g_config.internalScale);

        const double scaledHeight =
            static_cast<double>(g_displayHeight) *
            static_cast<double>(g_config.internalScale);

        const UINT resolvedWidth =
            static_cast<UINT>(scaledWidth + 0.5);

        const UINT resolvedHeight =
            static_cast<UINT>(scaledHeight + 0.5);

        if (IsReasonableResolution(resolvedWidth, resolvedHeight))
        {
            g_internalWidth = resolvedWidth;
            g_internalHeight = resolvedHeight;
        }
        else
        {
            AppendLog(
                "[Config] WARNING: InternalScale resolved to an unsupported "
                "resolution. Falling back to Display resolution.\n"
            );
            g_internalWidth = g_displayWidth;
            g_internalHeight = g_displayHeight;
        }
    }

    char text[512] = {};

    sprintf_s(
        text,
        "[Config] Monitor=%u x %u, Display=%u x %u, "
        "Internal=%u x %u, InternalScale=%.2f, Borderless=%s, ShadowScale=%u, "
        "ReflectionScale=%u, ImproveDOF=%s, FixPixelOffset=%s, "
        "Profiler=%s Key=0x%02X\n",
        monitorWidth,
        monitorHeight,
        g_displayWidth,
        g_displayHeight,
        g_internalWidth,
        g_internalHeight,
        g_config.internalScale,
        g_config.borderless ? "true" : "false",
        g_config.shadowScale,
        g_config.reflectionScale,
        g_config.improveDofResolution ? "true" : "false",
        g_config.fixPixelOffset ? "true" : "false",
        g_config.profilerEnabled ? "true" : "false",
        g_config.profilerCaptureKey
    );

    AppendLog(text);

    return true;
}


