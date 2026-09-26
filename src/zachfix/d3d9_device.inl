// -----------------------------------------------------------------------------
// Device hooks
// -----------------------------------------------------------------------------

static void ObservePostFxGBufferRenderTarget(
    IDirect3DDevice9* device,
    DWORD index,
    IDirect3DSurface9* effectiveTarget);

static HRESULT WINAPI HookCreateTexture(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DTexture9** texture,
    HANDLE* sharedHandle)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalCreateTexture(
            self, width, height, levels, usage, format, pool, texture, sharedHandle);
    }

    const UINT originalWidth = width;
    const UINT originalHeight = height;
    const D3DFORMAT originalFormat = format;

    const bool isKnownShadow =
        IsKnownShadowTexture(width, height, usage, format);

    const bool isKnownShadowDepth =
        isKnownShadow &&
        (usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
        format == D3DFMT_D16;

    const bool isKnownReflection =
        IsKnownReflectionTexture(width, height, usage, format);

    const bool isStorageRt =
        IsStorageRenderTarget(width, height, usage, format);

    const bool isDofRt =
        IsDofRenderTarget(width, height, usage, format);

    const bool isMainColor =
        IsMainColorResource(width, height, usage, format);

    const bool isMainDepth =
        IsMainDepthResource(width, height, usage, format);

    if (isKnownShadow && g_config.shadowScale > 1)
    {
        width = ScaleShadowDimension(width);
        height = ScaleShadowDimension(height);
    }
    else if (isKnownReflection && g_config.reflectionScale > 1)
    {
        width = ScaleReflectionDimension(width);
        height = ScaleReflectionDimension(height);
    }
    else if (isStorageRt)
    {
        width = ScaleFromBaseWidth(width);
        height = ScaleFromBaseHeight(height);
    }
    else if (isDofRt && g_config.improveDofResolution)
    {
        // 448/1280 == 252/720 == 0.35, exactly matching original DPFix.
        width = ScaleFromBaseWidth(width);
        height = ScaleFromBaseHeight(height);
    }
    else if (isMainColor || isMainDepth)
    {
        width = g_internalWidth;
        height = g_internalHeight;
    }

    // Original DPFix compatibility fix by Peter "Durante" Thoman.
    // DP's shadow-map depth textures are D16. Replacing only the recognized
    // 512/1024 shadow depth resources with D32F_LOCKABLE reduces depth
    // quantization artifacts that show up as saw-tooth shadow edges.
    const bool requestImprovedShadowDepth =
        isKnownShadowDepth && g_config.improveShadowPrecision;

    if (requestImprovedShadowDepth)
        format = D3DFMT_D32F_LOCKABLE;

    // DP creates its full-resolution main D24S8 as a texture. Prefer sampleable
    // INTZ so ZachFix PostFX can read the hardware depth directly while native
    // DP shaders continue using their separate packed-depth RT0 unchanged.
    const bool requestNativeDepthTexture =
        isMainDepth &&
        originalFormat == D3DFMT_D24S8 &&
        (usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
        levels == 1 &&
        pool == D3DPOOL_DEFAULT &&
        texture != nullptr &&
        sharedHandle == nullptr;

    HRESULT result = E_FAIL;
    bool nativeDepthTextureActive = false;
    if (requestNativeDepthTexture)
    {
        IDirect3DTexture9* sampleableDepthTexture = nullptr;
        IDirect3DSurface9* sampleableDepthSurface = nullptr;
        if (TryCreatePostFxSampleableMainDepth(
                self,
                width,
                height,
                D3DMULTISAMPLE_NONE,
                0,
                &sampleableDepthTexture,
                &sampleableDepthSurface))
        {
            if (sampleableDepthSurface != nullptr)
                sampleableDepthSurface->Release();

            *texture = sampleableDepthTexture;
            format = kPostFxIntzFormat;
            result = S_OK;
            nativeDepthTextureActive = true;

            AppendLog(
                "[PostFX][Depth] Main D24S8 texture upgraded to sampleable INTZ.\n");
        }
    }

    if (!nativeDepthTextureActive)
    {
        result = g_originalCreateTexture(
            self,
            width,
            height,
            levels,
            usage,
            format,
            pool,
            texture,
            sharedHandle
        );

        if (requestNativeDepthTexture && SUCCEEDED(result))
        {
            AppendLog(
                "[PostFX][Depth] INTZ unavailable for main depth texture; "
                "using native D24S8 with packed-depth PostFX fallback.\n");
        }
    }

    // Keep wrappers/backends usable even if they reject D32F_LOCKABLE.
    // A failed precision upgrade falls back to the game's original D16.
    if (FAILED(result) && requestImprovedShadowDepth)
    {
        AppendLog(
            "[Shadows] WARNING: D32F_LOCKABLE shadow depth creation failed; "
            "falling back to D16.\n"
        );
        format = originalFormat;
        result = g_originalCreateTexture(
            self,
            width,
            height,
            levels,
            usage,
            format,
            pool,
            texture,
            sharedHandle
        );
    }

    if (SUCCEEDED(result) &&
        texture != nullptr &&
        *texture != nullptr)
    {
        TrackRuntimeTextureResource(
            *texture,
            originalWidth,
            originalHeight,
            width,
            height,
            levels,
            usage,
            originalFormat,
            format,
            pool);

    }

    if (SUCCEEDED(result) &&
        requestImprovedShadowDepth &&
        format == D3DFMT_D32F_LOCKABLE)
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[Shadows] DPFix precision correction: %u x %u D16 -> D32F_LOCKABLE.\n",
            width,
            height
        );
        AppendLog(text);
    }

    if (SUCCEEDED(result) &&
        isKnownShadow &&
        g_config.shadowScale > 1)
    {
        char text[512] = {};

        sprintf_s(
            text,
            "[Shadows] Texture %u x %u -> %u x %u, "
            "Format=%u (0x%08X), Usage=0x%08X\n",
            originalWidth,
            originalHeight,
            width,
            height,
            static_cast<unsigned>(format),
            static_cast<unsigned>(format),
            usage
        );

        AppendLog(text);
    }

    if (SUCCEEDED(result) &&
        isKnownReflection &&
        g_config.reflectionScale > 1)
    {
        char text[512] = {};

        sprintf_s(
            text,
            "[Reflections] Texture %u x %u -> %u x %u, "
            "Format=%u (0x%08X), Usage=0x%08X\n",
            originalWidth,
            originalHeight,
            width,
            height,
            static_cast<unsigned>(format),
            static_cast<unsigned>(format),
            usage
        );

        AppendLog(text);
    }

    if (SUCCEEDED(result) && isStorageRt)
    {
        char text[512] = {};

        sprintf_s(
            text,
            "[PostFX] Storage RT %u x %u -> %u x %u, "
            "Format=%u (0x%08X)\n",
            originalWidth,
            originalHeight,
            width,
            height,
            static_cast<unsigned>(format),
            static_cast<unsigned>(format)
        );

        AppendLog(text);
    }

    if (SUCCEEDED(result) &&
        isDofRt &&
        g_config.improveDofResolution)
    {
        char text[512] = {};

        sprintf_s(
            text,
            "[DoF] Texture %u x %u -> %u x %u, "
            "Format=%u (0x%08X)\n",
            originalWidth,
            originalHeight,
            width,
            height,
            static_cast<unsigned>(format),
            static_cast<unsigned>(format)
        );

        AppendLog(text);
    }

    if (SUCCEEDED(result) && (isMainColor || isMainDepth))
    {
        LogResolutionOverride(
            "CreateTexture",
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            usage
        );

        if (isMainColor &&
            texture != nullptr &&
            *texture != nullptr)
        {
            RegisterMainTextureSurface(*texture);
        }
    }


    return result;
}


static HRESULT WINAPI HookCreateRenderTarget(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL lockable,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalCreateRenderTarget(
            self, width, height, format, multiSample, multiSampleQuality,
            lockable, surface, sharedHandle);
    }

    const UINT originalWidth = width;
    const UINT originalHeight = height;

    const bool isMainColor =
        IsMainColorResource(
            width,
            height,
            D3DUSAGE_RENDERTARGET,
            format
        );

    if (isMainColor)
    {
        width = g_internalWidth;
        height = g_internalHeight;
    }

    const HRESULT result = g_originalCreateRenderTarget(
        self,
        width,
        height,
        format,
        multiSample,
        multiSampleQuality,
        lockable,
        surface,
        sharedHandle
    );

    if (SUCCEEDED(result) &&
        isMainColor &&
        surface != nullptr &&
        *surface != nullptr)
    {
        TrackRuntimeSurfaceResource(
            *surface,
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            D3DUSAGE_RENDERTARGET,
            D3DPOOL_DEFAULT,
            multiSample,
            multiSampleQuality,
            lockable);
    }

    if (SUCCEEDED(result) && isMainColor)
    {
        LogResolutionOverride(
            "CreateRenderTarget",
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            D3DUSAGE_RENDERTARGET
        );

        if (surface != nullptr && *surface != nullptr)
            RegisterMainRenderSurface(*surface);
    }


    return result;
}


static HRESULT WINAPI HookCreateDepthStencilSurface(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL discard,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalCreateDepthStencilSurface(
            self, width, height, format, multiSample, multiSampleQuality,
            discard, surface, sharedHandle);
    }

    const UINT originalWidth = width;
    const UINT originalHeight = height;

    const bool isMainDepth =
        IsMainDepthResource(
            width,
            height,
            D3DUSAGE_DEPTHSTENCIL,
            format
        );

    if (isMainDepth)
    {
        width = g_internalWidth;
        height = g_internalHeight;
    }

    HRESULT result = E_FAIL;
    IDirect3DTexture9* sampleableDepthTexture = nullptr;
    const bool requestNativeDepth = isMainDepth;

    if (requestNativeDepth && surface != nullptr && sharedHandle == nullptr &&
        TryCreatePostFxSampleableMainDepth(
            self,
            width,
            height,
            multiSample,
            multiSampleQuality,
            &sampleableDepthTexture,
            surface))
    {
        result = S_OK;
        TrackRuntimeTextureResource(
            sampleableDepthTexture,
            originalWidth,
            originalHeight,
            width,
            height,
            1,
            D3DUSAGE_DEPTHSTENCIL,
            D3DFMT_D24S8,
            kPostFxIntzFormat,
            D3DPOOL_DEFAULT);
        AppendLog(
            "[PostFX][Depth] Main D24S8 surface upgraded to sampleable INTZ.\n");
        sampleableDepthTexture->Release();
        sampleableDepthTexture = nullptr;
    }
    else
    {
        result = g_originalCreateDepthStencilSurface(
            self,
            width,
            height,
            format,
            multiSample,
            multiSampleQuality,
            discard,
            surface,
            sharedHandle
        );

        if (requestNativeDepth && SUCCEEDED(result))
        {
            AppendLog(
                "[PostFX][Depth] INTZ unavailable for main depth surface; "
                "using native D24S8 with packed-depth PostFX fallback.\n");
        }

        if (SUCCEEDED(result) &&
            isMainDepth &&
            surface != nullptr &&
            *surface != nullptr)
        {
            TrackRuntimeSurfaceResource(
                *surface,
                originalWidth,
                originalHeight,
                width,
                height,
                format,
                D3DUSAGE_DEPTHSTENCIL,
                D3DPOOL_DEFAULT,
                multiSample,
                multiSampleQuality,
                discard);
        }
    }

    if (SUCCEEDED(result) && isMainDepth)
    {
        LogResolutionOverride(
            "CreateDepthStencilSurface",
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            D3DUSAGE_DEPTHSTENCIL
        );
    }


    return result;
}


