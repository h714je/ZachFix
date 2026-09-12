#include "texture_override.h"

#include "config.h"
#include "logging.h"

#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <mutex>
#include <unordered_set>

namespace
{
// D3DX9 is part of the legacy DirectX runtime used by Deadly Premonition.
// Keeping these declarations local avoids making the old DirectX SDK a build dependency.
struct D3DXIMAGE_INFO
{
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    D3DFORMAT Format;
    D3DRESOURCETYPE ResourceType;
    int ImageFileFormat;
};

static_assert(sizeof(D3DXIMAGE_INFO) == 28, "Unexpected D3DXIMAGE_INFO layout");

using D3DXCreateTextureFromFileInMemoryFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* device,
    const void* sourceData,
    UINT sourceDataSize,
    IDirect3DTexture9** texture);

using D3DXCreateTextureFromFileInMemoryExFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* device,
    const void* sourceData,
    UINT sourceDataSize,
    UINT width,
    UINT height,
    UINT mipLevels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    DWORD filter,
    DWORD mipFilter,
    D3DCOLOR colorKey,
    D3DXIMAGE_INFO* sourceInfo,
    PALETTEENTRY* palette,
    IDirect3DTexture9** texture);

using D3DXCreateTextureFromFileExWFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* device,
    const wchar_t* sourceFile,
    UINT width,
    UINT height,
    UINT mipLevels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    DWORD filter,
    DWORD mipFilter,
    D3DCOLOR colorKey,
    D3DXIMAGE_INFO* sourceInfo,
    PALETTEENTRY* palette,
    IDirect3DTexture9** texture);

using D3DXSaveSurfaceToFileWFn = HRESULT (WINAPI*)(
    const wchar_t* destinationFile,
    int destinationFormat,
    IDirect3DSurface9* sourceSurface,
    const PALETTEENTRY* sourcePalette,
    const RECT* sourceRect);

using D3DXGetImageInfoFromFileInMemoryFn = HRESULT (WINAPI*)(
    const void* sourceData,
    UINT sourceDataSize,
    D3DXIMAGE_INFO* sourceInfo);

using D3DXGetImageInfoFromFileWFn = HRESULT (WINAPI*)(
    const wchar_t* sourceFile,
    D3DXIMAGE_INFO* sourceInfo);

constexpr UINT kDPFixUnknownSourceSize = 0x7fffffffu;
constexpr UINT kD3DXDefault = 0xffffffffu;
constexpr UINT kD3DXDefaultNonPow2 = 0xfffffffeu;
constexpr int kD3DXImageFileFormatTga = 2;
constexpr UINT kHotReloadMetadataMagic = 0x58465444u; // 'DTFX'
constexpr UINT kHotReloadMetadataVersion = 3u;

// Private-data GUIDs are deliberately ZachFix specific. The metadata blob is
// copied by D3D9, while the replacement slot uses D3DSPD_IUNKNOWN so D3D9
// owns the COM reference and releases it automatically with the logical texture.
const GUID kHotReloadMetadataGuid =
{ 0x4fd3195e, 0x2f7f, 0x4f3f, { 0x8b, 0x73, 0x75, 0x2f, 0xc1, 0x31, 0x0d, 0x42 } };
const GUID kHotReloadReplacementGuid =
{ 0xe75c974d, 0xe79f, 0x45d8, { 0xaf, 0xe7, 0x52, 0x4f, 0x37, 0xd8, 0x8f, 0xa1 } };

struct TextureHotReloadMetadata
{
    UINT magic = kHotReloadMetadataMagic;
    UINT version = kHotReloadMetadataVersion;
    UINT hash = 0;
    UINT appliedGeneration = 0;

    TextureImageInfo sourceImage{};
    UINT gameRequestedWidth = kD3DXDefault;
    UINT gameRequestedHeight = kD3DXDefault;
    UINT gameRequestedMipLevels = kD3DXDefault;
    UINT gameRequestedFormat = static_cast<UINT>(D3DFMT_UNKNOWN);

    UINT mipLevels = kD3DXDefault;
    DWORD usage = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
    D3DPOOL pool = D3DPOOL_MANAGED;
    DWORD filter = kD3DXDefault;
    DWORD mipFilter = kD3DXDefault;
    D3DCOLOR colorKey = 0;
};

D3DXCreateTextureFromFileInMemoryFn g_originalCreateTextureFromMemory = nullptr;
D3DXCreateTextureFromFileInMemoryExFn g_originalCreateTextureFromMemoryEx = nullptr;
D3DXCreateTextureFromFileExWFn g_createTextureFromFileExW = nullptr;
D3DXSaveSurfaceToFileWFn g_saveSurfaceToFileW = nullptr;
D3DXGetImageInfoFromFileInMemoryFn g_getImageInfoFromMemory = nullptr;
D3DXGetImageInfoFromFileWFn g_getImageInfoFromFileW = nullptr;

std::atomic<unsigned long long> g_sourceLoads{ 0 };
std::atomic<unsigned long long> g_uniqueHashes{ 0 };
std::atomic<unsigned long long> g_overrideHits{ 0 };
std::atomic<unsigned long long> g_dumpedTextures{ 0 };
std::atomic<unsigned long long> g_dumpFailures{ 0 };
std::atomic<UINT> g_lastHash{ 0 };

std::atomic<UINT> g_hotReloadGeneration{ 0 };
std::atomic<unsigned long long> g_hotReloadRequests{ 0 };
std::atomic<unsigned long long> g_hotReloadAttempts{ 0 };
std::atomic<unsigned long long> g_hotReloadSuccesses{ 0 };
std::atomic<unsigned long long> g_hotReloadFailures{ 0 };
std::atomic<unsigned long long> g_hotReloadReverts{ 0 };
std::atomic<unsigned long long> g_hotReloadTrackedLoads{ 0 };

std::mutex g_registryMutex;
std::mutex g_hotReloadMutex;
std::unordered_set<UINT> g_seenHashes;
std::unordered_set<UINT> g_dumpAttemptedHashes;
std::unordered_set<UINT> g_loggedDimensionHashes;
std::unordered_set<UINT> g_loggedOverrideDimensionHashes;
std::unordered_set<UINT> g_loggedOverrideLoadFailureHashes;
TextureInspectionRecord g_lastObservedInspection{};
TextureInspectionRecord g_lastOverrideInspection{};

thread_local bool g_bypassTextureHooks = false;

