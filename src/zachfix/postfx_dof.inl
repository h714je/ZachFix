// -----------------------------------------------------------------------------
// ZachFix PostFX NG: two-layer resolution-independent DoF v0.2
// -----------------------------------------------------------------------------

namespace
{
std::mutex g_postFxDofMutex;
std::atomic_uint g_postFxDofMode{ static_cast<UINT>(PostFxDofMode::Legacy) };
std::atomic<float> g_postFxDofMaxRadiusPixels{ 12.0f };
std::atomic<float> g_postFxDofNearStrength{ 1.0f };
std::atomic<float> g_postFxDofFarStrength{ 1.0f };
std::atomic<float> g_postFxDofDepthReject{ 1.5f };
std::atomic<float> g_postFxDofHighlightBoost{ 0.25f };
std::atomic_uint g_postFxDofResolutionDivisor{ 2 };

std::atomic_bool g_postFxDofShaderReady{ false };
std::atomic_bool g_postFxDofShaderCompileFailed{ false };
std::atomic_bool g_postFxDofActiveThisFrame{ false };
std::atomic_bool g_postFxDofFallbackToLegacy{ false };
std::atomic_uint g_postFxDofWidth{ 0 };
std::atomic_uint g_postFxDofHeight{ 0 };
std::atomic_uint g_postFxDofSourceWidth{ 0 };
std::atomic_uint g_postFxDofSourceHeight{ 0 };
std::atomic_ullong g_postFxDofPreparedFrame{ 0 };
std::atomic_bool g_postFxDofFreezePending{ false };
std::atomic_bool g_postFxDofFrameFrozen{ false };
std::atomic_ullong g_postFxDofFrozenFrame{ 0 };

IDirect3DTexture9* g_postFxDofFrozenDiffuse = nullptr;
IDirect3DTexture9* g_postFxDofFrozenBloom = nullptr;
IDirect3DTexture9* g_postFxDofFrozenLuminance = nullptr;
IDirect3DTexture9* g_postFxDofFrozenGaussian = nullptr;
IDirect3DTexture9* g_postFxDofFrozenDepth = nullptr;
IDirect3DTexture9* g_postFxDofFrozenNormal = nullptr;
float g_postFxDofFrozenBloomForce[4] = {};
float g_postFxDofFrozenExposure[4] = {};
float g_postFxDofFrozenPrm[4] = {};
float g_postFxDofFrozenFocus[4] = {};
float g_postFxDofFrozenProjectionScaleX = 0.0f;
float g_postFxDofFrozenProjectionScaleY = 0.0f;

IDirect3DDevice9* g_postFxDofShaderOwner = nullptr; // borrowed
IDirect3DPixelShader9* g_postFxDofNearShader = nullptr;
IDirect3DPixelShader9* g_postFxDofFarShader = nullptr;

static const char kPostFxDofShaderSource[] = R"HLSL(
sampler2D g_tHdr   : register(s0);
sampler2D g_tDepth : register(s1);
sampler2D g_tAO    : register(s2);

// c0 = { invOutputWidth, invOutputHeight, maxRadiusPixels, depthReject }
// c1 = { nearStrength, farStrength, applyAO, highlightBoost }
// c2 = DP g_vDofprm (original c15)
// c3 = DP g_fFocus  (original c16)
float4 g_zfDof0 : register(c0);
float4 g_zfDof1 : register(c1);
float4 g_zfDofPrm : register(c2);
float4 g_zfFocus : register(c3);

float DecodeDepth(float4 packed)
{
    float depth = dot(packed.rgb, float3(65535.0, 255.0, 1.0));
    return saturate(depth / 255.0) * 255.0;
}

float NearCoC(float depth)
{
    return saturate(g_zfDofPrm.x - depth * g_zfDofPrm.y);
}

float FarCoC(float depth)
{
    return saturate(1.0 - (g_zfDofPrm.z - depth * g_zfDofPrm.w));
}

float3 SampleHdr(float2 uv)
{
    // ps_3_0 / D3DX9 cannot place implicit-gradient tex2D operations behind
    // dynamic flow control. Always sample AO and select the multiplier
    // branchlessly instead. When AO is disabled, s2 is bound to a harmless
    // fallback texture and applyAO is 0.
    float3 color = max(tex2D(g_tHdr, uv).rgb, 0.0);
    float ao = saturate(tex2D(g_tAO, uv).r);
    color *= lerp(1.0, ao, saturate(g_zfDof1.z));
    return color;
}

void AccumulateNear(
    float2 uv,
    float2 direction,
    float radial,
    float centerDepth,
    inout float3 colorSum,
    inout float weightSum,
    inout float coverageSum)
{
    float2 offset = direction * radial * g_zfDof0.z * g_zfDof0.xy;
    float2 sampleUv = saturate(uv + offset);
    float sampleDepth = DecodeDepth(tex2D(g_tDepth, sampleUv));
    float coc = max(NearCoC(sampleDepth), saturate(g_zfFocus.x));

    // Near foreground is allowed to spread over farther pixels, but samples
    // significantly behind the center should not bleed through it.
    float behind = max(sampleDepth - centerDepth, 0.0);
    float depthWeight = saturate(1.0 - behind / max(g_zfDof0.w, 1e-3));

    // A sample may only influence distances supported by its own CoC. This is
    // the gather equivalent of a cheap near-field dilation/splat.
    float reach = saturate((coc - radial * 0.55) * 5.0 + 0.35);
    float distanceWeight = 1.0 - radial * 0.35;
    float3 sampleColor = SampleHdr(sampleUv);
    float sampleLuma = dot(sampleColor, float3(0.2126, 0.7152, 0.0722));
    float highlightWeight = 1.0 +
        saturate((sampleLuma - 1.0) * 0.5) * max(g_zfDof1.w, 0.0);
    float weight = coc * depthWeight * reach * distanceWeight * highlightWeight;

    colorSum += sampleColor * weight;
    weightSum += weight;
    coverageSum += coc * depthWeight * reach;
}

void AccumulateFar(
    float2 uv,
    float2 direction,
    float radial,
    float radiusPixels,
    float centerDepth,
    inout float3 colorSum,
    inout float weightSum)
{
    float2 offset = direction * radial * radiusPixels * g_zfDof0.xy;
    float2 sampleUv = saturate(uv + offset);
    float sampleDepth = DecodeDepth(tex2D(g_tDepth, sampleUv));
    float sampleCoc = max(FarCoC(sampleDepth), saturate(g_zfFocus.x));

    // Foreground samples should not smear into a far-background blur.
    float foreground = max(centerDepth - sampleDepth, 0.0);
    float depthWeight = saturate(1.0 - foreground / max(g_zfDof0.w, 1e-3));
    float distanceWeight = 1.0 - radial * 0.30;
    float3 sampleColor = SampleHdr(sampleUv);
    float sampleLuma = dot(sampleColor, float3(0.2126, 0.7152, 0.0722));
    float highlightWeight = 1.0 +
        saturate((sampleLuma - 1.0) * 0.5) * max(g_zfDof1.w, 0.0);
    float weight = depthWeight * distanceWeight *
        lerp(0.25, 1.0, sampleCoc) * highlightWeight;

    colorSum += sampleColor * weight;
    weightSum += weight;
}

float4 NearMain(float2 uv : TEXCOORD0) : COLOR0
{
    float centerDepth = DecodeDepth(tex2D(g_tDepth, uv));
    float centerCoc = max(NearCoC(centerDepth), saturate(g_zfFocus.x));

    float3 colorSum = SampleHdr(uv) * max(centerCoc, 0.05);
    float weightSum = max(centerCoc, 0.05);
    float coverageSum = centerCoc;

    // Four rotated rings give a rounder 16-tap disk than the original
    // axis/diagonal 12-tap pattern, while remaining fully unrolled for ps_3_0.
    AccumulateNear(uv, float2( 1.0000,  0.0000), 0.24, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2( 0.0000,  1.0000), 0.24, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-1.0000,  0.0000), 0.24, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2( 0.0000, -1.0000), 0.24, centerDepth, colorSum, weightSum, coverageSum);

