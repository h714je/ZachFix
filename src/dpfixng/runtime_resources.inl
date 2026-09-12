// -----------------------------------------------------------------------------
// Runtime render-resource replacement manager
// -----------------------------------------------------------------------------
//
// Deadly Premonition creates its long-lived render targets once and keeps the
// COM handles. For hot apply we deliberately do NOT Reset the D3D9 device.
// Instead, the original game handles remain logical identities and the D3D9
// binding hooks transparently substitute a new backing generation.
//
// This keeps all engine-side ownership/lifetime assumptions intact while still
// allowing InternalScale, shadow/reflection resolution and DoF resolution to
// change between frames.

struct RuntimeManagedResource
{
    bool textureBacked = false;
    IDirect3DTexture9* originalTexture = nullptr;
    IDirect3DSurface9* originalSurface = nullptr;

    UINT requestedWidth = 0;
    UINT requestedHeight = 0;
    UINT initialEffectiveWidth = 0;
    UINT initialEffectiveHeight = 0;
    UINT levels = 1;
    DWORD usage = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    D3DPOOL pool = D3DPOOL_DEFAULT;
    D3DMULTISAMPLE_TYPE multiSample = D3DMULTISAMPLE_NONE;
    DWORD multiSampleQuality = 0;
    BOOL lockableOrDiscard = FALSE;
    ProfilerResourceTag tag = ProfilerResourceTag::Unknown;

    std::atomic<IDirect3DTexture9*> replacementTexture{ nullptr };
    std::atomic<IDirect3DSurface9*> replacementSurface{ nullptr };
    std::atomic<UINT> currentEffectiveWidth{ 0 };
    std::atomic<UINT> currentEffectiveHeight{ 0 };
};

static constexpr UINT kMaxRuntimeManagedResources = 96;
static RuntimeManagedResource g_runtimeManagedResources[kMaxRuntimeManagedResources];
static std::atomic<UINT> g_runtimeManagedResourceCount{ 0 };
static std::mutex g_runtimeManagedResourceMutex;

static std::atomic<UINT> g_runtimeGeneration{ 0 };
static std::atomic<UINT> g_runtimeLastChangedResources{ 0 };
static std::atomic<unsigned long long> g_runtimeReplacementCreates{ 0 };
static std::atomic<unsigned long long> g_runtimeReplacementReleases{ 0 };
static std::atomic<unsigned long long> g_runtimeApplySuccesses{ 0 };
static std::atomic<unsigned long long> g_runtimeApplyFailures{ 0 };

static UINT RuntimeBytesPerPixel(D3DFORMAT format)
{
    switch (format)
    {
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_D24S8:
        case D3DFMT_D24X8:
        case D3DFMT_R32F:
            return 4;
        case D3DFMT_A16B16G16R16F:
            return 8;
        case D3DFMT_D16:
        case D3DFMT_R5G6B5:
        case D3DFMT_A1R5G5B5:
            return 2;
        default:
            return 0;
    }
}

static unsigned long long EstimateRuntimeResourceBytes(
    const RuntimeManagedResource& resource,
    UINT width,
    UINT height)
{
    const UINT bpp = RuntimeBytesPerPixel(resource.format);
    if (bpp == 0 || width == 0 || height == 0)
        return 0;

    unsigned long long total = 0;
    UINT w = width;
    UINT h = height;
    const UINT levels = resource.textureBacked ? std::max<UINT>(1, resource.levels) : 1;

    for (UINT level = 0; level < levels; ++level)
    {
        total += static_cast<unsigned long long>(w) *
                 static_cast<unsigned long long>(h) *
                 static_cast<unsigned long long>(bpp);
        if (w == 1 && h == 1)
            break;
        w = std::max<UINT>(1, w / 2);
        h = std::max<UINT>(1, h / 2);
    }

    return total;
}

