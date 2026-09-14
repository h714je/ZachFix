// -----------------------------------------------------------------------------
// ZachFix PostFX NG: modern bloom pyramid v0
// -----------------------------------------------------------------------------

namespace
{
std::mutex g_postFxBloomMutex;
std::atomic_uint g_postFxBloomMode{
    static_cast<UINT>(PostFxBloomMode::Legacy)
};
std::atomic<float> g_postFxBloomThresholdEv{ 0.0f };
std::atomic<float> g_postFxBloomSoftKnee{ 0.50f };
std::atomic<float> g_postFxBloomIntensity{ 0.35f };
std::atomic<float> g_postFxBloomScatter{ 0.70f };
std::atomic_uint g_postFxBloomMaxLevels{ 5 };

std::atomic_bool g_postFxBloomShaderReady{ false };
std::atomic_bool g_postFxBloomActiveThisFrame{ false };
std::atomic_bool g_postFxBloomUsedAo{ false };
std::atomic_bool g_postFxBloomFallbackToLegacy{ false };
std::atomic_uint g_postFxBloomSourceWidth{ 0 };
std::atomic_uint g_postFxBloomSourceHeight{ 0 };
std::atomic_uint g_postFxBloomBaseWidth{ 0 };
std::atomic_uint g_postFxBloomBaseHeight{ 0 };
std::atomic_uint g_postFxBloomLevels{ 0 };
std::atomic_ullong g_postFxBloomPreparedFrame{ 0 };

IDirect3DDevice9* g_postFxBloomShaderOwner = nullptr; // borrowed
IDirect3DPixelShader9* g_postFxBloomPrefilterShader = nullptr;
IDirect3DPixelShader9* g_postFxBloomDownsampleShader = nullptr;
IDirect3DPixelShader9* g_postFxBloomUpsampleShader = nullptr;

static const char kPostFxBloomShaderSource[] = R"HLSL(
sampler2D g_tInput : register(s0);
sampler2D g_tAO    : register(s1);

// Prefilter: c0 = { filterStepX, filterStepY, thresholdLinear, kneeLinear }
//            c1 = { applyAO, unused, unused, unused }
// Downsample: c0 = { sourceTexelX, sourceTexelY, unused, unused }
// Upsample:   c0 = { sourceTexelX, sourceTexelY, scatter, unused }
float4 g_zfBloom0 : register(c0);
float4 g_zfBloom1 : register(c1);
static const float3 kLuma = float3(0.2126, 0.7152, 0.0722);

float3 Downsample13(float2 uv, float2 t)
{
    float3 A = max(tex2D(g_tInput, uv + t * float2(-2.0, -2.0)).rgb, 0.0);
    float3 B = max(tex2D(g_tInput, uv + t * float2( 0.0, -2.0)).rgb, 0.0);
    float3 C = max(tex2D(g_tInput, uv + t * float2( 2.0, -2.0)).rgb, 0.0);
    float3 D = max(tex2D(g_tInput, uv + t * float2(-2.0,  0.0)).rgb, 0.0);
    float3 E = max(tex2D(g_tInput, uv).rgb, 0.0);
    float3 F = max(tex2D(g_tInput, uv + t * float2( 2.0,  0.0)).rgb, 0.0);
    float3 G = max(tex2D(g_tInput, uv + t * float2(-2.0,  2.0)).rgb, 0.0);
    float3 H = max(tex2D(g_tInput, uv + t * float2( 0.0,  2.0)).rgb, 0.0);
    float3 I = max(tex2D(g_tInput, uv + t * float2( 2.0,  2.0)).rgb, 0.0);
    float3 J = max(tex2D(g_tInput, uv + t * float2(-1.0, -1.0)).rgb, 0.0);
    float3 K = max(tex2D(g_tInput, uv + t * float2( 1.0, -1.0)).rgb, 0.0);
    float3 L = max(tex2D(g_tInput, uv + t * float2(-1.0,  1.0)).rgb, 0.0);
    float3 M = max(tex2D(g_tInput, uv + t * float2( 1.0,  1.0)).rgb, 0.0);

    float3 result = (J + K + L + M) * 0.5;
    result += (A + B + D + E) * 0.125;
    result += (B + C + E + F) * 0.125;
    result += (D + E + G + H) * 0.125;
    result += (E + F + H + I) * 0.125;
    return result * 0.25;
}

float3 ApplySoftKnee(float3 color)
{
    float brightness = max(dot(color, kLuma), 0.0);
    float threshold = max(g_zfBloom0.z, 0.0);
    float knee = max(g_zfBloom0.w, 1e-5);

    float soft = brightness - threshold + knee;
    soft = clamp(soft, 0.0, 2.0 * knee);
    soft = soft * soft / (4.0 * knee + 1e-5);

    float contribution = max(brightness - threshold, soft);
    contribution /= max(brightness, 1e-5);
    return color * saturate(contribution);
}

float4 PrefilterMain(float2 uv : TEXCOORD0) : COLOR0
{
    float3 color = Downsample13(uv, g_zfBloom0.xy);
    float ao = saturate(tex2D(g_tAO, uv).r);
    color *= lerp(1.0, ao, saturate(g_zfBloom1.x));
    return float4(ApplySoftKnee(color), 0.0);
}

float4 DownsampleMain(float2 uv : TEXCOORD0) : COLOR0
{
    return float4(Downsample13(uv, g_zfBloom0.xy), 0.0);
}

float4 UpsampleMain(float2 uv : TEXCOORD0) : COLOR0
{
    float2 t = g_zfBloom0.xy;
    float3 s = 0.0;
    s += tex2D(g_tInput, uv + t * float2(-1.0, -1.0)).rgb;
    s += tex2D(g_tInput, uv + t * float2( 1.0, -1.0)).rgb;
    s += tex2D(g_tInput, uv + t * float2(-1.0,  1.0)).rgb;
    s += tex2D(g_tInput, uv + t * float2( 1.0,  1.0)).rgb;
    s += tex2D(g_tInput, uv + t * float2(-1.0,  0.0)).rgb * 2.0;
    s += tex2D(g_tInput, uv + t * float2( 1.0,  0.0)).rgb * 2.0;
    s += tex2D(g_tInput, uv + t * float2( 0.0, -1.0)).rgb * 2.0;
    s += tex2D(g_tInput, uv + t * float2( 0.0,  1.0)).rgb * 2.0;
    s += tex2D(g_tInput, uv).rgb * 4.0;
    s *= (1.0 / 16.0) * saturate(g_zfBloom0.z);
    return float4(max(s, 0.0), 0.0);
}
)HLSL";

