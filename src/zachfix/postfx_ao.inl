// -----------------------------------------------------------------------------
// ZachFix PostFX - thickness-aware horizon ambient occlusion
// -----------------------------------------------------------------------------

namespace
{
std::mutex g_postFxAoMutex;
IDirect3DDevice9* g_postFxAoShaderOwner = nullptr; // borrowed
IDirect3DPixelShader9* g_postFxAoRawShader = nullptr;
IDirect3DPixelShader9* g_postFxAoDenoiseShader = nullptr;
IDirect3DPixelShader9* g_postFxAoDisplayShader = nullptr;

float g_postFxProjectionScaleX = 0.0f;
float g_postFxProjectionScaleY = 0.0f;
float g_postFxAoProjectionDepthQ = 0.0f;
float g_postFxAoProjectionDepthQn = 0.0f;
unsigned long long g_postFxProjectionFrame = 0;

std::atomic_bool g_postFxNativeDepthActiveThisFrame{ false };

std::atomic_bool g_postFxAoShaderReady{ false };
std::atomic_bool g_postFxAoActiveThisFrame{ false };
std::atomic_bool g_postFxAoSkippedStale{ false };
std::atomic_bool g_postFxAoSkippedProjection{ false };
std::atomic_uint g_postFxAoWidth{ 0 };
std::atomic_uint g_postFxAoHeight{ 0 };
std::atomic_ullong g_postFxAoPreparedFrame{ 0 };

static const char kPostFxAoRawShaderSource[] = R"HLSL(
sampler2D g_tDepth  : register(s0);
sampler2D g_tNormal : register(s1);

// c0: depthTexel.xy, projectionScale.xy
// c1: radius, strength, bias, power
// c2: aoSize.xy, thicknessRatio, depthEncoding (0 packed / 1 INTZ / 2 linear)
// c3: projectionDepthQ, projectionDepthQn, reserved, reserved
float4 g_vInput : register(c0);
float4 g_vAo    : register(c1);
float4 g_vSize  : register(c2);
float4 g_vDepth : register(c3);

float DecodeDepth(float2 uv)
{
    float4 packed = tex2D(g_tDepth, uv);
    if (g_vSize.w > 1.5)
        return packed.r;
    if (g_vSize.w > 0.5)
    {
        float denom = g_vDepth.x - packed.r;
        if (denom <= 1e-7 || g_vDepth.y <= 1e-7)
            return 1e20;
        return g_vDepth.y / denom;
    }
    return saturate(dot(packed.rgb, float3(65535.0, 255.0, 1.0)) * (1.0 / 255.0)) * 255.0;
}

float3 DecodeNormal(float2 uv)
{
    float3 n = tex2D(g_tNormal, uv).rgb * 2.0 - 1.0;
    float len2 = dot(n, n);
    if (len2 < 0.20 || len2 > 2.25)
        return float3(0.0, 0.0, 0.0);
    return n * rsqrt(max(len2, 1e-5));
}

float3 ReconstructViewPosition(float2 uv, float depth)
{
    float2 ndc = float2(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0);
    return float3(
        ndc.x * depth / max(g_vInput.z, 1e-4),
        ndc.y * depth / max(g_vInput.w, 1e-4),
        depth);
}

float InterleavedGradientNoise(float2 pixel)
{
    return frac(52.9829189 * frac(dot(pixel, float2(0.06711056, 0.00583715))));
}

// Compiler-safe horizon proxy for ps_3_0 / legacy D3DX9.
// Instead of integrating acos-based arcs, compare each observed horizon with
// the tangent ray that would stay on the center pixel's local plane. Samples
// rising above that tangent contribute occlusion; coplanar samples do not.
float SampleHorizonExcess(
    float2 centerUv,
    float3 centerPos,
    float3 centerNormal,
    float2 sampleUv,
    float radius,
    float bias,
    float thicknessRatio)
{
    if (sampleUv.x <= 0.0 || sampleUv.x >= 1.0 ||
        sampleUv.y <= 0.0 || sampleUv.y >= 1.0)
        return 0.0;

    float sampleDepth = DecodeDepth(sampleUv);
    if (sampleDepth <= 1e-4 || sampleDepth >= 1e19 ||
        (g_vSize.w < 0.5 && sampleDepth >= 254.5))
    {
        return 0.0;
    }

    // Foreground silhouettes are the dominant source of screen-space AO halos:
    // a much closer object must not become a giant occluder for a distant
    // background surface. Thickness is expressed as a fraction of AO radius,
    // so the rejection scale remains stable while Radius is hot-adjusted.
    float thickness = max(radius * max(thicknessRatio, 0.01), 0.05);
    float frontGap = max(centerPos.z - sampleDepth, 0.0);
    float frontWeight = saturate(1.0 - frontGap / thickness);
    frontWeight = frontWeight * frontWeight * (3.0 - 2.0 * frontWeight);
    if (frontWeight <= 1e-4)
        return 0.0;

    float3 samplePos = ReconstructViewPosition(sampleUv, sampleDepth);
    float3 delta = samplePos - centerPos;
    float dist2 = dot(delta, delta);
    if (dist2 <= 1e-5 || dist2 >= radius * radius)
        return 0.0;

    float invDist = rsqrt(dist2);
    float sampleSin = dot(centerNormal, delta) * invDist;

    // Reconstruct where this screen-space ray would land if it stayed at the
    // center depth. Its angle is the local tangent baseline for this slice.
    float3 tangentPos = ReconstructViewPosition(sampleUv, centerPos.z);
    float3 tangentDelta = tangentPos - centerPos;
    float tangentLen2 = dot(tangentDelta, tangentDelta);
    float tangentSin = 0.0;
    if (tangentLen2 > 1e-7)
        tangentSin = dot(centerNormal, tangentDelta) * rsqrt(tangentLen2);

    float horizonExcess = sampleSin - tangentSin - bias;
    float dist = dist2 * invDist;
    float falloff = saturate(1.0 - dist / radius);
    falloff *= falloff;
    falloff *= falloff; // quartic: keep contact AO, suppress long-range halos

    // The factor maps a modest horizon elevation into a useful AO range while
    // retaining Strength as the final artistic multiplier.
    return saturate(horizonExcess * 2.5) * falloff * frontWeight;
}

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float centerDepth = DecodeDepth(uv);
    if (centerDepth <= 1e-4 || centerDepth >= 1e19 ||
        (g_vSize.w < 0.5 && centerDepth >= 254.5))
    {
        return 1.0;
    }

