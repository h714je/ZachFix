#include "screenshot_presets.h"

#include "config.h"
#include "gameplay_pause.h"
#include "logging.h"
#include "postfx_tuning.h"
#include "postfx_exposure.h"
#include "runtime_resources.h"
#include "ui_settings.h"

#include <Windows.h>
#include <d3d9.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cstdio>
#include <cwchar>
#include <cwctype>
#include <iterator>
#include <string>

namespace
{
constexpr UINT kMaxScreenshotPresets = 8;
constexpr UINT kDefaultSettleFrames = 6;
constexpr int kD3DXImageFileFormatPng = 3;

using D3DXSaveSurfaceToFileWFn = HRESULT (WINAPI*)(
    LPCWSTR destinationFile,
    int destinationFormat,
    IDirect3DSurface9* sourceSurface,
    const PALETTEENTRY* sourcePalette,
    const RECT* sourceRect);

struct ScreenshotPreset
{
    wchar_t name[48] = {};
    ZachFixConfig config{};
    PostFxTuningSnapshot tuning{};
};

struct ScreenshotConfig
{
    bool enabled = false;
    UINT cycleKey = VK_F6;
    UINT captureKey = VK_F7;
    UINT captureAllKey = VK_F8;
    UINT settleFrames = kDefaultSettleFrames;
    bool autoCaptureOnSwitch = false;
    bool pauseDuringCaptureAll = false;
    wchar_t directory[MAX_PATH] = L"ZachFix\\screenshots";
};

ScreenshotConfig g_screenshotConfig{};
std::array<ScreenshotPreset, kMaxScreenshotPresets> g_presets{};
UINT g_presetCount = 0;
int g_activePreset = -1;
ZachFixConfig g_configuredBase{};
PostFxTuningSnapshot g_configuredTuning{};

std::atomic_bool g_initialized{ false };
bool g_cycleKeyLatched = false;
bool g_captureKeyLatched = false;
bool g_captureAllKeyLatched = false;

UINT g_singleCaptureCountdown = 0;
int g_singleCapturePreset = -1;

bool g_batchActive = false;
UINT g_batchPresetIndex = 0;
UINT g_batchCountdown = 0;
bool g_batchOwnsPause = false;
ZachFixConfig g_batchRestoreConfig{};
PostFxTuningSnapshot g_batchRestoreTuning{};
int g_batchRestoreActivePreset = -1;
SYSTEMTIME g_batchTimestamp{};
unsigned long long g_manualCaptureSerial = 0;

D3DXSaveSurfaceToFileWFn g_saveSurfaceToFileW = nullptr;

std::wstring Trim(const wchar_t* text)
{
    if (text == nullptr)
        return {};

    const wchar_t* begin = text;
    while (*begin != L'\0' && iswspace(*begin))
        ++begin;

    const wchar_t* end = begin + wcslen(begin);
    while (end > begin && iswspace(end[-1]))
        --end;

    return std::wstring(begin, end);
}

bool TryReadString(
    const wchar_t* path,
    const wchar_t* section,
    const wchar_t* key,
    wchar_t* value,
    DWORD valueCount)
{
    if (value == nullptr || valueCount == 0)
        return false;

    value[0] = L'\0';
    const DWORD length = GetPrivateProfileStringW(
        section, key, L"", value, valueCount, path);
    return length != 0;
}

bool TryReadBool(
    const wchar_t* path,
    const wchar_t* section,
    const wchar_t* key,
    bool& value)
{
    wchar_t text[32] = {};
    if (!TryReadString(path, section, key, text, static_cast<DWORD>(std::size(text))))
        return false;

    if (_wcsicmp(text, L"true") == 0 ||
        _wcsicmp(text, L"yes") == 0 ||
        wcscmp(text, L"1") == 0 ||
        _wcsicmp(text, L"on") == 0)
    {
        value = true;
        return true;
    }

    if (_wcsicmp(text, L"false") == 0 ||
        _wcsicmp(text, L"no") == 0 ||
        wcscmp(text, L"0") == 0 ||
        _wcsicmp(text, L"off") == 0)
    {
        value = false;
        return true;
    }

    return false;
}

bool TryReadUInt(
    const wchar_t* path,
    const wchar_t* section,
    const wchar_t* key,
    UINT& value)
{
    wchar_t text[32] = {};
    if (!TryReadString(path, section, key, text, static_cast<DWORD>(std::size(text))))
        return false;

    wchar_t* end = nullptr;
    const unsigned long parsed = wcstoul(text, &end, 0);
    if (end == text || (end != nullptr && *end != L'\0'))
        return false;

    value = static_cast<UINT>(parsed);
    return true;
}

bool TryReadFloat(
    const wchar_t* path,
    const wchar_t* section,
    const wchar_t* key,
    float& value)
{
    wchar_t text[64] = {};
    if (!TryReadString(path, section, key, text, static_cast<DWORD>(std::size(text))))
        return false;

    wchar_t* end = nullptr;
    const double parsed = wcstod(text, &end);
    if (end == text || (end != nullptr && *end != L'\0'))
        return false;

    value = static_cast<float>(parsed);
    return true;
}

UINT ParseVirtualKey(const wchar_t* text, UINT fallback)
{
    const std::wstring trimmed = Trim(text);
    if (trimmed.empty())
        return fallback;

    if (trimmed.size() >= 2 &&
        (trimmed[0] == L'F' || trimmed[0] == L'f'))
    {
        wchar_t* end = nullptr;
        const long number = wcstol(trimmed.c_str() + 1, &end, 10);
        if (end != trimmed.c_str() + 1 && *end == L'\0' &&
            number >= 1 && number <= 24)
        {
            return VK_F1 + static_cast<UINT>(number - 1);
        }
    }

    if (trimmed.size() == 1)
    {
        const wchar_t ch = static_cast<wchar_t>(towupper(trimmed[0]));
        if ((ch >= L'A' && ch <= L'Z') || (ch >= L'0' && ch <= L'9'))
            return static_cast<UINT>(ch);
    }

    struct NamedKey
    {
        const wchar_t* name;
        UINT key;
    };

    static constexpr NamedKey kNamedKeys[] = {
        { L"HOME", VK_HOME },
        { L"END", VK_END },
        { L"INSERT", VK_INSERT },
        { L"DELETE", VK_DELETE },
        { L"PAUSE", VK_PAUSE },
        { L"SCROLLLOCK", VK_SCROLL },
        { L"NUMLOCK", VK_NUMLOCK },
    };

    for (const NamedKey& named : kNamedKeys)
    {
        if (_wcsicmp(trimmed.c_str(), named.name) == 0)
            return named.key;
    }

    wchar_t* end = nullptr;
    const unsigned long numeric = wcstoul(trimmed.c_str(), &end, 0);
    if (end != trimmed.c_str() && *end == L'\0' && numeric <= 0xffu)
        return static_cast<UINT>(numeric);

    return fallback;
}

void FormatKeyName(UINT key, char* output, size_t count)
{
    if (output == nullptr || count == 0)
        return;

    if (key >= VK_F1 && key <= VK_F24)
    {
        sprintf_s(output, count, "F%u", key - VK_F1 + 1);
        return;
    }

    if ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9'))
    {
        if (count >= 2)
        {
            output[0] = static_cast<char>(key);
            output[1] = '\0';
        }
        return;
    }

    sprintf_s(output, count, "0x%02X", key);
}

ZachFixConfig MakeOriginalConfig(const ZachFixConfig& configured)
{
    ZachFixConfig original = configured;
    original.internalWidth = 0;
    original.internalHeight = 0;
    original.internalScale = 1.0f;
    original.shadowScale = 1;
    // Shadow precision is restart-only. Keep the configured allocation format;
    // screenshot presets deliberately change only live-safe settings.
    original.reflectionScale = 1;
    original.improveDofResolution = false;
    original.additionalDofBlur = 0;
    original.fixPixelOffset = false;
    original.highDetailDistanceScale = 1;
    original.mainFrustumDistanceMode = 0;
    original.objectActivationDistanceScale = 1;
    original.objectLodDistanceScale = 1;
    original.fixInteriorOcclusionBugs = false;
    original.textureFilteringMode = TextureFilteringMode::Original;
    original.maxAnisotropy = std::clamp<UINT>(configured.maxAnisotropy, 2, 16);
    return original;
}

PostFxTuningSnapshot MakeOriginalTuning(const PostFxTuningSnapshot& configured)
{
    PostFxTuningSnapshot original = configured;
    original.ao.mode = PostFxAoMode::Off;
    original.bloom.mode = PostFxBloomMode::Legacy;
    original.dof.mode = PostFxDofMode::Legacy;
    original.exposure.mode = PostFxExposureMode::Legacy;
    original.colorGradeMode = PostFxColorGradeMode::PcDirectorsCut;
    original.displayGammaMode = PostFxDisplayGammaMode::PcSrgb;
    return original;
}

void ApplyPostFxSnapshot(const PostFxTuningSnapshot& tuning)
{
    SetPostFxAoMode(tuning.ao.mode);
    SetPostFxAoRadius(tuning.ao.radius);
    SetPostFxAoStrength(tuning.ao.strength);
    SetPostFxAoBias(tuning.ao.bias);
    SetPostFxAoThickness(tuning.ao.thickness);
    SetPostFxAoPower(tuning.ao.power);
    SetPostFxAoResolutionDivisor(tuning.ao.resolutionDivisor);

    SetPostFxBloomMode(tuning.bloom.mode);
    SetPostFxBloomThresholdEv(tuning.bloom.thresholdEv);
    SetPostFxBloomSoftKnee(tuning.bloom.softKnee);
    SetPostFxBloomIntensity(tuning.bloom.intensity);
    SetPostFxBloomScatter(tuning.bloom.scatter);
    SetPostFxBloomMaxLevels(tuning.bloom.maxLevels);

    SetPostFxDofMode(tuning.dof.mode);
    SetPostFxDofMaxRadiusPixels(tuning.dof.maxRadiusPixels);
    SetPostFxDofNearStrength(tuning.dof.nearStrength);
    SetPostFxDofFarStrength(tuning.dof.farStrength);
    SetPostFxDofDepthReject(tuning.dof.depthReject);
    SetPostFxDofHighlightBoost(tuning.dof.highlightBoost);
    SetPostFxDofResolutionDivisor(tuning.dof.resolutionDivisor);

    SetPostFxExposureMode(tuning.exposure.mode);
    SetPostFxExposureCompensationEv(tuning.exposure.compensationEv);
    SetPostFxExposureMeterMinEv(tuning.exposure.meterMinEv);
    SetPostFxExposureMeterMaxEv(tuning.exposure.meterMaxEv);
    SetPostFxExposureMinEv(tuning.exposure.minExposureEv);
    SetPostFxExposureMaxEv(tuning.exposure.maxExposureEv);
    SetPostFxExposureBrightenSpeed(tuning.exposure.brightenSpeed);
    SetPostFxExposureDarkenSpeed(tuning.exposure.darkenSpeed);
    SetPostFxExposureShoulderStrength(tuning.exposure.shoulderStrength);
    SetPostFxExposureWhitePoint(tuning.exposure.whitePoint);

    SetPostFxColorGradeMode(tuning.colorGradeMode);
    SetPostFxDisplayGammaMode(tuning.displayGammaMode);

    // Do not let a previous preset's temporal exposure history leak into a
    // comparison capture. SettleFrames then controls how long the new preset
    // is allowed to adapt from a clean meter state before it is captured.
    RequestPostFxExposureAdaptationReset();
}

void OverlayPresetConfig(
    const wchar_t* path,
    const wchar_t* section,
    ScreenshotPreset& preset)
{
    TryReadUInt(path, section, L"InternalWidth", preset.config.internalWidth);
    TryReadUInt(path, section, L"InternalHeight", preset.config.internalHeight);
    TryReadFloat(path, section, L"InternalScale", preset.config.internalScale);
    TryReadUInt(path, section, L"ShadowScale", preset.config.shadowScale);
    TryReadUInt(path, section, L"ReflectionScale", preset.config.reflectionScale);
    TryReadBool(path, section, L"ImproveDoFResolution", preset.config.improveDofResolution);
    TryReadUInt(path, section, L"AdditionalDoFBlur", preset.config.additionalDofBlur);
    TryReadBool(path, section, L"FixPixelOffset", preset.config.fixPixelOffset);
    TryReadUInt(path, section, L"HighDetailDistanceScale", preset.config.highDetailDistanceScale);
    TryReadUInt(path, section, L"MainFrustumDistanceMode", preset.config.mainFrustumDistanceMode);
    TryReadUInt(path, section, L"ObjectActivationDistanceScale", preset.config.objectActivationDistanceScale);
    TryReadUInt(path, section, L"ObjectLODDistanceScale", preset.config.objectLodDistanceScale);
    TryReadBool(path, section, L"FixInteriorOcclusionBugs", preset.config.fixInteriorOcclusionBugs);
    TryReadUInt(path, section, L"MaxAnisotropy", preset.config.maxAnisotropy);

    wchar_t value[64] = {};
    if (TryReadString(path, section, L"FilteringMode", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"Original") == 0)
            preset.config.textureFilteringMode = TextureFilteringMode::Original;
        else if (_wcsicmp(value, L"Bilinear") == 0)
            preset.config.textureFilteringMode = TextureFilteringMode::Bilinear;
        else if (_wcsicmp(value, L"Anisotropic") == 0)
            preset.config.textureFilteringMode = TextureFilteringMode::Anisotropic;
    }