float ClampBloomFloat(float value, float minimum, float maximum)
{
    if (!std::isfinite(value))
        return minimum;
    return std::max(minimum, std::min(maximum, value));
}

void ReleasePostFxBloomShadersUnlocked()
{
    if (g_postFxBloomPrefilterShader != nullptr)
    {
        g_postFxBloomPrefilterShader->Release();
        g_postFxBloomPrefilterShader = nullptr;
    }
    if (g_postFxBloomDownsampleShader != nullptr)
    {
        g_postFxBloomDownsampleShader->Release();
        g_postFxBloomDownsampleShader = nullptr;
    }
    if (g_postFxBloomUpsampleShader != nullptr)
    {
        g_postFxBloomUpsampleShader->Release();
        g_postFxBloomUpsampleShader = nullptr;
    }
    g_postFxBloomShaderOwner = nullptr;
    g_postFxBloomShaderReady.store(false, std::memory_order_relaxed);
}

bool EnsurePostFxBloomShadersUnlocked(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    if (g_postFxBloomShaderOwner != nullptr &&
        g_postFxBloomShaderOwner != device)
    {
        ReleasePostFxBloomShadersUnlocked();
    }
    g_postFxBloomShaderOwner = device;

    if (g_postFxBloomPrefilterShader != nullptr &&
        g_postFxBloomDownsampleShader != nullptr &&
        g_postFxBloomUpsampleShader != nullptr)
    {
        g_postFxBloomShaderReady.store(true, std::memory_order_relaxed);
        return true;
    }

    AppendLog("[PostFX][Bloom] Compiling Bloom NG v0 prefilter shader.\n");
    if (!CompilePostFxPixelShader(
            device,
            kPostFxBloomShaderSource,
            "PrefilterMain",
            "Bloom NG v0 prefilter",
            &g_postFxBloomPrefilterShader))
    {
        ReleasePostFxBloomShadersUnlocked();
        return false;
    }

    AppendLog("[PostFX][Bloom] Prefilter compiled; compiling downsample shader.\n");
    if (!CompilePostFxPixelShader(
            device,
            kPostFxBloomShaderSource,
            "DownsampleMain",
            "Bloom NG v0 downsample",
            &g_postFxBloomDownsampleShader))
    {
        ReleasePostFxBloomShadersUnlocked();
        return false;
    }

    AppendLog("[PostFX][Bloom] Downsample compiled; compiling tent upsample shader.\n");
    if (!CompilePostFxPixelShader(
            device,
            kPostFxBloomShaderSource,
            "UpsampleMain",
            "Bloom NG v0 upsample",
            &g_postFxBloomUpsampleShader))
    {
        ReleasePostFxBloomShadersUnlocked();
        return false;
    }

    g_postFxBloomShaderReady.store(true, std::memory_order_relaxed);
    AppendLog("[PostFX][Bloom] Bloom NG v0 shaders compiled.\n");
    return true;
}

