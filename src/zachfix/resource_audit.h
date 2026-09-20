#pragma once

#include <Windows.h>
#include <d3d9.h>

#include <cstddef>
#include <cstdint>

struct D3D9ResourceAuditStats
{
    bool active = false;
    bool optionalHooksInstalled = false;
    bool hookCoverageComplete = true;
    unsigned long long samples = 0;
    unsigned long long created = 0;
    unsigned long long released = 0;
    unsigned long long live = 0;
    unsigned long long estimatedLiveBytes = 0;
    char logFileName[128] = {};
};

bool SetD3D9ResourceAuditEnabled(
    IDirect3DDevice9* device,
    bool enabled);

bool IsD3D9ResourceAuditEnabled();
D3D9ResourceAuditStats GetD3D9ResourceAuditStats();
void D3D9ResourceAuditOnPresent(IDirect3DDevice9* device);

void D3D9ResourceAuditTrackTexture(
    IDirect3DTexture9* texture,
    UINT width,
    UINT height,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    void* creationSite);

void D3D9ResourceAuditTrackRenderTarget(
    IDirect3DSurface9* surface,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    void* creationSite);

void D3D9ResourceAuditTrackDepthStencil(
    IDirect3DSurface9* surface,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    void* creationSite);

void D3D9ResourceAuditTrackVertexShader(
    IDirect3DVertexShader9* shader,
    void* creationSite);

void D3D9ResourceAuditTrackPixelShader(
    IDirect3DPixelShader9* shader,
    void* creationSite);
