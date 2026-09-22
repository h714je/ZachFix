#pragma once

#include <Windows.h>
#include <d3d9.h>

#include "postfx_tuning.h"

struct PostFxGBufferView;

// Resolution-independent two-layer depth of field for the ZachFix PostFX framework.
//
// DP's authored c15/c16 focus constants remain the source of near/far CoC.
// ZachFix replaces only the old fixed-size gaussian texture with half/quarter
// resolution near/far gather layers and depth-aware reconstruction.

struct PostFxDofStats
{
    bool shaderReady = false;
    bool activeThisFrame = false;
    bool fallbackToLegacy = false;
    bool freezePending = false;
    bool frameFrozen = false;
    UINT width = 0;
    UINT height = 0;
    UINT sourceWidth = 0;
    UINT sourceHeight = 0;
    unsigned long long preparedFrame = 0;
    unsigned long long frozenFrame = 0;
};

struct PostFxDofFreezeView
{
    // AddRef-owned snapshot. Release with ReleasePostFxDofFreezeView().
    IDirect3DTexture9* diffuse = nullptr;
    IDirect3DTexture9* bloom = nullptr;
    IDirect3DTexture9* luminance = nullptr;
    IDirect3DTexture9* gaussian = nullptr;
    IDirect3DTexture9* depth = nullptr;
    IDirect3DTexture9* normal = nullptr;
    float bloomForce[4] = {};
    float exposure[4] = {};
    float dofPrm[4] = {};
    float focus[4] = {};
    float projectionScaleX = 0.0f;
    float projectionScaleY = 0.0f;
    unsigned long long frameIndex = 0;
};

PostFxDofStats GetPostFxDofStats();
void TogglePostFxDofFreezeFrame();
bool IsPostFxDofFreezeRequestedOrActive();
bool UpdatePostFxDofFreezeCapture(IDirect3DDevice9* device, IDirect3DTexture9* filteredAo);
bool AcquirePostFxDofFreezeView(PostFxDofFreezeView* view);
void ReleasePostFxDofFreezeView(PostFxDofFreezeView* view);

// Global PostFX preview freeze. Internally this reuses the original DoF freeze
// snapshot plumbing, but also captures the full-resolution normal buffer and
// projection scale so GTAO can be recomputed from the same frozen scene.
void TogglePostFxPreviewFreeze();
bool IsPostFxPreviewFreezeRequestedOrActive();
bool AcquirePostFxPreviewGBuffer(
    PostFxGBufferView* view,
    float* projectionScaleX,
    float* projectionScaleY);

bool ShouldUsePostFxDofReplacement();

// Returned near/far textures are AddRef-owned by the caller.
bool PreparePostFxDof(
    IDirect3DDevice9* device,
    IDirect3DTexture9* filteredAo,
    IDirect3DTexture9** nearTexture,
    IDirect3DTexture9** farTexture);

void ReleasePostFxDofResources();
