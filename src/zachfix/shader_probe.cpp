#include "shader_probe.h"

#include "logging.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdio>
#include <cstdint>
#include <cwchar>
#include <cmath>
#include <limits>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace
{
constexpr std::uint64_t kFnvOffsetBasis64 = 14695981039346656037ull;
constexpr std::uint64_t kFnvPrime64 = 1099511628211ull;
constexpr size_t kMaxCapturedEvents = 8192;
constexpr size_t kProbeConstantCount = 32;
constexpr size_t kProbeTextureCount = 5;
constexpr size_t kProbeRenderTargetCount = 4;
constexpr size_t kMaxTargetDrawSnapshots = 32;
constexpr size_t kMaxReadbackPixels = 65536;

// These three post-process shaders were identified from the first diner captures.
// v2 deliberately watches only them so the probe remains narrow and cheap.
constexpr std::uint64_t kLuminanceShaderHash = 0xD9D61A86053C5C81ull;
constexpr std::uint64_t kBrightPassShaderHash = 0x8EBC510F0900A440ull;
constexpr std::uint64_t kFinalCompositeShaderHash = 0x12F56FADBD80F13Bull;

// Research target identified from a dumped skinned, normal-mapped geometry VS.
// Probe v5 keeps the v4 draw-pair/MRT map and also assigns per-capture COM
// resource IDs so render-target surfaces can be linked back to sampled textures.
constexpr std::uint64_t kAoResearchVertexShaderHash = 0xA90F5468C507A2DAull;

enum class ShaderKind : unsigned char
{
    Vertex,
    Pixel
};

struct ShaderBlob
{
    std::vector<unsigned char> bytes;
};

struct CaptureEvent
{
    ShaderKind kind = ShaderKind::Vertex;
    std::uint64_t hash = 0;
};

struct TextureSnapshot
{
    HRESULT getResult = E_FAIL;
    UINT resourceId = 0;
    HRESULT descResult = E_FAIL;
    bool bound = false;
    D3DRESOURCETYPE type = static_cast<D3DRESOURCETYPE>(0);
    UINT width = 0;
    UINT height = 0;
    UINT depth = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    DWORD usage = 0;
    D3DPOOL pool = D3DPOOL_DEFAULT;
};

struct RenderTargetSnapshot
{
    HRESULT getResult = E_FAIL;
    UINT surfaceId = 0;
    UINT resourceId = 0;
    bool resourceIsTexture = false;
    HRESULT descResult = E_FAIL;
    bool bound = false;
    D3DSURFACE_DESC desc = {};
};

struct DrawSurfaceState
{
    bool bound = false;
    UINT surfaceId = 0;
    UINT resourceId = 0;
    bool resourceIsTexture = false;
    UINT width = 0;
    UINT height = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    DWORD usage = 0;
    D3DPOOL pool = D3DPOOL_DEFAULT;
    D3DMULTISAMPLE_TYPE multiSampleType = D3DMULTISAMPLE_NONE;
    DWORD multiSampleQuality = 0;
};

struct DrawStateSignature
{
    std::uint64_t vertexShaderHash = 0;
    std::uint64_t pixelShaderHash = 0;
    UINT viewportX = 0;
    UINT viewportY = 0;
    UINT viewportWidth = 0;
    UINT viewportHeight = 0;
    bool viewportAvailable = false;
    std::array<DrawSurfaceState, kProbeRenderTargetCount> renderTargets = {};
    DrawSurfaceState depthStencil = {};
};

struct DrawStateAggregate
{
    DrawStateSignature signature = {};
    unsigned long long drawCount = 0;
};

struct TextureReadbackSnapshot
{
    bool attempted = false;
    HRESULT getTextureResult = E_FAIL;
    HRESULT descResult = E_FAIL;
    HRESULT getDeviceResult = E_FAIL;
    HRESULT getSurfaceResult = E_FAIL;
    HRESULT createSystemMemResult = E_FAIL;
    HRESULT copyResult = E_FAIL;
    HRESULT lockResult = E_FAIL;
    UINT width = 0;
    UINT height = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    IDirect3DTexture9* texture = nullptr;
    size_t pixelCount = 0;
    std::array<float, 4> first = {};
    std::array<float, 4> minimum = {};
    std::array<float, 4> maximum = {};
    std::array<float, 4> mean = {};
    double clippedRgbFraction = 0.0;
};

struct TargetDrawSnapshot
{
    std::uint64_t pixelShaderHash = 0;
    char drawKind[32] = {};
    HRESULT viewportResult = E_FAIL;
    D3DVIEWPORT9 viewport = {};
    HRESULT constantsResult = E_FAIL;
    std::array<float, kProbeConstantCount * 4> constants = {};
    RenderTargetSnapshot renderTarget;
    std::array<TextureSnapshot, kProbeTextureCount> textures = {};
    std::array<TextureReadbackSnapshot, kProbeTextureCount> readbacks = {};
};

std::mutex g_mutex;
std::unordered_map<IDirect3DVertexShader9*, std::uint64_t> g_vertexShaderHashes;
std::unordered_map<IDirect3DPixelShader9*, std::uint64_t> g_pixelShaderHashes;
std::unordered_map<std::uint64_t, ShaderBlob> g_vertexShaderBlobs;
std::unordered_map<std::uint64_t, ShaderBlob> g_pixelShaderBlobs;
std::unordered_set<std::uint64_t> g_gameVertexShaderHashes;
std::unordered_set<std::uint64_t> g_gamePixelShaderHashes;

std::atomic<unsigned long long> g_registeredVertexShaders{ 0 };
std::atomic<unsigned long long> g_registeredPixelShaders{ 0 };
std::atomic<unsigned long long> g_gameShaderBinds{ 0 };
std::atomic<unsigned long long> g_unknownGameShaderBinds{ 0 };
std::atomic<unsigned long long> g_dumpSuccesses{ 0 };
std::atomic<unsigned long long> g_dumpFailures{ 0 };
std::atomic<std::uint64_t> g_currentVertexShaderHash{ 0 };
std::atomic<std::uint64_t> g_currentPixelShaderHash{ 0 };

bool g_captureRequested = false;
bool g_captureActive = false;
bool g_captureAvailable = false;
std::vector<CaptureEvent> g_captureEvents;
std::unordered_map<std::uint64_t, UINT> g_captureVertexCounts;
std::unordered_map<std::uint64_t, UINT> g_capturePixelCounts;
std::vector<CaptureEvent> g_lastCaptureEvents;
std::unordered_map<std::uint64_t, UINT> g_lastCaptureVertexCounts;
std::unordered_map<std::uint64_t, UINT> g_lastCapturePixelCounts;
std::vector<TargetDrawSnapshot> g_captureTargetDrawSnapshots;
std::vector<TargetDrawSnapshot> g_lastTargetDrawSnapshots;
std::vector<DrawStateAggregate> g_captureDrawStates;
std::vector<DrawStateAggregate> g_lastDrawStates;
unsigned long long g_captureDrawCount = 0;
unsigned long long g_lastCaptureDrawCount = 0;
UINT g_lastTargetVertexShaderPairCount = 0;
UINT g_captureMaxSimultaneousRenderTargets = 0;
UINT g_lastMaxSimultaneousRenderTargets = 0;
std::unordered_map<std::uintptr_t, UINT> g_captureResourceIds;
UINT g_nextCaptureResourceId = 1;
std::atomic<float> g_researchBloomMultiplier{ 1.0f };
std::atomic<float> g_researchExposureMultiplier{ 1.0f };
std::atomic<UINT> g_compositeDebugMode{
    static_cast<UINT>(ShaderProbeCompositeDebugMode::Vanilla)
};

std::uint64_t HashShaderBytes(const void* data, size_t size)
{
    const auto* bytes = static_cast<const unsigned char*>(data);
    std::uint64_t hash = kFnvOffsetBasis64;

    for (size_t i = 0; i < size; ++i)
    {
        hash ^= bytes[i];
        hash *= kFnvPrime64;
    }

    return hash;
}

bool GetGameDirectory(wchar_t* path, size_t pathCount)
{
    if (path == nullptr || pathCount == 0)
        return false;

    const DWORD length = GetModuleFileNameW(nullptr, path, static_cast<DWORD>(pathCount));
    if (length == 0 || length >= pathCount)
        return false;

    wchar_t* slash = wcsrchr(path, L'\\');
    if (slash == nullptr)
        return false;

    *slash = L'\0';
    return true;
}

bool EnsureDirectory(const wchar_t* path)
{
    if (CreateDirectoryW(path, nullptr) != FALSE)
        return true;

    return GetLastError() == ERROR_ALREADY_EXISTS;
}

bool EnsureShaderDirectories()
{
    wchar_t root[MAX_PATH] = {};
    if (!GetGameDirectory(root, MAX_PATH))
        return false;

    wchar_t path[MAX_PATH] = {};
    if (swprintf_s(path, L"%ls\\ZachFix", root) < 0 || !EnsureDirectory(path))
        return false;
    if (swprintf_s(path, L"%ls\\ZachFix\\shaders", root) < 0 || !EnsureDirectory(path))
        return false;
    if (swprintf_s(path, L"%ls\\ZachFix\\shaders\\dump", root) < 0 || !EnsureDirectory(path))
        return false;

    return true;
}

bool BuildShaderPath(
    wchar_t* path,
    size_t pathCount,
    ShaderKind kind,
    std::uint64_t hash,
    const wchar_t* extension)
{
    wchar_t root[MAX_PATH] = {};
    if (!GetGameDirectory(root, MAX_PATH))
        return false;

    return swprintf_s(
        path,
        pathCount,
        L"%ls\\ZachFix\\shaders\\dump\\%ls_%016llX.%ls",
        root,
        kind == ShaderKind::Vertex ? L"vs" : L"ps",
        static_cast<unsigned long long>(hash),
        extension) >= 0;
}

bool BuildCapturePath(wchar_t* path, size_t pathCount)
{
    wchar_t root[MAX_PATH] = {};
    if (!GetGameDirectory(root, MAX_PATH))
        return false;

    return swprintf_s(
        path,
        pathCount,
        L"%ls\\ZachFix\\shaders\\last_frame.txt",
        root) >= 0;
}

template <typename TShader>
bool ReadShaderBytecode(TShader* shader, std::vector<unsigned char>& bytes)
{
    if (shader == nullptr)
        return false;

    UINT size = 0;
    if (FAILED(shader->GetFunction(nullptr, &size)) || size == 0)
        return false;

    bytes.resize(size);
    UINT readSize = size;
    if (FAILED(shader->GetFunction(bytes.data(), &readSize)) || readSize == 0)
    {
        bytes.clear();
        return false;
    }

    bytes.resize(readSize);
    return true;
}

template <typename TShader>
void RegisterShader(
    TShader* shader,
    std::unordered_map<TShader*, std::uint64_t>& pointerHashes,
    std::unordered_map<std::uint64_t, ShaderBlob>& blobs,
    std::atomic<unsigned long long>& registeredCount)
{
    std::vector<unsigned char> bytecode;
    if (!ReadShaderBytecode(shader, bytecode))
        return;

    const std::uint64_t hash = HashShaderBytes(bytecode.data(), bytecode.size());

    std::lock_guard<std::mutex> lock(g_mutex);
    pointerHashes[shader] = hash;

    const auto [it, inserted] = blobs.emplace(hash, ShaderBlob{});
    if (inserted)
    {
        it->second.bytes = std::move(bytecode);
        registeredCount.fetch_add(1, std::memory_order_relaxed);
    }
    else if (it->second.bytes != bytecode)
    {
        char text[256] = {};
        sprintf_s(
            text,
            "[ShaderProbe] WARNING: FNV-1a hash collision for %016llX; keeping first bytecode blob.\n",
            static_cast<unsigned long long>(hash));
        AppendLog(text);
    }
}

template <typename TShader>
void TrackShaderBind(
    TShader* shader,
    ShaderKind kind,
    const std::unordered_map<TShader*, std::uint64_t>& pointerHashes,
    std::unordered_set<std::uint64_t>& gameHashes,
    std::atomic<std::uint64_t>& currentHash)
{
    g_gameShaderBinds.fetch_add(1, std::memory_order_relaxed);

    if (shader == nullptr)
    {
        currentHash.store(0, std::memory_order_release);
        return;
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    const auto found = pointerHashes.find(shader);
    if (found == pointerHashes.end())
    {
        currentHash.store(0, std::memory_order_release);
        g_unknownGameShaderBinds.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    const std::uint64_t hash = found->second;
    currentHash.store(hash, std::memory_order_release);
    gameHashes.insert(hash);

    if (!g_captureActive)
        return;

    auto& counts = kind == ShaderKind::Vertex
        ? g_captureVertexCounts
        : g_capturePixelCounts;
    ++counts[hash];

    if (g_captureEvents.size() < kMaxCapturedEvents)
        g_captureEvents.push_back({ kind, hash });
}

bool IsTargetPostProcessShader(std::uint64_t hash)
{
    return hash == kLuminanceShaderHash ||
           hash == kBrightPassShaderHash ||
           hash == kFinalCompositeShaderHash;
}

const char* TargetPostProcessShaderName(std::uint64_t hash)
{
    if (hash == kLuminanceShaderHash)
        return "luminance/adaptation";
    if (hash == kBrightPassShaderHash)
        return "bright-pass";
    if (hash == kFinalCompositeShaderHash)
        return "final-composite";
    return "unknown";
}

const char* TargetSamplerName(std::uint64_t hash, size_t stage)
{
    if (hash == kLuminanceShaderHash)
    {
        if (stage == 0) return "g_tDiffuse";
        if (stage == 2) return "g_tLuminance";
    }
    else if (hash == kBrightPassShaderHash)
    {
        if (stage == 0) return "g_tDiffuse";
    }
    else if (hash == kFinalCompositeShaderHash)
    {
        if (stage == 0) return "g_tDiffuse";
        if (stage == 1) return "g_tBloom";
        if (stage == 2) return "g_tLuminance";
        if (stage == 3) return "g_tGaussian";
        if (stage == 4) return "g_tDepth";
    }
    return nullptr;
}

const char* ResourceTypeName(D3DRESOURCETYPE type)
{
    switch (type)
    {
    case D3DRTYPE_TEXTURE: return "Texture2D";
    case D3DRTYPE_CUBETEXTURE: return "CubeTexture";
    case D3DRTYPE_VOLUMETEXTURE: return "VolumeTexture";
    default: return "Other";
    }
}

const char* FormatName(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_A8R8G8B8: return "A8R8G8B8";
    case D3DFMT_X8R8G8B8: return "X8R8G8B8";
    case D3DFMT_A2R10G10B10: return "A2R10G10B10";
    case D3DFMT_A16B16G16R16: return "A16B16G16R16";
    case D3DFMT_R16F: return "R16F";
    case D3DFMT_G16R16F: return "G16R16F";
    case D3DFMT_A16B16G16R16F: return "A16B16G16R16F";
    case D3DFMT_R32F: return "R32F";
    case D3DFMT_G32R32F: return "G32R32F";
    case D3DFMT_A32B32G32R32F: return "A32B32G32R32F";
    case D3DFMT_D16: return "D16";
    case D3DFMT_D24X8: return "D24X8";
    case D3DFMT_D24S8: return "D24S8";
    case D3DFMT_D32: return "D32";
    case D3DFMT_D32F_LOCKABLE: return "D32F_LOCKABLE";
    default: return "UNKNOWN/FOURCC";
    }
}

const char* PoolName(D3DPOOL pool)
{
    switch (pool)
    {
    case D3DPOOL_DEFAULT: return "DEFAULT";
    case D3DPOOL_MANAGED: return "MANAGED";
    case D3DPOOL_SYSTEMMEM: return "SYSTEMMEM";
    case D3DPOOL_SCRATCH: return "SCRATCH";
    default: return "UNKNOWN";
    }
}

UINT GetCaptureComObjectId(IUnknown* object)
{
    if (object == nullptr)
        return 0;

    IUnknown* identity = nullptr;
    if (FAILED(object->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&identity))) ||
        identity == nullptr)
    {
        return 0;
    }

    const std::uintptr_t key = reinterpret_cast<std::uintptr_t>(identity);
    identity->Release();

    const auto found = g_captureResourceIds.find(key);
    if (found != g_captureResourceIds.end())
        return found->second;

    const UINT id = g_nextCaptureResourceId++;
    g_captureResourceIds.emplace(key, id);
    return id;
}

