// -----------------------------------------------------------------------------
// D3D9 function types
// -----------------------------------------------------------------------------

using Direct3DCreate9Fn = IDirect3D9* (WINAPI*)(UINT sdkVersion);

using CreateDeviceFn = HRESULT (WINAPI*)(
    IDirect3D9* self,
    UINT adapter,
    D3DDEVTYPE deviceType,
    HWND focusWindow,
    DWORD behaviorFlags,
    D3DPRESENT_PARAMETERS* presentationParameters,
    IDirect3DDevice9** returnedDevice);

using CreateTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DTexture9** texture,
    HANDLE* sharedHandle);

using CreateRenderTargetFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL lockable,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle);

using CreateDepthStencilSurfaceFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL discard,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle);

using PresentFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion);

using SwapChainPresentFn = HRESULT (WINAPI*)(
    IDirect3DSwapChain9* self,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion,
    DWORD flags);

using EndSceneFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self);

using StretchRectFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DSurface9* sourceSurface,
    const RECT* sourceRect,
    IDirect3DSurface9* destSurface,
    const RECT* destRect,
    D3DTEXTUREFILTERTYPE filter);

using SetRenderTargetFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    DWORD renderTargetIndex,
    IDirect3DSurface9* renderTarget);

using SetDepthStencilSurfaceFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DSurface9* newDepthStencil);

using SetTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    DWORD stage,
    IDirect3DBaseTexture9* texture);

using SetSamplerStateFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    DWORD sampler,
    D3DSAMPLERSTATETYPE type,
    DWORD value);

using SetViewportFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport);

using DrawPrimitiveFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT startVertex,
    UINT primitiveCount);

using DrawIndexedPrimitiveFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount);

using DrawPrimitiveUPFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT primitiveCount,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride);

using DrawIndexedPrimitiveUPFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT minVertexIndex,
    UINT numVertices,
    UINT primitiveCount,
    const void* indexData,
    D3DFORMAT indexDataFormat,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride);

using CreateVertexShaderFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const DWORD* function,
    IDirect3DVertexShader9** shader);

using SetVertexShaderFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DVertexShader9* shader);

using SetVertexShaderConstantFFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount);

using SetStreamSourceFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT streamNumber,
    IDirect3DVertexBuffer9* streamData,
    UINT offsetInBytes,
    UINT stride);

using CreatePixelShaderFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const DWORD* function,
    IDirect3DPixelShader9** shader);

using SetPixelShaderFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DPixelShader9* shader);

using SetPixelShaderConstantFFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount);

// -----------------------------------------------------------------------------
// Original D3D9 entry points
// -----------------------------------------------------------------------------

static Direct3DCreate9Fn g_originalDirect3DCreate9 = nullptr;
static CreateDeviceFn g_originalCreateDevice = nullptr;
static CreateTextureFn g_originalCreateTexture = nullptr;
static CreateRenderTargetFn g_originalCreateRenderTarget = nullptr;
static CreateDepthStencilSurfaceFn g_originalCreateDepthStencilSurface = nullptr;
static PresentFn g_originalPresent = nullptr;
static SwapChainPresentFn g_originalSwapChainPresent = nullptr;
static EndSceneFn g_originalEndScene = nullptr;
static StretchRectFn g_originalStretchRect = nullptr;
static SetRenderTargetFn g_originalSetRenderTarget = nullptr;
static SetDepthStencilSurfaceFn g_originalSetDepthStencilSurface = nullptr;
static SetTextureFn g_originalSetTexture = nullptr;
static SetSamplerStateFn g_originalSetSamplerState = nullptr;
static SetViewportFn g_originalSetViewport = nullptr;
static DrawPrimitiveFn g_originalDrawPrimitive = nullptr;
static DrawIndexedPrimitiveFn g_originalDrawIndexedPrimitive = nullptr;
static DrawPrimitiveUPFn g_originalDrawPrimitiveUP = nullptr;
static DrawIndexedPrimitiveUPFn g_originalDrawIndexedPrimitiveUP = nullptr;
static CreateVertexShaderFn g_originalCreateVertexShader = nullptr;
static SetVertexShaderFn g_originalSetVertexShader = nullptr;
static SetVertexShaderConstantFFn g_originalSetVertexShaderConstantF = nullptr;
static SetStreamSourceFn g_originalSetStreamSource = nullptr;
static CreatePixelShaderFn g_originalCreatePixelShader = nullptr;
static SetPixelShaderFn g_originalSetPixelShader = nullptr;
static SetPixelShaderConstantFFn g_originalSetPixelShaderConstantF = nullptr;

// -----------------------------------------------------------------------------
// Shared render state
// -----------------------------------------------------------------------------

static std::once_flag g_createDeviceHookOnce;
static std::once_flag g_deviceHooksOnce;

// Deadly Premonition's original internal render resolution.
static constexpr UINT kBaseRenderWidth = 1280;
static constexpr UINT kBaseRenderHeight = 720;

// Main render surfaces are few and long-lived. Atomic slots keep SetViewport
// free of mutexes and COM queries.
static std::array<std::atomic<IDirect3DSurface9*>, 16> g_mainRenderSurfaces{};
static std::atomic<IDirect3DSurface9*> g_currentRenderTarget0{ nullptr };
static std::atomic<IDirect3DSurface9*> g_backBuffer0{ nullptr };

// State used by the original DPFix dual-view compatibility correction.
static std::atomic_bool g_firstStreamSourceAfterRenderTarget{ false };
static std::atomic_bool g_lastTextureWasDualViewCandidate{ false };

static std::atomic_bool g_loggedViewportOverride{ false };

// Effective viewport currently submitted to D3D9. Original DPFix uses this
// to keep vertex-shader c0 in sync with viewport changes.
static std::atomic<UINT> g_currentViewportWidth{ kBaseRenderWidth };
static std::atomic<UINT> g_currentViewportHeight{ kBaseRenderHeight };
static std::atomic_bool g_lastVertexC0WasPixelSize{ false };
