// -----------------------------------------------------------------------------
// Texture filtering override
// -----------------------------------------------------------------------------
//
// This is intentionally more conservative than original DPFix filteringOverride=2.
// ZachFix never forces anisotropic magnification and avoids render targets,
// depth resources, dynamic textures and intentionally point-sampled assets.
//
// Original mode is a near-zero-cost passthrough. Active modes track only the
// 16 pixel samplers used by Direct3D 9 fixed/pixel-shader texture stages.

static constexpr DWORD kTextureFilteringPixelSamplerCount = 16;

struct TextureFilteringSamplerState
{
    DWORD rawMinFilter = D3DTEXF_POINT;
    DWORD rawMipFilter = D3DTEXF_NONE;
    DWORD rawMaxAnisotropy = 1;

    DWORD appliedMinFilter = D3DTEXF_POINT;
    DWORD appliedMipFilter = D3DTEXF_NONE;
    DWORD appliedMaxAnisotropy = 1;

    IDirect3DBaseTexture9* boundTexture = nullptr; // non-owning; device owns the current bind
    bool regular2DTexture = false;
    bool mipmapped = false;
    bool captured = false;
};

static std::array<TextureFilteringSamplerState, kTextureFilteringPixelSamplerCount>
    g_textureFilteringSamplers{};
static std::mutex g_textureFilteringMutex;
static UINT g_textureFilteringDeviceMaxAnisotropy = 1;
static bool g_textureFilteringMinAnisotropySupported = false;
static bool g_textureFilteringInitialized = false;
static TextureFilteringMode g_textureFilteringAppliedMode = TextureFilteringMode::Original;
static UINT g_textureFilteringAppliedMaxAnisotropy = 16;

static const char* TextureFilteringModeName(TextureFilteringMode mode)
{
    switch (mode)
    {
    case TextureFilteringMode::Bilinear: return "Bilinear";
    case TextureFilteringMode::Anisotropic: return "Anisotropic";
    default: return "Original";
    }
}

static bool IsTextureFilteringGameCall(void* returnAddress)
{
    if (!g_mainExeInfoValid && !InitializeMainExeInfo())
        return false;

    const uintptr_t address = reinterpret_cast<uintptr_t>(returnAddress);
    return address >= g_mainExeBase &&
           address < g_mainExeBase + g_mainExeSize;
}

static bool IsTextureFilteringDepthFormat(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_D16:
    case D3DFMT_D15S1:
    case D3DFMT_D24X8:
    case D3DFMT_D24S8:
    case D3DFMT_D24X4S4:
    case D3DFMT_D32:
        return true;
    default:
        return false;
    }
}

static void ClassifyTextureForFiltering(
    IDirect3DBaseTexture9* texture,
    bool& regular2DTexture,
    bool& mipmapped)
{
    regular2DTexture = false;
    mipmapped = false;

    if (texture == nullptr || texture->GetType() != D3DRTYPE_TEXTURE)
        return;

    IDirect3DTexture9* texture2D = static_cast<IDirect3DTexture9*>(texture);
    D3DSURFACE_DESC desc = {};
    if (FAILED(texture2D->GetLevelDesc(0, &desc)))
        return;

    const DWORD excludedUsage =
        D3DUSAGE_RENDERTARGET |
        D3DUSAGE_DEPTHSTENCIL |
        D3DUSAGE_DYNAMIC;

    if ((desc.Usage & excludedUsage) != 0 ||
        IsTextureFilteringDepthFormat(desc.Format))
    {
        return;
    }

    regular2DTexture = true;
    mipmapped = texture2D->GetLevelCount() > 1;
}

static bool IsAnisotropicCandidate(const TextureFilteringSamplerState& state)
{
    if (!state.regular2DTexture ||
        !state.mipmapped ||
        !g_textureFilteringMinAnisotropySupported)
    {
        return false;
    }

    // Preserve intentional point sampling. The smart AF path only upgrades
    // textures for which the game already requested smooth minification.
    return state.rawMinFilter == D3DTEXF_LINEAR ||
           state.rawMinFilter == D3DTEXF_ANISOTROPIC;
}