    float3 normal = DecodeNormal(uv);
    if (dot(normal, normal) < 0.5)
        return 1.0;

    float radius = max(g_vAo.x, 0.05);
    float strength = max(g_vAo.y, 0.0);
    float bias = saturate(g_vAo.z);
    float thicknessRatio = max(g_vSize.z, 0.01);
    float power = max(g_vAo.w, 0.05);
    float3 centerPos = ReconstructViewPosition(uv, centerDepth);

    float2 uvRadius = float2(g_vInput.z, g_vInput.w) *
                      (radius / max(centerDepth, 0.05)) * 0.5;
    float2 minRadius = 1.5 / max(g_vSize.xy, 1.0);
    uvRadius = max(uvRadius, minRadius);
    uvRadius = min(uvRadius, 0.24);

    float noise = InterleavedGradientNoise(floor(uv * g_vSize.xy));
    float angle = noise * 1.57079632679;
    float s, c;
    sincos(angle, s, c);

    const float k = 0.70710678118;
    float2 d0 = float2(c, s);
    float2 d1 = float2((c - s) * k, (s + c) * k);
    float2 d2 = float2(-s, c);
    float2 d3 = float2(-(s + c) * k, (c - s) * k);

    float nearT = 0.22 + 0.08 * noise;
    float farT = 0.62 + 0.18 * frac(noise + 0.38196601125);

    float occ = 0.0;
    float a, b;

