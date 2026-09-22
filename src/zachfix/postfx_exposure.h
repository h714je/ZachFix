#pragma once

#include <Windows.h>
#include <d3d9.h>

#include "postfx_tuning.h"

// Exposure / highlight-rolloff replacement for Deadly
// Premonition's identified final-composite draw.
//
// The replacement keeps DP's original DoF and grading, but replaces the legacy
// adapted-luminance denominator with a ZachFix log-luminance meter and a
// temporal EV adaptation pass. DP's g_fExposure (c10.x) remains the authored
// exposure key so scene-specific intent is preserved. GTAO Composite can be
// folded in before grading/exposure, and ZachFix bloom can replace DP's legacy
// bright-pass texture while sharing the same final-composite replacement.

struct PostFxAoFinalCompositeState;

struct PostFxDisplayGammaStats
{
    bool shaderReady = false;
    bool appliedLastPresent = false;
    unsigned long long applyCount = 0;
};

struct PostFxExposureStats
{
    bool shaderReady = false;
    bool meterShadersReady = false;
    bool adaptationInitialized = false;
    bool telemetryAvailable = false;
    bool telemetryReadbackFailed = false;
    UINT meterWidth = 0;
    UINT meterHeight = 0;
    float frameDeltaMs = 0.0f;
    float gameExposureKey = 0.0f;
    float averageLogLuminance = 0.0f;
    float targetEv = 0.0f;
    float adaptedEv = 0.0f;
    float exposureGain = 1.0f;
    float nativeFinalExposureKey = 0.0f;
    float xboxRestoredExposureKey = 0.0f;
    bool xboxSpecialExposureUndo = false;
    unsigned long long lastAppliedFrame = 0;
};

struct PostFxExposureDrawState
{
    bool active = false;
    bool changedStage0 = false;
    bool changedStage1 = false;
    bool changedStage2 = false;
    bool changedStage3 = false;
    bool changedStage4 = false;
    bool changedStage5 = false;
    bool changedStage6 = false;
    bool changedStage7 = false;
    bool changedStage8 = false;
    bool changedStage9 = false;
    bool changedSampler6 = false;
    bool changedSampler7 = false;
    bool changedSampler8 = false;
    bool changedSampler9 = false;
    bool changedFrozenSceneConstants = false;
    bool changedConstants26 = false;
    bool changedConstants27 = false;
    bool changedConstants28 = false;
    bool changedConstants29 = false;
    bool changedConstants30 = false;
    bool changedConstants32 = false;
    IDirect3DBaseTexture9* previousStage0 = nullptr;
    IDirect3DBaseTexture9* previousStage1 = nullptr;
    IDirect3DBaseTexture9* previousStage2 = nullptr;
    IDirect3DBaseTexture9* previousStage3 = nullptr;
    IDirect3DBaseTexture9* previousStage4 = nullptr;
    IDirect3DBaseTexture9* previousStage5 = nullptr;
    IDirect3DBaseTexture9* previousStage6 = nullptr;
    IDirect3DBaseTexture9* previousStage7 = nullptr;
    IDirect3DBaseTexture9* previousStage8 = nullptr;
    IDirect3DBaseTexture9* previousStage9 = nullptr;
    IDirect3DTexture9* adaptedExposureTexture = nullptr;
    IDirect3DTexture9* preparedBloomTexture = nullptr;
    IDirect3DTexture9* preparedDofNearTexture = nullptr;
    IDirect3DTexture9* preparedDofFarTexture = nullptr;
    DWORD previousSampler6AddressU = D3DTADDRESS_WRAP;
    DWORD previousSampler6AddressV = D3DTADDRESS_WRAP;
    DWORD previousSampler6MinFilter = D3DTEXF_POINT;
    DWORD previousSampler6MagFilter = D3DTEXF_POINT;
    DWORD previousSampler6MipFilter = D3DTEXF_NONE;
    DWORD previousSampler6Srgb = FALSE;
    DWORD previousSampler7AddressU = D3DTADDRESS_WRAP;
    DWORD previousSampler7AddressV = D3DTADDRESS_WRAP;
    DWORD previousSampler7MinFilter = D3DTEXF_POINT;
    DWORD previousSampler7MagFilter = D3DTEXF_POINT;
    DWORD previousSampler7MipFilter = D3DTEXF_NONE;
    DWORD previousSampler7Srgb = FALSE;
    DWORD previousSampler8AddressU = D3DTADDRESS_WRAP;
    DWORD previousSampler8AddressV = D3DTADDRESS_WRAP;
    DWORD previousSampler8MinFilter = D3DTEXF_POINT;
    DWORD previousSampler8MagFilter = D3DTEXF_POINT;
    DWORD previousSampler8MipFilter = D3DTEXF_NONE;
    DWORD previousSampler8Srgb = FALSE;
    DWORD previousSampler9AddressU = D3DTADDRESS_WRAP;
    DWORD previousSampler9AddressV = D3DTADDRESS_WRAP;
    DWORD previousSampler9MinFilter = D3DTEXF_POINT;
    DWORD previousSampler9MagFilter = D3DTEXF_POINT;
    DWORD previousSampler9MipFilter = D3DTEXF_NONE;
    DWORD previousSampler9Srgb = FALSE;
    float previousConstants9[4] = {};
    float previousConstants10[4] = {};
    float previousConstants15[4] = {};
    float previousConstants16[4] = {};
    float previousConstants26[4] = {};
    float previousConstants27[4] = {};
    float previousConstants28[4] = {};
    float previousConstants29[4] = {};
    float previousConstants30[4] = {};
    float previousConstants32[4] = {};
};

PostFxDisplayGammaStats GetPostFxDisplayGammaStats();
bool ShouldUsePostFxDisplayGamma();
bool ApplyPostFxDisplayGamma(
    IDirect3DDevice9* device,
    IDirect3DSurface9* backBuffer);
void NotifyPostFxDisplayGammaPresentComplete(bool applied);
void RequestPostFxExposureAdaptationReset();
PostFxExposureStats GetPostFxExposureStats();

bool ShouldUsePostFxExposureReplacement();
IDirect3DPixelShader9* GetPostFxExposureReplacementShader(IDirect3DDevice9* device);
void NotifyPostFxExposureReplacementBound(bool bound);

bool BeginPostFxExposureFinalComposite(
    IDirect3DDevice9* device,
    PostFxAoFinalCompositeState* aoState,
    PostFxExposureDrawState* state);
void EndPostFxExposureFinalComposite(
    IDirect3DDevice9* device,
    PostFxExposureDrawState* state);

void ReleasePostFxExposureResources();