    AccumulateNear(uv, float2( 0.7071,  0.7071), 0.46, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-0.7071,  0.7071), 0.46, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-0.7071, -0.7071), 0.46, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2( 0.7071, -0.7071), 0.46, centerDepth, colorSum, weightSum, coverageSum);

    AccumulateNear(uv, float2( 0.9239,  0.3827), 0.70, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-0.3827,  0.9239), 0.70, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-0.9239, -0.3827), 0.70, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2( 0.3827, -0.9239), 0.70, centerDepth, colorSum, weightSum, coverageSum);

    AccumulateNear(uv, float2( 0.3827,  0.9239), 0.94, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-0.9239,  0.3827), 0.94, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2(-0.3827, -0.9239), 0.94, centerDepth, colorSum, weightSum, coverageSum);
    AccumulateNear(uv, float2( 0.9239, -0.3827), 0.94, centerDepth, colorSum, weightSum, coverageSum);

    float3 color = colorSum / max(weightSum, 1e-4);
    float coverage = saturate(coverageSum / 8.0);
    coverage = max(coverage, centerCoc);
    return float4(color, saturate(coverage));
}

float4 FarMain(float2 uv : TEXCOORD0) : COLOR0
{
    float centerDepth = DecodeDepth(tex2D(g_tDepth, uv));
    float centerCoc = max(FarCoC(centerDepth), saturate(g_zfFocus.x));
    float radiusPixels = g_zfDof0.z * centerCoc;

    // Keep this path branchless as well: AccumulateFar performs tex2D
    // sampling, so an early dynamic branch would trigger D3DX9 X3528.
    // With centerCoc == 0 radiusPixels is also 0, so all gathers collapse to
    // the center sample and alpha remains 0 naturally.
    float3 center = SampleHdr(uv);
    float3 colorSum = center;
    float weightSum = 1.0;

    AccumulateFar(uv, float2( 1.0000,  0.0000), 0.24, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2( 0.0000,  1.0000), 0.24, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-1.0000,  0.0000), 0.24, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2( 0.0000, -1.0000), 0.24, radiusPixels, centerDepth, colorSum, weightSum);

    AccumulateFar(uv, float2( 0.7071,  0.7071), 0.46, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-0.7071,  0.7071), 0.46, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-0.7071, -0.7071), 0.46, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2( 0.7071, -0.7071), 0.46, radiusPixels, centerDepth, colorSum, weightSum);

    AccumulateFar(uv, float2( 0.9239,  0.3827), 0.70, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-0.3827,  0.9239), 0.70, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-0.9239, -0.3827), 0.70, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2( 0.3827, -0.9239), 0.70, radiusPixels, centerDepth, colorSum, weightSum);

    AccumulateFar(uv, float2( 0.3827,  0.9239), 0.94, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-0.9239,  0.3827), 0.94, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2(-0.3827, -0.9239), 0.94, radiusPixels, centerDepth, colorSum, weightSum);
    AccumulateFar(uv, float2( 0.9239, -0.3827), 0.94, radiusPixels, centerDepth, colorSum, weightSum);

    return float4(colorSum / max(weightSum, 1e-4), centerCoc);
}
)HLSL";

