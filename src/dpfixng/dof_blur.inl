// -----------------------------------------------------------------------------
// Additional Depth-of-Field blur
// -----------------------------------------------------------------------------
//
// Original DPFix optionally softened Deadly Premonition's DoF buffer after the
// game's own blur pass. DPFix-NG keeps that concept, but uses a lightweight
// render-target downsample/upsample filter instead of importing the old effect
// framework. The game's own DoF selection/focus logic remains untouched.
//
// The injection point mirrors the original DPFix observation: when a viable
// DoF target has been the currently-bound RT0 twice in direct succession, the
// second occurrence is the post-blur 448x252 (or scaled equivalent) target that
// is about to be consumed by the final tone-map/resolve pass.

static std::mutex g_dofBlurMutex;
static IDirect3DDevice9* g_dofBlurScratchDevice = nullptr;
static IDirect3DSurface9* g_dofBlurScratchSurface = nullptr;
static UINT g_dofBlurScratchWidth = 0;
static UINT g_dofBlurScratchHeight = 0;
static D3DFORMAT g_dofBlurScratchFormat = D3DFMT_UNKNOWN;
static thread_local UINT g_dofBlurConsecutiveTargets = 0;
static std::atomic<unsigned long long> g_dofBlurApplications{ 0 };
static std::atomic<unsigned long long> g_dofBlurFailures{ 0 };

static void ReleaseDofBlurScratchLocked()
{
    if (g_dofBlurScratchSurface != nullptr)
    {
        g_dofBlurScratchSurface->Release();
        g_dofBlurScratchSurface = nullptr;
    }

    g_dofBlurScratchDevice = nullptr;
    g_dofBlurScratchWidth = 0;
    g_dofBlurScratchHeight = 0;
    g_dofBlurScratchFormat = D3DFMT_UNKNOWN;
}

static UINT DofBlurScratchDimension(UINT value)
{
    // A 3/4-size linear downsample followed by an upsample is deliberately
    // gentler than a half-resolution box. Repeating it twice gives a useful
    // stronger setting while preserving the game's original DoF character.
    return std::max<UINT>(1u, (value * 3u + 2u) / 4u);
}

static bool IsAdditionalDofBlurTarget(const D3DSURFACE_DESC& desc)
{
    if ((desc.Usage & D3DUSAGE_RENDERTARGET) == 0)
        return false;

    // The top of DP's HDR pyramid / DoF chain is the floating-point 448x252
    // target. The A8R8G8B8 target at the same size is dual-scene storage and
    // must not be touched.
    if (desc.Format != D3DFMT_A16B16G16R16F)
        return false;

    UINT expectedWidth = 448;
    UINT expectedHeight = 252;

    if (g_config.improveDofResolution)
    {
        expectedWidth = ScaleFromBaseWidth(expectedWidth);
        expectedHeight = ScaleFromBaseHeight(expectedHeight);
    }

    return desc.Width == expectedWidth && desc.Height == expectedHeight;
}

static bool EnsureDofBlurScratchLocked(
    IDirect3DDevice9* device,
    const D3DSURFACE_DESC& targetDesc)
{
    if (device == nullptr || g_originalCreateRenderTarget == nullptr)
        return false;

    const UINT scratchWidth = DofBlurScratchDimension(targetDesc.Width);
    const UINT scratchHeight = DofBlurScratchDimension(targetDesc.Height);

    if (g_dofBlurScratchSurface != nullptr &&
        g_dofBlurScratchDevice == device &&
        g_dofBlurScratchWidth == scratchWidth &&
        g_dofBlurScratchHeight == scratchHeight &&
        g_dofBlurScratchFormat == targetDesc.Format)
    {
        return true;
    }

    ReleaseDofBlurScratchLocked();

    IDirect3DSurface9* scratch = nullptr;
    const HRESULT result = g_originalCreateRenderTarget(
        device,
        scratchWidth,
        scratchHeight,
        targetDesc.Format,
        D3DMULTISAMPLE_NONE,
        0,
        FALSE,
        &scratch,
        nullptr);

    if (FAILED(result) || scratch == nullptr)
    {
        char text[320] = {};
        sprintf_s(
            text,
            "[DoF] WARNING: Additional blur scratch allocation failed for %u x %u, hr=0x%08X.\n",
            scratchWidth,
            scratchHeight,
            static_cast<unsigned>(result));
        AppendLog(text);
        return false;
    }

    g_dofBlurScratchDevice = device;
    g_dofBlurScratchSurface = scratch;
    g_dofBlurScratchWidth = scratchWidth;
    g_dofBlurScratchHeight = scratchHeight;
    g_dofBlurScratchFormat = targetDesc.Format;

    char text[320] = {};
    sprintf_s(
        text,
        "[DoF] Additional blur scratch ready: %u x %u -> %u x %u, format=0x%08X.\n",
        targetDesc.Width,
        targetDesc.Height,
        scratchWidth,
        scratchHeight,
        static_cast<unsigned>(targetDesc.Format));
    AppendLog(text);

    return true;
}