PostFxTargetSlot BloomSlot(UINT index)
{
    const UINT first = static_cast<UINT>(PostFxTargetSlot::Bloom0);
    return static_cast<PostFxTargetSlot>(first + std::min<UINT>(index, 5u));
}
} // namespace

PostFxBloomSettings GetPostFxBloomSettings()
{
    PostFxBloomSettings settings{};
    settings.mode = static_cast<PostFxBloomMode>(
        g_postFxBloomMode.load(std::memory_order_relaxed));
    settings.thresholdEv = g_postFxBloomThresholdEv.load(std::memory_order_relaxed);
    settings.softKnee = g_postFxBloomSoftKnee.load(std::memory_order_relaxed);
    settings.intensity = g_postFxBloomIntensity.load(std::memory_order_relaxed);
    settings.scatter = g_postFxBloomScatter.load(std::memory_order_relaxed);
    settings.maxLevels = g_postFxBloomMaxLevels.load(std::memory_order_relaxed);
    return settings;
}

void SetPostFxBloomMode(PostFxBloomMode mode)
{
    const UINT sanitized = std::min<UINT>(
        static_cast<UINT>(mode),
        static_cast<UINT>(PostFxBloomMode::ShowBloom));
    g_postFxBloomMode.store(sanitized, std::memory_order_relaxed);
}

void SetPostFxBloomThresholdEv(float ev)
{
    g_postFxBloomThresholdEv.store(
        ClampBloomFloat(ev, -6.0f, 12.0f), std::memory_order_relaxed);
}

void SetPostFxBloomSoftKnee(float softKnee)
{
    g_postFxBloomSoftKnee.store(
        ClampBloomFloat(softKnee, 0.01f, 1.0f), std::memory_order_relaxed);
}

void SetPostFxBloomIntensity(float intensity)
{
    g_postFxBloomIntensity.store(
        ClampBloomFloat(intensity, 0.0f, 4.0f), std::memory_order_relaxed);
}

void SetPostFxBloomScatter(float scatter)
{
    g_postFxBloomScatter.store(
        ClampBloomFloat(scatter, 0.0f, 1.0f), std::memory_order_relaxed);
}

void SetPostFxBloomMaxLevels(UINT levels)
{
    g_postFxBloomMaxLevels.store(
        std::max<UINT>(2, std::min<UINT>(6, levels)),
        std::memory_order_relaxed);
}

void ResetPostFxBloomSettings()
{
    g_postFxBloomMode.store(
        static_cast<UINT>(PostFxBloomMode::Legacy), std::memory_order_relaxed);
    g_postFxBloomThresholdEv.store(0.0f, std::memory_order_relaxed);
    g_postFxBloomSoftKnee.store(0.50f, std::memory_order_relaxed);
    g_postFxBloomIntensity.store(0.35f, std::memory_order_relaxed);
    g_postFxBloomScatter.store(0.70f, std::memory_order_relaxed);
    g_postFxBloomMaxLevels.store(5, std::memory_order_relaxed);
}

PostFxBloomStats GetPostFxBloomStats()
{
    PostFxBloomStats stats{};
    stats.shaderReady = g_postFxBloomShaderReady.load(std::memory_order_relaxed);
    stats.activeThisFrame = g_postFxBloomActiveThisFrame.load(std::memory_order_relaxed);
    stats.usedAo = g_postFxBloomUsedAo.load(std::memory_order_relaxed);
    stats.fallbackToLegacy = g_postFxBloomFallbackToLegacy.load(std::memory_order_relaxed);
    stats.sourceWidth = g_postFxBloomSourceWidth.load(std::memory_order_relaxed);
    stats.sourceHeight = g_postFxBloomSourceHeight.load(std::memory_order_relaxed);
    stats.baseWidth = g_postFxBloomBaseWidth.load(std::memory_order_relaxed);
    stats.baseHeight = g_postFxBloomBaseHeight.load(std::memory_order_relaxed);
    stats.levels = g_postFxBloomLevels.load(std::memory_order_relaxed);
    stats.preparedFrame = g_postFxBloomPreparedFrame.load(std::memory_order_relaxed);
    return stats;
}

