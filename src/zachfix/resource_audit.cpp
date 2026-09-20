#include "resource_audit.h"

#include "logging.h"
#include "main_exe.h"
#include "version.h"

#include <MinHook.h>
#include <Psapi.h>
#include <TlHelp32.h>
#include <intrin.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
constexpr double kBytesPerMiB = 1024.0 * 1024.0;
constexpr double kSampleIntervalSeconds = 30.0;
constexpr size_t kReleaseHookSlots = 24;

enum class ResourceKind : unsigned int
{
    Texture2D,
    VolumeTexture,
    CubeTexture,
    VertexBuffer,
    IndexBuffer,
    RenderTarget,
    DepthStencil,
    OffscreenSurface,
    StateBlock,
    VertexDeclaration,
    VertexShader,
    PixelShader,
    Query,
    SwapChain,
    Count
};

constexpr size_t kResourceKindCount =
    static_cast<size_t>(ResourceKind::Count);

const char* ResourceKindName(ResourceKind kind)
{
    switch (kind)
    {
    case ResourceKind::Texture2D: return "Texture2D";
    case ResourceKind::VolumeTexture: return "VolumeTexture";
    case ResourceKind::CubeTexture: return "CubeTexture";
    case ResourceKind::VertexBuffer: return "VertexBuffer";
    case ResourceKind::IndexBuffer: return "IndexBuffer";
    case ResourceKind::RenderTarget: return "RenderTarget";
    case ResourceKind::DepthStencil: return "DepthStencil";
    case ResourceKind::OffscreenSurface: return "OffscreenSurface";
    case ResourceKind::StateBlock: return "StateBlock";
    case ResourceKind::VertexDeclaration: return "VertexDeclaration";
    case ResourceKind::VertexShader: return "VertexShader";
    case ResourceKind::PixelShader: return "PixelShader";
    case ResourceKind::Query: return "Query";
    case ResourceKind::SwapChain: return "SwapChain";
    default: return "Unknown";
    }
}

struct LiveResource
{
    ResourceKind kind = ResourceKind::Texture2D;
    unsigned long long estimatedBytes = 0;
    uintptr_t creationSite = 0;
};

struct SiteKey
{
    ResourceKind kind = ResourceKind::Texture2D;
    uintptr_t address = 0;

    bool operator==(const SiteKey& other) const
    {
        return kind == other.kind && address == other.address;
    }
};

struct SiteKeyHash
{
    size_t operator()(const SiteKey& key) const
    {
        const size_t a = static_cast<size_t>(key.address);
        const size_t b = static_cast<size_t>(key.kind);
        return a ^ (b + 0x9e3779b9u + (a << 6) + (a >> 2));
    }
};

struct SiteAggregate
{
    ResourceKind kind = ResourceKind::Texture2D;
    uintptr_t address = 0;
    unsigned long long count = 0;
    unsigned long long estimatedBytes = 0;
};

std::atomic_bool g_active{ false };
std::atomic<IDirect3DDevice9*> g_auditDevice{ nullptr };
std::atomic_bool g_optionalHooksInstalled{ false };
std::atomic_bool g_hookCoverageComplete{ true };

std::mutex g_stateMutex;
std::unordered_map<void*, LiveResource> g_liveResources;
std::array<unsigned long long, kResourceKindCount> g_created{};
std::array<unsigned long long, kResourceKindCount> g_released{};
unsigned long long g_sampleCount = 0;

std::mutex g_logMutex;
HMODULE g_selfModule = nullptr;
wchar_t g_auditLogPath[MAX_PATH] = {};
char g_auditLogFileName[128] = {};

std::mutex g_sampleMutex;
LARGE_INTEGER g_qpcFrequency = {};
LARGE_INTEGER g_sessionStartQpc = {};
LARGE_INTEGER g_lastSampleQpc = {};
unsigned long long g_presentCount = 0;
unsigned long long g_lastSamplePresentCount = 0;

std::mutex g_hookMutex;
std::array<void*, kReleaseHookSlots> g_releaseTargets{};
std::array<void*, kReleaseHookSlots> g_releaseOriginals{};
size_t g_releaseHookCount = 0;

using ReleaseFn = ULONG (WINAPI*)(IUnknown* self);

using CreateAdditionalSwapChainFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, D3DPRESENT_PARAMETERS*, IDirect3DSwapChain9**);
using CreateVolumeTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, UINT, UINT, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL,
    IDirect3DVolumeTexture9**, HANDLE*);
using CreateCubeTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, UINT, UINT, DWORD, D3DFORMAT, D3DPOOL,
    IDirect3DCubeTexture9**, HANDLE*);
using CreateVertexBufferFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, UINT, DWORD, DWORD, D3DPOOL,
    IDirect3DVertexBuffer9**, HANDLE*);
using CreateIndexBufferFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, UINT, DWORD, D3DFORMAT, D3DPOOL,
    IDirect3DIndexBuffer9**, HANDLE*);
using CreateOffscreenPlainSurfaceFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, UINT, UINT, D3DFORMAT, D3DPOOL,
    IDirect3DSurface9**, HANDLE*);
using CreateStateBlockFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, D3DSTATEBLOCKTYPE, IDirect3DStateBlock9**);
using EndStateBlockFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, IDirect3DStateBlock9**);
using CreateVertexDeclarationFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, const D3DVERTEXELEMENT9*, IDirect3DVertexDeclaration9**);
using CreateQueryFn = HRESULT (WINAPI*)(
    IDirect3DDevice9*, D3DQUERYTYPE, IDirect3DQuery9**);

