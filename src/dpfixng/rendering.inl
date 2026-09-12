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


static const char* ProfilerResourceTagName(
    ProfilerResourceTag tag)
{
    switch (tag)
    {
        case ProfilerResourceTag::BackBuffer:
            return "BackBuffer";
        case ProfilerResourceTag::MainLdr:
            return "MainLDR";
        case ProfilerResourceTag::MainHdr:
            return "MainHDR";
        case ProfilerResourceTag::MainDepth:
            return "MainDepth";
        case ProfilerResourceTag::ShadowColor512:
            return "ShadowColor512";
        case ProfilerResourceTag::ShadowDepth512:
            return "ShadowDepth512";
        case ProfilerResourceTag::ShadowColor1024:
            return "ShadowColor1024";
        case ProfilerResourceTag::ShadowDepth1024:
            return "ShadowDepth1024";
        case ProfilerResourceTag::ReflectionSmallColor:
            return "ReflectionSmallColor";
        case ProfilerResourceTag::ReflectionSmallDepth:
            return "ReflectionSmallDepth";
        case ProfilerResourceTag::ReflectionLargeColor:
            return "ReflectionLargeColor";
        case ProfilerResourceTag::ReflectionLargeDepth:
            return "ReflectionLargeDepth";
        case ProfilerResourceTag::Storage448:
            return "Storage448";
        case ProfilerResourceTag::Storage896:
            return "Storage896";
        case ProfilerResourceTag::DofHdr448:
            return "DoF-HDR-448";
        case ProfilerResourceTag::ExposureChain:
            return "ExposureChain";
        case ProfilerResourceTag::PostFx224:
            return "PostFX-224";
        case ProfilerResourceTag::PostFx256:
            return "PostFX-256";
        case ProfilerResourceTag::HdrAux128:
            return "HDR-Aux-128";
        default:
            return "Unknown";
    }
}