    a = SampleHorizonExcess(uv, centerPos, normal, uv + d0 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv + d0 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);
    a = SampleHorizonExcess(uv, centerPos, normal, uv - d0 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv - d0 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);

    a = SampleHorizonExcess(uv, centerPos, normal, uv + d1 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv + d1 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);
    a = SampleHorizonExcess(uv, centerPos, normal, uv - d1 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv - d1 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);

    a = SampleHorizonExcess(uv, centerPos, normal, uv + d2 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv + d2 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);
    a = SampleHorizonExcess(uv, centerPos, normal, uv - d2 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv - d2 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);

    a = SampleHorizonExcess(uv, centerPos, normal, uv + d3 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv + d3 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);
    a = SampleHorizonExcess(uv, centerPos, normal, uv - d3 * uvRadius * nearT, radius, bias, thicknessRatio);
    b = SampleHorizonExcess(uv, centerPos, normal, uv - d3 * uvRadius * farT,  radius, bias, thicknessRatio);
    occ += max(a, b);

    occ *= 0.125;
    float ao = saturate(1.0 - strength * occ);
    ao = pow(max(ao, 1e-4), power);
    return float4(ao, ao, ao, 1.0);
}
)HLSL";

static const char kPostFxAoDenoiseShaderSource[] = R"HLSL(
sampler2D g_tAo     : register(s0);
sampler2D g_tDepth  : register(s1);
sampler2D g_tNormal : register(s2);

// c0: aoTexel.xy, depthSharpness, depthEncoding (0 packed / 1 INTZ / 2 linear)
// c1: projectionDepthQ, projectionDepthQn, reserved, reserved
float4 g_vFilter : register(c0);
float4 g_vDepth  : register(c1);

float DecodeDepth(float2 uv)
{
    float4 packed = tex2D(g_tDepth, uv);
    if (g_vFilter.w > 1.5)
        return packed.r;
    if (g_vFilter.w > 0.5)
    {
        float denom = g_vDepth.x - packed.r;
        if (denom <= 1e-7 || g_vDepth.y <= 1e-7)
            return 1e20;
        return g_vDepth.y / denom;
    }
    return saturate(dot(packed.rgb, float3(65535.0, 255.0, 1.0)) * (1.0 / 255.0)) * 255.0;
}

float3 DecodeNormal(float2 uv)
{
    float3 n = tex2D(g_tNormal, uv).rgb * 2.0 - 1.0;
    return n * rsqrt(max(dot(n, n), 1e-5));
}

float EdgeWeight(float2 uv, float centerDepth, float3 centerNormal)
{
    float d = DecodeDepth(uv);
    float depthW = saturate(1.0 - abs(d - centerDepth) * g_vFilter.z);
    depthW *= depthW;

    float3 n = DecodeNormal(uv);
    float normalW = saturate(dot(centerNormal, n));
    normalW *= normalW;
    normalW *= normalW;
    return depthW * normalW;
}

void Accum(
    inout float sum,
    inout float weightSum,
    float2 uv,
    float spatialWeight,
    float centerDepth,
    float3 centerNormal)
{
    float w = spatialWeight * EdgeWeight(uv, centerDepth, centerNormal);
    sum += tex2D(g_tAo, uv).r * w;
    weightSum += w;
}

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float centerDepth = DecodeDepth(uv);
    float3 centerNormal = DecodeNormal(uv);
    float2 t = g_vFilter.xy;

    float centerAo = tex2D(g_tAo, uv).r;
    float sum = centerAo;
    float weightSum = 1.0;

    Accum(sum, weightSum, uv + float2( t.x, 0.0), 0.75, centerDepth, centerNormal);
    Accum(sum, weightSum, uv + float2(-t.x, 0.0), 0.75, centerDepth, centerNormal);
    Accum(sum, weightSum, uv + float2(0.0,  t.y), 0.75, centerDepth, centerNormal);
    Accum(sum, weightSum, uv + float2(0.0, -t.y), 0.75, centerDepth, centerNormal);

    Accum(sum, weightSum, uv + float2( t.x,  t.y), 0.45, centerDepth, centerNormal);
    Accum(sum, weightSum, uv + float2(-t.x,  t.y), 0.45, centerDepth, centerNormal);
    Accum(sum, weightSum, uv + float2( t.x, -t.y), 0.45, centerDepth, centerNormal);
    Accum(sum, weightSum, uv + float2(-t.x, -t.y), 0.45, centerDepth, centerNormal);

    float ao = sum / max(weightSum, 1e-4);
    return float4(ao, ao, ao, 1.0);
}
)HLSL";