    if (TryReadString(path, section, L"AOMode", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"Off") == 0)
            preset.tuning.ao.mode = PostFxAoMode::Off;
        else if (_wcsicmp(value, L"ShowRaw") == 0)
            preset.tuning.ao.mode = PostFxAoMode::ShowRaw;
        else if (_wcsicmp(value, L"ShowFiltered") == 0)
            preset.tuning.ao.mode = PostFxAoMode::ShowFiltered;
        else if (_wcsicmp(value, L"ShowEnhanced") == 0)
            preset.tuning.ao.mode = PostFxAoMode::ShowEnhanced;
        else if (_wcsicmp(value, L"CompositeHDR") == 0 || _wcsicmp(value, L"Composite") == 0)
            preset.tuning.ao.mode = PostFxAoMode::Composite;
    }

    if (TryReadString(path, section, L"BloomMode", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"Legacy") == 0)
            preset.tuning.bloom.mode = PostFxBloomMode::Legacy;
        else if (_wcsicmp(value, L"Bloom") == 0 || _wcsicmp(value, L"BloomNG") == 0)
            preset.tuning.bloom.mode = PostFxBloomMode::Bloom;
        else if (_wcsicmp(value, L"ShowBloom") == 0)
            preset.tuning.bloom.mode = PostFxBloomMode::ShowBloom;
    }

    if (TryReadString(path, section, L"DoFMode", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"Legacy") == 0)
            preset.tuning.dof.mode = PostFxDofMode::Legacy;
        else if (_wcsicmp(value, L"DepthOfField") == 0 || _wcsicmp(value, L"DoFNG") == 0)
            preset.tuning.dof.mode = PostFxDofMode::DepthOfField;
        else if (_wcsicmp(value, L"ShowCoC") == 0)
            preset.tuning.dof.mode = PostFxDofMode::ShowCoC;
        else if (_wcsicmp(value, L"ShowNear") == 0)
            preset.tuning.dof.mode = PostFxDofMode::ShowNear;
        else if (_wcsicmp(value, L"ShowFar") == 0)
            preset.tuning.dof.mode = PostFxDofMode::ShowFar;
    }

    if (TryReadString(path, section, L"ExposureMode", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"Legacy") == 0)
            preset.tuning.exposure.mode = PostFxExposureMode::Legacy;
        else if (_wcsicmp(value, L"AutoExposure") == 0 || _wcsicmp(value, L"ExposureOnly") == 0)
            preset.tuning.exposure.mode = PostFxExposureMode::ExposureOnly;
        else if (_wcsicmp(value, L"Shoulder") == 0 || _wcsicmp(value, L"ShoulderOnly") == 0)
            preset.tuning.exposure.mode = PostFxExposureMode::ShoulderOnly;
        else if (_wcsicmp(value, L"AutoExposureShoulder") == 0 || _wcsicmp(value, L"ExposureAndShoulder") == 0)
            preset.tuning.exposure.mode = PostFxExposureMode::ExposureAndShoulder;
    }

    if (TryReadString(path, section, L"ColorMode", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"PC") == 0)
            preset.tuning.colorGradeMode = PostFxColorGradeMode::PcDirectorsCut;
        else if (_wcsicmp(value, L"Xbox360Grading") == 0)
            preset.tuning.colorGradeMode = PostFxColorGradeMode::Xbox360Grading;
        else if (_wcsicmp(value, L"Xbox360Full") == 0)
            preset.tuning.colorGradeMode = PostFxColorGradeMode::Xbox360Full;
    }

    if (TryReadString(path, section, L"DisplayGamma", value, static_cast<DWORD>(std::size(value))))
    {
        if (_wcsicmp(value, L"PC") == 0)
            preset.tuning.displayGammaMode = PostFxDisplayGammaMode::PcSrgb;
        else if (_wcsicmp(value, L"Xbox360HDTV") == 0)
            preset.tuning.displayGammaMode = PostFxDisplayGammaMode::Xbox360HdtvBt709;
    }
}

