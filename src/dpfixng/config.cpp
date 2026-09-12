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


TextureDimensionMode ParseTextureDimensionMode(
    const wchar_t* value,
    TextureDimensionMode defaultValue)
{
    if (value == nullptr || value[0] == L'\0')
        return defaultValue;

    if (_wcsicmp(value, L"DPFix") == 0 ||
        _wcsicmp(value, L"Compatible") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return TextureDimensionMode::DPFix;
    }

    if (_wcsicmp(value, L"Preserve") == 0 ||
        _wcsicmp(value, L"NPOT") == 0 ||
        wcscmp(value, L"1") == 0)
    {
        return TextureDimensionMode::Preserve;
    }

    return defaultValue;
}

const char* TextureDimensionModeLogName(TextureDimensionMode mode)
{
    return mode == TextureDimensionMode::Preserve ? "Preserve" : "DPFix";
}

const wchar_t* TextureDimensionModeIniName(TextureDimensionMode mode)
{
    return mode == TextureDimensionMode::Preserve ? L"Preserve" : L"DPFix";
}


TextureFilteringMode ParseTextureFilteringMode(
    const wchar_t* value,
    TextureFilteringMode defaultValue)
{
    if (value == nullptr || value[0] == L'\0')
        return defaultValue;

    if (_wcsicmp(value, L"Original") == 0 ||
        _wcsicmp(value, L"Off") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return TextureFilteringMode::Original;
    }

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

    return defaultValue;
}

const char* TextureFilteringModeLogName(TextureFilteringMode mode)
{
    switch (mode)
    {
    case TextureFilteringMode::Bilinear: return "Bilinear";
    case TextureFilteringMode::Anisotropic: return "Anisotropic";
    default: return "Original";
    }
}

