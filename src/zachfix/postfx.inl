// -----------------------------------------------------------------------------
// ZachFix PostFX NG framework
// -----------------------------------------------------------------------------

struct PostFxD3DXBuffer : IUnknown
{
    virtual LPVOID STDMETHODCALLTYPE GetBufferPointer() = 0;
    virtual DWORD STDMETHODCALLTYPE GetBufferSize() = 0;
};

using PostFxD3DXCompileShaderFn = HRESULT (WINAPI*)(
    LPCSTR sourceData,
    UINT sourceDataLength,
    const void* defines,
    void* includeHandler,
    LPCSTR entryPoint,
    LPCSTR profile,
    DWORD flags,
    PostFxD3DXBuffer** shader,
    PostFxD3DXBuffer** errors,
    void** constantTable);

namespace
{
struct PostFxTargetResource
{
    IDirect3DTexture9* texture = nullptr;
    IDirect3DSurface9* surface = nullptr;
    UINT width = 0;
    UINT height = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    unsigned long long estimatedBytes = 0;
};

std::mutex g_postFxMutex;
IDirect3DDevice9* g_postFxDevice = nullptr; // borrowed; game owns the device
std::array<PostFxTargetResource,
           static_cast<size_t>(PostFxTargetSlot::Count)> g_postFxTargets{};

std::atomic_bool g_postFxDeviceReady{ false };
std::atomic_uint g_postFxPixelShaderMajor{ 0 };
std::atomic_uint g_postFxPixelShaderMinor{ 0 };
std::atomic_uint g_postFxMaxMrt{ 0 };
std::atomic_uint g_postFxMaxTextureWidth{ 0 };
std::atomic_uint g_postFxMaxTextureHeight{ 0 };
std::atomic_ullong g_postFxFrameIndex{ 0 };
std::atomic_ullong g_postFxFullscreenPasses{ 0 };
std::atomic_ullong g_postFxResetCount{ 0 };
std::atomic_ullong g_postFxResourceGeneration{ 0 };
std::atomic_uint g_postFxAllocatedTargets{ 0 };
std::atomic_ullong g_postFxEstimatedBytes{ 0 };
std::atomic_bool g_postFxLoggedReady{ false };

IDirect3DTexture9* g_postFxGBufferDepth = nullptr;
IDirect3DTexture9* g_postFxGBufferNormal = nullptr;
unsigned long long g_postFxGBufferFrame = 0;
UINT g_postFxGBufferWidth = 0;
UINT g_postFxGBufferHeight = 0;

void ReplacePostFxTextureRef(
    IDirect3DTexture9*& slot,
    IDirect3DTexture9* texture)
{
    if (texture != nullptr)
        texture->AddRef();

    IDirect3DTexture9* old = slot;
    slot = texture;

    if (old != nullptr)
        old->Release();
}

void ReleasePostFxGBufferUnlocked()
{
    ReplacePostFxTextureRef(g_postFxGBufferDepth, nullptr);
    ReplacePostFxTextureRef(g_postFxGBufferNormal, nullptr);
    g_postFxGBufferFrame = 0;
    g_postFxGBufferWidth = 0;
    g_postFxGBufferHeight = 0;
}

unsigned long long EstimatePostFxTargetBytes(
    UINT width,
    UINT height,
    D3DFORMAT format)
{
    unsigned bytesPerPixel = 4;
    switch (format)
    {
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_A16B16G16R16:
        bytesPerPixel = 8;
        break;
    case D3DFMT_R32F:
    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
        bytesPerPixel = 4;
        break;
    case D3DFMT_R16F:
        bytesPerPixel = 2;
        break;
    default:
        bytesPerPixel = 4;
        break;
    }

    return static_cast<unsigned long long>(width) *
           static_cast<unsigned long long>(height) * bytesPerPixel;
}

void ReleasePostFxTargetUnlocked(PostFxTargetResource& target)
{
    if (target.surface != nullptr)
    {
        target.surface->Release();
        target.surface = nullptr;
    }
    if (target.texture != nullptr)
    {
        target.texture->Release();
        target.texture = nullptr;
    }

    target.width = 0;
    target.height = 0;
    target.format = D3DFMT_UNKNOWN;
    target.estimatedBytes = 0;
}

void RecalculatePostFxAllocationStatsUnlocked()
{
    UINT count = 0;
    unsigned long long bytes = 0;
    for (const PostFxTargetResource& target : g_postFxTargets)
    {
        if (target.texture == nullptr)
            continue;
        ++count;
        bytes += target.estimatedBytes;
    }

    g_postFxAllocatedTargets.store(count, std::memory_order_relaxed);
    g_postFxEstimatedBytes.store(bytes, std::memory_order_relaxed);
}

void ReleaseAllPostFxTargetsUnlocked()
{
    for (PostFxTargetResource& target : g_postFxTargets)
        ReleasePostFxTargetUnlocked(target);

    RecalculatePostFxAllocationStatsUnlocked();
    g_postFxResourceGeneration.fetch_add(1, std::memory_order_relaxed);
}

bool IsPostFxTargetRequestValid(
    UINT width,
    UINT height,
    D3DFORMAT format)
{
    if (width == 0 || height == 0 || format == D3DFMT_UNKNOWN)
        return false;

    const UINT maxWidth = g_postFxMaxTextureWidth.load(std::memory_order_relaxed);
    const UINT maxHeight = g_postFxMaxTextureHeight.load(std::memory_order_relaxed);
    if ((maxWidth != 0 && width > maxWidth) ||
        (maxHeight != 0 && height > maxHeight))
    {
        return false;
    }

    return true;
}
} // namespace

