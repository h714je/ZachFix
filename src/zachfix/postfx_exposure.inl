// -----------------------------------------------------------------------------
// ZachFix PostFX NG: log-luminance exposure + filmic shoulder v1
// -----------------------------------------------------------------------------

namespace
{
std::mutex g_postFxExposureMutex;
std::atomic_uint g_postFxExposureMode{
    static_cast<UINT>(PostFxExposureMode::Legacy)
};
std::atomic<float> g_postFxExposureCompensationEv{ 0.0f };
std::atomic<float> g_postFxExposureMeterMinEv{ -10.0f };
std::atomic<float> g_postFxExposureMeterMaxEv{ 6.0f };
std::atomic<float> g_postFxExposureMinEv{ -8.0f };
std::atomic<float> g_postFxExposureMaxEv{ 4.0f };
std::atomic<float> g_postFxExposureBrightenSpeed{ 1.5f };
std::atomic<float> g_postFxExposureDarkenSpeed{ 3.0f };
std::atomic<float> g_postFxExposureShoulderStrength{ 1.0f };
std::atomic<float> g_postFxExposureWhitePoint{ 4.0f };
std::atomic_bool g_postFxExposureReplacementBound{ false };
std::atomic_bool g_postFxExposureShaderReady{ false };
std::atomic_bool g_postFxExposureMeterShadersReady{ false };
std::atomic_bool g_postFxExposureAdaptationInitialized{ false };
std::atomic_uint g_postFxExposureAdaptReadIndex{ 0 };
std::atomic_bool g_postFxExposureAdaptationResetRequested{ true };

std::atomic_uint g_postFxExposureMeterWidth{ 0 };
std::atomic_uint g_postFxExposureMeterHeight{ 0 };
std::atomic<float> g_postFxExposureFrameDeltaMs{ 0.0f };
std::atomic<float> g_postFxExposureGameKey{ 0.0f };
std::atomic_bool g_postFxExposureTelemetryAvailable{ false };
std::atomic_bool g_postFxExposureTelemetryReadbackFailed{ false };
std::atomic<float> g_postFxExposureTelemetryAverageLogLum{ 0.0f };
std::atomic<float> g_postFxExposureTelemetryTargetEv{ 0.0f };
std::atomic<float> g_postFxExposureTelemetryAdaptedEv{ 0.0f };
std::atomic_ullong g_postFxExposureLastAppliedFrame{ 0 };

IDirect3DDevice9* g_postFxExposureShaderOwner = nullptr; // borrowed
IDirect3DPixelShader9* g_postFxExposureShader = nullptr;
IDirect3DPixelShader9* g_postFxExposureMeterShader = nullptr;
IDirect3DPixelShader9* g_postFxExposureReduceShader = nullptr;
IDirect3DPixelShader9* g_postFxExposureFinalReduceShader = nullptr;
IDirect3DPixelShader9* g_postFxExposureAdaptShader = nullptr;

IDirect3DDevice9* g_postFxExposureTelemetryOwner = nullptr; // borrowed
IDirect3DSurface9* g_postFxExposureTelemetrySurface = nullptr;
bool g_postFxExposureTelemetryDisabled = false;

UINT g_postFxExposureSourceWidth = 0;
UINT g_postFxExposureSourceHeight = 0;
LARGE_INTEGER g_postFxExposureQpcFrequency = {};
LARGE_INTEGER g_postFxExposureLastQpc = {};

static const char kPostFxExposureShaderSource[] = R"HLSL(
sampler2D g_tDiffuse   : register(s0);
sampler2D g_tBloom     : register(s1);
sampler2D g_tLuminance : register(s2);
sampler2D g_tGaussian  : register(s3);
sampler2D g_tDepth      : register(s4);
sampler2D g_tZfExposure  : register(s5);
sampler2D g_tZfAO        : register(s6);
sampler2D g_tZfBloom     : register(s7);
sampler2D g_tZfDofNear   : register(s8);
sampler2D g_tZfDofFar    : register(s9);

float4 g_fBloomForce : register(c9);
float4 g_fExposure   : register(c10);
float4 g_vDofprm     : register(c15);
float4 g_fFocus      : register(c16);

// c26 = { dofMode, nearStrength, farStrength, unused }
// c27 = { customExposureEnable, unused, unused, unused }
// c28 = { shoulderEnable, shoulderStrength, whitePoint, unused }
// c29 = { hdrAoEnable, unused, unused, unused }
// c30 = { bloomMode: 0 legacy / 1 NG / 2 show, intensity, unused, unused }
float4 g_zfDof      : register(c26);
float4 g_zfExposure : register(c27);
float4 g_zfShoulder : register(c28);
float4 g_zfAO       : register(c29);
float4 g_zfBloom    : register(c30);

static const float3 kLuma = float3(0.2125, 0.7154, 0.0721);
static const float3 kGrade = float3(1.05, 0.97, 1.27);

float DecodeDepth(float4 packed)
{
    float depth = dot(packed.rgb, float3(65535.0, 255.0, 1.0));
    depth = saturate(depth / 255.0) * 255.0;
    return depth;
}

float3 ApplyLegacyGrade(float3 scene, float adaptedLuminance)
{
    float luminance = dot(scene, kLuma);
    float desaturate = saturate(1.0 - (adaptedLuminance + 1.5) * 0.243902445);
    float3 target = luminance * kGrade;
    return lerp(scene, target, desaturate);
}

float SmoothStep01(float x)
{
    x = saturate(x);
    return x * x * (3.0 - 2.0 * x);
}

float ApplyShoulderLuminance(float luminance, float strength, float whitePoint)
{
    luminance = max(luminance, 0.0);
    whitePoint = max(whitePoint, 1.05);

    float reinhard = luminance / (1.0 + luminance);
    float whiteNorm = whitePoint / (1.0 + whitePoint);
    float filmic = reinhard / max(whiteNorm, 1e-5);

    float highlightMask = SmoothStep01((luminance - 0.55) / 0.45);
    float blend = saturate(strength) * highlightMask;
    return lerp(luminance, filmic, blend);
}

float3 ApplyShoulder(float3 color, float strength, float whitePoint)
{
    color = max(color, 0.0);
    float luminance = dot(color, kLuma);
    if (luminance <= 1e-5)
        return color;

    float mapped = ApplyShoulderLuminance(luminance, strength, whitePoint);
    return color * (mapped / luminance);
}

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float4 diffuse = tex2D(g_tDiffuse, uv);
    float depth = DecodeDepth(tex2D(g_tDepth, uv));
    float nearCoc = max(saturate(g_vDofprm.x - depth * g_vDofprm.y), saturate(g_fFocus.x));
    float farCoc = max(saturate(1.0 - (g_vDofprm.z - depth * g_vDofprm.w)), saturate(g_fFocus.x));

    float aoValue = 1.0;
    if (g_zfAO.x > 0.5)
        aoValue = saturate(tex2D(g_tZfAO, uv).r);

    float3 scene = diffuse.rgb;
    if (g_zfDof.x > 0.5)
    {
        // DoF NG layers are generated from AO-modulated HDR. Apply the same
        // AO to the sharp center sample before mixing so focused pixels and
        // blurred pixels live in the same scene-linear space.
        if (g_zfDof.w > 0.5)
            scene *= aoValue;
        float4 nearLayer = tex2D(g_tZfDofNear, uv);
        float4 farLayer = tex2D(g_tZfDofFar, uv);

        if (g_zfDof.x > 1.5 && g_zfDof.x < 2.5)
            return float4(nearCoc, 0.0, farCoc, 1.0);
        if (g_zfDof.x > 2.5 && g_zfDof.x < 3.5)
        {
            float3 preview = max(nearLayer.rgb, 0.0);
            preview = preview / (1.0 + preview);
            return float4(preview, saturate(nearLayer.a));
        }
        if (g_zfDof.x > 3.5)
        {
            float3 preview = max(farLayer.rgb, 0.0);
            preview = preview / (1.0 + preview);
            return float4(preview, 1.0);
        }

        float farMix = saturate(farCoc * max(g_zfDof.z, 0.0));
        scene = lerp(scene, farLayer.rgb, farMix);
        float nearMix = saturate(max(nearCoc, nearLayer.a) * max(g_zfDof.y, 0.0));
        scene = lerp(scene, nearLayer.rgb, nearMix);
    }
    else
    {
        float3 gaussian = tex2D(g_tGaussian, uv).rgb;
        float dofFactor = max(nearCoc, farCoc);
        scene = lerp(diffuse.rgb, gaussian, dofFactor);
    }