void CaptureSurfaceResourceIdentity(
    IDirect3DSurface9* surface,
    RenderTargetSnapshot& snapshot)
{
    if (surface == nullptr)
        return;

    snapshot.surfaceId = GetCaptureComObjectId(surface);

    IDirect3DTexture9* texture = nullptr;
    if (SUCCEEDED(surface->GetContainer(
            __uuidof(IDirect3DTexture9),
            reinterpret_cast<void**>(&texture))) &&
        texture != nullptr)
    {
        snapshot.resourceId = GetCaptureComObjectId(texture);
        snapshot.resourceIsTexture = snapshot.resourceId != 0;
        texture->Release();
    }

    if (snapshot.resourceId == 0)
        snapshot.resourceId = snapshot.surfaceId;
}

DrawSurfaceState MakeDrawSurfaceState(const RenderTargetSnapshot& snapshot)
{
    DrawSurfaceState state{};
    if (!snapshot.bound || FAILED(snapshot.descResult))
        return state;

    state.bound = true;
    state.surfaceId = snapshot.surfaceId;
    state.resourceId = snapshot.resourceId;
    state.resourceIsTexture = snapshot.resourceIsTexture;
    state.width = snapshot.desc.Width;
    state.height = snapshot.desc.Height;
    state.format = snapshot.desc.Format;
    state.usage = snapshot.desc.Usage;
    state.pool = snapshot.desc.Pool;
    state.multiSampleType = snapshot.desc.MultiSampleType;
    state.multiSampleQuality = snapshot.desc.MultiSampleQuality;
    return state;
}