static ProfilerResourceTag ClassifyProfilerResource(
    UINT requestedWidth,
    UINT requestedHeight,
    DWORD usage,
    D3DFORMAT format)
{
    if (requestedWidth == kBaseRenderWidth &&
        requestedHeight == kBaseRenderHeight)
    {
        if ((usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
            format == D3DFMT_D24S8)
        {
            return ProfilerResourceTag::MainDepth;
        }

        if ((usage & D3DUSAGE_RENDERTARGET) != 0 &&
            format == D3DFMT_A16B16G16R16F)
        {
            return ProfilerResourceTag::MainHdr;
        }

        if ((usage & D3DUSAGE_RENDERTARGET) != 0 &&
            format == D3DFMT_A8R8G8B8)
        {
            return ProfilerResourceTag::MainLdr;
        }
    }

    if ((requestedWidth == 512 &&
         requestedHeight == 512) ||
        (requestedWidth == 1024 &&
         requestedHeight == 1024))
    {
        const bool is1024 =
            requestedWidth == 1024;

        if ((usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
            format == D3DFMT_D16)
        {
            return is1024
                ? ProfilerResourceTag::ShadowDepth1024
                : ProfilerResourceTag::ShadowDepth512;
        }

        if ((usage & D3DUSAGE_RENDERTARGET) != 0 &&
            format == D3DFMT_A8R8G8B8)
        {
            return is1024
                ? ProfilerResourceTag::ShadowColor1024
                : ProfilerResourceTag::ShadowColor512;
        }
    }

    if ((requestedWidth == 320 &&
         requestedHeight == 180) ||
        (requestedWidth == 640 &&
         requestedHeight == 360))
    {
        const bool large =
            requestedWidth == 640;

        if ((usage & D3DUSAGE_DEPTHSTENCIL) != 0 ||
            format == D3DFMT_D24S8)
        {
            return large
                ? ProfilerResourceTag::ReflectionLargeDepth
                : ProfilerResourceTag::ReflectionSmallDepth;
        }

        if ((usage & D3DUSAGE_RENDERTARGET) != 0)
        {
            return large
                ? ProfilerResourceTag::ReflectionLargeColor
                : ProfilerResourceTag::ReflectionSmallColor;
        }
    }

    if ((usage & D3DUSAGE_RENDERTARGET) != 0)
    {
        if (requestedWidth == 896 &&
            requestedHeight == 504 &&
            format == D3DFMT_A8R8G8B8)
        {
            return ProfilerResourceTag::Storage896;
        }

        if (requestedWidth == 448 &&
            requestedHeight == 252)
        {
            if (format == D3DFMT_A8R8G8B8)
                return ProfilerResourceTag::Storage448;

            if (format == D3DFMT_A16B16G16R16F)
                return ProfilerResourceTag::DofHdr448;
        }

        if (requestedWidth == 224 &&
            requestedHeight == 126 &&
            format == D3DFMT_A8R8G8B8)
        {
            return ProfilerResourceTag::PostFx224;
        }

        if (requestedWidth == 256 &&
            requestedHeight == 256 &&
            format == D3DFMT_A8R8G8B8)
        {
            return ProfilerResourceTag::PostFx256;
        }

        if (requestedWidth == 128 &&
            requestedHeight == 128 &&
            format == D3DFMT_A16B16G16R16F)
        {
            return ProfilerResourceTag::HdrAux128;
        }

        if (format == D3DFMT_A16B16G16R16F)
        {
            const bool exposureSize =
                (requestedWidth == 256 && requestedHeight == 128) ||
                (requestedWidth == 128 && requestedHeight == 64) ||
                (requestedWidth == 64 && requestedHeight == 32) ||
                (requestedWidth == 32 && requestedHeight == 16) ||
                (requestedWidth == 16 && requestedHeight == 8) ||
                (requestedWidth == 8 && requestedHeight == 4) ||
                (requestedWidth == 4 && requestedHeight == 2) ||
                (requestedWidth == 2 && requestedHeight == 1) ||
                (requestedWidth == 1 && requestedHeight == 1);

            if (exposureSize)
                return ProfilerResourceTag::ExposureChain;
        }
    }

    return ProfilerResourceTag::Unknown;
}


static const ProfilerResourceInfo* FindProfilerResource(
    UINT id)
{
    if (id == 0 ||
        id > g_profilerResources.size())
    {
        return nullptr;
    }

    return &g_profilerResources[
        static_cast<size_t>(id - 1)
    ];
}


static UINT FindProfilerResourceByTexture(
    IDirect3DBaseTexture9* texture)
{
    if (texture == nullptr)
        return 0;

    const auto it =
        g_profilerTextureToResource.find(texture);

    if (it == g_profilerTextureToResource.end())
        return 0;

    return it->second;
}


static UINT FindProfilerResourceBySurface(
    IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return 0;

    const auto it =
        g_profilerSurfaceToResource.find(surface);

    if (it == g_profilerSurfaceToResource.end())
        return 0;

    return it->second;
}


static UINT RegisterProfilerSurfaceResource(
    IDirect3DSurface9* surface,
    ProfilerResourceTag tag,
    UINT requestedWidth,
    UINT requestedHeight,
    UINT effectiveWidth,
    UINT effectiveHeight,
    D3DFORMAT format,
    DWORD usage,
    D3DPOOL pool)
{
    if (surface == nullptr)
        return 0;

    ProfilerResourceInfo info{};
    info.id =
        static_cast<UINT>(
            g_profilerResources.size() + 1
        );
    info.tag = tag;
    info.surfacePointer = surface;
    info.requestedWidth = requestedWidth;
    info.requestedHeight = requestedHeight;
    info.effectiveWidth = effectiveWidth;
    info.effectiveHeight = effectiveHeight;
    info.format = static_cast<UINT>(format);
    info.usage = usage;
    info.pool = pool;
    info.textureBacked = false;

    g_profilerResources.push_back(info);
    g_profilerSurfaceToResource[surface] = info.id;

    return info.id;
}


static UINT RegisterProfilerTextureResource(
    IDirect3DTexture9* texture,
    ProfilerResourceTag tag,
    UINT requestedWidth,
    UINT requestedHeight,
    UINT effectiveWidth,
    UINT effectiveHeight,
    D3DFORMAT format,
    DWORD usage,
    D3DPOOL pool)
{
    if (texture == nullptr)
        return 0;

    ProfilerResourceInfo info{};
    info.id =
        static_cast<UINT>(
            g_profilerResources.size() + 1
        );
    info.tag = tag;
    info.texturePointer = texture;
    info.requestedWidth = requestedWidth;
    info.requestedHeight = requestedHeight;
    info.effectiveWidth = effectiveWidth;
    info.effectiveHeight = effectiveHeight;
    info.format = static_cast<UINT>(format);
    info.usage = usage;
    info.pool = pool;
    info.textureBacked = true;

    IDirect3DSurface9* surface = nullptr;

    if (SUCCEEDED(texture->GetSurfaceLevel(
            0,
            &surface)) &&
        surface != nullptr)
    {
        info.surfacePointer = surface;
    }

    g_profilerResources.push_back(info);
    g_profilerTextureToResource[texture] = info.id;

    if (surface != nullptr)
    {
        g_profilerSurfaceToResource[surface] = info.id;
        surface->Release();
    }

    return info.id;
}


static void RegisterProfilerBackBuffer(
    IDirect3DSurface9* surface,
    const D3DSURFACE_DESC& desc)
{
    if (surface == nullptr)
        return;

    const auto existing =
        g_profilerSurfaceToResource.find(surface);

    if (existing !=
        g_profilerSurfaceToResource.end())
    {
        return;
    }

    RegisterProfilerSurfaceResource(
        surface,
        ProfilerResourceTag::BackBuffer,
        desc.Width,
        desc.Height,
        desc.Width,
        desc.Height,
        desc.Format,
        desc.Usage,
        D3DPOOL_DEFAULT
    );
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
        RegisterProfilerBackBuffer(
            backBuffer,
            desc
        );

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


static const char* ProfilerEventName(
    ProfilerEventType type)
{
    switch (type)
    {
        case ProfilerEventType::FrameStart:
            return "FrameStart";
        case ProfilerEventType::BeginScene:
            return "BeginScene";
        case ProfilerEventType::EndScene:
            return "EndScene";
        case ProfilerEventType::Present:
            return "Present";
        case ProfilerEventType::SetRenderTarget:
            return "SetRenderTarget";
        case ProfilerEventType::SetDepthStencilSurface:
            return "SetDepthStencilSurface";
        case ProfilerEventType::SetViewport:
            return "SetViewport";
        case ProfilerEventType::SetTexture:
            return "SetTexture";
        case ProfilerEventType::SetVertexShader:
            return "SetVertexShader";
        case ProfilerEventType::SetPixelShader:
            return "SetPixelShader";
        case ProfilerEventType::SetRenderState:
            return "SetRenderState";
        case ProfilerEventType::Clear:
            return "Clear";
        case ProfilerEventType::StretchRect:
            return "StretchRect";
        case ProfilerEventType::DrawPrimitive:
            return "DrawPrimitive";
        case ProfilerEventType::DrawIndexedPrimitive:
            return "DrawIndexedPrimitive";
        case ProfilerEventType::DrawPrimitiveUP:
            return "DrawPrimitiveUP";
        case ProfilerEventType::DrawIndexedPrimitiveUP:
            return "DrawIndexedPrimitiveUP";
        default:
            return "Unknown";
    }
}


static unsigned long long Fnv1a64(
    const unsigned char* data,
    size_t size)
{
    unsigned long long hash =
        1469598103934665603ull;

    for (size_t i = 0; i < size; ++i)
    {
        hash ^= static_cast<unsigned long long>(data[i]);
        hash *= 1099511628211ull;
    }

    return hash;
}


static bool GetProfilerOutputPath(
    wchar_t* path,
    size_t pathCount,
    UINT captureNumber,
    const wchar_t* suffix)
{
    const HMODULE module = GetLogModule();

    if (module == nullptr ||
        path == nullptr ||
        pathCount == 0 ||
        suffix == nullptr)
    {
        return false;
    }

    const DWORD length = GetModuleFileNameW(
        module,
        path,
        static_cast<DWORD>(pathCount)
    );

    if (length == 0 || length >= pathCount)
        return false;

    wchar_t* slash = wcsrchr(path, L'\\');

    if (slash == nullptr)
        return false;

    *(slash + 1) = L'\0';

    wchar_t filename[128] = {};

    if (swprintf_s(
            filename,
            L"DPFixNG-frame-%04u.%ls",
            captureNumber,
            suffix) < 0)
    {
        return false;
    }

    return wcscat_s(
        path,
        pathCount,
        filename
    ) == 0;
}


static double ProfilerMilliseconds(
    LONGLONG ticks)
{
    if (g_profilerQpcFrequency.QuadPart <= 0)
        return 0.0;

    return
        static_cast<double>(
            ticks - g_profilerStartCounter.QuadPart
        ) *
        1000.0 /
        static_cast<double>(
            g_profilerQpcFrequency.QuadPart
        );
}





