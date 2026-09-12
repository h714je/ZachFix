#pragma once

#include <Windows.h>

// Shared render/config safety caps. These are used both by config validation
// and by render-target scaling helpers in the main D3D9 translation unit.
inline constexpr UINT kMaxResolutionWidth = 16384;
inline constexpr UINT kMaxResolutionHeight = 16384;

struct DPFixNGConfig
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
    bool fixPixelOffset = true;

    // Experimental engine-aware SSAO. ResolutionScale is a downsample divisor:
    // 1 = full internal resolution, 2 = half, 4 = quarter.
    bool ssaoEnabled = false;
    float ssaoStrength = 1.0f;
    float ssaoRadius = 1.0f;
    UINT ssaoResolutionScale = 2;
    // SSAO debug views:
    // 0 Combined, 1 AO, 2 DP fixed-point decoded depth, 3 magenta, 4 raw RGB,
    // 5 AO contrast, 6/7/8/9 raw R/G/B/A, 10 standard packed RGB 0..1,
    // 11 inverted packed RGB, 12 perspective-linearized packed RGB,
    // 13 legacy DSFix inverted-depth decode.
    UINT ssaoDebugView = 0;

    // 1 = original inner 2x2 full-detail cells.
    // 2 = promote the existing outer 4x4 ring to full detail.
    UINT highDetailDistanceScale = 1;

    bool uiEnabled = true;
    UINT uiToggleKey = VK_F10;

    bool profilerEnabled = true;
    UINT profilerCaptureKey = VK_F11;
    bool profilerDumpShaders = false;
    bool profilerGpuTimings = true;
    bool profilerCallerTracing = true;
    bool profilerSceneObjectTracing = true;
    bool profilerContinuousTrace = true;
    UINT profilerContinuousMaxFrames = 600;
    UINT profilerMaxEvents = 20000;
};

extern DPFixNGConfig g_config;

extern UINT g_displayWidth;
extern UINT g_displayHeight;
extern UINT g_internalWidth;
extern UINT g_internalHeight;

void LoadConfig();
bool ResolveConfigForWindow(HWND window);
bool GetConfigFilePath(wchar_t* path, size_t pathCount);
bool SaveEditableConfig(const DPFixNGConfig& config);