static HRESULT WINAPI HookSetRenderTarget(
    IDirect3DDevice9* self,
    DWORD index,
    IDirect3DSurface9* target)
{
    if (!IsGameD3D9Device(self))
        return g_originalSetRenderTarget(self, index, target);

    if (index == 0)
        ObserveAndApplyAdditionalDofBlur(self);

    RuntimeSurfaceBinding binding = AcquireRuntimeSurfaceBinding(target);
    IDirect3DSurface9* logicalTarget = binding.logical;
    IDirect3DSurface9* effectiveTarget =
        binding.replacement != nullptr ? binding.replacement : logicalTarget;

    const HRESULT result = g_originalSetRenderTarget(
        self,
        index,
        effectiveTarget
    );

    if (SUCCEEDED(result))
    {
        ObservePostFxGBufferRenderTarget(self, index, effectiveTarget);

        // Original DPFix only applies its dual-view correction on the first
        // SetStreamSource after a render-target change.
        g_firstStreamSourceAfterRenderTarget.store(
            true,
            std::memory_order_release
        );

        if (index == 0)
        {
            g_currentRenderTarget0.store(
                logicalTarget,
                std::memory_order_release
            );
        }
    }

    if (binding.replacement != nullptr)
        binding.replacement->Release();

    return result;
}


static UINT ScaleCoordinate(
    UINT value,
    UINT targetSize,
    UINT baseSize)
{
    return static_cast<UINT>(
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(targetSize) /
        static_cast<unsigned long long>(baseSize)
    );
}


static bool NearlyEqualFloat(
    float a,
    float b,
    float epsilon = 1.0e-7f)
{
    return std::fabs(a - b) <= epsilon;
}


static void RefreshVertexPixelSizeForViewport(
    IDirect3DDevice9* self,
    UINT width,
    UINT height)
{
    if (!g_config.fixPixelOffset ||
        !g_lastVertexC0WasPixelSize.load(std::memory_order_relaxed) ||
        g_originalSetVertexShaderConstantF == nullptr ||
        width == 0 ||
        height == 0)
    {
        return;
    }

    const float replacement[4] =
    {
        0.5f / static_cast<float>(width),
        0.5f / static_cast<float>(height),
        0.0f,
        0.0f
    };

    g_originalSetVertexShaderConstantF(
        self,
        0,
        replacement,
        1
    );

    static std::atomic_bool loggedRefresh{ false };
    bool expected = false;

    if (loggedRefresh.compare_exchange_strong(
            expected,
            true,
            std::memory_order_relaxed))
    {
        char text[256] = {};

        sprintf_s(
            text,
            "[PixelOffset] VS c0 refreshed after viewport change: "
            "%u x %u -> {%.9f, %.9f, 0, 0}\n",
            width,
            height,
            replacement[0],
            replacement[1]
        );

        AppendLog(text);
    }
}


static HRESULT SubmitViewport(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport)
{
    if (viewport == nullptr)
        return g_originalSetViewport(self, viewport);

    g_currentViewportWidth.store(
        viewport->Width,
        std::memory_order_relaxed
    );

    g_currentViewportHeight.store(
        viewport->Height,
        std::memory_order_relaxed
    );

    RefreshVertexPixelSizeForViewport(
        self,
        viewport->Width,
        viewport->Height
    );

    return g_originalSetViewport(
        self,
        viewport
    );
}


static bool IsPostFxGameCall(void* returnAddress)
{
    if (!g_mainExeInfoValid && !InitializeMainExeInfo())
        return false;

    const uintptr_t address = reinterpret_cast<uintptr_t>(returnAddress);
    return address >= g_mainExeBase &&
           address < g_mainExeBase + g_mainExeSize;
}

// -----------------------------------------------------------------------------
// PostFX game-resource tracking
// -----------------------------------------------------------------------------

static std::mutex g_postFxGBufferCaptureMutex;
static IDirect3DTexture9* g_postFxCurrentRt0 = nullptr;
static IDirect3DTexture9* g_postFxCurrentRt1 = nullptr;
static std::atomic<IDirect3DSurface9*> g_postFxCurrentRtSurface0{ nullptr };
static std::atomic<IDirect3DSurface9*> g_postFxCurrentRtSurface1{ nullptr };
static std::atomic_bool g_postFxGBufferPairActive{ false };
static std::atomic_bool g_postFxGBufferTrackingEnabled{ false };
static std::atomic_bool g_postFxLoggedGBufferPair{ false };
static std::atomic_ullong g_postFxProjectionCapturedFrame{ ~0ull };
static std::atomic_ullong g_postFxNativeDepthCapturedFrame{ ~0ull };
static std::atomic_bool g_postFxFinalCompositeBound{ false };
static std::atomic<IDirect3DPixelShader9*> g_postFxFinalCompositeShader{ nullptr };