static const char kPostFxAoDisplayShaderSource[] = R"HLSL(
sampler2D g_tAo : register(s0);
float4 g_vDisplay : register(c0); // x = display contrast exponent
float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float ao = tex2D(g_tAo, uv).r;
    ao = pow(max(ao, 1e-4), max(g_vDisplay.x, 0.05));
    return float4(ao, ao, ao, 1.0);
}
)HLSL";

void ReleasePostFxAoShadersUnlocked()
{
    if (g_postFxAoRawShader != nullptr)
    {
        g_postFxAoRawShader->Release();
        g_postFxAoRawShader = nullptr;
    }
    if (g_postFxAoDenoiseShader != nullptr)
    {
        g_postFxAoDenoiseShader->Release();
        g_postFxAoDenoiseShader = nullptr;
    }
    if (g_postFxAoDisplayShader != nullptr)
    {
        g_postFxAoDisplayShader->Release();
        g_postFxAoDisplayShader = nullptr;
    }
    g_postFxAoShaderReady.store(false, std::memory_order_release);
}

bool EnsurePostFxAoShaders(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(g_postFxAoMutex);
    if (g_postFxAoShaderOwner != device)
    {
        ReleasePostFxAoShadersUnlocked();
        g_postFxAoShaderOwner = device;
    }

    if (g_postFxAoRawShader != nullptr &&
        g_postFxAoDenoiseShader != nullptr &&
        g_postFxAoDisplayShader != nullptr)
    {
        g_postFxAoShaderReady.store(true, std::memory_order_release);
        return true;
    }

    ReleasePostFxAoShadersUnlocked();

    AppendLog("[PostFX][AO] Compiling ambient-occlusion raw shader.\n");
    if (!CompilePostFxPixelShader(
            device,
            kPostFxAoRawShaderSource,
            "main",
            "Ambient occlusion raw",
            &g_postFxAoRawShader))
    {
        ReleasePostFxAoShadersUnlocked();
        return false;
    }

    AppendLog("[PostFX][AO] Raw shader compiled; compiling denoise shader.\n");
    if (!CompilePostFxPixelShader(
            device,
            kPostFxAoDenoiseShaderSource,
            "main",
            "Ambient occlusion denoise",
            &g_postFxAoDenoiseShader))
    {
        ReleasePostFxAoShadersUnlocked();
        return false;
    }

    AppendLog("[PostFX][AO] Denoise shader compiled; compiling display shader.\n");
    if (!CompilePostFxPixelShader(
            device,
            kPostFxAoDisplayShaderSource,
            "main",
            "Ambient occlusion display",
            &g_postFxAoDisplayShader))
    {
        ReleasePostFxAoShadersUnlocked();
        return false;
    }

    g_postFxAoShaderReady.store(true, std::memory_order_release);
    AppendLog("[PostFX][AO] Ambient-occlusion shaders compiled.\n");
    return true;
}

bool PostFxTextureIdentityMatches(
    IDirect3DTexture9* a,
    IDirect3DBaseTexture9* b)
{
    if (a == nullptr || b == nullptr || b->GetType() != D3DRTYPE_TEXTURE)
        return false;

    IUnknown* identityA = nullptr;
    IUnknown* identityB = nullptr;
    const HRESULT ra = a->QueryInterface(
        __uuidof(IUnknown), reinterpret_cast<void**>(&identityA));
    const HRESULT rb = b->QueryInterface(
        __uuidof(IUnknown), reinterpret_cast<void**>(&identityB));

    const bool match =
        SUCCEEDED(ra) && SUCCEEDED(rb) &&
        identityA != nullptr && identityB != nullptr &&
        identityA == identityB;

    if (identityA != nullptr)
        identityA->Release();
    if (identityB != nullptr)
        identityB->Release();
    return match;
}

} // namespace