bool SurfaceStateEquals(const DrawSurfaceState& a, const DrawSurfaceState& b)
{
    if (a.bound != b.bound)
        return false;
    if (!a.bound)
        return true;

    return a.resourceId == b.resourceId &&
           a.resourceIsTexture == b.resourceIsTexture &&
           a.width == b.width &&
           a.height == b.height &&
           a.format == b.format &&
           a.usage == b.usage &&
           a.pool == b.pool &&
           a.multiSampleType == b.multiSampleType &&
           a.multiSampleQuality == b.multiSampleQuality;
}

bool DrawStateSignatureEquals(const DrawStateSignature& a, const DrawStateSignature& b)
{
    if (a.vertexShaderHash != b.vertexShaderHash ||
        a.pixelShaderHash != b.pixelShaderHash ||
        a.viewportAvailable != b.viewportAvailable)
    {
        return false;
    }

    if (a.viewportAvailable &&
        (a.viewportX != b.viewportX ||
         a.viewportY != b.viewportY ||
         a.viewportWidth != b.viewportWidth ||
         a.viewportHeight != b.viewportHeight))
    {
        return false;
    }

    for (size_t i = 0; i < kProbeRenderTargetCount; ++i)
    {
        if (!SurfaceStateEquals(a.renderTargets[i], b.renderTargets[i]))
            return false;
    }

    return SurfaceStateEquals(a.depthStencil, b.depthStencil);
}

TextureSnapshot CaptureTextureSnapshot(IDirect3DDevice9* device, DWORD stage)
{
    TextureSnapshot snapshot{};
    IDirect3DBaseTexture9* texture = nullptr;
    snapshot.getResult = device->GetTexture(stage, &texture);
    if (FAILED(snapshot.getResult) || texture == nullptr)
        return snapshot;

    snapshot.bound = true;
    snapshot.resourceId = GetCaptureComObjectId(texture);
    snapshot.type = texture->GetType();

    if (snapshot.type == D3DRTYPE_TEXTURE)
    {
        D3DSURFACE_DESC desc = {};
        snapshot.descResult = static_cast<IDirect3DTexture9*>(texture)->GetLevelDesc(0, &desc);
        if (SUCCEEDED(snapshot.descResult))
        {
            snapshot.width = desc.Width;
            snapshot.height = desc.Height;
            snapshot.depth = 1;
            snapshot.format = desc.Format;
            snapshot.usage = desc.Usage;
            snapshot.pool = desc.Pool;
        }
    }
    else if (snapshot.type == D3DRTYPE_CUBETEXTURE)
    {
        D3DSURFACE_DESC desc = {};
        snapshot.descResult = static_cast<IDirect3DCubeTexture9*>(texture)->GetLevelDesc(0, &desc);
        if (SUCCEEDED(snapshot.descResult))
        {
            snapshot.width = desc.Width;
            snapshot.height = desc.Height;
            snapshot.depth = 1;
            snapshot.format = desc.Format;
            snapshot.usage = desc.Usage;
            snapshot.pool = desc.Pool;
        }
    }
    else if (snapshot.type == D3DRTYPE_VOLUMETEXTURE)
    {
        D3DVOLUME_DESC desc = {};
        snapshot.descResult = static_cast<IDirect3DVolumeTexture9*>(texture)->GetLevelDesc(0, &desc);
        if (SUCCEEDED(snapshot.descResult))
        {
            snapshot.width = desc.Width;
            snapshot.height = desc.Height;
            snapshot.depth = desc.Depth;
            snapshot.format = desc.Format;
            snapshot.usage = desc.Usage;
            snapshot.pool = desc.Pool;
        }
    }

    texture->Release();
    return snapshot;
}

RenderTargetSnapshot CaptureRenderTargetSnapshot(IDirect3DDevice9* device, DWORD index)
{
    RenderTargetSnapshot snapshot{};
    IDirect3DSurface9* surface = nullptr;
    snapshot.getResult = device->GetRenderTarget(index, &surface);
    if (FAILED(snapshot.getResult) || surface == nullptr)
        return snapshot;

    snapshot.bound = true;
    CaptureSurfaceResourceIdentity(surface, snapshot);
    snapshot.descResult = surface->GetDesc(&snapshot.desc);
    surface->Release();
    return snapshot;
}

RenderTargetSnapshot CaptureDepthStencilSnapshot(IDirect3DDevice9* device)
{
    RenderTargetSnapshot snapshot{};
    IDirect3DSurface9* surface = nullptr;
    snapshot.getResult = device->GetDepthStencilSurface(&surface);
    if (FAILED(snapshot.getResult) || surface == nullptr)
        return snapshot;

    snapshot.bound = true;
    CaptureSurfaceResourceIdentity(surface, snapshot);
    snapshot.descResult = surface->GetDesc(&snapshot.desc);
    surface->Release();
    return snapshot;
}

DrawStateSignature CaptureDrawStateSignature(
    IDirect3DDevice9* device,
    std::uint64_t vertexShaderHash,
    std::uint64_t pixelShaderHash)
{
    DrawStateSignature signature{};
    signature.vertexShaderHash = vertexShaderHash;
    signature.pixelShaderHash = pixelShaderHash;

    D3DVIEWPORT9 viewport = {};
    if (SUCCEEDED(device->GetViewport(&viewport)))
    {
        signature.viewportAvailable = true;
        signature.viewportX = viewport.X;
        signature.viewportY = viewport.Y;
        signature.viewportWidth = viewport.Width;
        signature.viewportHeight = viewport.Height;
    }

    for (DWORD index = 0; index < kProbeRenderTargetCount; ++index)
    {
        signature.renderTargets[index] =
            MakeDrawSurfaceState(CaptureRenderTargetSnapshot(device, index));
    }

    signature.depthStencil =
        MakeDrawSurfaceState(CaptureDepthStencilSnapshot(device));
    return signature;
}

void AggregateDrawState(const DrawStateSignature& signature)
{
    auto found = std::find_if(
        g_captureDrawStates.begin(),
        g_captureDrawStates.end(),
        [&signature](const DrawStateAggregate& aggregate)
        {
            return DrawStateSignatureEquals(aggregate.signature, signature);
        });

    if (found == g_captureDrawStates.end())
    {
        DrawStateAggregate aggregate{};
        aggregate.signature = signature;
        g_captureDrawStates.push_back(aggregate);
        found = g_captureDrawStates.end() - 1;
    }

    ++found->drawCount;
}

float HalfToFloat(std::uint16_t value)
{
    const std::uint32_t sign = (static_cast<std::uint32_t>(value & 0x8000u)) << 16;
    std::uint32_t exponent = (value >> 10) & 0x1Fu;
    std::uint32_t mantissa = value & 0x03FFu;
    std::uint32_t bits = 0;

    if (exponent == 0)
    {
        if (mantissa == 0)
        {
            bits = sign;
        }
        else
        {
            int unbiasedExponent = -14;
            while ((mantissa & 0x0400u) == 0)
            {
                mantissa <<= 1;
                --unbiasedExponent;
            }
            mantissa &= 0x03FFu;
            const std::uint32_t floatExponent =
                static_cast<std::uint32_t>(unbiasedExponent + 127);
            bits = sign | (floatExponent << 23) | (mantissa << 13);
        }
    }
    else if (exponent == 0x1Fu)
    {
        bits = sign | 0x7F800000u | (mantissa << 13);
    }
    else
    {
        const std::uint32_t floatExponent = exponent + (127u - 15u);
        bits = sign | (floatExponent << 23) | (mantissa << 13);
    }

    union FloatBits
    {
        std::uint32_t u;
        float f;
    } converted{};
    converted.u = bits;
    return converted.f;
}

