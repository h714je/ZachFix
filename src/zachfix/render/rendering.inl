// -----------------------------------------------------------------------------
// Resolution helpers
// -----------------------------------------------------------------------------

static bool IsBaseRenderSize(UINT width, UINT height)
{
    return width == kBaseRenderWidth &&
           height == kBaseRenderHeight;
}


static bool IsKnownShadowMapSize(UINT width, UINT height)
{
    if (width != height)
        return false;

    return width == 512 || width == 1024;
}


static bool IsKnownShadowTexture(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    if (!IsKnownShadowMapSize(width, height))
        return false;

    const bool colorShadow =
        (usage & D3DUSAGE_RENDERTARGET) != 0 &&
        format == D3DFMT_A8R8G8B8;

    const bool depthShadow =
        (usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
        format == D3DFMT_D16;

    return colorShadow || depthShadow;
}


static UINT ScaleShadowDimension(UINT value)
{
    const unsigned long long scaled =
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(g_config.shadowScale);

    if (scaled > kMaxResolutionWidth)
        return kMaxResolutionWidth;

    return static_cast<UINT>(scaled);
}


static bool IsKnownReflectionSize(UINT width, UINT height)
{
    return
        (width == 640 && height == 360) ||
        (width == 320 && height == 180);
}


static bool IsKnownReflectionTexture(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    if (!IsKnownReflectionSize(width, height))
        return false;

    return
        format == D3DFMT_D24S8 ||
        (usage & D3DUSAGE_RENDERTARGET) != 0;
}


static UINT ScaleReflectionDimension(UINT value)
{
    const unsigned long long scaled =
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(g_config.reflectionScale);

    if (scaled > kMaxResolutionWidth)
        return kMaxResolutionWidth;

    return static_cast<UINT>(scaled);
}


static bool IsStorageRenderTarget(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    if ((usage & D3DUSAGE_RENDERTARGET) == 0)
        return false;

    // Observed in the Steam build and explicitly handled by original DPFix.
    if (width == 896 && height == 504)
        return format == D3DFMT_A8R8G8B8;

    if (width == 448 && height == 252)
        return format == D3DFMT_A8R8G8B8;

    return false;
}


static bool IsDofRenderTarget(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    if (width != 448 || height != 252)
        return false;

    if ((usage & D3DUSAGE_RENDERTARGET) == 0)
        return false;

    // The A8R8G8B8 448x252 target is the dual-scene storage RT handled above.
    // In our discovery logs the remaining 448x252 RT is A16B16G16R16F.
    return format != D3DFMT_A8R8G8B8;
}


static UINT ScaleFromBaseWidth(UINT value)
{
    return static_cast<UINT>(
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(g_internalWidth) /
        static_cast<unsigned long long>(kBaseRenderWidth)
    );
}


static UINT ScaleFromBaseHeight(UINT value)
{
    return static_cast<UINT>(
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(g_internalHeight) /
        static_cast<unsigned long long>(kBaseRenderHeight)
    );
}


static bool IsMainColorResource(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    if (!IsBaseRenderSize(width, height))
        return false;

    if ((usage & D3DUSAGE_RENDERTARGET) == 0)
        return false;

    // Observed in the Steam build:
    // 21  = D3DFMT_A8R8G8B8
    // 113 = D3DFMT_A16B16G16R16F
    return format == D3DFMT_A8R8G8B8 ||
           format == D3DFMT_A16B16G16R16F;
}


static bool IsMainDepthResource(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    return IsBaseRenderSize(width, height) &&
           (usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
           format == D3DFMT_D24S8;
}


static void RegisterMainRenderSurface(IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return;

    for (auto& slot : g_mainRenderSurfaces)
    {
        if (slot.load(std::memory_order_relaxed) == surface)
            return;
    }

    for (auto& slot : g_mainRenderSurfaces)
    {
        IDirect3DSurface9* expected = nullptr;

        if (slot.compare_exchange_strong(
                expected,
                surface,
                std::memory_order_release,
                std::memory_order_relaxed))
        {
            char text[256] = {};
            sprintf_s(
                text,
                "[Resolution] Registered main render surface: %p\n",
                static_cast<void*>(surface)
            );
            AppendLog(text);
            return;
        }
    }

    AppendLog("WARNING: No free slot for main render surface.\n");
}


static void RegisterMainTextureSurface(IDirect3DTexture9* texture)
{
    if (texture == nullptr)
        return;

    IDirect3DSurface9* surface = nullptr;

    if (SUCCEEDED(texture->GetSurfaceLevel(0, &surface)) &&
        surface != nullptr)
    {
        // Keep only the pointer identity. The texture owns the surface lifetime.
        RegisterMainRenderSurface(surface);
        surface->Release();
    }
}


static void ResetRenderTrackingForDeviceReset()
{
    // All cached surface identities refer to the pre-Reset implicit/default-pool
    // resource generation. They are raw pointer identities only, so carrying
    // them across Reset can make the viewport/final-output hooks compare new
    // surfaces against stale addresses.
    for (auto& slot : g_mainRenderSurfaces)
        slot.store(nullptr, std::memory_order_release);

    g_currentRenderTarget0.store(nullptr, std::memory_order_release);
    g_backBuffer0.store(nullptr, std::memory_order_release);
    g_firstStreamSourceAfterRenderTarget.store(true, std::memory_order_release);

    // Re-arm the one-shot resolution diagnostic for the new resource generation.
    g_loggedViewportOverride.store(false, std::memory_order_relaxed);
    g_currentViewportWidth.store(kBaseRenderWidth, std::memory_order_relaxed);
    g_currentViewportHeight.store(kBaseRenderHeight, std::memory_order_relaxed);
}


static bool IsRegisteredMainRenderSurface(IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return false;

    for (auto& slot : g_mainRenderSurfaces)
    {
        if (slot.load(std::memory_order_acquire) == surface)
            return true;
    }

    return false;
}


static void LogBackBufferInfo(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    IDirect3DSurface9* backBuffer = nullptr;

    if (FAILED(device->GetBackBuffer(
            0,
            0,
            D3DBACKBUFFER_TYPE_MONO,
            &backBuffer)) ||
        backBuffer == nullptr)
    {
        AppendLog("WARNING: Could not query back buffer 0.\n");
        return;
    }

    D3DSURFACE_DESC desc = {};

    if (SUCCEEDED(backBuffer->GetDesc(&desc)))
    {
        char text[256] = {};
        sprintf_s(
            text,
            "BackBuffer 0: %u x %u, Format=%u (0x%08X)\n",
            desc.Width,
            desc.Height,
            static_cast<unsigned>(desc.Format),
            static_cast<unsigned>(desc.Format)
        );
        AppendLog(text);
    }

    g_backBuffer0.store(
        backBuffer,
        std::memory_order_release
    );

    // Immediately after CreateDevice the backbuffer is render target 0
    // even if the game has not called SetRenderTarget yet.
    g_currentRenderTarget0.store(
        backBuffer,
        std::memory_order_release
    );

    backBuffer->Release();
}