    // GTAO is a scene-linear lighting modulation. Keep it before DP's legacy
    // grading and exposure so the post stack can remap the occluded HDR result
    // naturally. Legacy bloom is still added later and will be replaced by
    // Bloom NG in a subsequent stage.
    if (g_zfAO.x > 0.5 && g_zfDof.w < 0.5)
        scene *= aoValue;

    float adaptedLuminance = tex2D(g_tLuminance, float2(0.5, 0.5)).x;
    float3 graded = ApplyLegacyGrade(scene, adaptedLuminance);

    float gain = g_fExposure.x / max(adaptedLuminance + 0.001, 1e-5);
    if (g_zfExposure.x > 0.5)
    {
        float adaptedEv = tex2D(g_tZfExposure, float2(0.5, 0.5)).x;
        gain = exp2(adaptedEv);
    }

    float4 legacyBloom = tex2D(g_tBloom, uv) * g_fBloomForce.x;
    float3 hdr = graded * gain;
    float bloomAlpha = legacyBloom.a;

    if (g_zfBloom.x > 0.5)
    {
        // Bloom NG is scene-linear and therefore receives the same exposure
        // gain as the scene. DP's authored g_fBloomForce remains the scene
        // bloom key; the ZachFix intensity is an additional user multiplier.
        float3 bloomNg = max(tex2D(g_tZfBloom, uv).rgb, 0.0);
        bloomNg *= g_fBloomForce.x * max(g_zfBloom.y, 0.0) * gain;

        if (g_zfBloom.x > 1.5)
        {
            // Display-only diagnostic preview. Compress HDR bloom so the
            // pyramid structure can be inspected on an LDR backbuffer.
            float3 preview = bloomNg / (1.0 + bloomNg);
            return float4(saturate(preview), 1.0);
        }

        hdr += bloomNg;
        bloomAlpha = 0.0;
    }
    else
    {
        hdr += legacyBloom.rgb;
    }

    if (g_zfShoulder.x > 0.5)
        hdr = ApplyShoulder(hdr, g_zfShoulder.y, g_zfShoulder.z);

    float alpha = diffuse.a * gain + bloomAlpha;
    return float4(hdr, alpha);
}
)HLSL";

static const char kPostFxExposureMeterShaderSource[] = R"HLSL(
sampler2D g_tHdr : register(s0);
sampler2D g_tAO  : register(s1);
float4 g_zfMeter : register(c0); // inv output size, meter min/max log2 luminance
float4 g_zfMeterAo : register(c1); // x = apply filtered AO before metering
static const float3 kLuma = float3(0.2126, 0.7152, 0.0722);

float LogLuminance(float2 uv)
{
    float3 color = max(tex2D(g_tHdr, uv).rgb, 0.0);
    if (g_zfMeterAo.x > 0.5)
        color *= saturate(tex2D(g_tAO, uv).r);
    float lum = max(dot(color, kLuma), 1e-5);
    return clamp(log2(lum), g_zfMeter.z, g_zfMeter.w);
}

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float2 offset = 0.25 * g_zfMeter.xy;
    float sum = 0.0;
    sum += LogLuminance(uv + float2(-offset.x, -offset.y));
    sum += LogLuminance(uv + float2( offset.x, -offset.y));
    sum += LogLuminance(uv + float2(-offset.x,  offset.y));
    sum += LogLuminance(uv + float2( offset.x,  offset.y));
    float average = sum * 0.25;
    return float4(average, average, average, 1.0);
}
)HLSL";

static const char kPostFxExposureReduceShaderSource[] = R"HLSL(
sampler2D g_tInput : register(s0);
float4 g_zfReduce : register(c0); // inv output size

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float2 offset = 0.25 * g_zfReduce.xy;
    float sum = 0.0;
    sum += tex2D(g_tInput, uv + float2(-offset.x, -offset.y)).x;
    sum += tex2D(g_tInput, uv + float2( offset.x, -offset.y)).x;
    sum += tex2D(g_tInput, uv + float2(-offset.x,  offset.y)).x;
    sum += tex2D(g_tInput, uv + float2( offset.x,  offset.y)).x;
    float average = sum * 0.25;
    return float4(average, average, average, 1.0);
}
)HLSL";

static const char kPostFxExposureFinalReduceShaderSource[] = R"HLSL(
sampler2D g_tInput : register(s0);

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float sum = 0.0;
    sum += tex2D(g_tInput, float2(0.125, 1.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.375, 1.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.625, 1.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.875, 1.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.125, 0.5)).x;
    sum += tex2D(g_tInput, float2(0.375, 0.5)).x;
    sum += tex2D(g_tInput, float2(0.625, 0.5)).x;
    sum += tex2D(g_tInput, float2(0.875, 0.5)).x;
    sum += tex2D(g_tInput, float2(0.125, 5.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.375, 5.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.625, 5.0 / 6.0)).x;
    sum += tex2D(g_tInput, float2(0.875, 5.0 / 6.0)).x;
    float average = sum / 12.0;
    return float4(average, average, average, 1.0);
}
)HLSL";

static const char kPostFxExposureAdaptShaderSource[] = R"HLSL(
sampler2D g_tAverageLogLum : register(s0);
sampler2D g_tPreviousEv    : register(s1);

// c0 = compensation EV, min exposure EV, max exposure EV, delta seconds
// c1 = brighten speed, darken speed, initialized, log2(game exposure key)
float4 g_zfAdapt0 : register(c0);
float4 g_zfAdapt1 : register(c1);

float4 main(float2 uv : TEXCOORD0) : COLOR0
{
    float averageLogLum = tex2D(g_tAverageLogLum, float2(0.5, 0.5)).x;
    float targetEv = g_zfAdapt1.w - averageLogLum + g_zfAdapt0.x;
    targetEv = clamp(targetEv, g_zfAdapt0.y, g_zfAdapt0.z);

    float previousEv = tex2D(g_tPreviousEv, float2(0.5, 0.5)).x;
    if (g_zfAdapt1.z < 0.5)
        previousEv = targetEv;

    float speed = targetEv > previousEv ? g_zfAdapt1.x : g_zfAdapt1.y;
    float alpha = 1.0 - exp2(-1.442695041 * max(speed, 0.0) * g_zfAdapt0.w);
    float adaptedEv = lerp(previousEv, targetEv, saturate(alpha));
    return float4(adaptedEv, targetEv, averageLogLum, 1.0);
}
)HLSL";

bool IsPostFxCustomExposureMode(PostFxExposureMode mode)
{
    return mode == PostFxExposureMode::ExposureOnly ||
           mode == PostFxExposureMode::ExposureAndShoulder;
}

void ReleasePostFxExposureShader(IDirect3DPixelShader9*& shader)
{
    if (shader != nullptr)
    {
        shader->Release();
        shader = nullptr;
    }
}

void ReleasePostFxExposureShadersUnlocked()
{
    ReleasePostFxExposureShader(g_postFxExposureShader);
    ReleasePostFxExposureShader(g_postFxExposureMeterShader);
    ReleasePostFxExposureShader(g_postFxExposureReduceShader);
    ReleasePostFxExposureShader(g_postFxExposureFinalReduceShader);
    ReleasePostFxExposureShader(g_postFxExposureAdaptShader);
    g_postFxExposureShaderOwner = nullptr;
    g_postFxExposureShaderReady.store(false, std::memory_order_relaxed);
    g_postFxExposureMeterShadersReady.store(false, std::memory_order_relaxed);
}

void ReleasePostFxExposureTelemetryUnlocked()
{
    if (g_postFxExposureTelemetrySurface != nullptr)
    {
        g_postFxExposureTelemetrySurface->Release();
        g_postFxExposureTelemetrySurface = nullptr;
    }
    g_postFxExposureTelemetryOwner = nullptr;
    g_postFxExposureTelemetryDisabled = false;
    g_postFxExposureTelemetryAvailable.store(false, std::memory_order_relaxed);
    g_postFxExposureTelemetryReadbackFailed.store(false, std::memory_order_relaxed);
}

float ClampExposureFloat(float value, float minValue, float maxValue)
{
    if (!std::isfinite(value))
        return minValue;
    return std::max(minValue, std::min(maxValue, value));
}