bool ShouldReadBackTexture(std::uint64_t hash, DWORD stage)
{
    // Luminance history/current value plus the small bloom target.
    if (hash == kLuminanceShaderHash)
        return stage == 0 || stage == 2;
    if (hash == kFinalCompositeShaderHash)
        return stage == 1 || stage == 2;
    return false;
}

TextureReadbackSnapshot CaptureTextureReadbackReference(
    IDirect3DDevice9* device,
    DWORD stage)
{
    TextureReadbackSnapshot snapshot{};
    snapshot.attempted = true;

    IDirect3DBaseTexture9* baseTexture = nullptr;
    snapshot.getTextureResult = device->GetTexture(stage, &baseTexture);
    if (FAILED(snapshot.getTextureResult) || baseTexture == nullptr)
        return snapshot;

    if (baseTexture->GetType() != D3DRTYPE_TEXTURE)
    {
        baseTexture->Release();
        return snapshot;
    }

    auto* texture = static_cast<IDirect3DTexture9*>(baseTexture);
    D3DSURFACE_DESC desc = {};
    snapshot.descResult = texture->GetLevelDesc(0, &desc);
    if (FAILED(snapshot.descResult))
    {
        baseTexture->Release();
        return snapshot;
    }

    snapshot.width = desc.Width;
    snapshot.height = desc.Height;
    snapshot.format = desc.Format;

    const size_t pixelCount = static_cast<size_t>(desc.Width) * static_cast<size_t>(desc.Height);
    if (pixelCount == 0 || pixelCount > kMaxReadbackPixels ||
        (desc.Usage & D3DUSAGE_RENDERTARGET) == 0 || desc.Pool != D3DPOOL_DEFAULT ||
        (desc.Format != D3DFMT_A16B16G16R16F &&
         desc.Format != D3DFMT_A8R8G8B8 && desc.Format != D3DFMT_X8R8G8B8))
    {
        baseTexture->Release();
        return snapshot;
    }

    // GetTexture returned an AddRef'd pointer. Keep it until Present, when we
    // are outside the game's BeginScene/EndScene pair and can safely read it back.
    snapshot.texture = texture;
    return snapshot;
}

void CompleteTextureReadback(TextureReadbackSnapshot& snapshot)
{
    if (!snapshot.attempted || snapshot.texture == nullptr)
        return;

    IDirect3DTexture9* texture = snapshot.texture;
    snapshot.texture = nullptr;

    IDirect3DDevice9* device = nullptr;
    snapshot.getDeviceResult = texture->GetDevice(&device);
    if (FAILED(snapshot.getDeviceResult) || device == nullptr)
    {
        texture->Release();
        return;
    }

    IDirect3DSurface9* source = nullptr;
    snapshot.getSurfaceResult = texture->GetSurfaceLevel(0, &source);
    if (FAILED(snapshot.getSurfaceResult) || source == nullptr)
    {
        device->Release();
        texture->Release();
        return;
    }

    IDirect3DSurface9* systemMem = nullptr;
    snapshot.createSystemMemResult = device->CreateOffscreenPlainSurface(
        snapshot.width, snapshot.height, snapshot.format,
        D3DPOOL_SYSTEMMEM, &systemMem, nullptr);
    if (FAILED(snapshot.createSystemMemResult) || systemMem == nullptr)
    {
        source->Release();
        device->Release();
        texture->Release();
        return;
    }

    snapshot.copyResult = device->GetRenderTargetData(source, systemMem);
    source->Release();
    if (FAILED(snapshot.copyResult))
    {
        systemMem->Release();
        device->Release();
        texture->Release();
        return;
    }

    D3DLOCKED_RECT locked = {};
    snapshot.lockResult = systemMem->LockRect(&locked, nullptr, D3DLOCK_READONLY);
    if (FAILED(snapshot.lockResult))
    {
        systemMem->Release();
        device->Release();
        texture->Release();
        return;
    }

    std::array<double, 4> sums = {};
    snapshot.minimum.fill(std::numeric_limits<float>::infinity());
    snapshot.maximum.fill(-std::numeric_limits<float>::infinity());
    size_t clippedRgb = 0;

    for (UINT y = 0; y < snapshot.height; ++y)
    {
        const auto* row = static_cast<const unsigned char*>(locked.pBits) +
                          static_cast<size_t>(y) * static_cast<size_t>(locked.Pitch);
        for (UINT x = 0; x < snapshot.width; ++x)
        {
            std::array<float, 4> rgba = {};
            if (snapshot.format == D3DFMT_A16B16G16R16F)
            {
                const auto* half = reinterpret_cast<const std::uint16_t*>(
                    row + static_cast<size_t>(x) * 8u);
                rgba = {
                    HalfToFloat(half[0]), HalfToFloat(half[1]),
                    HalfToFloat(half[2]), HalfToFloat(half[3])
                };
            }
            else
            {
                const auto* pixel = reinterpret_cast<const std::uint32_t*>(
                    row + static_cast<size_t>(x) * 4u);
                const std::uint32_t packed = *pixel;
                rgba[0] = static_cast<float>((packed >> 16) & 0xFFu) / 255.0f;
                rgba[1] = static_cast<float>((packed >> 8) & 0xFFu) / 255.0f;
                rgba[2] = static_cast<float>(packed & 0xFFu) / 255.0f;
                rgba[3] = snapshot.format == D3DFMT_X8R8G8B8
                    ? 1.0f
                    : static_cast<float>((packed >> 24) & 0xFFu) / 255.0f;
            }

            if (snapshot.pixelCount == 0)
                snapshot.first = rgba;

            for (size_t channel = 0; channel < 4; ++channel)
            {
                sums[channel] += rgba[channel];
                snapshot.minimum[channel] = std::min(snapshot.minimum[channel], rgba[channel]);
                snapshot.maximum[channel] = std::max(snapshot.maximum[channel], rgba[channel]);
            }

            for (size_t channel = 0; channel < 3; ++channel)
            {
                if (rgba[channel] >= 0.98f)
                    ++clippedRgb;
            }
            ++snapshot.pixelCount;
        }
    }

    systemMem->UnlockRect();
    systemMem->Release();
    device->Release();
    texture->Release();

    if (snapshot.pixelCount != 0)
    {
        for (size_t channel = 0; channel < 4; ++channel)
        {
            snapshot.mean[channel] = static_cast<float>(
                sums[channel] / static_cast<double>(snapshot.pixelCount));
        }
        snapshot.clippedRgbFraction = static_cast<double>(clippedRgb) /
            static_cast<double>(snapshot.pixelCount * 3u);
    }
}

void CompleteCaptureReadbacks(std::vector<TargetDrawSnapshot>& snapshots)
{
    for (TargetDrawSnapshot& draw : snapshots)
    {
        for (TextureReadbackSnapshot& readback : draw.readbacks)
            CompleteTextureReadback(readback);
    }
}

TargetDrawSnapshot CaptureTargetDrawSnapshot(
    IDirect3DDevice9* device,
    const char* drawKind,
    std::uint64_t pixelShaderHash)
{
    TargetDrawSnapshot snapshot{};
    snapshot.pixelShaderHash = pixelShaderHash;
    strcpy_s(
        snapshot.drawKind,
        sizeof(snapshot.drawKind),
        drawKind != nullptr ? drawKind : "Draw");
    snapshot.viewportResult = device->GetViewport(&snapshot.viewport);
    snapshot.constantsResult = device->GetPixelShaderConstantF(
        0, snapshot.constants.data(), static_cast<UINT>(kProbeConstantCount));
    snapshot.renderTarget = CaptureRenderTargetSnapshot(device, 0);

    for (DWORD stage = 0; stage < kProbeTextureCount; ++stage)
    {
        snapshot.textures[stage] = CaptureTextureSnapshot(device, stage);
        if (ShouldReadBackTexture(pixelShaderHash, stage))
            snapshot.readbacks[stage] = CaptureTextureReadbackReference(device, stage);
    }

    return snapshot;
}

