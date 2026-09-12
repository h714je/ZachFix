#include "ssao.h"

#include "logging.h"
#include "main_exe.h"
#include "runtime_resources.h"

#include <Windows.h>
#include <d3dcompiler.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <vector>

namespace
{
thread_local bool g_internalPass = false;

IDirect3DSurface9* g_packedDepthLogical = nullptr;
IDirect3DSurface9* g_normalSurfaceLogical = nullptr;
IDirect3DSurface9* g_mainSurfaceLogical = nullptr;
bool g_onBackbuffer = false;
bool g_appliedThisFrame = false;

IDirect3DPixelShader9* g_aoShader = nullptr;
IDirect3DPixelShader9* g_blurShader = nullptr;
IDirect3DPixelShader9* g_combineShader = nullptr;

IDirect3DTexture9* g_aoTextureA = nullptr;
IDirect3DSurface9* g_aoSurfaceA = nullptr;
IDirect3DTexture9* g_aoTextureB = nullptr;
IDirect3DSurface9* g_aoSurfaceB = nullptr;
IDirect3DTexture9* g_combinedTexture = nullptr;
IDirect3DSurface9* g_combinedSurface = nullptr;

UINT g_frameWidth = 0;
UINT g_frameHeight = 0;
UINT g_aoWidth = 0;
UINT g_aoHeight = 0;
UINT g_resourceScale = 0;
D3DFORMAT g_frameFormat = D3DFMT_UNKNOWN;

std::atomic<unsigned long long> g_framesApplied{0};
std::atomic<unsigned long long> g_failures{0};
std::atomic<bool> g_depthPairSeen{false};
std::atomic<bool> g_probeRequested{true};

std::mutex g_probeMutex;
SsaoRuntimeStats g_probeSnapshot{};

bool g_loggedCompilerFailure = false;
bool g_compilerUnavailable = false;
bool g_shaderPermanentlyFailed = false;
bool g_loggedShaderReady = false;
bool g_loggedMainSurface = false;
bool g_loggedEarlyApply = false;
bool g_loggedDepthPair = false;
bool g_loggedPassFailure = false;
bool g_shaderFailureCounted = false;
const char* g_lastRenderFailureStage = "none";

bool g_targetAllocationFailed = false;
UINT g_failedFrameWidth = 0;
UINT g_failedFrameHeight = 0;
UINT g_failedAoScale = 0;
D3DFORMAT g_failedFrameFormat = D3DFMT_UNKNOWN;

using D3DCompileFn = HRESULT (WINAPI*)(
    LPCVOID,
    SIZE_T,
    LPCSTR,
    const D3D_SHADER_MACRO*,
    ID3DInclude*,
    LPCSTR,
    LPCSTR,
    UINT,
    UINT,
    ID3DBlob**,
    ID3DBlob**);

D3DCompileFn g_d3dCompile = nullptr;
HMODULE g_d3dCompilerModule = nullptr;

struct FullscreenVertex
{
    float x;
    float y;
    float z;
    float rhw;
    float u;
    float v;
};

constexpr DWORD kFullscreenFvf = D3DFVF_XYZRHW | D3DFVF_TEX1;

UINT BytesPerPixel(D3DFORMAT format)
{
    switch (format)
    {
        case D3DFMT_A16B16G16R16F:
            return 8;
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_R32F:
            return 4;
        case D3DFMT_R5G6B5:
        case D3DFMT_A1R5G5B5:
            return 2;
        default:
            return 0;
    }
}

const char* kSsaoShaderSource = R"HLSL(
sampler inputSampler : register(s0);
sampler depthSampler : register(s1);
sampler normalSampler : register(s2);

float4 passParams0 : register(c0);
float4 passParams1 : register(c1);

float DecodeLegacyDsfixDepth(float4 col)
{
    // Dark Souls / DSFix uses an inverted RGB packing. Keep this only as a
    // diagnostic reference; Deadly Premonition's G-buffer is not inverted.
    float posZ = (1.0 - col.b) +
                 (1.0 - col.g) * 256.0 +
                 (1.0 - col.r) * (257.0 * 256.0);
    return (posZ - 1.0) / 5000.0;
}

float DecodeEyeZ(float4 col)
{
    // DP writes view-space Z as a 24-bit-ish fixed-point value split over RGB:
    //   B = fractional part, G = middle byte, R = coarse/high byte.
    // This is visible directly in DP's G-buffer pixel shaders (1/256 packing).
    return col.b + col.g * 256.0 + col.r * 65536.0;
}

float DecodeDepth(float4 col)
{
    const float nearZ = 1.0;
    const float farZ = 5000.0;
    return (DecodeEyeZ(col) - nearZ) / (farZ - nearZ);
}

float DecodePackedRgbUnit(float4 col)
{
    // Standard normalized-RGB candidate retained for comparison only.
    // Production AO uses DP's fixed-point view-Z above.
    return saturate(col.r + col.g / 255.0 + col.b / 65025.0);
}

float LinearizePackedRgbCandidate(float z)
{
    const float nearZ = 1.0;
    const float farZ = 5000.0;
    z = saturate(z);
    float eye = (nearZ * farZ) / max(farZ - z * (farZ - nearZ), 1e-5);
    return saturate(log2(1.0 + eye) / log2(1.0 + farZ));
}

float3 ReconstructPosition(float2 uv, float eyeZ)
{
    const float tanHalfFovY = 0.9163311740; // tan(85 deg / 2)
    // passParams0.xy = 1 / full-resolution width,height.
    float aspect = passParams0.y / max(passParams0.x, 1e-8); // width / height
    float2 tanHalfFov = float2(tanHalfFovY * aspect, tanHalfFovY);
    float2 ndc = uv * float2(2.0, -2.0) - float2(1.0, -1.0);
    return float3(ndc * tanHalfFov * eyeZ, eyeZ);
}

float RandomAngle(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453) * 6.28318530718;
}

static const float2 kAoDisk[12] =
{
    float2( 0.2500,  0.0000),
    float2(-0.1250,  0.2165),
    float2(-0.1250, -0.2165),
    float2( 0.5000,  0.0000),
    float2(-0.2500,  0.4330),
    float2(-0.2500, -0.4330),
    float2( 0.7500,  0.0000),
    float2(-0.3750,  0.6495),
    float2(-0.3750, -0.6495),
    float2( 1.0000,  0.0000),
    float2(-0.5000,  0.8660),
    float2(-0.5000, -0.8660)
};