CreateAdditionalSwapChainFn g_originalCreateAdditionalSwapChain = nullptr;
CreateVolumeTextureFn g_originalCreateVolumeTexture = nullptr;
CreateCubeTextureFn g_originalCreateCubeTexture = nullptr;
CreateVertexBufferFn g_originalCreateVertexBuffer = nullptr;
CreateIndexBufferFn g_originalCreateIndexBuffer = nullptr;
CreateOffscreenPlainSurfaceFn g_originalCreateOffscreenPlainSurface = nullptr;
CreateStateBlockFn g_originalCreateStateBlock = nullptr;
EndStateBlockFn g_originalEndStateBlock = nullptr;
CreateVertexDeclarationFn g_originalCreateVertexDeclaration = nullptr;
CreateQueryFn g_originalCreateQuery = nullptr;

unsigned int FormatBitsPerPixel(D3DFORMAT format)
{
    switch (format)
    {
    case D3DFMT_A8:
    case D3DFMT_L8:
    case D3DFMT_P8:
        return 8;

    case D3DFMT_R5G6B5:
    case D3DFMT_X1R5G5B5:
    case D3DFMT_A1R5G5B5:
    case D3DFMT_A4R4G4B4:
    case D3DFMT_A8L8:
    case D3DFMT_V8U8:
    case D3DFMT_L6V5U5:
    case D3DFMT_D16_LOCKABLE:
    case D3DFMT_D16:
    case D3DFMT_D15S1:
    case D3DFMT_R16F:
        return 16;

    case D3DFMT_R8G8B8:
        return 24;

    case D3DFMT_A8R8G8B8:
    case D3DFMT_X8R8G8B8:
    case D3DFMT_A8B8G8R8:
    case D3DFMT_X8B8G8R8:
    case D3DFMT_A2R10G10B10:
    case D3DFMT_A2B10G10R10:
    case D3DFMT_G16R16:
    case D3DFMT_A2W10V10U10:
    case D3DFMT_Q8W8V8U8:
    case D3DFMT_V16U16:
    case D3DFMT_X8L8V8U8:
    case D3DFMT_D32:
    case D3DFMT_D24S8:
    case D3DFMT_D24X8:
    case D3DFMT_D24X4S4:
    case D3DFMT_D32F_LOCKABLE:
    case D3DFMT_D24FS8:
    case D3DFMT_R32F:
    case D3DFMT_G16R16F:
        return 32;

    case D3DFMT_A16B16G16R16:
    case D3DFMT_Q16W16V16U16:
    case D3DFMT_A16B16G16R16F:
    case D3DFMT_G32R32F:
        return 64;

    case D3DFMT_A32B32G32R32F:
        return 128;

    default:
        return 32;
    }
}

unsigned long long Estimate2DLevelBytes(
    UINT width,
    UINT height,
    D3DFORMAT format)
{
    const UINT safeWidth = std::max<UINT>(1, width);
    const UINT safeHeight = std::max<UINT>(1, height);

    if (format == D3DFMT_DXT1)
    {
        return static_cast<unsigned long long>((safeWidth + 3) / 4) *
               static_cast<unsigned long long>((safeHeight + 3) / 4) * 8ull;
    }

    if (format == D3DFMT_DXT2 ||
        format == D3DFMT_DXT3 ||
        format == D3DFMT_DXT4 ||
        format == D3DFMT_DXT5)
    {
        return static_cast<unsigned long long>((safeWidth + 3) / 4) *
               static_cast<unsigned long long>((safeHeight + 3) / 4) * 16ull;
    }

    const unsigned int bits = FormatBitsPerPixel(format);
    return static_cast<unsigned long long>(safeWidth) *
           static_cast<unsigned long long>(safeHeight) * bits / 8ull;
}

unsigned long long EstimateTexture2DBytes(
    UINT width,
    UINT height,
    UINT levels,
    D3DFORMAT format)
{
    unsigned long long total = 0;
    UINT currentWidth = std::max<UINT>(1, width);
    UINT currentHeight = std::max<UINT>(1, height);
    UINT remaining = levels;

    for (;;)
    {
        total += Estimate2DLevelBytes(currentWidth, currentHeight, format);

        if (levels != 0)
        {
            if (--remaining == 0)
                break;
        }
        else if (currentWidth == 1 && currentHeight == 1)
        {
            break;
        }

        currentWidth = std::max<UINT>(1, currentWidth / 2);
        currentHeight = std::max<UINT>(1, currentHeight / 2);
    }

    return total;
}

unsigned long long EstimateVolumeTextureBytes(
    UINT width,
    UINT height,
    UINT depth,
    UINT levels,
    D3DFORMAT format)
{
    unsigned long long total = 0;
    UINT currentWidth = std::max<UINT>(1, width);
    UINT currentHeight = std::max<UINT>(1, height);
    UINT currentDepth = std::max<UINT>(1, depth);
    UINT remaining = levels;

    for (;;)
    {
        total += Estimate2DLevelBytes(currentWidth, currentHeight, format) *
                 static_cast<unsigned long long>(currentDepth);

        if (levels != 0)
        {
            if (--remaining == 0)
                break;
        }
        else if (currentWidth == 1 && currentHeight == 1 && currentDepth == 1)
        {
            break;
        }

        currentWidth = std::max<UINT>(1, currentWidth / 2);
        currentHeight = std::max<UINT>(1, currentHeight / 2);
        currentDepth = std::max<UINT>(1, currentDepth / 2);
    }

    return total;
}

unsigned int MultiSampleFactor(D3DMULTISAMPLE_TYPE type)
{
    const unsigned int value = static_cast<unsigned int>(type);
    return value >= 2 && value <= 16 ? value : 1;
}