// Captured once when D3DX hooks initialize. Developer Mode changes the texture
// ownership model, so it is intentionally restart-only for a coherent session.
bool g_textureDeveloperModeActive = false;

class TextureHookBypassScope
{
public:
    TextureHookBypassScope()
        : previous_(g_bypassTextureHooks)
    {
        g_bypassTextureHooks = true;
    }

    ~TextureHookBypassScope()
    {
        g_bypassTextureHooks = previous_;
    }

private:
    bool previous_;
};

/*
 * SuperFastHash compatibility implementation below is based on the algorithm by
 * Paul Hsieh, Copyright (c) 2010, Paul Hsieh. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the copyright notice, this list of
 * conditions and the disclaimer are retained. Neither Paul Hsieh's name nor the
 * names of contributors may be used to endorse derived products without prior
 * written permission. The software is provided "AS IS", without warranty; the
 * copyright holders are not liable for damages arising from its use.
 */
UINT ReadLE16(const char* data)
{
    const UINT lo = static_cast<unsigned char>(data[0]);
    const UINT hi = static_cast<unsigned char>(data[1]);
    return lo | (hi << 8);
}

// Paul Hsieh's SuperFastHash with the exact signed-byte tail behavior used by
// the original DPFix Hash.h. This detail matters for binary texture data.
UINT DPFixSuperFastHash(const void* sourceData, UINT sourceDataSize)
{
    if (sourceData == nullptr || sourceDataSize == 0)
        return 0;

    const char* data = static_cast<const char*>(sourceData);
    UINT hash = sourceDataSize;
    UINT temp = 0;
    UINT remaining = sourceDataSize & 3u;
    UINT blocks = sourceDataSize >> 2u;

    while (blocks-- != 0)
    {
        hash += ReadLE16(data);
        temp = (ReadLE16(data + 2) << 11u) ^ hash;
        hash = (hash << 16u) ^ temp;
        data += 4;
        hash += hash >> 11u;
    }

    switch (remaining)
    {
    case 3:
    {
        hash += ReadLE16(data);
        hash ^= hash << 16u;
        const auto signedTail = static_cast<std::int32_t>(
            static_cast<std::int8_t>(data[2]));
        const UINT signedTerm = static_cast<UINT>(signedTail * (1 << 18));
        hash ^= signedTerm;
        hash += hash >> 11u;
        break;
    }
    case 2:
        hash += ReadLE16(data);
        hash ^= hash << 11u;
        hash += hash >> 17u;
        break;
    case 1:
    {
        const auto signedTail = static_cast<std::int32_t>(
            static_cast<std::int8_t>(data[0]));
        hash += static_cast<UINT>(signedTail);
        hash ^= hash << 10u;
        hash += hash >> 1u;
        break;
    }
    default:
        break;
    }

    hash ^= hash << 3u;
    hash += hash >> 5u;
    hash ^= hash << 4u;
    hash += hash >> 17u;
    hash ^= hash << 25u;
    hash += hash >> 6u;

    return hash;
}

bool GetGameDirectory(wchar_t* path, size_t pathCount)
{
    if (path == nullptr || pathCount == 0)
        return false;

    const DWORD length = GetModuleFileNameW(
        nullptr,
        path,
        static_cast<DWORD>(pathCount));
    if (length == 0 || length >= pathCount)
        return false;

    wchar_t* slash = wcsrchr(path, L'\\');
    if (slash == nullptr)
        return false;

    *slash = L'\0';
    return true;
}