void InitializePostFxFramework(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    {
        std::lock_guard<std::mutex> lock(g_postFxMutex);
        if (g_postFxDevice != device)
        {
            ReleaseAllPostFxTargetsUnlocked();
            ReleasePostFxGBufferUnlocked();
            g_postFxDevice = device;
        }
    }

    D3DCAPS9 caps = {};
    if (FAILED(device->GetDeviceCaps(&caps)))
        return;

    g_postFxPixelShaderMajor.store(
        D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion),
        std::memory_order_relaxed);
    g_postFxPixelShaderMinor.store(
        D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion),
        std::memory_order_relaxed);
    g_postFxMaxMrt.store(
        caps.NumSimultaneousRTs,
        std::memory_order_relaxed);
    g_postFxMaxTextureWidth.store(caps.MaxTextureWidth, std::memory_order_relaxed);
    g_postFxMaxTextureHeight.store(caps.MaxTextureHeight, std::memory_order_relaxed);
    g_postFxDeviceReady.store(true, std::memory_order_release);

    bool expected = false;
    if (g_postFxLoggedReady.compare_exchange_strong(
            expected, true, std::memory_order_relaxed))
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[PostFX] Framework ready: ps_%u_%u, MRT=%u, max texture=%ux%u.\n",
            D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion),
            D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion),
            caps.NumSimultaneousRTs,
            caps.MaxTextureWidth,
            caps.MaxTextureHeight);
        AppendLog(text);
    }
}

void AdvancePostFxFrame()
{
    g_postFxFrameIndex.fetch_add(1, std::memory_order_relaxed);
}

unsigned long long GetPostFxFrameIndex()
{
    return g_postFxFrameIndex.load(std::memory_order_relaxed);
}