float4 AoPS(float2 uv : TEXCOORD0) : COLOR0
{
    const float nearZ = 1.0;
    const float farZ = 5000.0;
    const float tanHalfFovY = 0.9163311740;

    float eyeZ = DecodeEyeZ(tex2D(depthSampler, uv));
    if (eyeZ < nearZ || eyeZ > farZ)
        return float4(1.0, 1.0, 1.0, 1.0);

    float radiusMul = max(passParams0.z, 0.05);
    float strength = max(passParams0.w, 0.0);
    float2 pixelSize = passParams0.xy;
    float aspect = pixelSize.y / max(pixelSize.x, 1e-8); // width / height

    // Radius is now calibrated in DP view-space units rather than inherited
    // DSFix normalized-depth units. Radius=1 corresponds to ~2 view units.
    float worldRadius = 2.0 * radiusMul;

    float3 centerPos = ReconstructPosition(uv, eyeZ);

    // DP already gives us a full-resolution normal buffer in RT1. Its G-buffer
    // shaders encode normals as N * 0.5 + 0.5, so decode them directly instead
    // of differentiating quantized packed depth. The ddx/ddy path is retained
    // only as a fallback for degenerate normal texels.
    float3 normal = tex2D(normalSampler, uv).xyz * 2.0 - 1.0;
    float normalLenSq = dot(normal, normal);
    if (normalLenSq < 0.05)
    {
        float3 dx = ddx(centerPos);
        float3 dy = ddy(centerPos);
        normal = cross(dx, dy);
        normalLenSq = dot(normal, normal);
    }
    normal *= rsqrt(max(normalLenSq, 1e-6));

    // View-space positions point away from the camera (+Z). Make the G-buffer
    // normal face the camera consistently so the hemisphere test has one sign.
    if (dot(normal, centerPos) > 0.0)
        normal = -normal;

    float uvRadiusY = worldRadius / max(2.0 * eyeZ * tanHalfFovY, 1e-5);
    float uvRadiusX = worldRadius / max(2.0 * eyeZ * tanHalfFovY * aspect, 1e-5);
    float2 uvRadius = float2(uvRadiusX, uvRadiusY);

    // Avoid giant near-camera kernels and sub-pixel far-field noise.
    float2 maxUvRadius = float2(pixelSize.x * 48.0, pixelSize.y * 48.0);
    uvRadius = min(uvRadius, maxUvRadius);
    if (uvRadius.x < pixelSize.x * 0.35 && uvRadius.y < pixelSize.y * 0.35)
        return float4(1.0, 1.0, 1.0, 1.0);

    float angle = RandomAngle(uv);
    float ca = cos(angle);
    float sa = sin(angle);

    float occ = 0.0;
    float weightSum = 0.0;

    [unroll]
    for (int i = 0; i < 12; ++i)
    {
        float2 p = kAoDisk[i];
        float2 rotated = float2(p.x * ca - p.y * sa,
                                p.x * sa + p.y * ca);
        float2 sampleUv = uv + rotated * uvRadius;

        // Keep taps inside the frame. Outside samples are simply ignored.
        if (sampleUv.x <= 0.0 || sampleUv.x >= 1.0 ||
            sampleUv.y <= 0.0 || sampleUv.y >= 1.0)
            continue;

        float sampleEyeZ = DecodeEyeZ(tex2D(depthSampler, sampleUv));
        if (sampleEyeZ < nearZ || sampleEyeZ > farZ)
            continue;

        float3 samplePos = ReconstructPosition(sampleUv, sampleEyeZ);
        float3 delta = samplePos - centerPos;
        float distSq = dot(delta, delta);
        if (distSq <= 1e-8)
            continue;

        float dist = sqrt(distSq);
        float3 dir = delta / dist;

        // Hemisphere visibility term plus a smooth world-space range falloff.
        // Small bias suppresses self-occlusion on nearly coplanar surfaces.
        float facing = max(dot(normal, dir) - 0.06, 0.0);
        float rangeWeight = saturate(1.0 - dist / worldRadius);
        rangeWeight *= rangeWeight;

        occ += facing * rangeWeight;
        weightSum += 1.0;
    }

    if (weightSum < 1.0)
        return float4(1.0, 1.0, 1.0, 1.0);

    occ /= weightSum;

    // Radius=1/Strength=1 is intended to be visible but restrained.
    float ao = saturate(1.0 - occ * strength * 3.0);
    return float4(ao, ao, ao, 1.0);
}

float3 DecodeNormal(float2 uv)
{
    float3 n = tex2D(normalSampler, uv).xyz * 2.0 - 1.0;
    float lenSq = dot(n, n);
    if (lenSq < 1e-5)
        return float3(0.0, 0.0, 1.0);
    return n * rsqrt(lenSq);
}

float4 BlurPS(float2 uv : TEXCOORD0) : COLOR0
{
    float2 stepUv = passParams0.xy * passParams0.zw;
    float centerAo = tex2D(inputSampler, uv).r;
    float centerZ = DecodeEyeZ(tex2D(depthSampler, uv));

    if (centerZ < 1.0 || centerZ > 5000.0)
        return float4(centerAo, centerAo, centerAo, 1.0);

    float3 centerN = DecodeNormal(uv);
    float result = centerAo * 0.40;
    float total = 0.40;

    float2 uv1p = uv + stepUv * 1.5;
    float2 uv1m = uv - stepUv * 1.5;
    float2 uv2p = uv + stepUv * 3.5;
    float2 uv2m = uv - stepUv * 3.5;

    float z1p = DecodeEyeZ(tex2D(depthSampler, uv1p));
    float z1m = DecodeEyeZ(tex2D(depthSampler, uv1m));
    float z2p = DecodeEyeZ(tex2D(depthSampler, uv2p));
    float z2m = DecodeEyeZ(tex2D(depthSampler, uv2m));

    float tolerance = max(0.75, centerZ * 0.015);

    if (z1p >= 1.0 && z1p <= 5000.0)
    {
        float nw = pow(saturate(dot(centerN, DecodeNormal(uv1p))), 8.0);
        float w = 0.24 * exp(-abs(z1p - centerZ) / tolerance) * nw;
        result += tex2D(inputSampler, uv1p).r * w; total += w;
    }
    if (z1m >= 1.0 && z1m <= 5000.0)
    {
        float nw = pow(saturate(dot(centerN, DecodeNormal(uv1m))), 8.0);
        float w = 0.24 * exp(-abs(z1m - centerZ) / tolerance) * nw;
        result += tex2D(inputSampler, uv1m).r * w; total += w;
    }
    if (z2p >= 1.0 && z2p <= 5000.0)
    {
        float nw = pow(saturate(dot(centerN, DecodeNormal(uv2p))), 8.0);
        float w = 0.06 * exp(-abs(z2p - centerZ) / tolerance) * nw;
        result += tex2D(inputSampler, uv2p).r * w; total += w;
    }
    if (z2m >= 1.0 && z2m <= 5000.0)
    {
        float nw = pow(saturate(dot(centerN, DecodeNormal(uv2m))), 8.0);
        float w = 0.06 * exp(-abs(z2m - centerZ) / tolerance) * nw;
        result += tex2D(inputSampler, uv2m).r * w; total += w;
    }

    float ao = result / max(total, 1e-5);
    return float4(ao, ao, ao, 1.0);
}

float4 CombinePS(float2 uv : TEXCOORD0) : COLOR0
{
    float mode = passParams1.x;
    float4 aux = tex2D(depthSampler, uv);

    // Diagnostic modes deliberately bypass as much SSAO logic as possible.
    if (mode > 12.5)
    {
        float d = max(DecodeLegacyDsfixDepth(aux), 0.0);
        float v = saturate(log2(1.0 + d) / log2(17.0));
        return float4(v, v, v, 1.0);
    }
    if (mode > 11.5)
    {
        float v = LinearizePackedRgbCandidate(DecodePackedRgbUnit(aux));
        return float4(v, v, v, 1.0);
    }
    if (mode > 10.5)
    {
        float v = 1.0 - DecodePackedRgbUnit(aux);
        return float4(v, v, v, 1.0);
    }
    if (mode > 9.5)
    {
        float v = DecodePackedRgbUnit(aux);
        return float4(v, v, v, 1.0);
    }
    if (mode > 8.5)
        return float4(aux.a, aux.a, aux.a, 1.0);
    if (mode > 7.5)
        return float4(aux.b, aux.b, aux.b, 1.0);
    if (mode > 6.5)
        return float4(aux.g, aux.g, aux.g, 1.0);
    if (mode > 5.5)
        return float4(aux.r, aux.r, aux.r, 1.0);
    if (mode > 4.5)
    {
        float ao = tex2D(depthSampler, uv).r;
        float v = saturate((1.0 - ao) * 32.0);
        return float4(v, v, v, 1.0);
    }
    if (mode > 2.5 && mode < 3.5)
        return float4(1.0, 0.0, 1.0, 1.0);
    if (mode > 3.5)
        return float4(aux.rgb, 1.0);
    if (mode > 1.5)
    {
        float d = saturate(DecodeDepth(aux));
        // Log-ish display curve so the near field remains visible while the
        // actual AO continues to use the unmodified DP depth value.
        float v = log2(1.0 + d * 255.0) / 8.0;
        return float4(v, v, v, 1.0);
    }
    if (mode > 0.5)
        return float4(aux.r, aux.r, aux.r, 1.0);

    float4 scene = tex2D(inputSampler, uv);
    scene.rgb *= aux.r;
    return scene;
}
)HLSL";

class InternalPassScope
{
public:
    InternalPassScope() { g_internalPass = true; }
    ~InternalPassScope() { g_internalPass = false; }
    InternalPassScope(const InternalPassScope&) = delete;
    InternalPassScope& operator=(const InternalPassScope&) = delete;
};

template <typename T>
void SafeReleaseTyped(T*& value)
{
    if (value != nullptr)
    {
        value->Release();
        value = nullptr;
    }
}

