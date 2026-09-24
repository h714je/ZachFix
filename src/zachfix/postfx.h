#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <cstddef>
#include <cstdint>

// Shared post-processing infrastructure for ZachFix-native effects.
//
// The framework intentionally owns only ZachFix-created resources. Game-owned
// G-buffer/HDR textures stay tracked by the renderer-specific hooks and are
// passed into effects as borrowed inputs.

enum class PostFxBlendMode : UINT
{
    Opaque = 0,
    Multiply,
    Additive
};

enum class PostFxTargetSlot : UINT
{
    HdrPing = 0,
    HdrPong,
    AoRaw,
    AoFiltered,
    DoFNear,
    DoFFar,
    Bloom0,
    Bloom1,
    Bloom2,
    Bloom3,
    Bloom4,
    Bloom5,
    ExposureMeter0,
    ExposureMeter1,
    ExposureMeter2,
    ExposureMeter3,
    ExposureMeter4,
    ExposureAdapt0,
    ExposureAdapt1,
    DoFFreezeDiffuse,
    DoFFreezeBloom,
    DoFFreezeLuminance,
    DoFFreezeGaussian,
    DoFFreezeDepth,
    DoFFreezeNormal,
    DisplayGammaScratch,
    Count
};

struct PostFxTargetView
{
    // Borrowed pointers. Valid until the slot is recreated/released or the
    // D3D9 device is reset. Callers must not Release() them.
    IDirect3DTexture9* texture = nullptr;
    IDirect3DSurface9* surface = nullptr;
    UINT width = 0;
    UINT height = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;
};

struct PostFxTextureBinding
{
    UINT stage = 0;
    IDirect3DBaseTexture9* texture = nullptr;
    D3DTEXTUREFILTERTYPE filter = D3DTEXF_LINEAR;
    D3DTEXTUREADDRESS addressMode = D3DTADDRESS_CLAMP;
};

struct PostFxStateBackup
{
    IDirect3DStateBlock9* stateBlock = nullptr;
    IDirect3DSurface9* renderTargets[4] = {};
    IDirect3DSurface9* depthStencil = nullptr;
    IDirect3DBaseTexture9* textures[16] = {};
    unsigned int textureStageMask = 0;
    D3DVIEWPORT9 viewport = {};
    bool haveViewport = false;
    bool active = false;
};

enum class PostFxDepthEncoding : UINT
{
    Packed = 0,
    NativeD24,
    LinearView
};

struct PostFxDepthView
{
    // AddRef-owned depth source. NativeD24 is the INTZ main depth captured
    // during the proven geometry G-buffer pass; LinearView is an R32F snapshot.
    IDirect3DTexture9* texture = nullptr;
    PostFxDepthEncoding encoding = PostFxDepthEncoding::Packed;
    UINT width = 0;
    UINT height = 0;
    float projectionDepthQ = 0.0f;
    float projectionDepthQn = 0.0f;
    unsigned long long frameIndex = 0;
    bool fresh = false;
};

struct PostFxGBufferView
{
    // AddRef-owned snapshot returned by AcquirePostFxGBuffer(). Release it with
    // ReleasePostFxGBuffer() when the effect has finished issuing its passes.
    IDirect3DTexture9* depth = nullptr;
    IDirect3DTexture9* normal = nullptr;
    PostFxDepthEncoding depthEncoding = PostFxDepthEncoding::Packed;
    UINT width = 0;
    UINT height = 0;
    float projectionDepthQ = 0.0f;
    float projectionDepthQn = 0.0f;
    unsigned long long frameIndex = 0;
    bool fresh = false;
};

struct PostFxStats
{
    bool deviceReady = false;
    UINT pixelShaderMajor = 0;
    UINT pixelShaderMinor = 0;
    UINT maxSimultaneousRenderTargets = 0;
    UINT maxTextureWidth = 0;
    UINT maxTextureHeight = 0;

    unsigned long long frameIndex = 0;
    unsigned long long fullscreenPasses = 0;
    unsigned long long resetCount = 0;
    unsigned long long resourceGeneration = 0;
    UINT allocatedTargets = 0;
    unsigned long long estimatedBytes = 0;