void ReleasePostFxDofFrozenInputsUnlocked()
{
    IDirect3DTexture9** textures[] =
    {
        &g_postFxDofFrozenDiffuse,
        &g_postFxDofFrozenBloom,
        &g_postFxDofFrozenLuminance,
        &g_postFxDofFrozenGaussian,
        &g_postFxDofFrozenDepth,
        &g_postFxDofFrozenNormal
    };
    for (IDirect3DTexture9** texture : textures)
    {
        if (*texture != nullptr)
        {
            (*texture)->Release();
            *texture = nullptr;
        }
    }

    ReleasePostFxTarget(PostFxTargetSlot::DoFFreezeDiffuse);
    ReleasePostFxTarget(PostFxTargetSlot::DoFFreezeBloom);
    ReleasePostFxTarget(PostFxTargetSlot::DoFFreezeLuminance);
    ReleasePostFxTarget(PostFxTargetSlot::DoFFreezeGaussian);
    ReleasePostFxTarget(PostFxTargetSlot::DoFFreezeDepth);
    ReleasePostFxTarget(PostFxTargetSlot::DoFFreezeNormal);

    g_postFxDofFrameFrozen.store(false, std::memory_order_relaxed);
    g_postFxDofFrozenFrame.store(0, std::memory_order_relaxed);
    g_postFxDofFrozenProjectionScaleX = 0.0f;
    g_postFxDofFrozenProjectionScaleY = 0.0f;
}

bool CapturePostFxDofTexture(
    IDirect3DDevice9* device,
    IDirect3DTexture9* source,
    PostFxTargetSlot slot,
    IDirect3DTexture9** captured)
{
    if (captured != nullptr)
        *captured = nullptr;
    if (device == nullptr || source == nullptr || captured == nullptr ||
        g_originalStretchRect == nullptr)
    {
        return false;
    }

    D3DSURFACE_DESC desc = {};
    if (FAILED(source->GetLevelDesc(0, &desc)) || desc.Width == 0 || desc.Height == 0)
        return false;

    PostFxTargetView target{};
    if (!EnsurePostFxTarget(device, slot, desc.Width, desc.Height, desc.Format, &target))
        return false;

    IDirect3DSurface9* sourceSurface = nullptr;
    if (FAILED(source->GetSurfaceLevel(0, &sourceSurface)) || sourceSurface == nullptr)
        return false;

    const HRESULT copyResult = g_originalStretchRect(
        device, sourceSurface, nullptr, target.surface, nullptr, D3DTEXF_NONE);
    sourceSurface->Release();
    if (FAILED(copyResult))
        return false;

    target.texture->AddRef();
    *captured = target.texture;
    return true;
}

float ClampPostFxDofFloat(float value, float minimum, float maximum)
{
    if (!std::isfinite(value))
        return minimum;
    return std::max(minimum, std::min(maximum, value));
}

void ReleasePostFxDofShadersUnlocked()
{
    if (g_postFxDofNearShader != nullptr)
    {
        g_postFxDofNearShader->Release();
        g_postFxDofNearShader = nullptr;
    }
    if (g_postFxDofFarShader != nullptr)
    {
        g_postFxDofFarShader->Release();
        g_postFxDofFarShader = nullptr;
    }
    g_postFxDofShaderOwner = nullptr;
    g_postFxDofShaderReady.store(false, std::memory_order_relaxed);
}

