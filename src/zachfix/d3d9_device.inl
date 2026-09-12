// -----------------------------------------------------------------------------
// Device hooks
// -----------------------------------------------------------------------------

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
    const UINT originalWidth = width;
    const UINT originalHeight = height;

    const bool isKnownShadow =
        IsKnownShadowTexture(width, height, usage, format);

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

    const HRESULT result = g_originalCreateTexture(
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
            format,
            pool);
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

    const HRESULT result = g_originalCreateDepthStencilSurface(
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
    if (index == 0)
        ObserveAndApplyAdditionalDofBlur(self);

    IDirect3DSurface9* logicalTarget =
        ResolveRuntimeLogicalSurface(target);
    IDirect3DSurface9* replacement =
        AcquireRuntimeReplacementSurface(logicalTarget);
    IDirect3DSurface9* effectiveTarget =
        replacement != nullptr ? replacement : logicalTarget;

    const HRESULT result = g_originalSetRenderTarget(
        self,
        index,
        effectiveTarget
    );

    if (SUCCEEDED(result) && index == 0)
    {
        g_currentRenderTarget0.store(
            logicalTarget,
            std::memory_order_release
        );
    }

    if (replacement != nullptr)
        replacement->Release();

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


static HRESULT WINAPI HookSetVertexShaderConstantF(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount)
{
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


static HRESULT WINAPI HookSetViewport(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport)
{
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


static HRESULT WINAPI HookEndScene(IDirect3DDevice9* self)
{
    g_endSceneUiPathActive.store(true, std::memory_order_release);

    bool expected = false;
    if (g_loggedEndSceneUiPath.compare_exchange_strong(
            expected, true, std::memory_order_relaxed))
    {
        AppendLog("[UI] EndScene UI path active.\n");
    }

    // We are still inside the game's BeginScene/EndScene pair here, so the
    // ImGui DX9 backend can draw without ZachFix opening a second scene.
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
    const bool outermostPresent = g_presentHookDepth++ == 0;
    if (outermostPresent)
    {
        bool expected = false;
        if (g_loggedDevicePresentPath.compare_exchange_strong(
                expected, true, std::memory_order_relaxed))
        {
            AppendLog("[UI] Device Present path active.\n");
        }

        if (!g_endSceneUiPathActive.load(std::memory_order_acquire))
            RenderSettingsUi(self);
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
    const bool outermostPresent = g_presentHookDepth++ == 0;
    if (outermostPresent)
    {
        bool expected = false;
        if (g_loggedSwapChainPresentPath.compare_exchange_strong(
                expected, true, std::memory_order_relaxed))
        {
            AppendLog("[UI] SwapChain Present path active.\n");
        }

        IDirect3DDevice9* device = nullptr;
        if (SUCCEEDED(self->GetDevice(&device)) && device != nullptr)
        {
            if (!g_endSceneUiPathActive.load(std::memory_order_acquire))
                RenderSettingsUi(device);
            device->Release();
        }
    }

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
    IDirect3DSurface9* logicalSource =
        ResolveRuntimeLogicalSurface(sourceSurface);
    IDirect3DSurface9* logicalDest =
        ResolveRuntimeLogicalSurface(destSurface);
    IDirect3DSurface9* replacementSource =
        AcquireRuntimeReplacementSurface(logicalSource);
    IDirect3DSurface9* replacementDest =
        AcquireRuntimeReplacementSurface(logicalDest);

    IDirect3DSurface9* effectiveSource =
        replacementSource != nullptr ? replacementSource : logicalSource;
    IDirect3DSurface9* effectiveDest =
        replacementDest != nullptr ? replacementDest : logicalDest;

    const HRESULT result = g_originalStretchRect(
        self,
        effectiveSource,
        sourceRect,
        effectiveDest,
        destRect,
        filter
    );

    if (replacementSource != nullptr)
        replacementSource->Release();
    if (replacementDest != nullptr)
        replacementDest->Release();

    return result;
}


static HRESULT WINAPI HookSetDepthStencilSurface(
    IDirect3DDevice9* self,
    IDirect3DSurface9* newDepthStencil)
{
    IDirect3DSurface9* logicalDepth =
        ResolveRuntimeLogicalSurface(newDepthStencil);
    IDirect3DSurface9* replacement =
        AcquireRuntimeReplacementSurface(logicalDepth);
    IDirect3DSurface9* effectiveDepth =
        replacement != nullptr ? replacement : logicalDepth;

    const HRESULT result =
        g_originalSetDepthStencilSurface(
            self,
            effectiveDepth
        );

    if (replacement != nullptr)
        replacement->Release();

    return result;
}


static HRESULT WINAPI HookSetTexture(
    IDirect3DDevice9* self,
    DWORD stage,
    IDirect3DBaseTexture9* texture)
{
    IDirect3DBaseTexture9* logicalTexture =
        ResolveRuntimeLogicalTexture(texture);

    // Asset hot reload and render-target Hot Apply intentionally share this
    // already-existing bind hook, but keep separate lifetime managers. A
    // normal D3DX asset cannot also be one of our scalable render targets.
    IDirect3DTexture9* hotTexture =
        AcquireTextureOverrideHotReplacement(self, logicalTexture);
    IDirect3DTexture9* runtimeTexture =
        hotTexture == nullptr
            ? AcquireRuntimeReplacementTexture(logicalTexture)
            : nullptr;

    IDirect3DBaseTexture9* effectiveTexture = logicalTexture;
    if (hotTexture != nullptr)
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

    if (hotTexture != nullptr)
        hotTexture->Release();
    if (runtimeTexture != nullptr)
        runtimeTexture->Release();

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
    // viewport scaling, texture filtering or shader-constant correction.
    HookEntry hooks[] =
    {
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
        { vtable[94], reinterpret_cast<void*>(&HookSetVertexShaderConstantF),
          reinterpret_cast<void**>(&g_originalSetVertexShaderConstantF), "SetVertexShaderConstantF" },
        { vtable[109], reinterpret_cast<void*>(&HookSetPixelShaderConstantF),
          reinterpret_cast<void**>(&g_originalSetPixelShaderConstantF), "SetPixelShaderConstantF" }
    };

    for (const HookEntry& entry : hooks)
    {
        MH_STATUS status = MH_CreateHook(
            entry.target,
            entry.hook,
            entry.original
        );

        if (status != MH_OK)
        {
            char text[256] = {};
            sprintf_s(
                text,
                "ERROR: MH_CreateHook failed for %s (%d).\n",
                entry.name,
                static_cast<int>(status)
            );
            AppendLog(text);
            return false;
        }

        status = MH_EnableHook(entry.target);

        if (status != MH_OK)
        {
            char text[256] = {};
            sprintf_s(
                text,
                "ERROR: MH_EnableHook failed for %s (%d).\n",
                entry.name,
                static_cast<int>(status)
            );
            AppendLog(text);
            return false;
        }

        char text[256] = {};
        sprintf_s(text, "%s hook installed.\n", entry.name);
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