bool LoadPreset(
    const wchar_t* path,
    const wchar_t* name,
    ScreenshotPreset& preset)
{
    if (name == nullptr || *name == L'\0')
        return false;

    wcscpy_s(preset.name, name);

    wchar_t section[96] = {};
    swprintf_s(section, L"ScreenshotPreset.%ls", name);

    wchar_t base[32] = L"Configured";
    GetPrivateProfileStringW(
        section, L"Base", L"Configured", base,
        static_cast<DWORD>(std::size(base)), path);

    if (_wcsicmp(base, L"Original") == 0 ||
        _wcsicmp(base, L"Vanilla") == 0)
    {
        preset.config = MakeOriginalConfig(g_configuredBase);
        preset.tuning = MakeOriginalTuning(g_configuredTuning);
    }
    else
    {
        preset.config = g_configuredBase;
        preset.tuning = g_configuredTuning;
    }

    OverlayPresetConfig(path, section, preset);
    return true;
}

bool LoadScreenshotConfig(const wchar_t* path)
{
    ScreenshotConfig config{};
    TryReadBool(path, L"Screenshots", L"Enabled", config.enabled);
    TryReadUInt(path, L"Screenshots", L"SettleFrames", config.settleFrames);
    config.settleFrames = std::clamp<UINT>(config.settleFrames, 1, 240);
    TryReadBool(path, L"Screenshots", L"AutoCaptureOnSwitch", config.autoCaptureOnSwitch);
    TryReadBool(path, L"Screenshots", L"PauseDuringCaptureAll", config.pauseDuringCaptureAll);

    wchar_t keyText[32] = {};
    if (TryReadString(path, L"Screenshots", L"CyclePresetKey", keyText, static_cast<DWORD>(std::size(keyText))))
        config.cycleKey = ParseVirtualKey(keyText, config.cycleKey);
    if (TryReadString(path, L"Screenshots", L"CaptureKey", keyText, static_cast<DWORD>(std::size(keyText))))
        config.captureKey = ParseVirtualKey(keyText, config.captureKey);
    if (TryReadString(path, L"Screenshots", L"CaptureAllKey", keyText, static_cast<DWORD>(std::size(keyText))))
        config.captureAllKey = ParseVirtualKey(keyText, config.captureAllKey);

    wchar_t directory[MAX_PATH] = {};
    if (TryReadString(path, L"Screenshots", L"Directory", directory, MAX_PATH))
        wcscpy_s(config.directory, directory);

    UINT requestedCount = 2;
    TryReadUInt(path, L"Screenshots", L"PresetCount", requestedCount);
    requestedCount = std::clamp<UINT>(requestedCount, 1, kMaxScreenshotPresets);

    std::array<ScreenshotPreset, kMaxScreenshotPresets> loaded{};
    UINT loadedCount = 0;
    for (UINT i = 0; i < requestedCount; ++i)
    {
        wchar_t key[24] = {};
        swprintf_s(key, L"Preset%u", i + 1);

        wchar_t defaultName[48] = {};
        if (i == 0)
            wcscpy_s(defaultName, L"Original");
        else if (i == 1)
            wcscpy_s(defaultName, L"ZachFix");
        else
            swprintf_s(defaultName, L"Preset%u", i + 1);

        wchar_t presetName[48] = {};
        GetPrivateProfileStringW(
            L"Screenshots", key, defaultName, presetName,
            static_cast<DWORD>(std::size(presetName)), path);
        const std::wstring trimmedName = Trim(presetName);
        if (trimmedName.empty())
            continue;

        if (LoadPreset(path, trimmedName.c_str(), loaded[loadedCount]))
            ++loadedCount;
    }

    if (loadedCount == 0)
    {
        AppendLog("[Screenshots] WARNING: no valid screenshot presets were configured; feature disabled.\n");
        return false;
    }

    if (config.cycleKey == config.captureKey ||
        config.cycleKey == config.captureAllKey ||
        config.captureKey == config.captureAllKey)
    {
        AppendLog("[Screenshots] WARNING: screenshot hotkeys must be distinct; feature disabled.\n");
        return false;
    }

    g_screenshotConfig = config;
    g_presets = loaded;
    g_presetCount = loadedCount;
    return true;
}