static void ReplacePostFxCapturedTextureRef(
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

static bool ReleasePostFxGBufferCaptureRefs()
{
    std::lock_guard<std::mutex> lock(g_postFxGBufferCaptureMutex);

    const bool hadRefs =
        g_postFxCurrentRt0 != nullptr ||
        g_postFxCurrentRt1 != nullptr;

    ReplacePostFxCapturedTextureRef(g_postFxCurrentRt0, nullptr);
    ReplacePostFxCapturedTextureRef(g_postFxCurrentRt1, nullptr);
    g_postFxCurrentRtSurface0.store(nullptr, std::memory_order_relaxed);
    g_postFxCurrentRtSurface1.store(nullptr, std::memory_order_relaxed);
    g_postFxGBufferPairActive.store(false, std::memory_order_release);
    g_postFxProjectionCapturedFrame.store(~0ull, std::memory_order_relaxed);
    g_postFxNativeDepthCapturedFrame.store(~0ull, std::memory_order_relaxed);
    return hadRefs;
}

static IDirect3DTexture9* GetPostFxSurfaceTexture(IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return nullptr;

    IDirect3DTexture9* texture = nullptr;
    if (FAILED(surface->GetContainer(
            __uuidof(IDirect3DTexture9),
            reinterpret_cast<void**>(&texture))))
    {
        return nullptr;
    }

    return texture;
}

static bool IsPostFxGBufferPair(
    IDirect3DTexture9* depth,
    IDirect3DTexture9* normal)
{
    if (depth == nullptr || normal == nullptr)
        return false;

    D3DSURFACE_DESC depthDesc = {};
    D3DSURFACE_DESC normalDesc = {};
    if (FAILED(depth->GetLevelDesc(0, &depthDesc)) ||
        FAILED(normal->GetLevelDesc(0, &normalDesc)))
    {
        return false;
    }

    return depthDesc.Width == normalDesc.Width &&
           depthDesc.Height == normalDesc.Height &&
           depthDesc.Width == g_internalWidth &&
           depthDesc.Height == g_internalHeight &&
           depthDesc.Format == D3DFMT_A8R8G8B8 &&
           normalDesc.Format == D3DFMT_A8R8G8B8 &&
           (depthDesc.Usage & D3DUSAGE_RENDERTARGET) != 0 &&
           (normalDesc.Usage & D3DUSAGE_RENDERTARGET) != 0;
}

static void CapturePostFxActiveNativeDepth(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    const unsigned long long frame = GetPostFxFrameIndex();
    if (g_postFxNativeDepthCapturedFrame.load(std::memory_order_relaxed) == frame)
        return;

    // The G-buffer surfaces are normally reused every frame, but the depth
    // freshness contract is frame-based. Refresh the active depth identity
    // once per frame instead of tying it to a render-target identity change.
    ObservePostFxSampleableMainDepth(nullptr);

    bool captureComplete = false;
    IDirect3DSurface9* activeDepthSurface = nullptr;
    if (SUCCEEDED(device->GetDepthStencilSurface(&activeDepthSurface)) &&
        activeDepthSurface != nullptr)
    {
        D3DSURFACE_DESC activeDepthDesc = {};
        const HRESULT descResult = activeDepthSurface->GetDesc(&activeDepthDesc);
        const bool expectedSize =
            SUCCEEDED(descResult) &&
            activeDepthDesc.Width == g_internalWidth &&
            activeDepthDesc.Height == g_internalHeight;

        if (expectedSize && activeDepthDesc.Format == kPostFxIntzFormat)
        {
            IDirect3DTexture9* activeDepthTexture = nullptr;
            const HRESULT containerResult = activeDepthSurface->GetContainer(
                __uuidof(IDirect3DTexture9),
                reinterpret_cast<void**>(&activeDepthTexture));

            if (SUCCEEDED(containerResult) && activeDepthTexture != nullptr)
            {
                ObservePostFxSampleableMainDepth(activeDepthTexture);
                captureComplete = true;

                static std::atomic_bool loggedActiveNativeDepth{ false };
                bool logExpected = false;
                if (loggedActiveNativeDepth.compare_exchange_strong(
                        logExpected, true, std::memory_order_relaxed))
                {
                    AppendLog(
                        "[PostFX][Depth] Captured active G-buffer INTZ depth.\n");
                }
            }

            if (activeDepthTexture != nullptr)
                activeDepthTexture->Release();
        }
        else if (expectedSize && activeDepthDesc.Format == D3DFMT_D24S8)
        {
            // A correctly sized native D24S8 is a definitive packed-depth
            // fallback for this frame, so no further COM queries are needed.
            captureComplete = true;
        }

        activeDepthSurface->Release();
    }

    // If the expected geometry depth was not bound yet, allow a later
    // SetRenderTarget in the same frame to retry instead of caching a miss.
    if (captureComplete)
        g_postFxNativeDepthCapturedFrame.store(frame, std::memory_order_relaxed);
}

static void ObservePostFxGBufferRenderTarget(
    IDirect3DDevice9* device,
    DWORD index,
    IDirect3DSurface9* effectiveTarget)
{
    if (index > 1)
        return;

    const bool trackingRequired =
        IsPostFxGBufferCaptureRequired() ||
        IsPostFxPreviewFreezeRequestedOrActive();
    if (!trackingRequired)
    {
        if (g_postFxGBufferTrackingEnabled.exchange(
                false, std::memory_order_acq_rel))
        {
            ReleasePostFxGBufferCaptureRefs();
        }
        return;
    }

    if (!g_postFxGBufferTrackingEnabled.exchange(
            true, std::memory_order_acq_rel))
    {
        ReleasePostFxGBufferCaptureRefs();
    }

    // SetRenderTarget frequently re-submits the same surface. Once tracking is
    // active, skip the COM GetContainer + mutex + descriptor validation when
    // the identity for this slot has not changed.
    std::atomic<IDirect3DSurface9*>& surfaceSlot = index == 0
        ? g_postFxCurrentRtSurface0
        : g_postFxCurrentRtSurface1;
    if (surfaceSlot.load(std::memory_order_acquire) == effectiveTarget)
    {
        if (g_postFxGBufferPairActive.load(std::memory_order_acquire))
            CapturePostFxActiveNativeDepth(device);
        return;
    }

    IDirect3DTexture9* texture = GetPostFxSurfaceTexture(effectiveTarget);

    std::lock_guard<std::mutex> lock(g_postFxGBufferCaptureMutex);
    if (surfaceSlot.load(std::memory_order_relaxed) == effectiveTarget)
    {
        if (texture != nullptr)
            texture->Release();
        return;
    }
    surfaceSlot.store(effectiveTarget, std::memory_order_release);

    if (index == 0)
        ReplacePostFxCapturedTextureRef(g_postFxCurrentRt0, texture);
    else
        ReplacePostFxCapturedTextureRef(g_postFxCurrentRt1, texture);

    if (texture != nullptr)
        texture->Release();

    const bool pairActive = IsPostFxGBufferPair(
        g_postFxCurrentRt0,
        g_postFxCurrentRt1);
    g_postFxGBufferPairActive.store(pairActive, std::memory_order_release);

    if (!pairActive)
        return;

    ObservePostFxGBufferPair(g_postFxCurrentRt0, g_postFxCurrentRt1);

    // Bind NativeD24 to the depth texture that is actually active while the
    // game's full-resolution G-buffer pair is bound. DP creates two full-size
    // D24S8 resources, so creation order is not a reliable selector.
    CapturePostFxActiveNativeDepth(device);

    bool expected = false;
    if (g_postFxLoggedGBufferPair.compare_exchange_strong(
            expected, true, std::memory_order_relaxed))
    {
        AppendLog(
            "[PostFX] Captured full-resolution G-buffer pair: "
            "RT0 packed depth + RT1 encoded view-space normals.\n");
    }
}

static bool CapturePostFxProjectionColumns(const float* columns)
{
    if (columns == nullptr)
        return false;

    const float projectionScaleX = std::sqrt(
        columns[0] * columns[0] +
        columns[1] * columns[1] +
        columns[2] * columns[2]);
    const float projectionScaleY = std::sqrt(
        columns[4] * columns[4] +
        columns[5] * columns[5] +
        columns[6] * columns[6]);

    // c245..c248 are the columns of DP's combined view-projection matrix.
    // For the D3D perspective form, col3.xyz = q * col4.xyz and
    // col3.w = q * col4.w - qn, giving z_ndc = q - qn / viewZ.
    const float forwardLength2 =
        columns[12] * columns[12] +
        columns[13] * columns[13] +
        columns[14] * columns[14];
    if (forwardLength2 <= 1e-6f)
        return false;

    const float projectionDepthQ =
        (columns[8] * columns[12] +
         columns[9] * columns[13] +
         columns[10] * columns[14]) / forwardLength2;
    const float projectionDepthQn =
        projectionDepthQ * columns[15] - columns[11];

    if (!std::isfinite(projectionScaleX) ||
        !std::isfinite(projectionScaleY) ||
        !std::isfinite(projectionDepthQ) ||
        !std::isfinite(projectionDepthQn) ||
        projectionScaleX <= 0.01f || projectionScaleY <= 0.01f)
    {
        return false;
    }

    ObservePostFxProjectionParameters(
        projectionScaleX,
        projectionScaleY,
        projectionDepthQ,
        projectionDepthQn);
    g_postFxProjectionCapturedFrame.store(
        GetPostFxFrameIndex(), std::memory_order_relaxed);
    return true;
}

static void ObservePostFxProjectionForGeometryDraw(IDirect3DDevice9* device)
{
    if (device == nullptr ||
        !g_postFxGBufferPairActive.load(std::memory_order_acquire))
    {
        return;
    }

    const unsigned long long frame = GetPostFxFrameIndex();
    if (g_postFxProjectionCapturedFrame.load(std::memory_order_relaxed) == frame)
        return;

    // Fallback for frames where c245-c248 were written before G-buffer
    // tracking became active. The hot path captures these constants directly
    // in HookSetVertexShaderConstantF and normally avoids this device readback.
    float columns[16] = {};
    if (FAILED(device->GetVertexShaderConstantF(245, columns, 4)))
        return;

    CapturePostFxProjectionColumns(columns);
}

static std::uint64_t HashPostFxShaderBytes(const void* data, size_t size)
{
    constexpr std::uint64_t kFnvOffsetBasis64 = 14695981039346656037ull;
    constexpr std::uint64_t kFnvPrime64 = 1099511628211ull;

    const auto* bytes = static_cast<const unsigned char*>(data);
    std::uint64_t hash = kFnvOffsetBasis64;
    for (size_t i = 0; i < size; ++i)
    {
        hash ^= bytes[i];
        hash *= kFnvPrime64;
    }
    return hash;
}

static void RegisterPostFxPixelShader(IDirect3DPixelShader9* shader)
{
    if (shader == nullptr)
        return;

    UINT size = 0;
    if (FAILED(shader->GetFunction(nullptr, &size)) || size == 0)
        return;

    std::vector<unsigned char> bytecode(size);
    UINT readSize = size;
    if (FAILED(shader->GetFunction(bytecode.data(), &readSize)) || readSize == 0)
        return;

    constexpr std::uint64_t kFinalCompositeShaderHash = 0x12F56FADBD80F13Bull;
    if (HashPostFxShaderBytes(bytecode.data(), readSize) == kFinalCompositeShaderHash)
    {
        g_postFxFinalCompositeShader.store(shader, std::memory_order_release);
        AppendLog("[PostFX] Identified the game's final-composite pixel shader.\n");
    }
}

static bool IsPostFxFinalCompositeShader(IDirect3DPixelShader9* shader)
{
    return shader != nullptr &&
           shader == g_postFxFinalCompositeShader.load(std::memory_order_acquire);
}

static HRESULT WINAPI HookDrawPrimitive(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT startVertex,
    UINT primitiveCount)
{
    if (!IsGameD3D9Device(self))
        return g_originalDrawPrimitive(self, primitiveType, startVertex, primitiveCount);

    const bool captureProjection =
        g_postFxGBufferPairActive.load(std::memory_order_relaxed);
    const bool finalCompositePending =
        g_postFxFinalCompositeBound.load(std::memory_order_acquire);
    if (!captureProjection && !finalCompositePending)
        return g_originalDrawPrimitive(self, primitiveType, startVertex, primitiveCount);

    void* returnAddress = _ReturnAddress();
    const bool gameCall = IsPostFxGameCall(returnAddress);
    if (!gameCall)
        return g_originalDrawPrimitive(self, primitiveType, startVertex, primitiveCount);

    if (captureProjection)
        ObservePostFxProjectionForGeometryDraw(self);

    const bool finalCompositeDraw = finalCompositePending &&
        g_postFxFinalCompositeBound.exchange(false, std::memory_order_acq_rel);
    if (!finalCompositeDraw)
        return g_originalDrawPrimitive(self, primitiveType, startVertex, primitiveCount);

    PostFxAoFinalCompositeState aoState{};
    const bool aoPrepared = finalCompositeDraw &&
        BeginPostFxAoFinalComposite(self, &aoState);

    PostFxExposureDrawState postFxExposureState{};
    const bool postFxExposurePrepared = finalCompositeDraw &&
        BeginPostFxExposureFinalComposite(
            self, aoPrepared ? &aoState : nullptr, &postFxExposureState);


    const HRESULT result = g_originalDrawPrimitive(
        self, primitiveType, startVertex, primitiveCount);


    if (postFxExposurePrepared)
        EndPostFxExposureFinalComposite(self, &postFxExposureState);

    if (aoPrepared && SUCCEEDED(result))
        EndPostFxAoFinalComposite(self, &aoState);
    else if (aoPrepared)
        EndPostFxAoFinalComposite(nullptr, &aoState);

    // The final-composite draw has consumed the replacement state. Clear the
    // logical flag here so ordinary shader binds can stay on the fast path.
    NotifyPostFxExposureReplacementBound(false);

    return result;
}

static HRESULT WINAPI HookDrawIndexedPrimitive(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalDrawIndexedPrimitive(
            self, primitiveType, baseVertexIndex, minVertexIndex, numVertices,
            startIndex, primitiveCount);
    }

    const bool captureProjection =
        g_postFxGBufferPairActive.load(std::memory_order_relaxed);
    const bool finalCompositePending =
        g_postFxFinalCompositeBound.load(std::memory_order_acquire);
    if (!captureProjection && !finalCompositePending)
    {
        return g_originalDrawIndexedPrimitive(
            self, primitiveType, baseVertexIndex, minVertexIndex, numVertices,
            startIndex, primitiveCount);
    }

    void* returnAddress = _ReturnAddress();
    const bool gameCall = IsPostFxGameCall(returnAddress);
    if (!gameCall)
    {
        return g_originalDrawIndexedPrimitive(
            self, primitiveType, baseVertexIndex, minVertexIndex, numVertices,
            startIndex, primitiveCount);
    }

    if (captureProjection)
        ObservePostFxProjectionForGeometryDraw(self);

    const bool finalCompositeDraw = finalCompositePending &&
        g_postFxFinalCompositeBound.exchange(false, std::memory_order_acq_rel);
    if (!finalCompositeDraw)
    {
        return g_originalDrawIndexedPrimitive(
            self, primitiveType, baseVertexIndex, minVertexIndex, numVertices,
            startIndex, primitiveCount);
    }

    PostFxAoFinalCompositeState aoState{};
    const bool aoPrepared = finalCompositeDraw &&
        BeginPostFxAoFinalComposite(self, &aoState);

    PostFxExposureDrawState postFxExposureState{};
    const bool postFxExposurePrepared = finalCompositeDraw &&
        BeginPostFxExposureFinalComposite(
            self, aoPrepared ? &aoState : nullptr, &postFxExposureState);


    const HRESULT result = g_originalDrawIndexedPrimitive(
        self, primitiveType, baseVertexIndex, minVertexIndex, numVertices,
        startIndex, primitiveCount);


    if (postFxExposurePrepared)
        EndPostFxExposureFinalComposite(self, &postFxExposureState);

    if (aoPrepared && SUCCEEDED(result))
        EndPostFxAoFinalComposite(self, &aoState);
    else if (aoPrepared)
        EndPostFxAoFinalComposite(nullptr, &aoState);

    // The final-composite draw has consumed the replacement state. Clear the
    // logical flag here so ordinary shader binds can stay on the fast path.
    NotifyPostFxExposureReplacementBound(false);

    return result;
}

static HRESULT WINAPI HookDrawPrimitiveUP(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT primitiveCount,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalDrawPrimitiveUP(
            self, primitiveType, primitiveCount, vertexStreamZeroData,
            vertexStreamZeroStride);
    }

    const bool captureProjection =
        g_postFxGBufferPairActive.load(std::memory_order_relaxed);
    const bool finalCompositePending =
        g_postFxFinalCompositeBound.load(std::memory_order_acquire);
    if (!captureProjection && !finalCompositePending)
    {
        return g_originalDrawPrimitiveUP(
            self, primitiveType, primitiveCount, vertexStreamZeroData,
            vertexStreamZeroStride);
    }

    void* returnAddress = _ReturnAddress();
    const bool gameCall = IsPostFxGameCall(returnAddress);
    if (!gameCall)
    {
        return g_originalDrawPrimitiveUP(
            self, primitiveType, primitiveCount, vertexStreamZeroData,
            vertexStreamZeroStride);
    }

    if (captureProjection)
        ObservePostFxProjectionForGeometryDraw(self);

    const bool finalCompositeDraw = finalCompositePending &&
        g_postFxFinalCompositeBound.exchange(false, std::memory_order_acq_rel);
    if (!finalCompositeDraw)
    {
        return g_originalDrawPrimitiveUP(
            self, primitiveType, primitiveCount, vertexStreamZeroData,
            vertexStreamZeroStride);
    }

    PostFxAoFinalCompositeState aoState{};
    const bool aoPrepared = finalCompositeDraw &&
        BeginPostFxAoFinalComposite(self, &aoState);

    PostFxExposureDrawState postFxExposureState{};
    const bool postFxExposurePrepared = finalCompositeDraw &&
        BeginPostFxExposureFinalComposite(
            self, aoPrepared ? &aoState : nullptr, &postFxExposureState);


    const HRESULT result = g_originalDrawPrimitiveUP(
        self, primitiveType, primitiveCount,
        vertexStreamZeroData, vertexStreamZeroStride);


    if (postFxExposurePrepared)
        EndPostFxExposureFinalComposite(self, &postFxExposureState);

    if (aoPrepared && SUCCEEDED(result))
        EndPostFxAoFinalComposite(self, &aoState);
    else if (aoPrepared)
        EndPostFxAoFinalComposite(nullptr, &aoState);

    // The final-composite draw has consumed the replacement state. Clear the
    // logical flag here so ordinary shader binds can stay on the fast path.
    NotifyPostFxExposureReplacementBound(false);

    return result;
}