void ReleaseSsaoTargets()
{
    SafeReleaseTyped(g_aoSurfaceA);
    SafeReleaseTyped(g_aoTextureA);
    SafeReleaseTyped(g_aoSurfaceB);
    SafeReleaseTyped(g_aoTextureB);
    SafeReleaseTyped(g_combinedSurface);
    SafeReleaseTyped(g_combinedTexture);

    g_frameWidth = 0;
    g_frameHeight = 0;
    g_aoWidth = 0;
    g_aoHeight = 0;
    g_resourceScale = 0;
    g_frameFormat = D3DFMT_UNKNOWN;
}

bool LoadCompiler()
{
    if (g_d3dCompile != nullptr)
        return true;
    if (g_compilerUnavailable)
        return false;

    const wchar_t* names[] =
    {
        L"d3dcompiler_47.dll",
        L"d3dcompiler_46.dll",
        L"d3dcompiler_43.dll"
    };

    for (const wchar_t* name : names)
    {
        HMODULE module = LoadLibraryW(name);
        if (module == nullptr)
            continue;

        auto compile = reinterpret_cast<D3DCompileFn>(
            GetProcAddress(module, "D3DCompile"));
        if (compile != nullptr)
        {
            g_d3dCompilerModule = module;
            g_d3dCompile = compile;
            return true;
        }

        FreeLibrary(module);
    }

    if (!g_loggedCompilerFailure)
    {
        AppendLog("[SSAO] ERROR: no D3DCompiler DLL with D3DCompile was found. SSAO disabled for this session.\n");
        g_loggedCompilerFailure = true;
    }
    g_compilerUnavailable = true;
    return false;
}

bool CompilePixelShader(
    IDirect3DDevice9* device,
    const char* entry,
    IDirect3DPixelShader9** shader)
{
    if (device == nullptr || entry == nullptr || shader == nullptr)
        return false;
    if (g_shaderPermanentlyFailed)
        return false;

    if (*shader != nullptr)
        return true;

    if (!LoadCompiler())
        return false;

    ID3DBlob* code = nullptr;
    ID3DBlob* errors = nullptr;
    const HRESULT compileHr = g_d3dCompile(
        kSsaoShaderSource,
        std::strlen(kSsaoShaderSource),
        "DPFixNG_SSAO",
        nullptr,
        nullptr,
        entry,
        "ps_3_0",
        D3DCOMPILE_OPTIMIZATION_LEVEL3,
        0,
        &code,
        &errors);

    if (FAILED(compileHr) || code == nullptr)
    {
        char text[1024] = {};
        if (errors != nullptr && errors->GetBufferPointer() != nullptr)
        {
            sprintf_s(
                text,
                "[SSAO] ERROR: HLSL compile failed for %s: %.850s\n",
                entry,
                static_cast<const char*>(errors->GetBufferPointer()));
        }
        else
        {
            sprintf_s(
                text,
                "[SSAO] ERROR: HLSL compile failed for %s (HRESULT=0x%08X).\n",
                entry,
                static_cast<unsigned>(compileHr));
        }
        AppendLog(text);
        if (!g_shaderFailureCounted)
        {
            g_failures.fetch_add(1, std::memory_order_relaxed);
            g_shaderFailureCounted = true;
        }
        SafeReleaseTyped(errors);
        SafeReleaseTyped(code);
        g_shaderPermanentlyFailed = true;
        return false;
    }

    const HRESULT createHr = device->CreatePixelShader(
        static_cast<const DWORD*>(code->GetBufferPointer()),
        shader);

    SafeReleaseTyped(errors);
    SafeReleaseTyped(code);

    if (FAILED(createHr) || *shader == nullptr)
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[SSAO] ERROR: CreatePixelShader failed for %s (HRESULT=0x%08X).\n",
            entry,
            static_cast<unsigned>(createHr));
        AppendLog(text);
        if (!g_shaderFailureCounted)
        {
            g_failures.fetch_add(1, std::memory_order_relaxed);
            g_shaderFailureCounted = true;
        }
        g_shaderPermanentlyFailed = true;
        return false;
    }

    return true;
}

bool EnsureShaders(IDirect3DDevice9* device)
{
    if (!CompilePixelShader(device, "AoPS", &g_aoShader) ||
        !CompilePixelShader(device, "BlurPS", &g_blurShader) ||
        !CompilePixelShader(device, "CombinePS", &g_combineShader))
    {
        return false;
    }

    if (!g_loggedShaderReady)
    {
        AppendLog("[SSAO] Pixel shaders compiled (ps_3_0).\n");
        g_loggedShaderReady = true;
    }
    return true;
}

bool CreateRenderTexture(
    IDirect3DDevice9* device,
    UINT width,
    UINT height,
    D3DFORMAT format,
    IDirect3DTexture9** texture,
    IDirect3DSurface9** surface)
{
    if (device == nullptr || texture == nullptr || surface == nullptr)
        return false;

    *texture = nullptr;
    *surface = nullptr;

    const HRESULT textureHr = device->CreateTexture(
        width,
        height,
        1,
        D3DUSAGE_RENDERTARGET,
        format,
        D3DPOOL_DEFAULT,
        texture,
        nullptr);
    if (FAILED(textureHr) || *texture == nullptr)
        return false;

    const HRESULT surfaceHr = (*texture)->GetSurfaceLevel(0, surface);
    if (FAILED(surfaceHr) || *surface == nullptr)
    {
        SafeReleaseTyped(*texture);
        return false;
    }

    return true;
}

bool EnsureTargets(
    IDirect3DDevice9* device,
    UINT frameWidth,
    UINT frameHeight,
    D3DFORMAT frameFormat)
{
    const UINT scale =
        (g_config.ssaoResolutionScale == 1 ||
         g_config.ssaoResolutionScale == 2 ||
         g_config.ssaoResolutionScale == 4)
            ? g_config.ssaoResolutionScale
            : 2;

    const UINT aoWidth = std::max<UINT>(1, (frameWidth + scale - 1) / scale);
    const UINT aoHeight = std::max<UINT>(1, (frameHeight + scale - 1) / scale);

    if (g_targetAllocationFailed &&
        g_failedFrameWidth == frameWidth &&
        g_failedFrameHeight == frameHeight &&
        g_failedAoScale == scale &&
        g_failedFrameFormat == frameFormat)
    {
        return false;
    }

    if (g_aoTextureA != nullptr &&
        g_aoTextureB != nullptr &&
        g_combinedTexture != nullptr &&
        g_frameWidth == frameWidth &&
        g_frameHeight == frameHeight &&
        g_aoWidth == aoWidth &&
        g_aoHeight == aoHeight &&
        g_resourceScale == scale &&
        g_frameFormat == frameFormat)
    {
        return true;
    }

    ReleaseSsaoTargets();

    IDirect3DTexture9* newAoA = nullptr;
    IDirect3DSurface9* newAoASurface = nullptr;
    IDirect3DTexture9* newAoB = nullptr;
    IDirect3DSurface9* newAoBSurface = nullptr;
    IDirect3DTexture9* newCombined = nullptr;
    IDirect3DSurface9* newCombinedSurface = nullptr;

    if (!CreateRenderTexture(
            device, aoWidth, aoHeight, D3DFMT_A8R8G8B8,
            &newAoA, &newAoASurface) ||
        !CreateRenderTexture(
            device, aoWidth, aoHeight, D3DFMT_A8R8G8B8,
            &newAoB, &newAoBSurface) ||
        !CreateRenderTexture(
            device, frameWidth, frameHeight, frameFormat,
            &newCombined, &newCombinedSurface))
    {
        SafeReleaseTyped(newAoASurface);
        SafeReleaseTyped(newAoA);
        SafeReleaseTyped(newAoBSurface);
        SafeReleaseTyped(newAoB);
        SafeReleaseTyped(newCombinedSurface);
        SafeReleaseTyped(newCombined);
        AppendLog("[SSAO] ERROR: could not allocate SSAO render targets. Further attempts with the same dimensions are suppressed.\n");
        g_targetAllocationFailed = true;
        g_failedFrameWidth = frameWidth;
        g_failedFrameHeight = frameHeight;
        g_failedAoScale = scale;
        g_failedFrameFormat = frameFormat;
        return false;
    }

    g_targetAllocationFailed = false;
    g_aoTextureA = newAoA;
    g_aoSurfaceA = newAoASurface;
    g_aoTextureB = newAoB;
    g_aoSurfaceB = newAoBSurface;
    g_combinedTexture = newCombined;
    g_combinedSurface = newCombinedSurface;
    g_frameWidth = frameWidth;
    g_frameHeight = frameHeight;
    g_aoWidth = aoWidth;
    g_aoHeight = aoHeight;
    g_resourceScale = scale;
    g_frameFormat = frameFormat;

    char text[256] = {};
    sprintf_s(
        text,
        "[SSAO] Resources ready: frame=%u x %u fmt=%u, AO=%u x %u (1/%u).\n",
        frameWidth,
        frameHeight,
        static_cast<unsigned>(frameFormat),
        aoWidth,
        aoHeight,
        scale);
    AppendLog(text);
    return true;
}

