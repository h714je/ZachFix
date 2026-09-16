#include "config.h"
#include "logging.h"
#include "postfx_ao.h"
#include "postfx_bloom.h"
#include "postfx_dof.h"
#include "postfx_exposure.h"

#include <cmath>
#include <cstdio>
#include <cwchar>

ZachFixConfig g_config{};

UINT g_displayWidth = 1280;
UINT g_displayHeight = 720;
UINT g_internalWidth = 1280;
UINT g_internalHeight = 720;

namespace
{
static constexpr UINT kMinResolutionWidth = 640;
static constexpr UINT kMinResolutionHeight = 360;

bool BuildSiblingPath(const wchar_t* fileName, wchar_t* path, size_t pathCount)
{
    if (fileName == nullptr || path == nullptr || pathCount == 0)
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
    return wcscat_s(path, pathCount, fileName) == 0;
}

bool BuildConfigPath(wchar_t* path, size_t pathCount)
{
    return BuildSiblingPath(L"ZachFix.ini", path, pathCount);
}

bool ResolveConfigPathForLoad(wchar_t* path, size_t pathCount, bool* usedPreReleaseName)
{
    if (usedPreReleaseName != nullptr)
        *usedPreReleaseName = false;

    if (!BuildConfigPath(path, pathCount))
        return false;

    if (GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES)
        return true;

    // Private development builds before the public ZachFix rename used
    // DPFixNG.ini. Accept it as a one-way migration fallback; all saves go to
    // ZachFix.ini. This path is intentionally undocumented for new installs.
    if (!BuildSiblingPath(L"DPFixNG.ini", path, pathCount))
        return false;

    if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES)
    {
        // Return the public filename even when neither file exists so Win32 INI
        // APIs read defaults from the expected location.
        return BuildConfigPath(path, pathCount);
    }