static HRESULT WINAPI HookDrawIndexedPrimitiveUP(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT minVertexIndex,
    UINT numVertices,
    UINT primitiveCount,
    const void* indexData,
    D3DFORMAT indexDataFormat,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalDrawIndexedPrimitiveUP(
            self, primitiveType, minVertexIndex, numVertices, primitiveCount,
            indexData, indexDataFormat, vertexStreamZeroData, vertexStreamZeroStride);
    }

    const bool captureProjection =
        g_postFxGBufferPairActive.load(std::memory_order_relaxed);
    const bool finalCompositePending =
        g_postFxFinalCompositeBound.load(std::memory_order_acquire);
    if (!captureProjection && !finalCompositePending)
    {
        return g_originalDrawIndexedPrimitiveUP(
            self, primitiveType, minVertexIndex, numVertices, primitiveCount,
            indexData, indexDataFormat, vertexStreamZeroData, vertexStreamZeroStride);
    }

    void* returnAddress = _ReturnAddress();
    const bool gameCall = IsPostFxGameCall(returnAddress);
    if (!gameCall)
    {
        return g_originalDrawIndexedPrimitiveUP(
            self, primitiveType, minVertexIndex, numVertices, primitiveCount,
            indexData, indexDataFormat, vertexStreamZeroData, vertexStreamZeroStride);
    }

    if (captureProjection)
        ObservePostFxProjectionForGeometryDraw(self);

    const bool finalCompositeDraw = finalCompositePending &&
        g_postFxFinalCompositeBound.exchange(false, std::memory_order_acq_rel);
    if (!finalCompositeDraw)
    {
        return g_originalDrawIndexedPrimitiveUP(
            self, primitiveType, minVertexIndex, numVertices, primitiveCount,
            indexData, indexDataFormat, vertexStreamZeroData, vertexStreamZeroStride);
    }

    PostFxAoFinalCompositeState aoState{};
    const bool aoPrepared = finalCompositeDraw &&
        BeginPostFxAoFinalComposite(self, &aoState);

    PostFxExposureDrawState postFxExposureState{};
    const bool postFxExposurePrepared = finalCompositeDraw &&
        BeginPostFxExposureFinalComposite(
            self, aoPrepared ? &aoState : nullptr, &postFxExposureState);


    const HRESULT result = g_originalDrawIndexedPrimitiveUP(
        self, primitiveType, minVertexIndex, numVertices, primitiveCount,
        indexData, indexDataFormat, vertexStreamZeroData,
        vertexStreamZeroStride);


    if (postFxExposurePrepared)
        EndPostFxExposureFinalComposite(self, &postFxExposureState);

    if (aoPrepared && SUCCEEDED(result))
        EndPostFxAoFinalComposite(self, &aoState);
    else if (aoPrepared)
        EndPostFxAoFinalComposite(nullptr, &aoState);

    // The final-composite draw has consumed the replacement state. Clear the
    // logical flag here so ordinary shader binds can stay on the fast path.
    NotifyPostFxExposureReplacementBound(false);

    return result;
}

static HRESULT WINAPI HookCreatePixelShader(
    IDirect3DDevice9* self,
    const DWORD* function,
    IDirect3DPixelShader9** shader)
{
    const HRESULT result = g_originalCreatePixelShader(self, function, shader);
    if (IsGameD3D9Device(self) &&
        SUCCEEDED(result) && shader != nullptr && *shader != nullptr)
    {
        RegisterPostFxPixelShader(*shader);
    }
    return result;
}

static HRESULT WINAPI HookSetPixelShader(
    IDirect3DDevice9* self,
    IDirect3DPixelShader9* shader)
{
    if (!IsGameD3D9Device(self))
        return g_originalSetPixelShader(self, shader);

    const bool finalCompositeCandidate = IsPostFxFinalCompositeShader(shader);
    const bool finalCompositePending =
        g_postFxFinalCompositeBound.load(std::memory_order_relaxed);

    // Nearly every game shader bind is unrelated to ZachFix PostFX. Avoid the
    // return-address range test unless this is the identified final shader or
    // a previous final-composite bind is still waiting to be consumed.
    if (!finalCompositeCandidate && !finalCompositePending)
        return g_originalSetPixelShader(self, shader);

    const bool gameCall = IsPostFxGameCall(_ReturnAddress());
    if (!gameCall)
        return g_originalSetPixelShader(self, shader);

    const bool exposureReplacementRequired =
        finalCompositeCandidate && ShouldUsePostFxExposureReplacement();
    const bool processFinalComposite = finalCompositeCandidate &&
        (IsPostFxAoEnabled() || exposureReplacementRequired);

    IDirect3DPixelShader9* actualShader = shader;
    bool exposureReplacement = false;
    if (exposureReplacementRequired)
    {
        if (IDirect3DPixelShader9* replacement =
                GetPostFxExposureReplacementShader(self))
        {
            actualShader = replacement;
            exposureReplacement = true;
        }
    }

    const HRESULT result = g_originalSetPixelShader(self, actualShader);
    if (SUCCEEDED(result))
    {
        NotifyPostFxExposureReplacementBound(exposureReplacement);
        g_postFxFinalCompositeBound.store(
            processFinalComposite, std::memory_order_release);
    }
    return result;
}

static HRESULT WINAPI HookSetVertexShaderConstantF(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalSetVertexShaderConstantF(
            self, startRegister, constantData, vector4fCount);
    }

    if (constantData != nullptr &&
        g_postFxGBufferPairActive.load(std::memory_order_relaxed) &&
        startRegister <= 245)
    {
        const UINT projectionOffset = 245u - startRegister;
        if (vector4fCount >= projectionOffset + 4u &&
            g_postFxProjectionCapturedFrame.load(std::memory_order_relaxed) !=
                GetPostFxFrameIndex() &&
            IsPostFxGameCall(_ReturnAddress()))
        {
            CapturePostFxProjectionColumns(
                constantData + static_cast<size_t>(projectionOffset) * 4u);
        }
    }

    if (constantData == nullptr ||
        !g_config.fixPixelOffset)
    {
        return g_originalSetVertexShaderConstantF(
            self,
            startRegister,
            constantData,
            vector4fCount
        );
    }

    // Original DPFix heuristic:
    // VS c0 with z=w=0 is treated as the D3D9 half-pixel size constant.
    if (startRegister == 0 && vector4fCount == 1)
    {
        const bool looksLikePixelSize =
            NearlyEqualFloat(constantData[2], 0.0f) &&
            NearlyEqualFloat(constantData[3], 0.0f);

        g_lastVertexC0WasPixelSize.store(
            looksLikePixelSize,
            std::memory_order_relaxed
        );

        if (looksLikePixelSize)
        {
            UINT width = g_currentViewportWidth.load(
                std::memory_order_relaxed
            );

            UINT height = g_currentViewportHeight.load(
                std::memory_order_relaxed
            );

            if (width == 0)
                width = kBaseRenderWidth;

            if (height == 0)
                height = kBaseRenderHeight;

            const float replacement[4] =
            {
                0.5f / static_cast<float>(width),
                0.5f / static_cast<float>(height),
                0.0f,
                0.0f
            };

            static std::atomic_bool loggedVsCorrection{ false };
            bool expected = false;

            if (loggedVsCorrection.compare_exchange_strong(
                    expected,
                    true,
                    std::memory_order_relaxed))
            {
                char text[512] = {};

                sprintf_s(
                    text,
                    "[PixelOffset] VS c0 corrected for viewport %u x %u: "
                    "{%.9f, %.9f, %.9f, %.9f} -> "
                    "{%.9f, %.9f, 0, 0}\n",
                    width,
                    height,
                    constantData[0],
                    constantData[1],
                    constantData[2],
                    constantData[3],
                    replacement[0],
                    replacement[1]
                );

                AppendLog(text);
            }

            return g_originalSetVertexShaderConstantF(
                self,
                startRegister,
                replacement,
                vector4fCount
            );
        }
    }

    return g_originalSetVertexShaderConstantF(
        self,
        startRegister,
        constantData,
        vector4fCount
    );
}


static bool IsDpfixDualViewTexture(
    IDirect3DBaseTexture9* texture)
{
    if (texture == nullptr)
        return false;

    // Avoid QueryInterface(IID_IDirect3DTexture9) here. Referencing the
    // exported IID would require an additional d3d9 GUID library dependency,
    // while IDirect3DBaseTexture9 already exposes the concrete resource type.
    if (texture->GetType() != D3DRTYPE_TEXTURE)
        return false;

    IDirect3DTexture9* texture2d =
        static_cast<IDirect3DTexture9*>(texture);

    D3DSURFACE_DESC desc = {};
    return
        SUCCEEDED(texture2d->GetLevelDesc(0, &desc)) &&
        desc.Width == 1024 &&
        desc.Height == 1024 &&
        (desc.Usage & D3DUSAGE_RENDERTARGET) == 0;
}


