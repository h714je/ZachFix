#pragma once

#include <Windows.h>
#include <d3d9.h>

#include "postfx_tuning.h"

// GTAO-style horizon ambient occlusion built on the ZachFix PostFX framework.
// All controls are live and take effect on the next final-composite draw.

struct PostFxAoStats
{
    bool shaderReady = false;
    bool projectionReady = false;
    bool activeThisFrame = false;
    bool skippedStaleGBuffer = false;
    bool skippedProjection = false;
    bool nativeDepthAvailable = false;
    bool nativeDepthActive = false;
    UINT width = 0;
    UINT height = 0;
    float projectionScaleX = 0.0f;
    float projectionScaleY = 0.0f;
    float projectionDepthQ = 0.0f;
    float projectionDepthQn = 0.0f;
    unsigned long long projectionFrame = 0;
    unsigned long long preparedFrame = 0;
};

struct PostFxAoFinalCompositeState
{
    bool active = false;
    bool hdrIntegrated = false;
    PostFxAoMode mode = PostFxAoMode::Off;
    IDirect3DTexture9* rawAo = nullptr;
    IDirect3DTexture9* filteredAo = nullptr;
    UINT aoWidth = 0;
    UINT aoHeight = 0;
    DWORD srgbWrite = FALSE;
};

PostFxAoStats GetPostFxAoStats();

// Projection scale captured from DP's geometry VS g_mViewProj c245/c246.
// For a standard perspective matrix, the lengths of the xyz portions of the
// first two matrix columns are the horizontal/vertical projection scales.
void ObservePostFxProjectionParameters(
    float projectionScaleX,
    float projectionScaleY,
    float projectionDepthQ,
    float projectionDepthQn);

// Called around the game's identified final-composite draw. Begin computes raw
// and depth/normal-aware filtered AO. Composite mode is consumed by the PostFX
// final-composite replacement in HDR when available; End keeps the old LDR
// multiply only as a safe fallback. Debug views still render before DP's HUD/UI.
bool BeginPostFxAoFinalComposite(
    IDirect3DDevice9* device,
    PostFxAoFinalCompositeState* state);
void EndPostFxAoFinalComposite(
    IDirect3DDevice9* device,
    PostFxAoFinalCompositeState* state);

void ReleasePostFxAoResources();