PostFxAoStats GetPostFxAoStats()
{
    PostFxAoStats stats{};
    stats.shaderReady = g_postFxAoShaderReady.load(std::memory_order_acquire);
    stats.activeThisFrame = g_postFxAoActiveThisFrame.load(std::memory_order_relaxed);
    stats.skippedStaleGBuffer = g_postFxAoSkippedStale.load(std::memory_order_relaxed);
    stats.skippedProjection = g_postFxAoSkippedProjection.load(std::memory_order_relaxed);
    stats.nativeDepthAvailable = IsPostFxNativeDepthAvailable();
    stats.nativeDepthActive = g_postFxNativeDepthActiveThisFrame.load(std::memory_order_relaxed);
    stats.width = g_postFxAoWidth.load(std::memory_order_relaxed);
    stats.height = g_postFxAoHeight.load(std::memory_order_relaxed);
    stats.preparedFrame = g_postFxAoPreparedFrame.load(std::memory_order_relaxed);

    std::lock_guard<std::mutex> lock(g_postFxAoMutex);
    stats.projectionScaleX = g_postFxProjectionScaleX;
    stats.projectionScaleY = g_postFxProjectionScaleY;
    stats.projectionDepthQ = g_postFxAoProjectionDepthQ;
    stats.projectionDepthQn = g_postFxAoProjectionDepthQn;
    stats.projectionFrame = g_postFxProjectionFrame;
    stats.projectionReady =
        g_postFxProjectionScaleX > 0.01f && g_postFxProjectionScaleY > 0.01f;
    return stats;
}

void ObservePostFxProjectionParameters(
    float projectionScaleX,
    float projectionScaleY,
    float projectionDepthQ,
    float projectionDepthQn)
{
    if (!std::isfinite(projectionScaleX) ||
        !std::isfinite(projectionScaleY) ||
        !std::isfinite(projectionDepthQ) ||
        !std::isfinite(projectionDepthQn) ||
        projectionScaleX <= 0.01f || projectionScaleY <= 0.01f ||
        projectionScaleX > 100.0f || projectionScaleY > 100.0f ||
        projectionDepthQ <= 0.1f || projectionDepthQ > 10.0f ||
        projectionDepthQn <= 1e-7f || projectionDepthQn > 10000.0f)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(g_postFxAoMutex);
    g_postFxProjectionScaleX = projectionScaleX;
    g_postFxProjectionScaleY = projectionScaleY;
    g_postFxAoProjectionDepthQ = projectionDepthQ;
    g_postFxAoProjectionDepthQn = projectionDepthQn;
    g_postFxProjectionFrame = GetPostFxFrameIndex();
    ObservePostFxDepthProjection(projectionDepthQ, projectionDepthQn);
}