void WriteConstant(
    FILE* file,
    const TargetDrawSnapshot& snapshot,
    UINT registerIndex,
    const char* name)
{
    if (FAILED(snapshot.constantsResult) || registerIndex >= kProbeConstantCount)
    {
        std::fprintf(file, "  c%u %-16s <unavailable>\n", registerIndex, name);
        return;
    }

    const size_t base = static_cast<size_t>(registerIndex) * 4;
    std::fprintf(
        file,
        "  c%u %-16s { %.9g, %.9g, %.9g, %.9g }\n",
        registerIndex,
        name,
        snapshot.constants[base + 0],
        snapshot.constants[base + 1],
        snapshot.constants[base + 2],
        snapshot.constants[base + 3]);
}

void WriteTargetDrawSnapshot(FILE* file, size_t index, const TargetDrawSnapshot& snapshot)
{
    std::fprintf(
        file,
        "Draw %zu: %s  ps_%016llX  (%s)\n",
        index,
        snapshot.drawKind,
        static_cast<unsigned long long>(snapshot.pixelShaderHash),
        TargetPostProcessShaderName(snapshot.pixelShaderHash));

    if (SUCCEEDED(snapshot.viewportResult))
    {
        std::fprintf(
            file,
            "Viewport: %u,%u %ux%u  Z=[%.6g, %.6g]\n",
            snapshot.viewport.X, snapshot.viewport.Y,
            snapshot.viewport.Width, snapshot.viewport.Height,
            snapshot.viewport.MinZ, snapshot.viewport.MaxZ);
    }
    else
    {
        std::fprintf(file, "Viewport: <GetViewport failed 0x%08X>\n",
                     static_cast<unsigned>(snapshot.viewportResult));
    }

    if (SUCCEEDED(snapshot.renderTarget.getResult) &&
        SUCCEEDED(snapshot.renderTarget.descResult))
    {
        const D3DSURFACE_DESC& desc = snapshot.renderTarget.desc;
        std::fprintf(
            file,
            "RT0: %ux%u  %s (0x%08X)  usage=0x%08X  pool=%s  msaa=%u  resource=%s#%u  surface=surf#%u\n",
            desc.Width, desc.Height, FormatName(desc.Format),
            static_cast<unsigned>(desc.Format),
            static_cast<unsigned>(desc.Usage), PoolName(desc.Pool),
            static_cast<unsigned>(desc.MultiSampleType),
            snapshot.renderTarget.resourceIsTexture ? "tex" : "res",
            snapshot.renderTarget.resourceId,
            snapshot.renderTarget.surfaceId);
    }
    else
    {
        std::fprintf(
            file,
            "RT0: <unavailable get=0x%08X desc=0x%08X>\n",
            static_cast<unsigned>(snapshot.renderTarget.getResult),
            static_cast<unsigned>(snapshot.renderTarget.descResult));
    }

    std::fprintf(file, "Named constants:\n");
    if (snapshot.pixelShaderHash == kLuminanceShaderHash)
    {
        WriteConstant(file, snapshot, 1, "g_fElapsed");
        WriteConstant(file, snapshot, 2, "g_vBrightLim");
    }
    else if (snapshot.pixelShaderHash == kBrightPassShaderHash)
    {
        WriteConstant(file, snapshot, 3, "g_fBrightPass");
    }
    else if (snapshot.pixelShaderHash == kFinalCompositeShaderHash)
    {
        WriteConstant(file, snapshot, 9, "g_fBloomForce");
        WriteConstant(file, snapshot, 10, "g_fExposure");
        WriteConstant(file, snapshot, 15, "g_vDofprm");
        WriteConstant(file, snapshot, 16, "g_fFocus");
    }

    std::fprintf(file, "Textures s0..s%zu:\n", kProbeTextureCount - 1);
    for (size_t stage = 0; stage < snapshot.textures.size(); ++stage)
    {
        const TextureSnapshot& texture = snapshot.textures[stage];
        const char* samplerName = TargetSamplerName(snapshot.pixelShaderHash, stage);
        const char* samplerSuffix = samplerName != nullptr ? samplerName : "unused/unknown";
        if (FAILED(texture.getResult))
        {
            std::fprintf(file, "  s%zu (%s): <GetTexture failed 0x%08X>\n",
                         stage, samplerSuffix, static_cast<unsigned>(texture.getResult));
            continue;
        }
        if (!texture.bound)
        {
            std::fprintf(file, "  s%zu (%s): <null>\n", stage, samplerSuffix);
            continue;
        }
        if (FAILED(texture.descResult))
        {
            std::fprintf(
                file,
                "  s%zu (%s): %s  <level-0 desc unavailable 0x%08X>\n",
                stage, samplerSuffix, ResourceTypeName(texture.type),
                static_cast<unsigned>(texture.descResult));
            continue;
        }

        std::fprintf(
            file,
            "  s%zu (%s): %s tex#%u %ux%u",
            stage, samplerSuffix, ResourceTypeName(texture.type), texture.resourceId,
            texture.width, texture.height);
        if (texture.depth > 1)
            std::fprintf(file, "x%u", texture.depth);
        std::fprintf(
            file,
            "  %s (0x%08X)  usage=0x%08X  pool=%s\n",
            FormatName(texture.format), static_cast<unsigned>(texture.format),
            static_cast<unsigned>(texture.usage), PoolName(texture.pool));
    }

    bool wroteReadbackHeader = false;
    for (size_t stage = 0; stage < snapshot.readbacks.size(); ++stage)
    {
        const TextureReadbackSnapshot& readback = snapshot.readbacks[stage];
        if (!readback.attempted)
            continue;

        if (!wroteReadbackHeader)
        {
            std::fprintf(file, "Selected texture readbacks:\n");
            wroteReadbackHeader = true;
        }

        const char* samplerName = TargetSamplerName(snapshot.pixelShaderHash, stage);
        const char* samplerSuffix = samplerName != nullptr ? samplerName : "unused/unknown";
        if (readback.pixelCount == 0)
        {
            std::fprintf(
                file,
                "  s%zu (%s): <readback unavailable get=0x%08X desc=0x%08X device=0x%08X surface=0x%08X sysmem=0x%08X copy=0x%08X lock=0x%08X>\n",
                stage, samplerSuffix,
                static_cast<unsigned>(readback.getTextureResult),
                static_cast<unsigned>(readback.descResult),
                static_cast<unsigned>(readback.getDeviceResult),
                static_cast<unsigned>(readback.getSurfaceResult),
                static_cast<unsigned>(readback.createSystemMemResult),
                static_cast<unsigned>(readback.copyResult),
                static_cast<unsigned>(readback.lockResult));
            continue;
        }

        std::fprintf(
            file,
            "  s%zu (%s): %ux%u %s  pixels=%zu\n",
            stage, samplerSuffix,
            readback.width, readback.height, FormatName(readback.format), readback.pixelCount);
        std::fprintf(
            file,
            "    first RGBA = { %.9g, %.9g, %.9g, %.9g }\n",
            readback.first[0], readback.first[1], readback.first[2], readback.first[3]);
        std::fprintf(
            file,
            "    mean  RGBA = { %.9g, %.9g, %.9g, %.9g }\n",
            readback.mean[0], readback.mean[1], readback.mean[2], readback.mean[3]);
        std::fprintf(
            file,
            "    min   RGBA = { %.9g, %.9g, %.9g, %.9g }\n",
            readback.minimum[0], readback.minimum[1], readback.minimum[2], readback.minimum[3]);
        std::fprintf(
            file,
            "    max   RGBA = { %.9g, %.9g, %.9g, %.9g }\n",
            readback.maximum[0], readback.maximum[1], readback.maximum[2], readback.maximum[3]);
        std::fprintf(
            file,
            "    RGB samples >= 0.98: %.3f%%\n",
            readback.clippedRgbFraction * 100.0);
    }

    std::fprintf(file, "All float constants c0..c%zu:\n", kProbeConstantCount - 1);
    if (SUCCEEDED(snapshot.constantsResult))
    {
        for (size_t reg = 0; reg < kProbeConstantCount; ++reg)
        {
            const size_t base = reg * 4;
            std::fprintf(
                file,
                "  c%02zu = { %.9g, %.9g, %.9g, %.9g }\n",
                reg,
                snapshot.constants[base + 0],
                snapshot.constants[base + 1],
                snapshot.constants[base + 2],
                snapshot.constants[base + 3]);
        }
    }
    else
    {
        std::fprintf(file, "  <GetPixelShaderConstantF failed 0x%08X>\n",
                     static_cast<unsigned>(snapshot.constantsResult));
    }

    std::fprintf(file, "\n");
}