bool BuildAuditLogPath()
{
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&SetD3D9ResourceAuditEnabled),
            &module))
    {
        return false;
    }

    g_selfModule = module;

    wchar_t modulePath[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(module, modulePath, MAX_PATH);
    if (length == 0 || length >= MAX_PATH)
        return false;

    wchar_t* slash = wcsrchr(modulePath, L'\\');
    if (slash == nullptr)
        return false;
    *(slash + 1) = L'\0';

    SYSTEMTIME now = {};
    GetLocalTime(&now);

    for (unsigned int suffix = 0; suffix < 1000; ++suffix)
    {
        wchar_t fileName[128] = {};
        if (suffix == 0)
        {
            swprintf_s(
                fileName,
                L"ZachFix-resource-audit-%04u%02u%02u-%02u%02u%02u.log",
                static_cast<unsigned>(now.wYear),
                static_cast<unsigned>(now.wMonth),
                static_cast<unsigned>(now.wDay),
                static_cast<unsigned>(now.wHour),
                static_cast<unsigned>(now.wMinute),
                static_cast<unsigned>(now.wSecond));
        }
        else
        {
            swprintf_s(
                fileName,
                L"ZachFix-resource-audit-%04u%02u%02u-%02u%02u%02u-%03u.log",
                static_cast<unsigned>(now.wYear),
                static_cast<unsigned>(now.wMonth),
                static_cast<unsigned>(now.wDay),
                static_cast<unsigned>(now.wHour),
                static_cast<unsigned>(now.wMinute),
                static_cast<unsigned>(now.wSecond),
                suffix);
        }

        wchar_t candidate[MAX_PATH] = {};
        if (wcscpy_s(candidate, modulePath) != 0 ||
            wcscat_s(candidate, fileName) != 0)
        {
            return false;
        }

        if (GetFileAttributesW(candidate) != INVALID_FILE_ATTRIBUTES)
            continue;

        FILE* file = nullptr;
        if (_wfopen_s(&file, candidate, L"w") != 0 || file == nullptr)
            continue;
        std::fclose(file);

        if (wcscpy_s(g_auditLogPath, candidate) != 0)
            return false;

        const int converted = WideCharToMultiByte(
            CP_UTF8,
            0,
            fileName,
            -1,
            g_auditLogFileName,
            static_cast<int>(sizeof(g_auditLogFileName)),
            nullptr,
            nullptr);
        if (converted == 0)
            strcpy_s(g_auditLogFileName, "ZachFix-resource-audit.log");

        return true;
    }

    g_auditLogPath[0] = L'\0';
    return false;
}

void AppendAuditV(const char* format, va_list args)
{
    std::lock_guard<std::mutex> lock(g_logMutex);
    if (g_auditLogPath[0] == L'\0')
        return;

    FILE* file = nullptr;
    if (_wfopen_s(&file, g_auditLogPath, L"a") != 0 || file == nullptr)
        return;

    std::vfprintf(file, format, args);
    std::fclose(file);
}

void AppendAudit(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    AppendAuditV(format, args);
    va_end(args);
}

void DescribeAddress(uintptr_t address, char* buffer, size_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0)
        return;

    if (address == 0)
    {
        strcpy_s(buffer, bufferSize, "unknown");
        return;
    }

    const void* pointer = reinterpret_cast<const void*>(address);
    if (IsMainExeAddress(pointer))
    {
        sprintf_s(
            buffer,
            bufferSize,
            "DP.exe+0x%08llX",
            static_cast<unsigned long long>(address - g_mainExeBase));
        return;
    }

    HMODULE module = nullptr;
    if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(address),
            &module))
    {
        char path[MAX_PATH] = {};
        if (GetModuleFileNameA(module, path, MAX_PATH) != 0)
        {
            const char* name = strrchr(path, '\\');
            name = name != nullptr ? name + 1 : path;
            const uintptr_t moduleBase = reinterpret_cast<uintptr_t>(module);
            sprintf_s(
                buffer,
                bufferSize,
                "%s+0x%08llX",
                name,
                static_cast<unsigned long long>(address - moduleBase));
            return;
        }
    }

    sprintf_s(
        buffer,
        bufferSize,
        "0x%08llX",
        static_cast<unsigned long long>(address));
}

bool PathStartsWithDirectory(const char* path, const char* directory)
{
    if (path == nullptr || directory == nullptr)
        return false;

    size_t directoryLength = std::strlen(directory);
    while (directoryLength > 0 &&
           (directory[directoryLength - 1] == '\\' ||
            directory[directoryLength - 1] == '/'))
    {
        --directoryLength;
    }

    if (directoryLength == 0 ||
        _strnicmp(path, directory, directoryLength) != 0)
    {
        return false;
    }

    return path[directoryLength] == '\\' || path[directoryLength] == '/';
}

void DescribeBackendAddress(
    uintptr_t address,
    char* buffer,
    size_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0)
        return;

    HMODULE module = nullptr;
    if (address != 0 &&
        GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(address),
            &module))
    {
        char modulePath[MAX_PATH] = {};
        if (GetModuleFileNameA(module, modulePath, MAX_PATH) != 0)
        {
            const char* name = strrchr(modulePath, '\\');
            name = name != nullptr ? name + 1 : modulePath;

            const char* origin = "other";
            char systemDirectory[MAX_PATH] = {};
            if (GetSystemDirectoryA(systemDirectory, MAX_PATH) != 0 &&
                PathStartsWithDirectory(modulePath, systemDirectory))
            {
                origin = "system";
            }
            else
            {
                char exePath[MAX_PATH] = {};
                if (GetModuleFileNameA(nullptr, exePath, MAX_PATH) != 0)
                {
                    char* slash = strrchr(exePath, '\\');
                    if (slash != nullptr)
                    {
                        *slash = '\0';
                        if (PathStartsWithDirectory(modulePath, exePath))
                            origin = "game-local";
                    }
                }
            }

            const uintptr_t moduleBase = reinterpret_cast<uintptr_t>(module);
            sprintf_s(
                buffer,
                bufferSize,
                "%s origin=%s +0x%08llX",
                name,
                origin,
                static_cast<unsigned long long>(address - moduleBase));
            return;
        }
    }

    DescribeAddress(address, buffer, bufferSize);
}