const wchar_t* TextureFilteringModeIniName(TextureFilteringMode mode)
{
    switch (mode)
    {
    case TextureFilteringMode::Bilinear: return L"Bilinear";
    case TextureFilteringMode::Anisotropic: return L"Anisotropic";
    default: return L"Original";
    }
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

    g_config.additionalDofBlur = GetPrivateProfileIntW(
        L"DepthOfField",
        L"AdditionalBlur",
        0,
        path
    );

    if (g_config.additionalDofBlur > 2)
    {
        AppendLog(
            "[Config] WARNING: DepthOfField.AdditionalBlur must be 0, 1 or 2. "
            "Falling back to 0.\n"
        );
        g_config.additionalDofBlur = 0;
    }

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

    wchar_t textureOverrideText[32] = L"true";

    GetPrivateProfileStringW(
        L"Textures",
        L"EnableOverride",
        L"true",
        textureOverrideText,
        static_cast<DWORD>(sizeof(textureOverrideText) / sizeof(textureOverrideText[0])),
        path
    );

    g_config.enableTextureOverride = ParseBool(textureOverrideText, true);

    wchar_t textureDeveloperModeText[32] = L"false";

    GetPrivateProfileStringW(
        L"Textures",
        L"DeveloperMode",
        L"false",
        textureDeveloperModeText,
        static_cast<DWORD>(sizeof(textureDeveloperModeText) / sizeof(textureDeveloperModeText[0])),
        path
    );

    g_config.textureDeveloperMode = ParseBool(textureDeveloperModeText, false);

    wchar_t dumpTexturesText[32] = L"false";

    GetPrivateProfileStringW(
        L"Textures",
        L"DumpTextures",
        L"false",
        dumpTexturesText,
        static_cast<DWORD>(sizeof(dumpTexturesText) / sizeof(dumpTexturesText[0])),
        path
    );

    g_config.dumpTextures = ParseBool(dumpTexturesText, false);

    wchar_t textureDimensionModeText[32] = L"DPFix";

    GetPrivateProfileStringW(
        L"Textures",
        L"DimensionMode",
        L"DPFix",
        textureDimensionModeText,
        static_cast<DWORD>(sizeof(textureDimensionModeText) / sizeof(textureDimensionModeText[0])),
        path
    );

    const TextureDimensionMode parsedTextureDimensionMode =
        ParseTextureDimensionMode(textureDimensionModeText, TextureDimensionMode::DPFix);
    if (parsedTextureDimensionMode == TextureDimensionMode::DPFix &&
        _wcsicmp(textureDimensionModeText, L"DPFix") != 0 &&
        _wcsicmp(textureDimensionModeText, L"Compatible") != 0 &&
        wcscmp(textureDimensionModeText, L"0") != 0)
    {
        AppendLog(
            "[Config] WARNING: Textures.DimensionMode must be DPFix or Preserve. "
            "Falling back to DPFix.\n"
        );
    }
    g_config.textureDimensionMode = parsedTextureDimensionMode;

    wchar_t textureFilteringModeText[32] = L"Original";

    GetPrivateProfileStringW(
        L"Filtering",
        L"Mode",
        L"Original",
        textureFilteringModeText,
        static_cast<DWORD>(sizeof(textureFilteringModeText) / sizeof(textureFilteringModeText[0])),
        path
    );

    const TextureFilteringMode parsedTextureFilteringMode =
        ParseTextureFilteringMode(textureFilteringModeText, TextureFilteringMode::Original);
    if (parsedTextureFilteringMode == TextureFilteringMode::Original &&
        _wcsicmp(textureFilteringModeText, L"Original") != 0 &&
        _wcsicmp(textureFilteringModeText, L"Off") != 0 &&
        wcscmp(textureFilteringModeText, L"0") != 0)
    {
        AppendLog(
            "[Config] WARNING: Filtering.Mode must be Original, Bilinear or Anisotropic. "
            "Falling back to Original.\n"
        );
    }
    g_config.textureFilteringMode = parsedTextureFilteringMode;

    g_config.maxAnisotropy = GetPrivateProfileIntW(
        L"Filtering",
        L"MaxAnisotropy",
        16,
        path
    );

    if (g_config.maxAnisotropy < 2 || g_config.maxAnisotropy > 16)
    {
        AppendLog(
            "[Config] WARNING: Filtering.MaxAnisotropy must be between 2 and 16. "
            "Falling back to 16.\n"
        );
        g_config.maxAnisotropy = 16;
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

    char text[896] = {};

    sprintf_s(
        text,
        "[Config] Requested Display=%u x %u, Borderless=%s, "
        "Internal=%u x %u, InternalScale=%.2f, ShadowScale=%u, ReflectionScale=%u, "
        "ImproveDOF=%s, AdditionalDOFBlur=%u, FixPixelOffset=%s, HighDetailDistanceScale=%u, "
        "TextureOverride=%s, TextureDeveloperMode=%s, DumpTextures=%s, TextureDimensionMode=%s, "
        "Filtering=%s, MaxAnisotropy=%ux, UI=%s UIKey=0x%02X\n",
        g_config.displayWidth,
        g_config.displayHeight,
        g_config.borderless ? "true" : "false",
        g_config.internalWidth,
        g_config.internalHeight,
        g_config.internalScale,
        g_config.shadowScale,
        g_config.reflectionScale,
        g_config.improveDofResolution ? "true" : "false",
        g_config.additionalDofBlur,
        g_config.fixPixelOffset ? "true" : "false",
        g_config.highDetailDistanceScale,
        g_config.enableTextureOverride ? "true" : "false",
        g_config.textureDeveloperMode ? "true" : "false",
        g_config.dumpTextures ? "true" : "false",
        TextureDimensionModeLogName(g_config.textureDimensionMode),
        TextureFilteringModeLogName(g_config.textureFilteringMode),
        g_config.maxAnisotropy,
        g_config.uiEnabled ? "true" : "false",
        g_config.uiToggleKey
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
    ok &= writeUInt(L"DepthOfField", L"AdditionalBlur", config.additionalDofBlur);
    ok &= writeUInt(L"World", L"HighDetailDistanceScale", config.highDetailDistanceScale);
    ok &= writeBool(L"Textures", L"EnableOverride", config.enableTextureOverride);
    ok &= writeBool(L"Textures", L"DeveloperMode", config.textureDeveloperMode);
    ok &= writeBool(L"Textures", L"DumpTextures", config.dumpTextures);
    ok &= WritePrivateProfileStringW(
        L"Textures",
        L"DimensionMode",
        TextureDimensionModeIniName(config.textureDimensionMode),
        path) != FALSE;
    ok &= WritePrivateProfileStringW(
        L"Filtering",
        L"Mode",
        TextureFilteringModeIniName(config.textureFilteringMode),
        path) != FALSE;
    ok &= writeUInt(L"Filtering", L"MaxAnisotropy", config.maxAnisotropy);
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
        "ReflectionScale=%u, ImproveDOF=%s, FixPixelOffset=%s\n",
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
        g_config.fixPixelOffset ? "true" : "false"
    );

    AppendLog(text);

    return true;
}