bool EnsurePostFxDofShadersUnlocked(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    if (g_postFxDofShaderOwner != nullptr && g_postFxDofShaderOwner != device)
    {
        ReleasePostFxDofShadersUnlocked();
        g_postFxDofShaderCompileFailed.store(false, std::memory_order_relaxed);
    }
    g_postFxDofShaderOwner = device;

    // Shader compilation is deterministic. Do not retry every final-composite
    // draw after a failure and grow ZachFix.log by tens of thousands of lines.
    if (g_postFxDofShaderCompileFailed.load(std::memory_order_relaxed))
        return false;

    if (g_postFxDofNearShader != nullptr && g_postFxDofFarShader != nullptr)
    {
        g_postFxDofShaderReady.store(true, std::memory_order_relaxed);
        return true;
    }

    AppendLog("[PostFX][DoF] Compiling DoF NG v0.2 bokeh near-layer shader.\n");
    if (!CompilePostFxPixelShader(
            device, kPostFxDofShaderSource,
            "NearMain", "DoF NG v0.2 near", &g_postFxDofNearShader))
    {
        ReleasePostFxDofShadersUnlocked();
        g_postFxDofShaderOwner = device;
        g_postFxDofShaderCompileFailed.store(true, std::memory_order_relaxed);
        return false;
    }

    AppendLog("[PostFX][DoF] Near layer compiled; compiling far-layer shader.\n");
    if (!CompilePostFxPixelShader(
            device, kPostFxDofShaderSource,
            "FarMain", "DoF NG v0.2 far", &g_postFxDofFarShader))
    {
        ReleasePostFxDofShadersUnlocked();
        g_postFxDofShaderOwner = device;
        g_postFxDofShaderCompileFailed.store(true, std::memory_order_relaxed);
        return false;
    }

    g_postFxDofShaderReady.store(true, std::memory_order_relaxed);
    AppendLog("[PostFX][DoF] DoF NG v0.2 bokeh shaders compiled.\n");
    return true;
}
} // namespace

PostFxDofSettings GetPostFxDofSettings()
{
    PostFxDofSettings settings{};
    settings.mode = static_cast<PostFxDofMode>(g_postFxDofMode.load(std::memory_order_relaxed));
    settings.maxRadiusPixels = g_postFxDofMaxRadiusPixels.load(std::memory_order_relaxed);
    settings.nearStrength = g_postFxDofNearStrength.load(std::memory_order_relaxed);
    settings.farStrength = g_postFxDofFarStrength.load(std::memory_order_relaxed);
    settings.depthReject = g_postFxDofDepthReject.load(std::memory_order_relaxed);
    settings.highlightBoost = g_postFxDofHighlightBoost.load(std::memory_order_relaxed);
    settings.resolutionDivisor = g_postFxDofResolutionDivisor.load(std::memory_order_relaxed);
    return settings;
}

void SetPostFxDofMode(PostFxDofMode mode)
{
    g_postFxDofMode.store(
        std::min<UINT>(static_cast<UINT>(mode), static_cast<UINT>(PostFxDofMode::ShowFar)),
        std::memory_order_relaxed);
}

void SetPostFxDofMaxRadiusPixels(float radiusPixels)
{
    g_postFxDofMaxRadiusPixels.store(
        ClampPostFxDofFloat(radiusPixels, 1.0f, 32.0f), std::memory_order_relaxed);
}

void SetPostFxDofNearStrength(float strength)
{
    g_postFxDofNearStrength.store(
        ClampPostFxDofFloat(strength, 0.0f, 2.0f), std::memory_order_relaxed);
}

void SetPostFxDofFarStrength(float strength)
{
    g_postFxDofFarStrength.store(
        ClampPostFxDofFloat(strength, 0.0f, 2.0f), std::memory_order_relaxed);
}

void SetPostFxDofDepthReject(float depthReject)
{
    g_postFxDofDepthReject.store(
        ClampPostFxDofFloat(depthReject, 0.05f, 8.0f), std::memory_order_relaxed);
}

void SetPostFxDofHighlightBoost(float highlightBoost)
{
    g_postFxDofHighlightBoost.store(
        ClampPostFxDofFloat(highlightBoost, 0.0f, 2.0f), std::memory_order_relaxed);
}

void SetPostFxDofResolutionDivisor(UINT divisor)
{
    const UINT sanitized = divisor <= 2 ? 2u : 4u;
    if (g_postFxDofResolutionDivisor.exchange(sanitized, std::memory_order_relaxed) != sanitized)
    {
        ReleasePostFxTarget(PostFxTargetSlot::DoFNear);
        ReleasePostFxTarget(PostFxTargetSlot::DoFFar);
    }
}