unsigned int CountProcessThreads()
{
    const DWORD processId = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    unsigned int count = 0;
    THREADENTRY32 entry = {};
    entry.dwSize = sizeof(entry);
    if (Thread32First(snapshot, &entry))
    {
        do
        {
            if (entry.th32OwnerProcessID == processId)
                ++count;
        }
        while (Thread32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return count;
}

bool IsZachFixModuleAddress(const void* address)
{
    if (address == nullptr || g_selfModule == nullptr)
        return false;

    HMODULE addressModule = nullptr;
    if (!GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(address),
            &addressModule))
    {
        return false;
    }

    return g_selfModule == addressModule;
}

uintptr_t ResolveCreationSite(void* directSite)
{
    if (directSite == nullptr)
        return 0;

    // Preserve direct game calls and ZachFix-owned allocations exactly. A
    // ZachFix Create* call can itself sit under a DP.exe Present call, so a
    // blind stack scan would otherwise misattribute our own resources to DP.
    if (IsMainExeAddress(directSite) || IsZachFixModuleAddress(directSite))
        return reinterpret_cast<uintptr_t>(directSite);

    // D3DX and wrapper helpers often sit between DP.exe and the actual D3D9
    // Create* call. While the audit is active, walk only a small stack window
    // and prefer the first frame that belongs to the game executable. This
    // turns generic d3dx9_43.dll creation sites into actionable DP.exe RVAs.
    void* frames[16] = {};
    const USHORT captured = RtlCaptureStackBackTrace(0, 16, frames, nullptr);
    for (USHORT i = 0; i < captured; ++i)
    {
        if (IsMainExeAddress(frames[i]))
            return reinterpret_cast<uintptr_t>(frames[i]);
    }

    return reinterpret_cast<uintptr_t>(directSite);
}

void RecordFinalRelease(void* object)
{
    if (!g_active.load(std::memory_order_acquire) || object == nullptr)
        return;

    std::lock_guard<std::mutex> lock(g_stateMutex);
    const auto it = g_liveResources.find(object);
    if (it == g_liveResources.end())
        return;

    const size_t kindIndex = static_cast<size_t>(it->second.kind);
    if (kindIndex < g_released.size())
        ++g_released[kindIndex];
    g_liveResources.erase(it);
}

template<size_t Slot>
ULONG WINAPI HookAuditedRelease(IUnknown* self)
{
    const ReleaseFn original = reinterpret_cast<ReleaseFn>(g_releaseOriginals[Slot]);
    if (original == nullptr)
        return 0;

    const ULONG references = original(self);
    if (references == 0)
        RecordFinalRelease(self);
    return references;
}

using ReleaseDetour = ULONG (WINAPI*)(IUnknown*);

const std::array<ReleaseDetour, kReleaseHookSlots> g_releaseDetours =
{
    &HookAuditedRelease<0>, &HookAuditedRelease<1>, &HookAuditedRelease<2>,
    &HookAuditedRelease<3>, &HookAuditedRelease<4>, &HookAuditedRelease<5>,
    &HookAuditedRelease<6>, &HookAuditedRelease<7>, &HookAuditedRelease<8>,
    &HookAuditedRelease<9>, &HookAuditedRelease<10>, &HookAuditedRelease<11>,
    &HookAuditedRelease<12>, &HookAuditedRelease<13>, &HookAuditedRelease<14>,
    &HookAuditedRelease<15>, &HookAuditedRelease<16>, &HookAuditedRelease<17>,
    &HookAuditedRelease<18>, &HookAuditedRelease<19>, &HookAuditedRelease<20>,
    &HookAuditedRelease<21>, &HookAuditedRelease<22>, &HookAuditedRelease<23>
};

bool EnsureReleaseHook(void* object)
{
    if (object == nullptr)
        return false;

    void** vtable = *reinterpret_cast<void***>(object);
    if (vtable == nullptr || vtable[2] == nullptr)
        return false;

    void* target = vtable[2];
    std::lock_guard<std::mutex> lock(g_hookMutex);

    for (size_t i = 0; i < g_releaseHookCount; ++i)
    {
        if (g_releaseTargets[i] == target)
            return true;
    }

    if (g_releaseHookCount >= kReleaseHookSlots)
    {
        g_hookCoverageComplete.store(false, std::memory_order_release);
        AppendAudit("[warning] release-hook slot capacity exhausted; lifetime coverage is partial.\n");
        return false;
    }

    const size_t slot = g_releaseHookCount;
    void* original = nullptr;
    const MH_STATUS createStatus = MH_CreateHook(
        target,
        reinterpret_cast<void*>(g_releaseDetours[slot]),
        &original);

    if (createStatus != MH_OK)
    {
        g_hookCoverageComplete.store(false, std::memory_order_release);
        AppendAudit(
            "[warning] MH_CreateHook failed for resource Release target=%p status=%d; lifetime coverage is partial.\n",
            target,
            static_cast<int>(createStatus));
        return false;
    }

    g_releaseOriginals[slot] = original;
    g_releaseTargets[slot] = target;

    const MH_STATUS enableStatus = MH_EnableHook(target);
    if (enableStatus != MH_OK && enableStatus != MH_ERROR_ENABLED)
    {
        MH_RemoveHook(target);
        g_releaseOriginals[slot] = nullptr;
        g_releaseTargets[slot] = nullptr;
        g_hookCoverageComplete.store(false, std::memory_order_release);
        AppendAudit(
            "[warning] MH_EnableHook failed for resource Release target=%p status=%d; lifetime coverage is partial.\n",
            target,
            static_cast<int>(enableStatus));
        return false;
    }

    ++g_releaseHookCount;
    return true;
}

void TrackResource(
    void* object,
    ResourceKind kind,
    unsigned long long estimatedBytes,
    void* creationSite)
{
    if (!g_active.load(std::memory_order_acquire) || object == nullptr)
        return;

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        const size_t index = static_cast<size_t>(kind);
        if (index < g_created.size())
            ++g_created[index];

        g_liveResources[object] = LiveResource{
            kind,
            estimatedBytes,
            ResolveCreationSite(creationSite)
        };
    }

    EnsureReleaseHook(object);
}