bool PathExists(const wchar_t* path)
{
    if (path == nullptr || path[0] == L'\0')
        return false;

    const DWORD attributes = GetFileAttributesW(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

bool EnsureDirectory(const wchar_t* path)
{
    if (CreateDirectoryW(path, nullptr) != FALSE)
        return true;

    return GetLastError() == ERROR_ALREADY_EXISTS;
}

bool EnsureTextureDirectories()
{
    wchar_t root[MAX_PATH] = {};
    if (!GetGameDirectory(root, MAX_PATH))
        return false;

    wchar_t path[MAX_PATH] = {};
    if (swprintf_s(path, L"%ls\\ZachFix", root) < 0 || !EnsureDirectory(path))
        return false;
    if (swprintf_s(path, L"%ls\\ZachFix\\textures", root) < 0 || !EnsureDirectory(path))
        return false;
    if (swprintf_s(path, L"%ls\\ZachFix\\textures\\dump", root) < 0 || !EnsureDirectory(path))
        return false;
    if (swprintf_s(path, L"%ls\\ZachFix\\textures\\override", root) < 0 || !EnsureDirectory(path))
        return false;

    return true;
}

bool BuildTexturePath(
    wchar_t* path,
    size_t pathCount,
    UINT hash,
    const wchar_t* relativeDirectory,
    const wchar_t* extension)
{
    wchar_t root[MAX_PATH] = {};
    if (!GetGameDirectory(root, MAX_PATH))
        return false;

    return swprintf_s(
        path,
        pathCount,
        L"%ls\\%ls\\%08x.%ls",
        root,
        relativeDirectory,
        hash,
        extension) >= 0;
}

bool FindOverrideTexture(
    UINT hash,
    wchar_t* path,
    size_t pathCount,
    TextureOverridePath& location)
{
    struct SearchEntry
    {
        const wchar_t* directory;
        const wchar_t* extension;
        TextureOverridePath location;
    };

    // Match original DPFix's extension preference (DDS before PNG).
    // Prefer the public ZachFix path. Private pre-release DPFixNG packs are
    // accepted as a migration fallback, followed by original DPFix packs.
    const SearchEntry entries[] =
    {
        { L"ZachFix\\textures\\override", L"dds", TextureOverridePath::ZachFix },
        { L"ZachFix\\textures\\override", L"png", TextureOverridePath::ZachFix },
        { L"DPFixNG\\textures\\override", L"dds", TextureOverridePath::PreReleaseDPFixNG },
        { L"DPFixNG\\textures\\override", L"png", TextureOverridePath::PreReleaseDPFixNG },
        { L"dpfix\\tex_override", L"dds", TextureOverridePath::LegacyDPFix },
        { L"dpfix\\tex_override", L"png", TextureOverridePath::LegacyDPFix }
    };

    for (const SearchEntry& entry : entries)
    {
        if (!BuildTexturePath(
                path,
                pathCount,
                hash,
                entry.directory,
                entry.extension))
        {
            continue;
        }

        if (PathExists(path))
        {
            location = entry.location;
            return true;
        }
    }

    location = TextureOverridePath::None;
    return false;
}

TextureImageInfo ToPublicImageInfo(const D3DXIMAGE_INFO& info)
{
    TextureImageInfo result{};
    result.valid = true;
    result.width = info.Width;
    result.height = info.Height;
    result.depth = info.Depth;
    result.mipLevels = info.MipLevels;
    result.format = static_cast<UINT>(info.Format);
    result.resourceType = static_cast<UINT>(info.ResourceType);
    result.fileFormat = static_cast<UINT>(info.ImageFileFormat);
    return result;
}

TextureImageInfo QueryMemoryImageInfo(const void* sourceData, UINT sourceDataSize)
{
    TextureImageInfo result{};
    if (g_getImageInfoFromMemory == nullptr ||
        sourceData == nullptr || sourceDataSize == 0 ||
        sourceDataSize == kDPFixUnknownSourceSize)
    {
        return result;
    }

    D3DXIMAGE_INFO info{};
    if (SUCCEEDED(g_getImageInfoFromMemory(sourceData, sourceDataSize, &info)))
        result = ToPublicImageInfo(info);

    return result;
}

TextureImageInfo QueryFileImageInfo(const wchar_t* path)
{
    TextureImageInfo result{};
    if (g_getImageInfoFromFileW == nullptr || path == nullptr || path[0] == L'\0')
        return result;

    D3DXIMAGE_INFO info{};
    if (SUCCEEDED(g_getImageInfoFromFileW(path, &info)))
        result = ToPublicImageInfo(info);

    return result;
}

TextureGpuInfo QueryGpuTextureInfo(IDirect3DTexture9* texture)
{
    TextureGpuInfo result{};
    if (texture == nullptr)
        return result;

    D3DSURFACE_DESC desc{};
    if (FAILED(texture->GetLevelDesc(0, &desc)))
        return result;

    result.valid = true;
    result.width = desc.Width;
    result.height = desc.Height;
    result.mipLevels = texture->GetLevelCount();
    result.format = static_cast<UINT>(desc.Format);
    return result;
}

bool ReadHotReloadMetadata(
    IDirect3DBaseTexture9* texture,
    TextureHotReloadMetadata& metadata)
{
    if (texture == nullptr)
        return false;

    DWORD size = sizeof(metadata);
    TextureHotReloadMetadata candidate{};
    const HRESULT result = texture->GetPrivateData(
        kHotReloadMetadataGuid,
        &candidate,
        &size);

    if (FAILED(result) || size != sizeof(candidate) ||
        candidate.magic != kHotReloadMetadataMagic ||
        candidate.version != kHotReloadMetadataVersion)
    {
        return false;
    }

    metadata = candidate;
    return true;
}

bool WriteHotReloadMetadata(
    IDirect3DBaseTexture9* texture,
    const TextureHotReloadMetadata& metadata)
{
    if (texture == nullptr)
        return false;

    return SUCCEEDED(texture->SetPrivateData(
        kHotReloadMetadataGuid,
        &metadata,
        sizeof(metadata),
        0));
}

IDirect3DTexture9* AcquireStoredHotReplacement(
    IDirect3DBaseTexture9* logicalTexture)
{
    if (logicalTexture == nullptr)
        return nullptr;

    IUnknown* stored = nullptr;
    DWORD size = sizeof(stored);
    const HRESULT result = logicalTexture->GetPrivateData(
        kHotReloadReplacementGuid,
        &stored,
        &size);

    if (FAILED(result) || stored == nullptr || size != sizeof(stored))
    {
        if (stored != nullptr)
            stored->Release();
        return nullptr;
    }

    // GetPrivateData AddRefs IUnknown values that were stored with
    // D3DSPD_IUNKNOWN, so this reference is ready for the caller to own.
    return static_cast<IDirect3DTexture9*>(stored);
}

bool StoreHotReplacement(
    IDirect3DBaseTexture9* logicalTexture,
    IDirect3DTexture9* replacement)
{
    if (logicalTexture == nullptr || replacement == nullptr)
        return false;

    // D3DSPD_IUNKNOWN expects pData itself to be the IUnknown interface
    // pointer. Passing the address of a local IUnknown* variable would make
    // D3D9 interpret that stack address as a COM object and call AddRef
    // through an invalid vtable.
    IUnknown* unknown = static_cast<IUnknown*>(replacement);
    return SUCCEEDED(logicalTexture->SetPrivateData(
        kHotReloadReplacementGuid,
        unknown,
        sizeof(IUnknown*),
        D3DSPD_IUNKNOWN));
}

bool HasStoredHotReplacement(IDirect3DBaseTexture9* logicalTexture)
{
    IDirect3DTexture9* replacement = AcquireStoredHotReplacement(logicalTexture);
    if (replacement == nullptr)
        return false;

    replacement->Release();
    return true;
}

bool ClearStoredHotReplacement(IDirect3DBaseTexture9* logicalTexture)
{
    if (logicalTexture == nullptr)
        return false;

    if (!HasStoredHotReplacement(logicalTexture))
        return true;

    return SUCCEEDED(logicalTexture->FreePrivateData(kHotReloadReplacementGuid));
}

void TrackTextureForHotReload(
    IDirect3DTexture9* texture,
    const TextureInspectionRecord& inspection,
    UINT mipLevels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    DWORD filter,
    DWORD mipFilter,
    D3DCOLOR colorKey)
{
    if (texture == nullptr)
        return;

    TextureHotReloadMetadata metadata{};
    metadata.hash = inspection.hash;
    metadata.appliedGeneration =
        g_hotReloadGeneration.load(std::memory_order_acquire);
    metadata.sourceImage = inspection.sourceImage;
    metadata.gameRequestedWidth = inspection.gameRequestedWidth;
    metadata.gameRequestedHeight = inspection.gameRequestedHeight;
    metadata.gameRequestedMipLevels = inspection.gameRequestedMipLevels;
    metadata.gameRequestedFormat = inspection.gameRequestedFormat;
    metadata.mipLevels = mipLevels;
    metadata.usage = usage;
    metadata.format = format;
    metadata.pool = pool;
    metadata.filter = filter;
    metadata.mipFilter = mipFilter;
    metadata.colorKey = colorKey;

    if (WriteHotReloadMetadata(texture, metadata))
    {
        g_hotReloadTrackedLoads.fetch_add(1, std::memory_order_relaxed);
    }
    else
    {
        char text[224] = {};
        sprintf_s(
            text,
            "[Textures] WARNING: Could not attach live-override metadata to %08x.\n",
            inspection.hash);
        AppendLog(text);
    }
}

const char* EntryPointName(TextureLoadEntryPoint entryPoint)
{
    switch (entryPoint)
    {
    case TextureLoadEntryPoint::InMemory: return "InMemory";
    case TextureLoadEntryPoint::InMemoryEx: return "InMemoryEx";
    default: return "Unknown";
    }
}

const char* OverridePathName(TextureOverridePath path)
{
    switch (path)
    {
    case TextureOverridePath::ZachFix: return "ZachFix";
    case TextureOverridePath::PreReleaseDPFixNG: return "pre-release DPFixNG";
    case TextureOverridePath::LegacyDPFix: return "legacy DPFix";
    default: return "none";
    }
}

const char* DimensionModeName(TextureDimensionMode mode)
{
    return mode == TextureDimensionMode::Preserve ? "Preserve" : "DPFix";
}

void LogInspectionOnce(const TextureInspectionRecord& record)
{
    bool shouldLog = false;
    bool shouldLogOverride = false;
    {
        std::lock_guard<std::mutex> lock(g_registryMutex);
        shouldLog = g_loggedDimensionHashes.insert(record.hash).second;
        if (record.usedOverride)
            shouldLogOverride = g_loggedOverrideDimensionHashes.insert(record.hash).second;
    }

    if (shouldLog)
    {
        char text[512] = {};
        sprintf_s(
            text,
            "[Textures] Inspect %08x [%s]: source=%s%ux%u mips=%u fmt=0x%08X, "
            "game-request=%08Xx%08X mips=%08X fmt=0x%08X, load-request=%08Xx%08X, "
            "loaded=%s%ux%u mips=%u fmt=0x%08X.\n",
            record.hash,
            EntryPointName(record.entryPoint),
            record.sourceImage.valid ? "" : "? ",
            record.sourceImage.width,
            record.sourceImage.height,
            record.sourceImage.mipLevels,
            record.sourceImage.format,
            record.gameRequestedWidth,
            record.gameRequestedHeight,
            record.gameRequestedMipLevels,
            record.gameRequestedFormat,
            record.loadRequestedWidth,
            record.loadRequestedHeight,
            record.loadedTexture.valid ? "" : "? ",
            record.loadedTexture.width,
            record.loadedTexture.height,
            record.loadedTexture.mipLevels,
            record.loadedTexture.format);
        AppendLog(text);
    }

    if (shouldLogOverride)
    {
        char text[384] = {};
        sprintf_s(
            text,
            "[Textures] Override inspect %08x (%s): file=%s%ux%u mips=%u fmt=0x%08X -> loaded=%s%ux%u.\n",
            record.hash,
            OverridePathName(record.overridePath),
            record.overrideImage.valid ? "" : "? ",
            record.overrideImage.width,
            record.overrideImage.height,
            record.overrideImage.mipLevels,
            record.overrideImage.format,
            record.loadedTexture.valid ? "" : "? ",
            record.loadedTexture.width,
            record.loadedTexture.height);
        AppendLog(text);

        if (record.overrideImage.valid && record.loadedTexture.valid &&
            (record.overrideImage.width != record.loadedTexture.width ||
             record.overrideImage.height != record.loadedTexture.height))
        {
            char warning[384] = {};
            sprintf_s(
                warning,
                "[Textures] WARNING: D3DX resized override %08x from %ux%u to %ux%u "
                "(load-request=%08Xx%08X).\n",
                record.hash,
                record.overrideImage.width,
                record.overrideImage.height,
                record.loadedTexture.width,
                record.loadedTexture.height,
                record.loadRequestedWidth,
                record.loadRequestedHeight);
            AppendLog(warning);
        }
    }
}

void PublishInspection(TextureInspectionRecord record)
{
    record.valid = true;
    {
        std::lock_guard<std::mutex> lock(g_registryMutex);
        g_lastObservedInspection = record;
        if (record.usedOverride)
            g_lastOverrideInspection = record;
    }

    LogInspectionOnce(record);
}

bool RefreshHotReloadTexture(
    IDirect3DDevice9* device,
    IDirect3DBaseTexture9* logicalTexture,
    TextureHotReloadMetadata& metadata,
    UINT generation)
{
    g_hotReloadAttempts.fetch_add(1, std::memory_order_relaxed);

    auto commitGeneration = [&]()
    {
        metadata.appliedGeneration = generation;
        if (!WriteHotReloadMetadata(logicalTexture, metadata))
        {
            char text[256] = {};
            sprintf_s(
                text,
                "[Textures] WARNING: Hot reload %u could not update metadata for %08x.\n",
                generation,
                metadata.hash);
            AppendLog(text);
        }
    };

    // Missing/disabled override is a normal state for the live texture-maker
    // workflow. If this texture acquired a hot replacement in an earlier
    // generation, drop that private-data COM reference so SetTexture falls
    // back to the baseline resource returned when D3DX originally loaded it.
    wchar_t overridePath[MAX_PATH] = {};
    TextureOverridePath location = TextureOverridePath::None;
    const bool overrideAvailable =
        g_config.enableTextureOverride &&
        g_createTextureFromFileExW != nullptr &&
        FindOverrideTexture(metadata.hash, overridePath, MAX_PATH, location);

    if (!overrideAvailable)
    {
        const bool hadReplacement = HasStoredHotReplacement(logicalTexture);
        if (hadReplacement)
        {
            if (!ClearStoredHotReplacement(logicalTexture))
            {
                g_hotReloadFailures.fetch_add(1, std::memory_order_relaxed);
                commitGeneration();

                char text[288] = {};
                sprintf_s(
                    text,
                    "[Textures] WARNING: Hot reload %u could not clear replacement %08x; "
                    "keeping the previous working texture.\n",
                    generation,
                    metadata.hash);
                AppendLog(text);
                return false;
            }

            g_hotReloadReverts.fetch_add(1, std::memory_order_relaxed);

            char text[320] = {};
            sprintf_s(
                text,
                "[Textures] Hot reload %u reverted %08x to baseline (%s).\n",
                generation,
                metadata.hash,
                "original game texture");
            AppendLog(text);
        }

        commitGeneration();
        return hadReplacement;
    }

    const bool hadReplacement = HasStoredHotReplacement(logicalTexture);
    const TextureImageInfo overrideImage = QueryFileImageInfo(overridePath);
    const TextureDimensionMode dimensionMode = g_config.textureDimensionMode;
    const UINT dimensionRequest =
        dimensionMode == TextureDimensionMode::Preserve
            ? kD3DXDefaultNonPow2
            : kD3DXDefault;

    IDirect3DTexture9* replacement = nullptr;
    HRESULT result = E_FAIL;
    {
        TextureHookBypassScope bypass;
        result = g_createTextureFromFileExW(
            device,
            overridePath,
            dimensionRequest,
            dimensionRequest,
            metadata.mipLevels,
            metadata.usage,
            metadata.format,
            metadata.pool,
            metadata.filter,
            metadata.mipFilter,
            metadata.colorKey,
            nullptr,
            nullptr,
            &replacement);
    }

    if (FAILED(result) || replacement == nullptr)
    {
        if (replacement != nullptr)
            replacement->Release();

        g_hotReloadFailures.fetch_add(1, std::memory_order_relaxed);
        commitGeneration();

        char text[320] = {};
        sprintf_s(
            text,
            "[Textures] WARNING: Hot reload %u failed for %08x "
            "(HRESULT 0x%08X). Keeping the previous working texture.\n",
            generation,
            metadata.hash,
            static_cast<unsigned>(result));
        AppendLog(text);
        return false;
    }

    // SetPrivateData with D3DSPD_IUNKNOWN AddRefs the candidate and releases
    // the previous candidate stored under the same GUID. This gives us an
    // atomic lifetime hand-off tied to the game's baseline texture object.
    if (!StoreHotReplacement(logicalTexture, replacement))
    {
        replacement->Release();
        g_hotReloadFailures.fetch_add(1, std::memory_order_relaxed);
        commitGeneration();

        char text[288] = {};
        sprintf_s(
            text,
            "[Textures] WARNING: Hot reload %u could not attach replacement %08x; "
            "keeping the previous working texture.\n",
            generation,
            metadata.hash);
        AppendLog(text);
        return false;
    }

    TextureInspectionRecord inspection{};
    inspection.hash = metadata.hash;
    inspection.entryPoint = TextureLoadEntryPoint::InMemoryEx;
    inspection.sourceImage = metadata.sourceImage;
    inspection.gameRequestedWidth = metadata.gameRequestedWidth;
    inspection.gameRequestedHeight = metadata.gameRequestedHeight;
    inspection.gameRequestedMipLevels = metadata.gameRequestedMipLevels;
    inspection.gameRequestedFormat = metadata.gameRequestedFormat;
    inspection.loadRequestedWidth = dimensionRequest;
    inspection.loadRequestedHeight = dimensionRequest;
    inspection.loadRequestedMipLevels = metadata.mipLevels;
    inspection.loadRequestedFormat = static_cast<UINT>(metadata.format);
    inspection.usedOverride = true;
    inspection.overridePath = location;
    inspection.overrideImage = overrideImage;
    inspection.loadedTexture = QueryGpuTextureInfo(replacement);
    PublishInspection(inspection);

    commitGeneration();
    g_hotReloadSuccesses.fetch_add(1, std::memory_order_relaxed);

    char text[352] = {};
    sprintf_s(
        text,
        "[Textures] Hot reload %u %s %08x (%s, dimensions=%s): %ux%u -> GPU %ux%u.\n",
        generation,
        hadReplacement ? "refreshed" : "activated",
        metadata.hash,
        OverridePathName(location),
        DimensionModeName(dimensionMode),
        overrideImage.width,
        overrideImage.height,
        inspection.loadedTexture.width,
        inspection.loadedTexture.height);
    AppendLog(text);

    // The private-data slot now owns its reference.
    replacement->Release();
    return true;
}

void RegisterSourceHash(UINT hash)
{
    g_sourceLoads.fetch_add(1, std::memory_order_relaxed);
    g_lastHash.store(hash, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lock(g_registryMutex);
    if (g_seenHashes.insert(hash).second)
        g_uniqueHashes.fetch_add(1, std::memory_order_relaxed);
}

void DumpTextureIfRequested(UINT hash, IDirect3DTexture9* texture, bool usedOverride)
{
    if (!g_textureDeveloperModeActive || !g_config.dumpTextures || texture == nullptr)
        return;

    // In Developer Mode the logical texture is always the original game
    // resource, so dumping remains useful even when an override is active.
    (void)usedOverride;

    bool shouldAttempt = false;
    {
        std::lock_guard<std::mutex> lock(g_registryMutex);
        shouldAttempt = g_dumpAttemptedHashes.insert(hash).second;
    }

    if (!shouldAttempt)
        return;

    wchar_t path[MAX_PATH] = {};
    if (!BuildTexturePath(
            path,
            MAX_PATH,
            hash,
            L"ZachFix\\textures\\dump",
            L"tga"))
    {
        g_dumpFailures.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    if (PathExists(path))
        return;

    if (!EnsureTextureDirectories() || g_saveSurfaceToFileW == nullptr)
    {
        g_dumpFailures.fetch_add(1, std::memory_order_relaxed);
        return;
    }

    IDirect3DSurface9* surface = nullptr;
    HRESULT result = texture->GetSurfaceLevel(0, &surface);
    if (SUCCEEDED(result) && surface != nullptr)
    {
        result = g_saveSurfaceToFileW(
            path,
            kD3DXImageFileFormatTga,
            surface,
            nullptr,
            nullptr);
        surface->Release();
    }
    else if (SUCCEEDED(result))
    {
        result = E_FAIL;
    }

    if (SUCCEEDED(result))
    {
        g_dumpedTextures.fetch_add(1, std::memory_order_relaxed);

        char text[160] = {};
        sprintf_s(
            text,
            "[Textures] Dumped %08x.tga (DPFix-compatible hash).\n",
            hash);
        AppendLog(text);
    }
    else
    {
        g_dumpFailures.fetch_add(1, std::memory_order_relaxed);

        char text[192] = {};
        sprintf_s(
            text,
            "[Textures] WARNING: D3DXSaveSurfaceToFile failed for %08x (HRESULT 0x%08X).\n",
            hash,
            static_cast<unsigned>(result));
        AppendLog(text);
    }
}

HRESULT LoadExtendedOverride(
    IDirect3DDevice9* device,
    UINT hash,
    TextureDimensionMode dimensionMode,
    UINT mipLevels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    DWORD filter,
    DWORD mipFilter,
    D3DCOLOR colorKey,
    D3DXIMAGE_INFO* sourceInfo,
    PALETTEENTRY* palette,
    IDirect3DTexture9** texture,
    bool& usedOverride,
    TextureOverridePath& overrideLocation,
    TextureImageInfo& overrideImage)
{
    usedOverride = false;
    overrideLocation = TextureOverridePath::None;
    overrideImage = {};

    if (!g_config.enableTextureOverride || g_createTextureFromFileExW == nullptr)
        return E_FAIL;

    wchar_t overridePath[MAX_PATH] = {};
    TextureOverridePath location = TextureOverridePath::None;
    if (!FindOverrideTexture(hash, overridePath, MAX_PATH, location))
        return E_FAIL;

    if (g_textureDeveloperModeActive)
        overrideImage = QueryFileImageInfo(overridePath);

    const UINT dimensionRequest =
        dimensionMode == TextureDimensionMode::Preserve
            ? kD3DXDefaultNonPow2
            : kD3DXDefault;

    TextureHookBypassScope bypass;
    const HRESULT result = g_createTextureFromFileExW(
        device,
        overridePath,
        dimensionRequest,
        dimensionRequest,
        mipLevels,
        usage,
        format,
        pool,
        filter,
        mipFilter,
        colorKey,
        sourceInfo,
        palette,
        texture);

    if (SUCCEEDED(result))
    {
        usedOverride = true;
        overrideLocation = location;
        g_overrideHits.fetch_add(1, std::memory_order_relaxed);

        char text[192] = {};
        sprintf_s(
            text,
            "[Textures] Override hit %08x (%s path, dimensions=%s).\n",
            hash,
            OverridePathName(location),
            DimensionModeName(dimensionMode));
        AppendLog(text);
    }
    else if (dimensionMode == TextureDimensionMode::Preserve)
    {
        bool shouldLogFailure = false;
        {
            std::lock_guard<std::mutex> lock(g_registryMutex);
            shouldLogFailure = g_loggedOverrideLoadFailureHashes.insert(hash).second;
        }

        if (shouldLogFailure)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[Textures] WARNING: Preserve-mode override load failed for %08x "
                "(HRESULT 0x%08X). The original game texture will be used.\n",
                hash,
                static_cast<unsigned>(result));
            AppendLog(text);
        }
    }

    return result;
}

HRESULT WINAPI HookD3DXCreateTextureFromFileInMemory(
    IDirect3DDevice9* device,
    const void* sourceData,
    UINT sourceDataSize,
    IDirect3DTexture9** texture)
{
    if (g_bypassTextureHooks || !g_textureDeveloperModeActive)
        return g_originalCreateTextureFromMemory(device, sourceData, sourceDataSize, texture);

    // Original DPFix uses 256 bytes for this sentinel in the non-Ex path.
    const UINT hashSize =
        sourceDataSize == kDPFixUnknownSourceSize ? 256u : sourceDataSize;
    const UINT hash = DPFixSuperFastHash(sourceData, hashSize);
    RegisterSourceHash(hash);

    // The non-Ex entry point is never overridden by original DPFix. Outside
    // Developer Mode, keep this path as close to passthrough as possible.
    const HRESULT result = g_originalCreateTextureFromMemory(
        device,
        sourceData,
        sourceDataSize,
        texture);

    if (!g_textureDeveloperModeActive ||
        FAILED(result) || texture == nullptr || *texture == nullptr)
    {
        return result;
    }

    TextureInspectionRecord inspection{};
    inspection.hash = hash;
    inspection.entryPoint = TextureLoadEntryPoint::InMemory;
    inspection.sourceImage = QueryMemoryImageInfo(sourceData, sourceDataSize);
    inspection.loadedTexture = QueryGpuTextureInfo(*texture);
    PublishInspection(inspection);
    DumpTextureIfRequested(hash, *texture, false);
    return result;
}

HRESULT WINAPI HookD3DXCreateTextureFromFileInMemoryEx(
    IDirect3DDevice9* device,
    const void* sourceData,
    UINT sourceDataSize,
    UINT width,
    UINT height,
    UINT mipLevels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    DWORD filter,
    DWORD mipFilter,
    D3DCOLOR colorKey,
    D3DXIMAGE_INFO* sourceInfo,
    PALETTEENTRY* palette,
    IDirect3DTexture9** texture)
{
    if (g_bypassTextureHooks ||
        (!g_textureDeveloperModeActive && !g_config.enableTextureOverride))
    {
        return g_originalCreateTextureFromMemoryEx(
            device,
            sourceData,
            sourceDataSize,
            width,
            height,
            mipLevels,
            usage,
            format,
            pool,
            filter,
            mipFilter,
            colorKey,
            sourceInfo,
            palette,
            texture);
    }

    // This odd fallback is part of DPFix's historical texture-hash contract.
    const UINT hashSize =
        sourceDataSize == kDPFixUnknownSourceSize
            ? (width * height) / 2u
            : sourceDataSize;
    const UINT hash = DPFixSuperFastHash(sourceData, hashSize);
    RegisterSourceHash(hash);

    const TextureDimensionMode dimensionMode = g_config.textureDimensionMode;

    // Production mode deliberately preserves the old DPFix ownership model:
    // an override, when present, is the texture object returned to the game.
    // No private data, baseline retention, live-rescan state, or SetTexture
    // replacement chain is created in this mode.
    if (!g_textureDeveloperModeActive)
    {
        bool usedOverride = false;
        TextureOverridePath overrideLocation = TextureOverridePath::None;
        TextureImageInfo unusedOverrideImage{};
        HRESULT result = LoadExtendedOverride(
            device,
            hash,
            dimensionMode,
            mipLevels,
            usage,
            format,
            pool,
            filter,
            mipFilter,
            colorKey,
            sourceInfo,
            palette,
            texture,
            usedOverride,
            overrideLocation,
            unusedOverrideImage);

        if (FAILED(result))
        {
            result = g_originalCreateTextureFromMemoryEx(
                device,
                sourceData,
                sourceDataSize,
                width,
                height,
                mipLevels,
                usage,
                format,
                pool,
                filter,
                mipFilter,
                colorKey,
                sourceInfo,
                palette,
                texture);
        }

        return result;
    }

    // Developer Mode uses a stable logical-original model. The texture object
    // returned to Deadly Premonition is always the original game texture.
    // Overrides live in D3D9 private data and are substituted only at SetTexture.
    // This keeps a real original baseline available for ADD / EDIT / DELETE.
    HRESULT result = g_originalCreateTextureFromMemoryEx(
        device,
        sourceData,
        sourceDataSize,
        width,
        height,
        mipLevels,
        usage,
        format,
        pool,
        filter,
        mipFilter,
        colorKey,
        sourceInfo,
        palette,
        texture);

    if (FAILED(result) || texture == nullptr || *texture == nullptr)
        return result;

    IDirect3DTexture9* logicalOriginal = *texture;

    TextureInspectionRecord inspection{};
    inspection.hash = hash;
    inspection.entryPoint = TextureLoadEntryPoint::InMemoryEx;
    inspection.sourceImage = QueryMemoryImageInfo(sourceData, sourceDataSize);
    inspection.gameRequestedWidth = width;
    inspection.gameRequestedHeight = height;
    inspection.gameRequestedMipLevels = mipLevels;
    inspection.gameRequestedFormat = static_cast<UINT>(format);
    inspection.loadRequestedWidth = width;
    inspection.loadRequestedHeight = height;
    inspection.loadRequestedMipLevels = mipLevels;
    inspection.loadRequestedFormat = static_cast<UINT>(format);
    inspection.loadedTexture = QueryGpuTextureInfo(logicalOriginal);

    TrackTextureForHotReload(
        logicalOriginal,
        inspection,
        mipLevels,
        usage,
        format,
        pool,
        filter,
        mipFilter,
        colorKey);

    // Keep dumping tied to the real baseline, even when an override already
    // exists at startup. This is exactly what a texture-maker expects.
    DumpTextureIfRequested(hash, logicalOriginal, false);

    IDirect3DTexture9* replacement = nullptr;
    bool usedOverride = false;
    TextureOverridePath overrideLocation = TextureOverridePath::None;
    TextureImageInfo overrideImage{};
    const HRESULT overrideResult = LoadExtendedOverride(
        device,
        hash,
        dimensionMode,
        mipLevels,
        usage,
        format,
        pool,
        filter,
        mipFilter,
        colorKey,
        nullptr,
        nullptr,
        &replacement,
        usedOverride,
        overrideLocation,
        overrideImage);

    if (SUCCEEDED(overrideResult) && usedOverride && replacement != nullptr)
    {
        if (StoreHotReplacement(logicalOriginal, replacement))
        {
            inspection.usedOverride = true;
            inspection.overridePath = overrideLocation;
            inspection.overrideImage = overrideImage;
            const UINT dimensionRequest =
                dimensionMode == TextureDimensionMode::Preserve
                    ? kD3DXDefaultNonPow2
                    : kD3DXDefault;
            inspection.loadRequestedWidth = dimensionRequest;
            inspection.loadRequestedHeight = dimensionRequest;
            inspection.loadedTexture = QueryGpuTextureInfo(replacement);
        }
        else
        {
            char text[288] = {};
            sprintf_s(
                text,
                "[Textures] WARNING: Developer Mode could not attach initial override %08x; "
                "using the original game texture.\n",
                hash);
            AppendLog(text);
        }

        replacement->Release();
    }
    else if (replacement != nullptr)
    {
        replacement->Release();
    }

    PublishInspection(inspection);
    return result;
}

bool InstallOneHook(void* target, void* detour, void** original, const char* name)
{
    if (target == nullptr)
    {
        char text[192] = {};
        sprintf_s(text, "[Textures] WARNING: %s export not found.\n", name);
        AppendLog(text);
        return false;
    }

    MH_STATUS status = MH_CreateHook(target, detour, original);
    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(text, "[Textures] WARNING: MH_CreateHook failed for %s (%d).\n", name, status);
        AppendLog(text);
        return false;
    }

    status = MH_EnableHook(target);
    if (status != MH_OK)
    {
        char text[192] = {};
        sprintf_s(text, "[Textures] WARNING: MH_EnableHook failed for %s (%d).\n", name, status);
        AppendLog(text);
        return false;
    }

    return true;
}
} // namespace