bool IsKeyPressedEdge(UINT key, bool& latched)
{
    const bool down = (GetAsyncKeyState(static_cast<int>(key)) & 0x8000) != 0;
    if (!down)
    {
        latched = false;
        return false;
    }

    if (latched)
        return false;

    latched = true;
    return true;
}

bool ApplyPresetState(
    IDirect3DDevice9* device,
    const ZachFixConfig& config,
    const PostFxTuningSnapshot& tuning,
    const char* context)
{
    char status[256] = {};
    if (!ApplyRuntimeRenderSettings(device, config, status, sizeof(status)))
    {
        char text[512] = {};
        sprintf_s(
            text,
            "[Screenshots] ERROR: %s render preset apply failed: %s\n",
            context != nullptr ? context : "comparison",
            status[0] != '\0' ? status : "unknown error");
        AppendLog(text);
        return false;
    }

    ApplyPostFxSnapshot(tuning);
    return true;
}

bool ApplyPreset(IDirect3DDevice9* device, UINT index)
{
    if (index >= g_presetCount)
        return false;

    char context[96] = {};
    sprintf_s(context, "preset %u/%u", index + 1, g_presetCount);
    if (!ApplyPresetState(device, g_presets[index].config, g_presets[index].tuning, context))
        return false;

    g_activePreset = static_cast<int>(index);

    char text[256] = {};
    sprintf_s(
        text,
        "[Screenshots] Preset %u/%u active: %ls.\n",
        index + 1,
        g_presetCount,
        g_presets[index].name);
    AppendLog(text);
    return true;
}

