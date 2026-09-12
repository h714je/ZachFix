#pragma once

#include <cstddef>
#include <cstdint>
#include <d3d9.h>

#include "config.h"

struct RuntimeResourceStats
{
    UINT managedLogicalResources = 0;
    UINT activeReplacementResources = 0;
    UINT activeTextureRefs = 0;
    UINT activeSurfaceRefs = 0;
    UINT lastChangedResources = 0;
    UINT generation = 0;
    unsigned long long replacementCreates = 0;
    unsigned long long replacementReleases = 0;
    unsigned long long applySuccesses = 0;
    unsigned long long applyFailures = 0;
    unsigned long long estimatedActiveBytes = 0;
};

// Registers the render resources whose effective dimensions are controlled by
// ZachFix. The game keeps its original COM handles; hot-apply may transparently
// bind replacement backing resources for those handles.
void TrackRuntimeTextureResource(
    IDirect3DTexture9* texture,
    UINT requestedWidth,
    UINT requestedHeight,
    UINT effectiveWidth,
    UINT effectiveHeight,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool);

void TrackRuntimeSurfaceResource(
    IDirect3DSurface9* surface,
    UINT requestedWidth,
    UINT requestedHeight,
    UINT effectiveWidth,
    UINT effectiveHeight,
    D3DFORMAT format,
    DWORD usage,
    D3DPOOL pool,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL lockableOrDiscard);

// Acquire functions return an AddRef'd replacement when one is active.
// The caller must Release() it after the D3D9 call finishes.
IDirect3DTexture9* AcquireRuntimeReplacementTexture(IDirect3DBaseTexture9* original);
IDirect3DSurface9* AcquireRuntimeReplacementSurface(IDirect3DSurface9* original);

// Normalizes a replacement pointer returned by the device back to the logical
// handle owned by the game. No reference is transferred.
IDirect3DBaseTexture9* ResolveRuntimeLogicalTexture(IDirect3DBaseTexture9* texture);
IDirect3DSurface9* ResolveRuntimeLogicalSurface(IDirect3DSurface9* surface);

// Creates only the resources whose target dimensions actually changed, then
// atomically switches those bindings if every changed resource was created.
bool ApplyRuntimeRenderSettings(
    IDirect3DDevice9* device,
    const ZachFixConfig& requested,
    char* status,
    size_t statusCount);

RuntimeResourceStats GetRuntimeResourceStats();