bool InstallTextureOverrideHooks()
{
    g_textureDeveloperModeActive = g_config.textureDeveloperMode;

    HMODULE d3dx9 = GetModuleHandleW(L"d3dx9_43.dll");
    if (d3dx9 == nullptr)
    {
        // DP depends on this legacy runtime. Taking our own reference also makes
        // hook installation deterministic when the ASI loader runs very early.
        d3dx9 = LoadLibraryW(L"d3dx9_43.dll");
    }

    if (d3dx9 == nullptr)
    {
        AppendLog("[Textures] WARNING: d3dx9_43.dll not found; texture override disabled.\n");
        return false;
    }

    void* simpleTarget = reinterpret_cast<void*>(
        GetProcAddress(d3dx9, "D3DXCreateTextureFromFileInMemory"));
    void* extendedTarget = reinterpret_cast<void*>(
        GetProcAddress(d3dx9, "D3DXCreateTextureFromFileInMemoryEx"));

    g_createTextureFromFileExW = reinterpret_cast<D3DXCreateTextureFromFileExWFn>(
        GetProcAddress(d3dx9, "D3DXCreateTextureFromFileExW"));
    g_saveSurfaceToFileW = reinterpret_cast<D3DXSaveSurfaceToFileWFn>(
        GetProcAddress(d3dx9, "D3DXSaveSurfaceToFileW"));
    g_getImageInfoFromMemory = reinterpret_cast<D3DXGetImageInfoFromFileInMemoryFn>(
        GetProcAddress(d3dx9, "D3DXGetImageInfoFromFileInMemory"));
    g_getImageInfoFromFileW = reinterpret_cast<D3DXGetImageInfoFromFileWFn>(
        GetProcAddress(d3dx9, "D3DXGetImageInfoFromFileW"));

    const bool simpleInstalled = InstallOneHook(
        simpleTarget,
        reinterpret_cast<void*>(&HookD3DXCreateTextureFromFileInMemory),
        reinterpret_cast<void**>(&g_originalCreateTextureFromMemory),
        "D3DXCreateTextureFromFileInMemory");

    const bool extendedInstalled = InstallOneHook(
        extendedTarget,
        reinterpret_cast<void*>(&HookD3DXCreateTextureFromFileInMemoryEx),
        reinterpret_cast<void**>(&g_originalCreateTextureFromMemoryEx),
        "D3DXCreateTextureFromFileInMemoryEx");

    if (!simpleInstalled && !extendedInstalled)
    {
        AppendLog("[Textures] WARNING: No D3DX texture hooks installed.\n");
        return false;
    }

    EnsureTextureDirectories();

    AppendLog(
        "[Textures] DPFix-compatible texture hashing active (SuperFastHash over original D3DX source bytes).\n");
    AppendLog(
        "[Textures] Override lookup: ZachFix\\textures\\override, pre-release DPFixNG\\textures\\override, then legacy dpfix\\tex_override.\n");
    AppendLog(
        "[Textures] Dimension modes: DPFix uses D3DX_DEFAULT (POT rounding); Preserve uses "
        "D3DX_DEFAULT_NONPOW2 (exact file dimensions when supported).\n");
    if (g_textureDeveloperModeActive)
    {
        AppendLog(
            "[Textures] Developer Mode ACTIVE: logical textures stay original; add/edit/remove overrides are applied lazily on SetTexture.\n");
    }
    else
    {
        AppendLog(
            "[Textures] Developer Mode OFF: production override path only; no live tracking, private-data replacements, baseline retention, or hot reload.\n");
    }

    if (g_saveSurfaceToFileW == nullptr)
    {
        AppendLog(
            "[Textures] WARNING: D3DXSaveSurfaceToFileW not found; texture dumping unavailable.\n");
    }

    if (g_textureDeveloperModeActive)
    {
        if (g_getImageInfoFromMemory == nullptr || g_getImageInfoFromFileW == nullptr)
        {
            AppendLog(
                "[Textures] WARNING: D3DX image-info helpers incomplete; Texture Inspector source dimensions may be unavailable.\n");
        }
        else
        {
            AppendLog("[Textures] Texture Inspector dimension tracking active.\n");
        }
    }

    return true;
}

