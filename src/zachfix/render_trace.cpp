#include "render_trace.h"

#include "logging.h"
#include "main_exe.h"
#include "shader_probe.h"

#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>
#include <intrin.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{
constexpr int kD3dxImageFileFormatTga = 2;
constexpr int kD3dxImageFileFormatDds = 4;
constexpr UINT kIsolationFanout = 8;
constexpr UINT kDetailedRangeLimit = 16;
constexpr UINT kDetailedTextureStages = 8;
constexpr UINT kDetailedPsConstantCount = 32;

using D3DXSaveSurfaceToFileWFn = HRESULT (WINAPI*)(
    const wchar_t* destinationFile,
    int destinationFormat,
    IDirect3DSurface9* sourceSurface,
    const PALETTEENTRY* sourcePalette,
    const RECT* sourceRect);

const GUID kRenderTraceTextureHashGuid =
{ 0x6cc28a31, 0x94f7, 0x47b1, { 0x88, 0x7f, 0x9e, 0x45, 0x5b, 0xd2, 0x31, 0x61 } };

struct IsolationRange
{
    unsigned long long start = 0;
    unsigned long long end = 0;
};

std::atomic_bool g_captureRequested{ false };
std::atomic_bool g_captureActive{ false };
std::atomic_uint g_captureSerial{ 0 };
std::atomic_bool g_bloomBypass{ false };
float g_savedBloomMultiplier = 1.0f;

bool g_isolationEnabled = false;
bool g_isolationArming = false;
unsigned long long g_currentFrameIndexedDraws = 0;
unsigned long long g_previousFrameIndexedDraws = 0;
IsolationRange g_parentRange{};
UINT g_childIndex = 0;
bool g_logSelectionThisFrame = false;
std::vector<unsigned long long> g_referenceFingerprints;
std::vector<unsigned long long> g_armingFrameFingerprints;
std::unordered_set<unsigned long long> g_activeFingerprintSet;

bool g_f6WasDown = false;
bool g_f7WasDown = false;
bool g_f8WasDown = false;
bool g_f9WasDown = false;
wchar_t g_captureDirectory[MAX_PATH] = {};
bool g_captureLogicalSaved = false;
bool g_captureBoundSaved = false;
thread_local unsigned g_internalCaptureDepth = 0;

// Reuse the proven attribution path from the old LOD profiler. It remains
// research-only and is enabled only while F8 fingerprint isolation is active.
// Steam RVAs are confirmed; GOG falls back to fingerprint-only diagnostics.
constexpr uintptr_t kSteamGetRenderObjectRva = 0x002E1150;
constexpr uintptr_t kSteamRenderSceneObjectRva = 0x002D69E0;

using GetRenderObjectFn = void* (__thiscall*)(void* owner);
using RenderSceneObjectFn = void (__thiscall*)(
    void* renderer,
    void* sceneObject,
    const short* visibleSubmeshList,
    int passMode);
GetRenderObjectFn g_originalGetRenderObject = nullptr;
RenderSceneObjectFn g_originalRenderSceneObject = nullptr;
void* g_getRenderObjectTarget = nullptr;
void* g_renderSceneObjectTarget = nullptr;
bool g_ownerHooksPrepared = false;
bool g_ownerHooksEnabled = false;

thread_local uintptr_t g_currentSceneObject = 0;
thread_local uintptr_t g_currentSceneOwner = 0;
thread_local UINT g_currentSceneObjectCallerRva = 0;
thread_local int g_currentScenePassMode = -1;
std::unordered_map<uintptr_t, uintptr_t> g_sceneObjectOwners;

std::atomic_ullong g_renderSceneObjectHookCalls{ 0 };
std::atomic_ullong g_getRenderObjectHookCalls{ 0 };
std::atomic_ullong g_sceneOwnerInsertionHits{ 0 };
std::atomic_uint g_lastRenderSceneObjectCallerRva{ 0 };
std::atomic_int g_lastRenderScenePassMode{ -1 };

bool g_ownerLockEnabled = false;
uintptr_t g_lockedOwner = 0;
std::unordered_set<uintptr_t> g_currentSelectedOwners;
std::unordered_set<uintptr_t> g_previousSelectedOwners;
UINT g_ownerLockSummaryBudget = 0;

struct ScopedRenderTraceInternalCapture
{
    ScopedRenderTraceInternalCapture() { ++g_internalCaptureDepth; }
    ~ScopedRenderTraceInternalCapture() { --g_internalCaptureDepth; }
};

UINT MainExeAddressRva(const void* rawAddress)
{
    if (!g_mainExeInfoValid.load(std::memory_order_acquire) &&
        !InitializeMainExeInfo())
    {
        return 0;
    }

    const uintptr_t address = reinterpret_cast<uintptr_t>(rawAddress);
    if (address < g_mainExeBase || address >= g_mainExeBase + g_mainExeSize)
        return 0;
    return static_cast<UINT>(address - g_mainExeBase);
}

void* __fastcall HookGetRenderObject(void* self, void*)
{
    g_getRenderObjectHookCalls.fetch_add(1, std::memory_order_relaxed);
    const UINT callerRva = MainExeAddressRva(_ReturnAddress());
    void* result = g_originalGetRenderObject != nullptr
        ? g_originalGetRenderObject(self)
        : nullptr;

    if (result == nullptr || self == nullptr)
        return result;

    // These are the main-scene insertion call sites immediately before the
    // render object is passed to renderer+0x63A8. They were validated by the
    // old LOD profiler and are intentionally narrower than all GetRenderObject
    // callers.
    if (callerRva == 0x002D3001 ||
        callerRva == 0x002D3237 ||
        callerRva == 0x002D3246)
    {
        g_sceneObjectOwners[reinterpret_cast<uintptr_t>(result)] =
            reinterpret_cast<uintptr_t>(self);
        g_sceneOwnerInsertionHits.fetch_add(1, std::memory_order_relaxed);
    }

    return result;
}

