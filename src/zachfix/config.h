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