static bool IsRuntimeManagedTag(ProfilerResourceTag tag)
{
    switch (tag)
    {
        case ProfilerResourceTag::MainLdr:
        case ProfilerResourceTag::MainHdr:
        case ProfilerResourceTag::MainDepth:
        case ProfilerResourceTag::ShadowColor512:
        case ProfilerResourceTag::ShadowDepth512:
        case ProfilerResourceTag::ShadowColor1024:
        case ProfilerResourceTag::ShadowDepth1024:
        case ProfilerResourceTag::ReflectionSmallColor:
        case ProfilerResourceTag::ReflectionSmallDepth:
        case ProfilerResourceTag::ReflectionLargeColor:
        case ProfilerResourceTag::ReflectionLargeDepth:
        case ProfilerResourceTag::Storage448:
        case ProfilerResourceTag::Storage896:
        case ProfilerResourceTag::DofHdr448:
            return true;
        default:
            return false;
    }
}

static bool IsShadowRuntimeTag(ProfilerResourceTag tag)
{
    return tag == ProfilerResourceTag::ShadowColor512 ||
           tag == ProfilerResourceTag::ShadowDepth512 ||
           tag == ProfilerResourceTag::ShadowColor1024 ||
           tag == ProfilerResourceTag::ShadowDepth1024;
}

static bool IsReflectionRuntimeTag(ProfilerResourceTag tag)
{
    return tag == ProfilerResourceTag::ReflectionSmallColor ||
           tag == ProfilerResourceTag::ReflectionSmallDepth ||
           tag == ProfilerResourceTag::ReflectionLargeColor ||
           tag == ProfilerResourceTag::ReflectionLargeDepth;
}

static bool ResolveRuntimeInternalSize(
    const DPFixNGConfig& requested,
    UINT& width,
    UINT& height)
{
    const bool hasExplicit =
        requested.internalWidth != 0 ||
        requested.internalHeight != 0;

    if (hasExplicit)
    {
        if (requested.internalWidth < 640 ||
            requested.internalHeight < 360 ||
            requested.internalWidth > kMaxResolutionWidth ||
            requested.internalHeight > kMaxResolutionHeight)
        {
            return false;
        }

        width = requested.internalWidth;
        height = requested.internalHeight;
        return true;
    }

    if (!std::isfinite(requested.internalScale) ||
        requested.internalScale < 0.25f ||
        requested.internalScale > 4.0f ||
        g_displayWidth == 0 ||
        g_displayHeight == 0)
    {
        return false;
    }

    const double scaledWidth =
        static_cast<double>(g_displayWidth) *
        static_cast<double>(requested.internalScale);
    const double scaledHeight =
        static_cast<double>(g_displayHeight) *
        static_cast<double>(requested.internalScale);

    width = static_cast<UINT>(scaledWidth + 0.5);
    height = static_cast<UINT>(scaledHeight + 0.5);

    return width >= 640 && height >= 360 &&
           width <= kMaxResolutionWidth &&
           height <= kMaxResolutionHeight;
}

static UINT ScaleRuntimeDimension(UINT value, UINT scale)
{
    const unsigned long long scaled =
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(scale);

    return static_cast<UINT>(
        std::min<unsigned long long>(scaled, kMaxResolutionWidth));
}

static UINT ScaleRuntimeFromBase(UINT value, UINT target, UINT base)
{
    return static_cast<UINT>(
        static_cast<unsigned long long>(value) *
        static_cast<unsigned long long>(target) /
        static_cast<unsigned long long>(base));
}