void __fastcall HookRenderSceneObject(
    void* self,
    void*,
    void* sceneObject,
    const short* visibleSubmeshList,
    int passMode)
{
    g_renderSceneObjectHookCalls.fetch_add(1, std::memory_order_relaxed);
    const UINT currentCallerRva = MainExeAddressRva(_ReturnAddress());
    g_lastRenderSceneObjectCallerRva.store(currentCallerRva, std::memory_order_relaxed);
    g_lastRenderScenePassMode.store(passMode, std::memory_order_relaxed);

    const uintptr_t previousObject = g_currentSceneObject;
    const uintptr_t previousOwner = g_currentSceneOwner;
    const UINT previousCallerRva = g_currentSceneObjectCallerRva;
    const int previousPassMode = g_currentScenePassMode;

    g_currentSceneObject = reinterpret_cast<uintptr_t>(sceneObject);
    g_currentSceneOwner = 0;
    g_currentSceneObjectCallerRva = currentCallerRva;
    g_currentScenePassMode = passMode;

    const auto ownerIt = g_sceneObjectOwners.find(g_currentSceneObject);
    if (ownerIt != g_sceneObjectOwners.end())
        g_currentSceneOwner = ownerIt->second;

    if (g_originalRenderSceneObject != nullptr)
    {
        g_originalRenderSceneObject(
            self,
            sceneObject,
            visibleSubmeshList,
            passMode);
    }

    g_currentSceneObject = previousObject;
    g_currentSceneOwner = previousOwner;
    g_currentSceneObjectCallerRva = previousCallerRva;
    g_currentScenePassMode = previousPassMode;
}

bool PrepareOwnerAttributionHooksImpl();

bool SetOwnerAttributionHooksEnabled(bool enabled)
{
    if (enabled && !g_ownerHooksPrepared && !PrepareOwnerAttributionHooksImpl())
        return false;
    if (!enabled && !g_ownerHooksPrepared)
        return true;

    if (enabled == g_ownerHooksEnabled)
        return true;

    if (enabled)
    {
        const MH_STATUS ownerStatus = MH_EnableHook(g_getRenderObjectTarget);
        if (ownerStatus != MH_OK && ownerStatus != MH_ERROR_ENABLED)
        {
            AppendLog("[RenderTrace][Owner] GetRenderObject hook enable failed; fingerprint-only isolation remains active.\n");
            return false;
        }

        const MH_STATUS objectStatus = MH_EnableHook(g_renderSceneObjectTarget);
        if (objectStatus != MH_OK && objectStatus != MH_ERROR_ENABLED)
        {
            MH_DisableHook(g_getRenderObjectTarget);
            AppendLog("[RenderTrace][Owner] RenderSceneObject hook enable failed; fingerprint-only isolation remains active.\n");
            return false;
        }

        g_ownerHooksEnabled = true;
        g_sceneObjectOwners.clear();
        g_currentSceneObject = 0;
        g_currentSceneOwner = 0;
        g_currentSceneObjectCallerRva = 0;
        g_currentScenePassMode = -1;
        g_renderSceneObjectHookCalls.store(0, std::memory_order_relaxed);
        g_getRenderObjectHookCalls.store(0, std::memory_order_relaxed);
        g_sceneOwnerInsertionHits.store(0, std::memory_order_relaxed);
        g_lastRenderSceneObjectCallerRva.store(0, std::memory_order_relaxed);
        g_lastRenderScenePassMode.store(-1, std::memory_order_relaxed);
        AppendLog("[RenderTrace][Owner] Steam scene attribution enabled for F8 isolation.\n");
        return true;
    }

    MH_DisableHook(g_renderSceneObjectTarget);
    MH_DisableHook(g_getRenderObjectTarget);
    g_ownerHooksEnabled = false;
    g_sceneObjectOwners.clear();
    g_currentSceneObject = 0;
    g_currentSceneOwner = 0;
    g_currentSceneObjectCallerRva = 0;
    g_currentScenePassMode = -1;
    AppendLog("[RenderTrace][Owner] Steam scene attribution disabled.\n");
    return true;
}

bool PrepareOwnerAttributionHooksImpl()
{
    if (g_ownerHooksPrepared)
        return true;

    if (!g_mainExeInfoValid.load(std::memory_order_acquire) &&
        !InitializeMainExeInfo())
    {
        AppendLog("[RenderTrace][Owner] attribution unavailable: DP.exe info initialization failed; fingerprint isolation still works.\n");
        return false;
    }

    const DpBuildProfile* profile = GetDpBuildProfile();
    if (profile == nullptr || profile->build != DpBuild::Steam101b)
    {
        AppendLog("[RenderTrace][Owner] attribution unavailable on this build; fingerprint isolation still works.\n");
        return false;
    }

    auto* getRenderObject = reinterpret_cast<unsigned char*>(
        g_mainExeBase + kSteamGetRenderObjectRva);
    auto* renderSceneObject = reinterpret_cast<unsigned char*>(
        g_mainExeBase + kSteamRenderSceneObjectRva);

    static const unsigned char getRenderObjectPrologue[] =
        { 0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x0C, 0x89, 0x4D, 0xFC };
    static const unsigned char renderSceneObjectPrologue[] =
        { 0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0x0D, 0x8C, 0x76, 0x00 };

    if (memcmp(getRenderObject, getRenderObjectPrologue,
            sizeof(getRenderObjectPrologue)) != 0 ||
        memcmp(renderSceneObject, renderSceneObjectPrologue,
            sizeof(renderSceneObjectPrologue)) != 0)
    {
        AppendLog("[RenderTrace][Owner] attribution signature mismatch; fingerprint isolation still works.\n");
        return false;
    }

    MH_STATUS status = MH_CreateHook(
        getRenderObject,
        reinterpret_cast<void*>(&HookGetRenderObject),
        reinterpret_cast<void**>(&g_originalGetRenderObject));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        AppendLog("[RenderTrace][Owner] GetRenderObject MH_CreateHook failed.\n");
        return false;
    }

    status = MH_CreateHook(
        renderSceneObject,
        reinterpret_cast<void*>(&HookRenderSceneObject),
        reinterpret_cast<void**>(&g_originalRenderSceneObject));
    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        MH_RemoveHook(getRenderObject);
        AppendLog("[RenderTrace][Owner] RenderSceneObject MH_CreateHook failed.\n");
        return false;
    }

    g_getRenderObjectTarget = getRenderObject;
    g_renderSceneObjectTarget = renderSceneObject;
    g_ownerHooksPrepared = true;
    AppendLog("[RenderTrace][Owner] Steam attribution probes prepared at DP.exe+0x002E1150 / +0x002D69E0; enabled only during F8 isolation.\n");
    return true;
}