void ObservePostFxGBufferPair(
    IDirect3DTexture9* depth,
    IDirect3DTexture9* normal)
{
    if (depth == nullptr || normal == nullptr)
        return;

    D3DSURFACE_DESC depthDesc = {};
    D3DSURFACE_DESC normalDesc = {};
    if (FAILED(depth->GetLevelDesc(0, &depthDesc)) ||
        FAILED(normal->GetLevelDesc(0, &normalDesc)) ||
        depthDesc.Width != normalDesc.Width ||
        depthDesc.Height != normalDesc.Height)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(g_postFxMutex);
    ReplacePostFxTextureRef(g_postFxGBufferDepth, depth);
    ReplacePostFxTextureRef(g_postFxGBufferNormal, normal);
    g_postFxGBufferFrame = g_postFxFrameIndex.load(std::memory_order_relaxed);
    g_postFxGBufferWidth = depthDesc.Width;
    g_postFxGBufferHeight = depthDesc.Height;
}

bool AcquirePostFxGBuffer(PostFxGBufferView* view)
{
    if (view == nullptr)
        return false;

    *view = {};
    std::lock_guard<std::mutex> lock(g_postFxMutex);
    if (g_postFxGBufferDepth == nullptr || g_postFxGBufferNormal == nullptr)
        return false;

    g_postFxGBufferDepth->AddRef();
    g_postFxGBufferNormal->AddRef();
    view->depth = g_postFxGBufferDepth;
    view->normal = g_postFxGBufferNormal;
    view->width = g_postFxGBufferWidth;
    view->height = g_postFxGBufferHeight;
    view->frameIndex = g_postFxGBufferFrame;
    view->fresh =
        g_postFxGBufferFrame == g_postFxFrameIndex.load(std::memory_order_relaxed);
    return true;
}

void ReleasePostFxGBuffer(PostFxGBufferView* view)
{
    if (view == nullptr)
        return;

    if (view->depth != nullptr)
        view->depth->Release();
    if (view->normal != nullptr)
        view->normal->Release();
    *view = {};
}

void ReleasePostFxResources()
{
    std::lock_guard<std::mutex> lock(g_postFxMutex);
    ReleaseAllPostFxTargetsUnlocked();
    ReleasePostFxGBufferUnlocked();
}

void NotifyPostFxResetResult(IDirect3DDevice9* device, HRESULT resetResult)
{
    g_postFxResetCount.fetch_add(1, std::memory_order_relaxed);
    if (SUCCEEDED(resetResult))
        InitializePostFxFramework(device);
}

PostFxStats GetPostFxStats()
{
    PostFxStats stats{};
    stats.deviceReady = g_postFxDeviceReady.load(std::memory_order_acquire);
    stats.pixelShaderMajor = g_postFxPixelShaderMajor.load(std::memory_order_relaxed);
    stats.pixelShaderMinor = g_postFxPixelShaderMinor.load(std::memory_order_relaxed);
    stats.maxSimultaneousRenderTargets = g_postFxMaxMrt.load(std::memory_order_relaxed);
    stats.maxTextureWidth = g_postFxMaxTextureWidth.load(std::memory_order_relaxed);
    stats.maxTextureHeight = g_postFxMaxTextureHeight.load(std::memory_order_relaxed);
    stats.frameIndex = g_postFxFrameIndex.load(std::memory_order_relaxed);
    stats.fullscreenPasses = g_postFxFullscreenPasses.load(std::memory_order_relaxed);
    stats.resetCount = g_postFxResetCount.load(std::memory_order_relaxed);
    stats.resourceGeneration = g_postFxResourceGeneration.load(std::memory_order_relaxed);
    stats.allocatedTargets = g_postFxAllocatedTargets.load(std::memory_order_relaxed);
    stats.estimatedBytes = g_postFxEstimatedBytes.load(std::memory_order_relaxed);

    {
        std::lock_guard<std::mutex> lock(g_postFxMutex);
        stats.gbufferCaptured =
            g_postFxGBufferDepth != nullptr && g_postFxGBufferNormal != nullptr;
        stats.gbufferFrame = g_postFxGBufferFrame;
        stats.gbufferWidth = g_postFxGBufferWidth;
        stats.gbufferHeight = g_postFxGBufferHeight;
        stats.gbufferFresh =
            stats.gbufferCaptured &&
            g_postFxGBufferFrame == g_postFxFrameIndex.load(std::memory_order_relaxed);
    }
    return stats;
}

