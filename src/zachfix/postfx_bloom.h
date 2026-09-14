#pragma once

#include <Windows.h>
#include <d3d9.h>

// Research-only modern bloom pyramid for PostFX NG.
//
// Bloom NG is generated from the scene-linear HDR buffer before the final
// composite. When GTAO Composite (HDR) is active, the bloom prefilter sees the
// AO-modulated scene as well. The final composite applies the authored DP
// g_fBloomForce and the user intensity, then exposes Bloom NG together with the
// scene before the filmic shoulder.

enum class PostFxBloomMode : UINT
{
    Legacy = 0,
    BloomNg,
    ShowBloom
};

struct PostFxBloomSettings
{
    PostFxBloomMode mode = PostFxBloomMode::Legacy;
    float thresholdEv = 0.0f;
    float softKnee = 0.50f;
    float intensity = 0.35f;
    float scatter = 0.70f;
    UINT maxLevels = 5;
};

struct PostFxBloomStats
{
    bool shaderReady = false;
    bool activeThisFrame = false;
    bool usedAo = false;
    bool fallbackToLegacy = false;
    UINT sourceWidth = 0;
    UINT sourceHeight = 0;
    UINT baseWidth = 0;
    UINT baseHeight = 0;
    UINT levels = 0;
    unsigned long long preparedFrame = 0;
};

PostFxBloomSettings GetPostFxBloomSettings();
void SetPostFxBloomMode(PostFxBloomMode mode);
void SetPostFxBloomThresholdEv(float ev);
void SetPostFxBloomSoftKnee(float softKnee);
void SetPostFxBloomIntensity(float intensity);
void SetPostFxBloomScatter(float scatter);
void SetPostFxBloomMaxLevels(UINT levels);
void ResetPostFxBloomSettings();
PostFxBloomStats GetPostFxBloomStats();

bool ShouldUsePostFxBloomReplacement();

// Returns an AddRef-owned Bloom0 texture. Release it after binding/restoring the
// final-composite draw. A null AO input is valid and simply blooms the original
// HDR scene.
bool PreparePostFxBloom(
    IDirect3DDevice9* device,
    IDirect3DTexture9* filteredAo,
    IDirect3DTexture9** bloomTexture);

void ReleasePostFxBloomResources();