bool GetEffectiveSurfaceDesc(
    IDirect3DSurface9* logical,
    D3DSURFACE_DESC& desc)
{
    if (logical == nullptr)
        return false;

    IDirect3DSurface9* replacement = AcquireRuntimeReplacementSurface(logical);
    IDirect3DSurface9* effective = replacement != nullptr ? replacement : logical;
    const HRESULT hr = effective->GetDesc(&desc);
    SafeReleaseTyped(replacement);
    return SUCCEEDED(hr);
}

IDirect3DSurface9* AcquireEffectiveSurface(IDirect3DSurface9* logical)
{
    if (logical == nullptr)
        return nullptr;

    IDirect3DSurface9* replacement = AcquireRuntimeReplacementSurface(logical);
    if (replacement != nullptr)
        return replacement;

    logical->AddRef();
    return logical;
}

IDirect3DTexture9* AcquireSurfaceTexture(IDirect3DSurface9* logical)
{
    IDirect3DSurface9* effective = AcquireEffectiveSurface(logical);
    if (effective == nullptr)
        return nullptr;

    IDirect3DTexture9* texture = nullptr;
    const HRESULT hr = effective->GetContainer(
        __uuidof(IDirect3DTexture9),
        reinterpret_cast<void**>(&texture));

    effective->Release();
    if (FAILED(hr))
        return nullptr;

    return texture;
}

bool IsMainExeCaller(uintptr_t returnAddress)
{
    return g_mainExeInfoValid &&
        returnAddress >= g_mainExeBase &&
        returnAddress < g_mainExeBase + g_mainExeSize;
}

void StoreLogicalSurface(IDirect3DSurface9*& slot, IDirect3DSurface9* value)
{
    if (slot == value)
        return;

    SafeReleaseTyped(slot);
    if (value != nullptr)
    {
        value->AddRef();
        slot = value;
    }
}

bool IsFullSizeRenderTexture(IDirect3DBaseTexture9* logicalTexture)
{
    if (logicalTexture == nullptr)
        return false;

    IDirect3DBaseTexture9* logical = ResolveRuntimeLogicalTexture(logicalTexture);
    IDirect3DTexture9* replacement = AcquireRuntimeReplacementTexture(logical);
    IDirect3DTexture9* texture = replacement;

    if (texture == nullptr)
    {
        if (FAILED(logical->QueryInterface(
                __uuidof(IDirect3DTexture9),
                reinterpret_cast<void**>(&texture))) ||
            texture == nullptr)
        {
            return false;
        }
    }

    D3DSURFACE_DESC desc{};
    const bool ok = SUCCEEDED(texture->GetLevelDesc(0, &desc)) &&
        desc.Width == g_internalWidth &&
        desc.Height == g_internalHeight &&
        (desc.Usage & D3DUSAGE_RENDERTARGET) != 0;

    texture->Release();
    return ok;
}


float Percentile(std::vector<float>& values, float q)
{
    if (values.empty())
        return 0.0f;
    std::sort(values.begin(), values.end());
    const float pos = std::clamp(q, 0.0f, 1.0f) * static_cast<float>(values.size() - 1);
    const size_t lo = static_cast<size_t>(pos);
    const size_t hi = std::min(values.size() - 1, lo + 1);
    const float t = pos - static_cast<float>(lo);
    return values[lo] * (1.0f - t) + values[hi] * t;
}

float DecodePackedDepthBytes(BYTE b, BYTE g, BYTE r)
{
    const float rf = static_cast<float>(r) / 255.0f;
    const float gf = static_cast<float>(g) / 255.0f;
    const float bf = static_cast<float>(b) / 255.0f;
    const float posZ = bf +
                       gf * 256.0f +
                       rf * 65536.0f;
    return (posZ - 1.0f) / 4999.0f;
}

bool ReadbackA8R8G8B8(
    IDirect3DDevice9* device,
    IDirect3DSurface9* source,
    std::vector<BYTE>& bytes,
    UINT& width,
    UINT& height,
    UINT& pitch)
{
    if (device == nullptr || source == nullptr)
        return false;

    D3DSURFACE_DESC desc{};
    if (FAILED(source->GetDesc(&desc)) ||
        (desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_X8R8G8B8))
    {
        return false;
    }

    IDirect3DSurface9* systemSurface = nullptr;
    HRESULT hr = device->CreateOffscreenPlainSurface(
        desc.Width,
        desc.Height,
        desc.Format,
        D3DPOOL_SYSTEMMEM,
        &systemSurface,
        nullptr);
    if (FAILED(hr) || systemSurface == nullptr)
        return false;

    hr = device->GetRenderTargetData(source, systemSurface);
    if (FAILED(hr))
    {
        systemSurface->Release();
        return false;
    }

    D3DLOCKED_RECT locked{};
    hr = systemSurface->LockRect(&locked, nullptr, D3DLOCK_READONLY);
    if (FAILED(hr))
    {
        systemSurface->Release();
        return false;
    }

    width = desc.Width;
    height = desc.Height;
    pitch = static_cast<UINT>(locked.Pitch);
    bytes.resize(static_cast<size_t>(pitch) * height);
    std::memcpy(bytes.data(), locked.pBits, bytes.size());

    systemSurface->UnlockRect();
    systemSurface->Release();
    return true;
}