static bool ApplyAdditionalDofBlur(
    IDirect3DDevice9* device,
    IDirect3DSurface9* target,
    const D3DSURFACE_DESC& targetDesc)
{
    const UINT amount = g_config.additionalDofBlur;
    if (amount == 0 || device == nullptr || target == nullptr)
        return true;

    if (g_originalStretchRect == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(g_dofBlurMutex);

    if (!EnsureDofBlurScratchLocked(device, targetDesc))
    {
        g_dofBlurFailures.fetch_add(1, std::memory_order_relaxed);
        return false;
    }

    for (UINT pass = 0; pass < amount; ++pass)
    {
        HRESULT result = g_originalStretchRect(
            device,
            target,
            nullptr,
            g_dofBlurScratchSurface,
            nullptr,
            D3DTEXF_LINEAR);

        if (FAILED(result))
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[DoF] WARNING: Additional blur downsample failed on pass %u, hr=0x%08X.\n",
                pass + 1,
                static_cast<unsigned>(result));
            AppendLog(text);
            g_dofBlurFailures.fetch_add(1, std::memory_order_relaxed);
            return false;
        }

        result = g_originalStretchRect(
            device,
            g_dofBlurScratchSurface,
            nullptr,
            target,
            nullptr,
            D3DTEXF_LINEAR);

        if (FAILED(result))
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[DoF] WARNING: Additional blur upsample failed on pass %u, hr=0x%08X.\n",
                pass + 1,
                static_cast<unsigned>(result));
            AppendLog(text);
            g_dofBlurFailures.fetch_add(1, std::memory_order_relaxed);
            return false;
        }
    }

    const unsigned long long application =
        g_dofBlurApplications.fetch_add(1, std::memory_order_relaxed) + 1;

    // Keep normal logs quiet. The first successful application is enough to
    // prove the injection point and effective target size during testing.
    if (application == 1)
    {
        char text[320] = {};
        sprintf_s(
            text,
            "[DoF] Additional blur active: amount=%u, target=%u x %u, scratch=%u x %u.\n",
            amount,
            targetDesc.Width,
            targetDesc.Height,
            g_dofBlurScratchWidth,
            g_dofBlurScratchHeight);
        AppendLog(text);
    }

    return true;
}

static void ObserveAndApplyAdditionalDofBlur(IDirect3DDevice9* device)
{
    if (g_config.additionalDofBlur == 0 || device == nullptr)
    {
        g_dofBlurConsecutiveTargets = 0;
        return;
    }

    IDirect3DSurface9* currentTarget = nullptr;
    if (FAILED(device->GetRenderTarget(0, &currentTarget)) || currentTarget == nullptr)
    {
        g_dofBlurConsecutiveTargets = 0;
        return;
    }

    D3DSURFACE_DESC desc = {};
    const HRESULT descResult = currentTarget->GetDesc(&desc);
    if (FAILED(descResult))
    {
        currentTarget->Release();
        g_dofBlurConsecutiveTargets = 0;
        return;
    }

    if (IsAdditionalDofBlurTarget(desc))
    {
        ++g_dofBlurConsecutiveTargets;

        if (g_dofBlurConsecutiveTargets == 2)
            ApplyAdditionalDofBlur(device, currentTarget, desc);
    }
    else
    {
        g_dofBlurConsecutiveTargets = 0;
    }

    currentTarget->Release();
}

bool SetAdditionalDofBlurLive(UINT amount)
{
    if (amount > 2)
        return false;

    if (g_config.additionalDofBlur == amount)
        return true;

    g_config.additionalDofBlur = amount;
    g_dofBlurConsecutiveTargets = 0;

    if (amount == 0)
    {
        std::lock_guard<std::mutex> lock(g_dofBlurMutex);
        ReleaseDofBlurScratchLocked();
    }

    char text[192] = {};
    sprintf_s(
        text,
        "[DoF] Additional blur changed live: amount=%u.\n",
        amount);
    AppendLog(text);
    return true;
}

static void OnAdditionalDofBlurSettingsApplied()
{
    g_dofBlurConsecutiveTargets = 0;

    if (g_config.additionalDofBlur != 0)
        return;

    std::lock_guard<std::mutex> lock(g_dofBlurMutex);
    ReleaseDofBlurScratchLocked();
}