    if (usedPreReleaseName != nullptr)
        *usedPreReleaseName = true;
    return true;
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


float ReadIniFloat(
    const wchar_t* path,
    const wchar_t* section,
    const wchar_t* key,
    float fallback)
{
    wchar_t fallbackText[64] = {};
    swprintf_s(fallbackText, L"%.6f", static_cast<double>(fallback));
    wchar_t value[64] = {};
    GetPrivateProfileStringW(section, key, fallbackText, value, 64, path);
    return ParseFloat(value, fallback);
}

UINT ReadIniResolutionDivisor(
    const wchar_t* path,
    const wchar_t* section,
    UINT fallback)
{
    const wchar_t* fallbackText = fallback == 1 ? L"Full" : fallback == 4 ? L"Quarter" : L"Half";
    wchar_t value[32] = {};
    GetPrivateProfileStringW(section, L"Resolution", fallbackText, value, 32, path);
    if (_wcsicmp(value, L"Full") == 0 || wcscmp(value, L"1") == 0) return 1;
    if (_wcsicmp(value, L"Half") == 0 || wcscmp(value, L"2") == 0) return 2;
    if (_wcsicmp(value, L"Quarter") == 0 || wcscmp(value, L"4") == 0) return 4;
    return fallback;
}

PostFxAoMode ReadPostFxAoMode(const wchar_t* path, PostFxAoMode fallback)
{
    wchar_t value[48] = {};
    const wchar_t* defaultText = L"Off";
    switch (fallback)
    {
    case PostFxAoMode::ShowRaw: defaultText = L"ShowRaw"; break;
    case PostFxAoMode::ShowFiltered: defaultText = L"ShowFiltered"; break;
    case PostFxAoMode::ShowEnhanced: defaultText = L"ShowEnhanced"; break;
    case PostFxAoMode::Composite: defaultText = L"CompositeHDR"; break;
    default: break;
    }
    GetPrivateProfileStringW(L"PostFX.AO", L"Mode", defaultText, value, 48, path);
    if (_wcsicmp(value, L"Off") == 0) return PostFxAoMode::Off;
    if (_wcsicmp(value, L"ShowRaw") == 0) return PostFxAoMode::ShowRaw;
    if (_wcsicmp(value, L"ShowFiltered") == 0) return PostFxAoMode::ShowFiltered;
    if (_wcsicmp(value, L"ShowEnhanced") == 0) return PostFxAoMode::ShowEnhanced;
    if (_wcsicmp(value, L"Composite") == 0 || _wcsicmp(value, L"CompositeHDR") == 0)
        return PostFxAoMode::Composite;
    return fallback;
}

const wchar_t* PostFxAoModeIniName(PostFxAoMode mode)
{
    switch (mode)
    {
    case PostFxAoMode::ShowRaw: return L"ShowRaw";
    case PostFxAoMode::ShowFiltered: return L"ShowFiltered";
    case PostFxAoMode::ShowEnhanced: return L"ShowEnhanced";
    case PostFxAoMode::Composite: return L"CompositeHDR";
    default: return L"Off";
    }
}

PostFxBloomMode ReadPostFxBloomMode(const wchar_t* path, PostFxBloomMode fallback)
{
    wchar_t value[48] = {};
    const wchar_t* defaultText = fallback == PostFxBloomMode::BloomNg
        ? L"BloomNG" : fallback == PostFxBloomMode::ShowBloom ? L"ShowBloom" : L"Legacy";
    GetPrivateProfileStringW(L"PostFX.Bloom", L"Mode", defaultText, value, 48, path);
    if (_wcsicmp(value, L"Legacy") == 0) return PostFxBloomMode::Legacy;
    if (_wcsicmp(value, L"BloomNG") == 0 || _wcsicmp(value, L"NG") == 0)
        return PostFxBloomMode::BloomNg;
    if (_wcsicmp(value, L"ShowBloom") == 0) return PostFxBloomMode::ShowBloom;
    return fallback;
}

const wchar_t* PostFxBloomModeIniName(PostFxBloomMode mode)
{
    switch (mode)
    {
    case PostFxBloomMode::BloomNg: return L"BloomNG";
    case PostFxBloomMode::ShowBloom: return L"ShowBloom";
    default: return L"Legacy";
    }
}

PostFxDofMode ReadPostFxDofMode(const wchar_t* path, PostFxDofMode fallback)
{
    wchar_t value[48] = {};
    const wchar_t* defaultText = L"Legacy";
    switch (fallback)
    {
    case PostFxDofMode::DofNg: defaultText = L"DoFNG"; break;
    case PostFxDofMode::ShowCoC: defaultText = L"ShowCoC"; break;
    case PostFxDofMode::ShowNear: defaultText = L"ShowNear"; break;
    case PostFxDofMode::ShowFar: defaultText = L"ShowFar"; break;
    default: break;
    }
    GetPrivateProfileStringW(L"PostFX.DoF", L"Mode", defaultText, value, 48, path);
    if (_wcsicmp(value, L"Legacy") == 0) return PostFxDofMode::Legacy;
    if (_wcsicmp(value, L"DoFNG") == 0 || _wcsicmp(value, L"NG") == 0)
        return PostFxDofMode::DofNg;
    if (_wcsicmp(value, L"ShowCoC") == 0) return PostFxDofMode::ShowCoC;
    if (_wcsicmp(value, L"ShowNear") == 0) return PostFxDofMode::ShowNear;
    if (_wcsicmp(value, L"ShowFar") == 0) return PostFxDofMode::ShowFar;
    return fallback;
}

const wchar_t* PostFxDofModeIniName(PostFxDofMode mode)
{
    switch (mode)
    {
    case PostFxDofMode::DofNg: return L"DoFNG";
    case PostFxDofMode::ShowCoC: return L"ShowCoC";
    case PostFxDofMode::ShowNear: return L"ShowNear";
    case PostFxDofMode::ShowFar: return L"ShowFar";
    default: return L"Legacy";
    }
}

PostFxExposureMode ReadPostFxExposureMode(const wchar_t* path, PostFxExposureMode fallback)
{
    wchar_t value[64] = {};
    const wchar_t* defaultText = L"Legacy";
    switch (fallback)
    {
    case PostFxExposureMode::ExposureOnly: defaultText = L"AutoExposure"; break;
    case PostFxExposureMode::ShoulderOnly: defaultText = L"Shoulder"; break;
    case PostFxExposureMode::ExposureAndShoulder: defaultText = L"AutoExposureShoulder"; break;
    default: break;
    }
    GetPrivateProfileStringW(L"PostFX.Exposure", L"Mode", defaultText, value, 64, path);
    if (_wcsicmp(value, L"Legacy") == 0) return PostFxExposureMode::Legacy;
    if (_wcsicmp(value, L"AutoExposure") == 0 || _wcsicmp(value, L"ExposureOnly") == 0)
        return PostFxExposureMode::ExposureOnly;
    if (_wcsicmp(value, L"Shoulder") == 0 || _wcsicmp(value, L"ShoulderOnly") == 0)
        return PostFxExposureMode::ShoulderOnly;
    if (_wcsicmp(value, L"AutoExposureShoulder") == 0 ||
        _wcsicmp(value, L"ExposureAndShoulder") == 0)
        return PostFxExposureMode::ExposureAndShoulder;
    return fallback;
}

const wchar_t* PostFxExposureModeIniName(PostFxExposureMode mode)
{
    switch (mode)
    {
    case PostFxExposureMode::ExposureOnly: return L"AutoExposure";
    case PostFxExposureMode::ShoulderOnly: return L"Shoulder";
    case PostFxExposureMode::ExposureAndShoulder: return L"AutoExposureShoulder";
    default: return L"Legacy";
    }
}

const wchar_t* ResolutionDivisorIniName(UINT divisor)
{
    return divisor == 1 ? L"Full" : divisor == 4 ? L"Quarter" : L"Half";
}

void LoadPostFxSettingsFromPath(const wchar_t* path)
{
    const PostFxAoSettings ao = GetPostFxAoSettings();
    SetPostFxAoMode(ReadPostFxAoMode(path, ao.mode));
    SetPostFxAoRadius(ReadIniFloat(path, L"PostFX.AO", L"Radius", ao.radius));
    SetPostFxAoStrength(ReadIniFloat(path, L"PostFX.AO", L"Strength", ao.strength));
    SetPostFxAoBias(ReadIniFloat(path, L"PostFX.AO", L"Bias", ao.bias));
    SetPostFxAoThickness(ReadIniFloat(path, L"PostFX.AO", L"Thickness", ao.thickness));
    SetPostFxAoPower(ReadIniFloat(path, L"PostFX.AO", L"Power", ao.power));
    SetPostFxAoResolutionDivisor(ReadIniResolutionDivisor(path, L"PostFX.AO", ao.resolutionDivisor));

    const PostFxBloomSettings bloom = GetPostFxBloomSettings();
    SetPostFxBloomMode(ReadPostFxBloomMode(path, bloom.mode));
    SetPostFxBloomThresholdEv(ReadIniFloat(path, L"PostFX.Bloom", L"ThresholdEV", bloom.thresholdEv));
    SetPostFxBloomSoftKnee(ReadIniFloat(path, L"PostFX.Bloom", L"SoftKnee", bloom.softKnee));
    SetPostFxBloomIntensity(ReadIniFloat(path, L"PostFX.Bloom", L"Intensity", bloom.intensity));
    SetPostFxBloomScatter(ReadIniFloat(path, L"PostFX.Bloom", L"Scatter", bloom.scatter));
    SetPostFxBloomMaxLevels(GetPrivateProfileIntW(L"PostFX.Bloom", L"Levels", bloom.maxLevels, path));

    const PostFxDofSettings dof = GetPostFxDofSettings();
    SetPostFxDofMode(ReadPostFxDofMode(path, dof.mode));
    SetPostFxDofMaxRadiusPixels(ReadIniFloat(path, L"PostFX.DoF", L"MaxRadiusPixels", dof.maxRadiusPixels));
    SetPostFxDofNearStrength(ReadIniFloat(path, L"PostFX.DoF", L"NearStrength", dof.nearStrength));
    SetPostFxDofFarStrength(ReadIniFloat(path, L"PostFX.DoF", L"FarStrength", dof.farStrength));
    SetPostFxDofDepthReject(ReadIniFloat(path, L"PostFX.DoF", L"DepthReject", dof.depthReject));
    SetPostFxDofHighlightBoost(ReadIniFloat(path, L"PostFX.DoF", L"HighlightBoost", dof.highlightBoost));
    SetPostFxDofResolutionDivisor(ReadIniResolutionDivisor(path, L"PostFX.DoF", dof.resolutionDivisor));

    const PostFxExposureSettings exposure = GetPostFxExposureSettings();
    SetPostFxExposureMode(ReadPostFxExposureMode(path, exposure.mode));
    SetPostFxExposureCompensationEv(ReadIniFloat(path, L"PostFX.Exposure", L"CompensationEV", exposure.compensationEv));
    SetPostFxExposureMeterMinEv(ReadIniFloat(path, L"PostFX.Exposure", L"MeterMinEV", exposure.meterMinEv));
    SetPostFxExposureMeterMaxEv(ReadIniFloat(path, L"PostFX.Exposure", L"MeterMaxEV", exposure.meterMaxEv));
    SetPostFxExposureMinEv(ReadIniFloat(path, L"PostFX.Exposure", L"MinExposureEV", exposure.minExposureEv));
    SetPostFxExposureMaxEv(ReadIniFloat(path, L"PostFX.Exposure", L"MaxExposureEV", exposure.maxExposureEv));
    SetPostFxExposureBrightenSpeed(ReadIniFloat(path, L"PostFX.Exposure", L"BrightenSpeed", exposure.brightenSpeed));
    SetPostFxExposureDarkenSpeed(ReadIniFloat(path, L"PostFX.Exposure", L"DarkenSpeed", exposure.darkenSpeed));
    SetPostFxExposureShoulderStrength(ReadIniFloat(path, L"PostFX.Exposure", L"ShoulderStrength", exposure.shoulderStrength));
    SetPostFxExposureWhitePoint(ReadIniFloat(path, L"PostFX.Exposure", L"WhitePoint", exposure.whitePoint));
    RequestPostFxExposureAdaptationReset();
}



} // namespace