void ProbeSsaoBuffers(
    IDirect3DDevice9* device,
    IDirect3DTexture9* depthTexture,
    IDirect3DSurface9* aoSurface)
{
    if (!g_probeRequested.exchange(false, std::memory_order_acq_rel) ||
        device == nullptr || depthTexture == nullptr || aoSurface == nullptr)
    {
        return;
    }

    IDirect3DSurface9* depthSurface = nullptr;
    if (FAILED(depthTexture->GetSurfaceLevel(0, &depthSurface)) || depthSurface == nullptr)
        return;

    std::vector<BYTE> depthBytes;
    std::vector<BYTE> aoBytes;
    UINT dw = 0, dh = 0, dpitch = 0;
    UINT aw = 0, ah = 0, apitch = 0;

    const bool depthOk = ReadbackA8R8G8B8(device, depthSurface, depthBytes, dw, dh, dpitch);
    const bool aoOk = ReadbackA8R8G8B8(device, aoSurface, aoBytes, aw, ah, apitch);
    depthSurface->Release();

    if (!depthOk || !aoOk)
    {
        AppendLog("[SSAO] Probe readback failed; use the F10 probe button to retry.\n");
        return;
    }

    std::vector<float> depths;
    std::vector<float> chanR, chanG, chanB, chanA, packedUnit;
    depths.reserve(8192);
    chanR.reserve(8192);
    chanG.reserve(8192);
    chanB.reserve(8192);
    chanA.reserve(8192);
    packedUnit.reserve(8192);
    unsigned below0075 = 0;
    unsigned above1 = 0;

    const UINT stepX = std::max<UINT>(1, dw / 96);
    const UINT stepY = std::max<UINT>(1, dh / 54);
    for (UINT y = stepY / 2; y < dh; y += stepY)
    {
        const BYTE* row = depthBytes.data() + static_cast<size_t>(y) * dpitch;
        for (UINT x = stepX / 2; x < dw; x += stepX)
        {
            const BYTE* px = row + static_cast<size_t>(x) * 4;
            const float bf = static_cast<float>(px[0]) / 255.0f;
            const float gf = static_cast<float>(px[1]) / 255.0f;
            const float rf = static_cast<float>(px[2]) / 255.0f;
            const float af = static_cast<float>(px[3]) / 255.0f;
            const float d = DecodePackedDepthBytes(px[0], px[1], px[2]);
            const float packed = std::clamp(rf + gf / 255.0f + bf / 65025.0f, 0.0f, 1.0f);
            depths.push_back(d);
            chanR.push_back(rf);
            chanG.push_back(gf);
            chanB.push_back(bf);
            chanA.push_back(af);
            packedUnit.push_back(packed);
            if (d < 0.075f)
                ++below0075;
            if (d > 1.0f)
                ++above1;
        }
    }

    if (depths.empty())
        return;

    std::vector<float> sortedDepths = depths;
    const float dMin = Percentile(sortedDepths, 0.0f);
    sortedDepths = depths;
    const float dP05 = Percentile(sortedDepths, 0.05f);
    sortedDepths = depths;
    const float dMedian = Percentile(sortedDepths, 0.50f);
    sortedDepths = depths;
    const float dP95 = Percentile(sortedDepths, 0.95f);
    sortedDepths = depths;
    const float dMax = Percentile(sortedDepths, 1.0f);

    auto stats5 = [](const std::vector<float>& source, float& mn, float& p05, float& med, float& p95, float& mx)
    {
        std::vector<float> tmp = source; mn = Percentile(tmp, 0.0f);
        tmp = source; p05 = Percentile(tmp, 0.05f);
        tmp = source; med = Percentile(tmp, 0.50f);
        tmp = source; p95 = Percentile(tmp, 0.95f);
        tmp = source; mx = Percentile(tmp, 1.0f);
    };

    // Approximate the new DP-native world-space AO radius at the median depth.
    constexpr float tanHalfFov = 0.9163311740f;
    const float medianEyeZ = std::clamp(1.0f + dMedian * 4999.0f, 1.0f, 5000.0f);
    const float radiusMul = std::clamp(g_config.ssaoRadius, 0.05f, 8.0f);
    const float worldRadius = 2.0f * radiusMul;
    const float tapMedian = std::min(48.0f,
        worldRadius * static_cast<float>(dh) /
        (2.0f * medianEyeZ * tanHalfFov));

    float aoMin = 1.0f;
    float aoMax = 0.0f;
    double aoSum = 0.0;
    unsigned aoSamples = 0;
    const UINT aoStepX = std::max<UINT>(1, aw / 96);
    const UINT aoStepY = std::max<UINT>(1, ah / 54);
    for (UINT y = aoStepY / 2; y < ah; y += aoStepY)
    {
        const BYTE* row = aoBytes.data() + static_cast<size_t>(y) * apitch;
        for (UINT x = aoStepX / 2; x < aw; x += aoStepX)
        {
            const BYTE* px = row + static_cast<size_t>(x) * 4;
            const float a = static_cast<float>(px[2]) / 255.0f; // R channel
            aoMin = std::min(aoMin, a);
            aoMax = std::max(aoMax, a);
            aoSum += a;
            ++aoSamples;
        }
    }

    SsaoRuntimeStats snapshot{};
    snapshot.probeValid = true;
    snapshot.probeSamples = static_cast<unsigned>(depths.size());
    snapshot.depthMin = dMin;
    snapshot.depthP05 = dP05;
    snapshot.depthMedian = dMedian;
    snapshot.depthP95 = dP95;
    snapshot.depthMax = dMax;
    snapshot.depthBelow0075Percent = 100.0f * static_cast<float>(below0075) / static_cast<float>(depths.size());
    snapshot.depthAbove1Percent = 100.0f * static_cast<float>(above1) / static_cast<float>(depths.size());
    snapshot.estimatedTapRadiusMedianPx = tapMedian;
    snapshot.aoMin = aoSamples ? aoMin : 1.0f;
    snapshot.aoMean = aoSamples ? static_cast<float>(aoSum / aoSamples) : 1.0f;
    snapshot.aoMax = aoSamples ? aoMax : 1.0f;

    stats5(chanR, snapshot.channelRMin, snapshot.channelRP05, snapshot.channelRMedian, snapshot.channelRP95, snapshot.channelRMax);
    stats5(chanG, snapshot.channelGMin, snapshot.channelGP05, snapshot.channelGMedian, snapshot.channelGP95, snapshot.channelGMax);
    stats5(chanB, snapshot.channelBMin, snapshot.channelBP05, snapshot.channelBMedian, snapshot.channelBP95, snapshot.channelBMax);
    stats5(chanA, snapshot.channelAMin, snapshot.channelAP05, snapshot.channelAMedian, snapshot.channelAP95, snapshot.channelAMax);
    stats5(packedUnit, snapshot.packedUnitMin, snapshot.packedUnitP05, snapshot.packedUnitMedian, snapshot.packedUnitP95, snapshot.packedUnitMax);

    {
        std::lock_guard<std::mutex> lock(g_probeMutex);
        g_probeSnapshot.probeValid = snapshot.probeValid;
        g_probeSnapshot.probeSamples = snapshot.probeSamples;
        g_probeSnapshot.depthMin = snapshot.depthMin;
        g_probeSnapshot.depthP05 = snapshot.depthP05;
        g_probeSnapshot.depthMedian = snapshot.depthMedian;
        g_probeSnapshot.depthP95 = snapshot.depthP95;
        g_probeSnapshot.depthMax = snapshot.depthMax;
        g_probeSnapshot.depthBelow0075Percent = snapshot.depthBelow0075Percent;
        g_probeSnapshot.depthAbove1Percent = snapshot.depthAbove1Percent;
        g_probeSnapshot.estimatedTapRadiusMedianPx = snapshot.estimatedTapRadiusMedianPx;
        g_probeSnapshot.aoMin = snapshot.aoMin;
        g_probeSnapshot.aoMean = snapshot.aoMean;
        g_probeSnapshot.aoMax = snapshot.aoMax;
        g_probeSnapshot.channelRMin = snapshot.channelRMin; g_probeSnapshot.channelRP05 = snapshot.channelRP05; g_probeSnapshot.channelRMedian = snapshot.channelRMedian; g_probeSnapshot.channelRP95 = snapshot.channelRP95; g_probeSnapshot.channelRMax = snapshot.channelRMax;
        g_probeSnapshot.channelGMin = snapshot.channelGMin; g_probeSnapshot.channelGP05 = snapshot.channelGP05; g_probeSnapshot.channelGMedian = snapshot.channelGMedian; g_probeSnapshot.channelGP95 = snapshot.channelGP95; g_probeSnapshot.channelGMax = snapshot.channelGMax;
        g_probeSnapshot.channelBMin = snapshot.channelBMin; g_probeSnapshot.channelBP05 = snapshot.channelBP05; g_probeSnapshot.channelBMedian = snapshot.channelBMedian; g_probeSnapshot.channelBP95 = snapshot.channelBP95; g_probeSnapshot.channelBMax = snapshot.channelBMax;
        g_probeSnapshot.channelAMin = snapshot.channelAMin; g_probeSnapshot.channelAP05 = snapshot.channelAP05; g_probeSnapshot.channelAMedian = snapshot.channelAMedian; g_probeSnapshot.channelAP95 = snapshot.channelAP95; g_probeSnapshot.channelAMax = snapshot.channelAMax;
        g_probeSnapshot.packedUnitMin = snapshot.packedUnitMin; g_probeSnapshot.packedUnitP05 = snapshot.packedUnitP05; g_probeSnapshot.packedUnitMedian = snapshot.packedUnitMedian; g_probeSnapshot.packedUnitP95 = snapshot.packedUnitP95; g_probeSnapshot.packedUnitMax = snapshot.packedUnitMax;
    }

    char text[1024] = {};
    sprintf_s(
        text,
        "[SSAO] Probe: samples=%u dpDepth[min=%.5f p05=%.5f median=%.5f p95=%.5f max=%.5f] viewZmed~=%.2f <0.075=%.1f%% >1=%.1f%% tapRadius~=%.3f px AO[min=%.5f mean=%.5f max=%.5f].\n",
        snapshot.probeSamples,
        snapshot.depthMin,
        snapshot.depthP05,
        snapshot.depthMedian,
        snapshot.depthP95,
        snapshot.depthMax,
        1.0f + snapshot.depthMedian * 4999.0f,
        snapshot.depthBelow0075Percent,
        snapshot.depthAbove1Percent,
        snapshot.estimatedTapRadiusMedianPx,
        snapshot.aoMin,
        snapshot.aoMean,
        snapshot.aoMax);
    AppendLog(text);
    sprintf_s(
        text,
        "[SSAO] Depth channels: R[%.4f %.4f %.4f %.4f %.4f] G[%.4f %.4f %.4f %.4f %.4f] B[%.4f %.4f %.4f %.4f %.4f] A[%.4f %.4f %.4f %.4f %.4f] packedRGB01[%.5f %.5f %.5f %.5f %.5f].\n",
        snapshot.channelRMin, snapshot.channelRP05, snapshot.channelRMedian, snapshot.channelRP95, snapshot.channelRMax,
        snapshot.channelGMin, snapshot.channelGP05, snapshot.channelGMedian, snapshot.channelGP95, snapshot.channelGMax,
        snapshot.channelBMin, snapshot.channelBP05, snapshot.channelBMedian, snapshot.channelBP95, snapshot.channelBMax,
        snapshot.channelAMin, snapshot.channelAP05, snapshot.channelAMedian, snapshot.channelAP95, snapshot.channelAMax,
        snapshot.packedUnitMin, snapshot.packedUnitP05, snapshot.packedUnitMedian, snapshot.packedUnitP95, snapshot.packedUnitMax);
    AppendLog(text);
}