void SanitizeFileComponent(const wchar_t* source, wchar_t* destination, size_t count)
{
    if (destination == nullptr || count == 0)
        return;

    size_t out = 0;
    for (const wchar_t* cursor = source; cursor != nullptr && *cursor != L'\0'; ++cursor)
    {
        wchar_t ch = *cursor;
        const bool valid =
            (ch >= L'0' && ch <= L'9') ||
            (ch >= L'A' && ch <= L'Z') ||
            (ch >= L'a' && ch <= L'z') ||
            ch == L'-' || ch == L'_';
        if (!valid)
            ch = L'_';

        if (out + 1 >= count)
            break;
        destination[out++] = ch;
    }
    destination[out] = L'\0';
}

bool EnsureDirectoryTree(const wchar_t* path)
{
    if (path == nullptr || *path == L'\0')
        return false;

    wchar_t mutablePath[MAX_PATH] = {};
    wcscpy_s(mutablePath, path);

    const size_t length = wcslen(mutablePath);
    for (size_t i = 0; i < length; ++i)
    {
        if (mutablePath[i] != L'\\' && mutablePath[i] != L'/')
            continue;

        if (i == 2 && mutablePath[1] == L':')
            continue;

        const wchar_t saved = mutablePath[i];
        mutablePath[i] = L'\0';
        if (mutablePath[0] != L'\0')
        {
            if (!CreateDirectoryW(mutablePath, nullptr) &&
                GetLastError() != ERROR_ALREADY_EXISTS)
            {
                mutablePath[i] = saved;
                return false;
            }
        }
        mutablePath[i] = saved;
    }

    return CreateDirectoryW(mutablePath, nullptr) ||
           GetLastError() == ERROR_ALREADY_EXISTS;
}