static void GetRuntimeTargetSize(
    const RuntimeManagedResource& resource,
    const DPFixNGConfig& requested,
    UINT internalWidth,
    UINT internalHeight,
    UINT& width,
    UINT& height)
{
    width = resource.requestedWidth;
    height = resource.requestedHeight;

    if (resource.tag == ProfilerResourceTag::MainLdr ||
        resource.tag == ProfilerResourceTag::MainHdr ||
        resource.tag == ProfilerResourceTag::MainDepth)
    {
        width = internalWidth;
        height = internalHeight;
        return;
    }

    if (IsShadowRuntimeTag(resource.tag))
    {
        width = ScaleRuntimeDimension(resource.requestedWidth, requested.shadowScale);
        height = ScaleRuntimeDimension(resource.requestedHeight, requested.shadowScale);
        return;
    }

    if (IsReflectionRuntimeTag(resource.tag))
    {
        width = ScaleRuntimeDimension(resource.requestedWidth, requested.reflectionScale);
        height = ScaleRuntimeDimension(resource.requestedHeight, requested.reflectionScale);
        return;
    }

    if (resource.tag == ProfilerResourceTag::Storage448 ||
        resource.tag == ProfilerResourceTag::Storage896)
    {
        width = ScaleRuntimeFromBase(resource.requestedWidth, internalWidth, kBaseRenderWidth);
        height = ScaleRuntimeFromBase(resource.requestedHeight, internalHeight, kBaseRenderHeight);
        return;
    }

    if (resource.tag == ProfilerResourceTag::DofHdr448 &&
        requested.improveDofResolution)
    {
        width = ScaleRuntimeFromBase(resource.requestedWidth, internalWidth, kBaseRenderWidth);
        height = ScaleRuntimeFromBase(resource.requestedHeight, internalHeight, kBaseRenderHeight);
    }
}

static bool RuntimeResourceAlreadyTracked(
    IDirect3DTexture9* texture,
    IDirect3DSurface9* surface)
{
    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_acquire);
    for (UINT i = 0; i < count; ++i)
    {
        const RuntimeManagedResource& r = g_runtimeManagedResources[i];
        if (texture != nullptr && r.originalTexture == texture)
            return true;
        if (surface != nullptr && r.originalSurface == surface)
            return true;
    }
    return false;
}

void TrackRuntimeTextureResource(
    IDirect3DTexture9* texture,
    UINT requestedWidth,
    UINT requestedHeight,
    UINT effectiveWidth,
    UINT effectiveHeight,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool)
{
    if (texture == nullptr)
        return;

    const ProfilerResourceTag tag =
        ClassifyProfilerResource(requestedWidth, requestedHeight, usage, format);
    if (!IsRuntimeManagedTag(tag))
        return;

    IDirect3DSurface9* level0 = nullptr;
    if (FAILED(texture->GetSurfaceLevel(0, &level0)) || level0 == nullptr)
        return;

    std::lock_guard<std::mutex> lock(g_runtimeManagedResourceMutex);

    if (RuntimeResourceAlreadyTracked(texture, level0))
    {
        level0->Release();
        return;
    }

    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_relaxed);
    if (count >= kMaxRuntimeManagedResources)
    {
        static bool logged = false;
        if (!logged)
        {
            AppendLog("[Runtime] WARNING: managed render-resource table is full.\n");
            logged = true;
        }
        level0->Release();
        return;
    }

    RuntimeManagedResource& r = g_runtimeManagedResources[count];
    r.textureBacked = true;
    r.originalTexture = texture;
    r.originalSurface = level0; // pointer identity only; texture owns lifetime
    r.requestedWidth = requestedWidth;
    r.requestedHeight = requestedHeight;
    r.initialEffectiveWidth = effectiveWidth;
    r.initialEffectiveHeight = effectiveHeight;
    r.levels = levels;
    r.usage = usage;
    r.format = format;
    r.pool = pool;
    r.tag = tag;
    r.currentEffectiveWidth.store(effectiveWidth, std::memory_order_relaxed);
    r.currentEffectiveHeight.store(effectiveHeight, std::memory_order_relaxed);

    g_runtimeManagedResourceCount.store(count + 1, std::memory_order_release);
    level0->Release();
}

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
    BOOL lockableOrDiscard)
{
    if (surface == nullptr)
        return;

    const ProfilerResourceTag tag =
        ClassifyProfilerResource(requestedWidth, requestedHeight, usage, format);
    if (!IsRuntimeManagedTag(tag))
        return;

    std::lock_guard<std::mutex> lock(g_runtimeManagedResourceMutex);
    if (RuntimeResourceAlreadyTracked(nullptr, surface))
        return;

    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_relaxed);
    if (count >= kMaxRuntimeManagedResources)
        return;

    RuntimeManagedResource& r = g_runtimeManagedResources[count];
    r.textureBacked = false;
    r.originalSurface = surface;
    r.requestedWidth = requestedWidth;
    r.requestedHeight = requestedHeight;
    r.initialEffectiveWidth = effectiveWidth;
    r.initialEffectiveHeight = effectiveHeight;
    r.usage = usage;
    r.format = format;
    r.pool = pool;
    r.multiSample = multiSample;
    r.multiSampleQuality = multiSampleQuality;
    r.lockableOrDiscard = lockableOrDiscard;
    r.tag = tag;
    r.currentEffectiveWidth.store(effectiveWidth, std::memory_order_relaxed);
    r.currentEffectiveHeight.store(effectiveHeight, std::memory_order_relaxed);

    g_runtimeManagedResourceCount.store(count + 1, std::memory_order_release);
}

