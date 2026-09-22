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
    D3DFORMAT requestedFormat,
    D3DFORMAT effectiveFormat,
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

struct RuntimeTextureBinding
{
    IDirect3DBaseTexture9* logical = nullptr;
    IDirect3DTexture9* replacement = nullptr;
};

struct RuntimeSurfaceBinding
{
    IDirect3DSurface9* logical = nullptr;
    IDirect3DSurface9* replacement = nullptr;
};

// Resolves a game-owned logical resource and acquires its active replacement in
// one registry lookup. replacement is AddRef'd and must be Released by caller.
// When no hot-applied replacements exist these are lock-free identity fast paths.
RuntimeTextureBinding AcquireRuntimeTextureBinding(IDirect3DBaseTexture9* texture);
RuntimeSurfaceBinding AcquireRuntimeSurfaceBinding(IDirect3DSurface9* surface);

// Releases ZachFix-owned replacement resources and forgets all raw logical
// resource identities before IDirect3DDevice9::Reset crosses the device
// generation boundary. Creation hooks repopulate the registry afterwards.
void ResetRuntimeResourcesForDeviceReset();

// Creates only the resources whose target dimensions actually changed, then
// atomically switches those bindings if every changed resource was created.
bool ApplyRuntimeRenderSettings(
    IDirect3DDevice9* device,
    const ZachFixConfig& requested,
    char* status,
    size_t statusCount);

RuntimeResourceStats GetRuntimeResourceStats();