void ResetPostFxDofSettings()
{
    g_postFxDofMode.store(static_cast<UINT>(PostFxDofMode::Legacy), std::memory_order_relaxed);
    g_postFxDofMaxRadiusPixels.store(12.0f, std::memory_order_relaxed);
    g_postFxDofNearStrength.store(1.0f, std::memory_order_relaxed);
    g_postFxDofFarStrength.store(1.0f, std::memory_order_relaxed);
    g_postFxDofDepthReject.store(1.5f, std::memory_order_relaxed);
    g_postFxDofHighlightBoost.store(0.25f, std::memory_order_relaxed);
    SetPostFxDofResolutionDivisor(2);
}

PostFxDofStats GetPostFxDofStats()
{
    PostFxDofStats stats{};
    stats.shaderReady = g_postFxDofShaderReady.load(std::memory_order_relaxed);
    stats.activeThisFrame = g_postFxDofActiveThisFrame.load(std::memory_order_relaxed);
    stats.fallbackToLegacy = g_postFxDofFallbackToLegacy.load(std::memory_order_relaxed);
    stats.freezePending = g_postFxDofFreezePending.load(std::memory_order_relaxed);
    stats.frameFrozen = g_postFxDofFrameFrozen.load(std::memory_order_relaxed);
    stats.width = g_postFxDofWidth.load(std::memory_order_relaxed);
    stats.height = g_postFxDofHeight.load(std::memory_order_relaxed);
    stats.sourceWidth = g_postFxDofSourceWidth.load(std::memory_order_relaxed);
    stats.sourceHeight = g_postFxDofSourceHeight.load(std::memory_order_relaxed);
    stats.preparedFrame = g_postFxDofPreparedFrame.load(std::memory_order_relaxed);
    stats.frozenFrame = g_postFxDofFrozenFrame.load(std::memory_order_relaxed);
    return stats;
}

void TogglePostFxDofFreezeFrame()
{
    std::lock_guard<std::mutex> lock(g_postFxDofMutex);
    if (g_postFxDofFrameFrozen.load(std::memory_order_relaxed))
    {
        ReleasePostFxDofFrozenInputsUnlocked();
        g_postFxDofFreezePending.store(false, std::memory_order_relaxed);
        RequestPostFxExposureAdaptationReset();
        AppendLog("[PostFX][Freeze] Frozen PostFX preview released.\n");
        return;
    }

    const bool pending = g_postFxDofFreezePending.load(std::memory_order_relaxed);
    g_postFxDofFreezePending.store(!pending, std::memory_order_relaxed);
    AppendLog(pending ?
        "[PostFX][Freeze] Preview capture cancelled.\n" :
        "[PostFX][Freeze] Preview capture requested for next final composite.\n");
}

bool IsPostFxDofFreezeRequestedOrActive()
{
    return g_postFxDofFreezePending.load(std::memory_order_relaxed) ||
        g_postFxDofFrameFrozen.load(std::memory_order_relaxed);
}