HRESULT WINAPI HookCreateAdditionalSwapChain(
    IDirect3DDevice9* self,
    D3DPRESENT_PARAMETERS* parameters,
    IDirect3DSwapChain9** swapChain)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateAdditionalSwapChain(
        self, parameters, swapChain);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && swapChain != nullptr && *swapChain != nullptr)
    {
        TrackResource(
            *swapChain,
            ResourceKind::SwapChain,
            0,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateVolumeTexture(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    UINT depth,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DVolumeTexture9** texture,
    HANDLE* sharedHandle)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateVolumeTexture(
        self, width, height, depth, levels, usage, format, pool, texture, sharedHandle);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && texture != nullptr && *texture != nullptr)
    {
        TrackResource(
            *texture,
            ResourceKind::VolumeTexture,
            EstimateVolumeTextureBytes(width, height, depth, levels, format),
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateCubeTexture(
    IDirect3DDevice9* self,
    UINT edgeLength,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DCubeTexture9** texture,
    HANDLE* sharedHandle)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateCubeTexture(
        self, edgeLength, levels, usage, format, pool, texture, sharedHandle);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && texture != nullptr && *texture != nullptr)
    {
        TrackResource(
            *texture,
            ResourceKind::CubeTexture,
            EstimateTexture2DBytes(edgeLength, edgeLength, levels, format) * 6ull,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateVertexBuffer(
    IDirect3DDevice9* self,
    UINT length,
    DWORD usage,
    DWORD fvf,
    D3DPOOL pool,
    IDirect3DVertexBuffer9** buffer,
    HANDLE* sharedHandle)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateVertexBuffer(
        self, length, usage, fvf, pool, buffer, sharedHandle);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && buffer != nullptr && *buffer != nullptr)
    {
        TrackResource(
            *buffer,
            ResourceKind::VertexBuffer,
            length,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateIndexBuffer(
    IDirect3DDevice9* self,
    UINT length,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DIndexBuffer9** buffer,
    HANDLE* sharedHandle)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateIndexBuffer(
        self, length, usage, format, pool, buffer, sharedHandle);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && buffer != nullptr && *buffer != nullptr)
    {
        TrackResource(
            *buffer,
            ResourceKind::IndexBuffer,
            length,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateOffscreenPlainSurface(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateOffscreenPlainSurface(
        self, width, height, format, pool, surface, sharedHandle);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && surface != nullptr && *surface != nullptr)
    {
        TrackResource(
            *surface,
            ResourceKind::OffscreenSurface,
            Estimate2DLevelBytes(width, height, format),
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateStateBlock(
    IDirect3DDevice9* self,
    D3DSTATEBLOCKTYPE type,
    IDirect3DStateBlock9** stateBlock)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateStateBlock(self, type, stateBlock);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && stateBlock != nullptr && *stateBlock != nullptr)
    {
        TrackResource(
            *stateBlock,
            ResourceKind::StateBlock,
            0,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookEndStateBlock(
    IDirect3DDevice9* self,
    IDirect3DStateBlock9** stateBlock)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalEndStateBlock(self, stateBlock);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && stateBlock != nullptr && *stateBlock != nullptr)
    {
        TrackResource(
            *stateBlock,
            ResourceKind::StateBlock,
            0,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateVertexDeclaration(
    IDirect3DDevice9* self,
    const D3DVERTEXELEMENT9* elements,
    IDirect3DVertexDeclaration9** declaration)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateVertexDeclaration(
        self, elements, declaration);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && declaration != nullptr && *declaration != nullptr)
    {
        TrackResource(
            *declaration,
            ResourceKind::VertexDeclaration,
            0,
            const_cast<void*>(site));
    }

    return result;
}

HRESULT WINAPI HookCreateQuery(
    IDirect3DDevice9* self,
    D3DQUERYTYPE type,
    IDirect3DQuery9** query)
{
    const void* site = _ReturnAddress();
    const HRESULT result = g_originalCreateQuery(self, type, query);

    if (g_active.load(std::memory_order_acquire) &&
        self == g_auditDevice.load(std::memory_order_acquire) &&
        SUCCEEDED(result) && query != nullptr && *query != nullptr)
    {
        TrackResource(
            *query,
            ResourceKind::Query,
            0,
            const_cast<void*>(site));
    }

    return result;
}

bool InstallOptionalHook(
    void* target,
    void* detour,
    void** original,
    const char* name)
{
    const MH_STATUS createStatus = MH_CreateHook(target, detour, original);
    if (createStatus != MH_OK)
    {
        AppendLog("[ResourceAudit] WARNING: optional D3D9 hook creation failed.\n");
        AppendAudit(
            "[warning] optional create hook failed: %s createStatus=%d\n",
            name,
            static_cast<int>(createStatus));
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(target);
    if (enableStatus != MH_OK && enableStatus != MH_ERROR_ENABLED)
    {
        MH_RemoveHook(target);
        *original = nullptr;
        AppendLog("[ResourceAudit] WARNING: optional D3D9 hook enable failed.\n");
        AppendAudit(
            "[warning] optional create hook failed: %s enableStatus=%d\n",
            name,
            static_cast<int>(enableStatus));
        return false;
    }

    return true;
}

bool InstallOptionalCreateHooks(IDirect3DDevice9* device)
{
    if (g_optionalHooksInstalled.load(std::memory_order_acquire))
        return g_hookCoverageComplete.load(std::memory_order_acquire);

    std::lock_guard<std::mutex> lock(g_hookMutex);
    if (g_optionalHooksInstalled.load(std::memory_order_relaxed))
        return g_hookCoverageComplete.load(std::memory_order_relaxed);

    if (device == nullptr)
        return false;

    void** vtable = *reinterpret_cast<void***>(device);
    if (vtable == nullptr)
        return false;

    bool complete = true;
    complete &= InstallOptionalHook(
        vtable[13],
        reinterpret_cast<void*>(&HookCreateAdditionalSwapChain),
        reinterpret_cast<void**>(&g_originalCreateAdditionalSwapChain),
        "CreateAdditionalSwapChain");
    complete &= InstallOptionalHook(
        vtable[24],
        reinterpret_cast<void*>(&HookCreateVolumeTexture),
        reinterpret_cast<void**>(&g_originalCreateVolumeTexture),
        "CreateVolumeTexture");
    complete &= InstallOptionalHook(
        vtable[25],
        reinterpret_cast<void*>(&HookCreateCubeTexture),
        reinterpret_cast<void**>(&g_originalCreateCubeTexture),
        "CreateCubeTexture");
    complete &= InstallOptionalHook(
        vtable[26],
        reinterpret_cast<void*>(&HookCreateVertexBuffer),
        reinterpret_cast<void**>(&g_originalCreateVertexBuffer),
        "CreateVertexBuffer");
    complete &= InstallOptionalHook(
        vtable[27],
        reinterpret_cast<void*>(&HookCreateIndexBuffer),
        reinterpret_cast<void**>(&g_originalCreateIndexBuffer),
        "CreateIndexBuffer");
    complete &= InstallOptionalHook(
        vtable[36],
        reinterpret_cast<void*>(&HookCreateOffscreenPlainSurface),
        reinterpret_cast<void**>(&g_originalCreateOffscreenPlainSurface),
        "CreateOffscreenPlainSurface");
    complete &= InstallOptionalHook(
        vtable[59],
        reinterpret_cast<void*>(&HookCreateStateBlock),
        reinterpret_cast<void**>(&g_originalCreateStateBlock),
        "CreateStateBlock");
    complete &= InstallOptionalHook(
        vtable[61],
        reinterpret_cast<void*>(&HookEndStateBlock),
        reinterpret_cast<void**>(&g_originalEndStateBlock),
        "EndStateBlock");
    complete &= InstallOptionalHook(
        vtable[86],
        reinterpret_cast<void*>(&HookCreateVertexDeclaration),
        reinterpret_cast<void**>(&g_originalCreateVertexDeclaration),
        "CreateVertexDeclaration");
    complete &= InstallOptionalHook(
        vtable[118],
        reinterpret_cast<void*>(&HookCreateQuery),
        reinterpret_cast<void**>(&g_originalCreateQuery),
        "CreateQuery");

    g_hookCoverageComplete.store(complete, std::memory_order_release);
    g_optionalHooksInstalled.store(true, std::memory_order_release);
    return complete;
}

void WriteAuditHeader(IDirect3DDevice9* device)
{
    AppendAudit("# ZachFix D3D9 Resource Lifetime Audit\n");
    AppendAudit("# ZachFix version: %s\n", kZachFixVersion);
    if (const DpBuildProfile* build = GetDpBuildProfile())
        AppendAudit("# DP build: %s\n", build->name);
    else
        AppendAudit("# DP build: unknown\n");
    AppendAudit("# Sampling interval: %.0f seconds\n", kSampleIntervalSeconds);
    AppendAudit("# Scope: resources created after audit activation only.\n");
    AppendAudit("# Estimated resource bytes are diagnostic approximations, not exact VRAM accounting.\n");
    AppendAudit("# IDirect3DDevice9::GetAvailableTextureMem is driver-defined and should be read as a trend only.\n");
    AppendAudit(
        "# Hook coverage at activation: %s\n",
        g_hookCoverageComplete.load(std::memory_order_acquire)
            ? "complete"
            : "PARTIAL");

    if (device == nullptr)
        return;

    void** vtable = *reinterpret_cast<void***>(device);
    if (vtable != nullptr)
    {
        char backend[512] = {};
        DescribeBackendAddress(
            reinterpret_cast<uintptr_t>(vtable[17]),
            backend,
            sizeof(backend));
        AppendAudit("# Device Present implementation: %s\n", backend);
    }

    IDirect3D9* d3d9 = nullptr;
    if (SUCCEEDED(device->GetDirect3D(&d3d9)) && d3d9 != nullptr)
    {
        D3DDEVICE_CREATION_PARAMETERS creation = {};
        if (SUCCEEDED(device->GetCreationParameters(&creation)))
        {
            D3DADAPTER_IDENTIFIER9 identifier = {};
            if (SUCCEEDED(d3d9->GetAdapterIdentifier(
                    creation.AdapterOrdinal,
                    0,
                    &identifier)))
            {
                const DWORD driverHigh =
                    static_cast<DWORD>(identifier.DriverVersion.HighPart);
                const DWORD driverLow =
                    static_cast<DWORD>(identifier.DriverVersion.LowPart);
                AppendAudit(
                    "# Adapter: %s | driver=%s %u.%u.%u.%u | vendor=0x%04X device=0x%04X\n",
                    identifier.Description,
                    identifier.Driver,
                    HIWORD(driverHigh),
                    LOWORD(driverHigh),
                    HIWORD(driverLow),
                    LOWORD(driverLow),
                    identifier.VendorId,
                    identifier.DeviceId);
            }
        }
        d3d9->Release();
    }

    AppendAudit("\n");
}

void WriteSnapshot(
    IDirect3DDevice9* device,
    const char* reason,
    double elapsedSeconds,
    double intervalFps)
{
    std::array<unsigned long long, kResourceKindCount> created = {};
    std::array<unsigned long long, kResourceKindCount> released = {};
    std::array<unsigned long long, kResourceKindCount> liveCounts = {};
    std::array<unsigned long long, kResourceKindCount> liveBytes = {};
    std::unordered_map<SiteKey, SiteAggregate, SiteKeyHash> siteMap;
    unsigned long long sampleNumber = 0;

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        created = g_created;
        released = g_released;
        sampleNumber = ++g_sampleCount;

        for (const auto& pair : g_liveResources)
        {
            const LiveResource& resource = pair.second;
            const size_t index = static_cast<size_t>(resource.kind);
            if (index >= kResourceKindCount)
                continue;

            ++liveCounts[index];
            liveBytes[index] += resource.estimatedBytes;

            const SiteKey key{ resource.kind, resource.creationSite };
            SiteAggregate& aggregate = siteMap[key];
            aggregate.kind = resource.kind;
            aggregate.address = resource.creationSite;
            ++aggregate.count;
            aggregate.estimatedBytes += resource.estimatedBytes;
        }
    }

    PROCESS_MEMORY_COUNTERS_EX memory = {};
    memory.cb = sizeof(memory);
    const bool haveMemory = GetProcessMemoryInfo(
        GetCurrentProcess(),
        reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memory),
        sizeof(memory)) != FALSE;

    DWORD handleCount = 0;
    GetProcessHandleCount(GetCurrentProcess(), &handleCount);
    const DWORD gdiObjects = GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS);
    const DWORD userObjects = GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS);
    const unsigned int threadCount = CountProcessThreads();
    const UINT availableTextureMemory =
        device != nullptr ? device->GetAvailableTextureMem() : 0;

    unsigned long long totalCreated = 0;
    unsigned long long totalReleased = 0;
    unsigned long long totalLive = 0;
    unsigned long long totalLiveBytes = 0;
    for (size_t i = 0; i < kResourceKindCount; ++i)
    {
        totalCreated += created[i];
        totalReleased += released[i];
        totalLive += liveCounts[i];
        totalLiveBytes += liveBytes[i];
    }

    AppendAudit(
        "[sample %llu] reason=%s t=%.1fs fps=%.2f coverage=%s\n",
        sampleNumber,
        reason != nullptr ? reason : "periodic",
        elapsedSeconds,
        intervalFps,
        g_hookCoverageComplete.load(std::memory_order_acquire)
            ? "complete"
            : "PARTIAL");

    if (haveMemory)
    {
        AppendAudit(
            "process private=%.1fMiB working=%.1fMiB commit=%.1fMiB handles=%lu gdi=%lu user=%lu threads=%u availableTexture=%.1fMiB\n",
            static_cast<double>(memory.PrivateUsage) / kBytesPerMiB,
            static_cast<double>(memory.WorkingSetSize) / kBytesPerMiB,
            static_cast<double>(memory.PagefileUsage) / kBytesPerMiB,
            static_cast<unsigned long>(handleCount),
            static_cast<unsigned long>(gdiObjects),
            static_cast<unsigned long>(userObjects),
            threadCount,
            static_cast<double>(availableTextureMemory) / kBytesPerMiB);
    }
    else
    {
        AppendAudit(
            "process private=unavailable working=unavailable handles=%lu gdi=%lu user=%lu threads=%u availableTexture=%.1fMiB\n",
            static_cast<unsigned long>(handleCount),
            static_cast<unsigned long>(gdiObjects),
            static_cast<unsigned long>(userObjects),
            threadCount,
            static_cast<double>(availableTextureMemory) / kBytesPerMiB);
    }

    AppendAudit(
        "resources created=%llu released=%llu live=%llu estimatedLive=%.1fMiB\n",
        totalCreated,
        totalReleased,
        totalLive,
        static_cast<double>(totalLiveBytes) / kBytesPerMiB);

    for (size_t i = 0; i < kResourceKindCount; ++i)
    {
        if (created[i] == 0 && released[i] == 0 && liveCounts[i] == 0)
            continue;

        AppendAudit(
            "  %-18s created=%llu released=%llu live=%llu estimatedLive=%.1fMiB\n",
            ResourceKindName(static_cast<ResourceKind>(i)),
            created[i],
            released[i],
            liveCounts[i],
            static_cast<double>(liveBytes[i]) / kBytesPerMiB);
    }

    std::vector<SiteAggregate> sites;
    sites.reserve(siteMap.size());
    for (const auto& pair : siteMap)
        sites.push_back(pair.second);

    std::sort(
        sites.begin(),
        sites.end(),
        [](const SiteAggregate& a, const SiteAggregate& b)
        {
            if (a.count != b.count)
                return a.count > b.count;
            return a.estimatedBytes > b.estimatedBytes;
        });

    if (!sites.empty())
    {
        AppendAudit("top-live-creation-sites:\n");
        const size_t count = std::min<size_t>(12, sites.size());
        for (size_t i = 0; i < count; ++i)
        {
            char location[320] = {};
            DescribeAddress(sites[i].address, location, sizeof(location));
            AppendAudit(
                "  %2zu. %-18s live=%llu estimated=%.1fMiB site=%s\n",
                i + 1,
                ResourceKindName(sites[i].kind),
                sites[i].count,
                static_cast<double>(sites[i].estimatedBytes) / kBytesPerMiB,
                location);
        }
    }

    AppendAudit("\n");
}

} // namespace

bool SetD3D9ResourceAuditEnabled(
    IDirect3DDevice9* device,
    bool enabled)
{
    if (enabled)
    {
        if (g_active.load(std::memory_order_acquire))
            return true;
        if (device == nullptr)
            return false;

        g_auditDevice.store(device, std::memory_order_release);

        if (!BuildAuditLogPath())
        {
            AppendLog("[ResourceAudit] ERROR: could not create the dedicated audit log.\n");
            g_auditDevice.store(nullptr, std::memory_order_release);
            return false;
        }

        InstallOptionalCreateHooks(device);

        {
            std::lock_guard<std::mutex> lock(g_stateMutex);
            g_liveResources.clear();
            g_created.fill(0);
            g_released.fill(0);
            g_sampleCount = 0;
        }

        QueryPerformanceFrequency(&g_qpcFrequency);
        QueryPerformanceCounter(&g_sessionStartQpc);
        g_lastSampleQpc = g_sessionStartQpc;
        g_presentCount = 0;
        g_lastSamplePresentCount = 0;

        WriteAuditHeader(device);
        g_active.store(true, std::memory_order_release);
        WriteSnapshot(device, "start", 0.0, 0.0);

        char text[256] = {};
        sprintf_s(
            text,
            "[ResourceAudit] Started dedicated D3D9 lifetime audit: %s\n",
            g_auditLogFileName);
        AppendLog(text);
        return true;
    }

    if (!g_active.exchange(false, std::memory_order_acq_rel))
        return true;

    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);
    const double intervalElapsed =
        g_qpcFrequency.QuadPart > 0
            ? static_cast<double>(now.QuadPart - g_lastSampleQpc.QuadPart) /
                static_cast<double>(g_qpcFrequency.QuadPart)
            : 0.0;
    const double totalElapsed =
        g_qpcFrequency.QuadPart > 0
            ? static_cast<double>(now.QuadPart - g_sessionStartQpc.QuadPart) /
                static_cast<double>(g_qpcFrequency.QuadPart)
            : 0.0;
    const unsigned long long intervalPresents =
        g_presentCount - g_lastSamplePresentCount;
    const double fps = intervalElapsed > 0.0
        ? static_cast<double>(intervalPresents) / intervalElapsed
        : 0.0;

    WriteSnapshot(device, "stop", totalElapsed, fps);
    AppendAudit("# Audit stopped by user.\n");
    AppendLog("[ResourceAudit] Stopped dedicated D3D9 lifetime audit.\n");
    g_auditDevice.store(nullptr, std::memory_order_release);
    return true;
}

bool IsD3D9ResourceAuditEnabled()
{
    return g_active.load(std::memory_order_acquire);
}

D3D9ResourceAuditStats GetD3D9ResourceAuditStats()
{
    D3D9ResourceAuditStats stats = {};
    stats.active = g_active.load(std::memory_order_acquire);
    stats.optionalHooksInstalled =
        g_optionalHooksInstalled.load(std::memory_order_acquire);
    stats.hookCoverageComplete =
        g_hookCoverageComplete.load(std::memory_order_acquire);

    {
        std::lock_guard<std::mutex> lock(g_stateMutex);
        stats.samples = g_sampleCount;
        stats.live = static_cast<unsigned long long>(g_liveResources.size());
        for (size_t i = 0; i < kResourceKindCount; ++i)
        {
            stats.created += g_created[i];
            stats.released += g_released[i];
        }
        for (const auto& pair : g_liveResources)
            stats.estimatedLiveBytes += pair.second.estimatedBytes;
    }

    strcpy_s(stats.logFileName, g_auditLogFileName);
    return stats;
}

void D3D9ResourceAuditOnPresent(IDirect3DDevice9* device)
{
    if (!g_active.load(std::memory_order_acquire) || device == nullptr)
        return;

    std::lock_guard<std::mutex> lock(g_sampleMutex);
    ++g_presentCount;

    LARGE_INTEGER now = {};
    QueryPerformanceCounter(&now);
    if (g_qpcFrequency.QuadPart <= 0)
        return;

    const double intervalElapsed =
        static_cast<double>(now.QuadPart - g_lastSampleQpc.QuadPart) /
        static_cast<double>(g_qpcFrequency.QuadPart);
    if (intervalElapsed < kSampleIntervalSeconds)
        return;

    const double totalElapsed =
        static_cast<double>(now.QuadPart - g_sessionStartQpc.QuadPart) /
        static_cast<double>(g_qpcFrequency.QuadPart);
    const unsigned long long intervalPresents =
        g_presentCount - g_lastSamplePresentCount;
    const double fps = intervalElapsed > 0.0
        ? static_cast<double>(intervalPresents) / intervalElapsed
        : 0.0;

    g_lastSampleQpc = now;
    g_lastSamplePresentCount = g_presentCount;
    WriteSnapshot(device, "periodic", totalElapsed, fps);
}

void D3D9ResourceAuditTrackTexture(
    IDirect3DTexture9* texture,
    UINT width,
    UINT height,
    UINT levels,
    DWORD,
    D3DFORMAT format,
    D3DPOOL,
    void* creationSite)
{
    TrackResource(
        texture,
        ResourceKind::Texture2D,
        EstimateTexture2DBytes(width, height, levels, format),
        creationSite);
}

void D3D9ResourceAuditTrackRenderTarget(
    IDirect3DSurface9* surface,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    void* creationSite)
{
    TrackResource(
        surface,
        ResourceKind::RenderTarget,
        Estimate2DLevelBytes(width, height, format) * MultiSampleFactor(multiSample),
        creationSite);
}

void D3D9ResourceAuditTrackDepthStencil(
    IDirect3DSurface9* surface,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    void* creationSite)
{
    TrackResource(
        surface,
        ResourceKind::DepthStencil,
        Estimate2DLevelBytes(width, height, format) * MultiSampleFactor(multiSample),
        creationSite);
}

void D3D9ResourceAuditTrackVertexShader(
    IDirect3DVertexShader9* shader,
    void* creationSite)
{
    TrackResource(shader, ResourceKind::VertexShader, 0, creationSite);
}

void D3D9ResourceAuditTrackPixelShader(
    IDirect3DPixelShader9* shader,
    void* creationSite)
{
    TrackResource(shader, ResourceKind::PixelShader, 0, creationSite);
}