D3DXSaveSurfaceToFileWFn ResolveSaveSurface()
{
    static D3DXSaveSurfaceToFileWFn fn = nullptr;
    static bool attempted = false;
    if (attempted)
        return fn;

    attempted = true;
    HMODULE d3dx = GetModuleHandleW(L"d3dx9_43.dll");
    if (d3dx == nullptr)
        d3dx = LoadLibraryW(L"d3dx9_43.dll");
    if (d3dx != nullptr)
    {
        fn = reinterpret_cast<D3DXSaveSurfaceToFileWFn>(
            GetProcAddress(d3dx, "D3DXSaveSurfaceToFileW"));
    }
    return fn;
}

bool EnsureDirectory(const wchar_t* path)
{
    if (path == nullptr || path[0] == L'\0')
        return false;
    if (CreateDirectoryW(path, nullptr) != FALSE)
        return true;
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

bool BuildCaptureDirectory()
{
    wchar_t root[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, root, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return false;

    wchar_t* slash = wcsrchr(root, L'\\');
    if (slash == nullptr)
        return false;
    *slash = L'\0';

    wchar_t zachFixDir[MAX_PATH] = {};
    wchar_t traceDir[MAX_PATH] = {};
    if (swprintf_s(zachFixDir, L"%ls\\ZachFix", root) < 0 ||
        !EnsureDirectory(zachFixDir))
    {
        return false;
    }
    if (swprintf_s(traceDir, L"%ls\\render_trace", zachFixDir) < 0 ||
        !EnsureDirectory(traceDir))
    {
        return false;
    }

    const UINT serial = g_captureSerial.fetch_add(1, std::memory_order_relaxed) + 1;
    if (swprintf_s(
            g_captureDirectory,
            L"%ls\\capture_%04u",
            traceDir,
            serial) < 0)
    {
        g_captureDirectory[0] = L'\0';
        return false;
    }

    return EnsureDirectory(g_captureDirectory);
}

bool IsKeyPressedEdge(int vk, bool& wasDown)
{
    const bool down = (GetAsyncKeyState(vk) & 0x8000) != 0;
    const bool pressed = down && !wasDown;
    wasDown = down;
    return pressed;
}

UINT GetTextureSourceHash(IDirect3DBaseTexture9* texture)
{
    if (texture == nullptr)
        return 0;

    UINT hash = 0;
    DWORD size = sizeof(hash);
    if (FAILED(texture->GetPrivateData(kRenderTraceTextureHashGuid, &hash, &size)) ||
        size != sizeof(hash))
    {
        return 0;
    }
    return hash;
}

void MixFingerprintValue(unsigned long long& hash, unsigned long long value)
{
    constexpr unsigned long long kFnvPrime = 1099511628211ull;
    for (unsigned i = 0; i < 8; ++i)
    {
        hash ^= static_cast<unsigned char>((value >> (i * 8)) & 0xFFu);
        hash *= kFnvPrime;
    }
}

unsigned long long BuildDrawFingerprint(
    IDirect3DDevice9* device,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount)
{
    constexpr unsigned long long kFnvOffset = 1469598103934665603ull;
    unsigned long long hash = kFnvOffset;

    const ShaderProbeStats shaderStats = GetShaderProbeStats();
    MixFingerprintValue(hash, static_cast<unsigned long long>(primitiveType));
    MixFingerprintValue(hash, static_cast<unsigned long long>(static_cast<unsigned int>(baseVertexIndex)));
    MixFingerprintValue(hash, minVertexIndex);
    MixFingerprintValue(hash, numVertices);
    MixFingerprintValue(hash, startIndex);
    MixFingerprintValue(hash, primitiveCount);
    MixFingerprintValue(hash, shaderStats.currentVertexShaderHash);
    MixFingerprintValue(hash, shaderStats.currentPixelShaderHash);

    if (device == nullptr)
        return hash;

    IDirect3DVertexBuffer9* vertexBuffer = nullptr;
    UINT streamOffset = 0;
    UINT streamStride = 0;
    if (SUCCEEDED(device->GetStreamSource(0, &vertexBuffer, &streamOffset, &streamStride)) &&
        vertexBuffer != nullptr)
    {
        MixFingerprintValue(hash, reinterpret_cast<uintptr_t>(vertexBuffer));
        MixFingerprintValue(hash, streamOffset);
        MixFingerprintValue(hash, streamStride);
        vertexBuffer->Release();
    }

    IDirect3DIndexBuffer9* indexBuffer = nullptr;
    if (SUCCEEDED(device->GetIndices(&indexBuffer)) && indexBuffer != nullptr)
    {
        MixFingerprintValue(hash, reinterpret_cast<uintptr_t>(indexBuffer));
        indexBuffer->Release();
    }

    IDirect3DBaseTexture9* texture0 = nullptr;
    if (SUCCEEDED(device->GetTexture(0, &texture0)) && texture0 != nullptr)
    {
        MixFingerprintValue(hash, reinterpret_cast<uintptr_t>(texture0));
        MixFingerprintValue(hash, GetTextureSourceHash(texture0));
        texture0->Release();
    }

    return hash;
}

IsolationRange GetActiveRange()
{
    IsolationRange active = g_parentRange;
    if (!g_isolationEnabled || active.end <= active.start)
        return active;

    const unsigned long long size = active.end - active.start;
    const UINT bins = static_cast<UINT>((std::min<unsigned long long>)(kIsolationFanout, size));
    if (bins <= 1)
        return active;

    const UINT child = (std::min)(g_childIndex, bins - 1);
    active.start = g_parentRange.start + (size * child) / bins;
    active.end = g_parentRange.start + (size * (child + 1)) / bins;
    if (active.end <= active.start)
        active.end = (std::min)(g_parentRange.end, active.start + 1);
    return active;
}

void LogIsolationState(const char* reason)
{
    const IsolationRange active = GetActiveRange();
    const unsigned long long parentSize =
        g_parentRange.end > g_parentRange.start
            ? g_parentRange.end - g_parentRange.start
            : 0;
    const unsigned long long activeSize =
        active.end > active.start ? active.end - active.start : 0;
    const UINT bins = parentSize == 0
        ? 0
        : static_cast<UINT>((std::min<unsigned long long>)(kIsolationFanout, parentSize));

    char text[384] = {};
    sprintf_s(
        text,
        "[RenderTrace][Isolation] %s enabled=%s arming=%s liveDIPs=%llu reference=%llu parent=[%llu,%llu) child=%u/%u active=[%llu,%llu) count=%llu.\n",
        reason != nullptr ? reason : "state",
        g_isolationEnabled ? "yes" : "no",
        g_isolationArming ? "yes" : "no",
        g_previousFrameIndexedDraws,
        static_cast<unsigned long long>(g_referenceFingerprints.size()),
        g_parentRange.start,
        g_parentRange.end,
        bins == 0 ? 0 : g_childIndex + 1,
        bins,
        active.start,
        active.end,
        activeSize);
    AppendLog(text);
}

void RebuildActiveFingerprintSet()
{
    g_activeFingerprintSet.clear();
    if (!g_isolationEnabled || g_referenceFingerprints.empty())
        return;

    const IsolationRange active = GetActiveRange();
    const unsigned long long end =
        (std::min<unsigned long long>)(active.end, g_referenceFingerprints.size());
    for (unsigned long long i = active.start; i < end; ++i)
        g_activeFingerprintSet.insert(g_referenceFingerprints[static_cast<size_t>(i)]);
}

void ResetIsolationTree()
{
    g_parentRange.start = 0;
    g_parentRange.end = g_referenceFingerprints.size();
    g_childIndex = 0;
    RebuildActiveFingerprintSet();
    g_logSelectionThisFrame = true;
}

void RefineIsolationTree()
{
    const IsolationRange active = GetActiveRange();
    if (active.end <= active.start + 1)
    {
        LogIsolationState("already-single-draw");
        return;
    }
    g_parentRange = active;
    g_childIndex = 0;
    RebuildActiveFingerprintSet();
    g_logSelectionThisFrame = true;
    LogIsolationState("refined");
}

void StepIsolationChild(int direction)
{
    if (!g_isolationEnabled || g_parentRange.end <= g_parentRange.start)
        return;

    const unsigned long long size = g_parentRange.end - g_parentRange.start;
    const UINT bins = static_cast<UINT>((std::min<unsigned long long>)(kIsolationFanout, size));
    if (bins == 0)
        return;

    int next = static_cast<int>(g_childIndex) + direction;
    while (next < 0)
        next += static_cast<int>(bins);
    next %= static_cast<int>(bins);
    g_childIndex = static_cast<UINT>(next);
    RebuildActiveFingerprintSet();
    g_logSelectionThisFrame = true;
    LogIsolationState(direction >= 0 ? "next" : "previous");
}

void LogRenderStates(IDirect3DDevice9* device, unsigned long long drawIndex)
{
    if (device == nullptr)
        return;

    const std::array<D3DRENDERSTATETYPE, 11> states =
    {
        D3DRS_ALPHABLENDENABLE,
        D3DRS_SRCBLEND,
        D3DRS_DESTBLEND,
        D3DRS_BLENDOP,
        D3DRS_ALPHATESTENABLE,
        D3DRS_ALPHAREF,
        D3DRS_ALPHAFUNC,
        D3DRS_ZENABLE,
        D3DRS_ZWRITEENABLE,
        D3DRS_CULLMODE,
        D3DRS_COLORWRITEENABLE
    };

    std::array<DWORD, 11> values{};
    for (size_t i = 0; i < states.size(); ++i)
        device->GetRenderState(states[i], &values[i]);

    char text[768] = {};
    sprintf_s(
        text,
        "[RenderTrace][State] draw=%llu alphaBlend=%u src=%u dst=%u blendOp=%u alphaTest=%u alphaRef=%u alphaFunc=%u zEnable=%u zWrite=%u cull=%u colorWrite=0x%08X.\n",
        drawIndex,
        values[0], values[1], values[2], values[3], values[4], values[5],
        values[6], values[7], values[8], values[9], values[10]);
    AppendLog(text);
}

void LogTextures(IDirect3DDevice9* device, unsigned long long drawIndex)
{
    if (device == nullptr)
        return;

    for (UINT stage = 0; stage < kDetailedTextureStages; ++stage)
    {
        IDirect3DBaseTexture9* base = nullptr;
        const HRESULT getResult = device->GetTexture(stage, &base);
        if (FAILED(getResult) || base == nullptr)
        {
            char text[192] = {};
            sprintf_s(text,
                "[RenderTrace][Texture] draw=%llu s%u=<null> hr=0x%08X.\n",
                drawIndex,
                stage,
                static_cast<unsigned>(getResult));
            AppendLog(text);
            if (base != nullptr)
                base->Release();
            continue;
        }

        const UINT hash = GetTextureSourceHash(base);
        IDirect3DTexture9* texture2d =
            base->GetType() == D3DRTYPE_TEXTURE
                ? reinterpret_cast<IDirect3DTexture9*>(base)
                : nullptr;
        if (texture2d != nullptr)
        {
            D3DSURFACE_DESC desc{};
            const HRESULT descResult = texture2d->GetLevelDesc(0, &desc);
            char text[320] = {};
            sprintf_s(
                text,
                "[RenderTrace][Texture] draw=%llu s%u=%p hash=%08X type=2D descHr=0x%08X %ux%u fmt=%u usage=0x%08X pool=%u.\n",
                drawIndex,
                stage,
                base,
                hash,
                static_cast<unsigned>(descResult),
                SUCCEEDED(descResult) ? desc.Width : 0,
                SUCCEEDED(descResult) ? desc.Height : 0,
                SUCCEEDED(descResult) ? static_cast<unsigned>(desc.Format) : 0,
                SUCCEEDED(descResult) ? static_cast<unsigned>(desc.Usage) : 0,
                SUCCEEDED(descResult) ? static_cast<unsigned>(desc.Pool) : 0);
            AppendLog(text);
        }
        else
        {
            char text[224] = {};
            sprintf_s(
                text,
                "[RenderTrace][Texture] draw=%llu s%u=%p hash=%08X type=%u.\n",
                drawIndex,
                stage,
                base,
                hash,
                static_cast<unsigned>(base->GetType()));
            AppendLog(text);
        }
        base->Release();
    }
}

void LogPixelShaderConstants(IDirect3DDevice9* device, unsigned long long drawIndex)
{
    if (device == nullptr)
        return;

    std::array<float, kDetailedPsConstantCount * 4> constants{};
    const HRESULT result = device->GetPixelShaderConstantF(
        0,
        constants.data(),
        kDetailedPsConstantCount);
    if (FAILED(result))
    {
        char text[192] = {};
        sprintf_s(text,
            "[RenderTrace][PSConst] draw=%llu read failed hr=0x%08X.\n",
            drawIndex,
            static_cast<unsigned>(result));
        AppendLog(text);
        return;
    }

    for (UINT reg = 0; reg < kDetailedPsConstantCount; ++reg)
    {
        const float* v = constants.data() + reg * 4;
        const bool interesting =
            !std::isfinite(v[0]) || !std::isfinite(v[1]) ||
            !std::isfinite(v[2]) || !std::isfinite(v[3]) ||
            std::fabs(v[0]) > 1.0e-7f || std::fabs(v[1]) > 1.0e-7f ||
            std::fabs(v[2]) > 1.0e-7f || std::fabs(v[3]) > 1.0e-7f;
        if (!interesting)
            continue;

        char text[256] = {};
        sprintf_s(
            text,
            "[RenderTrace][PSConst] draw=%llu c%u={%.9g,%.9g,%.9g,%.9g}.\n",
            drawIndex,
            reg,
            v[0], v[1], v[2], v[3]);
        AppendLog(text);
    }
}

void LogDetailedIndexedDraw(
    IDirect3DDevice9* device,
    unsigned long long drawIndex,
    unsigned long long fingerprint,
    unsigned long long referenceSlot,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount)
{
    if (device == nullptr)
        return;

    const ShaderProbeStats shaderStats = GetShaderProbeStats();
    IDirect3DVertexShader9* vs = nullptr;
    IDirect3DPixelShader9* ps = nullptr;
    device->GetVertexShader(&vs);
    device->GetPixelShader(&ps);

    D3DVIEWPORT9 viewport{};
    const HRESULT viewportResult = device->GetViewport(&viewport);
    IDirect3DSurface9* rt0 = nullptr;
    D3DSURFACE_DESC rtDesc{};
    HRESULT rtDescResult = E_FAIL;
    if (SUCCEEDED(device->GetRenderTarget(0, &rt0)) && rt0 != nullptr)
        rtDescResult = rt0->GetDesc(&rtDesc);

    char text[768] = {};
    sprintf_s(
        text,
        "[RenderTrace][SelectedDraw] liveIndex=%llu refSlot=%llu key=%016llX owner=%p obj=%p objCaller=DP.exe+0x%08X primType=%u base=%d min=%u verts=%u start=%u prims=%u VS=%p/%016llX PS=%p/%016llX viewportHr=0x%08X viewport=%u,%u %ux%u RT0=%p descHr=0x%08X %ux%u fmt=%u.\n",
        drawIndex,
        referenceSlot,
        fingerprint,
        reinterpret_cast<void*>(g_currentSceneOwner),
        reinterpret_cast<void*>(g_currentSceneObject),
        g_currentSceneObjectCallerRva,
        static_cast<unsigned>(primitiveType),
        baseVertexIndex,
        minVertexIndex,
        numVertices,
        startIndex,
        primitiveCount,
        vs,
        static_cast<unsigned long long>(shaderStats.currentVertexShaderHash),
        ps,
        static_cast<unsigned long long>(shaderStats.currentPixelShaderHash),
        static_cast<unsigned>(viewportResult),
        SUCCEEDED(viewportResult) ? viewport.X : 0,
        SUCCEEDED(viewportResult) ? viewport.Y : 0,
        SUCCEEDED(viewportResult) ? viewport.Width : 0,
        SUCCEEDED(viewportResult) ? viewport.Height : 0,
        rt0,
        static_cast<unsigned>(rtDescResult),
        SUCCEEDED(rtDescResult) ? rtDesc.Width : 0,
        SUCCEEDED(rtDescResult) ? rtDesc.Height : 0,
        SUCCEEDED(rtDescResult) ? static_cast<unsigned>(rtDesc.Format) : 0);
    AppendLog(text);

    if (vs != nullptr)
        vs->Release();
    if (ps != nullptr)
        ps->Release();
    if (rt0 != nullptr)
        rt0->Release();

    LogRenderStates(device, drawIndex);
    LogTextures(device, drawIndex);
    LogPixelShaderConstants(device, drawIndex);
}

void LogOwnerLockedDrawSummary(
    IDirect3DDevice9* device,
    unsigned long long drawIndex,
    unsigned long long fingerprint,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount)
{
    if (device == nullptr || g_ownerLockSummaryBudget == 0)
        return;

    --g_ownerLockSummaryBudget;
    const ShaderProbeStats shaderStats = GetShaderProbeStats();
    UINT texture0Hash = 0;
    IDirect3DBaseTexture9* texture0 = nullptr;
    if (SUCCEEDED(device->GetTexture(0, &texture0)) && texture0 != nullptr)
    {
        texture0Hash = GetTextureSourceHash(texture0);
        texture0->Release();
    }

    char text[640] = {};
    sprintf_s(
        text,
        "[RenderTrace][OwnerDraw] liveIndex=%llu key=%016llX owner=%p obj=%p objCaller=DP.exe+0x%08X primType=%u base=%d verts=%u start=%u prims=%u VS=%016llX PS=%016llX tex0=%08X.\n",
        drawIndex,
        fingerprint,
        reinterpret_cast<void*>(g_currentSceneOwner),
        reinterpret_cast<void*>(g_currentSceneObject),
        g_currentSceneObjectCallerRva,
        static_cast<unsigned>(primitiveType),
        baseVertexIndex,
        numVertices,
        startIndex,
        primitiveCount,
        static_cast<unsigned long long>(shaderStats.currentVertexShaderHash),
        static_cast<unsigned long long>(shaderStats.currentPixelShaderHash),
        texture0Hash);
    AppendLog(text);
}

bool SaveSurfaceFile(
    IDirect3DSurface9* surface,
    const wchar_t* baseName,
    bool saveDds)
{
    if (surface == nullptr || baseName == nullptr || g_captureDirectory[0] == L'\0')
        return false;

    D3DXSaveSurfaceToFileWFn saveSurface = ResolveSaveSurface();
    if (saveSurface == nullptr)
    {
        AppendLog("[RenderTrace][Capture] WARNING: D3DXSaveSurfaceToFileW unavailable.\n");
        return false;
    }

    wchar_t path[MAX_PATH] = {};
    if (swprintf_s(path, L"%ls\\%ls.tga", g_captureDirectory, baseName) < 0)
        return false;

    // D3DX may allocate temporary render targets/textures while exporting.
    // Those allocations are implementation details of the capture path, not
    // DP resources, so resolution hooks must leave their requested sizes alone.
    ScopedRenderTraceInternalCapture captureScope;

    const HRESULT tgaResult = saveSurface(
        path,
        kD3dxImageFileFormatTga,
        surface,
        nullptr,
        nullptr);

    HRESULT ddsResult = S_OK;
    if (saveDds)
    {
        if (swprintf_s(path, L"%ls\\%ls.dds", g_captureDirectory, baseName) >= 0)
        {
            ddsResult = saveSurface(
                path,
                kD3dxImageFileFormatDds,
                surface,
                nullptr,
                nullptr);
        }
    }

    char text[320] = {};
    sprintf_s(
        text,
        "[RenderTrace][Capture] %ls -> TGA hr=0x%08X%s.\n",
        baseName,
        static_cast<unsigned>(tgaResult),
        saveDds
            ? (SUCCEEDED(ddsResult) ? ", DDS=OK" : ", DDS=FAILED")
            : "");
    AppendLog(text);
    return SUCCEEDED(tgaResult) && (!saveDds || SUCCEEDED(ddsResult));
}

void SaveTextureStage(
    IDirect3DDevice9* device,
    DWORD stage,
    const wchar_t* prefix,
    bool saveDds)
{
    if (device == nullptr || prefix == nullptr)
        return;

    IDirect3DBaseTexture9* base = nullptr;
    const HRESULT getResult = device->GetTexture(stage, &base);
    if (FAILED(getResult) || base == nullptr)
        return;

    if (base->GetType() == D3DRTYPE_TEXTURE)
    {
        auto* texture = reinterpret_cast<IDirect3DTexture9*>(base);
        IDirect3DSurface9* surface = nullptr;
        if (SUCCEEDED(texture->GetSurfaceLevel(0, &surface)) && surface != nullptr)
        {
            wchar_t name[96] = {};
            swprintf_s(name, L"%ls_s%u", prefix, stage);
            SaveSurfaceFile(surface, name, saveDds);
            surface->Release();
        }
    }
    base->Release();
}

void SaveCurrentRenderTarget(IDirect3DDevice9* device, const wchar_t* name)
{
    if (device == nullptr || name == nullptr)
        return;

    IDirect3DSurface9* surface = nullptr;
    if (SUCCEEDED(device->GetRenderTarget(0, &surface)) && surface != nullptr)
    {
        SaveSurfaceFile(surface, name, false);
        surface->Release();
    }
}

void WriteCompositeState(IDirect3DDevice9* device, const wchar_t* label)
{
    if (device == nullptr || label == nullptr || g_captureDirectory[0] == L'\0')
        return;

    wchar_t path[MAX_PATH] = {};
    if (swprintf_s(path, L"%ls\\%ls_state.txt", g_captureDirectory, label) < 0)
        return;

    FILE* file = nullptr;
    if (_wfopen_s(&file, path, L"wt") != 0 || file == nullptr)
        return;

    const ShaderProbeStats stats = GetShaderProbeStats();
    std::fprintf(file,
        "VS hash=%016llX\nPS hash=%016llX\nBloom multiplier=%.6f\n",
        static_cast<unsigned long long>(stats.currentVertexShaderHash),
        static_cast<unsigned long long>(stats.currentPixelShaderHash),
        GetShaderProbeBloomMultiplier());

    for (UINT stage = 0; stage < 10; ++stage)
    {
        IDirect3DBaseTexture9* base = nullptr;
        const HRESULT getResult = device->GetTexture(stage, &base);
        if (FAILED(getResult) || base == nullptr)
        {
            std::fprintf(file, "s%u=<null> hr=0x%08X\n", stage, static_cast<unsigned>(getResult));
            if (base != nullptr)
                base->Release();
            continue;
        }

        const UINT hash = GetTextureSourceHash(base);
        IDirect3DTexture9* texture =
            base->GetType() == D3DRTYPE_TEXTURE
                ? reinterpret_cast<IDirect3DTexture9*>(base)
                : nullptr;
        if (texture != nullptr)
        {
            D3DSURFACE_DESC desc{};
            const HRESULT descResult = texture->GetLevelDesc(0, &desc);
            std::fprintf(
                file,
                "s%u=%p hash=%08X descHr=0x%08X %ux%u fmt=%u usage=0x%08X pool=%u\n",
                stage,
                base,
                hash,
                static_cast<unsigned>(descResult),
                SUCCEEDED(descResult) ? desc.Width : 0,
                SUCCEEDED(descResult) ? desc.Height : 0,
                SUCCEEDED(descResult) ? static_cast<unsigned>(desc.Format) : 0,
                SUCCEEDED(descResult) ? static_cast<unsigned>(desc.Usage) : 0,
                SUCCEEDED(descResult) ? static_cast<unsigned>(desc.Pool) : 0);
        }
        else
        {
            std::fprintf(file, "s%u=%p hash=%08X type=%u\n", stage, base, hash, static_cast<unsigned>(base->GetType()));
        }
        base->Release();
    }

    std::array<float, kDetailedPsConstantCount * 4> constants{};
    const HRESULT constantResult = device->GetPixelShaderConstantF(
        0, constants.data(), kDetailedPsConstantCount);
    std::fprintf(file, "PS constants hr=0x%08X\n", static_cast<unsigned>(constantResult));
    if (SUCCEEDED(constantResult))
    {
        for (UINT reg = 0; reg < kDetailedPsConstantCount; ++reg)
        {
            const float* v = constants.data() + reg * 4;
            std::fprintf(file, "c%02u={%.9g,%.9g,%.9g,%.9g}\n", reg, v[0], v[1], v[2], v[3]);
        }
    }
    std::fclose(file);
}

void BeginRequestedCapture()
{
    if (!g_captureRequested.exchange(false, std::memory_order_acq_rel))
        return;

    if (!BuildCaptureDirectory())
    {
        AppendLog("[RenderTrace][Capture] ERROR: could not create capture directory.\n");
        return;
    }

    g_captureLogicalSaved = false;
    g_captureBoundSaved = false;
    g_captureActive.store(true, std::memory_order_release);

    char text[512] = {};
    sprintf_s(text,
        "[RenderTrace][Capture] Armed capture directory: %ls. Waiting for final composite.\n",
        g_captureDirectory);
    AppendLog(text);
}

void PollHotkeys()
{
    const bool f6Pressed = IsKeyPressedEdge(VK_F6, g_f6WasDown);
    const bool f7Pressed = IsKeyPressedEdge(VK_F7, g_f7WasDown);
    const bool f8Pressed = IsKeyPressedEdge(VK_F8, g_f8WasDown);
    const bool f9Pressed = IsKeyPressedEdge(VK_F9, g_f9WasDown);

    if (f6Pressed)
    {
        g_captureRequested.store(true, std::memory_order_release);
        AppendLog("[RenderTrace] F6: multi-stage final-composite capture requested for next frame.\n");
    }

    if (f7Pressed)
    {
        const bool next = !g_bloomBypass.load(std::memory_order_acquire);
        if (next)
        {
            g_savedBloomMultiplier = GetShaderProbeBloomMultiplier();
            SetShaderProbeBloomMultiplier(0.0f);
        }
        else
        {
            SetShaderProbeBloomMultiplier(g_savedBloomMultiplier);
        }
        g_bloomBypass.store(next, std::memory_order_release);
        AppendLog(next
            ? "[RenderTrace] F7: final-composite bloom term BYPASSED (multiplier=0, research only).\n"
            : "[RenderTrace] F7: final-composite bloom term restored.\n");
    }

    if (f8Pressed)
    {
        if (g_isolationEnabled || g_isolationArming)
        {
            g_isolationEnabled = false;
            g_isolationArming = false;
            g_referenceFingerprints.clear();
            g_armingFrameFingerprints.clear();
            g_activeFingerprintSet.clear();
            g_parentRange = {};
            g_childIndex = 0;
            g_ownerLockEnabled = false;
            g_lockedOwner = 0;
            g_currentSelectedOwners.clear();
            g_previousSelectedOwners.clear();
            SetOwnerAttributionHooksEnabled(false);
            AppendLog("[RenderTrace][Isolation] disabled; all main-scene indexed draws submit normally.\n");
        }
        else
        {
            SetOwnerAttributionHooksEnabled(true);
            g_isolationArming = true;
            g_armingFrameFingerprints.clear();
            g_ownerLockEnabled = false;
            g_lockedOwner = 0;
            g_currentSelectedOwners.clear();
            g_previousSelectedOwners.clear();
            AppendLog("[RenderTrace][Isolation] F8: capturing one full fingerprint reference frame; isolation starts on the following frame.\n");
        }
    }

    if (f9Pressed && g_isolationEnabled)
    {
        const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool control = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        const bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;

        if (alt)
        {
            if (g_ownerLockEnabled)
            {
                g_ownerLockEnabled = false;
                g_lockedOwner = 0;
                g_logSelectionThisFrame = true;
                AppendLog("[RenderTrace][Owner] Alt+F9: owner lock OFF; returning to fingerprint selection.\n");
            }
            else if (!g_ownerHooksEnabled)
            {
                AppendLog("[RenderTrace][Owner] Alt+F9: owner attribution unavailable; fingerprint selection unchanged.\n");
            }
            else if (g_previousSelectedOwners.size() == 1)
            {
                g_lockedOwner = *g_previousSelectedOwners.begin();
                g_ownerLockEnabled = g_lockedOwner != 0;
                g_ownerLockSummaryBudget = 64;
                char text[256] = {};
                sprintf_s(text,
                    "[RenderTrace][Owner] Alt+F9: owner lock ON owner=%p; all main-scene draws attributed to this owner are now submitted.\n",
                    reinterpret_cast<void*>(g_lockedOwner));
                AppendLog(text);
            }
            else
            {
                char text[256] = {};
                sprintf_s(text,
                    "[RenderTrace][Owner] Alt+F9: need exactly one selected owner from the previous frame, observed=%zu. Narrow the fingerprint range first.\n",
                    g_previousSelectedOwners.size());
                AppendLog(text);
            }
        }
        else if (control && shift)
        {
            ResetIsolationTree();
            g_ownerLockEnabled = false;
            g_lockedOwner = 0;
            LogIsolationState("reset");
        }
        else if (control)
        {
            RefineIsolationTree();
            g_ownerLockEnabled = false;
            g_lockedOwner = 0;
        }
        else
        {
            StepIsolationChild(shift ? -1 : 1);
            g_ownerLockEnabled = false;
            g_lockedOwner = 0;
        }
    }
}
} // namespace

bool IsRenderTraceInternalCaptureCall()
{
    return g_internalCaptureDepth != 0;
}

void RegisterRenderTraceTextureSource(IDirect3DBaseTexture9* texture, UINT hash)
{
    if (texture == nullptr || hash == 0)
        return;
    texture->SetPrivateData(
        kRenderTraceTextureHashGuid,
        &hash,
        sizeof(hash),
        0);
}

void AdvanceRenderMaterialTraceFrame(IDirect3DDevice9*)
{
    g_previousFrameIndexedDraws = g_currentFrameIndexedDraws;
    g_currentFrameIndexedDraws = 0;

    g_previousSelectedOwners.swap(g_currentSelectedOwners);
    g_currentSelectedOwners.clear();

    // Owner->scene-object insertion is rebuilt every frame by the native scene
    // selection path. Clearing here prevents stale pointer reuse while keeping
    // the map live for the whole frame that follows this Present.
    if (g_ownerHooksEnabled)
        g_sceneObjectOwners.clear();

    if (g_logSelectionThisFrame)
        g_logSelectionThisFrame = false;

    if (g_isolationArming && !g_armingFrameFingerprints.empty())
    {
        g_referenceFingerprints = g_armingFrameFingerprints;
        g_armingFrameFingerprints.clear();
        g_isolationArming = false;
        g_isolationEnabled = true;
        ResetIsolationTree();
        LogIsolationState("reference-frozen");
    }

    PollHotkeys();
    BeginRequestedCapture();
}

void NotifyRenderTraceFinalCompositeGameState(IDirect3DDevice9* device)
{
    if (device == nullptr || !g_captureActive.load(std::memory_order_acquire) ||
        g_captureLogicalSaved)
    {
        return;
    }

    g_captureLogicalSaved = true;
    AppendLog("[RenderTrace][Capture] Final composite reached: saving game/logical inputs before ZachFix final-composite overrides.\n");
    WriteCompositeState(device, L"01_game");
    SaveCurrentRenderTarget(device, L"01_target_before");
    for (DWORD stage = 0; stage < 5; ++stage)
        SaveTextureStage(device, stage, L"01_game", stage == 0);
}

void NotifyRenderTraceFinalCompositeBoundState(IDirect3DDevice9* device)
{
    if (device == nullptr || !g_captureActive.load(std::memory_order_acquire) ||
        g_captureBoundSaved)
    {
        return;
    }

    g_captureBoundSaved = true;
    AppendLog("[RenderTrace][Capture] Saving actual final-composite bindings immediately before the draw.\n");
    WriteCompositeState(device, L"02_bound");
    for (DWORD stage = 0; stage < 10; ++stage)
        SaveTextureStage(device, stage, L"02_bound", stage == 0);
}

void NotifyRenderTraceFinalCompositeAfter(IDirect3DDevice9* device)
{
    if (device == nullptr || !g_captureActive.load(std::memory_order_acquire))
        return;

    SaveCurrentRenderTarget(device, L"03_final_after");
    g_captureActive.store(false, std::memory_order_release);
    AppendLog("[RenderTrace][Capture] Capture complete. Compare 01_game_s0 / 02_bound_s0 against 03_final_after; bloom is normally stage 1 on the vanilla composite.\n");
}

bool ShouldSubmitRenderMaterialIndexedDraw(
    IDirect3DDevice9* device,
    bool mainSceneGeometry,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount)
{
    if (!mainSceneGeometry)
        return true;

    const unsigned long long drawIndex = g_currentFrameIndexedDraws++;

    if (!g_isolationEnabled && !g_isolationArming)
        return true;

    const unsigned long long fingerprint = BuildDrawFingerprint(
        device,
        primitiveType,
        baseVertexIndex,
        minVertexIndex,
        numVertices,
        startIndex,
        primitiveCount);

    if (g_isolationArming)
    {
        g_armingFrameFingerprints.push_back(fingerprint);
        return true;
    }

    const bool fingerprintSelected =
        g_activeFingerprintSet.find(fingerprint) != g_activeFingerprintSet.end();

    if (fingerprintSelected && g_currentSceneOwner != 0)
        g_currentSelectedOwners.insert(g_currentSceneOwner);

    if (g_ownerLockEnabled)
    {
        const bool ownerSelected =
            g_currentSceneOwner != 0 && g_currentSceneOwner == g_lockedOwner;
        if (ownerSelected)
        {
            LogOwnerLockedDrawSummary(
                device,
                drawIndex,
                fingerprint,
                primitiveType,
                baseVertexIndex,
                numVertices,
                startIndex,
                primitiveCount);
        }
        return ownerSelected;
    }

    const IsolationRange active = GetActiveRange();
    if (fingerprintSelected && g_logSelectionThisFrame &&
        active.end > active.start &&
        active.end - active.start <= kDetailedRangeLimit)
    {
        unsigned long long referenceSlot = active.start;
        const unsigned long long end =
            (std::min<unsigned long long>)(active.end, g_referenceFingerprints.size());
        for (unsigned long long i = active.start; i < end; ++i)
        {
            if (g_referenceFingerprints[static_cast<size_t>(i)] == fingerprint)
            {
                referenceSlot = i;
                break;
            }
        }

        LogDetailedIndexedDraw(
            device,
            drawIndex,
            fingerprint,
            referenceSlot,
            primitiveType,
            baseVertexIndex,
            minVertexIndex,
            numVertices,
            startIndex,
            primitiveCount);
    }

    return fingerprintSelected;
}

void ResetRenderMaterialTraceForDeviceReset()
{
    g_captureRequested.store(false, std::memory_order_release);
    g_captureActive.store(false, std::memory_order_release);
    g_captureDirectory[0] = L'\0';
    g_captureLogicalSaved = false;
    g_captureBoundSaved = false;
    g_currentFrameIndexedDraws = 0;
    g_previousFrameIndexedDraws = 0;
    g_isolationEnabled = false;
    g_isolationArming = false;
    g_referenceFingerprints.clear();
    g_armingFrameFingerprints.clear();
    g_activeFingerprintSet.clear();
    g_parentRange = {};
    g_childIndex = 0;
    g_logSelectionThisFrame = false;
    g_ownerLockEnabled = false;
    g_lockedOwner = 0;
    g_ownerLockSummaryBudget = 0;
    g_currentSelectedOwners.clear();
    g_previousSelectedOwners.clear();
    SetOwnerAttributionHooksEnabled(false);

    if (g_bloomBypass.exchange(false, std::memory_order_acq_rel))
        SetShaderProbeBloomMultiplier(g_savedBloomMultiplier);
}
