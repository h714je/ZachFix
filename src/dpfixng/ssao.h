#pragma once

#include <cstdint>
#include <d3d9.h>

#include "config.h"

struct SsaoRuntimeStats
{
    bool enabled = false;
    bool depthPairSeen = false;
    bool resourcesReady = false;
    UINT frameWidth = 0;
    UINT frameHeight = 0;
    UINT aoWidth = 0;
    UINT aoHeight = 0;
    unsigned long long estimatedBytes = 0;
    unsigned long long framesApplied = 0;
    unsigned long long failures = 0;

    bool probeValid = false;
    unsigned probeSamples = 0;
    float depthMin = 0.0f;
    float depthP05 = 0.0f;
    float depthMedian = 0.0f;
    float depthP95 = 0.0f;
    float depthMax = 0.0f;
    float depthBelow0075Percent = 0.0f;
    float depthAbove1Percent = 0.0f;
    float estimatedTapRadiusMedianPx = 0.0f;
    float aoMin = 1.0f;
    float aoMean = 1.0f;
    float aoMax = 1.0f;

    float channelRMin = 0.0f, channelRP05 = 0.0f, channelRMedian = 0.0f, channelRP95 = 0.0f, channelRMax = 0.0f;
    float channelGMin = 0.0f, channelGP05 = 0.0f, channelGMedian = 0.0f, channelGP95 = 0.0f, channelGMax = 0.0f;
    float channelBMin = 0.0f, channelBP05 = 0.0f, channelBMedian = 0.0f, channelBP95 = 0.0f, channelBMax = 0.0f;
    float channelAMin = 0.0f, channelAP05 = 0.0f, channelAMedian = 0.0f, channelAP95 = 0.0f, channelAMax = 0.0f;
    float packedUnitMin = 0.0f, packedUnitP05 = 0.0f, packedUnitMedian = 0.0f, packedUnitP95 = 0.0f, packedUnitMax = 0.0f;
};

// True only while DPFix-NG itself is issuing helper passes. D3D9 hooks use
// this to bypass normal game-resource rewriting/profiling for our own work.
bool IsSsaoInternalPass();
bool IsSsaoEnabled();

// Deadly Premonition specific pipeline tracking, modelled after the original
// DPFix render-state integration:
//  * RT1 binding identifies the normal buffer and the currently bound RT0 is
//    the RGB-packed depth buffer.
//  * a full-size render-target texture bound to sampler stage 8 identifies the
//    main HDR scene surface currently bound as RT0.
//  * after the game has switched to the backbuffer, the next game RT switch is
//    the safe point to apply SSAO to the saved main scene before DoF/exposure.
void SsaoBeforeGameSetRenderTarget(
    IDirect3DDevice9* device,
    DWORD index,
    IDirect3DSurface9* logicalTarget,
    uintptr_t returnAddress);

void SsaoAfterGameSetRenderTarget(
    IDirect3DDevice9* device,
    DWORD index,
    IDirect3DSurface9* logicalTarget,
    uintptr_t returnAddress,
    HRESULT setResult);

void SsaoObserveGameTextureBind(
    IDirect3DDevice9* device,
    DWORD stage,
    IDirect3DBaseTexture9* logicalTexture,
    uintptr_t returnAddress);

// Drops frame-local G-buffer/main-scene references after Present.
void SsaoOnPresent();

// Commits editable SSAO fields after the main hot-apply transaction succeeds.
void ApplySsaoSettings(const DPFixNGConfig& requested);
void RequestSsaoProbe();

void ReleaseSsaoResources();
SsaoRuntimeStats GetSsaoRuntimeStats();