void ConfigureCommonPassState(IDirect3DDevice9* device)
{
    device->SetDepthStencilSurface(nullptr);
    device->SetRenderTarget(1, nullptr);
    device->SetRenderTarget(2, nullptr);
    device->SetRenderTarget(3, nullptr);
    device->SetVertexShader(nullptr);
    device->SetFVF(kFullscreenFvf);

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
    device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

    for (DWORD stage = 0; stage < 3; ++stage)
    {
        device->SetSamplerState(stage, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        device->SetSamplerState(stage, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        device->SetSamplerState(stage, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
        device->SetSamplerState(stage, D3DSAMP_SRGBTEXTURE, FALSE);
    }

    // DP's depth is a fixed-point integer split over RGB. Bilinear filtering
    // the encoded channels before DecodeEyeZ corrupts byte carries and creates
    // bogus depth values around edges. Always point-sample packed depth.
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    device->SetSamplerState(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
}

HRESULT DrawFullscreen(IDirect3DDevice9* device, UINT width, UINT height)
{
    const float right = static_cast<float>(width) - 0.5f;
    const float bottom = static_cast<float>(height) - 0.5f;

    const FullscreenVertex vertices[4] =
    {
        { -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f },
        { right, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f },
        { -0.5f, bottom, 0.0f, 1.0f, 0.0f, 1.0f },
        { right, bottom, 0.0f, 1.0f, 1.0f, 1.0f }
    };

    return device->DrawPrimitiveUP(
        D3DPT_TRIANGLESTRIP,
        2,
        vertices,
        sizeof(FullscreenVertex));
}

bool SetPassTarget(
    IDirect3DDevice9* device,
    IDirect3DSurface9* target,
    UINT width,
    UINT height)
{
    device->SetTexture(0, nullptr);
    device->SetTexture(1, nullptr);
    device->SetTexture(2, nullptr);

    if (FAILED(device->SetRenderTarget(0, target)))
        return false;

    D3DVIEWPORT9 viewport{};
    viewport.X = 0;
    viewport.Y = 0;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
    return SUCCEEDED(device->SetViewport(&viewport));
}

bool RenderSsao(
    IDirect3DDevice9* device,
    IDirect3DTexture9* frameTexture,
    IDirect3DTexture9* depthTexture,
    IDirect3DTexture9* normalTexture)
{
    g_lastRenderFailureStage = "none";

    if (!EnsureShaders(device))
    {
        g_lastRenderFailureStage = "shader compilation/creation";
        return false;
    }

    D3DSURFACE_DESC frameDesc{};
    if (FAILED(frameTexture->GetLevelDesc(0, &frameDesc)))
    {
        g_lastRenderFailureStage = "frame texture description";
        return false;
    }

    if (!EnsureTargets(device, frameDesc.Width, frameDesc.Height, frameDesc.Format))
    {
        g_lastRenderFailureStage = "render-target allocation";
        return false;
    }

    ConfigureCommonPassState(device);

    // AO generation.
    if (!SetPassTarget(device, g_aoSurfaceA, g_aoWidth, g_aoHeight))
    {
        g_lastRenderFailureStage = "AO target setup";
        return false;
    }
    device->SetPixelShader(g_aoShader);
    device->SetTexture(0, nullptr);
    device->SetTexture(1, depthTexture);
    device->SetTexture(2, normalTexture);

    const float aoConstants[4] =
    {
        1.0f / static_cast<float>(frameDesc.Width),
        1.0f / static_cast<float>(frameDesc.Height),
        std::clamp(g_config.ssaoRadius, 0.05f, 8.0f),
        std::clamp(g_config.ssaoStrength, 0.0f, 2.5f)
    };
    device->SetPixelShaderConstantF(0, aoConstants, 1);
    if (FAILED(DrawFullscreen(device, g_aoWidth, g_aoHeight)))
    {
        g_lastRenderFailureStage = "AO draw";
        return false;
    }

    // Horizontal bilateral blur.
    if (!SetPassTarget(device, g_aoSurfaceB, g_aoWidth, g_aoHeight))
    {
        g_lastRenderFailureStage = "horizontal blur target setup";
        return false;
    }
    device->SetPixelShader(g_blurShader);
    device->SetTexture(0, g_aoTextureA);
    device->SetTexture(1, depthTexture);
    device->SetTexture(2, normalTexture);
    const float blurH[4] =
    {
        1.0f / static_cast<float>(g_aoWidth),
        1.0f / static_cast<float>(g_aoHeight),
        1.0f,
        0.0f
    };
    device->SetPixelShaderConstantF(0, blurH, 1);
    if (FAILED(DrawFullscreen(device, g_aoWidth, g_aoHeight)))
    {
        g_lastRenderFailureStage = "horizontal blur draw";
        return false;
    }

    // Vertical bilateral blur.
    if (!SetPassTarget(device, g_aoSurfaceA, g_aoWidth, g_aoHeight))
    {
        g_lastRenderFailureStage = "vertical blur target setup";
        return false;
    }
    device->SetPixelShader(g_blurShader);
    device->SetTexture(0, g_aoTextureB);
    device->SetTexture(1, depthTexture);
    device->SetTexture(2, normalTexture);
    const float blurV[4] =
    {
        1.0f / static_cast<float>(g_aoWidth),
        1.0f / static_cast<float>(g_aoHeight),
        0.0f,
        1.0f
    };
    device->SetPixelShaderConstantF(0, blurV, 1);
    if (FAILED(DrawFullscreen(device, g_aoWidth, g_aoHeight)))
    {
        g_lastRenderFailureStage = "vertical blur draw";
        return false;
    }

    // Composite AO into a full-resolution HDR/scene copy. The game's own final
    // composition draw will sample this in place of its original stage-0 frame.
    if (!SetPassTarget(device, g_combinedSurface, frameDesc.Width, frameDesc.Height))
    {
        g_lastRenderFailureStage = "combine target setup";
        return false;
    }
    device->SetPixelShader(g_combineShader);
    device->SetTexture(0, frameTexture);

    // AO-only uses the filtered AO target. Depth debug views bind the captured
    // packed-depth texture directly. Solid magenta ignores both in the shader.
    if (g_config.ssaoDebugView == 2 ||
        g_config.ssaoDebugView == 4 ||
        g_config.ssaoDebugView >= 6)
    {
        device->SetTexture(1, depthTexture);
    }
    else
    {
        device->SetTexture(1, g_aoTextureA);
    }

    const float debugConstants[4] =
    {
        static_cast<float>(std::min<UINT>(g_config.ssaoDebugView, 13)),
        0.0f, 0.0f, 0.0f
    };
    device->SetPixelShaderConstantF(1, debugConstants, 1);

    if (FAILED(DrawFullscreen(device, frameDesc.Width, frameDesc.Height)))
    {
        g_lastRenderFailureStage = "combine draw";
        return false;
    }

    ProbeSsaoBuffers(device, depthTexture, g_aoSurfaceA);
    return true;
}

void RestoreTargets(
    IDirect3DDevice9* device,
    IDirect3DSurface9* const renderTargets[4],
    IDirect3DSurface9* depth,
    const D3DVIEWPORT9& viewport,
    IDirect3DStateBlock9* state)
{
    if (state != nullptr)
        state->Apply();

    for (DWORD i = 0; i < 4; ++i)
    {
        if (i == 0 && renderTargets[i] == nullptr)
            continue;
        device->SetRenderTarget(i, renderTargets[i]);
    }
    device->SetDepthStencilSurface(depth);
    device->SetViewport(&viewport);
}

bool IsBackBufferSurface(IDirect3DDevice9* device, IDirect3DSurface9* logicalTarget)
{
    if (device == nullptr || logicalTarget == nullptr)
        return false;

    bool match = false;
    for (UINT i = 0; i < 2; ++i)
    {
        IDirect3DSurface9* backBuffer = nullptr;
        if (SUCCEEDED(device->GetBackBuffer(0, i, D3DBACKBUFFER_TYPE_MONO, &backBuffer)) &&
            backBuffer != nullptr)
        {
            match = logicalTarget == backBuffer;
            backBuffer->Release();
            if (match)
                break;
        }
    }
    return match;
}

bool ApplySsaoToSavedMainSurface(IDirect3DDevice9* device)
{
    g_lastRenderFailureStage = "none";

    if (device == nullptr ||
        g_mainSurfaceLogical == nullptr ||
        g_packedDepthLogical == nullptr ||
        g_normalSurfaceLogical == nullptr)
    {
        g_lastRenderFailureStage = "missing DP main/depth surface";
        return false;
    }

    IDirect3DSurface9* mainSurface = AcquireEffectiveSurface(g_mainSurfaceLogical);
    IDirect3DTexture9* frameTexture = AcquireSurfaceTexture(g_mainSurfaceLogical);
    IDirect3DTexture9* depthTexture = AcquireSurfaceTexture(g_packedDepthLogical);
    IDirect3DTexture9* normalTexture = AcquireSurfaceTexture(g_normalSurfaceLogical);
    if (mainSurface == nullptr || frameTexture == nullptr || depthTexture == nullptr || normalTexture == nullptr)
    {
        g_lastRenderFailureStage = "main/depth/normal texture acquisition";
        SafeReleaseTyped(mainSurface);
        SafeReleaseTyped(frameTexture);
        SafeReleaseTyped(depthTexture);
        SafeReleaseTyped(normalTexture);
        return false;
    }

    IDirect3DStateBlock9* state = nullptr;
    if (FAILED(device->CreateStateBlock(D3DSBT_ALL, &state)) || state == nullptr ||
        FAILED(state->Capture()))
    {
        g_lastRenderFailureStage = "state capture";
        SafeReleaseTyped(state);
        mainSurface->Release();
        frameTexture->Release();
        depthTexture->Release();
        normalTexture->Release();
        return false;
    }

    IDirect3DSurface9* renderTargets[4] = {};
    for (DWORD i = 0; i < 4; ++i)
        device->GetRenderTarget(i, &renderTargets[i]);

    IDirect3DSurface9* depthStencil = nullptr;
    device->GetDepthStencilSurface(&depthStencil);

    D3DVIEWPORT9 viewport{};
    device->GetViewport(&viewport);

    bool renderOk = false;
    {
        InternalPassScope internalScope;
        renderOk = RenderSsao(device, frameTexture, depthTexture, normalTexture);
        RestoreTargets(device, renderTargets, depthStencil, viewport, state);

        if (renderOk)
        {
            const HRESULT copyHr = device->StretchRect(
                g_combinedSurface, nullptr, mainSurface, nullptr, D3DTEXF_NONE);
            renderOk = SUCCEEDED(copyHr);
            if (!renderOk)
                g_lastRenderFailureStage = "copy combined scene back to DP main surface";
        }
    }

    for (IDirect3DSurface9*& rt : renderTargets)
        SafeReleaseTyped(rt);
    SafeReleaseTyped(depthStencil);
    SafeReleaseTyped(state);
    mainSurface->Release();
    frameTexture->Release();
    depthTexture->Release();
    normalTexture->Release();

    return renderOk;
}

void ClearFrameSurfaces()
{
    SafeReleaseTyped(g_packedDepthLogical);
    SafeReleaseTyped(g_normalSurfaceLogical);
    SafeReleaseTyped(g_mainSurfaceLogical);
    g_onBackbuffer = false;
    g_appliedThisFrame = false;
    g_depthPairSeen.store(false, std::memory_order_release);
}
} // namespace

bool IsSsaoInternalPass()
{
    return g_internalPass;
}

bool IsSsaoEnabled()
{
    return g_config.ssaoEnabled;
}

void SsaoBeforeGameSetRenderTarget(
    IDirect3DDevice9* device,
    DWORD,
    IDirect3DSurface9*,
    uintptr_t returnAddress)
{
    if (!g_config.ssaoEnabled || g_internalPass ||
        !IsMainExeCaller(returnAddress) ||
        !g_onBackbuffer || g_appliedThisFrame ||
        g_mainSurfaceLogical == nullptr ||
        g_packedDepthLogical == nullptr ||
        g_normalSurfaceLogical == nullptr)
    {
        return;
    }

    g_appliedThisFrame = true;

    if (ApplySsaoToSavedMainSurface(device))
    {
        g_framesApplied.fetch_add(1, std::memory_order_relaxed);
        if (!g_loggedEarlyApply)
        {
            AppendLog(
                "[SSAO] Applied to DP main scene at the original pre-DoF/pre-exposure integration point.\n");
            g_loggedEarlyApply = true;
        }
        return;
    }

    if (!g_shaderPermanentlyFailed)
        g_failures.fetch_add(1, std::memory_order_relaxed);

    if (!g_loggedPassFailure)
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[SSAO] ERROR: early SSAO pass failed at stage '%s'; game state was restored.\n",
            g_lastRenderFailureStage);
        AppendLog(text);
        g_loggedPassFailure = true;
    }
}

void SsaoAfterGameSetRenderTarget(
    IDirect3DDevice9* device,
    DWORD index,
    IDirect3DSurface9* logicalTarget,
    uintptr_t returnAddress,
    HRESULT setResult)
{
    if (!g_config.ssaoEnabled || g_internalPass || FAILED(setResult) ||
        !IsMainExeCaller(returnAddress))
    {
        return;
    }

    if (index == 1 && logicalTarget != nullptr)
    {
        IDirect3DSurface9* rt0 = nullptr;
        if (SUCCEEDED(device->GetRenderTarget(0, &rt0)) && rt0 != nullptr)
        {
            IDirect3DSurface9* logicalRt0 = ResolveRuntimeLogicalSurface(rt0);
            if (logicalRt0 != nullptr)
            {
                StoreLogicalSurface(g_packedDepthLogical, logicalRt0);
                StoreLogicalSurface(g_normalSurfaceLogical, logicalTarget);
                g_depthPairSeen.store(true, std::memory_order_release);

                if (!g_loggedDepthPair)
                {
                    D3DSURFACE_DESC depthDesc{};
                    D3DSURFACE_DESC normalDesc{};
                    if (GetEffectiveSurfaceDesc(logicalRt0, depthDesc) &&
                        GetEffectiveSurfaceDesc(logicalTarget, normalDesc))
                    {
                        char text[320] = {};
                        sprintf_s(
                            text,
                            "[SSAO] DP G-buffer identified: depth(RT0)=%u x %u fmt=%u, normals(RT1)=%u x %u fmt=%u.\n",
                            depthDesc.Width,
                            depthDesc.Height,
                            static_cast<unsigned>(depthDesc.Format),
                            normalDesc.Width,
                            normalDesc.Height,
                            static_cast<unsigned>(normalDesc.Format));
                        AppendLog(text);
                    }
                    else
                    {
                        AppendLog("[SSAO] DP G-buffer identified from RT1 binding.\n");
                    }
                    g_loggedDepthPair = true;
                }
            }
            rt0->Release();
        }
    }

    if (index == 0)
        g_onBackbuffer = IsBackBufferSurface(device, logicalTarget);
}

void SsaoObserveGameTextureBind(
    IDirect3DDevice9* device,
    DWORD stage,
    IDirect3DBaseTexture9* logicalTexture,
    uintptr_t returnAddress)
{
    if (!g_config.ssaoEnabled || g_internalPass ||
        !IsMainExeCaller(returnAddress) ||
        stage != 8 || logicalTexture == nullptr ||
        !IsFullSizeRenderTexture(logicalTexture))
    {
        return;
    }

    IDirect3DSurface9* currentRt0 = nullptr;
    if (FAILED(device->GetRenderTarget(0, &currentRt0)) || currentRt0 == nullptr)
        return;

    IDirect3DSurface9* logicalRt0 = ResolveRuntimeLogicalSurface(currentRt0);
    if (logicalRt0 != nullptr &&
        logicalRt0 != g_packedDepthLogical &&
        logicalRt0 != g_normalSurfaceLogical)
    {
        StoreLogicalSurface(g_mainSurfaceLogical, logicalRt0);
        if (!g_loggedMainSurface)
        {
            D3DSURFACE_DESC desc{};
            if (GetEffectiveSurfaceDesc(logicalRt0, desc))
            {
                char text[256] = {};
                sprintf_s(
                    text,
                    "[SSAO] DP main scene identified from sampler stage 8: %u x %u fmt=%u.\n",
                    desc.Width,
                    desc.Height,
                    static_cast<unsigned>(desc.Format));
                AppendLog(text);
            }
            else
            {
                AppendLog("[SSAO] DP main scene identified from sampler stage 8.\n");
            }
            g_loggedMainSurface = true;
        }
    }

    currentRt0->Release();
}

void SsaoOnPresent()
{
    ClearFrameSurfaces();
}

void ApplySsaoSettings(const DPFixNGConfig& requested)
{
    const bool resourcesNeedRefresh =
        requested.ssaoResolutionScale != g_config.ssaoResolutionScale;
    const bool disabling = !requested.ssaoEnabled && g_config.ssaoEnabled;
    const bool enabling = requested.ssaoEnabled && !g_config.ssaoEnabled;

    g_config.ssaoEnabled = requested.ssaoEnabled;
    g_config.ssaoStrength = std::clamp(requested.ssaoStrength, 0.0f, 2.5f);
    g_config.ssaoRadius = std::clamp(requested.ssaoRadius, 0.25f, 8.0f);
    g_config.ssaoResolutionScale =
        (requested.ssaoResolutionScale == 1 ||
         requested.ssaoResolutionScale == 2 ||
         requested.ssaoResolutionScale == 4)
            ? requested.ssaoResolutionScale
            : 2;
    const UINT oldDebugView = g_config.ssaoDebugView;
    g_config.ssaoDebugView = std::min<UINT>(requested.ssaoDebugView, 13);

    if (g_config.ssaoDebugView != oldDebugView)
    {
        char text[160] = {};
        sprintf_s(text, "[SSAO] Debug view set to %u.\n", g_config.ssaoDebugView);
        AppendLog(text);
    }

    if (resourcesNeedRefresh || disabling)
        ReleaseSsaoTargets();

    if (resourcesNeedRefresh || disabling || enabling)
    {
        g_targetAllocationFailed = false;
        g_loggedPassFailure = false;
        ClearFrameSurfaces();
    }

    if (g_config.ssaoEnabled)
        g_probeRequested.store(true, std::memory_order_release);
}

void RequestSsaoProbe()
{
    g_probeRequested.store(true, std::memory_order_release);
    AppendLog("[SSAO] Buffer probe requested for the next successful SSAO frame.\n");
}

void ReleaseSsaoResources()
{
    ClearFrameSurfaces();
    ReleaseSsaoTargets();
    SafeReleaseTyped(g_aoShader);
    SafeReleaseTyped(g_blurShader);
    SafeReleaseTyped(g_combineShader);

    if (g_d3dCompilerModule != nullptr)
    {
        FreeLibrary(g_d3dCompilerModule);
        g_d3dCompilerModule = nullptr;
        g_d3dCompile = nullptr;
    }
}

SsaoRuntimeStats GetSsaoRuntimeStats()
{
    SsaoRuntimeStats stats{};
    stats.enabled = g_config.ssaoEnabled;
    stats.depthPairSeen = g_depthPairSeen.load(std::memory_order_acquire);
    stats.resourcesReady =
        g_aoTextureA != nullptr &&
        g_aoTextureB != nullptr &&
        g_combinedTexture != nullptr;
    stats.frameWidth = g_frameWidth;
    stats.frameHeight = g_frameHeight;
    stats.aoWidth = g_aoWidth;
    stats.aoHeight = g_aoHeight;
    const UINT frameBpp = BytesPerPixel(g_frameFormat);
    stats.estimatedBytes =
        static_cast<unsigned long long>(g_aoWidth) *
        static_cast<unsigned long long>(g_aoHeight) * 8ull +
        static_cast<unsigned long long>(g_frameWidth) *
        static_cast<unsigned long long>(g_frameHeight) *
        static_cast<unsigned long long>(frameBpp);
    stats.framesApplied = g_framesApplied.load(std::memory_order_acquire);
    stats.failures = g_failures.load(std::memory_order_acquire);
    {
        std::lock_guard<std::mutex> lock(g_probeMutex);
        stats.probeValid = g_probeSnapshot.probeValid;
        stats.probeSamples = g_probeSnapshot.probeSamples;
        stats.depthMin = g_probeSnapshot.depthMin;
        stats.depthP05 = g_probeSnapshot.depthP05;
        stats.depthMedian = g_probeSnapshot.depthMedian;
        stats.depthP95 = g_probeSnapshot.depthP95;
        stats.depthMax = g_probeSnapshot.depthMax;
        stats.depthBelow0075Percent = g_probeSnapshot.depthBelow0075Percent;
        stats.depthAbove1Percent = g_probeSnapshot.depthAbove1Percent;
        stats.estimatedTapRadiusMedianPx = g_probeSnapshot.estimatedTapRadiusMedianPx;
        stats.aoMin = g_probeSnapshot.aoMin;
        stats.aoMean = g_probeSnapshot.aoMean;
        stats.aoMax = g_probeSnapshot.aoMax;
        stats.channelRMin = g_probeSnapshot.channelRMin; stats.channelRP05 = g_probeSnapshot.channelRP05; stats.channelRMedian = g_probeSnapshot.channelRMedian; stats.channelRP95 = g_probeSnapshot.channelRP95; stats.channelRMax = g_probeSnapshot.channelRMax;
        stats.channelGMin = g_probeSnapshot.channelGMin; stats.channelGP05 = g_probeSnapshot.channelGP05; stats.channelGMedian = g_probeSnapshot.channelGMedian; stats.channelGP95 = g_probeSnapshot.channelGP95; stats.channelGMax = g_probeSnapshot.channelGMax;
        stats.channelBMin = g_probeSnapshot.channelBMin; stats.channelBP05 = g_probeSnapshot.channelBP05; stats.channelBMedian = g_probeSnapshot.channelBMedian; stats.channelBP95 = g_probeSnapshot.channelBP95; stats.channelBMax = g_probeSnapshot.channelBMax;
        stats.channelAMin = g_probeSnapshot.channelAMin; stats.channelAP05 = g_probeSnapshot.channelAP05; stats.channelAMedian = g_probeSnapshot.channelAMedian; stats.channelAP95 = g_probeSnapshot.channelAP95; stats.channelAMax = g_probeSnapshot.channelAMax;
        stats.packedUnitMin = g_probeSnapshot.packedUnitMin; stats.packedUnitP05 = g_probeSnapshot.packedUnitP05; stats.packedUnitMedian = g_probeSnapshot.packedUnitMedian; stats.packedUnitP95 = g_probeSnapshot.packedUnitP95; stats.packedUnitMax = g_probeSnapshot.packedUnitMax;
    }
    return stats;
}