TextureOverrideStats GetTextureOverrideStats()
{
    TextureOverrideStats stats{};
    stats.sourceLoads = g_sourceLoads.load(std::memory_order_acquire);
    stats.uniqueHashes = g_uniqueHashes.load(std::memory_order_acquire);
    stats.overrideHits = g_overrideHits.load(std::memory_order_acquire);
    stats.dumpedTextures = g_dumpedTextures.load(std::memory_order_acquire);
    stats.dumpFailures = g_dumpFailures.load(std::memory_order_acquire);
    stats.lastHash = g_lastHash.load(std::memory_order_acquire);
    stats.hotReloadGeneration = g_hotReloadGeneration.load(std::memory_order_acquire);
    stats.hotReloadRequests = g_hotReloadRequests.load(std::memory_order_acquire);
    stats.hotReloadAttempts = g_hotReloadAttempts.load(std::memory_order_acquire);
    stats.hotReloadSuccesses = g_hotReloadSuccesses.load(std::memory_order_acquire);
    stats.hotReloadFailures = g_hotReloadFailures.load(std::memory_order_acquire);
    stats.hotReloadReverts = g_hotReloadReverts.load(std::memory_order_acquire);
    stats.hotReloadTrackedLoads = g_hotReloadTrackedLoads.load(std::memory_order_acquire);
    return stats;
}