static HRESULT WINAPI HookSetStreamSource(
    IDirect3DDevice9* self,
    UINT streamNumber,
    IDirect3DVertexBuffer9* streamData,
    UINT offsetInBytes,
    UINT stride)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalSetStreamSource(
            self, streamNumber, streamData, offsetInBytes, stride);
    }

    // Adapted from original DPFix RenderstateManager::redirectSetStreamSource
    // by Peter "Durante" Thoman.
    IDirect3DSurface9* current =
        g_currentRenderTarget0.load(std::memory_order_acquire);

    IDirect3DSurface9* backBuffer =
        g_backBuffer0.load(std::memory_order_acquire);

    const bool isOnBackbuffer =
        current != nullptr &&
        current == backBuffer;

    const bool isOffscreen =
        current != nullptr &&
        current != backBuffer;

    const bool isEnemyTrailStream =
        stride == 24 &&
        (offsetInBytes == 96 || offsetInBytes == 192);

    const bool isFirstStreamSource =
        g_firstStreamSourceAfterRenderTarget.exchange(
            false,
            std::memory_order_acq_rel
        );

    // Enemy shadow / afterimage trail compatibility path.
    if (isOffscreen &&
        isEnemyTrailStream &&
        g_originalSetVertexShaderConstantF != nullptr)
    {
        const float trailConstant[4] =
        {
            640.0f,
            360.0f,
            640.0f,
            360.0f
        };

        g_originalSetVertexShaderConstantF(
            self,
            254,
            trailConstant,
            1
        );

        static std::atomic_bool loggedEnemyTrailFix{ false };
        bool expected = false;

        if (loggedEnemyTrailFix.compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            AppendLog(
                "[Compatibility] Original DPFix enemy shadow-trail correction activated.\n"
            );
        }
    }

    // Original DPFix dual-view correction. The signature is intentionally
    // narrow: backbuffer, first stream after an RT change, offset 192 /
    // stride 24, with the known 1024x1024 non-RT texture bound at stage 0.
    const bool isDualViewStream =
        offsetInBytes == 192 &&
        stride == 24;

    if (isOnBackbuffer &&
        isFirstStreamSource &&
        isDualViewStream &&
        g_lastTextureWasDualViewCandidate.load(std::memory_order_acquire) &&
        g_originalSetVertexShaderConstantF != nullptr)
    {
        IDirect3DBaseTexture9* boundTexture = nullptr;
        const HRESULT textureResult =
            self->GetTexture(0, &boundTexture);

        const bool confirmed =
            SUCCEEDED(textureResult) &&
            IsDpfixDualViewTexture(boundTexture);

        if (boundTexture != nullptr)
            boundTexture->Release();

        if (confirmed)
        {
            const float dualViewConstant[4] =
            {
                640.0f,
                360.0f,
                640.0f,
                360.0f
            };

            g_originalSetVertexShaderConstantF(
                self,
                254,
                dualViewConstant,
                1
            );

            static std::atomic_bool loggedDualViewFix{ false };
            bool expected = false;

            if (loggedDualViewFix.compare_exchange_strong(
                    expected,
                    true,
                    std::memory_order_relaxed))
            {
                AppendLog(
                    "[Compatibility] Original DPFix dual-view correction activated.\n"
                );
            }
        }
    }

    return g_originalSetStreamSource(
        self,
        streamNumber,
        streamData,
        offsetInBytes,
        stride
    );
}


static HRESULT WINAPI HookSetViewport(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport)
{
    if (!IsGameD3D9Device(self))
        return g_originalSetViewport(self, viewport);

    if (viewport == nullptr)
        return g_originalSetViewport(self, viewport);

    //
    // Deadly Premonition uses several hard-coded 1280x720 viewports for maps.
    // These are UI/presentation viewports, so they scale to Display resolution,
    // not Internal rendering resolution.
    //
    const bool isMiniMap =
        viewport->X == 76 &&
        viewport->Y == 368 &&
        viewport->Width == 232 &&
        viewport->Height == 200;

    const bool isMenuMap =
        viewport->X == 252 &&
        viewport->Y == 60 &&
        viewport->Width == 776 &&
        viewport->Height == 520;

    const bool isLargeMap =
        viewport->X == 64 &&
        viewport->Y == 164 &&
        viewport->Width == 512 &&
        viewport->Height == 512;

    if (isMiniMap || isMenuMap || isLargeMap)
    {
        D3DVIEWPORT9 modified = *viewport;

        modified.X = ScaleCoordinate(
            viewport->X,
            g_displayWidth,
            kBaseRenderWidth
        );

        modified.Y = ScaleCoordinate(
            viewport->Y,
            g_displayHeight,
            kBaseRenderHeight
        );

        modified.Width = ScaleCoordinate(
            viewport->Width,
            g_displayWidth,
            kBaseRenderWidth
        );

        modified.Height = ScaleCoordinate(
            viewport->Height,
            g_displayHeight,
            kBaseRenderHeight
        );

        static std::atomic_bool loggedMiniMap{ false };
        static std::atomic_bool loggedMenuMap{ false };
        static std::atomic_bool loggedLargeMap{ false };

        std::atomic_bool* logFlag = nullptr;
        const char* name = nullptr;

        if (isMiniMap)
        {
            logFlag = &loggedMiniMap;
            name = "Minimap";
        }
        else if (isMenuMap)
        {
            logFlag = &loggedMenuMap;
            name = "Menu map";
        }
        else
        {
            logFlag = &loggedLargeMap;
            name = "Large map";
        }

        bool expected = false;

        if (logFlag->compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[Resolution] %s viewport "
                "%u,%u %ux%u -> %u,%u %ux%u\n",
                name,
                viewport->X,
                viewport->Y,
                viewport->Width,
                viewport->Height,
                modified.X,
                modified.Y,
                modified.Width,
                modified.Height
            );

            AppendLog(text);
        }

        return SubmitViewport(
            self,
            &modified
        );
    }

    //
    // Known shadow-map viewports. Keep this intentionally narrow:
    // only the 512x512 and 1024x1024 shadow passes documented by DPFix.
    //
    if (g_config.shadowScale > 1 &&
        viewport->X == 0 &&
        viewport->Y == 0 &&
        IsKnownShadowMapSize(viewport->Width, viewport->Height))
    {
        D3DVIEWPORT9 modified = *viewport;

        modified.Width = ScaleShadowDimension(viewport->Width);
        modified.Height = ScaleShadowDimension(viewport->Height);

        static std::atomic_bool loggedShadow512{ false };
        static std::atomic_bool loggedShadow1024{ false };

        std::atomic_bool& flag =
            viewport->Width == 512
                ? loggedShadow512
                : loggedShadow1024;

        bool expected = false;

        if (flag.compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[Shadows] Viewport %u x %u -> %u x %u\n",
                viewport->Width,
                viewport->Height,
                modified.Width,
                modified.Height
            );

            AppendLog(text);
        }

        return SubmitViewport(
            self,
            &modified
        );
    }

    //
    // Reflection viewports documented by the original DPFix.
    //
    if (g_config.reflectionScale > 1 &&
        IsKnownReflectionSize(viewport->Width, viewport->Height))
    {
        D3DVIEWPORT9 modified = *viewport;

        modified.Width =
            ScaleReflectionDimension(viewport->Width);

        modified.Height =
            ScaleReflectionDimension(viewport->Height);

        static std::atomic_bool loggedReflection640{ false };
        static std::atomic_bool loggedReflection320{ false };

        std::atomic_bool& flag =
            viewport->Width == 640
                ? loggedReflection640
                : loggedReflection320;

        bool expected = false;

        if (flag.compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[Reflections] Viewport %u x %u -> %u x %u\n",
                viewport->Width,
                viewport->Height,
                modified.Width,
                modified.Height
            );

            AppendLog(text);
        }

        return SubmitViewport(
            self,
            &modified
        );
    }

    //
    // Original DPFix DoF resolution override.
    // 448x252 is exactly 35% of the game's 1280x720 base render size.
    //
    if (g_config.improveDofResolution &&
        viewport->Width == 448 &&
        viewport->Height == 252)
    {
        D3DVIEWPORT9 modified = *viewport;

        modified.Width = ScaleFromBaseWidth(viewport->Width);
        modified.Height = ScaleFromBaseHeight(viewport->Height);

        static std::atomic_bool loggedDofViewport{ false };

        bool expected = false;

        if (loggedDofViewport.compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[DoF] Viewport %u x %u -> %u x %u\n",
                viewport->Width,
                viewport->Height,
                modified.Width,
                modified.Height
            );

            AppendLog(text);
        }

        return SubmitViewport(
            self,
            &modified
        );
    }

    //
    // The game's ordinary full-scene viewport is always expressed as 1280x720.
    //
    if (viewport->Width != kBaseRenderWidth ||
        viewport->Height != kBaseRenderHeight)
    {
        return SubmitViewport(
            self,
            viewport
        );
    }

    IDirect3DSurface9* current =
        g_currentRenderTarget0.load(
            std::memory_order_acquire
        );

    IDirect3DSurface9* backBuffer =
        g_backBuffer0.load(
            std::memory_order_acquire
        );

    const bool isMainRenderSurface =
        IsRegisteredMainRenderSurface(current);

    const bool isBackBuffer =
        current != nullptr &&
        current == backBuffer;

    if (!isMainRenderSurface && !isBackBuffer)
    {
        return SubmitViewport(
            self,
            viewport
        );
    }

    D3DVIEWPORT9 modified = *viewport;

    if (isBackBuffer)
    {
        modified.Width = g_displayWidth;
        modified.Height = g_displayHeight;

        static std::atomic_bool loggedBackBufferViewport{ false };

        bool expected = false;

        if (loggedBackBufferViewport.compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[Resolution] Backbuffer viewport "
                "%u x %u -> %u x %u\n",
                kBaseRenderWidth,
                kBaseRenderHeight,
                g_displayWidth,
                g_displayHeight
            );

            AppendLog(text);
        }
    }
    else
    {
        modified.Width = g_internalWidth;
        modified.Height = g_internalHeight;

        bool expected = false;

        if (g_loggedViewportOverride.compare_exchange_strong(
                expected,
                true,
                std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[Resolution] Main viewport "
                "%u x %u -> %u x %u\n",
                kBaseRenderWidth,
                kBaseRenderHeight,
                g_internalWidth,
                g_internalHeight
            );

            AppendLog(text);
        }
    }

    return SubmitViewport(
        self,
        &modified
    );
}