IDirect3DTexture9* AcquireRuntimeReplacementTexture(IDirect3DBaseTexture9* original)
{
    if (original == nullptr)
        return nullptr;

    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_acquire);
    for (UINT i = 0; i < count; ++i)
    {
        RuntimeManagedResource& r = g_runtimeManagedResources[i];
        if (r.originalTexture != original)
            continue;

        IDirect3DTexture9* replacement =
            r.replacementTexture.load(std::memory_order_acquire);
        if (replacement != nullptr)
            replacement->AddRef();
        return replacement;
    }

    return nullptr;
}

IDirect3DSurface9* AcquireRuntimeReplacementSurface(IDirect3DSurface9* original)
{
    if (original == nullptr)
        return nullptr;

    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_acquire);
    for (UINT i = 0; i < count; ++i)
    {
        RuntimeManagedResource& r = g_runtimeManagedResources[i];
        if (r.originalSurface != original)
            continue;

        IDirect3DSurface9* replacement =
            r.replacementSurface.load(std::memory_order_acquire);
        if (replacement != nullptr)
            replacement->AddRef();
        return replacement;
    }

    return nullptr;
}

IDirect3DBaseTexture9* ResolveRuntimeLogicalTexture(IDirect3DBaseTexture9* texture)
{
    if (texture == nullptr)
        return nullptr;

    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_acquire);
    for (UINT i = 0; i < count; ++i)
    {
        RuntimeManagedResource& r = g_runtimeManagedResources[i];
        IDirect3DTexture9* replacement =
            r.replacementTexture.load(std::memory_order_acquire);
        if (replacement != nullptr && replacement == texture)
            return static_cast<IDirect3DBaseTexture9*>(r.originalTexture);
    }

    return texture;
}

IDirect3DSurface9* ResolveRuntimeLogicalSurface(IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return nullptr;

    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_acquire);
    for (UINT i = 0; i < count; ++i)
    {
        RuntimeManagedResource& r = g_runtimeManagedResources[i];
        IDirect3DSurface9* replacement =
            r.replacementSurface.load(std::memory_order_acquire);
        if (replacement != nullptr && replacement == surface)
            return r.originalSurface;
    }

    return surface;
}

UINT GetRuntimeManagedResourceCount()
{
    return g_runtimeManagedResourceCount.load(std::memory_order_acquire);
}

enum class RuntimeReplacementAction
{
    Keep,
    UseOriginal,
    Replace
};

struct PendingRuntimeReplacement
{
    RuntimeReplacementAction action = RuntimeReplacementAction::Keep;
    IDirect3DTexture9* texture = nullptr;
    IDirect3DSurface9* surface = nullptr;
    UINT width = 0;
    UINT height = 0;
};

static void ReleasePendingRuntimeReplacement(PendingRuntimeReplacement& r)
{
    const bool ownedReplacement =
        r.action == RuntimeReplacementAction::Replace &&
        (r.texture != nullptr || r.surface != nullptr);

    if (r.surface != nullptr)
    {
        r.surface->Release();
        r.surface = nullptr;
    }
    if (r.texture != nullptr)
    {
        r.texture->Release();
        r.texture = nullptr;
    }

    if (ownedReplacement)
        g_runtimeReplacementReleases.fetch_add(1, std::memory_order_relaxed);

    r.action = RuntimeReplacementAction::Keep;
}