bool EnsurePostFxTarget(
    IDirect3DDevice9* device,
    PostFxTargetSlot slot,
    UINT width,
    UINT height,
    D3DFORMAT format,
    PostFxTargetView* view)
{
    if (view != nullptr)
        *view = {};

    const size_t index = static_cast<size_t>(slot);
    if (device == nullptr ||
        index >= g_postFxTargets.size() ||
        !IsPostFxTargetRequestValid(width, height, format) ||
        g_originalCreateTexture == nullptr)
    {
        return false;
    }

    InitializePostFxFramework(device);

    std::lock_guard<std::mutex> lock(g_postFxMutex);
    if (g_postFxDevice != device)
    {
        ReleaseAllPostFxTargetsUnlocked();
        ReleasePostFxGBufferUnlocked();
        g_postFxDevice = device;
    }

    PostFxTargetResource& target = g_postFxTargets[index];
    if (target.texture == nullptr ||
        target.surface == nullptr ||
        target.width != width ||
        target.height != height ||
        target.format != format)
    {
        ReleasePostFxTargetUnlocked(target);

        IDirect3DTexture9* texture = nullptr;
        const HRESULT createResult = g_originalCreateTexture(
            device,
            width,
            height,
            1,
            D3DUSAGE_RENDERTARGET,
            format,
            D3DPOOL_DEFAULT,
            &texture,
            nullptr);

        if (FAILED(createResult) || texture == nullptr)
        {
            char text[256] = {};
            sprintf_s(
                text,
                "[PostFX] WARNING: CreateTexture failed for slot %u (%ux%u fmt=0x%08X), HRESULT=0x%08X.\n",
                static_cast<unsigned>(slot),
                width,
                height,
                static_cast<unsigned>(format),
                static_cast<unsigned>(createResult));
            AppendLog(text);
            RecalculatePostFxAllocationStatsUnlocked();
            return false;
        }

        IDirect3DSurface9* surface = nullptr;
        const HRESULT surfaceResult = texture->GetSurfaceLevel(0, &surface);
        if (FAILED(surfaceResult) || surface == nullptr)
        {
            texture->Release();
            char text[256] = {};
            sprintf_s(
                text,
                "[PostFX] WARNING: GetSurfaceLevel failed for slot %u, HRESULT=0x%08X.\n",
                static_cast<unsigned>(slot),
                static_cast<unsigned>(surfaceResult));
            AppendLog(text);
            RecalculatePostFxAllocationStatsUnlocked();
            return false;
        }

        target.texture = texture;
        target.surface = surface;
        target.width = width;
        target.height = height;
        target.format = format;
        target.estimatedBytes = EstimatePostFxTargetBytes(width, height, format);

        g_postFxResourceGeneration.fetch_add(1, std::memory_order_relaxed);
        RecalculatePostFxAllocationStatsUnlocked();

        char text[256] = {};
        sprintf_s(
            text,
            "[PostFX] Target slot %u ready: %ux%u fmt=0x%08X.\n",
            static_cast<unsigned>(slot),
            width,
            height,
            static_cast<unsigned>(format));
        AppendLog(text);
    }

    if (view != nullptr)
    {
        view->texture = target.texture;
        view->surface = target.surface;
        view->width = target.width;
        view->height = target.height;
        view->format = target.format;
    }

    return true;
}

void ReleasePostFxTarget(PostFxTargetSlot slot)
{
    const size_t index = static_cast<size_t>(slot);
    if (index >= g_postFxTargets.size())
        return;

    std::lock_guard<std::mutex> lock(g_postFxMutex);
    ReleasePostFxTargetUnlocked(g_postFxTargets[index]);
    g_postFxResourceGeneration.fetch_add(1, std::memory_order_relaxed);
    RecalculatePostFxAllocationStatsUnlocked();
}

