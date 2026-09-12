#pragma once

#include <Windows.h>
#include <d3d9.h>

struct TextureOverrideStats
{
    unsigned long long sourceLoads = 0;
    unsigned long long uniqueHashes = 0;
    unsigned long long overrideHits = 0;
    unsigned long long dumpedTextures = 0;
    unsigned long long dumpFailures = 0;
    UINT lastHash = 0;

    // Hot reload is generation based. A reload request increments the
    // generation; each tracked D3DX texture rescans lazily the next time it is
    // submitted through SetTexture. This can activate a newly-added override.
    UINT hotReloadGeneration = 0;
    unsigned long long hotReloadRequests = 0;
    unsigned long long hotReloadAttempts = 0;
    unsigned long long hotReloadSuccesses = 0;
    unsigned long long hotReloadFailures = 0;
    unsigned long long hotReloadReverts = 0;
    unsigned long long hotReloadTrackedLoads = 0;
};

enum class TextureLoadEntryPoint : UINT
{
    None = 0,
    InMemory,
    InMemoryEx
};

enum class TextureOverridePath : UINT
{
    None = 0,
    DPFixNG,
    LegacyDPFix
};

struct TextureImageInfo
{
    bool valid = false;
    UINT width = 0;
    UINT height = 0;
    UINT depth = 0;
    UINT mipLevels = 0;
    UINT format = 0;
    UINT resourceType = 0;
    UINT fileFormat = 0;
};

struct TextureGpuInfo
{
    bool valid = false;
    UINT width = 0;
    UINT height = 0;
    UINT mipLevels = 0;
    UINT format = 0;
};

struct TextureInspectionRecord
{
    bool valid = false;
    UINT hash = 0;
    TextureLoadEntryPoint entryPoint = TextureLoadEntryPoint::None;

    TextureImageInfo sourceImage{};

    // Request made by Deadly Premonition to D3DXCreateTextureFromFileInMemoryEx.
    // InMemory implicitly uses D3DX defaults.
    UINT gameRequestedWidth = 0xffffffffu;
    UINT gameRequestedHeight = 0xffffffffu;
    UINT gameRequestedMipLevels = 0xffffffffu;
    UINT gameRequestedFormat = 0xffffffffu;

    // Request that actually created the texture returned to the game. For a
    // DPFix-compatible override, width/height are deliberately D3DX_DEFAULT.
    UINT loadRequestedWidth = 0xffffffffu;
    UINT loadRequestedHeight = 0xffffffffu;
    UINT loadRequestedMipLevels = 0xffffffffu;
    UINT loadRequestedFormat = 0xffffffffu;

    bool usedOverride = false;
    TextureOverridePath overridePath = TextureOverridePath::None;
    TextureImageInfo overrideImage{};

    // The texture that was actually returned to Deadly Premonition, or the
    // newest hot-reload replacement when this inspection came from a reload.
    TextureGpuInfo loadedTexture{};
};

struct TextureInspectorSnapshot
{
    TextureInspectionRecord lastObserved{};
    TextureInspectionRecord lastOverride{};
};

// Hooks the D3DX9 texture-from-memory entry points used by Deadly Premonition.
// The resulting hash is intentionally compatible with the original DPFix.
bool InstallTextureOverrideHooks();
TextureOverrideStats GetTextureOverrideStats();
TextureInspectorSnapshot GetTextureInspectorSnapshot();

// Developer mode is captured when texture hooks initialize. Changing the INI
// value at runtime intentionally does not switch ownership models mid-session.
bool IsTextureDeveloperModeActive();

// Starts a new lazy hot-reload/rescan generation in Developer Mode. Every
// tracked InMemoryEx texture checks the override directories on its next
// SetTexture call, so add/edit/remove changes can be previewed live. Returns 0
// when Developer Mode is not active for the current process.
UINT RequestTextureOverrideHotReload();

// Returns an AddRef'd hot-reload replacement for a logical texture when one is
// active. The caller must Release() it after the D3D9 SetTexture call.
IDirect3DTexture9* AcquireTextureOverrideHotReplacement(
    IDirect3DDevice9* device,
    IDirect3DBaseTexture9* logicalTexture);
