#include "config.h"
#include "logging.h"
#include "postfx_ao.h"
#include "postfx_bloom.h"
#include "postfx_dof.h"
#include "postfx_exposure.h"
#include "postfx_tuning.h"

#include <cmath>
#include <cstdio>
#include <cwchar>
#include <iterator>

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

GamepadInputProfile ParseGamepadInputProfile(
    const wchar_t* value,
    GamepadInputProfile defaultValue)
{
    if (value == nullptr || value[0] == L'\0')
        return defaultValue;

    if (_wcsicmp(value, L"PC") == 0 ||
        _wcsicmp(value, L"Vanilla") == 0 ||
        wcscmp(value, L"0") == 0)
    {
        return GamepadInputProfile::PC;
    }

    if (_wcsicmp(value, L"Xbox360") == 0 ||
        _wcsicmp(value, L"Xbox 360") == 0 ||
        _wcsicmp(value, L"Xbox") == 0 ||
        wcscmp(value, L"1") == 0)
    {
        return GamepadInputProfile::Xbox360;
    }

    return defaultValue;
}

const char* GamepadInputProfileLogName(GamepadInputProfile profile)
{
    return profile == GamepadInputProfile::Xbox360 ? "Xbox360" : "PC";
}

const wchar_t* GamepadInputProfileIniName(GamepadInputProfile profile)
{
    return profile == GamepadInputProfile::Xbox360 ? L"Xbox360" : L"PC";
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
    const wchar_t* defaultText = fallback == PostFxBloomMode::Bloom
        ? L"BloomNG" : fallback == PostFxBloomMode::ShowBloom ? L"ShowBloom" : L"Legacy";
    GetPrivateProfileStringW(L"PostFX.Bloom", L"Mode", defaultText, value, 48, path);
    if (_wcsicmp(value, L"Legacy") == 0) return PostFxBloomMode::Legacy;
    if (_wcsicmp(value, L"BloomNG") == 0 ||
        _wcsicmp(value, L"Bloom") == 0 ||
        _wcsicmp(value, L"NG") == 0)
    {
        return PostFxBloomMode::Bloom;
    }
    if (_wcsicmp(value, L"ShowBloom") == 0) return PostFxBloomMode::ShowBloom;
    return fallback;
}

const wchar_t* PostFxBloomModeIniName(PostFxBloomMode mode)
{
    switch (mode)
    {
    case PostFxBloomMode::Bloom: return L"BloomNG";
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
    case PostFxDofMode::DepthOfField: defaultText = L"DoFNG"; break;
    case PostFxDofMode::ShowCoC: defaultText = L"ShowCoC"; break;
    case PostFxDofMode::ShowNear: defaultText = L"ShowNear"; break;
    case PostFxDofMode::ShowFar: defaultText = L"ShowFar"; break;
    default: break;
    }
    GetPrivateProfileStringW(L"PostFX.DoF", L"Mode", defaultText, value, 48, path);
    if (_wcsicmp(value, L"Legacy") == 0) return PostFxDofMode::Legacy;
    if (_wcsicmp(value, L"DoFNG") == 0 ||
        _wcsicmp(value, L"DepthOfField") == 0 ||
        _wcsicmp(value, L"DoF") == 0 ||
        _wcsicmp(value, L"NG") == 0)
    {
        return PostFxDofMode::DepthOfField;
    }
    if (_wcsicmp(value, L"ShowCoC") == 0) return PostFxDofMode::ShowCoC;
    if (_wcsicmp(value, L"ShowNear") == 0) return PostFxDofMode::ShowNear;
    if (_wcsicmp(value, L"ShowFar") == 0) return PostFxDofMode::ShowFar;
    return fallback;
}