bool BeginPostFxStateBackup(IDirect3DDevice9* device, PostFxStateBackup* backup)
{
    if (device == nullptr || backup == nullptr)
        return false;

    *backup = {};

    for (DWORD i = 0; i < 4; ++i)
        device->GetRenderTarget(i, &backup->renderTargets[i]);
    device->GetDepthStencilSurface(&backup->depthStencil);
    backup->haveViewport = SUCCEEDED(device->GetViewport(&backup->viewport));

    IDirect3DStateBlock9* stateBlock = nullptr;
    if (FAILED(device->CreateStateBlock(D3DSBT_ALL, &stateBlock)) ||
        stateBlock == nullptr)
    {
        EndPostFxStateBackup(device, backup);
        return false;
    }

    if (FAILED(stateBlock->Capture()))
    {
        stateBlock->Release();
        EndPostFxStateBackup(device, backup);
        return false;
    }

    backup->stateBlock = stateBlock;
    backup->active = true;
    return true;
}

void EndPostFxStateBackup(IDirect3DDevice9* device, PostFxStateBackup* backup)
{
    if (backup == nullptr)
        return;

    if (device != nullptr && backup->active && backup->stateBlock != nullptr)
        backup->stateBlock->Apply();

    if (device != nullptr && backup->active)
    {
        // Some D3D9 wrappers do not reliably restore texture bindings from an
        // ALL state block. Re-submit only the stages ZachFix actually changed.
        if (g_originalSetTexture != nullptr)
        {
            for (DWORD stage = 0; stage < 16; ++stage)
            {
                if ((backup->textureStageMask & (1u << stage)) != 0)
                    g_originalSetTexture(device, stage, backup->textures[stage]);
            }
        }

        if (g_originalSetRenderTarget != nullptr)
        {
            for (DWORD i = 0; i < 4; ++i)
                g_originalSetRenderTarget(device, i, backup->renderTargets[i]);
        }

        if (g_originalSetDepthStencilSurface != nullptr)
            g_originalSetDepthStencilSurface(device, backup->depthStencil);

        if (backup->haveViewport && g_originalSetViewport != nullptr)
            g_originalSetViewport(device, &backup->viewport);
    }

    if (backup->stateBlock != nullptr)
    {
        backup->stateBlock->Release();
        backup->stateBlock = nullptr;
    }

    for (IDirect3DSurface9*& surface : backup->renderTargets)
    {
        if (surface != nullptr)
        {
            surface->Release();
            surface = nullptr;
        }
    }

    if (backup->depthStencil != nullptr)
    {
        backup->depthStencil->Release();
        backup->depthStencil = nullptr;
    }

    for (IDirect3DBaseTexture9*& texture : backup->textures)
    {
        if (texture != nullptr)
        {
            texture->Release();
            texture = nullptr;
        }
    }

    backup->textureStageMask = 0;
    backup->haveViewport = false;
    backup->viewport = {};
    backup->active = false;
}