bool WriteBlob(const wchar_t* path, const std::vector<unsigned char>& bytes)
{
    FILE* file = nullptr;
    if (_wfopen_s(&file, path, L"wb") != 0 || file == nullptr)
        return false;

    const size_t written = std::fwrite(bytes.data(), 1, bytes.size(), file);
    std::fclose(file);
    return written == bytes.size();
}

void WriteSurfaceState(FILE* file, const char* label, const DrawSurfaceState& surface)
{
    if (!surface.bound)
    {
        std::fprintf(file, "%s=<none>", label);
        return;
    }

    std::fprintf(
        file,
        "%s=%ux%u/%s(0x%08X)/msaa=%u/%s#%u/surf#%u",
        label,
        surface.width,
        surface.height,
        FormatName(surface.format),
        static_cast<unsigned>(surface.format),
        static_cast<unsigned>(surface.multiSampleType),
        surface.resourceIsTexture ? "tex" : "res",
        surface.resourceId,
        surface.surfaceId);
}

void WriteDrawStateAggregate(FILE* file, size_t index, const DrawStateAggregate& aggregate)
{
    const DrawStateSignature& signature = aggregate.signature;
    std::fprintf(
        file,
        "%04zu  draws=%llu  VS=vs_%016llX  PS=ps_%016llX",
        index,
        aggregate.drawCount,
        static_cast<unsigned long long>(signature.vertexShaderHash),
        static_cast<unsigned long long>(signature.pixelShaderHash));

    if (signature.viewportAvailable)
    {
        std::fprintf(
            file,
            "  VP=%u,%u %ux%u",
            signature.viewportX,
            signature.viewportY,
            signature.viewportWidth,
            signature.viewportHeight);
    }
    else
    {
        std::fprintf(file, "  VP=<unavailable>");
    }

    for (size_t rt = 0; rt < kProbeRenderTargetCount; ++rt)
    {
        char label[8] = {};
        sprintf_s(label, "RT%zu", rt);
        std::fprintf(file, "  ");
        WriteSurfaceState(file, label, signature.renderTargets[rt]);
    }

    std::fprintf(file, "  ");
    WriteSurfaceState(file, "DS", signature.depthStencil);
    std::fprintf(file, "\n");
}

std::vector<std::pair<std::uint64_t, unsigned long long>> CollectTargetVertexShaderPairs(
    const std::vector<DrawStateAggregate>& drawStates)
{
    std::unordered_map<std::uint64_t, unsigned long long> counts;
    for (const DrawStateAggregate& aggregate : drawStates)
    {
        if (aggregate.signature.vertexShaderHash != kAoResearchVertexShaderHash ||
            aggregate.signature.pixelShaderHash == 0)
        {
            continue;
        }

        counts[aggregate.signature.pixelShaderHash] += aggregate.drawCount;
    }

    std::vector<std::pair<std::uint64_t, unsigned long long>> sorted(
        counts.begin(), counts.end());
    std::sort(
        sorted.begin(), sorted.end(),
        [](const auto& a, const auto& b)
        {
            if (a.second != b.second)
                return a.second > b.second;
            return a.first < b.first;
        });
    return sorted;
}

void DumpTargetVertexShaderPairBlobsLocked(
    const std::vector<std::pair<std::uint64_t, unsigned long long>>& pairs)
{
    if (pairs.empty())
        return;

    if (!EnsureShaderDirectories())
    {
        AppendLog("[ShaderProbe] WARNING: Could not create shader dump directory for target VS pairs.\n");
        return;
    }

    unsigned long long successes = 0;
    unsigned long long failures = 0;

    const auto dumpHash = [&successes, &failures](
        ShaderKind kind,
        std::uint64_t hash,
        const std::unordered_map<std::uint64_t, ShaderBlob>& blobs)
    {
        const auto found = blobs.find(hash);
        if (found == blobs.end())
        {
            ++failures;
            return;
        }

        wchar_t path[MAX_PATH] = {};
        if (!BuildShaderPath(path, MAX_PATH, kind, hash, L"bin") ||
            !WriteBlob(path, found->second.bytes))
        {
            ++failures;
            return;
        }

        ++successes;
    };

    dumpHash(ShaderKind::Vertex, kAoResearchVertexShaderHash, g_vertexShaderBlobs);
    for (const auto& [pixelShaderHash, drawCount] : pairs)
    {
        (void)drawCount;
        dumpHash(ShaderKind::Pixel, pixelShaderHash, g_pixelShaderBlobs);
    }

    g_dumpSuccesses.fetch_add(successes, std::memory_order_relaxed);
    g_dumpFailures.fetch_add(failures, std::memory_order_relaxed);

    char text[320] = {};
    sprintf_s(
        text,
        "[ShaderProbe] Target VS A90F5468C507A2DA: %zu PS pair(s), %llu shader blob(s) written, %llu failed.\n",
        pairs.size(),
        successes,
        failures);
    AppendLog(text);
}

void WriteResourceIdentitySummary(FILE* file)
{
    const TargetDrawSnapshot* finalComposite = nullptr;
    for (const TargetDrawSnapshot& snapshot : g_lastTargetDrawSnapshots)
    {
        if (snapshot.pixelShaderHash == kFinalCompositeShaderHash)
        {
            finalComposite = &snapshot;
            break;
        }
    }

    std::fprintf(file, "\n[Resource identity links]\n");
    if (finalComposite == nullptr)
    {
        std::fprintf(file, "Final composite draw was not captured.\n");
        return;
    }

    const UINT hdrResourceId = finalComposite->textures[0].resourceId;
    const UINT depthResourceId = finalComposite->textures[4].resourceId;
    std::fprintf(file, "Final composite s0 g_tDiffuse (HDR): tex#%u\n", hdrResourceId);
    std::fprintf(file, "Final composite s4 g_tDepth:        tex#%u\n", depthResourceId);

    unsigned long long depthAsRt0Draws = 0;
    unsigned long long depthAsRt1Draws = 0;
    unsigned long long hdrAsRt0Draws = 0;
    std::unordered_map<UINT, unsigned long long> normalPartnerDraws;

    for (const DrawStateAggregate& aggregate : g_lastDrawStates)
    {
        const DrawStateSignature& signature = aggregate.signature;
        if (signature.renderTargets[0].resourceId == hdrResourceId && hdrResourceId != 0)
            hdrAsRt0Draws += aggregate.drawCount;

        if (signature.renderTargets[0].resourceId == depthResourceId && depthResourceId != 0)
        {
            depthAsRt0Draws += aggregate.drawCount;
            const UINT partner = signature.renderTargets[1].resourceId;
            if (partner != 0)
                normalPartnerDraws[partner] += aggregate.drawCount;
        }

        if (signature.renderTargets[1].resourceId == depthResourceId && depthResourceId != 0)
            depthAsRt1Draws += aggregate.drawCount;
    }

    std::fprintf(
        file,
        "HDR tex#%u observed as RT0 on %llu captured draw(s).\n",
        hdrResourceId, hdrAsRt0Draws);
    std::fprintf(
        file,
        "Depth tex#%u observed as RT0 on %llu draw(s), as RT1 on %llu draw(s).\n",
        depthResourceId, depthAsRt0Draws, depthAsRt1Draws);

    if (normalPartnerDraws.empty())
    {
        std::fprintf(file, "No RT1 partner was observed while final s4 depth was bound as RT0.\n");
        return;
    }

    std::vector<std::pair<UINT, unsigned long long>> partners(
        normalPartnerDraws.begin(), normalPartnerDraws.end());
    std::sort(
        partners.begin(), partners.end(),
        [](const auto& a, const auto& b)
        {
            if (a.second != b.second)
                return a.second > b.second;
            return a.first < b.first;
        });

    std::fprintf(file, "RT1 partner resource(s) while final s4 depth is RT0:\n");
    for (const auto& [resourceId, drawCount] : partners)
    {
        std::fprintf(
            file,
            "  tex#%u  draws=%llu  (candidate normal buffer)\n",
            resourceId, drawCount);
    }
}