float HalfToFloatExposure(unsigned short value)
{
    const unsigned sign = (value >> 15) & 1u;
    const unsigned exponent = (value >> 10) & 0x1Fu;
    const unsigned mantissa = value & 0x03FFu;

    unsigned bits = sign << 31;
    if (exponent == 0)
    {
        if (mantissa == 0)
        {
            // Signed zero.
        }
        else
        {
            unsigned m = mantissa;
            int e = -14;
            while ((m & 0x0400u) == 0)
            {
                m <<= 1;
                --e;
            }
            m &= 0x03FFu;
            bits |= static_cast<unsigned>(e + 127) << 23;
            bits |= m << 13;
        }
    }
    else if (exponent == 31)
    {
        bits |= 0x7F800000u | (mantissa << 13);
    }
    else
    {
        bits |= (exponent - 15u + 127u) << 23;
        bits |= mantissa << 13;
    }

    float result = 0.0f;
    memcpy(&result, &bits, sizeof(result));
    return result;
}

float GetPostFxExposureDeltaSeconds()
{
    if (g_postFxExposureQpcFrequency.QuadPart == 0)
        QueryPerformanceFrequency(&g_postFxExposureQpcFrequency);

    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);

    float delta = 1.0f / 60.0f;
    if (g_postFxExposureLastQpc.QuadPart != 0 &&
        g_postFxExposureQpcFrequency.QuadPart > 0)
    {
        delta = static_cast<float>(
            static_cast<double>(now.QuadPart - g_postFxExposureLastQpc.QuadPart) /
            static_cast<double>(g_postFxExposureQpcFrequency.QuadPart));
    }
    g_postFxExposureLastQpc = now;
    return ClampExposureFloat(delta, 1.0f / 240.0f, 0.25f);
}

bool EnsurePostFxExposureFinalShaderUnlocked(IDirect3DDevice9* device)
{
    if (g_postFxExposureShader != nullptr)
        return true;

    AppendLog("[PostFX][Exposure] Compiling Exposure v1 final composite.\n");
    IDirect3DPixelShader9* shader = nullptr;
    if (!CompilePostFxPixelShader(
            device,
            kPostFxExposureShaderSource,
            "main",
            "ExposureV1FinalComposite",
            &shader))
    {
        return false;
    }

    g_postFxExposureShader = shader;
    g_postFxExposureShaderReady.store(true, std::memory_order_relaxed);
    AppendLog("[PostFX][Exposure] Exposure v1 final-composite shader compiled.\n");
    return true;
}

bool EnsurePostFxExposureMeterShaders(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(g_postFxExposureMutex);
    if (g_postFxExposureShaderOwner != nullptr &&
        g_postFxExposureShaderOwner != device)
    {
        ReleasePostFxExposureShadersUnlocked();
        ReleasePostFxExposureTelemetryUnlocked();
        g_postFxExposureAdaptationInitialized.store(false, std::memory_order_relaxed);
        g_postFxExposureAdaptationResetRequested.store(true, std::memory_order_relaxed);
    }
    g_postFxExposureShaderOwner = device;

    if (g_postFxExposureMeterShader != nullptr &&
        g_postFxExposureReduceShader != nullptr &&
        g_postFxExposureFinalReduceShader != nullptr &&
        g_postFxExposureAdaptShader != nullptr)
    {
        g_postFxExposureMeterShadersReady.store(true, std::memory_order_relaxed);
        return true;
    }

    AppendLog("[PostFX][Exposure] Compiling Exposure v1 log-luminance meter shaders.\n");

    IDirect3DPixelShader9* meter = nullptr;
    IDirect3DPixelShader9* reduce = nullptr;
    IDirect3DPixelShader9* finalReduce = nullptr;
    IDirect3DPixelShader9* adapt = nullptr;

    const bool ok =
        CompilePostFxPixelShader(
            device, kPostFxExposureMeterShaderSource,
            "main", "ExposureV1Meter", &meter) &&
        CompilePostFxPixelShader(
            device, kPostFxExposureReduceShaderSource,
            "main", "ExposureV1Reduce", &reduce) &&
        CompilePostFxPixelShader(
            device, kPostFxExposureFinalReduceShaderSource,
            "main", "ExposureV1FinalReduce", &finalReduce) &&
        CompilePostFxPixelShader(
            device, kPostFxExposureAdaptShaderSource,
            "main", "ExposureV1Adapt", &adapt);

    if (!ok)
    {
        ReleasePostFxExposureShader(meter);
        ReleasePostFxExposureShader(reduce);
        ReleasePostFxExposureShader(finalReduce);
        ReleasePostFxExposureShader(adapt);
        return false;
    }

    g_postFxExposureMeterShader = meter;
    g_postFxExposureReduceShader = reduce;
    g_postFxExposureFinalReduceShader = finalReduce;
    g_postFxExposureAdaptShader = adapt;
    g_postFxExposureMeterShadersReady.store(true, std::memory_order_relaxed);
    AppendLog("[PostFX][Exposure] Exposure v1 meter/adaptation shaders compiled.\n");
    return true;
}

bool EnsurePostFxExposureTelemetrySurface(IDirect3DDevice9* device)
{
    if (device == nullptr || g_postFxExposureTelemetryDisabled)
        return false;

    std::lock_guard<std::mutex> lock(g_postFxExposureMutex);
    if (g_postFxExposureTelemetryOwner == device &&
        g_postFxExposureTelemetrySurface != nullptr)
    {
        return true;
    }

    ReleasePostFxExposureTelemetryUnlocked();
    IDirect3DSurface9* surface = nullptr;
    const HRESULT result = device->CreateOffscreenPlainSurface(
        1,
        1,
        D3DFMT_A16B16G16R16F,
        D3DPOOL_SYSTEMMEM,
        &surface,
        nullptr);
    if (FAILED(result) || surface == nullptr)
    {
        g_postFxExposureTelemetryDisabled = true;
        g_postFxExposureTelemetryReadbackFailed.store(true, std::memory_order_relaxed);
        AppendLog("[PostFX][Exposure] Telemetry readback unavailable; exposure itself remains active.\n");
        return false;
    }

    g_postFxExposureTelemetryOwner = device;
    g_postFxExposureTelemetrySurface = surface;
    return true;
}

void UpdatePostFxExposureTelemetry(
    IDirect3DDevice9* device,
    IDirect3DSurface9* source,
    unsigned long long frame)
{
    if (device == nullptr || source == nullptr || (frame % 15ull) != 0ull)
        return;
    if (!EnsurePostFxExposureTelemetrySurface(device))
        return;

    if (FAILED(device->GetRenderTargetData(source, g_postFxExposureTelemetrySurface)))
    {
        if (!g_postFxExposureTelemetryDisabled)
            AppendLog("[PostFX][Exposure] Telemetry GetRenderTargetData failed; disabling readback.\n");
        g_postFxExposureTelemetryDisabled = true;
        g_postFxExposureTelemetryReadbackFailed.store(true, std::memory_order_relaxed);
        return;
    }

    D3DLOCKED_RECT locked = {};
    if (FAILED(g_postFxExposureTelemetrySurface->LockRect(&locked, nullptr, D3DLOCK_READONLY)) ||
        locked.pBits == nullptr)
    {
        g_postFxExposureTelemetryDisabled = true;
        g_postFxExposureTelemetryReadbackFailed.store(true, std::memory_order_relaxed);
        return;
    }

    const unsigned short* values = static_cast<const unsigned short*>(locked.pBits);
    const float adaptedEv = HalfToFloatExposure(values[0]);
    const float targetEv = HalfToFloatExposure(values[1]);
    const float averageLogLum = HalfToFloatExposure(values[2]);
    g_postFxExposureTelemetrySurface->UnlockRect();

    if (std::isfinite(adaptedEv) && std::isfinite(targetEv) &&
        std::isfinite(averageLogLum))
    {
        g_postFxExposureTelemetryAdaptedEv.store(adaptedEv, std::memory_order_relaxed);
        g_postFxExposureTelemetryTargetEv.store(targetEv, std::memory_order_relaxed);
        g_postFxExposureTelemetryAverageLogLum.store(averageLogLum, std::memory_order_relaxed);
        g_postFxExposureTelemetryAvailable.store(true, std::memory_order_relaxed);
    }
}

UINT QuarterExtent(UINT value)
{
    return std::max<UINT>(1, (value + 3u) / 4u);
}