bool UpdatePostFxDofFreezeCapture(
    IDirect3DDevice9* device,
    IDirect3DTexture9* filteredAo)
{
    (void)filteredAo;
    if (device == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(g_postFxDofMutex);
    if (!g_postFxDofFreezePending.load(std::memory_order_relaxed))
        return g_postFxDofFrameFrozen.load(std::memory_order_relaxed);

    IDirect3DTexture9* source[5] = {};
    for (UINT stage = 0; stage < 5; ++stage)
    {
        IDirect3DBaseTexture9* base = nullptr;
        if (FAILED(device->GetTexture(stage, &base)) || base == nullptr ||
            FAILED(base->QueryInterface(
                __uuidof(IDirect3DTexture9), reinterpret_cast<void**>(&source[stage]))))
        {
            if (base != nullptr) base->Release();
            for (IDirect3DTexture9*& texture : source)
                if (texture != nullptr) { texture->Release(); texture = nullptr; }
            g_postFxDofFreezePending.store(false, std::memory_order_relaxed);
            AppendLog("[PostFX][Freeze] WARNING: final-composite source textures unavailable.\n");
            return false;
        }
        base->Release();
    }

    float bloomForce[4] = {};
    float exposure[4] = {};
    float dofPrm[4] = {};
    float focus[4] = {};
    const bool constantsReady =
        SUCCEEDED(device->GetPixelShaderConstantF(9, bloomForce, 1)) &&
        SUCCEEDED(device->GetPixelShaderConstantF(10, exposure, 1)) &&
        SUCCEEDED(device->GetPixelShaderConstantF(15, dofPrm, 1)) &&
        SUCCEEDED(device->GetPixelShaderConstantF(16, focus, 1));

    PostFxGBufferView liveGBuffer{};
    const bool haveGBuffer = AcquirePostFxGBuffer(&liveGBuffer) &&
        liveGBuffer.depth != nullptr && liveGBuffer.normal != nullptr;
    const PostFxAoStats aoStats = GetPostFxAoStats();

    IDirect3DTexture9* frozen[6] = {};
    bool ok = constantsReady &&
        CapturePostFxDofTexture(device, source[0], PostFxTargetSlot::DoFFreezeDiffuse, &frozen[0]) &&
        CapturePostFxDofTexture(device, source[1], PostFxTargetSlot::DoFFreezeBloom, &frozen[1]) &&
        CapturePostFxDofTexture(device, source[2], PostFxTargetSlot::DoFFreezeLuminance, &frozen[2]) &&
        CapturePostFxDofTexture(device, source[3], PostFxTargetSlot::DoFFreezeGaussian, &frozen[3]) &&
        CapturePostFxDofTexture(device, source[4], PostFxTargetSlot::DoFFreezeDepth, &frozen[4]);

    // Normals are optional for the scene snapshot itself, but when present they
    // let GTAO be recomputed from the frozen G-buffer while every AO slider
    // remains live. Depth comes from final-composite s4; the normal resource is
    // the proven geometry RT1 captured by the shared PostFX G-buffer tracker.
    if (ok && haveGBuffer)
    {
        if (!CapturePostFxDofTexture(
                device,
                liveGBuffer.normal,
                PostFxTargetSlot::DoFFreezeNormal,
                &frozen[5]))
        {
            AppendLog("[PostFX][Freeze] WARNING: normal snapshot failed; frozen AO retuning unavailable.\n");
        }
    }

    ReleasePostFxGBuffer(&liveGBuffer);
    for (IDirect3DTexture9*& texture : source)
        if (texture != nullptr) { texture->Release(); texture = nullptr; }

    if (!ok)
    {
        for (IDirect3DTexture9*& texture : frozen)
            if (texture != nullptr) { texture->Release(); texture = nullptr; }
        ReleasePostFxDofFrozenInputsUnlocked();
        g_postFxDofFreezePending.store(false, std::memory_order_relaxed);
        AppendLog("[PostFX][Freeze] WARNING: PostFX preview capture failed.\n");
        return false;
    }

    g_postFxDofFrozenDiffuse = frozen[0];
    g_postFxDofFrozenBloom = frozen[1];
    g_postFxDofFrozenLuminance = frozen[2];
    g_postFxDofFrozenGaussian = frozen[3];
    g_postFxDofFrozenDepth = frozen[4];
    g_postFxDofFrozenNormal = frozen[5];
    std::memcpy(g_postFxDofFrozenBloomForce, bloomForce, sizeof(bloomForce));
    std::memcpy(g_postFxDofFrozenExposure, exposure, sizeof(exposure));
    std::memcpy(g_postFxDofFrozenPrm, dofPrm, sizeof(dofPrm));
    std::memcpy(g_postFxDofFrozenFocus, focus, sizeof(focus));
    g_postFxDofFrozenProjectionScaleX = aoStats.projectionScaleX;
    g_postFxDofFrozenProjectionScaleY = aoStats.projectionScaleY;

    const unsigned long long frame = GetPostFxFrameIndex();
    g_postFxDofFrozenFrame.store(frame, std::memory_order_relaxed);
    g_postFxDofFrameFrozen.store(true, std::memory_order_relaxed);
    g_postFxDofFreezePending.store(false, std::memory_order_relaxed);
    RequestPostFxExposureAdaptationReset();

    char text[256] = {};
    sprintf_s(
        text,
        "[PostFX][Freeze] Frozen PostFX preview captured at frame %llu; G-buffer normals=%s, projection=%.4f x %.4f.\n",
        frame,
        g_postFxDofFrozenNormal != nullptr ? "yes" : "no",
        g_postFxDofFrozenProjectionScaleX,
        g_postFxDofFrozenProjectionScaleY);
    AppendLog(text);
    return true;
}

bool AcquirePostFxDofFreezeView(PostFxDofFreezeView* view)
{
    if (view == nullptr)
        return false;
    *view = {};

    std::lock_guard<std::mutex> lock(g_postFxDofMutex);
    if (!g_postFxDofFrameFrozen.load(std::memory_order_relaxed) ||
        g_postFxDofFrozenDiffuse == nullptr || g_postFxDofFrozenDepth == nullptr)
    {
        return false;
    }

    view->diffuse = g_postFxDofFrozenDiffuse;
    view->bloom = g_postFxDofFrozenBloom;
    view->luminance = g_postFxDofFrozenLuminance;
    view->gaussian = g_postFxDofFrozenGaussian;
    view->depth = g_postFxDofFrozenDepth;
    view->normal = g_postFxDofFrozenNormal;
    IDirect3DTexture9* textures[] =
    {
        view->diffuse, view->bloom, view->luminance,
        view->gaussian, view->depth, view->normal
    };
    for (IDirect3DTexture9* texture : textures)
        if (texture != nullptr) texture->AddRef();

    std::memcpy(view->bloomForce, g_postFxDofFrozenBloomForce, sizeof(view->bloomForce));
    std::memcpy(view->exposure, g_postFxDofFrozenExposure, sizeof(view->exposure));
    std::memcpy(view->dofPrm, g_postFxDofFrozenPrm, sizeof(view->dofPrm));
    std::memcpy(view->focus, g_postFxDofFrozenFocus, sizeof(view->focus));
    view->projectionScaleX = g_postFxDofFrozenProjectionScaleX;
    view->projectionScaleY = g_postFxDofFrozenProjectionScaleY;
    view->frameIndex = g_postFxDofFrozenFrame.load(std::memory_order_relaxed);
    return true;
}

void ReleasePostFxDofFreezeView(PostFxDofFreezeView* view)
{
    if (view == nullptr)
        return;
    IDirect3DTexture9* textures[] =
    {
        view->diffuse, view->bloom, view->luminance,
        view->gaussian, view->depth, view->normal
    };
    for (IDirect3DTexture9* texture : textures)
        if (texture != nullptr) texture->Release();
    *view = {};
}

void TogglePostFxPreviewFreeze()
{
    TogglePostFxDofFreezeFrame();
}

bool IsPostFxPreviewFreezeRequestedOrActive()
{
    return IsPostFxDofFreezeRequestedOrActive();
}

bool AcquirePostFxPreviewGBuffer(
    PostFxGBufferView* view,
    float* projectionScaleX,
    float* projectionScaleY)
{
    if (view == nullptr)
        return false;
    *view = {};
    if (projectionScaleX != nullptr) *projectionScaleX = 0.0f;
    if (projectionScaleY != nullptr) *projectionScaleY = 0.0f;

    std::lock_guard<std::mutex> lock(g_postFxDofMutex);
    if (!g_postFxDofFrameFrozen.load(std::memory_order_relaxed) ||
        g_postFxDofFrozenDepth == nullptr || g_postFxDofFrozenNormal == nullptr)
    {
        return false;
    }

    D3DSURFACE_DESC depthDesc = {};
    D3DSURFACE_DESC normalDesc = {};
    if (FAILED(g_postFxDofFrozenDepth->GetLevelDesc(0, &depthDesc)) ||
        FAILED(g_postFxDofFrozenNormal->GetLevelDesc(0, &normalDesc)) ||
        depthDesc.Width != normalDesc.Width || depthDesc.Height != normalDesc.Height)
    {
        return false;
    }

    g_postFxDofFrozenDepth->AddRef();
    g_postFxDofFrozenNormal->AddRef();
    view->depth = g_postFxDofFrozenDepth;
    view->normal = g_postFxDofFrozenNormal;
    view->width = depthDesc.Width;
    view->height = depthDesc.Height;
    view->frameIndex = g_postFxDofFrozenFrame.load(std::memory_order_relaxed);
    view->fresh = true;

    if (projectionScaleX != nullptr)
        *projectionScaleX = g_postFxDofFrozenProjectionScaleX;
    if (projectionScaleY != nullptr)
        *projectionScaleY = g_postFxDofFrozenProjectionScaleY;
    return true;
}

bool ShouldUsePostFxDofReplacement()
{
    return static_cast<PostFxDofMode>(g_postFxDofMode.load(std::memory_order_relaxed)) !=
        PostFxDofMode::Legacy;
}

bool PreparePostFxDof(
    IDirect3DDevice9* device,
    IDirect3DTexture9* filteredAo,
    IDirect3DTexture9** nearTexture,
    IDirect3DTexture9** farTexture)
{
    if (nearTexture != nullptr)
        *nearTexture = nullptr;
    if (farTexture != nullptr)
        *farTexture = nullptr;

    g_postFxDofActiveThisFrame.store(false, std::memory_order_relaxed);
    g_postFxDofFallbackToLegacy.store(false, std::memory_order_relaxed);

    if (device == nullptr || nearTexture == nullptr || farTexture == nullptr ||
        !ShouldUsePostFxDofReplacement())
    {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_postFxDofMutex);
    if (!EnsurePostFxDofShadersUnlocked(device))
    {
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    IDirect3DBaseTexture9* baseHdr = nullptr;
    IDirect3DBaseTexture9* baseDepth = nullptr;
    if (FAILED(device->GetTexture(0, &baseHdr)) || baseHdr == nullptr ||
        FAILED(device->GetTexture(4, &baseDepth)) || baseDepth == nullptr)
    {
        if (baseHdr != nullptr) baseHdr->Release();
        if (baseDepth != nullptr) baseDepth->Release();
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    IDirect3DTexture9* hdr = nullptr;
    IDirect3DTexture9* depth = nullptr;
    const bool textureOk =
        SUCCEEDED(baseHdr->QueryInterface(__uuidof(IDirect3DTexture9), reinterpret_cast<void**>(&hdr))) &&
        SUCCEEDED(baseDepth->QueryInterface(__uuidof(IDirect3DTexture9), reinterpret_cast<void**>(&depth)));
    baseHdr->Release();
    baseDepth->Release();
    if (!textureOk || hdr == nullptr || depth == nullptr)
    {
        if (hdr != nullptr) hdr->Release();
        if (depth != nullptr) depth->Release();
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    D3DSURFACE_DESC hdrDesc = {};
    D3DVIEWPORT9 viewport = {};
    if (FAILED(hdr->GetLevelDesc(0, &hdrDesc)) || FAILED(device->GetViewport(&viewport)) ||
        viewport.Width == 0 || viewport.Height == 0)
    {
        hdr->Release();
        depth->Release();
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    const PostFxDofSettings settings = GetPostFxDofSettings();
    UINT workWidth = 0;
    UINT workHeight = 0;
    GetPostFxScaledExtent(
        viewport.Width, viewport.Height,
        settings.resolutionDivisor, &workWidth, &workHeight);

    PostFxTargetView nearTarget{};
    PostFxTargetView farTarget{};
    const D3DFORMAT format = D3DFMT_A16B16G16R16F;
    if (!EnsurePostFxTarget(device, PostFxTargetSlot::DoFNear,
            workWidth, workHeight, format, &nearTarget) ||
        !EnsurePostFxTarget(device, PostFxTargetSlot::DoFFar,
            workWidth, workHeight, format, &farTarget))
    {
        hdr->Release();
        depth->Release();
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    float dofPrm[4] = {};
    float focus[4] = {};
    if (FAILED(device->GetPixelShaderConstantF(15, dofPrm, 1)) ||
        FAILED(device->GetPixelShaderConstantF(16, focus, 1)))
    {
        hdr->Release();
        depth->Release();
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    const float constants[16] =
    {
        1.0f / static_cast<float>(viewport.Width),
        1.0f / static_cast<float>(viewport.Height),
        settings.maxRadiusPixels,
        settings.depthReject,

        settings.nearStrength,
        settings.farStrength,
        filteredAo != nullptr ? 1.0f : 0.0f,
        settings.highlightBoost,

        dofPrm[0], dofPrm[1], dofPrm[2], dofPrm[3],
        focus[0], focus[1], focus[2], focus[3]
    };

    IDirect3DTexture9* aoInput = filteredAo != nullptr ? filteredAo : hdr;
    const PostFxTextureBinding bindings[] =
    {
        { 0, hdr, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP },
        { 1, depth, D3DTEXF_POINT, D3DTADDRESS_CLAMP },
        { 2, aoInput, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP }
    };

    PostFxStateBackup backup{};
    bool ok = BeginPostFxStateBackup(device, &backup);
    if (ok)
    {
        ok = RunPostFxFullscreenPass(
            device,
            &backup,
            nearTarget.surface, nearTarget.width, nearTarget.height,
            g_postFxDofNearShader, bindings, 3,
            0, constants, 4, false, PostFxBlendMode::Opaque);
    }
    if (ok)
    {
        ok = RunPostFxFullscreenPass(
            device,
            &backup,
            farTarget.surface, farTarget.width, farTarget.height,
            g_postFxDofFarShader, bindings, 3,
            0, constants, 4, false, PostFxBlendMode::Opaque);
    }
    if (backup.active)
        EndPostFxStateBackup(device, &backup);

    hdr->Release();
    depth->Release();

    if (!ok)
    {
        g_postFxDofFallbackToLegacy.store(true, std::memory_order_relaxed);
        return false;
    }

    nearTarget.texture->AddRef();
    farTarget.texture->AddRef();
    *nearTexture = nearTarget.texture;
    *farTexture = farTarget.texture;

    g_postFxDofWidth.store(workWidth, std::memory_order_relaxed);
    g_postFxDofHeight.store(workHeight, std::memory_order_relaxed);
    g_postFxDofSourceWidth.store(hdrDesc.Width, std::memory_order_relaxed);
    g_postFxDofSourceHeight.store(hdrDesc.Height, std::memory_order_relaxed);
    g_postFxDofPreparedFrame.store(GetPostFxFrameIndex(), std::memory_order_relaxed);
    g_postFxDofActiveThisFrame.store(true, std::memory_order_relaxed);
    return true;
}

void ReleasePostFxDofResources()
{
    std::lock_guard<std::mutex> lock(g_postFxDofMutex);
    ReleasePostFxDofShadersUnlocked();
    g_postFxDofShaderCompileFailed.store(false, std::memory_order_relaxed);
    ReleasePostFxDofFrozenInputsUnlocked();
    g_postFxDofFreezePending.store(false, std::memory_order_relaxed);
    ReleasePostFxTarget(PostFxTargetSlot::DoFNear);
    ReleasePostFxTarget(PostFxTargetSlot::DoFFar);
    g_postFxDofActiveThisFrame.store(false, std::memory_order_relaxed);
    g_postFxDofFallbackToLegacy.store(false, std::memory_order_relaxed);
}
