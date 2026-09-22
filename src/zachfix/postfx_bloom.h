#pragma once

#include <Windows.h>
#include <d3d9.h>

#include "postfx_tuning.h"

// HDR bloom pyramid for the ZachFix PostFX framework.
//
// Bloom is generated from the scene-linear HDR buffer before the final
// composite. When GTAO Composite (HDR) is active, the bloom prefilter sees the
// AO-modulated scene as well. The final composite applies the authored DP
// g_fBloomForce and the user intensity, then exposes Bloom together with the
// scene before the filmic shoulder.

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

PostFxBloomStats GetPostFxBloomStats();


// Returns an AddRef-owned Bloom0 texture. Release it after binding/restoring the
// final-composite draw. A null AO input is valid and simply blooms the original
// HDR scene.
bool PreparePostFxBloom(
    IDirect3DDevice9* device,
    IDirect3DTexture9* filteredAo,
    IDirect3DTexture9** bloomTexture);

void ReleasePostFxBloomResources();