static HRESULT WINAPI HookSetPixelShaderConstantF(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalSetPixelShaderConstantF(
            self, startRegister, constantData, vector4fCount);
    }

    if (constantData == nullptr)
    {
        return g_originalSetPixelShaderConstantF(
            self,
            startRegister,
            constantData,
            vector4fCount
        );
    }

    if (g_config.fixPixelOffset)
    {
        // Original DPFix PS c17 half-pixel correction.
        if (startRegister == 17 &&
            vector4fCount == 1 &&
            NearlyEqualFloat(
                constantData[0],
                0.5f / static_cast<float>(kBaseRenderWidth)) &&
            NearlyEqualFloat(
                constantData[1],
                0.5f / static_cast<float>(kBaseRenderHeight)) &&
            NearlyEqualFloat(constantData[2], 0.0f))
        {
            const float replacement[4] =
            {
                0.5f / static_cast<float>(g_internalWidth),
                0.5f / static_cast<float>(g_internalHeight),
                0.0f,
                0.0f
            };

            static std::atomic_bool loggedPs17{ false };
            bool expected = false;

            if (loggedPs17.compare_exchange_strong(
                    expected,
                    true,
                    std::memory_order_relaxed))
            {
                char text[384] = {};

                sprintf_s(
                    text,
                    "[PixelOffset] PS c17 corrected: "
                    "1280 x 720 -> %u x %u "
                    "({%.9f, %.9f} -> {%.9f, %.9f})\n",
                    g_internalWidth,
                    g_internalHeight,
                    constantData[0],
                    constantData[1],
                    replacement[0],
                    replacement[1]
                );

                AppendLog(text);
            }

            return g_originalSetPixelShaderConstantF(
                self,
                startRegister,
                replacement,
                vector4fCount
            );
        }

        // Original DPFix PS c4..c8 texel-size correction.
        // constantData[4] is 1/1280 and constantData[17] is 1/720.
        if (startRegister == 4 &&
            vector4fCount == 5 &&
            NearlyEqualFloat(
                constantData[4],
                1.0f / static_cast<float>(kBaseRenderWidth)) &&
            NearlyEqualFloat(
                constantData[17],
                1.0f / static_cast<float>(kBaseRenderHeight)))
        {
            float replacement[20] = {};
            std::memcpy(
                replacement,
                constantData,
                sizeof(replacement)
            );

            replacement[4] =
                1.0f / static_cast<float>(g_internalWidth);

            replacement[8] =
                2.0f / static_cast<float>(g_internalWidth);

            replacement[17] =
                1.0f / static_cast<float>(g_internalHeight);

            static std::atomic_bool loggedPs4{ false };
            bool expected = false;

            if (loggedPs4.compare_exchange_strong(
                    expected,
                    true,
                    std::memory_order_relaxed))
            {
                char text[384] = {};

                sprintf_s(
                    text,
                    "[PixelOffset] PS c4..c8 texel size corrected: "
                    "1280 x 720 -> %u x %u\n",
                    g_internalWidth,
                    g_internalHeight
                );

                AppendLog(text);
            }

            return g_originalSetPixelShaderConstantF(
                self,
                startRegister,
                replacement,
                vector4fCount
            );
        }
    }

    // Existing shadow-map-size correction.
    if (g_config.shadowScale <= 1 ||
        startRegister != 0 ||
        vector4fCount != 1)
    {
        return g_originalSetPixelShaderConstantF(
            self,
            startRegister,
            constantData,
            vector4fCount
        );
    }

    const bool trailingZeros =
        constantData[1] == 0.0f &&
        constantData[2] == 0.0f &&
        constantData[3] == 0.0f;

    const bool knownShadowSize =
        constantData[0] == 512.0f ||
        constantData[0] == 1024.0f;

    if (!trailingZeros || !knownShadowSize)
    {
        return g_originalSetPixelShaderConstantF(
            self,
            startRegister,
            constantData,
            vector4fCount
        );
    }

    float replacement[4] =
    {
        constantData[0] * static_cast<float>(g_config.shadowScale),
        0.0f,
        0.0f,
        0.0f
    };

    static std::atomic_bool loggedShader512{ false };
    static std::atomic_bool loggedShader1024{ false };

    std::atomic_bool& flag =
        constantData[0] == 512.0f
            ? loggedShader512
            : loggedShader1024;

    bool expected = false;

    if (flag.compare_exchange_strong(
            expected,
            true,
            std::memory_order_relaxed))
    {
        char text[256] = {};

        sprintf_s(
            text,
            "[Shadows] Pixel shader size constant %.0f -> %.0f "
            "(c0 = {%.0f, %.0f, %.0f, %.0f})\n",
            constantData[0],
            replacement[0],
            replacement[0],
            replacement[1],
            replacement[2],
            replacement[3]
        );

        AppendLog(text);
    }

    return g_originalSetPixelShaderConstantF(
        self,
        startRegister,
        replacement,
        vector4fCount
    );
}


static thread_local unsigned g_presentHookDepth = 0;
static std::atomic_bool g_loggedDevicePresentPath{ false };
static std::atomic_bool g_loggedSwapChainPresentPath{ false };
static std::atomic_bool g_loggedEndSceneUiPath{ false };
static std::atomic_bool g_endSceneUiPathActive{ false };


static bool NormalizeExclusiveFullscreenPresentation(
    D3DPRESENT_PARAMETERS* presentationParameters)
{
    if (presentationParameters == nullptr || presentationParameters->Windowed)
        return false;

    const bool resolutionChanged =
        presentationParameters->BackBufferWidth != g_displayWidth ||
        presentationParameters->BackBufferHeight != g_displayHeight;

    presentationParameters->BackBufferWidth = g_displayWidth;
    presentationParameters->BackBufferHeight = g_displayHeight;

    // A refresh rate from another fullscreen resolution is not portable to
    // the resolved ZachFix display mode. Let D3D9 select the default rate.
    if (resolutionChanged)
        presentationParameters->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    return resolutionChanged;
}


static void LogActivePresentation(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    IDirect3DSwapChain9* swapChain = nullptr;
    const HRESULT swapChainResult = device->GetSwapChain(0, &swapChain);

    if (FAILED(swapChainResult) || swapChain == nullptr)
    {
        char text[160] = {};
        sprintf_s(
            text,
            "[Display] WARNING: GetSwapChain(0) failed: HRESULT=0x%08X.\n",
            static_cast<unsigned>(swapChainResult)
        );
        AppendLog(text);
        return;
    }

    bool havePresentation = false;
    bool windowed = false;

    D3DPRESENT_PARAMETERS actual = {};
    const HRESULT presentResult = swapChain->GetPresentParameters(&actual);

    if (SUCCEEDED(presentResult))
    {
        havePresentation = true;
        windowed = actual.Windowed != FALSE;

        char text[320] = {};
        sprintf_s(
            text,
            "[Display] Active presentation: %s, BackBuffer=%u x %u, Format=%u.\n",
            windowed ? "Windowed" : "Fullscreen",
            actual.BackBufferWidth,
            actual.BackBufferHeight,
            static_cast<unsigned>(actual.BackBufferFormat)
        );
        AppendLog(text);
    }
    else
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Display] WARNING: GetPresentParameters failed: HRESULT=0x%08X.\n",
            static_cast<unsigned>(presentResult)
        );
        AppendLog(text);
    }

    D3DDISPLAYMODE displayMode = {};
    const HRESULT displayModeResult = swapChain->GetDisplayMode(&displayMode);

    if (SUCCEEDED(displayModeResult))
    {
        char text[256] = {};
        sprintf_s(
            text,
            havePresentation && windowed
                ? "[Display] Desktop mode: %u x %u @ %u Hz, Format=%u.\n"
                : "[Display] Active display mode: %u x %u @ %u Hz, Format=%u.\n",
            displayMode.Width,
            displayMode.Height,
            displayMode.RefreshRate,
            static_cast<unsigned>(displayMode.Format)
        );
        AppendLog(text);
    }
    else
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[Display] WARNING: GetDisplayMode failed: HRESULT=0x%08X.\n",
            static_cast<unsigned>(displayModeResult)
        );
        AppendLog(text);
    }

    swapChain->Release();
}


static HRESULT WINAPI HookReset(
    IDirect3DDevice9* self,
    D3DPRESENT_PARAMETERS* presentationParameters)
{
    if (!IsGameD3D9Device(self))
        return g_originalReset(self, presentationParameters);

    if (presentationParameters != nullptr)
    {
        char text[384] = {};
        sprintf_s(
            text,
            "[Display] Reset request: %s, BackBuffer=%u x %u, Format=%u, "
            "RefreshRate=%u.\n",
            presentationParameters->Windowed ? "Windowed" : "Fullscreen",
            presentationParameters->BackBufferWidth,
            presentationParameters->BackBufferHeight,
            static_cast<unsigned>(presentationParameters->BackBufferFormat),
            presentationParameters->FullScreen_RefreshRateInHz
        );
        AppendLog(text);

        if (NormalizeExclusiveFullscreenPresentation(presentationParameters))
        {
            sprintf_s(
                text,
                "[Display] Reset normalized: Fullscreen, BackBuffer=%u x %u, "
                "Format=%u, RefreshRate=%u.\n",
                presentationParameters->BackBufferWidth,
                presentationParameters->BackBufferHeight,
                static_cast<unsigned>(presentationParameters->BackBufferFormat),
                presentationParameters->FullScreen_RefreshRateInHz
            );
            AppendLog(text);
        }
    }

    // All ZachFix-owned D3DPOOL_DEFAULT resources must be released before
    // Reset. Dear ImGui owns dynamic DX9 buffers, while PostFX keeps
    // AddRef'd references to the game's G-buffer render-target textures.
    InvalidateSettingsUiDeviceObjects();
    g_postFxGBufferTrackingEnabled.store(false, std::memory_order_release);
    const bool releasedPostFxGBufferRefs =
        ReleasePostFxGBufferCaptureRefs();
    if (releasedPostFxGBufferRefs)
        AppendLog("[Display] Released PostFX G-buffer refs before Reset.\n");

    // ZachFix PostFX targets live in D3DPOOL_DEFAULT and must not survive a
    // device Reset. Release them before the game resets the device; future
    // passes recreate only the slots they actually need.
    ReleasePostFxExposureResources();
    ReleasePostFxDofResources();
    ReleasePostFxBloomResources();
    ReleasePostFxAoResources();
    ReleasePostFxResources();
    ResetAdditionalDofBlurForDeviceReset();
    g_postFxFinalCompositeBound.store(false, std::memory_order_release);
    g_postFxProjectionCapturedFrame.store(~0ull, std::memory_order_relaxed);

    // Hot Apply replacement targets also live in D3DPOOL_DEFAULT. Retire all
    // ZachFix-owned replacements and forget the old game's raw resource
    // identities before Reset. The creation hooks repopulate a fresh registry
    // generation as the game rebuilds its resources.
    ResetRuntimeResourcesForDeviceReset();

    // Sampler tracking contains non-owning texture identities and cached device
    // state from the current generation. Do not carry either across Reset.
    ResetTextureFilteringStateForDeviceReset();

    // Reset invalidates the implicit backbuffer and the game's D3DPOOL_DEFAULT
    // render targets. Drop all raw surface identities before crossing the Reset
    // boundary; the game will rediscover its new main surfaces through the
    // creation hooks after a successful reset.
    ResetRenderTrackingForDeviceReset();

    const HRESULT result = g_originalReset(self, presentationParameters);

    if (FAILED(result))
    {
        char text[128] = {};
        sprintf_s(
            text,
            "[Display] Reset failed: HRESULT=0x%08X.\n",
            static_cast<unsigned>(result)
        );
        AppendLog(text);
    }
    else
    {
        AppendLog("[Display] Reset succeeded.\n");
        LogActivePresentation(self);
        LogBackBufferInfo(self);

        // ResetTextureFilteringStateForDeviceReset() deliberately invalidates
        // all cached sampler/device state before Reset. Recreate that state only
        // for a successful new device-resource generation.
        InitializeTextureFiltering(self);
    }

    NotifySettingsUiResetResult(result);
    NotifyPostFxResetResult(self, result);
    return result;
}


static thread_local bool g_zachFixPresentOwnedSceneActive = false;

static HRESULT WINAPI HookEndScene(IDirect3DDevice9* self)
{
    if (!IsGameD3D9Device(self) || g_zachFixPresentOwnedSceneActive)
        return g_originalEndScene(self);

    g_endSceneUiPathActive.store(true, std::memory_order_release);

    bool expected = false;
    if (g_loggedEndSceneUiPath.compare_exchange_strong(
            expected, true, std::memory_order_relaxed))
    {
        AppendLog("[UI] EndScene UI path active.\n");
    }

    // Xbox display gamma is a presentation/output transform. When enabled,
    // defer ZachFix's own UI until Present so the gamma pass can process the
    // complete game frame (including DP's HUD/menu) while the F10 UI remains
    // outside that emulated display transfer.
    if (!ShouldUsePostFxDisplayGamma())
        RenderSettingsUiInScene(self);
    return g_originalEndScene(self);
}