static bool BuildRuntimeReplacement(
    IDirect3DDevice9* device,
    const RuntimeManagedResource& resource,
    UINT width,
    UINT height,
    PendingRuntimeReplacement& pending)
{
    pending.width = width;
    pending.height = height;

    const UINT currentWidth =
        resource.currentEffectiveWidth.load(std::memory_order_acquire);
    const UINT currentHeight =
        resource.currentEffectiveHeight.load(std::memory_order_acquire);

    if (width == currentWidth && height == currentHeight)
    {
        pending.action = RuntimeReplacementAction::Keep;
        return true;
    }

    if (width == resource.initialEffectiveWidth &&
        height == resource.initialEffectiveHeight)
    {
        // Switching back to the game's original backing resource needs no
        // allocation. Commit will retire the current replacement, if any.
        pending.action = RuntimeReplacementAction::UseOriginal;
        return true;
    }

    pending.action = RuntimeReplacementAction::Replace;

    if (resource.textureBacked)
    {
        IDirect3DTexture9* texture = nullptr;
        const HRESULT hr = g_originalCreateTexture(
            device,
            width,
            height,
            resource.levels,
            resource.usage,
            resource.format,
            resource.pool,
            &texture,
            nullptr);

        if (FAILED(hr) || texture == nullptr)
            return false;

        IDirect3DSurface9* surface = nullptr;
        if (FAILED(texture->GetSurfaceLevel(0, &surface)) || surface == nullptr)
        {
            texture->Release();
            return false;
        }

        pending.texture = texture;
        pending.surface = surface;
        g_runtimeReplacementCreates.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    IDirect3DSurface9* surface = nullptr;
    HRESULT hr = E_FAIL;

    if ((resource.usage & D3DUSAGE_DEPTHSTENCIL) != 0)
    {
        hr = g_originalCreateDepthStencilSurface(
            device,
            width,
            height,
            resource.format,
            resource.multiSample,
            resource.multiSampleQuality,
            resource.lockableOrDiscard,
            &surface,
            nullptr);
    }
    else
    {
        hr = g_originalCreateRenderTarget(
            device,
            width,
            height,
            resource.format,
            resource.multiSample,
            resource.multiSampleQuality,
            resource.lockableOrDiscard,
            &surface,
            nullptr);
    }

    if (FAILED(hr) || surface == nullptr)
        return false;

    pending.surface = surface;
    g_runtimeReplacementCreates.fetch_add(1, std::memory_order_relaxed);
    return true;
}

RuntimeResourceStats GetRuntimeResourceStats()
{
    RuntimeResourceStats stats{};
    stats.generation = g_runtimeGeneration.load(std::memory_order_acquire);
    stats.lastChangedResources =
        g_runtimeLastChangedResources.load(std::memory_order_acquire);
    stats.replacementCreates =
        g_runtimeReplacementCreates.load(std::memory_order_acquire);
    stats.replacementReleases =
        g_runtimeReplacementReleases.load(std::memory_order_acquire);
    stats.applySuccesses =
        g_runtimeApplySuccesses.load(std::memory_order_acquire);
    stats.applyFailures =
        g_runtimeApplyFailures.load(std::memory_order_acquire);

    std::lock_guard<std::mutex> lock(g_runtimeManagedResourceMutex);
    const UINT count = g_runtimeManagedResourceCount.load(std::memory_order_acquire);
    stats.managedLogicalResources = count;

    for (UINT i = 0; i < count; ++i)
    {
        const RuntimeManagedResource& r = g_runtimeManagedResources[i];
        IDirect3DTexture9* texture =
            r.replacementTexture.load(std::memory_order_acquire);
        IDirect3DSurface9* surface =
            r.replacementSurface.load(std::memory_order_acquire);

        if (texture != nullptr)
            ++stats.activeTextureRefs;
        if (surface != nullptr)
            ++stats.activeSurfaceRefs;

        if (texture != nullptr || surface != nullptr)
        {
            ++stats.activeReplacementResources;
            stats.estimatedActiveBytes += EstimateRuntimeResourceBytes(
                r,
                r.currentEffectiveWidth.load(std::memory_order_acquire),
                r.currentEffectiveHeight.load(std::memory_order_acquire));
        }
    }

    return stats;
}

bool ApplyRuntimeRenderSettings(
    IDirect3DDevice9* device,
    const DPFixNGConfig& requested,
    char* status,
    size_t statusCount)
{
    if (status != nullptr && statusCount > 0)
        status[0] = '\0';

    auto fail = [&](const char* message)
    {
        g_runtimeApplyFailures.fetch_add(1, std::memory_order_relaxed);
        if (status != nullptr && statusCount > 0)
            strcpy_s(status, statusCount, message);
        return false;
    };

    if (device == nullptr)
        return fail("D3D9 device is unavailable.");

    if (requested.shadowScale < 1 || requested.shadowScale > 8 ||
        requested.reflectionScale < 1 || requested.reflectionScale > 8 ||
        requested.highDetailDistanceScale < 1 || requested.highDetailDistanceScale > 2)
    {
        return fail("One or more requested scales are invalid.");
    }

    UINT newInternalWidth = 0;
    UINT newInternalHeight = 0;
    if (!ResolveRuntimeInternalSize(requested, newInternalWidth, newInternalHeight))
        return fail("Internal resolution is outside supported limits.");

    const bool anyRenderSettingChanged =
        newInternalWidth != g_internalWidth ||
        newInternalHeight != g_internalHeight ||
        requested.shadowScale != g_config.shadowScale ||
        requested.reflectionScale != g_config.reflectionScale ||
        requested.improveDofResolution != g_config.improveDofResolution;

    UINT changedResources = 0;
    UINT newlyAllocatedResources = 0;

    {
        std::lock_guard<std::mutex> lock(g_runtimeManagedResourceMutex);

        const UINT count =
            g_runtimeManagedResourceCount.load(std::memory_order_acquire);
        if (anyRenderSettingChanged && count == 0)
            return fail("No scalable render resources have been discovered yet.");

        PendingRuntimeReplacement pending[kMaxRuntimeManagedResources] = {};

        for (UINT i = 0; i < count; ++i)
        {
            RuntimeManagedResource& resource = g_runtimeManagedResources[i];
            UINT targetWidth = 0;
            UINT targetHeight = 0;
            GetRuntimeTargetSize(
                resource,
                requested,
                newInternalWidth,
                newInternalHeight,
                targetWidth,
                targetHeight);

            if (targetWidth == 0 || targetHeight == 0 ||
                targetWidth > kMaxResolutionWidth ||
                targetHeight > kMaxResolutionHeight ||
                !BuildRuntimeReplacement(
                    device,
                    resource,
                    targetWidth,
                    targetHeight,
                    pending[i]))
            {
                for (UINT j = 0; j <= i; ++j)
                    ReleasePendingRuntimeReplacement(pending[j]);

                char logText[256] = {};
                sprintf_s(
                    logText,
                    "[Runtime] ERROR: selective hot rebuild failed for resource %u (%u x %u). Keeping previous generation.\n",
                    i,
                    targetWidth,
                    targetHeight);
                AppendLog(logText);
                return fail(
                    "Render-resource rebuild failed. Previous settings are still active.");
            }

            if (pending[i].action != RuntimeReplacementAction::Keep)
                ++changedResources;
            if (pending[i].action == RuntimeReplacementAction::Replace)
                ++newlyAllocatedResources;
        }

        // World switching is reversible. Do it before committing the render
        // generation so a signature failure leaves the whole Apply operation intact.
        if (!ApplyWorldDetailDistanceScale(requested.highDetailDistanceScale))
        {
            for (UINT i = 0; i < count; ++i)
                ReleasePendingRuntimeReplacement(pending[i]);
            return fail("World-detail switch failed. Nothing was applied.");
        }

        for (UINT i = 0; i < count; ++i)
        {
            PendingRuntimeReplacement& next = pending[i];
            if (next.action == RuntimeReplacementAction::Keep)
                continue;

            RuntimeManagedResource& resource = g_runtimeManagedResources[i];
            IDirect3DSurface9* nextSurface =
                next.action == RuntimeReplacementAction::Replace
                    ? next.surface
                    : nullptr;
            IDirect3DTexture9* nextTexture =
                next.action == RuntimeReplacementAction::Replace
                    ? next.texture
                    : nullptr;

            IDirect3DSurface9* oldSurface =
                resource.replacementSurface.exchange(
                    nextSurface,
                    std::memory_order_acq_rel);
            IDirect3DTexture9* oldTexture =
                resource.replacementTexture.exchange(
                    nextTexture,
                    std::memory_order_acq_rel);

            resource.currentEffectiveWidth.store(
                next.width,
                std::memory_order_release);
            resource.currentEffectiveHeight.store(
                next.height,
                std::memory_order_release);

            // Ownership has moved into the active generation.
            next.surface = nullptr;
            next.texture = nullptr;

            if (oldSurface != nullptr)
                oldSurface->Release();
            if (oldTexture != nullptr)
                oldTexture->Release();
            if (oldSurface != nullptr || oldTexture != nullptr)
            {
                g_runtimeReplacementReleases.fetch_add(
                    1,
                    std::memory_order_relaxed);
            }
        }

        g_config.internalWidth = requested.internalWidth;
        g_config.internalHeight = requested.internalHeight;
        g_config.internalScale = requested.internalScale;
        g_config.shadowScale = requested.shadowScale;
        g_config.reflectionScale = requested.reflectionScale;
        g_config.improveDofResolution = requested.improveDofResolution;
        g_config.fixPixelOffset = requested.fixPixelOffset;
        g_internalWidth = newInternalWidth;
        g_internalHeight = newInternalHeight;

        if (changedResources != 0)
            g_runtimeGeneration.fetch_add(1, std::memory_order_acq_rel);
        g_runtimeLastChangedResources.store(
            changedResources,
            std::memory_order_release);
        g_runtimeApplySuccesses.fetch_add(1, std::memory_order_relaxed);

        g_loggedViewportOverride.store(false, std::memory_order_relaxed);
        g_currentViewportWidth.store(kBaseRenderWidth, std::memory_order_relaxed);
        g_currentViewportHeight.store(kBaseRenderHeight, std::memory_order_relaxed);
    }

    // Query statistics only after releasing the managed-resource mutex.
    // GetRuntimeResourceStats() takes the same mutex.
    const RuntimeResourceStats stats = GetRuntimeResourceStats();
    const unsigned long long outstanding =
        stats.replacementCreates >= stats.replacementReleases
            ? stats.replacementCreates - stats.replacementReleases
            : 0;

    char logText[512] = {};
    sprintf_s(
        logText,
        "[Runtime] Hot apply committed: Internal=%u x %u, Shadow=%ux, Reflection=%ux, DoF=%s, PixelOffset=%s, changed=%u, allocated=%u, active=%u, created=%llu, released=%llu, outstanding=%llu, est=%.1f MiB.\n",
        g_internalWidth,
        g_internalHeight,
        g_config.shadowScale,
        g_config.reflectionScale,
        g_config.improveDofResolution ? "improved" : "original",
        g_config.fixPixelOffset ? "on" : "off",
        changedResources,
        newlyAllocatedResources,
        stats.activeReplacementResources,
        stats.replacementCreates,
        stats.replacementReleases,
        outstanding,
        static_cast<double>(stats.estimatedActiveBytes) / (1024.0 * 1024.0));
    AppendLog(logText);

    if (status != nullptr && statusCount > 0)
    {
        sprintf_s(
            status,
            statusCount,
            "Applied live: %u x %u, changed %u resource(s), active %u, outstanding %llu.",
            g_internalWidth,
            g_internalHeight,
            changedResources,
            stats.activeReplacementResources,
            outstanding);
    }

    return true;
}