static DWORD RequestedMaxAnisotropy()
{
    return std::max<DWORD>(
        1,
        std::min<DWORD>(
            static_cast<DWORD>(g_config.maxAnisotropy),
            static_cast<DWORD>(g_textureFilteringDeviceMaxAnisotropy)));
}

static DWORD DesiredMinFilter(const TextureFilteringSamplerState& state)
{
    if (!state.regular2DTexture)
        return state.rawMinFilter;

    if (g_config.textureFilteringMode == TextureFilteringMode::Bilinear)
    {
        if (state.rawMinFilter == D3DTEXF_POINT ||
            state.rawMinFilter == D3DTEXF_NONE)
        {
            return D3DTEXF_LINEAR;
        }
        return state.rawMinFilter;
    }

    if (g_config.textureFilteringMode == TextureFilteringMode::Anisotropic &&
        IsAnisotropicCandidate(state))
    {
        return D3DTEXF_ANISOTROPIC;
    }

    return state.rawMinFilter;
}

static DWORD DesiredMipFilter(const TextureFilteringSamplerState& state)
{
    if (g_config.textureFilteringMode == TextureFilteringMode::Bilinear &&
        state.regular2DTexture)
    {
        if (state.rawMipFilter == D3DTEXF_POINT ||
            state.rawMipFilter == D3DTEXF_NONE)
        {
            return D3DTEXF_LINEAR;
        }
    }

    // Smart anisotropic mode deliberately preserves the game's mip policy.
    return state.rawMipFilter;
}

static DWORD DesiredMaxAnisotropy(const TextureFilteringSamplerState& state)
{
    if (g_config.textureFilteringMode == TextureFilteringMode::Anisotropic &&
        IsAnisotropicCandidate(state))
    {
        return RequestedMaxAnisotropy();
    }

    return state.rawMaxAnisotropy;
}

static void ApplyTextureFilteringStageLocked(
    IDirect3DDevice9* device,
    DWORD sampler)
{
    if (device == nullptr ||
        g_originalSetSamplerState == nullptr ||
        sampler >= kTextureFilteringPixelSamplerCount)
    {
        return;
    }

    TextureFilteringSamplerState& state = g_textureFilteringSamplers[sampler];
    if (!state.captured)
        return;

    const DWORD minFilter = DesiredMinFilter(state);
    const DWORD mipFilter = DesiredMipFilter(state);
    const DWORD maxAnisotropy = DesiredMaxAnisotropy(state);

    if (minFilter != state.appliedMinFilter)
    {
        if (SUCCEEDED(g_originalSetSamplerState(
                device, sampler, D3DSAMP_MINFILTER, minFilter)))
        {
            state.appliedMinFilter = minFilter;
        }
    }

    if (mipFilter != state.appliedMipFilter)
    {
        if (SUCCEEDED(g_originalSetSamplerState(
                device, sampler, D3DSAMP_MIPFILTER, mipFilter)))
        {
            state.appliedMipFilter = mipFilter;
        }
    }

    if (maxAnisotropy != state.appliedMaxAnisotropy)
    {
        if (SUCCEEDED(g_originalSetSamplerState(
                device, sampler, D3DSAMP_MAXANISOTROPY, maxAnisotropy)))
        {
            state.appliedMaxAnisotropy = maxAnisotropy;
        }
    }
}