bool RunPostFxFullscreenPass(
    IDirect3DDevice9* device,
    PostFxStateBackup* stateBackup,
    IDirect3DSurface9* output,
    UINT outputWidth,
    UINT outputHeight,
    IDirect3DPixelShader9* shader,
    const PostFxTextureBinding* bindings,
    size_t bindingCount,
    UINT constantStartRegister,
    const float* constants,
    UINT constantVector4Count,
    bool srgbWrite,
    PostFxBlendMode blendMode)
{
    if (device == nullptr || stateBackup == nullptr || !stateBackup->active ||
        output == nullptr || shader == nullptr ||
        outputWidth == 0 || outputHeight == 0 ||
        (bindingCount != 0 && bindings == nullptr) ||
        (constantVector4Count != 0 && constants == nullptr) ||
        g_originalSetRenderTarget == nullptr ||
        g_originalSetDepthStencilSurface == nullptr ||
        g_originalSetViewport == nullptr ||
        g_originalSetPixelShader == nullptr ||
        g_originalSetPixelShaderConstantF == nullptr ||
        g_originalSetVertexShader == nullptr ||
        g_originalSetTexture == nullptr ||
        g_originalSetSamplerState == nullptr ||
        g_originalDrawPrimitiveUP == nullptr)
    {
        return false;
    }

    const size_t safeBindingCount = std::min<size_t>(bindingCount, 16);
    // Capture original bindings before changing RT/state. Some wrappers may
    // implicitly unbind texture hazards when a render target is selected.
    for (size_t i = 0; i < safeBindingCount; ++i)
    {
        const PostFxTextureBinding& binding = bindings[i];
        if (binding.stage >= 16)
            continue;

        const unsigned int stageBit = 1u << binding.stage;
        if ((stateBackup->textureStageMask & stageBit) != 0)
            continue;

        IDirect3DBaseTexture9* originalTexture = nullptr;
        if (FAILED(device->GetTexture(binding.stage, &originalTexture)))
            return false;

        stateBackup->textures[binding.stage] = originalTexture;
        stateBackup->textureStageMask |= stageBit;
    }

    if (FAILED(g_originalSetRenderTarget(device, 0, output)))
        return false;
    for (DWORD i = 1; i < 4; ++i)
        g_originalSetRenderTarget(device, i, nullptr);
    g_originalSetDepthStencilSurface(device, nullptr);

    D3DVIEWPORT9 viewport = {};
    viewport.Width = outputWidth;
    viewport.Height = outputHeight;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    if (FAILED(g_originalSetViewport(device, &viewport)))
        return false;

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    if (blendMode == PostFxBlendMode::Multiply)
    {
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    }
    else if (blendMode == PostFxBlendMode::Additive)
    {
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
        device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    }
    else
    {
        device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    }
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
    device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbWrite ? TRUE : FALSE);

    g_originalSetVertexShader(device, nullptr);
    g_originalSetPixelShader(device, shader);
    if (constantVector4Count != 0)
    {
        if (FAILED(g_originalSetPixelShaderConstantF(
                device, constantStartRegister, constants, constantVector4Count)))
        {
            return false;
        }
    }
    device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

    for (size_t i = 0; i < safeBindingCount; ++i)
    {
        const PostFxTextureBinding& binding = bindings[i];
        if (binding.stage >= 16)
            continue;

        g_originalSetTexture(device, binding.stage, binding.texture);
        g_originalSetSamplerState(device, binding.stage, D3DSAMP_ADDRESSU, binding.addressMode);
        g_originalSetSamplerState(device, binding.stage, D3DSAMP_ADDRESSV, binding.addressMode);
        g_originalSetSamplerState(device, binding.stage, D3DSAMP_MINFILTER, binding.filter);
        g_originalSetSamplerState(device, binding.stage, D3DSAMP_MAGFILTER, binding.filter);
        g_originalSetSamplerState(device, binding.stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    }

    struct FullscreenVertex
    {
        float x;
        float y;
        float z;
        float rhw;
        float u;
        float v;
    };

    // D3D9 transformed-vertex half-pixel convention: -0.5 aligns texel and
    // pixel centers without baking resolution-specific offsets into shaders.
    const float right = static_cast<float>(outputWidth) - 0.5f;
    const float bottom = static_cast<float>(outputHeight) - 0.5f;
    const FullscreenVertex vertices[4] =
    {
        { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
        { right, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f },
        { -0.5f, bottom, 0.0f, 1.0f, 0.0f, 1.0f },
        { right, bottom, 0.0f, 1.0f, 1.0f, 1.0f }
    };

    const HRESULT result = g_originalDrawPrimitiveUP(
        device,
        D3DPT_TRIANGLESTRIP,
        2,
        vertices,
        sizeof(FullscreenVertex));

    // Avoid D3D9 read/write hazards when a later pass uses one of these input
    // textures as its output render target.
    for (size_t i = 0; i < safeBindingCount; ++i)
    {
        if (bindings[i].stage < 16)
            g_originalSetTexture(device, bindings[i].stage, nullptr);
    }

    if (SUCCEEDED(result))
        g_postFxFullscreenPasses.fetch_add(1, std::memory_order_relaxed);

    return SUCCEEDED(result);
}

bool CompilePostFxPixelShader(
    IDirect3DDevice9* device,
    const char* source,
    const char* entryPoint,
    const char* debugName,
    IDirect3DPixelShader9** shader)
{
    if (shader != nullptr)
        *shader = nullptr;

    if (device == nullptr || source == nullptr || entryPoint == nullptr ||
        shader == nullptr || g_originalCreatePixelShader == nullptr)
    {
        return false;
    }

    HMODULE d3dx9 = GetModuleHandleW(L"d3dx9_43.dll");
    if (d3dx9 == nullptr)
        d3dx9 = LoadLibraryW(L"d3dx9_43.dll");
    if (d3dx9 == nullptr)
    {
        AppendLog("[PostFX] WARNING: d3dx9_43.dll unavailable; runtime shader compilation disabled.\n");
        return false;
    }

    const auto compileShader = reinterpret_cast<PostFxD3DXCompileShaderFn>(
        GetProcAddress(d3dx9, "D3DXCompileShader"));
    if (compileShader == nullptr)
    {
        AppendLog("[PostFX] WARNING: D3DXCompileShader export unavailable.\n");
        return false;
    }

    PostFxD3DXBuffer* bytecode = nullptr;
    PostFxD3DXBuffer* errors = nullptr;
    const HRESULT compileResult = compileShader(
        source,
        static_cast<UINT>(strlen(source)),
        nullptr,
        nullptr,
        entryPoint,
        "ps_3_0",
        0,
        &bytecode,
        &errors,
        nullptr);

    if (FAILED(compileResult) || bytecode == nullptr)
    {
        char text[512] = {};
        const char* label = debugName != nullptr ? debugName : "unnamed";
        sprintf_s(
            text,
            "[PostFX] WARNING: shader '%s' compilation failed (HRESULT=0x%08X).\n",
            label,
            static_cast<unsigned>(compileResult));
        AppendLog(text);

        if (errors != nullptr && errors->GetBufferPointer() != nullptr)
        {
            AppendLog(static_cast<const char*>(errors->GetBufferPointer()));
            AppendLog("\n");
        }

        if (errors != nullptr)
            errors->Release();
        if (bytecode != nullptr)
            bytecode->Release();
        return false;
    }

    IDirect3DPixelShader9* created = nullptr;
    const HRESULT createResult = g_originalCreatePixelShader(
        device,
        static_cast<const DWORD*>(bytecode->GetBufferPointer()),
        &created);

    if (errors != nullptr)
        errors->Release();
    bytecode->Release();

    if (FAILED(createResult) || created == nullptr)
    {
        char text[256] = {};
        const char* label = debugName != nullptr ? debugName : "unnamed";
        sprintf_s(
            text,
            "[PostFX] WARNING: shader '%s' creation failed (HRESULT=0x%08X).\n",
            label,
            static_cast<unsigned>(createResult));
        AppendLog(text);
        return false;
    }

    *shader = created;
    return true;
}

void GetPostFxScaledExtent(
    UINT baseWidth,
    UINT baseHeight,
    UINT divisor,
    UINT* width,
    UINT* height)
{
    const UINT safeDivisor = std::max<UINT>(divisor, 1);
    if (width != nullptr)
        *width = std::max<UINT>(1, (baseWidth + safeDivisor - 1) / safeDivisor);
    if (height != nullptr)
        *height = std::max<UINT>(1, (baseHeight + safeDivisor - 1) / safeDivisor);
}