void WriteCaptureReportLocked()
{
    if (!EnsureShaderDirectories())
    {
        AppendLog("[ShaderProbe] WARNING: Could not create shader capture directory.\n");
        return;
    }

    wchar_t path[MAX_PATH] = {};
    if (!BuildCapturePath(path, MAX_PATH))
        return;

    FILE* file = nullptr;
    if (_wfopen_s(&file, path, L"w") != 0 || file == nullptr)
    {
        AppendLog("[ShaderProbe] WARNING: Could not write last_frame.txt.\n");
        return;
    }

    std::fprintf(file, "ZachFix Shader Probe v5 - draw pairs + MRT/depth map + COM resource identity\n");
    std::fprintf(file, "Captured events: %llu (ordered list capped at %zu)\n",
                 static_cast<unsigned long long>(g_lastCaptureEvents.size()),
                 kMaxCapturedEvents);
    std::fprintf(file, "Unique VS: %zu\n", g_lastCaptureVertexCounts.size());
    std::fprintf(file, "Unique PS: %zu\n", g_lastCapturePixelCounts.size());
    std::fprintf(file, "Captured game draws: %llu\n", g_lastCaptureDrawCount);
    std::fprintf(file, "Device MaxSimultaneousRenderTargets: %u\n",
                 g_lastMaxSimultaneousRenderTargets);
    std::fprintf(file, "Unique draw-state signatures: %zu\n\n", g_lastDrawStates.size());

    auto writeCounts = [file](
        const char* title,
        const char* prefix,
        const std::unordered_map<std::uint64_t, UINT>& counts)
    {
        std::vector<std::pair<std::uint64_t, UINT>> sorted(counts.begin(), counts.end());
        std::sort(
            sorted.begin(), sorted.end(),
            [](const auto& a, const auto& b)
            {
                if (a.second != b.second)
                    return a.second > b.second;
                return a.first < b.first;
            });

        std::fprintf(file, "[%s]\n", title);
        for (const auto& [hash, count] : sorted)
        {
            std::fprintf(
                file,
                "%s_%016llX  binds=%u\n",
                prefix,
                static_cast<unsigned long long>(hash),
                count);
        }
        std::fprintf(file, "\n");
    };

    writeCounts("Vertex shader bind counts", "vs", g_lastCaptureVertexCounts);
    writeCounts("Pixel shader bind counts", "ps", g_lastCapturePixelCounts);

    std::fprintf(file, "[Ordered shader binds]\n");
    for (size_t i = 0; i < g_lastCaptureEvents.size(); ++i)
    {
        const CaptureEvent& event = g_lastCaptureEvents[i];
        std::fprintf(
            file,
            "%04zu  %s_%016llX\n",
            i,
            event.kind == ShaderKind::Vertex ? "vs" : "ps",
            static_cast<unsigned long long>(event.hash));
    }

    std::fprintf(file, "\n[Observed draw shader pairs + RT/MRT/depth state]\n");
    std::fprintf(
        file,
        "Each row is aggregated by VS + PS + viewport + RT0..RT3 + depth-stencil.\n");
    std::fprintf(
        file,
        "RT slots that are not bound (or unsupported by the device) appear as <none>.\n\n");

    std::vector<DrawStateAggregate> sortedDrawStates = g_lastDrawStates;
    std::sort(
        sortedDrawStates.begin(), sortedDrawStates.end(),
        [](const DrawStateAggregate& a, const DrawStateAggregate& b)
        {
            if (a.signature.vertexShaderHash == kAoResearchVertexShaderHash &&
                b.signature.vertexShaderHash != kAoResearchVertexShaderHash)
            {
                return true;
            }
            if (a.signature.vertexShaderHash != kAoResearchVertexShaderHash &&
                b.signature.vertexShaderHash == kAoResearchVertexShaderHash)
            {
                return false;
            }
            if (a.drawCount != b.drawCount)
                return a.drawCount > b.drawCount;
            if (a.signature.vertexShaderHash != b.signature.vertexShaderHash)
                return a.signature.vertexShaderHash < b.signature.vertexShaderHash;
            return a.signature.pixelShaderHash < b.signature.pixelShaderHash;
        });

    for (size_t i = 0; i < sortedDrawStates.size(); ++i)
        WriteDrawStateAggregate(file, i, sortedDrawStates[i]);

    WriteResourceIdentitySummary(file);

    const auto targetPairs = CollectTargetVertexShaderPairs(g_lastDrawStates);
    std::fprintf(
        file,
        "\n[Target VS vs_A90F5468C507A2DA pixel-shader pairs]\n");
    if (targetPairs.empty())
    {
        std::fprintf(file, "Target VS was not used by any captured draw.\n");
    }
    else
    {
        std::fprintf(
            file,
            "Matching VS/PS bytecode is auto-dumped to ZachFix\\shaders\\dump.\n");
        for (const auto& [pixelShaderHash, drawCount] : targetPairs)
        {
            std::fprintf(
                file,
                "vs_A90F5468C507A2DA -> ps_%016llX  draws=%llu\n",
                static_cast<unsigned long long>(pixelShaderHash),
                drawCount);
        }
    }

    std::fprintf(file, "\n[Target post-process draw snapshots]\n");
    std::fprintf(file, "Captured target draws: %zu (capped at %zu)\n\n",
                 g_lastTargetDrawSnapshots.size(), kMaxTargetDrawSnapshots);
    for (size_t i = 0; i < g_lastTargetDrawSnapshots.size(); ++i)
        WriteTargetDrawSnapshot(file, i, g_lastTargetDrawSnapshots[i]);

    std::fclose(file);
    AppendLog("[ShaderProbe] Captured one game frame -> ZachFix\\shaders\\last_frame.txt.\n");
}
} // namespace

void RegisterShaderProbeVertexShader(IDirect3DVertexShader9* shader)
{
    RegisterShader(shader, g_vertexShaderHashes, g_vertexShaderBlobs, g_registeredVertexShaders);
}

void RegisterShaderProbePixelShader(IDirect3DPixelShader9* shader)
{
    RegisterShader(shader, g_pixelShaderHashes, g_pixelShaderBlobs, g_registeredPixelShaders);
}

void NotifyShaderProbeVertexShaderBound(IDirect3DVertexShader9* shader)
{
    TrackShaderBind(
        shader,
        ShaderKind::Vertex,
        g_vertexShaderHashes,
        g_gameVertexShaderHashes,
        g_currentVertexShaderHash);
}

void NotifyShaderProbePixelShaderBound(IDirect3DPixelShader9* shader)
{
    TrackShaderBind(
        shader,
        ShaderKind::Pixel,
        g_pixelShaderHashes,
        g_gamePixelShaderHashes,
        g_currentPixelShaderHash);
}

void NotifyShaderProbeDraw(IDirect3DDevice9* device, const char* drawKind)
{
    if (device == nullptr)
        return;

    const std::uint64_t vertexShaderHash =
        g_currentVertexShaderHash.load(std::memory_order_acquire);
    const std::uint64_t pixelShaderHash =
        g_currentPixelShaderHash.load(std::memory_order_acquire);

    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_captureActive)
        return;

    if (g_captureDrawCount == 0)
    {
        D3DCAPS9 caps = {};
        if (SUCCEEDED(device->GetDeviceCaps(&caps)))
            g_captureMaxSimultaneousRenderTargets = caps.NumSimultaneousRTs;
    }

    ++g_captureDrawCount;
    AggregateDrawState(
        CaptureDrawStateSignature(device, vertexShaderHash, pixelShaderHash));

    if (IsTargetPostProcessShader(pixelShaderHash) &&
        g_captureTargetDrawSnapshots.size() < kMaxTargetDrawSnapshots)
    {
        g_captureTargetDrawSnapshots.push_back(
            CaptureTargetDrawSnapshot(
                device,
                drawKind,
                pixelShaderHash));
    }
}

void AdvanceShaderProbeFrame()
{
    std::lock_guard<std::mutex> lock(g_mutex);

    if (g_captureActive)
    {
        // Present is reached after EndScene in the normal game path. Defer GPU
        // readback until here instead of stalling/reading inside Draw* hooks.
        CompleteCaptureReadbacks(g_captureTargetDrawSnapshots);

        g_lastCaptureEvents = g_captureEvents;
        g_lastCaptureVertexCounts = g_captureVertexCounts;
        g_lastCapturePixelCounts = g_capturePixelCounts;
        g_lastTargetDrawSnapshots = g_captureTargetDrawSnapshots;
        g_lastDrawStates = g_captureDrawStates;
        g_lastCaptureDrawCount = g_captureDrawCount;
        g_lastMaxSimultaneousRenderTargets = g_captureMaxSimultaneousRenderTargets;

        const auto targetPairs = CollectTargetVertexShaderPairs(g_lastDrawStates);
        g_lastTargetVertexShaderPairCount = static_cast<UINT>(targetPairs.size());
        DumpTargetVertexShaderPairBlobsLocked(targetPairs);

        g_captureActive = false;
        g_captureAvailable = true;
        WriteCaptureReportLocked();
    }

    if (g_captureRequested)
    {
        g_captureEvents.clear();
        g_captureVertexCounts.clear();
        g_capturePixelCounts.clear();
        g_captureTargetDrawSnapshots.clear();
        g_captureDrawStates.clear();
        g_captureDrawCount = 0;
        g_captureMaxSimultaneousRenderTargets = 0;
        g_captureResourceIds.clear();
        g_nextCaptureResourceId = 1;
        g_captureRequested = false;
        g_captureActive = true;
        AppendLog("[ShaderProbe] Frame capture armed.\n");
    }
}