bool ShouldUsePostFxBloomReplacement()
{
    return static_cast<PostFxBloomMode>(
        g_postFxBloomMode.load(std::memory_order_relaxed)) !=
        PostFxBloomMode::Legacy;
}

bool PreparePostFxBloom(
    IDirect3DDevice9* device,
    IDirect3DTexture9* filteredAo,
    IDirect3DTexture9** bloomTexture)
{
    if (bloomTexture != nullptr)
        *bloomTexture = nullptr;

    g_postFxBloomActiveThisFrame.store(false, std::memory_order_relaxed);
    g_postFxBloomUsedAo.store(false, std::memory_order_relaxed);
    g_postFxBloomFallbackToLegacy.store(false, std::memory_order_relaxed);

    if (device == nullptr || bloomTexture == nullptr)
        return false;

    const PostFxBloomSettings settings = GetPostFxBloomSettings();
    if (settings.mode == PostFxBloomMode::Legacy)
        return false;

    {
        std::lock_guard<std::mutex> lock(g_postFxBloomMutex);
        if (!EnsurePostFxBloomShadersUnlocked(device))
        {
            g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
            return false;
        }
    }

    IDirect3DBaseTexture9* baseHdr = nullptr;
    if (FAILED(device->GetTexture(0, &baseHdr)) || baseHdr == nullptr ||
        baseHdr->GetType() != D3DRTYPE_TEXTURE)
    {
        if (baseHdr != nullptr)
            baseHdr->Release();
        g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    IDirect3DTexture9* hdr = nullptr;
    const HRESULT hdrQuery = baseHdr->QueryInterface(
        __uuidof(IDirect3DTexture9), reinterpret_cast<void**>(&hdr));
    baseHdr->Release();
    if (FAILED(hdrQuery) || hdr == nullptr)
    {
        g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    D3DSURFACE_DESC hdrDesc = {};
    if (FAILED(hdr->GetLevelDesc(0, &hdrDesc)) ||
        hdrDesc.Width == 0 || hdrDesc.Height == 0)
    {
        hdr->Release();
        g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    IDirect3DSurface9* outputSurface = nullptr;
    D3DSURFACE_DESC outputDesc = {};
    const bool haveOutput =
        SUCCEEDED(device->GetRenderTarget(0, &outputSurface)) &&
        outputSurface != nullptr &&
        SUCCEEDED(outputSurface->GetDesc(&outputDesc));
    if (outputSurface != nullptr)
        outputSurface->Release();

    if (!haveOutput || outputDesc.Width == 0 || outputDesc.Height == 0)
    {
        hdr->Release();
        g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    UINT baseWidth = 0;
    UINT baseHeight = 0;
    GetPostFxScaledExtent(
        outputDesc.Width, outputDesc.Height, 2, &baseWidth, &baseHeight);

    PostFxTargetView pyramid[6] = {};
    UINT levelWidths[6] = {};
    UINT levelHeights[6] = {};
    UINT levelCount = 0;
    UINT width = baseWidth;
    UINT height = baseHeight;
    const UINT requestedLevels = std::max<UINT>(
        2, std::min<UINT>(6, settings.maxLevels));

    for (UINT i = 0; i < requestedLevels; ++i)
    {
        if (!EnsurePostFxTarget(
                device,
                BloomSlot(i),
                width,
                height,
                D3DFMT_A16B16G16R16F,
                &pyramid[i]))
        {
            hdr->Release();
            g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
            return false;
        }

        levelWidths[i] = width;
        levelHeights[i] = height;
        ++levelCount;

        if (width == 1 && height == 1)
            break;

        width = std::max<UINT>(1, (width + 1) / 2);
        height = std::max<UINT>(1, (height + 1) / 2);
    }

    PostFxStateBackup backup{};
    if (!BeginPostFxStateBackup(device, &backup))
    {
        hdr->Release();
        g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    const bool useAo = filteredAo != nullptr;
    const PostFxTextureBinding prefilterBindings[] =
    {
        { 0, hdr, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP },
        { 1, useAo ? filteredAo : hdr, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP }
    };

    const float thresholdLinear = std::pow(2.0f, settings.thresholdEv);
    const float kneeLinear = std::max(
        thresholdLinear * settings.softKnee,
        1e-4f);
    const float prefilterConstants[8] =
    {
        0.5f / static_cast<float>(std::max<UINT>(baseWidth, 1)),
        0.5f / static_cast<float>(std::max<UINT>(baseHeight, 1)),
        thresholdLinear,
        kneeLinear,
        useAo ? 1.0f : 0.0f,
        0.0f,
        0.0f,
        0.0f
    };

    bool ok = RunPostFxFullscreenPass(
        device,
        pyramid[0].surface,
        pyramid[0].width,
        pyramid[0].height,
        g_postFxBloomPrefilterShader,
        prefilterBindings,
        2,
        0,
        prefilterConstants,
        2,
        false,
        PostFxBlendMode::Opaque);

    for (UINT i = 1; ok && i < levelCount; ++i)
    {
        const PostFxTextureBinding binding =
        {
            0,
            pyramid[i - 1].texture,
            D3DTEXF_LINEAR,
            D3DTADDRESS_CLAMP
        };
        const float constants[4] =
        {
            1.0f / static_cast<float>(std::max<UINT>(levelWidths[i - 1], 1)),
            1.0f / static_cast<float>(std::max<UINT>(levelHeights[i - 1], 1)),
            0.0f,
            0.0f
        };
        ok = RunPostFxFullscreenPass(
            device,
            pyramid[i].surface,
            pyramid[i].width,
            pyramid[i].height,
            g_postFxBloomDownsampleShader,
            &binding,
            1,
            0,
            constants,
            1,
            false,
            PostFxBlendMode::Opaque);
    }

    for (UINT i = levelCount; ok && i > 1; --i)
    {
        const UINT child = i - 1;
        const UINT parent = i - 2;
        const PostFxTextureBinding binding =
        {
            0,
            pyramid[child].texture,
            D3DTEXF_LINEAR,
            D3DTADDRESS_CLAMP
        };
        const float constants[4] =
        {
            1.0f / static_cast<float>(std::max<UINT>(levelWidths[child], 1)),
            1.0f / static_cast<float>(std::max<UINT>(levelHeights[child], 1)),
            settings.scatter,
            0.0f
        };
        ok = RunPostFxFullscreenPass(
            device,
            pyramid[parent].surface,
            pyramid[parent].width,
            pyramid[parent].height,
            g_postFxBloomUpsampleShader,
            &binding,
            1,
            0,
            constants,
            1,
            false,
            PostFxBlendMode::Additive);
    }

    EndPostFxStateBackup(device, &backup);
    hdr->Release();

    if (!ok)
    {
        g_postFxBloomFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    pyramid[0].texture->AddRef();
    *bloomTexture = pyramid[0].texture;

    g_postFxBloomActiveThisFrame.store(true, std::memory_order_relaxed);
    g_postFxBloomUsedAo.store(useAo, std::memory_order_relaxed);
    g_postFxBloomSourceWidth.store(hdrDesc.Width, std::memory_order_relaxed);
    g_postFxBloomSourceHeight.store(hdrDesc.Height, std::memory_order_relaxed);
    g_postFxBloomBaseWidth.store(baseWidth, std::memory_order_relaxed);
    g_postFxBloomBaseHeight.store(baseHeight, std::memory_order_relaxed);
    g_postFxBloomLevels.store(levelCount, std::memory_order_relaxed);
    g_postFxBloomPreparedFrame.store(
        GetPostFxFrameIndex(), std::memory_order_relaxed);
    return true;
}

void ReleasePostFxBloomResources()
{
    std::lock_guard<std::mutex> lock(g_postFxBloomMutex);
    ReleasePostFxBloomShadersUnlocked();
    g_postFxBloomActiveThisFrame.store(false, std::memory_order_relaxed);
    g_postFxBloomUsedAo.store(false, std::memory_order_relaxed);
    g_postFxBloomFallbackToLegacy.store(false, std::memory_order_relaxed);
    g_postFxBloomSourceWidth.store(0, std::memory_order_relaxed);
    g_postFxBloomSourceHeight.store(0, std::memory_order_relaxed);
    g_postFxBloomBaseWidth.store(0, std::memory_order_relaxed);
    g_postFxBloomBaseHeight.store(0, std::memory_order_relaxed);
    g_postFxBloomLevels.store(0, std::memory_order_relaxed);
    g_postFxBloomPreparedFrame.store(0, std::memory_order_relaxed);
}