bool BuildScreenshotDirectory(wchar_t* output, size_t count)
{
    wchar_t iniPath[MAX_PATH] = {};
    if (!GetConfigFilePath(iniPath, MAX_PATH))
        return false;

    wchar_t* slash = wcsrchr(iniPath, L'\\');
    if (slash != nullptr)
        *slash = L'\0';
    else
        wcscpy_s(iniPath, L".");

    const wchar_t* configured = g_screenshotConfig.directory;
    const bool absolute =
        (wcslen(configured) >= 2 && configured[1] == L':') ||
        (configured[0] == L'\\' && configured[1] == L'\\');

    if (absolute)
        return wcscpy_s(output, count, configured) == 0;

    return swprintf_s(output, count, L"%ls\\%ls", iniPath, configured) > 0;
}

bool ResolveD3DXSaveSurface()
{
    if (g_saveSurfaceToFileW != nullptr)
        return true;

    HMODULE d3dx = GetModuleHandleW(L"d3dx9_43.dll");
    if (d3dx == nullptr)
        d3dx = LoadLibraryW(L"d3dx9_43.dll");
    if (d3dx == nullptr)
    {
        AppendLog("[Screenshots] ERROR: d3dx9_43.dll is unavailable; PNG capture disabled.\n");
        return false;
    }

    g_saveSurfaceToFileW = reinterpret_cast<D3DXSaveSurfaceToFileWFn>(
        GetProcAddress(d3dx, "D3DXSaveSurfaceToFileW"));
    if (g_saveSurfaceToFileW == nullptr)
    {
        AppendLog("[Screenshots] ERROR: D3DXSaveSurfaceToFileW export is unavailable.\n");
        return false;
    }

    return true;
}