const wchar_t* PostFxDofModeIniName(PostFxDofMode mode)
{
    switch (mode)
    {
    case PostFxDofMode::DepthOfField: return L"DoFNG";
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

PostFxColorGradeMode ReadPostFxColorGradeMode(
    const wchar_t* path, PostFxColorGradeMode fallback)
{
    wchar_t value[64] = {};
    const wchar_t* defaultText = L"PC";
    if (fallback == PostFxColorGradeMode::Xbox360Grading)
        defaultText = L"Xbox360Grading";
    else if (fallback == PostFxColorGradeMode::Xbox360Full)
        defaultText = L"Xbox360Full";
    GetPrivateProfileStringW(
        L"PostFX.Color", L"Mode", defaultText, value, 64, path);
    if (_wcsicmp(value, L"PC") == 0 ||
        _wcsicmp(value, L"PcDirectorsCut") == 0 ||
        _wcsicmp(value, L"DirectorsCut") == 0)
    {
        return PostFxColorGradeMode::PcDirectorsCut;
    }
    if (_wcsicmp(value, L"Xbox360") == 0 ||
        _wcsicmp(value, L"Xbox") == 0 ||
        _wcsicmp(value, L"Xbox360Grading") == 0 ||
        _wcsicmp(value, L"XboxGrading") == 0)
    {
        // Backward compatibility: v1's Xbox360 token meant grading only.
        return PostFxColorGradeMode::Xbox360Grading;
    }
    if (_wcsicmp(value, L"Xbox360Full") == 0 ||
        _wcsicmp(value, L"XboxFull") == 0)
    {
        return PostFxColorGradeMode::Xbox360Full;
    }
    return fallback;
}

const wchar_t* PostFxColorGradeModeIniName(PostFxColorGradeMode mode)
{
    switch (mode)
    {
    case PostFxColorGradeMode::Xbox360Grading: return L"Xbox360Grading";
    case PostFxColorGradeMode::Xbox360Full: return L"Xbox360Full";
    default: return L"PC";
    }
}


PostFxDisplayGammaMode ReadPostFxDisplayGammaMode(
    const wchar_t* path, PostFxDisplayGammaMode fallback)
{
    wchar_t value[64] = {};
    const wchar_t* defaultText =
        fallback == PostFxDisplayGammaMode::Xbox360HdtvBt709
            ? L"Xbox360HDTV"
            : L"PC";
    GetPrivateProfileStringW(
        L"PostFX.Color", L"DisplayGamma", defaultText, value, 64, path);
    if (_wcsicmp(value, L"PC") == 0 ||
        _wcsicmp(value, L"sRGB") == 0)
    {
        return PostFxDisplayGammaMode::PcSrgb;
    }
    if (_wcsicmp(value, L"Xbox360HDTV") == 0 ||
        _wcsicmp(value, L"XboxHDTV") == 0 ||
        _wcsicmp(value, L"BT709") == 0 ||
        _wcsicmp(value, L"Xbox360") == 0)
    {
        return PostFxDisplayGammaMode::Xbox360HdtvBt709;
    }
    return fallback;
}

const wchar_t* PostFxDisplayGammaModeIniName(PostFxDisplayGammaMode mode)
{
    return mode == PostFxDisplayGammaMode::Xbox360HdtvBt709
        ? L"Xbox360HDTV"
        : L"PC";
}

const wchar_t* ResolutionDivisorIniName(UINT divisor)
{
    return divisor == 1 ? L"Full" : divisor == 4 ? L"Quarter" : L"Half";
}

void LoadPostFxSettingsFromPath(const wchar_t* path)
{
    const PostFxTuningSnapshot defaults{};
    const PostFxAoSettings& ao = defaults.ao;
    SetPostFxAoMode(ReadPostFxAoMode(path, ao.mode));
    SetPostFxAoRadius(ReadIniFloat(path, L"PostFX.AO", L"Radius", ao.radius));
    SetPostFxAoStrength(ReadIniFloat(path, L"PostFX.AO", L"Strength", ao.strength));
    SetPostFxAoBias(ReadIniFloat(path, L"PostFX.AO", L"Bias", ao.bias));
    SetPostFxAoThickness(ReadIniFloat(path, L"PostFX.AO", L"Thickness", ao.thickness));
    SetPostFxAoPower(ReadIniFloat(path, L"PostFX.AO", L"Power", ao.power));
    SetPostFxAoResolutionDivisor(ReadIniResolutionDivisor(path, L"PostFX.AO", ao.resolutionDivisor));

    const PostFxBloomSettings& bloom = defaults.bloom;
    SetPostFxBloomMode(ReadPostFxBloomMode(path, bloom.mode));
    SetPostFxBloomThresholdEv(ReadIniFloat(path, L"PostFX.Bloom", L"ThresholdEV", bloom.thresholdEv));
    SetPostFxBloomSoftKnee(ReadIniFloat(path, L"PostFX.Bloom", L"SoftKnee", bloom.softKnee));
    SetPostFxBloomIntensity(ReadIniFloat(path, L"PostFX.Bloom", L"Intensity", bloom.intensity));
    SetPostFxBloomScatter(ReadIniFloat(path, L"PostFX.Bloom", L"Scatter", bloom.scatter));
    SetPostFxBloomMaxLevels(GetPrivateProfileIntW(L"PostFX.Bloom", L"Levels", bloom.maxLevels, path));

    const PostFxDofSettings& dof = defaults.dof;
    SetPostFxDofMode(ReadPostFxDofMode(path, dof.mode));
    SetPostFxDofMaxRadiusPixels(ReadIniFloat(path, L"PostFX.DoF", L"MaxRadiusPixels", dof.maxRadiusPixels));
    SetPostFxDofNearStrength(ReadIniFloat(path, L"PostFX.DoF", L"NearStrength", dof.nearStrength));
    SetPostFxDofFarStrength(ReadIniFloat(path, L"PostFX.DoF", L"FarStrength", dof.farStrength));
    SetPostFxDofDepthReject(ReadIniFloat(path, L"PostFX.DoF", L"DepthReject", dof.depthReject));
    SetPostFxDofHighlightBoost(ReadIniFloat(path, L"PostFX.DoF", L"HighlightBoost", dof.highlightBoost));
    SetPostFxDofResolutionDivisor(ReadIniResolutionDivisor(path, L"PostFX.DoF", dof.resolutionDivisor));


    SetPostFxColorGradeMode(ReadPostFxColorGradeMode(
        path, defaults.colorGradeMode));
    SetPostFxDisplayGammaMode(ReadPostFxDisplayGammaMode(
        path, defaults.displayGammaMode));

    const PostFxExposureSettings& exposure = defaults.exposure;
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

bool LoadConfigFromIni(const wchar_t* path, ZachFixConfig& result)
{
    if (path == nullptr || path[0] == L'\0')
        return false;

    // Parse every ZachFixConfig field from the same canonical defaults on both
    // startup and F10 Reload. This deliberately avoids using the current live
    // config as a fallback, so malformed/missing values cannot resolve
    // differently depending on when the file is read.
    ZachFixConfig next{};

    next.displayWidth = GetPrivateProfileIntW(L"Display", L"Width", next.displayWidth, path);
    next.displayHeight = GetPrivateProfileIntW(L"Display", L"Height", next.displayHeight, path);

    wchar_t value[64] = {};

    GetPrivateProfileStringW(
        L"Display", L"Borderless", next.borderless ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.borderless = ParseBool(value, next.borderless);

    next.internalWidth = GetPrivateProfileIntW(
        L"Rendering", L"InternalWidth", next.internalWidth, path);
    next.internalHeight = GetPrivateProfileIntW(
        L"Rendering", L"InternalHeight", next.internalHeight, path);
    next.internalScale = ReadIniFloat(
        path, L"Rendering", L"InternalScale", next.internalScale);
    if (next.internalScale < 0.25f || next.internalScale > 4.0f)
    {
        AppendLog(
            "[Config] WARNING: Rendering.InternalScale must be between "
            "0.25 and 4.0. Falling back to 1.0.\n");
        next.internalScale = 1.0f;
    }

    next.shadowScale = GetPrivateProfileIntW(
        L"Shadows", L"Scale", next.shadowScale, path);
    if (next.shadowScale < 1 || next.shadowScale > 8)
    {
        AppendLog(
            "[Config] WARNING: Shadows.Scale must be between 1 and 8. "
            "Falling back to 1.\n");
        next.shadowScale = 1;
    }

    GetPrivateProfileStringW(
        L"Shadows", L"ImprovePrecision", next.improveShadowPrecision ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.improveShadowPrecision = ParseBool(value, next.improveShadowPrecision);

    next.reflectionScale = GetPrivateProfileIntW(
        L"Reflections", L"Scale", next.reflectionScale, path);
    if (next.reflectionScale < 1 || next.reflectionScale > 8)
    {
        AppendLog(
            "[Config] WARNING: Reflections.Scale must be between 1 and 8. "
            "Falling back to 1.\n");
        next.reflectionScale = 1;
    }

    GetPrivateProfileStringW(
        L"DepthOfField", L"ImproveResolution", next.improveDofResolution ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.improveDofResolution = ParseBool(value, next.improveDofResolution);

    next.additionalDofBlur = GetPrivateProfileIntW(
        L"DepthOfField", L"AdditionalBlur", next.additionalDofBlur, path);
    if (next.additionalDofBlur > 2)
    {
        AppendLog(
            "[Config] WARNING: DepthOfField.AdditionalBlur must be 0, 1 or 2. "
            "Falling back to 0.\n");
        next.additionalDofBlur = 0;
    }

    GetPrivateProfileStringW(
        L"Rendering", L"FixPixelOffset", next.fixPixelOffset ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.fixPixelOffset = ParseBool(value, next.fixPixelOffset);

    next.highDetailDistanceScale = GetPrivateProfileIntW(
        L"World", L"HighDetailDistanceScale", next.highDetailDistanceScale, path);
    if (next.highDetailDistanceScale < 1 || next.highDetailDistanceScale > 2)
    {
        AppendLog(
            "[Config] WARNING: World.HighDetailDistanceScale currently supports "
            "only 1 (original) or 2 (extended outer ring). Falling back to 1.\n");
        next.highDetailDistanceScale = 1;
    }

    next.mainFrustumDistanceMode = GetPrivateProfileIntW(
        L"World", L"MainFrustumDistanceMode", next.mainFrustumDistanceMode, path);
    if (next.mainFrustumDistanceMode > 3)
    {
        AppendLog(
            "[Config] WARNING: World.MainFrustumDistanceMode supports "
            "0 (Original), 1 (Extended), 2 (Extended Plus) or 3 (Extreme). Falling back to 0.\n");
        next.mainFrustumDistanceMode = 0;
    }

    next.objectActivationDistanceScale = GetPrivateProfileIntW(
        L"World", L"ObjectActivationDistanceScale", next.objectActivationDistanceScale, path);
    if (next.objectActivationDistanceScale < 1 || next.objectActivationDistanceScale > 2)
    {
        AppendLog(
            "[Config] WARNING: World.ObjectActivationDistanceScale currently supports "
            "only 1 (1000 units) or 2 (2000 units). Falling back to 1.\n");
        next.objectActivationDistanceScale = 1;
    }

    next.objectLodDistanceScale = GetPrivateProfileIntW(
        L"World", L"ObjectLODDistanceScale", next.objectLodDistanceScale, path);
    if (next.objectLodDistanceScale < 1 || next.objectLodDistanceScale > 4)
    {
        AppendLog(
            "[Config] WARNING: World.ObjectLODDistanceScale supports "
            "1, 2, 3 or 4. Falling back to 1.\n");
        next.objectLodDistanceScale = 1;
    }

    next.alternate3dDistanceScale = GetPrivateProfileIntW(
        L"World", L"Alternate3DDistanceScale", next.alternate3dDistanceScale, path);
    if (next.alternate3dDistanceScale < 1 || next.alternate3dDistanceScale > 4)
    {
        AppendLog(
            "[Config] WARNING: World.Alternate3DDistanceScale supports "
            "1, 2, 3 or 4. Falling back to 1.\n");
        next.alternate3dDistanceScale = 1;
    }

    GetPrivateProfileStringW(
        L"World", L"FixInteriorOcclusionBugs", next.fixInteriorOcclusionBugs ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.fixInteriorOcclusionBugs = ParseBool(value, next.fixInteriorOcclusionBugs);

    GetPrivateProfileStringW(
        L"Textures", L"EnableOverride", next.enableTextureOverride ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.enableTextureOverride = ParseBool(value, next.enableTextureOverride);

    GetPrivateProfileStringW(
        L"Textures", L"DeveloperMode", next.textureDeveloperMode ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.textureDeveloperMode = ParseBool(value, next.textureDeveloperMode);

    GetPrivateProfileStringW(
        L"Textures", L"DumpTextures", next.dumpTextures ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.dumpTextures = ParseBool(value, next.dumpTextures);

    GetPrivateProfileStringW(
        L"Textures", L"DimensionMode", TextureDimensionModeIniName(next.textureDimensionMode),
        value, static_cast<DWORD>(std::size(value)), path);
    const TextureDimensionMode parsedDimensionMode =
        ParseTextureDimensionMode(value, next.textureDimensionMode);
    if (parsedDimensionMode == TextureDimensionMode::DPFix &&
        _wcsicmp(value, L"DPFix") != 0 &&
        _wcsicmp(value, L"Compatible") != 0 &&
        wcscmp(value, L"0") != 0)
    {
        AppendLog(
            "[Config] WARNING: Textures.DimensionMode must be DPFix or Preserve. "
            "Falling back to DPFix.\n");
    }
    next.textureDimensionMode = parsedDimensionMode;

    GetPrivateProfileStringW(
        L"Filtering", L"Mode", TextureFilteringModeIniName(next.textureFilteringMode),
        value, static_cast<DWORD>(std::size(value)), path);
    const TextureFilteringMode parsedFilteringMode =
        ParseTextureFilteringMode(value, next.textureFilteringMode);
    if (parsedFilteringMode == TextureFilteringMode::Original &&
        _wcsicmp(value, L"Original") != 0 &&
        _wcsicmp(value, L"Off") != 0 &&
        wcscmp(value, L"0") != 0)
    {
        AppendLog(
            "[Config] WARNING: Filtering.Mode must be Original, Bilinear or Anisotropic. "
            "Falling back to Original.\n");
    }
    next.textureFilteringMode = parsedFilteringMode;

    next.maxAnisotropy = GetPrivateProfileIntW(
        L"Filtering", L"MaxAnisotropy", next.maxAnisotropy, path);
    if (next.maxAnisotropy < 2 || next.maxAnisotropy > 16)
    {
        AppendLog(
            "[Config] WARNING: Filtering.MaxAnisotropy must be between 2 and 16. "
            "Falling back to 16.\n");
        next.maxAnisotropy = 16;
    }

    GetPrivateProfileStringW(
        L"UI", L"Enabled", next.uiEnabled ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.uiEnabled = ParseBool(value, next.uiEnabled);

    GetPrivateProfileStringW(
        L"UI", L"ToggleKey", L"F10", value,
        static_cast<DWORD>(std::size(value)), path);
    next.uiToggleKey = ParseVirtualKey(value, next.uiToggleKey);

    GetPrivateProfileStringW(
        L"UI", L"PauseGameWhileOpen", next.pauseGameWhileUiOpen ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.pauseGameWhileUiOpen = ParseBool(value, next.pauseGameWhileUiOpen);

    GetPrivateProfileStringW(
        L"Gamepad", L"NativeXInput", next.nativeXInputEnabled ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.nativeXInputEnabled = ParseBool(value, next.nativeXInputEnabled);

    GetPrivateProfileStringW(
        L"Gamepad", L"InputProfile", GamepadInputProfileIniName(next.gamepadInputProfile),
        value, static_cast<DWORD>(std::size(value)), path);
    next.gamepadInputProfile = ParseGamepadInputProfile(value, next.gamepadInputProfile);

    GetPrivateProfileStringW(
        L"Gamepad", L"AnalogVehicleTriggers", next.analogVehicleTriggers ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.analogVehicleTriggers = ParseBool(value, next.analogVehicleTriggers);

    next.vehicleTriggerDeadzone = GetPrivateProfileIntW(
        L"Gamepad", L"VehicleTriggerDeadzone", next.vehicleTriggerDeadzone, path);
    if (next.vehicleTriggerDeadzone > 254)
    {
        AppendLog(
            "[Config] WARNING: Gamepad.VehicleTriggerDeadzone must be between 0 and 254. "
            "Falling back to Xbox 360 default 30.\n");
        next.vehicleTriggerDeadzone = 30;
    }

    GetPrivateProfileStringW(
        L"Gamepad", L"Vibration", next.vibrationEnabled ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.vibrationEnabled = ParseBool(value, next.vibrationEnabled);

    next.vibrationStrength = ReadIniFloat(
        path, L"Gamepad", L"VibrationStrength", next.vibrationStrength);
    if (next.vibrationStrength < 0.0f || next.vibrationStrength > 1.0f)
    {
        AppendLog(
            "[Config] WARNING: Gamepad.VibrationStrength must be between 0.0 and 1.0. "
            "Falling back to 1.0.\n");
        next.vibrationStrength = 1.0f;
    }

    GetPrivateProfileStringW(
        L"Input", L"AutoSwitch", next.autoInputModeSwitch ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.autoInputModeSwitch = ParseBool(value, next.autoInputModeSwitch);

    // Controller feature: prefer the new [Gamepad] location while accepting
    // the pre-release [Gameplay] key as a migration fallback.
    GetPrivateProfileStringW(
        L"Gamepad", L"RestoreCombatStrafe", L"",
        value, static_cast<DWORD>(std::size(value)), path);
    if (value[0] == L'\0')
    {
        GetPrivateProfileStringW(
            L"Gameplay", L"RestoreCombatStrafe",
            next.restoreCombatStrafe ? L"true" : L"false",
            value, static_cast<DWORD>(std::size(value)), path);
    }
    next.restoreCombatStrafe = ParseBool(value, next.restoreCombatStrafe);

    GetPrivateProfileStringW(
        L"SaveSafety", L"Enabled", next.saveSafetyEnabled ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.saveSafetyEnabled = ParseBool(value, next.saveSafetyEnabled);

    next.saveSafetyBackupCount = GetPrivateProfileIntW(
        L"SaveSafety", L"BackupCount", next.saveSafetyBackupCount, path);
    if (next.saveSafetyBackupCount < 1 || next.saveSafetyBackupCount > 100)
    {
        AppendLog(
            "[Config] WARNING: SaveSafety.BackupCount must be between 1 and 100. "
            "Falling back to 10.\n");
        next.saveSafetyBackupCount = 10;
    }

    GetPrivateProfileStringW(
        L"Glyphs", L"DynamicAtlas", next.dynamicGlyphAtlas ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.dynamicGlyphAtlas = ParseBool(value, next.dynamicGlyphAtlas);

    GetPrivateProfileStringW(
        L"Glyphs", L"HotReload", next.glyphHotReload ? L"true" : L"false",
        value, static_cast<DWORD>(std::size(value)), path);
    next.glyphHotReload = ParseBool(value, next.glyphHotReload);

    wchar_t keyboardFallback[64] = {};
    wcscpy_s(keyboardFallback, next.keyboardGlyphSet);
    GetPrivateProfileStringW(
        L"Glyphs", L"KeyboardSet", keyboardFallback,
        next.keyboardGlyphSet,
        static_cast<DWORD>(std::size(next.keyboardGlyphSet)), path);
    if (next.keyboardGlyphSet[0] == L'\0')
        wcscpy_s(next.keyboardGlyphSet, L"Native");

    wchar_t gamepadFallback[64] = {};
    wcscpy_s(gamepadFallback, next.gamepadGlyphSet);
    GetPrivateProfileStringW(
        L"Glyphs", L"GamepadSet", gamepadFallback,
        next.gamepadGlyphSet,
        static_cast<DWORD>(std::size(next.gamepadGlyphSet)), path);
    if (next.gamepadGlyphSet[0] == L'\0')
        wcscpy_s(next.gamepadGlyphSet, L"xbox");

    result = next;
    return true;
}

void LoadConfig()
{
    wchar_t path[MAX_PATH] = {};
    bool usedPreReleaseName = false;

    if (!ResolveConfigPathForLoad(path, MAX_PATH, &usedPreReleaseName))
    {
        AppendLog("[Config] WARNING: Could not build ZachFix.ini path. Using defaults.\n");
        g_config = ZachFixConfig{};
        return;
    }

    if (usedPreReleaseName)
    {
        AppendLog(
            "[Config] Migration: ZachFix.ini not found; loading pre-release DPFixNG.ini. "
            "Save from the UI to create ZachFix.ini.\n");
    }

    ZachFixConfig loaded{};
    if (!LoadConfigFromIni(path, loaded))
    {
        AppendLog("[Config] WARNING: Could not parse ZachFix.ini. Using defaults.\n");
        loaded = ZachFixConfig{};
    }
    g_config = loaded;

    LoadPostFxSettingsFromPath(path);

    char text[1536] = {};

    sprintf_s(
        text,
        "[Config] Requested Display=%u x %u, Borderless=%s, "
        "Internal=%u x %u, InternalScale=%.2f, ShadowScale=%u, ShadowPrecision=%s, ReflectionScale=%u, "
        "ImproveDOF=%s, AdditionalDOFBlur=%u, FixPixelOffset=%s, HighDetailDistanceScale=%u, "
        "MainFrustumDistanceMode=%u, ObjectActivationDistanceScale=%u, "
        "ObjectLODDistanceScale=%u, Alternate3DDistanceScale=%u, "
        "FixInteriorOcclusionBugs=%s, "
        "TextureOverride=%s, TextureDeveloperMode=%s, DumpTextures=%s, TextureDimensionMode=%s, "
        "Filtering=%s, MaxAnisotropy=%ux, UI=%s UIKey=0x%02X PauseWhileOpen=%s, "
        "NativeXInput=%s, GamepadProfile=%s, AnalogVehicleTriggers=%s, "
        "VehicleTriggerDeadzone=%u, Vibration=%s, VibrationStrength=%.2f, "
        "AutoInputSwitch=%s, RestoreCombatStrafe=%s, SaveSafety=%s, SaveBackupCount=%u, "
        "DynamicGlyphAtlas=%s, GlyphHotReload=%s, "
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
        g_config.mainFrustumDistanceMode,
        g_config.objectActivationDistanceScale,
        g_config.objectLodDistanceScale,
        g_config.alternate3dDistanceScale,
        g_config.fixInteriorOcclusionBugs ? "true" : "false",
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
        GamepadInputProfileLogName(g_config.gamepadInputProfile),
        g_config.analogVehicleTriggers ? "true" : "false",
        g_config.vehicleTriggerDeadzone,
        g_config.vibrationEnabled ? "true" : "false",
        static_cast<double>(g_config.vibrationStrength),
        g_config.autoInputModeSwitch ? "true" : "false",
        g_config.restoreCombatStrafe ? "true" : "false",
        g_config.saveSafetyEnabled ? "true" : "false",
        g_config.saveSafetyBackupCount,
        g_config.dynamicGlyphAtlas ? "true" : "false",
        g_config.glyphHotReload ? "true" : "false",
        g_config.keyboardGlyphSet,
        g_config.gamepadGlyphSet);

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
    ok &= writeUInt(L"World", L"MainFrustumDistanceMode", config.mainFrustumDistanceMode);
    ok &= writeUInt(L"World", L"ObjectActivationDistanceScale", config.objectActivationDistanceScale);
    ok &= writeUInt(L"World", L"ObjectLODDistanceScale", config.objectLodDistanceScale);
    ok &= writeUInt(L"World", L"Alternate3DDistanceScale", config.alternate3dDistanceScale);
    ok &= writeBool(L"World", L"FixInteriorOcclusionBugs", config.fixInteriorOcclusionBugs);
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
    ok &= WritePrivateProfileStringW(
        L"Gamepad",
        L"InputProfile",
        GamepadInputProfileIniName(config.gamepadInputProfile),
        path) != FALSE;
    ok &= writeBool(
        L"Gamepad", L"AnalogVehicleTriggers", config.analogVehicleTriggers);
    ok &= writeUInt(L"Gamepad", L"VehicleTriggerDeadzone", config.vehicleTriggerDeadzone);
    ok &= writeBool(L"Gamepad", L"Vibration", config.vibrationEnabled);
    ok &= writeFloat(L"Gamepad", L"VibrationStrength", config.vibrationStrength);
    ok &= writeBool(L"Gamepad", L"RestoreCombatStrafe", config.restoreCombatStrafe);
    // Remove the pre-release location after persisting the new canonical key.
    ok &= WritePrivateProfileStringW(
        L"Gameplay", L"RestoreCombatStrafe", nullptr, path) != FALSE;
    ok &= writeBool(L"SaveSafety", L"Enabled", config.saveSafetyEnabled);
    ok &= writeUInt(L"SaveSafety", L"BackupCount", config.saveSafetyBackupCount);
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

    const PostFxTuningSnapshot tuning = GetPostFxTuningSnapshot();
    const PostFxAoSettings& ao = tuning.ao;
    ok &= WritePrivateProfileStringW(L"PostFX.AO", L"Mode", PostFxAoModeIniName(ao.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.AO", L"Radius", ao.radius);
    ok &= writeFloat(L"PostFX.AO", L"Strength", ao.strength);
    ok &= writeFloat(L"PostFX.AO", L"Bias", ao.bias);
    ok &= writeFloat(L"PostFX.AO", L"Thickness", ao.thickness);
    ok &= writeFloat(L"PostFX.AO", L"Power", ao.power);
    ok &= WritePrivateProfileStringW(L"PostFX.AO", L"Resolution", ResolutionDivisorIniName(ao.resolutionDivisor), path) != FALSE;

    const PostFxBloomSettings& bloom = tuning.bloom;
    ok &= WritePrivateProfileStringW(L"PostFX.Bloom", L"Mode", PostFxBloomModeIniName(bloom.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.Bloom", L"ThresholdEV", bloom.thresholdEv);
    ok &= writeFloat(L"PostFX.Bloom", L"SoftKnee", bloom.softKnee);
    ok &= writeFloat(L"PostFX.Bloom", L"Intensity", bloom.intensity);
    ok &= writeFloat(L"PostFX.Bloom", L"Scatter", bloom.scatter);
    ok &= writeUInt(L"PostFX.Bloom", L"Levels", bloom.maxLevels);

    const PostFxDofSettings& dof = tuning.dof;
    ok &= WritePrivateProfileStringW(L"PostFX.DoF", L"Mode", PostFxDofModeIniName(dof.mode), path) != FALSE;
    ok &= writeFloat(L"PostFX.DoF", L"MaxRadiusPixels", dof.maxRadiusPixels);
    ok &= writeFloat(L"PostFX.DoF", L"NearStrength", dof.nearStrength);
    ok &= writeFloat(L"PostFX.DoF", L"FarStrength", dof.farStrength);
    ok &= writeFloat(L"PostFX.DoF", L"DepthReject", dof.depthReject);
    ok &= writeFloat(L"PostFX.DoF", L"HighlightBoost", dof.highlightBoost);
    ok &= WritePrivateProfileStringW(L"PostFX.DoF", L"Resolution", ResolutionDivisorIniName(dof.resolutionDivisor), path) != FALSE;


    const PostFxColorGradeMode colorGradeMode = tuning.colorGradeMode;
    ok &= WritePrivateProfileStringW(
        L"PostFX.Color", L"Mode", PostFxColorGradeModeIniName(colorGradeMode), path) != FALSE;
    const PostFxDisplayGammaMode displayGammaMode = tuning.displayGammaMode;
    ok &= WritePrivateProfileStringW(
        L"PostFX.Color", L"DisplayGamma",
        PostFxDisplayGammaModeIniName(displayGammaMode), path) != FALSE;

    const PostFxExposureSettings& exposure = tuning.exposure;
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