ShaderProbeStats GetShaderProbeStats()
{
    ShaderProbeStats stats{};
    stats.registeredVertexShaders = g_registeredVertexShaders.load(std::memory_order_acquire);
    stats.registeredPixelShaders = g_registeredPixelShaders.load(std::memory_order_acquire);
    stats.gameShaderBinds = g_gameShaderBinds.load(std::memory_order_acquire);
    stats.unknownGameShaderBinds = g_unknownGameShaderBinds.load(std::memory_order_acquire);
    stats.dumpSuccesses = g_dumpSuccesses.load(std::memory_order_acquire);
    stats.dumpFailures = g_dumpFailures.load(std::memory_order_acquire);
    stats.currentVertexShaderHash = g_currentVertexShaderHash.load(std::memory_order_acquire);
    stats.currentPixelShaderHash = g_currentPixelShaderHash.load(std::memory_order_acquire);

    std::lock_guard<std::mutex> lock(g_mutex);
    stats.gameVertexShaders = g_gameVertexShaderHashes.size();
    stats.gamePixelShaders = g_gamePixelShaderHashes.size();
    stats.captureRequested = g_captureRequested;
    stats.captureActive = g_captureActive;
    stats.captureAvailable = g_captureAvailable;
    stats.capturedEvents = g_lastCaptureEvents.size();
    stats.capturedUniqueVertexShaders = static_cast<UINT>(g_lastCaptureVertexCounts.size());
    stats.capturedUniquePixelShaders = static_cast<UINT>(g_lastCapturePixelCounts.size());
    stats.capturedTargetDraws = static_cast<UINT>(g_lastTargetDrawSnapshots.size());
    stats.capturedDraws = g_lastCaptureDrawCount;
    stats.capturedDrawSignatures = static_cast<UINT>(g_lastDrawStates.size());
    stats.capturedTargetVertexShaderPairs = g_lastTargetVertexShaderPairCount;
    return stats;
}

bool DumpShaderProbeShaders()
{
    std::lock_guard<std::mutex> lock(g_mutex);

    if (!EnsureShaderDirectories())
    {
        AppendLog("[ShaderProbe] WARNING: Could not create shader dump directory.\n");
        return false;
    }

    unsigned long long successes = 0;
    unsigned long long failures = 0;

    auto dump = [&successes, &failures](
        ShaderKind kind,
        const std::unordered_set<std::uint64_t>& observed,
        const std::unordered_map<std::uint64_t, ShaderBlob>& blobs)
    {
        for (const std::uint64_t hash : observed)
        {
            const auto found = blobs.find(hash);
            if (found == blobs.end())
            {
                ++failures;
                continue;
            }

            wchar_t path[MAX_PATH] = {};
            if (!BuildShaderPath(path, MAX_PATH, kind, hash, L"bin") ||
                !WriteBlob(path, found->second.bytes))
            {
                ++failures;
                continue;
            }

            ++successes;
        }
    };

    dump(ShaderKind::Vertex, g_gameVertexShaderHashes, g_vertexShaderBlobs);
    dump(ShaderKind::Pixel, g_gamePixelShaderHashes, g_pixelShaderBlobs);

    g_dumpSuccesses.fetch_add(successes, std::memory_order_relaxed);
    g_dumpFailures.fetch_add(failures, std::memory_order_relaxed);

    char text[256] = {};
    sprintf_s(
        text,
        "[ShaderProbe] Shader dump complete: %llu written, %llu failed.\n",
        successes,
        failures);
    AppendLog(text);
    return failures == 0 && successes != 0;
}

bool RequestShaderProbeFrameCapture()
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_captureRequested || g_captureActive)
        return false;

    g_captureRequested = true;
    return true;
}

ShaderProbeCompositeDebugMode GetShaderProbeCompositeDebugMode()
{
    return static_cast<ShaderProbeCompositeDebugMode>(
        g_compositeDebugMode.load(std::memory_order_acquire));
}

void SetShaderProbeCompositeDebugMode(ShaderProbeCompositeDebugMode mode)
{
    UINT value = static_cast<UINT>(mode);
    if (value > static_cast<UINT>(ShaderProbeCompositeDebugMode::NormalValidity))
        value = static_cast<UINT>(ShaderProbeCompositeDebugMode::Vanilla);

    g_compositeDebugMode.store(value, std::memory_order_release);
}

bool IsShaderProbeFinalCompositeShader(IDirect3DPixelShader9* shader)
{
    if (shader == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(g_mutex);
    const auto found = g_pixelShaderHashes.find(shader);
    return found != g_pixelShaderHashes.end() &&
           found->second == kFinalCompositeShaderHash;
}

float GetShaderProbeBloomMultiplier()
{
    return g_researchBloomMultiplier.load(std::memory_order_acquire);
}

void SetShaderProbeBloomMultiplier(float multiplier)
{
    if (!std::isfinite(multiplier))
        multiplier = 1.0f;

    multiplier = (std::max)(0.0f, (std::min)(multiplier, 2.0f));
    g_researchBloomMultiplier.store(multiplier, std::memory_order_release);
}

float GetShaderProbeExposureMultiplier()
{
    return g_researchExposureMultiplier.load(std::memory_order_acquire);
}

void SetShaderProbeExposureMultiplier(float multiplier)
{
    if (!std::isfinite(multiplier))
        multiplier = 1.0f;

    multiplier = (std::max)(0.0f, (std::min)(multiplier, 2.0f));
    g_researchExposureMultiplier.store(multiplier, std::memory_order_release);
}

bool BeginShaderProbeBloomOverride(
    IDirect3DDevice9* device,
    float previousConstant[4])
{
    if (device == nullptr || previousConstant == nullptr)
        return false;

    if (g_currentPixelShaderHash.load(std::memory_order_acquire) !=
        kFinalCompositeShaderHash)
    {
        return false;
    }

    const float multiplier = GetShaderProbeBloomMultiplier();
    if (std::fabs(multiplier - 1.0f) < 0.0001f)
        return false;

    if (FAILED(device->GetPixelShaderConstantF(9, previousConstant, 1)))
        return false;

    float replacement[4] =
    {
        previousConstant[0] * multiplier,
        previousConstant[1],
        previousConstant[2],
        previousConstant[3]
    };

    return SUCCEEDED(device->SetPixelShaderConstantF(9, replacement, 1));
}

void EndShaderProbeBloomOverride(
    IDirect3DDevice9* device,
    const float previousConstant[4])
{
    if (device == nullptr || previousConstant == nullptr)
        return;

    device->SetPixelShaderConstantF(9, previousConstant, 1);
}

bool BeginShaderProbeExposureOverride(
    IDirect3DDevice9* device,
    float previousConstant[4])
{
    if (device == nullptr || previousConstant == nullptr)
        return false;

    if (g_currentPixelShaderHash.load(std::memory_order_acquire) !=
        kFinalCompositeShaderHash)
    {
        return false;
    }

    const float multiplier = GetShaderProbeExposureMultiplier();
    if (std::fabs(multiplier - 1.0f) < 0.0001f)
        return false;

    if (FAILED(device->GetPixelShaderConstantF(10, previousConstant, 1)))
        return false;

    float replacement[4] =
    {
        previousConstant[0] * multiplier,
        previousConstant[1],
        previousConstant[2],
        previousConstant[3]
    };

    return SUCCEEDED(device->SetPixelShaderConstantF(10, replacement, 1));
}

void EndShaderProbeExposureOverride(
    IDirect3DDevice9* device,
    const float previousConstant[4])
{
    if (device == nullptr || previousConstant == nullptr)
        return;

    device->SetPixelShaderConstantF(10, previousConstant, 1);
}