bool PreparePostFxCustomExposure(
    IDirect3DDevice9* device,
    const PostFxExposureSettings& settings,
    IDirect3DTexture9* filteredAo,
    IDirect3DTexture9** adaptedTexture)
{
    if (adaptedTexture != nullptr)
        *adaptedTexture = nullptr;
    if (device == nullptr || adaptedTexture == nullptr ||
        !EnsurePostFxExposureMeterShaders(device))
    {
        return false;
    }

    IDirect3DBaseTexture9* baseHdr = nullptr;
    if (FAILED(device->GetTexture(0, &baseHdr)) || baseHdr == nullptr ||
        baseHdr->GetType() != D3DRTYPE_TEXTURE)
    {
        if (baseHdr != nullptr)
            baseHdr->Release();
        return false;
    }

    IDirect3DTexture9* hdr = nullptr;
    const HRESULT queryResult = baseHdr->QueryInterface(
        __uuidof(IDirect3DTexture9),
        reinterpret_cast<void**>(&hdr));
    baseHdr->Release();
    if (FAILED(queryResult) || hdr == nullptr)
        return false;

    D3DSURFACE_DESC hdrDesc = {};
    if (FAILED(hdr->GetLevelDesc(0, &hdrDesc)) ||
        hdrDesc.Width == 0 || hdrDesc.Height == 0)
    {
        hdr->Release();
        return false;
    }

    const UINT meter0Width = std::min<UINT>(256, hdrDesc.Width);
    const UINT meter0Height = std::max<UINT>(
        1,
        static_cast<UINT>(
            (static_cast<unsigned long long>(hdrDesc.Height) * meter0Width +
             hdrDesc.Width / 2u) /
            hdrDesc.Width));
    const UINT meter1Width = QuarterExtent(meter0Width);
    const UINT meter1Height = QuarterExtent(meter0Height);
    const UINT meter2Width = QuarterExtent(meter1Width);
    const UINT meter2Height = QuarterExtent(meter1Height);
    // Keep the last pre-1x1 level fixed at 4x3 so the final 12-tap
    // reduction weights every texel exactly, independent of aspect ratio.
    const UINT meter3Width = 4;
    const UINT meter3Height = 3;

    PostFxTargetView meter[5] = {};
    const D3DFORMAT meterFormat = D3DFMT_A16B16G16R16F;
    const bool targetsReady =
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureMeter0,
            meter0Width, meter0Height, meterFormat, &meter[0]) &&
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureMeter1,
            meter1Width, meter1Height, meterFormat, &meter[1]) &&
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureMeter2,
            meter2Width, meter2Height, meterFormat, &meter[2]) &&
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureMeter3,
            meter3Width, meter3Height, meterFormat, &meter[3]) &&
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureMeter4,
            1, 1, meterFormat, &meter[4]);

    PostFxTargetView adapt[2] = {};
    const bool adaptReady =
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureAdapt0,
            1, 1, meterFormat, &adapt[0]) &&
        EnsurePostFxTarget(device, PostFxTargetSlot::ExposureAdapt1,
            1, 1, meterFormat, &adapt[1]);

    if (!targetsReady || !adaptReady)
    {
        hdr->Release();
        return false;
    }

    float meterMinEv = settings.meterMinEv;
    float meterMaxEv = settings.meterMaxEv;
    if (meterMinEv > meterMaxEv)
        std::swap(meterMinEv, meterMaxEv);

    float minExposureEv = settings.minExposureEv;
    float maxExposureEv = settings.maxExposureEv;
    if (minExposureEv > maxExposureEv)
        std::swap(minExposureEv, maxExposureEv);

    float exposureConstant[4] = {};
    float gameKey = 0.18f;
    if (SUCCEEDED(device->GetPixelShaderConstantF(10, exposureConstant, 1)) &&
        std::isfinite(exposureConstant[0]) && exposureConstant[0] > 1e-5f)
    {
        gameKey = exposureConstant[0];
    }
    const float gameKeyLog2 = std::log2(std::max(gameKey, 1e-5f));
    const float deltaSeconds = GetPostFxExposureDeltaSeconds();

    if (g_postFxExposureSourceWidth != hdrDesc.Width ||
        g_postFxExposureSourceHeight != hdrDesc.Height)
    {
        g_postFxExposureSourceWidth = hdrDesc.Width;
        g_postFxExposureSourceHeight = hdrDesc.Height;
        g_postFxExposureAdaptationResetRequested.store(true, std::memory_order_relaxed);
    }

    if (g_postFxExposureAdaptationResetRequested.exchange(
            false, std::memory_order_acq_rel))
    {
        g_postFxExposureAdaptationInitialized.store(false, std::memory_order_relaxed);
        g_postFxExposureTelemetryAvailable.store(false, std::memory_order_relaxed);
        g_postFxExposureLastQpc = {};
    }

    bool initialized =
        g_postFxExposureAdaptationInitialized.load(std::memory_order_relaxed);
    UINT readIndex =
        std::min<UINT>(g_postFxExposureAdaptReadIndex.load(std::memory_order_relaxed), 1u);
    const UINT writeIndex = initialized ? (1u - readIndex) : 0u;

    PostFxStateBackup backup{};
    if (!BeginPostFxStateBackup(device, &backup))
    {
        hdr->Release();
        return false;
    }

    IDirect3DTexture9* meterAo = filteredAo != nullptr ? filteredAo : hdr;
    const PostFxTextureBinding firstBindings[] =
    {
        { 0, hdr, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP },
        { 1, meterAo, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP }
    };
    const float firstConstants[8] =
    {
        1.0f / static_cast<float>(meter0Width),
        1.0f / static_cast<float>(meter0Height),
        meterMinEv,
        meterMaxEv,

        filteredAo != nullptr ? 1.0f : 0.0f,
        0.0f,
        0.0f,
        0.0f
    };
    bool ok = RunPostFxFullscreenPass(
        device,
        &backup,
        meter[0].surface,
        meter[0].width,
        meter[0].height,
        g_postFxExposureMeterShader,
        firstBindings,
        2,
        0,
        firstConstants,
        2,
        false,
        PostFxBlendMode::Opaque);

    for (UINT i = 1; ok && i <= 3; ++i)
    {
        const PostFxTextureBinding binding =
        {
            0, meter[i - 1].texture, D3DTEXF_LINEAR, D3DTADDRESS_CLAMP
        };
        const float constants[4] =
        {
            1.0f / static_cast<float>(meter[i].width),
            1.0f / static_cast<float>(meter[i].height),
            0.0f,
            0.0f
        };
        ok = RunPostFxFullscreenPass(
            device,
            &backup,
            meter[i].surface,
            meter[i].width,
            meter[i].height,
            g_postFxExposureReduceShader,
            &binding,
            1,
            0,
            constants,
            1,
            false,
            PostFxBlendMode::Opaque);
    }

    if (ok)
    {
        const PostFxTextureBinding binding =
        {
            0, meter[3].texture, D3DTEXF_POINT, D3DTADDRESS_CLAMP
        };
        ok = RunPostFxFullscreenPass(
            device,
            &backup,
            meter[4].surface,
            1,
            1,
            g_postFxExposureFinalReduceShader,
            &binding,
            1,
            0,
            nullptr,
            0,
            false,
            PostFxBlendMode::Opaque);
    }

    if (ok)
    {
        const IDirect3DTexture9* previousTextureConst =
            initialized ? adapt[readIndex].texture : meter[4].texture;
        IDirect3DTexture9* previousTexture =
            const_cast<IDirect3DTexture9*>(previousTextureConst);
        const PostFxTextureBinding bindings[] =
        {
            { 0, meter[4].texture, D3DTEXF_POINT, D3DTADDRESS_CLAMP },
            { 1, previousTexture, D3DTEXF_POINT, D3DTADDRESS_CLAMP }
        };
        const float constants[8] =
        {
            settings.compensationEv,
            minExposureEv,
            maxExposureEv,
            deltaSeconds,

            settings.brightenSpeed,
            settings.darkenSpeed,
            initialized ? 1.0f : 0.0f,
            gameKeyLog2
        };
        ok = RunPostFxFullscreenPass(
            device,
            &backup,
            adapt[writeIndex].surface,
            1,
            1,
            g_postFxExposureAdaptShader,
            bindings,
            2,
            0,
            constants,
            2,
            false,
            PostFxBlendMode::Opaque);
    }

    EndPostFxStateBackup(device, &backup);
    hdr->Release();

    if (!ok)
        return false;

    g_postFxExposureAdaptReadIndex.store(writeIndex, std::memory_order_relaxed);
    g_postFxExposureAdaptationInitialized.store(true, std::memory_order_relaxed);
    g_postFxExposureMeterWidth.store(meter0Width, std::memory_order_relaxed);
    g_postFxExposureMeterHeight.store(meter0Height, std::memory_order_relaxed);
    g_postFxExposureFrameDeltaMs.store(deltaSeconds * 1000.0f, std::memory_order_relaxed);
    g_postFxExposureGameKey.store(gameKey, std::memory_order_relaxed);

    const unsigned long long frame = GetPostFxFrameIndex();
    UpdatePostFxExposureTelemetry(device, adapt[writeIndex].surface, frame);

    adapt[writeIndex].texture->AddRef();
    *adaptedTexture = adapt[writeIndex].texture;
    return true;
}