bool CaptureBackBuffer(
    IDirect3DDevice9* device,
    int presetIndex,
    const SYSTEMTIME* batchTimestamp,
    UINT batchOrdinal,
    UINT batchCount)
{
    if (device == nullptr || !ResolveD3DXSaveSurface())
        return false;

    wchar_t directory[MAX_PATH] = {};
    if (!BuildScreenshotDirectory(directory, std::size(directory)) ||
        !EnsureDirectoryTree(directory))
    {
        AppendLog("[Screenshots] ERROR: could not create screenshot output directory.\n");
        return false;
    }

    SYSTEMTIME now{};
    if (batchTimestamp != nullptr)
        now = *batchTimestamp;
    else
        GetLocalTime(&now);

    wchar_t label[64] = L"Current";
    if (presetIndex >= 0 && static_cast<UINT>(presetIndex) < g_presetCount)
        SanitizeFileComponent(g_presets[presetIndex].name, label, std::size(label));

    wchar_t path[MAX_PATH] = {};
    if (batchTimestamp != nullptr)
    {
        swprintf_s(
            path,
            L"%ls\\%04u%02u%02u-%02u%02u%02u_%02u-of-%02u_%ls.png",
            directory,
            now.wYear, now.wMonth, now.wDay,
            now.wHour, now.wMinute, now.wSecond,
            batchOrdinal, batchCount, label);
    }
    else
    {
        const unsigned long long serial = ++g_manualCaptureSerial;
        swprintf_s(
            path,
            L"%ls\\%04u%02u%02u-%02u%02u%02u-%03u_%ls_%03llu.png",
            directory,
            now.wYear, now.wMonth, now.wDay,
            now.wHour, now.wMinute, now.wSecond,
            now.wMilliseconds,
            label,
            serial);
    }

    IDirect3DSurface9* backBuffer = nullptr;
    const HRESULT backBufferResult = device->GetBackBuffer(
        0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
    if (FAILED(backBufferResult) || backBuffer == nullptr)
    {
        char text[224] = {};
        sprintf_s(
            text,
            "[Screenshots] ERROR: GetBackBuffer failed (HRESULT=0x%08X).\n",
            static_cast<unsigned int>(backBufferResult));
        AppendLog(text);
        return false;
    }

    const HRESULT saveResult = g_saveSurfaceToFileW(
        path,
        kD3DXImageFileFormatPng,
        backBuffer,
        nullptr,
        nullptr);
    backBuffer->Release();

    if (FAILED(saveResult))
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[Screenshots] ERROR: PNG save failed (HRESULT=0x%08X).\n",
            static_cast<unsigned int>(saveResult));
        AppendLog(text);
        return false;
    }

    char utf8Path[768] = {};
    WideCharToMultiByte(
        CP_UTF8, 0, path, -1, utf8Path, static_cast<int>(sizeof(utf8Path)), nullptr, nullptr);
    char text[896] = {};
    sprintf_s(text, "[Screenshots] Saved %s\n", utf8Path);
    AppendLog(text);
    return true;
}

void ReleaseBatchPause()
{
    if (!g_batchOwnsPause)
        return;

    // If the F10 panel was opened during the short comparison sequence and it
    // owns a gameplay pause policy, preserve that policy instead of blindly
    // unfreezing the game underneath it.
    const bool uiKeepsPause = IsSettingsUiOpen() && g_config.pauseGameWhileUiOpen;
    SetGameplayPauseActive(uiKeepsPause);
    g_batchOwnsPause = false;
}

void AbortBatch(IDirect3DDevice9* device, const char* reason)
{
    if (!g_batchActive)
        return;

    ApplyPresetState(
        device,
        g_batchRestoreConfig,
        g_batchRestoreTuning,
        "capture-all restore");
    ReleaseBatchPause();
    g_batchActive = false;
    g_batchCountdown = 0;
    g_activePreset = g_batchRestoreActivePreset;

    char text[320] = {};
    sprintf_s(text, "[Screenshots] Capture-all aborted: %s\n", reason);
    AppendLog(text);
}

void StartBatch(IDirect3DDevice9* device)
{
    if (g_batchActive || g_presetCount == 0)
        return;

    if (IsSettingsUiOpen())
    {
        AppendLog("[Screenshots] Capture-all ignored while F10 settings are open. Close the panel for clean frames.\n");
        return;
    }

    g_batchRestoreConfig = g_config;
    g_batchRestoreTuning = GetPostFxTuningSnapshot();
    g_batchRestoreActivePreset = g_activePreset;
    GetLocalTime(&g_batchTimestamp);
    g_batchPresetIndex = 0;

    if (g_screenshotConfig.pauseDuringCaptureAll)
    {
        SetGameplayPauseActive(true);
        g_batchOwnsPause = true;
    }

    if (!ApplyPreset(device, 0))
    {
        ReleaseBatchPause();
        return;
    }

    g_batchActive = true;
    g_batchCountdown = g_screenshotConfig.settleFrames;

    char text[256] = {};
    sprintf_s(
        text,
        "[Screenshots] Capture-all started: %u preset(s), settle=%u frame(s), timerPause=%s.\n",
        g_presetCount,
        g_screenshotConfig.settleFrames,
        g_batchOwnsPause ? "on" : "off");
    AppendLog(text);
}