void LoadConfig()
{
    wchar_t path[MAX_PATH] = {};
    bool usedPreReleaseName = false;

    if (!ResolveConfigPathForLoad(path, MAX_PATH, &usedPreReleaseName))
    {
        AppendLog("[Config] WARNING: Could not build ZachFix.ini path. Using defaults.\n");
        return;
    }

    if (usedPreReleaseName)
    {
        AppendLog(
            "[Config] Migration: ZachFix.ini not found; loading pre-release DPFixNG.ini. "
            "Save from the UI to create ZachFix.ini.\n");
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

    wchar_t improveShadowPrecisionText[32] = L"false";

    GetPrivateProfileStringW(
        L"Shadows",
        L"ImprovePrecision",
        L"false",
        improveShadowPrecisionText,
        static_cast<DWORD>(
            sizeof(improveShadowPrecisionText) /
            sizeof(improveShadowPrecisionText[0])
        ),
        path
    );

    g_config.improveShadowPrecision =
        ParseBool(improveShadowPrecisionText, false);

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

    wchar_t pauseWhileOpenText[32] = L"false";
    GetPrivateProfileStringW(
        L"UI",
        L"PauseGameWhileOpen",
        L"false",
        pauseWhileOpenText,
        static_cast<DWORD>(sizeof(pauseWhileOpenText) / sizeof(pauseWhileOpenText[0])),
        path);
    g_config.pauseGameWhileUiOpen = ParseBool(pauseWhileOpenText, false);

    wchar_t nativeXInputText[32] = L"false";
    GetPrivateProfileStringW(
        L"Gamepad",
        L"NativeXInput",
        L"false",
        nativeXInputText,
        static_cast<DWORD>(sizeof(nativeXInputText) / sizeof(nativeXInputText[0])),
        path);
    g_config.nativeXInputEnabled = ParseBool(nativeXInputText, false);

    wchar_t autoInputModeSwitchText[32] = L"false";
    GetPrivateProfileStringW(
        L"Input",
        L"AutoSwitch",
        L"false",
        autoInputModeSwitchText,
        static_cast<DWORD>(sizeof(autoInputModeSwitchText) / sizeof(autoInputModeSwitchText[0])),
        path);
    g_config.autoInputModeSwitch = ParseBool(autoInputModeSwitchText, false);

    wchar_t dynamicGlyphAtlasText[32] = L"false";
    GetPrivateProfileStringW(
        L"Glyphs",
        L"DynamicAtlas",
        L"false",
        dynamicGlyphAtlasText,
        static_cast<DWORD>(sizeof(dynamicGlyphAtlasText) / sizeof(dynamicGlyphAtlasText[0])),
        path);
    g_config.dynamicGlyphAtlas = ParseBool(dynamicGlyphAtlasText, false);

    wchar_t glyphHotReloadText[32] = L"true";
    GetPrivateProfileStringW(
        L"Glyphs",
        L"HotReload",
        L"true",
        glyphHotReloadText,
        static_cast<DWORD>(sizeof(glyphHotReloadText) / sizeof(glyphHotReloadText[0])),
        path);
    g_config.glyphHotReload = ParseBool(glyphHotReloadText, true);

    GetPrivateProfileStringW(
        L"Glyphs",
        L"KeyboardSet",
        L"Native",
        g_config.keyboardGlyphSet,
        static_cast<DWORD>(sizeof(g_config.keyboardGlyphSet) / sizeof(g_config.keyboardGlyphSet[0])),
        path);
    if (g_config.keyboardGlyphSet[0] == L'\0')
        wcscpy_s(
            g_config.keyboardGlyphSet,
            sizeof(g_config.keyboardGlyphSet) / sizeof(g_config.keyboardGlyphSet[0]),
            L"Native");

    GetPrivateProfileStringW(
        L"Glyphs",
        L"GamepadSet",
        L"xbox",
        g_config.gamepadGlyphSet,
        static_cast<DWORD>(sizeof(g_config.gamepadGlyphSet) / sizeof(g_config.gamepadGlyphSet[0])),
        path);
    if (g_config.gamepadGlyphSet[0] == L'\0')
        wcscpy_s(
            g_config.gamepadGlyphSet,
            sizeof(g_config.gamepadGlyphSet) / sizeof(g_config.gamepadGlyphSet[0]),
            L"xbox");

    LoadPostFxSettingsFromPath(path);

    char text[1152] = {};

    sprintf_s(
        text,
        "[Config] Requested Display=%u x %u, Borderless=%s, "
        "Internal=%u x %u, InternalScale=%.2f, ShadowScale=%u, ShadowPrecision=%s, ReflectionScale=%u, "
        "ImproveDOF=%s, AdditionalDOFBlur=%u, FixPixelOffset=%s, HighDetailDistanceScale=%u, "
        "TextureOverride=%s, TextureDeveloperMode=%s, DumpTextures=%s, TextureDimensionMode=%s, "
        "Filtering=%s, MaxAnisotropy=%ux, UI=%s UIKey=0x%02X PauseWhileOpen=%s, "
        "NativeXInput=%s, AutoInputSwitch=%s, DynamicGlyphAtlas=%s, GlyphHotReload=%s, "
        "KeyboardGlyphSet=%ls, GamepadGlyphSet=%ls\n",
        g_config.displayWidth,
        g_config.displayHeight,
        g_config.borderless ? "true" : "false",
        g_config.internalWidth,
        g_config.internalHeight,
        g_config.internalScale,
        g_config.shadowScale,
        g_config.improveShadowPrecision ? "D32F" : "D16",
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
        g_config.uiToggleKey,
        g_config.pauseGameWhileUiOpen ? "true" : "false",
        g_config.nativeXInputEnabled ? "true" : "false",
        g_config.autoInputModeSwitch ? "true" : "false",
        g_config.dynamicGlyphAtlas ? "true" : "false",
        g_config.glyphHotReload ? "true" : "false",
        g_config.keyboardGlyphSet,
        g_config.gamepadGlyphSet
    );

    AppendLog(text);
}



bool GetConfigFilePath(wchar_t* path, size_t pathCount)
{
    return ResolveConfigPathForLoad(path, pathCount, nullptr);
}

bool SaveEditableConfig(const ZachFixConfig& config)
{
    wchar_t path[MAX_PATH] = {};
    if (!BuildConfigPath(path, MAX_PATH))
    {
        AppendLog("[Config] ERROR: Could not build ZachFix.ini path for save.\n");
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
        swprintf_s(buffer, L"%.4f", static_cast<double>(value));
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
    ok &= writeBool(L"Shadows", L"ImprovePrecision", config.improveShadowPrecision);
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
    ok &= writeBool(L"UI", L"PauseGameWhileOpen", config.pauseGameWhileUiOpen);
    ok &= writeBool(L"Glyphs", L"DynamicAtlas", config.dynamicGlyphAtlas);
    ok &= writeBool(L"Glyphs", L"HotReload", config.glyphHotReload);
    ok &= WritePrivateProfileStringW(
        L"Glyphs", L"KeyboardSet", config.keyboardGlyphSet, path) != FALSE;
    ok &= WritePrivateProfileStringW(
        L"Glyphs", L"GamepadSet", config.gamepadGlyphSet, path) != FALSE;

    wchar_t keyText[16] = L"F10";
    if (config.uiToggleKey >= VK_F1 && config.uiToggleKey <= VK_F12)
        swprintf_s(keyText, L"F%u", config.uiToggleKey - VK_F1 + 1);
    else
        swprintf_s(keyText, L"0x%02X", config.uiToggleKey);
    ok &= WritePrivateProfileStringW(L"UI", L"ToggleKey", keyText, path) != FALSE;

    const PostFxAoSettings ao = GetPostFxAoSettings();
    ok &= WritePrivateProfileStringW(L"PostFX.AO", L"Mode", PostFxAoModeIniName(ao.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.AO", L"Radius", ao.radius);
    ok &= writeFloat(L"PostFX.AO", L"Strength", ao.strength);
    ok &= writeFloat(L"PostFX.AO", L"Bias", ao.bias);
    ok &= writeFloat(L"PostFX.AO", L"Thickness", ao.thickness);
    ok &= writeFloat(L"PostFX.AO", L"Power", ao.power);
    ok &= WritePrivateProfileStringW(L"PostFX.AO", L"Resolution", ResolutionDivisorIniName(ao.resolutionDivisor), path) != FALSE;

    const PostFxBloomSettings bloom = GetPostFxBloomSettings();
    ok &= WritePrivateProfileStringW(L"PostFX.Bloom", L"Mode", PostFxBloomModeIniName(bloom.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.Bloom", L"ThresholdEV", bloom.thresholdEv);
    ok &= writeFloat(L"PostFX.Bloom", L"SoftKnee", bloom.softKnee);
    ok &= writeFloat(L"PostFX.Bloom", L"Intensity", bloom.intensity);
    ok &= writeFloat(L"PostFX.Bloom", L"Scatter", bloom.scatter);
    ok &= writeUInt(L"PostFX.Bloom", L"Levels", bloom.maxLevels);

    const PostFxDofSettings dof = GetPostFxDofSettings();
    ok &= WritePrivateProfileStringW(L"PostFX.DoF", L"Mode", PostFxDofModeIniName(dof.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.DoF", L"MaxRadiusPixels", dof.maxRadiusPixels);
    ok &= writeFloat(L"PostFX.DoF", L"NearStrength", dof.nearStrength);
    ok &= writeFloat(L"PostFX.DoF", L"FarStrength", dof.farStrength);
    ok &= writeFloat(L"PostFX.DoF", L"DepthReject", dof.depthReject);
    ok &= writeFloat(L"PostFX.DoF", L"HighlightBoost", dof.highlightBoost);
    ok &= WritePrivateProfileStringW(L"PostFX.DoF", L"Resolution", ResolutionDivisorIniName(dof.resolutionDivisor), path) != FALSE;

    const PostFxExposureSettings exposure = GetPostFxExposureSettings();
    ok &= WritePrivateProfileStringW(L"PostFX.Exposure", L"Mode", PostFxExposureModeIniName(exposure.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.Exposure", L"CompensationEV", exposure.compensationEv);
    ok &= writeFloat(L"PostFX.Exposure", L"MeterMinEV", exposure.meterMinEv);
    ok &= writeFloat(L"PostFX.Exposure", L"MeterMaxEV", exposure.meterMaxEv);
    ok &= writeFloat(L"PostFX.Exposure", L"MinExposureEV", exposure.minExposureEv);
    ok &= writeFloat(L"PostFX.Exposure", L"MaxExposureEV", exposure.maxExposureEv);
    ok &= writeFloat(L"PostFX.Exposure", L"BrightenSpeed", exposure.brightenSpeed);
    ok &= writeFloat(L"PostFX.Exposure", L"DarkenSpeed", exposure.darkenSpeed);
    ok &= writeFloat(L"PostFX.Exposure", L"ShoulderStrength", exposure.shoulderStrength);
    ok &= writeFloat(L"PostFX.Exposure", L"WhitePoint", exposure.whitePoint);

    if (ok)
        AppendLog("[Config] Editable settings saved to ZachFix.ini.\n");
    else
        AppendLog("[Config] WARNING: One or more settings could not be saved.\n");

    return ok;
}

bool ReloadPostFxConfigFromIni()
{
    wchar_t path[MAX_PATH] = {};
    if (!ResolveConfigPathForLoad(path, MAX_PATH, nullptr))
        return false;

    LoadPostFxSettingsFromPath(path);
    AppendLog("[Config] PostFX settings reloaded from ZachFix.ini.\n");
    return true;
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
        "ShadowPrecision=%s, ReflectionScale=%u, ImproveDOF=%s, FixPixelOffset=%s\n",
        monitorWidth,
        monitorHeight,
        g_displayWidth,
        g_displayHeight,
        g_internalWidth,
        g_internalHeight,
        g_config.internalScale,
        g_config.borderless ? "true" : "false",
        g_config.shadowScale,
        g_config.improveShadowPrecision ? "D32F" : "D16",
        g_config.reflectionScale,
        g_config.improveDofResolution ? "true" : "false",
        g_config.fixPixelOffset ? "true" : "false"
    );

    AppendLog(text);

    return true;
}