void RestorePostFxDofFrozenSceneBindings(
    IDirect3DDevice9* device,
    PostFxExposureDrawState* state)
{
    if (device == nullptr || state == nullptr)
        return;

    if (state->changedFrozenSceneConstants && g_originalSetPixelShaderConstantF != nullptr)
    {
        g_originalSetPixelShaderConstantF(device, 9, state->previousConstants9, 1);
        g_originalSetPixelShaderConstantF(device, 10, state->previousConstants10, 1);
        g_originalSetPixelShaderConstantF(device, 15, state->previousConstants15, 1);
        g_originalSetPixelShaderConstantF(device, 16, state->previousConstants16, 1);
        state->changedFrozenSceneConstants = false;
    }

    if (g_originalSetTexture != nullptr)
    {
        if (state->changedStage4) g_originalSetTexture(device, 4, state->previousStage4);
        if (state->changedStage3) g_originalSetTexture(device, 3, state->previousStage3);
        if (state->changedStage2) g_originalSetTexture(device, 2, state->previousStage2);
        if (state->changedStage1) g_originalSetTexture(device, 1, state->previousStage1);
        if (state->changedStage0) g_originalSetTexture(device, 0, state->previousStage0);
    }
    state->changedStage0 = false;
    state->changedStage1 = false;
    state->changedStage2 = false;
    state->changedStage3 = false;
    state->changedStage4 = false;
}

void ReleasePostFxDofFrozenScenePreviousRefs(PostFxExposureDrawState* state)
{
    if (state == nullptr)
        return;
    IDirect3DBaseTexture9** textures[] =
    {
        &state->previousStage0,
        &state->previousStage1,
        &state->previousStage2,
        &state->previousStage3,
        &state->previousStage4
    };
    for (IDirect3DBaseTexture9** texture : textures)
    {
        if (*texture != nullptr)
        {
            (*texture)->Release();
            *texture = nullptr;
        }
    }
}

bool BindPostFxDofFrozenScene(
    IDirect3DDevice9* device,
    const PostFxDofFreezeView& frozen,
    PostFxExposureDrawState* state)
{
    if (device == nullptr || state == nullptr || frozen.diffuse == nullptr ||
        frozen.depth == nullptr || g_originalSetTexture == nullptr ||
        g_originalSetPixelShaderConstantF == nullptr)
    {
        return false;
    }

    IDirect3DBaseTexture9** previous[] =
    {
        &state->previousStage0,
        &state->previousStage1,
        &state->previousStage2,
        &state->previousStage3,
        &state->previousStage4
    };
    for (UINT stage = 0; stage < 5; ++stage)
    {
        if (FAILED(device->GetTexture(stage, previous[stage])))
        {
            ReleasePostFxDofFrozenScenePreviousRefs(state);
            return false;
        }
    }

    if (FAILED(device->GetPixelShaderConstantF(9, state->previousConstants9, 1)) ||
        FAILED(device->GetPixelShaderConstantF(10, state->previousConstants10, 1)) ||
        FAILED(device->GetPixelShaderConstantF(15, state->previousConstants15, 1)) ||
        FAILED(device->GetPixelShaderConstantF(16, state->previousConstants16, 1)))
    {
        ReleasePostFxDofFrozenScenePreviousRefs(state);
        return false;
    }

    IDirect3DTexture9* frozenStages[5] =
    {
        frozen.diffuse,
        frozen.bloom,
        frozen.luminance,
        frozen.gaussian,
        frozen.depth
    };
    bool ok = true;
    for (UINT stage = 0; stage < 5; ++stage)
    {
        if (frozenStages[stage] == nullptr)
            continue;
        if (FAILED(g_originalSetTexture(device, stage, frozenStages[stage])))
        {
            ok = false;
            break;
        }
        if (stage == 0) state->changedStage0 = true;
        if (stage == 1) state->changedStage1 = true;
        if (stage == 2) state->changedStage2 = true;
        if (stage == 3) state->changedStage3 = true;
        if (stage == 4) state->changedStage4 = true;
    }

    if (ok)
    {
        state->changedFrozenSceneConstants = true;
        ok = SUCCEEDED(g_originalSetPixelShaderConstantF(device, 9, frozen.bloomForce, 1)) &&
            SUCCEEDED(g_originalSetPixelShaderConstantF(device, 10, frozen.exposure, 1)) &&
            SUCCEEDED(g_originalSetPixelShaderConstantF(device, 15, frozen.dofPrm, 1)) &&
            SUCCEEDED(g_originalSetPixelShaderConstantF(device, 16, frozen.focus, 1));
    }

    if (!ok)
    {
        RestorePostFxDofFrozenSceneBindings(device, state);
        ReleasePostFxDofFrozenScenePreviousRefs(state);
    }
    return ok;
}
} // namespace

PostFxExposureSettings GetPostFxExposureSettings()
{
    PostFxExposureSettings settings{};
    settings.mode = static_cast<PostFxExposureMode>(
        g_postFxExposureMode.load(std::memory_order_relaxed));
    settings.compensationEv =
        g_postFxExposureCompensationEv.load(std::memory_order_relaxed);
    settings.meterMinEv =
        g_postFxExposureMeterMinEv.load(std::memory_order_relaxed);
    settings.meterMaxEv =
        g_postFxExposureMeterMaxEv.load(std::memory_order_relaxed);
    settings.minExposureEv =
        g_postFxExposureMinEv.load(std::memory_order_relaxed);
    settings.maxExposureEv =
        g_postFxExposureMaxEv.load(std::memory_order_relaxed);
    settings.brightenSpeed =
        g_postFxExposureBrightenSpeed.load(std::memory_order_relaxed);
    settings.darkenSpeed =
        g_postFxExposureDarkenSpeed.load(std::memory_order_relaxed);
    settings.shoulderStrength =
        g_postFxExposureShoulderStrength.load(std::memory_order_relaxed);
    settings.whitePoint =
        g_postFxExposureWhitePoint.load(std::memory_order_relaxed);
    return settings;
}

void SetPostFxExposureMode(PostFxExposureMode mode)
{
    const UINT sanitized = std::min<UINT>(
        static_cast<UINT>(mode),
        static_cast<UINT>(PostFxExposureMode::ExposureAndShoulder));
    const UINT previous = g_postFxExposureMode.exchange(
        sanitized, std::memory_order_relaxed);

    if (!IsPostFxCustomExposureMode(static_cast<PostFxExposureMode>(previous)) &&
        IsPostFxCustomExposureMode(static_cast<PostFxExposureMode>(sanitized)))
    {
        RequestPostFxExposureAdaptationReset();
    }
}