void AdvanceBatch(IDirect3DDevice9* device)
{
    if (!g_batchActive)
        return;

    if (IsSettingsUiOpen())
    {
        AbortBatch(device, "F10 settings were opened during capture-all");
        return;
    }

    if (g_batchCountdown > 0)
    {
        --g_batchCountdown;
        if (g_batchCountdown > 0)
            return;
    }

    CaptureBackBuffer(
        device,
        static_cast<int>(g_batchPresetIndex),
        &g_batchTimestamp,
        g_batchPresetIndex + 1,
        g_presetCount);

    ++g_batchPresetIndex;
    if (g_batchPresetIndex < g_presetCount)
    {
        if (!ApplyPreset(device, g_batchPresetIndex))
        {
            AbortBatch(device, "next preset could not be applied");
            return;
        }
        g_batchCountdown = g_screenshotConfig.settleFrames;
        return;
    }

    if (!ApplyPresetState(
            device,
            g_batchRestoreConfig,
            g_batchRestoreTuning,
            "capture-all restore"))
    {
        AppendLog("[Screenshots] WARNING: capture-all completed, but restoring the pre-batch render state failed.\n");
    }

    ReleaseBatchPause();
    g_batchActive = false;
    g_batchCountdown = 0;
    g_activePreset = g_batchRestoreActivePreset;
    AppendLog("[Screenshots] Capture-all complete; pre-batch ZachFix state restored.\n");
}

void ProcessSingleCapture(IDirect3DDevice9* device)
{
    if (g_singleCaptureCountdown == 0)
        return;

    --g_singleCaptureCountdown;
    if (g_singleCaptureCountdown == 0)
    {
        if (IsSettingsUiOpen())
        {
            // Keep waiting rather than save a comparison image with F10 in it.
            g_singleCaptureCountdown = 1;
            return;
        }
        CaptureBackBuffer(device, g_singleCapturePreset, nullptr, 0, 0);
    }
}
} // namespace

void InitializeScreenshotPresetSystem()
{
    if (g_initialized.exchange(true, std::memory_order_acq_rel))
        return;

    g_configuredBase = g_config;
    g_configuredTuning = GetPostFxTuningSnapshot();

    wchar_t path[MAX_PATH] = {};
    if (!GetConfigFilePath(path, MAX_PATH))
    {
        AppendLog("[Screenshots] WARNING: ZachFix.ini path unavailable; comparison capture disabled.\n");
        return;
    }

    if (!LoadScreenshotConfig(path))
        return;

    if (!g_screenshotConfig.enabled)
    {
        AppendLog("[Screenshots] Comparison capture disabled in ZachFix.ini.\n");
        return;
    }

    char cycleKey[16] = {};
    char captureKey[16] = {};
    char captureAllKey[16] = {};
    FormatKeyName(g_screenshotConfig.cycleKey, cycleKey, sizeof(cycleKey));
    FormatKeyName(g_screenshotConfig.captureKey, captureKey, sizeof(captureKey));
    FormatKeyName(g_screenshotConfig.captureAllKey, captureAllKey, sizeof(captureAllKey));

    char text[512] = {};
    sprintf_s(
        text,
        "[Screenshots] Ready: presets=%u, cycle=%s, capture=%s, capture-all=%s, settle=%u, autoCapture=%s.\n",
        g_presetCount,
        cycleKey,
        captureKey,
        captureAllKey,
        g_screenshotConfig.settleFrames,
        g_screenshotConfig.autoCaptureOnSwitch ? "on" : "off");
    AppendLog(text);
}

void ProcessScreenshotPresetFrame(IDirect3DDevice9* device)
{
    if (!g_initialized.load(std::memory_order_acquire) ||
        !g_screenshotConfig.enabled ||
        device == nullptr)
    {
        return;
    }

    // Capture-all owns preset transitions until it restores the exact pre-batch
    // state. Ignore manual hotkeys during that short sequence.
    if (g_batchActive)
    {
        AdvanceBatch(device);
        return;
    }

    ProcessSingleCapture(device);

    const bool captureAllPressed = IsKeyPressedEdge(
        g_screenshotConfig.captureAllKey, g_captureAllKeyLatched);
    const bool cyclePressed = IsKeyPressedEdge(
        g_screenshotConfig.cycleKey, g_cycleKeyLatched);
    const bool capturePressed = IsKeyPressedEdge(
        g_screenshotConfig.captureKey, g_captureKeyLatched);

    if (captureAllPressed)
    {
        g_singleCaptureCountdown = 0;
        StartBatch(device);
        return;
    }

    if (capturePressed)
    {
        if (IsSettingsUiOpen())
        {
            AppendLog("[Screenshots] Capture ignored while F10 settings are open. Close the panel for a clean frame.\n");
        }
        else
        {
            CaptureBackBuffer(device, g_activePreset, nullptr, 0, 0);
        }
    }

    if (cyclePressed)
    {
        const UINT next = g_activePreset < 0
            ? 0u
            : (static_cast<UINT>(g_activePreset) + 1u) % g_presetCount;
        if (ApplyPreset(device, next) && g_screenshotConfig.autoCaptureOnSwitch)
        {
            g_singleCapturePreset = static_cast<int>(next);
            g_singleCaptureCountdown = g_screenshotConfig.settleFrames;
        }
    }
}