static HRESULT WINAPI HookPresent(
    IDirect3DDevice9* self,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalPresent(
            self, sourceRect, destRect, destWindowOverride, dirtyRegion);
    }

    const bool outermostPresent = g_presentHookDepth++ == 0;
    if (outermostPresent)
    {
        AdvancePostFxFrame();
        PollVanillaZeroDeltaNaNFixLog();

        bool expected = false;
        if (g_loggedDevicePresentPath.compare_exchange_strong(
                expected, true, std::memory_order_relaxed))
        {
            AppendLog("[UI] Device Present path active.\n");
        }

        // Gamma and the fallback/F10 UI open ZachFix-owned scenes. Suppress
        // EndScene-hook UI recursion for those scenes, including the frame in
        // which the user hot-switches Display Gamma back to PC.
        g_zachFixPresentOwnedSceneActive = true;

        const bool displayGammaEnabled = ShouldUsePostFxDisplayGamma();
        bool displayGammaApplied = false;
        if (displayGammaEnabled)
        {
            IDirect3DSurface9* backBuffer = nullptr;
            if (SUCCEEDED(self->GetBackBuffer(
                    0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)) &&
                backBuffer != nullptr)
            {
                displayGammaApplied = ApplyPostFxDisplayGamma(self, backBuffer);
                backBuffer->Release();
            }
        }
        NotifyPostFxDisplayGammaPresentComplete(displayGammaApplied);

        // Comparison screenshots intentionally capture the final game frame
        // after the optional Xbox HDTV transfer but before ZachFix draws its
        // Present-time settings UI.
        ProcessScreenshotPresetFrame(self);

        if (displayGammaEnabled ||
            !g_endSceneUiPathActive.load(std::memory_order_acquire))
        {
            RenderSettingsUi(self);
        }

        g_zachFixPresentOwnedSceneActive = false;
    }

    const HRESULT result = g_originalPresent(
        self,
        sourceRect,
        destRect,
        destWindowOverride,
        dirtyRegion
    );

    --g_presentHookDepth;
    return result;
}


static HRESULT WINAPI HookSwapChainPresent(
    IDirect3DSwapChain9* self,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion,
    DWORD flags)
{
    // Swap-chain detours are shared too. Resolve their owning device once and
    // use it both as the scope check and, on the game path, for UI rendering.
    IDirect3DDevice9* device = nullptr;
    const HRESULT getDeviceResult = self->GetDevice(&device);
    if (FAILED(getDeviceResult) ||
        device == nullptr ||
        !IsGameD3D9Device(device))
    {
        if (device != nullptr)
            device->Release();

        return g_originalSwapChainPresent(
            self,
            sourceRect,
            destRect,
            destWindowOverride,
            dirtyRegion,
            flags);
    }

    const bool outermostPresent = g_presentHookDepth++ == 0;
    if (outermostPresent)
    {
        AdvancePostFxFrame();
        PollVanillaZeroDeltaNaNFixLog();

        bool expected = false;
        if (g_loggedSwapChainPresentPath.compare_exchange_strong(
                expected, true, std::memory_order_relaxed))
        {
            AppendLog("[UI] SwapChain Present path active.\n");
        }

        g_zachFixPresentOwnedSceneActive = true;

        const bool displayGammaEnabled = ShouldUsePostFxDisplayGamma();
        bool displayGammaApplied = false;
        if (displayGammaEnabled)
        {
            IDirect3DSurface9* backBuffer = nullptr;
            if (SUCCEEDED(self->GetBackBuffer(
                    0, D3DBACKBUFFER_TYPE_MONO, &backBuffer)) &&
                backBuffer != nullptr)
            {
                displayGammaApplied = ApplyPostFxDisplayGamma(device, backBuffer);
                backBuffer->Release();
            }
        }
        NotifyPostFxDisplayGammaPresentComplete(displayGammaApplied);

        ProcessScreenshotPresetFrame(device);

        if (displayGammaEnabled ||
            !g_endSceneUiPathActive.load(std::memory_order_acquire))
        {
            RenderSettingsUi(device);
        }

        g_zachFixPresentOwnedSceneActive = false;
    }

    device->Release();

    const HRESULT result = g_originalSwapChainPresent(
        self,
        sourceRect,
        destRect,
        destWindowOverride,
        dirtyRegion,
        flags
    );

    --g_presentHookDepth;
    return result;
}


static HRESULT WINAPI HookStretchRect(
    IDirect3DDevice9* self,
    IDirect3DSurface9* sourceSurface,
    const RECT* sourceRect,
    IDirect3DSurface9* destSurface,
    const RECT* destRect,
    D3DTEXTUREFILTERTYPE filter)
{
    if (!IsGameD3D9Device(self))
    {
        return g_originalStretchRect(
            self, sourceSurface, sourceRect, destSurface, destRect, filter);
    }

    RuntimeSurfaceBinding sourceBinding =
        AcquireRuntimeSurfaceBinding(sourceSurface);
    RuntimeSurfaceBinding destBinding =
        AcquireRuntimeSurfaceBinding(destSurface);

    IDirect3DSurface9* effectiveSource = sourceBinding.replacement != nullptr
        ? sourceBinding.replacement
        : sourceBinding.logical;
    IDirect3DSurface9* effectiveDest = destBinding.replacement != nullptr
        ? destBinding.replacement
        : destBinding.logical;

    const HRESULT result = g_originalStretchRect(
        self,
        effectiveSource,
        sourceRect,
        effectiveDest,
        destRect,
        filter
    );

    if (sourceBinding.replacement != nullptr)
        sourceBinding.replacement->Release();
    if (destBinding.replacement != nullptr)
        destBinding.replacement->Release();

    return result;
}


static HRESULT WINAPI HookSetDepthStencilSurface(
    IDirect3DDevice9* self,
    IDirect3DSurface9* newDepthStencil)
{
    if (!IsGameD3D9Device(self))
        return g_originalSetDepthStencilSurface(self, newDepthStencil);

    RuntimeSurfaceBinding binding =
        AcquireRuntimeSurfaceBinding(newDepthStencil);
    IDirect3DSurface9* effectiveDepth = binding.replacement != nullptr
        ? binding.replacement
        : binding.logical;

    const HRESULT result =
        g_originalSetDepthStencilSurface(
            self,
            effectiveDepth
        );

    if (binding.replacement != nullptr)
        binding.replacement->Release();

    return result;
}


static HRESULT WINAPI HookSetTexture(
    IDirect3DDevice9* self,
    DWORD stage,
    IDirect3DBaseTexture9* texture)
{
    if (!IsGameD3D9Device(self))
        return g_originalSetTexture(self, stage, texture);

    RuntimeTextureBinding runtimeBinding =
        AcquireRuntimeTextureBinding(texture);
    IDirect3DBaseTexture9* logicalTexture = runtimeBinding.logical;

    // Mirror original DPFix dual-view detection. A null bind leaves the marker
    // untouched; every non-null texture bind replaces the previous candidate.
    if (logicalTexture != nullptr)
    {
        IDirect3DSurface9* current =
            g_currentRenderTarget0.load(std::memory_order_acquire);
        IDirect3DSurface9* backBuffer =
            g_backBuffer0.load(std::memory_order_acquire);

        const bool isDualViewCandidate =
            stage == 0 &&
            current != nullptr &&
            current == backBuffer &&
            IsDpfixDualViewTexture(logicalTexture);

        g_lastTextureWasDualViewCandidate.store(
            isDualViewCandidate,
            std::memory_order_release
        );
    }

    // Asset hot reload and render-target Hot Apply intentionally share this
    // already-existing bind hook, but keep separate lifetime managers. A
    // normal D3DX asset cannot also be one of our scalable render targets.
    IDirect3DTexture9* glyphTexture =
        stage == 0 && g_config.dynamicGlyphAtlas
            ? AcquireDynamicGlyphAtlasReplacement(self, logicalTexture)
            : nullptr;
    IDirect3DTexture9* hotTexture =
        glyphTexture == nullptr && IsTextureDeveloperModeActive()
            ? AcquireTextureOverrideHotReplacement(self, logicalTexture)
            : nullptr;
    IDirect3DTexture9* runtimeTexture =
        glyphTexture == nullptr && hotTexture == nullptr
            ? runtimeBinding.replacement
            : nullptr;

    IDirect3DBaseTexture9* effectiveTexture = logicalTexture;
    if (glyphTexture != nullptr)
        effectiveTexture = static_cast<IDirect3DBaseTexture9*>(glyphTexture);
    else if (hotTexture != nullptr)
        effectiveTexture = static_cast<IDirect3DBaseTexture9*>(hotTexture);
    else if (runtimeTexture != nullptr)
        effectiveTexture = static_cast<IDirect3DBaseTexture9*>(runtimeTexture);

    const HRESULT result = g_originalSetTexture(
        self,
        stage,
        effectiveTexture
    );

    if (SUCCEEDED(result) &&
        g_textureFilteringAppliedMode != TextureFilteringMode::Original &&
        IsTextureFilteringGameCall(_ReturnAddress()))
    {
        NotifyTextureFilteringTextureBound(
            self,
            stage,
            effectiveTexture);
    }

    if (glyphTexture != nullptr)
        glyphTexture->Release();
    if (hotTexture != nullptr)
        hotTexture->Release();
    if (runtimeBinding.replacement != nullptr)
        runtimeBinding.replacement->Release();

    return result;
}


static bool InstallSwapChainPresentHook(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    IDirect3DSwapChain9* swapChain = nullptr;
    const HRESULT getResult = device->GetSwapChain(0, &swapChain);
    if (FAILED(getResult) || swapChain == nullptr)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[UI] WARNING: GetSwapChain(0) failed; SwapChain Present fallback unavailable (HRESULT=0x%08X).\n",
            static_cast<unsigned>(getResult));
        AppendLog(text);
        return false;
    }

    void** swapChainVtable = *reinterpret_cast<void***>(swapChain);
    void* target = swapChainVtable[3];

    MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookSwapChainPresent),
        reinterpret_cast<void**>(&g_originalSwapChainPresent));

    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[UI] WARNING: MH_CreateHook failed for SwapChain::Present (%d); Device::Present remains available.\n",
            static_cast<int>(status));
        AppendLog(text);
        swapChain->Release();
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(
            text,
            "[UI] WARNING: MH_EnableHook failed for SwapChain::Present (%d); Device::Present remains available.\n",
            static_cast<int>(status));
        AppendLog(text);

        // This hook is optional, but a successful create followed by a failed
        // enable must not leave a dormant MinHook entry behind. Only clear the
        // trampoline after MinHook confirms that the hook no longer exists; if
        // removal itself fails, retaining the trampoline is the safer failure
        // mode for any residual detour.
        const MH_STATUS removeStatus = MH_RemoveHook(target);
        if (removeStatus == MH_OK || removeStatus == MH_ERROR_NOT_CREATED)
        {
            g_originalSwapChainPresent = nullptr;
        }
        else
        {
            char rollbackText[224] = {};
            sprintf_s(
                rollbackText,
                "[UI] ERROR: SwapChain::Present rollback could not remove hook (%d); retaining trampoline.\n",
                static_cast<int>(removeStatus));
            AppendLog(rollbackText);
        }

        swapChain->Release();
        return false;
    }

    AppendLog("[UI] SwapChain::Present hook installed.\n");
    swapChain->Release();
    return true;
}