    bool gbufferCaptured = false;
    bool gbufferFresh = false;
    unsigned long long gbufferFrame = 0;
    UINT gbufferWidth = 0;
    UINT gbufferHeight = 0;
};

// Game scene inputs -----------------------------------------------------------
// Called by renderer hooks when DP binds the proven RT0 packed-depth + RT1
// encoded-normal pair. Holding AddRefs here gives every native effect one
// consistent source of truth and lets frame freshness disable AO during FMV.
void ObservePostFxGBufferPair(
    IDirect3DTexture9* depth,
    IDirect3DTexture9* normal);

bool AcquirePostFxGBuffer(PostFxGBufferView* view);
void ReleasePostFxGBuffer(PostFxGBufferView* view);

// High-precision depth ---------------------------------------------------------
// DP still writes its packed RT0 for native deferred shaders. ZachFix replaces
// the main D24S8 depth-stencil with sampleable INTZ when supported and captures
// the exact INTZ bound during the proven G-buffer geometry pass. All ZachFix
// PostFX consumers prefer this path and fall back to packed RT0 if unavailable.
bool TryCreatePostFxSampleableMainDepth(
    IDirect3DDevice9* device,
    UINT width,
    UINT height,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    IDirect3DTexture9** texture,
    IDirect3DSurface9** surface);
void ObservePostFxSampleableMainDepth(IDirect3DTexture9* texture);
void ObservePostFxDepthProjection(float projectionDepthQ, float projectionDepthQn);
bool AcquirePostFxPreferredDepth(PostFxDepthView* view);
void ReleasePostFxDepthView(PostFxDepthView* view);
bool IsPostFxNativeDepthAvailable();

// Device/frame lifecycle -------------------------------------------------------
void InitializePostFxFramework(IDirect3DDevice9* device);
void AdvancePostFxFrame();
unsigned long long GetPostFxFrameIndex();
void ReleasePostFxResources();
void NotifyPostFxResetResult(IDirect3DDevice9* device, HRESULT resetResult);
PostFxStats GetPostFxStats();

// Lazy render-target pool -----------------------------------------------------
// Reuses a slot if width/height/format match; otherwise only that slot is
// recreated. This is the hot-apply path future AO/DoF/Bloom quality settings
// use when their requested working resolution changes.
bool EnsurePostFxTarget(
    IDirect3DDevice9* device,
    PostFxTargetSlot slot,
    UINT width,
    UINT height,
    D3DFORMAT format,
    PostFxTargetView* view);

void ReleasePostFxTarget(PostFxTargetSlot slot);

// State-safe fullscreen processing -------------------------------------------
// Begin/End bracket an entire post stack, not every pass. The state block plus
// explicit RT/depth/viewport/texture backup keeps game/HUD state isolated.
bool BeginPostFxStateBackup(IDirect3DDevice9* device, PostFxStateBackup* backup);
void EndPostFxStateBackup(IDirect3DDevice9* device, PostFxStateBackup* backup);

bool RunPostFxFullscreenPass(
    IDirect3DDevice9* device,
    PostFxStateBackup* stateBackup,
    IDirect3DSurface9* output,
    UINT outputWidth,
    UINT outputHeight,
    IDirect3DPixelShader9* shader,
    const PostFxTextureBinding* bindings,
    size_t bindingCount,
    UINT constantStartRegister = 0,
    const float* constants = nullptr,
    UINT constantVector4Count = 0,
    bool srgbWrite = false,
    PostFxBlendMode blendMode = PostFxBlendMode::Opaque);

// Runtime shader compiler. Release builds can later replace runtime D3DX
// compilation with build-time FXC bytecode without changing effect plumbing.
bool CompilePostFxPixelShader(
    IDirect3DDevice9* device,
    const char* source,
    const char* entryPoint,
    const char* debugName,
    IDirect3DPixelShader9** shader);

// Utility for resolution-independent effects. divisor=1/2/4 etc.
void GetPostFxScaledExtent(
    UINT baseWidth,
    UINT baseHeight,
    UINT divisor,
    UINT* width,
    UINT* height);