static void CaptureTextureFilteringStageLocked(
    IDirect3DDevice9* device,
    DWORD sampler,
    bool captureTexture)
{
    if (device == nullptr || sampler >= kTextureFilteringPixelSamplerCount)
        return;

    TextureFilteringSamplerState& state = g_textureFilteringSamplers[sampler];

    DWORD value = 0;
    if (SUCCEEDED(device->GetSamplerState(sampler, D3DSAMP_MINFILTER, &value)))
    {
        state.rawMinFilter = value;
        state.appliedMinFilter = value;
    }
    if (SUCCEEDED(device->GetSamplerState(sampler, D3DSAMP_MIPFILTER, &value)))
    {
        state.rawMipFilter = value;
        state.appliedMipFilter = value;
    }
    if (SUCCEEDED(device->GetSamplerState(sampler, D3DSAMP_MAXANISOTROPY, &value)))
    {
        state.rawMaxAnisotropy = value;
        state.appliedMaxAnisotropy = value;
    }

    state.captured = true;

    if (!captureTexture)
        return;

    IDirect3DBaseTexture9* texture = nullptr;
    if (SUCCEEDED(device->GetTexture(sampler, &texture)))
    {
        state.boundTexture = texture;
        ClassifyTextureForFiltering(
            texture,
            state.regular2DTexture,
            state.mipmapped);
        if (texture != nullptr)
            texture->Release();
    }
    else
    {
        state.boundTexture = nullptr;
        state.regular2DTexture = false;
        state.mipmapped = false;
    }
}

static void InitializeTextureFiltering(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    std::lock_guard<std::mutex> lock(g_textureFilteringMutex);
    if (g_textureFilteringInitialized)
        return;

    D3DCAPS9 caps = {};
    if (SUCCEEDED(device->GetDeviceCaps(&caps)))
    {
        g_textureFilteringDeviceMaxAnisotropy =
            std::max<UINT>(1, caps.MaxAnisotropy);
        g_textureFilteringMinAnisotropySupported =
            (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) != 0;
    }

    for (DWORD sampler = 0; sampler < kTextureFilteringPixelSamplerCount; ++sampler)
        CaptureTextureFilteringStageLocked(device, sampler, true);

    g_textureFilteringAppliedMode = g_config.textureFilteringMode;
    g_textureFilteringAppliedMaxAnisotropy = g_config.maxAnisotropy;
    g_textureFilteringInitialized = true;

    char text[384] = {};
    sprintf_s(
        text,
        "[Filtering] Initialized: Mode=%s, requested AF=%ux, device max=%ux, MIN anisotropic=%s.\\n",
        TextureFilteringModeName(g_config.textureFilteringMode),
        g_config.maxAnisotropy,
        g_textureFilteringDeviceMaxAnisotropy,
        g_textureFilteringMinAnisotropySupported ? "yes" : "no");
    AppendLog(text);

    if (g_config.textureFilteringMode != TextureFilteringMode::Original)
    {
        for (DWORD sampler = 0; sampler < kTextureFilteringPixelSamplerCount; ++sampler)
            ApplyTextureFilteringStageLocked(device, sampler);
    }
}

static void ReapplyTextureFiltering(IDirect3DDevice9* device)
{
    if (device == nullptr || !g_textureFilteringInitialized)
        return;

    std::lock_guard<std::mutex> lock(g_textureFilteringMutex);

    const TextureFilteringMode previousMode = g_textureFilteringAppliedMode;
    const UINT previousMax = g_textureFilteringAppliedMaxAnisotropy;

    if (g_config.textureFilteringMode != TextureFilteringMode::Original &&
        previousMode == TextureFilteringMode::Original)
    {
        // While filtering is off there is intentionally no per-bind tracking.
        // Snapshot the exact game-visible sampler/texture state at Apply time.
        for (DWORD sampler = 0; sampler < kTextureFilteringPixelSamplerCount; ++sampler)
            CaptureTextureFilteringStageLocked(device, sampler, true);
    }

    for (DWORD sampler = 0; sampler < kTextureFilteringPixelSamplerCount; ++sampler)
        ApplyTextureFilteringStageLocked(device, sampler);

    g_textureFilteringAppliedMode = g_config.textureFilteringMode;
    g_textureFilteringAppliedMaxAnisotropy = g_config.maxAnisotropy;

    if (previousMode != g_textureFilteringAppliedMode ||
        previousMax != g_textureFilteringAppliedMaxAnisotropy)
    {
        char text[384] = {};
        sprintf_s(
            text,
            "[Filtering] Live apply: %s -> %s, requested AF %ux -> %ux, effective cap=%ux.\\n",
            TextureFilteringModeName(previousMode),
            TextureFilteringModeName(g_textureFilteringAppliedMode),
            previousMax,
            g_textureFilteringAppliedMaxAnisotropy,
            RequestedMaxAnisotropy());
        AppendLog(text);
    }
}