static bool InstallDeviceHooks(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    void** vtable =
        *reinterpret_cast<void***>(device);

    struct HookEntry
    {
        void* target;
        void* hook;
        void** original;
        const char* name;
    };

    // Keep the production hook surface deliberately small. Every entry below
    // directly supports a shipped feature: UI, render-resource replacement,
    // viewport scaling, texture filtering, shader-constant correction or a
    // known Deadly Premonition compatibility fix inherited from DPFix.
    HookEntry hooks[] =
    {
        { vtable[16], reinterpret_cast<void*>(&HookReset),
          reinterpret_cast<void**>(&g_originalReset), "Reset" },
        { vtable[17], reinterpret_cast<void*>(&HookPresent),
          reinterpret_cast<void**>(&g_originalPresent), "Present" },
        { vtable[42], reinterpret_cast<void*>(&HookEndScene),
          reinterpret_cast<void**>(&g_originalEndScene), "EndScene" },
        { vtable[23], reinterpret_cast<void*>(&HookCreateTexture),
          reinterpret_cast<void**>(&g_originalCreateTexture), "CreateTexture" },
        { vtable[28], reinterpret_cast<void*>(&HookCreateRenderTarget),
          reinterpret_cast<void**>(&g_originalCreateRenderTarget), "CreateRenderTarget" },
        { vtable[29], reinterpret_cast<void*>(&HookCreateDepthStencilSurface),
          reinterpret_cast<void**>(&g_originalCreateDepthStencilSurface), "CreateDepthStencilSurface" },
        { vtable[34], reinterpret_cast<void*>(&HookStretchRect),
          reinterpret_cast<void**>(&g_originalStretchRect), "StretchRect" },
        { vtable[37], reinterpret_cast<void*>(&HookSetRenderTarget),
          reinterpret_cast<void**>(&g_originalSetRenderTarget), "SetRenderTarget" },
        { vtable[39], reinterpret_cast<void*>(&HookSetDepthStencilSurface),
          reinterpret_cast<void**>(&g_originalSetDepthStencilSurface), "SetDepthStencilSurface" },
        { vtable[47], reinterpret_cast<void*>(&HookSetViewport),
          reinterpret_cast<void**>(&g_originalSetViewport), "SetViewport" },
        { vtable[65], reinterpret_cast<void*>(&HookSetTexture),
          reinterpret_cast<void**>(&g_originalSetTexture), "SetTexture" },
        { vtable[69], reinterpret_cast<void*>(&HookSetSamplerState),
          reinterpret_cast<void**>(&g_originalSetSamplerState), "SetSamplerState" },
        { vtable[81], reinterpret_cast<void*>(&HookDrawPrimitive),
          reinterpret_cast<void**>(&g_originalDrawPrimitive), "DrawPrimitive" },
        { vtable[82], reinterpret_cast<void*>(&HookDrawIndexedPrimitive),
          reinterpret_cast<void**>(&g_originalDrawIndexedPrimitive), "DrawIndexedPrimitive" },
        { vtable[83], reinterpret_cast<void*>(&HookDrawPrimitiveUP),
          reinterpret_cast<void**>(&g_originalDrawPrimitiveUP), "DrawPrimitiveUP" },
        { vtable[84], reinterpret_cast<void*>(&HookDrawIndexedPrimitiveUP),
          reinterpret_cast<void**>(&g_originalDrawIndexedPrimitiveUP), "DrawIndexedPrimitiveUP" },
        { vtable[94], reinterpret_cast<void*>(&HookSetVertexShaderConstantF),
          reinterpret_cast<void**>(&g_originalSetVertexShaderConstantF), "SetVertexShaderConstantF" },
        { vtable[100], reinterpret_cast<void*>(&HookSetStreamSource),
          reinterpret_cast<void**>(&g_originalSetStreamSource), "SetStreamSource" },
        { vtable[106], reinterpret_cast<void*>(&HookCreatePixelShader),
          reinterpret_cast<void**>(&g_originalCreatePixelShader), "CreatePixelShader" },
        { vtable[107], reinterpret_cast<void*>(&HookSetPixelShader),
          reinterpret_cast<void**>(&g_originalSetPixelShader), "SetPixelShader" },
        { vtable[109], reinterpret_cast<void*>(&HookSetPixelShaderConstantF),
          reinterpret_cast<void**>(&g_originalSetPixelShaderConstantF), "SetPixelShaderConstantF" }
    };

    constexpr size_t hookCount = sizeof(hooks) / sizeof(hooks[0]);

    auto removeCreatedHooks = [&hooks](size_t createdCount)
    {
        // Work backwards through the transaction and clear each trampoline
        // only after MinHook confirms that the corresponding hook is gone. A
        // failed remove can leave a residual detour; retaining its trampoline
        // keeps that failure path callable instead of turning it into a null
        // original-function dereference.
        for (size_t i = createdCount; i > 0; --i)
        {
            HookEntry& entry = hooks[i - 1];
            const MH_STATUS removeStatus = MH_RemoveHook(entry.target);
            if (removeStatus == MH_OK || removeStatus == MH_ERROR_NOT_CREATED)
            {
                if (entry.original != nullptr)
                    *entry.original = nullptr;
                continue;
            }

            char text[320] = {};
            sprintf_s(
                text,
                "ERROR: D3D9 hook rollback could not remove %s (%d); retaining trampoline.\n",
                entry.name,
                static_cast<int>(removeStatus));
            AppendLog(text);
        }
    };

    // Phase 1: create every mandatory hook before enabling any of them. If a
    // later create fails, no ZachFix D3D9 detour has become executable yet.
    size_t createdCount = 0;
    for (size_t i = 0; i < hookCount; ++i)
    {
        HookEntry& entry = hooks[i];
        const MH_STATUS status = MH_CreateHook(
            entry.target,
            entry.hook,
            entry.original);

        if (status != MH_OK)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "ERROR: MH_CreateHook failed for %s (%d); rolling back D3D9 hook batch.\n",
                entry.name,
                static_cast<int>(status));
            AppendLog(text);

            removeCreatedHooks(createdCount);
            return false;
        }

        ++createdCount;
    }

    // Phase 2: enable the fully-created batch. A failure here can briefly make
    // an earlier subset active, so disable that subset first, then remove every
    // hook created by this transaction before returning failure.
    size_t enabledCount = 0;
    for (size_t i = 0; i < hookCount; ++i)
    {
        HookEntry& entry = hooks[i];
        const MH_STATUS status = MH_EnableHook(entry.target);

        if (status != MH_OK)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "ERROR: MH_EnableHook failed for %s (%d); rolling back D3D9 hook batch.\n",
                entry.name,
                static_cast<int>(status));
            AppendLog(text);

            for (size_t j = 0; j < enabledCount; ++j)
            {
                const MH_STATUS disableStatus = MH_DisableHook(hooks[j].target);
                if (disableStatus != MH_OK && disableStatus != MH_ERROR_DISABLED)
                {
                    char rollbackText[320] = {};
                    sprintf_s(
                        rollbackText,
                        "ERROR: D3D9 hook rollback could not disable %s (%d).\n",
                        hooks[j].name,
                        static_cast<int>(disableStatus));
                    AppendLog(rollbackText);
                }
            }

            removeCreatedHooks(createdCount);
            return false;
        }

        ++enabledCount;
    }

    // Only after the mandatory set is fully active should dependent renderer
    // state be initialized or the individual hooks be reported as installed.
    InitializePostFxFramework(device);

    for (const HookEntry& entry : hooks)
    {
        char text[256] = {};
        sprintf_s(text, "%s hook installed.\n", entry.name);
        AppendLog(text);
    }

    {
        char text[160] = {};
        sprintf_s(
            text,
            "[D3D9] Mandatory hook transaction committed (%u hooks).\n",
            static_cast<unsigned>(hookCount));
        AppendLog(text);
    }

    // Some D3D9 applications present through the primary swap chain directly
    // rather than IDirect3DDevice9::Present. The native D3D9 test is consistent
    // with that path, while the established DXVK path reaches Device::Present.
    // Keep the extra hook non-fatal so a wrapper with an unusual swap-chain
    // implementation cannot disable the rest of ZachFix.
    InstallSwapChainPresentHook(device);

    InitializeTextureFiltering(device);
    return true;
}

static bool ConfigureGameWindow(
    HWND window,
    UINT clientWidth,
    UINT clientHeight,
    bool borderless)
{
    if (window == nullptr)
    {
        AppendLog("[Window] ERROR: null window handle.\n");
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(
        window,
        MONITOR_DEFAULTTONEAREST
    );

    if (monitor == nullptr)
    {
        AppendLog("[Window] ERROR: MonitorFromWindow failed.\n");
        return false;
    }

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfoW(monitor, &monitorInfo))
    {
        AppendLog("[Window] ERROR: GetMonitorInfoW failed.\n");
        return false;
    }

    int x = monitorInfo.rcMonitor.left;
    int y = monitorInfo.rcMonitor.top;
    int outerWidth = static_cast<int>(clientWidth);
    int outerHeight = static_cast<int>(clientHeight);

    if (borderless)
    {
        LONG_PTR style =
            GetWindowLongPtrW(window, GWL_STYLE);

        style &= ~static_cast<LONG_PTR>(
            WS_CAPTION |
            WS_THICKFRAME |
            WS_MINIMIZEBOX |
            WS_MAXIMIZEBOX |
            WS_SYSMENU
        );

        style |= WS_POPUP;

        SetWindowLongPtrW(
            window,
            GWL_STYLE,
            style
        );

        LONG_PTR exStyle =
            GetWindowLongPtrW(window, GWL_EXSTYLE);

        exStyle &= ~static_cast<LONG_PTR>(
            WS_EX_DLGMODALFRAME |
            WS_EX_WINDOWEDGE |
            WS_EX_CLIENTEDGE |
            WS_EX_STATICEDGE
        );

        SetWindowLongPtrW(
            window,
            GWL_EXSTYLE,
            exStyle
        );
    }
    else
    {
        const DWORD style = static_cast<DWORD>(
            GetWindowLongPtrW(window, GWL_STYLE)
        );

        const DWORD exStyle = static_cast<DWORD>(
            GetWindowLongPtrW(window, GWL_EXSTYLE)
        );

        RECT outerRect =
        {
            0,
            0,
            static_cast<LONG>(clientWidth),
            static_cast<LONG>(clientHeight)
        };

        if (AdjustWindowRectEx(
                &outerRect,
                style,
                FALSE,
                exStyle))
        {
            outerWidth = outerRect.right - outerRect.left;
            outerHeight = outerRect.bottom - outerRect.top;
        }

        const int monitorWidth =
            monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;

        const int monitorHeight =
            monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;

        x = monitorInfo.rcMonitor.left +
            (monitorWidth - outerWidth) / 2;

        y = monitorInfo.rcMonitor.top +
            (monitorHeight - outerHeight) / 2;
    }

    const BOOL positioned = SetWindowPos(
        window,
        HWND_TOP,
        x,
        y,
        outerWidth,
        outerHeight,
        SWP_FRAMECHANGED |
        SWP_NOOWNERZORDER |
        SWP_NOACTIVATE
    );

    if (!positioned)
    {
        AppendLog("[Window] ERROR: SetWindowPos failed.\n");
        return false;
    }

    RECT clientRect = {};

    if (!GetClientRect(window, &clientRect))
    {
        AppendLog("[Window] ERROR: GetClientRect failed.\n");
        return false;
    }

    const UINT actualWidth =
        static_cast<UINT>(clientRect.right - clientRect.left);

    const UINT actualHeight =
        static_cast<UINT>(clientRect.bottom - clientRect.top);

    char text[512] = {};

    sprintf_s(
        text,
        "[Window] %s client area: %u x %u, requested %u x %u, "
        "position %d,%d\n",
        borderless ? "Borderless" : "Windowed",
        actualWidth,
        actualHeight,
        clientWidth,
        clientHeight,
        x,
        y
    );

    AppendLog(text);

    return
        actualWidth == clientWidth &&
        actualHeight == clientHeight;
}