void SetPostFxExposureCompensationEv(float ev)
{
    g_postFxExposureCompensationEv.store(
        ClampExposureFloat(ev, -6.0f, 6.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMeterMinEv(float ev)
{
    g_postFxExposureMeterMinEv.store(
        ClampExposureFloat(ev, -16.0f, 8.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMeterMaxEv(float ev)
{
    g_postFxExposureMeterMaxEv.store(
        ClampExposureFloat(ev, -8.0f, 16.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMinEv(float ev)
{
    g_postFxExposureMinEv.store(
        ClampExposureFloat(ev, -12.0f, 4.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMaxEv(float ev)
{
    g_postFxExposureMaxEv.store(
        ClampExposureFloat(ev, -4.0f, 12.0f), std::memory_order_relaxed);
}

void SetPostFxExposureBrightenSpeed(float speed)
{
    g_postFxExposureBrightenSpeed.store(
        ClampExposureFloat(speed, 0.05f, 10.0f), std::memory_order_relaxed);
}

void SetPostFxExposureDarkenSpeed(float speed)
{
    g_postFxExposureDarkenSpeed.store(
        ClampExposureFloat(speed, 0.05f, 10.0f), std::memory_order_relaxed);
}

void SetPostFxExposureShoulderStrength(float strength)
{
    g_postFxExposureShoulderStrength.store(
        ClampExposureFloat(strength, 0.0f, 1.0f), std::memory_order_relaxed);
}

void SetPostFxExposureWhitePoint(float whitePoint)
{
    g_postFxExposureWhitePoint.store(
        ClampExposureFloat(whitePoint, 1.05f, 16.0f), std::memory_order_relaxed);
}

void RequestPostFxExposureAdaptationReset()
{
    g_postFxExposureAdaptationResetRequested.store(true, std::memory_order_release);
}

void ResetPostFxExposureSettings()
{
    g_postFxExposureMode.store(
        static_cast<UINT>(PostFxExposureMode::Legacy), std::memory_order_relaxed);
    g_postFxExposureCompensationEv.store(0.0f, std::memory_order_relaxed);
    g_postFxExposureMeterMinEv.store(-10.0f, std::memory_order_relaxed);
    g_postFxExposureMeterMaxEv.store(6.0f, std::memory_order_relaxed);
    g_postFxExposureMinEv.store(-8.0f, std::memory_order_relaxed);
    g_postFxExposureMaxEv.store(4.0f, std::memory_order_relaxed);
    g_postFxExposureBrightenSpeed.store(1.5f, std::memory_order_relaxed);
    g_postFxExposureDarkenSpeed.store(3.0f, std::memory_order_relaxed);
    g_postFxExposureShoulderStrength.store(1.0f, std::memory_order_relaxed);
    g_postFxExposureWhitePoint.store(4.0f, std::memory_order_relaxed);
    RequestPostFxExposureAdaptationReset();
}

PostFxExposureStats GetPostFxExposureStats()
{
    PostFxExposureStats stats{};
    stats.shaderReady = g_postFxExposureShaderReady.load(std::memory_order_relaxed);
    stats.meterShadersReady =
        g_postFxExposureMeterShadersReady.load(std::memory_order_relaxed);
    stats.adaptationInitialized =
        g_postFxExposureAdaptationInitialized.load(std::memory_order_relaxed);
    stats.telemetryAvailable =
        g_postFxExposureTelemetryAvailable.load(std::memory_order_relaxed);
    stats.telemetryReadbackFailed =
        g_postFxExposureTelemetryReadbackFailed.load(std::memory_order_relaxed);
    stats.meterWidth = g_postFxExposureMeterWidth.load(std::memory_order_relaxed);
    stats.meterHeight = g_postFxExposureMeterHeight.load(std::memory_order_relaxed);
    stats.frameDeltaMs = g_postFxExposureFrameDeltaMs.load(std::memory_order_relaxed);
    stats.gameExposureKey = g_postFxExposureGameKey.load(std::memory_order_relaxed);
    stats.averageLogLuminance =
        g_postFxExposureTelemetryAverageLogLum.load(std::memory_order_relaxed);
    stats.targetEv = g_postFxExposureTelemetryTargetEv.load(std::memory_order_relaxed);
    stats.adaptedEv = g_postFxExposureTelemetryAdaptedEv.load(std::memory_order_relaxed);
    stats.exposureGain = std::exp2(stats.adaptedEv);
    stats.lastAppliedFrame =
        g_postFxExposureLastAppliedFrame.load(std::memory_order_relaxed);
    return stats;
}

bool ShouldUsePostFxExposureReplacement()
{
    const PostFxExposureMode exposureMode = static_cast<PostFxExposureMode>(
        g_postFxExposureMode.load(std::memory_order_relaxed));
    const PostFxAoSettings aoSettings = GetPostFxAoSettings();
    const PostFxBloomSettings bloomSettings = GetPostFxBloomSettings();
    const PostFxDofSettings dofSettings = GetPostFxDofSettings();

    // Composite AO, Bloom NG and DoF NG need the replacement even in Legacy
    // exposure mode, since DP's original final shader has no samplers for the
    // ZachFix AO/bloom textures.
    return exposureMode != PostFxExposureMode::Legacy ||
           aoSettings.mode == PostFxAoMode::Composite ||
           bloomSettings.mode != PostFxBloomMode::Legacy ||
           dofSettings.mode != PostFxDofMode::Legacy ||
           IsPostFxPreviewFreezeRequestedOrActive();
}

IDirect3DPixelShader9* GetPostFxExposureReplacementShader(IDirect3DDevice9* device)
{
    if (device == nullptr || !ShouldUsePostFxExposureReplacement())
        return nullptr;

    std::lock_guard<std::mutex> lock(g_postFxExposureMutex);
    if (g_postFxExposureShaderOwner != nullptr &&
        g_postFxExposureShaderOwner != device)
    {
        ReleasePostFxExposureShadersUnlocked();
        ReleasePostFxExposureTelemetryUnlocked();
    }
    g_postFxExposureShaderOwner = device;

    if (!EnsurePostFxExposureFinalShaderUnlocked(device))
        return nullptr;
    return g_postFxExposureShader;
}

void NotifyPostFxExposureReplacementBound(bool bound)
{
    g_postFxExposureReplacementBound.store(bound, std::memory_order_release);
}

bool BeginPostFxExposureFinalComposite(
    IDirect3DDevice9* device,
    PostFxAoFinalCompositeState* aoState,
    PostFxExposureDrawState* state)
{
    if (state == nullptr)
        return false;
    *state = {};

    if (device == nullptr ||
        !g_postFxExposureReplacementBound.load(std::memory_order_acquire))
    {
        return false;
    }

    const bool currentAoCompositeReady =
        aoState != nullptr && aoState->active &&
        aoState->mode == PostFxAoMode::Composite &&
        aoState->filteredAo != nullptr;

    // Freeze-frame capture happens at the identified final-composite draw,
    // where DP's HDR scene, packed depth and authored focus constants are all
    // simultaneously available. The frozen stages then feed the entire
    // PostFX branch while HUD/UI keep rendering live afterwards.
    UpdatePostFxDofFreezeCapture(
        device, currentAoCompositeReady ? aoState->filteredAo : nullptr);

    PostFxDofFreezeView frozenView{};
    bool frozenSceneActive = AcquirePostFxDofFreezeView(&frozenView);
    if (frozenSceneActive && !BindPostFxDofFrozenScene(device, frozenView, state))
    {
        ReleasePostFxDofFreezeView(&frozenView);
        frozenSceneActive = false;
    }

    // AO is intentionally recomputed every frame from the frozen depth/normal
    // snapshot so AO radius/strength/bias/thickness/power stay live while the
    // scene itself is frozen.
    IDirect3DTexture9* aoForPostFx =
        currentAoCompositeReady ? aoState->filteredAo : nullptr;
    const bool aoCompositeReady = aoForPostFx != nullptr;

    const PostFxExposureSettings settings = GetPostFxExposureSettings();
    const bool customExposureRequested = IsPostFxCustomExposureMode(settings.mode);
    const bool shoulder =
        settings.mode == PostFxExposureMode::ShoulderOnly ||
        settings.mode == PostFxExposureMode::ExposureAndShoulder;
    const PostFxBloomSettings bloomSettings = GetPostFxBloomSettings();
    const PostFxDofSettings dofSettings = GetPostFxDofSettings();

    IDirect3DTexture9* preparedDofNear = nullptr;
    IDirect3DTexture9* preparedDofFar = nullptr;
    bool dofNgActive = false;
    if (dofSettings.mode != PostFxDofMode::Legacy)
    {
        dofNgActive = PreparePostFxDof(
            device,
            aoForPostFx,
            &preparedDofNear, &preparedDofFar);
    }

    // Build Bloom NG before metering/final output. The meter intentionally does
    // not include bloom, avoiding exposure feedback, while the bloom prefilter
    // can still see HDR-integrated AO when Composite (HDR) is active.
    IDirect3DTexture9* preparedBloom = nullptr;
    bool bloomNgActive = false;
    if (bloomSettings.mode != PostFxBloomMode::Legacy)
    {
        bloomNgActive = PreparePostFxBloom(
            device,
            aoForPostFx,
            &preparedBloom);
    }

    // The replacement can be bound solely for HDR AO or Bloom NG while
    // exposure remains Legacy. In that case c27/c28 stay disabled and the
    // shader otherwise reproduces DP's exposure/shoulder behavior.
    IDirect3DTexture9* adaptedTexture = nullptr;
    bool customExposureActive = false;
    if (customExposureRequested)
    {
        customExposureActive = PreparePostFxCustomExposure(
            device,
            settings,
            aoForPostFx,
            &adaptedTexture);
    }

    if (FAILED(device->GetPixelShaderConstantF(26, state->previousConstants26, 1)) ||
        FAILED(device->GetPixelShaderConstantF(27, state->previousConstants27, 1)) ||
        FAILED(device->GetPixelShaderConstantF(28, state->previousConstants28, 1)) ||
        FAILED(device->GetPixelShaderConstantF(29, state->previousConstants29, 1)) ||
        FAILED(device->GetPixelShaderConstantF(30, state->previousConstants30, 1)))
    {
        if (adaptedTexture != nullptr)
            adaptedTexture->Release();
        if (preparedBloom != nullptr)
            preparedBloom->Release();
        if (preparedDofNear != nullptr)
            preparedDofNear->Release();
        if (preparedDofFar != nullptr)
            preparedDofFar->Release();
        ReleasePostFxDofFreezeView(&frozenView);
        RestorePostFxDofFrozenSceneBindings(device, state);
        ReleasePostFxDofFrozenScenePreviousRefs(state);
        return false;
    }

    if (customExposureActive && adaptedTexture != nullptr)
    {
        if (SUCCEEDED(device->GetTexture(5, &state->previousStage5)) &&
            g_originalSetTexture != nullptr &&
            SUCCEEDED(g_originalSetTexture(device, 5, adaptedTexture)))
        {
            state->changedStage5 = true;
            state->adaptedExposureTexture = adaptedTexture;
        }
        else
        {
            if (state->previousStage5 != nullptr)
            {
                state->previousStage5->Release();
                state->previousStage5 = nullptr;
            }
            adaptedTexture->Release();
            adaptedTexture = nullptr;
            customExposureActive = false;
        }
    }
    else if (adaptedTexture != nullptr)
    {
        adaptedTexture->Release();
        adaptedTexture = nullptr;
    }

    bool hdrAoActive = false;
    if (aoCompositeReady &&
        g_originalSetTexture != nullptr &&
        g_originalSetSamplerState != nullptr &&
        SUCCEEDED(device->GetTexture(6, &state->previousStage6)) &&
        SUCCEEDED(device->GetSamplerState(6, D3DSAMP_ADDRESSU, &state->previousSampler6AddressU)) &&
        SUCCEEDED(device->GetSamplerState(6, D3DSAMP_ADDRESSV, &state->previousSampler6AddressV)) &&
        SUCCEEDED(device->GetSamplerState(6, D3DSAMP_MINFILTER, &state->previousSampler6MinFilter)) &&
        SUCCEEDED(device->GetSamplerState(6, D3DSAMP_MAGFILTER, &state->previousSampler6MagFilter)) &&
        SUCCEEDED(device->GetSamplerState(6, D3DSAMP_MIPFILTER, &state->previousSampler6MipFilter)) &&
        SUCCEEDED(device->GetSamplerState(6, D3DSAMP_SRGBTEXTURE, &state->previousSampler6Srgb)))
    {
        // Sampler state gets restored even if one of the setter calls below
        // fails partway through.
        state->changedSampler6 = true;
        const bool textureOk = SUCCEEDED(
            g_originalSetTexture(device, 6, aoForPostFx));
        const bool samplerOk = textureOk &&
            SUCCEEDED(g_originalSetSamplerState(device, 6, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 6, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 6, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 6, D3DSAMP_MIPFILTER, D3DTEXF_NONE)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 6, D3DSAMP_SRGBTEXTURE, FALSE));

        if (textureOk)
            state->changedStage6 = true;
        if (samplerOk)
        {
            hdrAoActive = true;
            aoState->hdrIntegrated = true;
        }
    }

    bool bloomFinalActive = false;
    if (bloomNgActive && preparedBloom != nullptr &&
        g_originalSetTexture != nullptr &&
        g_originalSetSamplerState != nullptr &&
        SUCCEEDED(device->GetTexture(7, &state->previousStage7)) &&
        SUCCEEDED(device->GetSamplerState(7, D3DSAMP_ADDRESSU, &state->previousSampler7AddressU)) &&
        SUCCEEDED(device->GetSamplerState(7, D3DSAMP_ADDRESSV, &state->previousSampler7AddressV)) &&
        SUCCEEDED(device->GetSamplerState(7, D3DSAMP_MINFILTER, &state->previousSampler7MinFilter)) &&
        SUCCEEDED(device->GetSamplerState(7, D3DSAMP_MAGFILTER, &state->previousSampler7MagFilter)) &&
        SUCCEEDED(device->GetSamplerState(7, D3DSAMP_MIPFILTER, &state->previousSampler7MipFilter)) &&
        SUCCEEDED(device->GetSamplerState(7, D3DSAMP_SRGBTEXTURE, &state->previousSampler7Srgb)))
    {
        state->changedSampler7 = true;
        const bool textureOk = SUCCEEDED(
            g_originalSetTexture(device, 7, preparedBloom));
        const bool samplerOk = textureOk &&
            SUCCEEDED(g_originalSetSamplerState(device, 7, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 7, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 7, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 7, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 7, D3DSAMP_MIPFILTER, D3DTEXF_NONE)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 7, D3DSAMP_SRGBTEXTURE, FALSE));

        if (textureOk)
            state->changedStage7 = true;
        if (samplerOk)
        {
            bloomFinalActive = true;
            state->preparedBloomTexture = preparedBloom;
            preparedBloom = nullptr;
        }
    }

    if (preparedBloom != nullptr)
    {
        preparedBloom->Release();
        preparedBloom = nullptr;
    }

    bool dofFinalActive = false;
    if (dofNgActive && preparedDofNear != nullptr && preparedDofFar != nullptr &&
        g_originalSetTexture != nullptr && g_originalSetSamplerState != nullptr &&
        SUCCEEDED(device->GetTexture(8, &state->previousStage8)) &&
        SUCCEEDED(device->GetTexture(9, &state->previousStage9)) &&
        SUCCEEDED(device->GetSamplerState(8, D3DSAMP_ADDRESSU, &state->previousSampler8AddressU)) &&
        SUCCEEDED(device->GetSamplerState(8, D3DSAMP_ADDRESSV, &state->previousSampler8AddressV)) &&
        SUCCEEDED(device->GetSamplerState(8, D3DSAMP_MINFILTER, &state->previousSampler8MinFilter)) &&
        SUCCEEDED(device->GetSamplerState(8, D3DSAMP_MAGFILTER, &state->previousSampler8MagFilter)) &&
        SUCCEEDED(device->GetSamplerState(8, D3DSAMP_MIPFILTER, &state->previousSampler8MipFilter)) &&
        SUCCEEDED(device->GetSamplerState(8, D3DSAMP_SRGBTEXTURE, &state->previousSampler8Srgb)) &&
        SUCCEEDED(device->GetSamplerState(9, D3DSAMP_ADDRESSU, &state->previousSampler9AddressU)) &&
        SUCCEEDED(device->GetSamplerState(9, D3DSAMP_ADDRESSV, &state->previousSampler9AddressV)) &&
        SUCCEEDED(device->GetSamplerState(9, D3DSAMP_MINFILTER, &state->previousSampler9MinFilter)) &&
        SUCCEEDED(device->GetSamplerState(9, D3DSAMP_MAGFILTER, &state->previousSampler9MagFilter)) &&
        SUCCEEDED(device->GetSamplerState(9, D3DSAMP_MIPFILTER, &state->previousSampler9MipFilter)) &&
        SUCCEEDED(device->GetSamplerState(9, D3DSAMP_SRGBTEXTURE, &state->previousSampler9Srgb)))
    {
        state->changedSampler8 = true;
        state->changedSampler9 = true;
        const bool nearTextureOk = SUCCEEDED(g_originalSetTexture(device, 8, preparedDofNear));
        const bool farTextureOk = nearTextureOk && SUCCEEDED(g_originalSetTexture(device, 9, preparedDofFar));
        if (nearTextureOk) state->changedStage8 = true;
        if (farTextureOk) state->changedStage9 = true;

        const bool samplerOk = farTextureOk &&
            SUCCEEDED(g_originalSetSamplerState(device, 8, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 8, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 8, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 8, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 8, D3DSAMP_MIPFILTER, D3DTEXF_NONE)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 8, D3DSAMP_SRGBTEXTURE, FALSE)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 9, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 9, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 9, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 9, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 9, D3DSAMP_MIPFILTER, D3DTEXF_NONE)) &&
            SUCCEEDED(g_originalSetSamplerState(device, 9, D3DSAMP_SRGBTEXTURE, FALSE));

        if (samplerOk)
        {
            dofFinalActive = true;
            state->preparedDofNearTexture = preparedDofNear;
            state->preparedDofFarTexture = preparedDofFar;
            preparedDofNear = nullptr;
            preparedDofFar = nullptr;
        }
    }

    if (preparedDofNear != nullptr) preparedDofNear->Release();
    if (preparedDofFar != nullptr) preparedDofFar->Release();

    ReleasePostFxDofFreezeView(&frozenView);

    const float c26[4] =
    {
        dofFinalActive ? static_cast<float>(dofSettings.mode) : 0.0f,
        dofSettings.nearStrength,
        dofSettings.farStrength,
        (dofFinalActive && aoCompositeReady) ? 1.0f : 0.0f
    };
    const float c27[4] =
    {
        customExposureActive ? 1.0f : 0.0f,
        0.0f,
        0.0f,
        0.0f
    };
    const float c28[4] =
    {
        shoulder ? 1.0f : 0.0f,
        settings.shoulderStrength,
        settings.whitePoint,
        0.0f
    };
    const float c29[4] =
    {
        hdrAoActive ? 1.0f : 0.0f,
        0.0f,
        0.0f,
        0.0f
    };
    const float c30[4] =
    {
        bloomFinalActive ? static_cast<float>(bloomSettings.mode) : 0.0f,
        bloomSettings.intensity,
        0.0f,
        0.0f
    };

    if (FAILED(device->SetPixelShaderConstantF(26, c26, 1)))
    {
        EndPostFxExposureFinalComposite(device, state);
        if (aoState != nullptr)
            aoState->hdrIntegrated = false;
        return false;
    }
    state->changedConstants26 = true;

    if (FAILED(device->SetPixelShaderConstantF(27, c27, 1)))
    {
        EndPostFxExposureFinalComposite(device, state);
        if (aoState != nullptr)
            aoState->hdrIntegrated = false;
        return false;
    }
    state->changedConstants27 = true;

    if (FAILED(device->SetPixelShaderConstantF(28, c28, 1)))
    {
        EndPostFxExposureFinalComposite(device, state);
        if (aoState != nullptr)
            aoState->hdrIntegrated = false;
        return false;
    }
    state->changedConstants28 = true;

    if (FAILED(device->SetPixelShaderConstantF(29, c29, 1)))
    {
        EndPostFxExposureFinalComposite(device, state);
        if (aoState != nullptr)
            aoState->hdrIntegrated = false;
        return false;
    }
    state->changedConstants29 = true;

    if (FAILED(device->SetPixelShaderConstantF(30, c30, 1)))
    {
        EndPostFxExposureFinalComposite(device, state);
        if (aoState != nullptr)
            aoState->hdrIntegrated = false;
        return false;
    }
    state->changedConstants30 = true;
    state->active = true;
    g_postFxExposureLastAppliedFrame.store(
        GetPostFxFrameIndex(), std::memory_order_relaxed);
    return true;
}

void EndPostFxExposureFinalComposite(
    IDirect3DDevice9* device,
    PostFxExposureDrawState* state)
{
    if (state == nullptr)
        return;

    if (device != nullptr)
    {
        if (state->changedConstants30)
            device->SetPixelShaderConstantF(30, state->previousConstants30, 1);
        if (state->changedConstants29)
            device->SetPixelShaderConstantF(29, state->previousConstants29, 1);
        if (state->changedConstants28)
            device->SetPixelShaderConstantF(28, state->previousConstants28, 1);
        if (state->changedConstants27)
            device->SetPixelShaderConstantF(27, state->previousConstants27, 1);
        if (state->changedConstants26)
            device->SetPixelShaderConstantF(26, state->previousConstants26, 1);

        if (state->changedSampler9 && g_originalSetSamplerState != nullptr)
        {
            g_originalSetSamplerState(device, 9, D3DSAMP_ADDRESSU, state->previousSampler9AddressU);
            g_originalSetSamplerState(device, 9, D3DSAMP_ADDRESSV, state->previousSampler9AddressV);
            g_originalSetSamplerState(device, 9, D3DSAMP_MINFILTER, state->previousSampler9MinFilter);
            g_originalSetSamplerState(device, 9, D3DSAMP_MAGFILTER, state->previousSampler9MagFilter);
            g_originalSetSamplerState(device, 9, D3DSAMP_MIPFILTER, state->previousSampler9MipFilter);
            g_originalSetSamplerState(device, 9, D3DSAMP_SRGBTEXTURE, state->previousSampler9Srgb);
        }
        if (state->changedStage9 && g_originalSetTexture != nullptr)
            g_originalSetTexture(device, 9, state->previousStage9);

        if (state->changedSampler8 && g_originalSetSamplerState != nullptr)
        {
            g_originalSetSamplerState(device, 8, D3DSAMP_ADDRESSU, state->previousSampler8AddressU);
            g_originalSetSamplerState(device, 8, D3DSAMP_ADDRESSV, state->previousSampler8AddressV);
            g_originalSetSamplerState(device, 8, D3DSAMP_MINFILTER, state->previousSampler8MinFilter);
            g_originalSetSamplerState(device, 8, D3DSAMP_MAGFILTER, state->previousSampler8MagFilter);
            g_originalSetSamplerState(device, 8, D3DSAMP_MIPFILTER, state->previousSampler8MipFilter);
            g_originalSetSamplerState(device, 8, D3DSAMP_SRGBTEXTURE, state->previousSampler8Srgb);
        }
        if (state->changedStage8 && g_originalSetTexture != nullptr)
            g_originalSetTexture(device, 8, state->previousStage8);

        if (state->changedSampler7 && g_originalSetSamplerState != nullptr)
        {
            g_originalSetSamplerState(device, 7, D3DSAMP_ADDRESSU, state->previousSampler7AddressU);
            g_originalSetSamplerState(device, 7, D3DSAMP_ADDRESSV, state->previousSampler7AddressV);
            g_originalSetSamplerState(device, 7, D3DSAMP_MINFILTER, state->previousSampler7MinFilter);
            g_originalSetSamplerState(device, 7, D3DSAMP_MAGFILTER, state->previousSampler7MagFilter);
            g_originalSetSamplerState(device, 7, D3DSAMP_MIPFILTER, state->previousSampler7MipFilter);
            g_originalSetSamplerState(device, 7, D3DSAMP_SRGBTEXTURE, state->previousSampler7Srgb);
        }
        if (state->changedStage7 && g_originalSetTexture != nullptr)
            g_originalSetTexture(device, 7, state->previousStage7);

        if (state->changedSampler6 && g_originalSetSamplerState != nullptr)
        {
            g_originalSetSamplerState(device, 6, D3DSAMP_ADDRESSU, state->previousSampler6AddressU);
            g_originalSetSamplerState(device, 6, D3DSAMP_ADDRESSV, state->previousSampler6AddressV);
            g_originalSetSamplerState(device, 6, D3DSAMP_MINFILTER, state->previousSampler6MinFilter);
            g_originalSetSamplerState(device, 6, D3DSAMP_MAGFILTER, state->previousSampler6MagFilter);
            g_originalSetSamplerState(device, 6, D3DSAMP_MIPFILTER, state->previousSampler6MipFilter);
            g_originalSetSamplerState(device, 6, D3DSAMP_SRGBTEXTURE, state->previousSampler6Srgb);
        }
        if (state->changedStage6 && g_originalSetTexture != nullptr)
            g_originalSetTexture(device, 6, state->previousStage6);
        if (state->changedStage5 && g_originalSetTexture != nullptr)
            g_originalSetTexture(device, 5, state->previousStage5);

        RestorePostFxDofFrozenSceneBindings(device, state);
    }

    ReleasePostFxDofFrozenScenePreviousRefs(state);
    if (state->previousStage5 != nullptr)
        state->previousStage5->Release();
    if (state->previousStage6 != nullptr)
        state->previousStage6->Release();
    if (state->previousStage7 != nullptr)
        state->previousStage7->Release();
    if (state->previousStage8 != nullptr)
        state->previousStage8->Release();
    if (state->previousStage9 != nullptr)
        state->previousStage9->Release();
    if (state->adaptedExposureTexture != nullptr)
        state->adaptedExposureTexture->Release();
    if (state->preparedBloomTexture != nullptr)
        state->preparedBloomTexture->Release();
    if (state->preparedDofNearTexture != nullptr)
        state->preparedDofNearTexture->Release();
    if (state->preparedDofFarTexture != nullptr)
        state->preparedDofFarTexture->Release();
    *state = {};
}

void ReleasePostFxExposureResources()
{
    std::lock_guard<std::mutex> lock(g_postFxExposureMutex);
    ReleasePostFxExposureShadersUnlocked();
    ReleasePostFxExposureTelemetryUnlocked();
    g_postFxExposureReplacementBound.store(false, std::memory_order_release);
    g_postFxExposureAdaptationInitialized.store(false, std::memory_order_relaxed);
    g_postFxExposureAdaptReadIndex.store(0, std::memory_order_relaxed);
    g_postFxExposureAdaptationResetRequested.store(true, std::memory_order_relaxed);
    g_postFxExposureSourceWidth = 0;
    g_postFxExposureSourceHeight = 0;
    g_postFxExposureLastQpc = {};
    g_postFxExposureMeterWidth.store(0, std::memory_order_relaxed);
    g_postFxExposureMeterHeight.store(0, std::memory_order_relaxed);
    g_postFxExposureLastAppliedFrame.store(0, std::memory_order_relaxed);
}