static void NotifyTextureFilteringTextureBound(
    IDirect3DDevice9* device,
    DWORD stage,
    IDirect3DBaseTexture9* texture)
{
    if (device == nullptr ||
        stage >= kTextureFilteringPixelSamplerCount ||
        g_textureFilteringAppliedMode == TextureFilteringMode::Original ||
        !g_textureFilteringInitialized)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(g_textureFilteringMutex);
    TextureFilteringSamplerState& state = g_textureFilteringSamplers[stage];

    if (state.boundTexture == texture)
        return;

    state.boundTexture = texture;
    ClassifyTextureForFiltering(
        texture,
        state.regular2DTexture,
        state.mipmapped);
    ApplyTextureFilteringStageLocked(device, stage);
}

static HRESULT WINAPI HookSetSamplerState(
    IDirect3DDevice9* self,
    DWORD sampler,
    D3DSAMPLERSTATETYPE type,
    DWORD value)
{
    if (g_originalSetSamplerState == nullptr)
        return D3DERR_INVALIDCALL;

    // Original mode is deliberately almost free and does not track texture
    // state. This is important because filtering is an optional enhancement.
    if (g_textureFilteringAppliedMode == TextureFilteringMode::Original ||
        !g_textureFilteringInitialized ||
        sampler >= kTextureFilteringPixelSamplerCount ||
        !IsTextureFilteringGameCall(_ReturnAddress()))
    {
        return g_originalSetSamplerState(self, sampler, type, value);
    }

    if (type != D3DSAMP_MINFILTER &&
        type != D3DSAMP_MIPFILTER &&
        type != D3DSAMP_MAXANISOTROPY)
    {
        return g_originalSetSamplerState(self, sampler, type, value);
    }

    std::lock_guard<std::mutex> lock(g_textureFilteringMutex);
    TextureFilteringSamplerState& state = g_textureFilteringSamplers[sampler];

    if (!state.captured)
        CaptureTextureFilteringStageLocked(self, sampler, false);

    DWORD desired = value;
    switch (type)
    {
    case D3DSAMP_MINFILTER:
        state.rawMinFilter = value;
        desired = DesiredMinFilter(state);
        break;
    case D3DSAMP_MIPFILTER:
        state.rawMipFilter = value;
        desired = DesiredMipFilter(state);
        break;
    case D3DSAMP_MAXANISOTROPY:
        state.rawMaxAnisotropy = value;
        desired = DesiredMaxAnisotropy(state);
        break;
    default:
        break;
    }

    const HRESULT result =
        g_originalSetSamplerState(self, sampler, type, desired);

    if (SUCCEEDED(result))
    {
        switch (type)
        {
        case D3DSAMP_MINFILTER:
            state.appliedMinFilter = desired;
            // MINFILTER eligibility controls whether MAXANISOTROPY matters.
            // Keep the companion state synchronized when the game switches
            // between point and smooth minification.
            {
                const DWORD desiredMax = DesiredMaxAnisotropy(state);
                if (desiredMax != state.appliedMaxAnisotropy &&
                    SUCCEEDED(g_originalSetSamplerState(
                        self,
                        sampler,
                        D3DSAMP_MAXANISOTROPY,
                        desiredMax)))
                {
                    state.appliedMaxAnisotropy = desiredMax;
                }
            }
            break;
        case D3DSAMP_MIPFILTER:
            state.appliedMipFilter = desired;
            break;
        case D3DSAMP_MAXANISOTROPY:
            state.appliedMaxAnisotropy = desired;
            break;
        default:
            break;
        }
    }

    return result;
}