bool IsTextureDeveloperModeActive()
{
    return g_textureDeveloperModeActive;
}

UINT RequestTextureOverrideHotReload()
{
    if (!g_textureDeveloperModeActive)
    {
        AppendLog("[Textures] Hot reload ignored: Developer Mode is not active for this session.\n");
        return 0;
    }

    // Reset per-generation counters before publishing the new generation so a
    // multithreaded SetTexture cannot refresh into counters that are then zeroed.
    g_hotReloadAttempts.store(0, std::memory_order_release);
    g_hotReloadSuccesses.store(0, std::memory_order_release);
    g_hotReloadFailures.store(0, std::memory_order_release);
    g_hotReloadReverts.store(0, std::memory_order_release);

    const UINT generation =
        g_hotReloadGeneration.fetch_add(1, std::memory_order_acq_rel) + 1u;
    g_hotReloadRequests.fetch_add(1, std::memory_order_relaxed);

    char text[224] = {};
    sprintf_s(
        text,
        "[Textures] Hot reload generation %u requested; tracked textures rescan overrides on next SetTexture.\n",
        generation);
    AppendLog(text);
    return generation;
}

IDirect3DTexture9* AcquireTextureOverrideHotReplacement(
    IDirect3DDevice9* device,
    IDirect3DBaseTexture9* logicalTexture)
{
    if (!g_textureDeveloperModeActive || device == nullptr || logicalTexture == nullptr)
        return nullptr;

    const UINT generation = g_hotReloadGeneration.load(std::memory_order_acquire);

    // Only regular 2D textures created by the tracked D3DX InMemoryEx path carry
    // this metadata. Avoid treating cube/volume textures as IDirect3DTexture9 objects.
    if (logicalTexture->GetType() != D3DRTYPE_TEXTURE)
        return nullptr;

    TextureHotReloadMetadata metadata{};
    if (!ReadHotReloadMetadata(logicalTexture, metadata))
        return nullptr;

    if (generation != 0 && metadata.appliedGeneration != generation)
    {
        // DP creates the D3D9 device with multithreaded support. Serialize only
        // generation transitions so two SetTexture calls cannot rebuild the
        // same logical texture concurrently. Normal binds do not take this lock.
        std::lock_guard<std::mutex> reloadLock(g_hotReloadMutex);

        // Another render thread may have completed this generation while we
        // waited for the lock, so re-read the copied metadata before rebuilding.
        if (!ReadHotReloadMetadata(logicalTexture, metadata))
            return nullptr;

        if (metadata.appliedGeneration != generation)
        {
            RefreshHotReloadTexture(
                device,
                logicalTexture,
                metadata,
                generation);
        }
    }

    return AcquireStoredHotReplacement(logicalTexture);
}

TextureInspectorSnapshot GetTextureInspectorSnapshot()
{
    std::lock_guard<std::mutex> lock(g_registryMutex);
    TextureInspectorSnapshot snapshot{};
    snapshot.lastObserved = g_lastObservedInspection;
    snapshot.lastOverride = g_lastOverrideInspection;
    return snapshot;
}
