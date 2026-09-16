#pragma once

#include <Windows.h>

enum class TextureDimensionMode : UINT
{
    DPFix = 0,
    Preserve = 1
};

enum class TextureFilteringMode : UINT
{
    Original = 0,
    Bilinear = 1,
    Anisotropic = 2
};

// Shared render/config safety caps. These are used both by config validation
// and by render-target scaling helpers in the main D3D9 translation unit.
inline constexpr UINT kMaxResolutionWidth = 16384;
inline constexpr UINT kMaxResolutionHeight = 16384;

struct ZachFixConfig
{
    UINT displayWidth = 0;
    UINT displayHeight = 0;
    bool borderless = true;

    UINT internalWidth = 0;
    UINT internalHeight = 0;
    float internalScale = 1.0f;

    UINT shadowScale = 1;
    bool improveShadowPrecision = false;
    UINT reflectionScale = 1;
    bool improveDofResolution = false;
    UINT additionalDofBlur = 0;
    bool fixPixelOffset = true;

    // 1 = original inner 2x2 full-detail cells.
    // 2 = promote the existing outer 4x4 ring to full detail.
    UINT highDetailDistanceScale = 1;

    bool enableTextureOverride = true;
    bool textureDeveloperMode = false;
    bool dumpTextures = false;
    TextureDimensionMode textureDimensionMode = TextureDimensionMode::DPFix;

    TextureFilteringMode textureFilteringMode = TextureFilteringMode::Original;
    UINT maxAnisotropy = 16;

    bool uiEnabled = true;
    UINT uiToggleKey = VK_F10;
    bool pauseGameWhileUiOpen = false;

    // Optional native XInput source. DP still consumes its familiar JOYINFOEX
    // shape, synthesized by ZachFix from XInputGetState.
    bool nativeXInputEnabled = false;

    // Restores DP's surviving native two-channel vibration path through XInput.
    // These are hot-applicable while NativeXInput is active.
    bool vibrationEnabled = true;
    float vibrationStrength = 1.0f;

    // Runtime switch around DP's vanilla USEJOY mode flag.
    bool autoInputModeSwitch = false;

    // Transactional protection for DP's destructive single-file save path.
    // DP writes to a temp file first; the previous live save is backed up only
    // after the temp file passes the conservative validator and before commit.
    bool saveSafetyEnabled = true;
    UINT saveSafetyBackupCount = 10;

    // Dynamically substitutes the glyph atlas selected by the current USEJOY
    // mode. Theme names resolve to files under ZachFix\glyphs\keyboard and
    // ZachFix\glyphs\gamepad; Native uses the atlas captured from DP.
    bool dynamicGlyphAtlas = false;
    bool glyphHotReload = true;
    wchar_t keyboardGlyphSet[64] = L"Native";
    wchar_t gamepadGlyphSet[64] = L"xbox";
};

extern ZachFixConfig g_config;

extern UINT g_displayWidth;
extern UINT g_displayHeight;
extern UINT g_internalWidth;
extern UINT g_internalHeight;

void LoadConfig();
bool ResolveConfigForWindow(HWND window);
bool GetConfigFilePath(wchar_t* path, size_t pathCount);
bool SaveEditableConfig(const ZachFixConfig& config);
bool ReloadPostFxConfigFromIni();