bool BeginPostFxAoFinalComposite(
    IDirect3DDevice9* device,
    PostFxAoFinalCompositeState* state)
{
    if (state == nullptr)
        return false;
    *state = {};

    g_postFxAoActiveThisFrame.store(false, std::memory_order_relaxed);
    g_postFxAoSkippedStale.store(false, std::memory_order_relaxed);
    g_postFxAoSkippedProjection.store(false, std::memory_order_relaxed);
    g_postFxNativeDepthActiveThisFrame.store(false, std::memory_order_relaxed);

    if (device == nullptr)
        return false;

    const PostFxAoSettings settings = GetPostFxAoSettings();
    if (settings.mode == PostFxAoMode::Off)
        return false;

    PostFxGBufferView gbuffer{};
    float projectionScaleX = 0.0f;
    float projectionScaleY = 0.0f;
    float projectionDepthQ = 0.0f;
    float projectionDepthQn = 0.0f;
    const bool frozenPreviewGBuffer = AcquirePostFxPreviewGBuffer(
        &gbuffer, &projectionScaleX, &projectionScaleY);

    if (!frozenPreviewGBuffer)
    {
        if (!AcquirePostFxGBuffer(&gbuffer))
            return false;

        if (!gbuffer.fresh)
        {
            g_postFxAoSkippedStale.store(true, std::memory_order_relaxed);
            ReleasePostFxGBuffer(&gbuffer);
            return false;
        }

        unsigned long long projectionFrame = 0;
        {
            std::lock_guard<std::mutex> lock(g_postFxAoMutex);
            projectionScaleX = g_postFxProjectionScaleX;
            projectionScaleY = g_postFxProjectionScaleY;
            projectionDepthQ = g_postFxAoProjectionDepthQ;
            projectionDepthQn = g_postFxAoProjectionDepthQn;
            projectionFrame = g_postFxProjectionFrame;
        }

        const unsigned long long currentFrame = GetPostFxFrameIndex();
        if (projectionScaleX <= 0.01f || projectionScaleY <= 0.01f ||
            projectionFrame != currentFrame)
        {
            g_postFxAoSkippedProjection.store(true, std::memory_order_relaxed);
            ReleasePostFxGBuffer(&gbuffer);
            return false;
        }

        IDirect3DBaseTexture9* finalDepth = nullptr;
        const bool haveFinalDepth = SUCCEEDED(device->GetTexture(4, &finalDepth));
        const bool depthMatches =
            haveFinalDepth && finalDepth != nullptr &&
            PostFxTextureIdentityMatches(gbuffer.depth, finalDepth);
        if (finalDepth != nullptr)
            finalDepth->Release();

        if (!depthMatches)
        {
            ReleasePostFxGBuffer(&gbuffer);
            return false;
        }
    }
    else if (projectionScaleX <= 0.01f || projectionScaleY <= 0.01f)
    {
        g_postFxAoSkippedProjection.store(true, std::memory_order_relaxed);
        ReleasePostFxGBuffer(&gbuffer);
        return false;
    }

    const unsigned long long currentFrame = GetPostFxFrameIndex();

    IDirect3DSurface9* outputSurface = nullptr;
    if (FAILED(device->GetRenderTarget(0, &outputSurface)) || outputSurface == nullptr)
    {
        ReleasePostFxGBuffer(&gbuffer);
        return false;
    }

    D3DSURFACE_DESC outputDesc = {};
    const bool haveOutputDesc = SUCCEEDED(outputSurface->GetDesc(&outputDesc));
    outputSurface->Release();
    if (!haveOutputDesc || outputDesc.Width == 0 || outputDesc.Height == 0)
    {
        ReleasePostFxGBuffer(&gbuffer);
        return false;
    }

    IDirect3DTexture9* aoDepthTexture = gbuffer.depth;
    PostFxDepthEncoding depthEncoding = gbuffer.depthEncoding;
    PostFxDepthView preferredDepth{};
    if (frozenPreviewGBuffer)
    {
        projectionDepthQ = gbuffer.projectionDepthQ;
        projectionDepthQn = gbuffer.projectionDepthQn;
    }
    else if (AcquirePostFxPreferredDepth(&preferredDepth))
    {
        if (preferredDepth.width == gbuffer.width &&
            preferredDepth.height == gbuffer.height)
        {
            aoDepthTexture = preferredDepth.texture;
            depthEncoding = preferredDepth.encoding;
            projectionDepthQ = preferredDepth.projectionDepthQ;
            projectionDepthQn = preferredDepth.projectionDepthQn;
        }
    }

    g_postFxNativeDepthActiveThisFrame.store(
        depthEncoding == PostFxDepthEncoding::NativeD24,
        std::memory_order_relaxed);

    UINT aoWidth = 0;
    UINT aoHeight = 0;
    GetPostFxScaledExtent(
        outputDesc.Width,
        outputDesc.Height,
        settings.resolutionDivisor,
        &aoWidth,
        &aoHeight);

    PostFxTargetView raw{};
    PostFxTargetView filtered{};
    if (!EnsurePostFxTarget(
            device,
            PostFxTargetSlot::AoRaw,
            aoWidth,
            aoHeight,
            D3DFMT_A8R8G8B8,
            &raw) ||
        !EnsurePostFxTarget(
            device,
            PostFxTargetSlot::AoFiltered,
            aoWidth,
            aoHeight,
            D3DFMT_A8R8G8B8,
            &filtered) ||
        !EnsurePostFxAoShaders(device))
    {
        ReleasePostFxDepthView(&preferredDepth);
        ReleasePostFxGBuffer(&gbuffer);
        return false;
    }

    PostFxStateBackup backup{};
    if (!BeginPostFxStateBackup(device, &backup))
    {
        ReleasePostFxDepthView(&preferredDepth);
        ReleasePostFxGBuffer(&gbuffer);
        return false;
    }

    const PostFxTextureBinding rawBindings[] =
    {
        { 0, aoDepthTexture, D3DTEXF_POINT, D3DTADDRESS_CLAMP },
        { 1, gbuffer.normal, D3DTEXF_POINT, D3DTADDRESS_CLAMP }
    };

    const float rawConstants[16] =
    {
        1.0f / static_cast<float>(std::max<UINT>(gbuffer.width, 1)),
        1.0f / static_cast<float>(std::max<UINT>(gbuffer.height, 1)),
        projectionScaleX,
        projectionScaleY,

        settings.radius,
        settings.strength,
        settings.bias,
        settings.power,

        static_cast<float>(aoWidth),
        static_cast<float>(aoHeight),
        settings.thickness,
        static_cast<float>(depthEncoding),

        projectionDepthQ,
        projectionDepthQn,
        0.0f,
        0.0f
    };

    // G-buffer channels and INTZ depth are data textures, never sRGB color.
    // Do not inherit arbitrary stage-0/1 sampler state from the game.
    if (g_originalSetSamplerState != nullptr)
    {
        g_originalSetSamplerState(device, 0, D3DSAMP_SRGBTEXTURE, FALSE);
        g_originalSetSamplerState(device, 1, D3DSAMP_SRGBTEXTURE, FALSE);
    }

    const bool rawOk = RunPostFxFullscreenPass(
        device,
        &backup,
        raw.surface,
        raw.width,
        raw.height,
        g_postFxAoRawShader,
        rawBindings,
        2,
        0,
        rawConstants,
        4,
        false,
        PostFxBlendMode::Opaque);

    bool filteredOk = false;
    if (rawOk)
    {
        const PostFxTextureBinding denoiseBindings[] =
        {
            { 0, raw.texture, D3DTEXF_POINT, D3DTADDRESS_CLAMP },
            { 1, aoDepthTexture, D3DTEXF_POINT, D3DTADDRESS_CLAMP },
            { 2, gbuffer.normal, D3DTEXF_POINT, D3DTADDRESS_CLAMP }
        };

        const float denoiseConstants[8] =
        {
            1.0f / static_cast<float>(std::max<UINT>(aoWidth, 1)),
            1.0f / static_cast<float>(std::max<UINT>(aoHeight, 1)),
            0.55f,
            static_cast<float>(depthEncoding),

            projectionDepthQ,
            projectionDepthQn,
            0.0f,
            0.0f
        };

        if (g_originalSetSamplerState != nullptr)
        {
            g_originalSetSamplerState(device, 0, D3DSAMP_SRGBTEXTURE, FALSE);
            g_originalSetSamplerState(device, 1, D3DSAMP_SRGBTEXTURE, FALSE);
            g_originalSetSamplerState(device, 2, D3DSAMP_SRGBTEXTURE, FALSE);
        }

        filteredOk = RunPostFxFullscreenPass(
            device,
            &backup,
            filtered.surface,
            filtered.width,
            filtered.height,
            g_postFxAoDenoiseShader,
            denoiseBindings,
            3,
            0,
            denoiseConstants,
            2,
            false,
            PostFxBlendMode::Opaque);
    }

    EndPostFxStateBackup(device, &backup);
    ReleasePostFxDepthView(&preferredDepth);
    ReleasePostFxGBuffer(&gbuffer);

    if (!rawOk || !filteredOk)
        return false;

    raw.texture->AddRef();
    filtered.texture->AddRef();
    state->rawAo = raw.texture;
    state->filteredAo = filtered.texture;
    state->aoWidth = aoWidth;
    state->aoHeight = aoHeight;
    state->mode = settings.mode;
    state->active = true;

    DWORD srgbWrite = FALSE;
    if (SUCCEEDED(device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbWrite)))
        state->srgbWrite = srgbWrite;

    g_postFxAoActiveThisFrame.store(true, std::memory_order_relaxed);
    g_postFxAoWidth.store(aoWidth, std::memory_order_relaxed);
    g_postFxAoHeight.store(aoHeight, std::memory_order_relaxed);
    g_postFxAoPreparedFrame.store(currentFrame, std::memory_order_relaxed);
    return true;
}

void EndPostFxAoFinalComposite(
    IDirect3DDevice9* device,
    PostFxAoFinalCompositeState* state)
{
    if (state == nullptr)
        return;

    const bool needsOutputPass =
        state->mode != PostFxAoMode::Composite || !state->hdrIntegrated;

    if (device != nullptr && state->active && needsOutputPass &&
        state->rawAo != nullptr && state->filteredAo != nullptr)
    {
        IDirect3DSurface9* output = nullptr;
        if (SUCCEEDED(device->GetRenderTarget(0, &output)) && output != nullptr)
        {
            D3DSURFACE_DESC outputDesc = {};
            if (SUCCEEDED(output->GetDesc(&outputDesc)))
            {
                IDirect3DTexture9* source =
                    state->mode == PostFxAoMode::ShowRaw
                        ? state->rawAo
                        : state->filteredAo;

                PostFxStateBackup backup{};
                if (BeginPostFxStateBackup(device, &backup))
                {
                    const PostFxTextureBinding binding =
                    {
                        0,
                        source,
                        D3DTEXF_LINEAR,
                        D3DTADDRESS_CLAMP
                    };

                    const PostFxBlendMode blendMode =
                        state->mode == PostFxAoMode::Composite
                            ? PostFxBlendMode::Multiply
                            : PostFxBlendMode::Opaque;

                    const float displayConstants[4] =
                    {
                        state->mode == PostFxAoMode::ShowEnhanced ? 6.0f : 1.0f,
                        0.0f,
                        0.0f,
                        0.0f
                    };

                    RunPostFxFullscreenPass(
                        device,
                        &backup,
                        output,
                        outputDesc.Width,
                        outputDesc.Height,
                        g_postFxAoDisplayShader,
                        &binding,
                        1,
                        0,
                        displayConstants,
                        1,
                        state->srgbWrite != FALSE,
                        blendMode);

                    EndPostFxStateBackup(device, &backup);
                }
            }
            output->Release();
        }
    }

    if (state->rawAo != nullptr)
        state->rawAo->Release();
    if (state->filteredAo != nullptr)
        state->filteredAo->Release();
    *state = {};
}

void ReleasePostFxAoResources()
{
    std::lock_guard<std::mutex> lock(g_postFxAoMutex);
    ReleasePostFxAoShadersUnlocked();
    g_postFxAoShaderOwner = nullptr;
    g_postFxProjectionScaleX = 0.0f;
    g_postFxProjectionScaleY = 0.0f;
    g_postFxAoProjectionDepthQ = 0.0f;
    g_postFxAoProjectionDepthQn = 0.0f;
    g_postFxProjectionFrame = 0;
    g_postFxNativeDepthActiveThisFrame.store(false, std::memory_order_relaxed);
    g_postFxAoWidth.store(0, std::memory_order_relaxed);
    g_postFxAoHeight.store(0, std::memory_order_relaxed);
    g_postFxAoActiveThisFrame.store(false, std::memory_order_relaxed);
}
