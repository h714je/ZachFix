// -----------------------------------------------------------------------------
// Function types
// -----------------------------------------------------------------------------

using Direct3DCreate9Fn = IDirect3D9* (WINAPI*)(
    UINT sdkVersion
);

using CreateDeviceFn = HRESULT (WINAPI*)(
    IDirect3D9* self,
    UINT adapter,
    D3DDEVTYPE deviceType,
    HWND focusWindow,
    DWORD behaviorFlags,
    D3DPRESENT_PARAMETERS* presentationParameters,
    IDirect3DDevice9** returnedDevice
);

using CreateTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DTexture9** texture,
    HANDLE* sharedHandle
);

using CreateCubeTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT edgeLength,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DCubeTexture9** texture,
    HANDLE* sharedHandle
);

using CreateRenderTargetFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL lockable,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle
);

using CreateDepthStencilSurfaceFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL discard,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle
);

using SetRenderTargetFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    DWORD renderTargetIndex,
    IDirect3DSurface9* renderTarget
);

using PresentFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const RECT* sourceRect,
    const RECT* destRect,
    HWND destWindowOverride,
    const RGNDATA* dirtyRegion
);

using BeginSceneFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self
);

using EndSceneFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self
);

using ClearFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    DWORD count,
    const D3DRECT* rects,
    DWORD flags,
    D3DCOLOR color,
    float z,
    DWORD stencil
);

using StretchRectFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DSurface9* sourceSurface,
    const RECT* sourceRect,
    IDirect3DSurface9* destSurface,
    const RECT* destRect,
    D3DTEXTUREFILTERTYPE filter
);

using SetDepthStencilSurfaceFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DSurface9* newDepthStencil
);

using SetRenderStateFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DRENDERSTATETYPE state,
    DWORD value
);

using SetTextureFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    DWORD stage,
    IDirect3DBaseTexture9* texture
);

using SetVertexShaderFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DVertexShader9* shader
);

using SetPixelShaderFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    IDirect3DPixelShader9* shader
);

using DrawPrimitiveFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT startVertex,
    UINT primitiveCount
);

using DrawIndexedPrimitiveFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount
);

using DrawPrimitiveUPFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT primitiveCount,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride
);

using DrawIndexedPrimitiveUPFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE primitiveType,
    UINT minVertexIndex,
    UINT numVertices,
    UINT primitiveCount,
    const void* indexData,
    D3DFORMAT indexDataFormat,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride
);

using SetViewportFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport
);

using SetVertexShaderConstantFFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount
);

using SetPixelShaderConstantFFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount
);

// Internal DP.exe scene-object renderer. This is used only by the profiler
// probe and is enabled only for the single captured frame.
using RenderSceneObjectFn = void (__thiscall*)(
    void* self,
    void* sceneObject,
    const short* visibleSubmeshList,
    int passMode
);

using GetRenderObjectFn = void* (__thiscall*)(
    void* self
);

using CandidateVisibilityFn = BYTE (__thiscall*)(
    void* self,
    void* sceneContext
);

using ObjectFactoryFn = void* (__thiscall*)(
    void* self,
    int objectType,
    void* createDesc
);

using Type2EEventThunkFn = void (__cdecl*)(
    int category,
    void* owner,
    int eventCode,
    void* payload
);

using LodOwnerDestructorFn = void* (__thiscall*)(
    void* self,
    unsigned int deleteFlags
);

using MarkForUnloadFn = void (__thiscall*)(
    void* self
);

using EnsureRenderResourceFn = void (__thiscall*)(
    void* self,
    void* sceneContext
);


// -----------------------------------------------------------------------------
// Originals
// -----------------------------------------------------------------------------

static Direct3DCreate9Fn g_originalDirect3DCreate9 = nullptr;
static CreateDeviceFn g_originalCreateDevice = nullptr;

static CreateTextureFn g_originalCreateTexture = nullptr;
static CreateCubeTextureFn g_originalCreateCubeTexture = nullptr;
static CreateRenderTargetFn g_originalCreateRenderTarget = nullptr;
static CreateDepthStencilSurfaceFn g_originalCreateDepthStencilSurface = nullptr;
static PresentFn g_originalPresent = nullptr;
static BeginSceneFn g_originalBeginScene = nullptr;
static EndSceneFn g_originalEndScene = nullptr;
static ClearFn g_originalClear = nullptr;
static StretchRectFn g_originalStretchRect = nullptr;
static SetRenderTargetFn g_originalSetRenderTarget = nullptr;
static SetDepthStencilSurfaceFn g_originalSetDepthStencilSurface = nullptr;
static SetRenderStateFn g_originalSetRenderState = nullptr;
static SetTextureFn g_originalSetTexture = nullptr;
static SetVertexShaderFn g_originalSetVertexShader = nullptr;
static SetPixelShaderFn g_originalSetPixelShader = nullptr;
static DrawPrimitiveFn g_originalDrawPrimitive = nullptr;
static DrawIndexedPrimitiveFn g_originalDrawIndexedPrimitive = nullptr;
static DrawPrimitiveUPFn g_originalDrawPrimitiveUP = nullptr;
static DrawIndexedPrimitiveUPFn g_originalDrawIndexedPrimitiveUP = nullptr;
static SetViewportFn g_originalSetViewport = nullptr;
static SetVertexShaderConstantFFn g_originalSetVertexShaderConstantF = nullptr;
static SetPixelShaderConstantFFn g_originalSetPixelShaderConstantF = nullptr;
static RenderSceneObjectFn g_originalRenderSceneObject = nullptr;
static GetRenderObjectFn g_originalGetRenderObject = nullptr;
static CandidateVisibilityFn g_originalCandidateVisibility = nullptr;
static ObjectFactoryFn g_originalObjectFactory = nullptr;
static Type2EEventThunkFn g_originalType2EEventThunk = nullptr;
static LodOwnerDestructorFn g_originalLodOwnerDestructor = nullptr;
static MarkForUnloadFn g_originalMarkForUnload = nullptr;
static EnsureRenderResourceFn g_originalEnsureRenderResource = nullptr;


// -----------------------------------------------------------------------------
// State
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
static std::atomic_bool g_loggedViewportOverride{ false };

// Effective viewport currently submitted to D3D9. Original DPFix uses this
// to keep vertex-shader c0 in sync with viewport changes.
static std::atomic<UINT> g_currentViewportWidth{ kBaseRenderWidth };
static std::atomic<UINT> g_currentViewportHeight{ kBaseRenderHeight };
static std::atomic_bool g_lastVertexC0WasPixelSize{ false };

enum class ProfilerEventType : unsigned
{
    FrameStart,
    BeginScene,
    EndScene,
    Present,
    SetRenderTarget,
    SetDepthStencilSurface,
    SetViewport,
    SetTexture,
    SetVertexShader,
    SetPixelShader,
    SetRenderState,
    Clear,
    StretchRect,
    DrawPrimitive,
    DrawIndexedPrimitive,
    DrawPrimitiveUP,
    DrawIndexedPrimitiveUP
};

struct ProfilerEvent
{
    ProfilerEventType type = ProfilerEventType::FrameStart;
    LONGLONG ticks = 0;
    unsigned long long v[8] = {};

    uintptr_t callerAddress = 0;
    UINT callerRva = 0;
    bool callerInMainExe = false;

    int passIndex = -1;
    UINT vertexShaderId = 0;
    UINT pixelShaderId = 0;
};

struct ProfilerShaderInfo
{
    UINT id = 0;
    bool pixel = false;
    void* pointer = nullptr;
    unsigned long long hash = 0;
    UINT bytecodeSize = 0;

    UINT shaderMajor = 0;
    UINT shaderMinor = 0;

    // s0..s15 referenced by executable shader instructions.
    // DCL declarations are tracked separately so an unused declaration
    // cannot create a false producer -> consumer edge.
    UINT samplerMask = 0;
    UINT declaredSamplerMask = 0;
    bool samplerMaskValid = false;

    std::vector<unsigned char> bytecode;
};

enum class ProfilerResourceTag : UINT
{
    Unknown = 0,
    BackBuffer,
    MainLdr,
    MainHdr,
    MainDepth,
    ShadowColor512,
    ShadowDepth512,
    ShadowColor1024,
    ShadowDepth1024,
    ReflectionSmallColor,
    ReflectionSmallDepth,
    ReflectionLargeColor,
    ReflectionLargeDepth,
    Storage448,
    Storage896,
    DofHdr448,
    ExposureChain,
    PostFx224,
    PostFx256,
    HdrAux128
};

struct ProfilerResourceInfo
{
    UINT id = 0;
    ProfilerResourceTag tag = ProfilerResourceTag::Unknown;

    void* texturePointer = nullptr;
    void* surfacePointer = nullptr;

    UINT requestedWidth = 0;
    UINT requestedHeight = 0;
    UINT effectiveWidth = 0;
    UINT effectiveHeight = 0;

    UINT format = 0;
    DWORD usage = 0;
    D3DPOOL pool = D3DPOOL_DEFAULT;

    bool textureBacked = false;
};

struct ProfilerSurfaceInfo
{
    void* pointer = nullptr;
    UINT resourceId = 0;
    ProfilerResourceTag tag = ProfilerResourceTag::Unknown;
    UINT width = 0;
    UINT height = 0;
    UINT format = 0;
    DWORD usage = 0;
};

struct ProfilerDependency
{
    UINT resourceId = 0;
    DWORD stage = 0;
    int producerPass = -1;
};

struct ProfilerCallerStats
{
    uintptr_t address = 0;
    UINT rva = 0;
    bool inMainExe = false;

    unsigned long long drawCalls = 0;
    unsigned long long primitives = 0;

    std::unordered_map<UINT, UINT> passDrawCounts;
    std::unordered_map<UINT, UINT> vsDrawCounts;
    std::unordered_map<UINT, UINT> psDrawCounts;
};


struct ProfilerPass
{
    UINT index = 0;

    ProfilerSurfaceInfo rt0{};
    ProfilerSurfaceInfo rt1{};
    ProfilerSurfaceInfo depth{};

    D3DVIEWPORT9 viewport{};

    size_t startEvent = 0;
    size_t endEvent = 0;

    UINT drawCalls = 0;
    unsigned long long primitives = 0;
    UINT textureBinds = 0;
    UINT shaderBinds = 0;
    UINT clears = 0;
    UINT stretchRects = 0;

    UINT firstVs = 0;
    UINT firstPs = 0;
    UINT lastVs = 0;
    UINT lastPs = 0;
    UINT dominantVs = 0;
    UINT dominantPs = 0;

    bool sawMrt = false;
    bool sawStage8FullRes = false;
    bool writesDepth = false;

    // Pixel-shader samplers actually referenced by draws in this pass.
    UINT sampledStagesMask = 0;
    UINT fallbackSamplerDraws = 0;

    // Draw-centric graph data. A pass may write multiple MRT/depth resources.
    std::vector<ProfilerDependency> dependencies;
    std::vector<UINT> outputResources;
    std::vector<UINT> depthOutputResources;

    std::unordered_map<UINT, UINT> vsDrawCounts;
    std::unordered_map<UINT, UINT> psDrawCounts;

    IDirect3DQuery9* gpuStartQuery = nullptr;
    IDirect3DQuery9* gpuEndQuery = nullptr;

    UINT64 gpuStartTicks = 0;
    UINT64 gpuEndTicks = 0;
    bool gpuTimingValid = false;
    double gpuMs = 0.0;
};

static std::atomic_bool g_profilerCaptureActive{ false };
static std::atomic_bool g_profilerCaptureArmed{ false };
static std::atomic_uint g_profilerCaptureCounter{ 0 };

static UINT g_profilerCurrentCapture = 0;
static LARGE_INTEGER g_profilerQpcFrequency{};
static LARGE_INTEGER g_profilerStartCounter{};
static bool g_profilerOverflow = false;

static std::vector<ProfilerEvent> g_profilerEvents;
static std::vector<ProfilerShaderInfo> g_profilerShaders;
static std::vector<ProfilerPass> g_profilerPasses;
static std::unordered_map<uintptr_t, ProfilerCallerStats> g_profilerCallers;

// v0.0.31-exp2: promote outer streaming cells on both bulk and incremental paths.
// TLS is required because D3D calls occur inside the hooked renderer.
static thread_local uintptr_t g_profilerSceneObject = 0;
static thread_local uintptr_t g_profilerSceneOwner = 0;
static thread_local UINT g_profilerSceneObjectCallerRva = 0;
static std::unordered_map<uintptr_t, uintptr_t> g_profilerSceneObjectOwners;

struct ProfilerSceneOwnerCall
{
    LONGLONG ticks = 0;
    uintptr_t owner = 0;
    uintptr_t result = 0;
    uintptr_t vtable = 0;
    uintptr_t rawRenderObject = 0;
    UINT callerRva = 0;
    BYTE type = 0;
    BYTE disableFlags = 0;
    unsigned long long flagsD8 = 0;
    unsigned long long flags138 = 0;
};

static std::vector<ProfilerSceneOwnerCall> g_profilerSceneOwnerCalls;

struct ProfilerCandidateOwnerCall
{
    LONGLONG ticks = 0;
    uintptr_t owner = 0;
    uintptr_t vtable = 0;
    uintptr_t rawRenderObject = 0;
    UINT callerRva = 0;
    BYTE result = 0;
    BYTE type = 0;
    BYTE disableFlags = 0;
    WORD classField458 = 0;
    BYTE classField45A = 0;
    unsigned long long flagsD8 = 0;
    unsigned long long flags138 = 0;
};

static std::vector<ProfilerCandidateOwnerCall> g_profilerCandidateOwnerCalls;

enum class ProfilerLifecycleKind : BYTE
{
    Create = 1,
    Destroy = 2,
    MarkUnload = 3,
    ResourceAttach = 4,
    StreamEvent3 = 5
};

struct ProfilerLifecycleEvent
{
    LONGLONG ticks = 0;
    ProfilerLifecycleKind kind = ProfilerLifecycleKind::Create;
    uintptr_t owner = 0;
    uintptr_t caller = 0;
    UINT callerRva = 0;
    uintptr_t rawRenderObject = 0;
    uintptr_t auxPointer = 0;
    uintptr_t result = 0;
    WORD classField458 = 0;
    BYTE classField45A = 0;
    BYTE state29 = 0;
    BYTE state2A = 0;
    WORD objectCategory2C = 0;
    WORD objectType30 = 0;
    UINT resourceKey144 = 0;
    UINT streamFlags434 = 0;
    WORD eventCategory = 0;
    BYTE eventCode = 0;
    unsigned long long flagsD8 = 0;
    unsigned long long flags138 = 0;
};

static std::vector<ProfilerLifecycleEvent> g_profilerLifecycleEvents;

struct ProfilerContinuousDraw
{
    LONGLONG ticks = 0;
    uintptr_t sceneObject = 0;
    uintptr_t sceneOwner = 0;
    UINT callerRva = 0;
    UINT objectCallerRva = 0;
    int baseVertex = 0;
    UINT minVertex = 0;
    UINT numVertices = 0;
    UINT startIndex = 0;
    UINT primitiveCount = 0;
};

struct ProfilerContinuousFrame
{
    UINT index = 0;
    LONGLONG startTicks = 0;
    LONGLONG endTicks = 0;
    std::vector<ProfilerCandidateOwnerCall> candidates;
    std::vector<ProfilerSceneOwnerCall> owners;
    std::vector<ProfilerLifecycleEvent> lifecycle;
    std::vector<ProfilerContinuousDraw> draws;
};

static std::atomic_bool g_profilerContinuousActive{ false };
static std::atomic_uint g_profilerContinuousCounter{ 0 };
static UINT g_profilerContinuousCurrent = 0;
static LARGE_INTEGER g_profilerContinuousStartCounter{};
static LARGE_INTEGER g_profilerContinuousFrameStartCounter{};
static std::vector<ProfilerContinuousDraw> g_profilerContinuousDraws;
static std::vector<ProfilerContinuousFrame> g_profilerContinuousFrames;

static void* g_sceneObjectTraceTarget = nullptr;
static void* g_sceneOwnerTraceTarget = nullptr;
static void* g_candidateVisibilityTraceTarget = nullptr;
static void* g_objectFactoryTraceTarget = nullptr;
static void* g_type2EEventThunkTraceTarget = nullptr;
static void* g_lodOwnerDestructorTraceTarget = nullptr;
static void* g_markForUnloadTraceTarget = nullptr;
static void* g_ensureRenderResourceTraceTarget = nullptr;
static bool g_sceneObjectTraceHookCreated = false;
static bool g_sceneObjectTraceHookEnabled = false;
static bool g_sceneOwnerTraceHookCreated = false;
static bool g_sceneOwnerTraceHookEnabled = false;
static bool g_candidateVisibilityTraceHookCreated = false;
static bool g_candidateVisibilityTraceHookEnabled = false;
static bool g_objectFactoryTraceHookCreated = false;
static bool g_objectFactoryTraceHookEnabled = false;
static bool g_type2EEventThunkTraceHookCreated = false;
static bool g_type2EEventThunkTraceHookEnabled = false;
static bool g_lodOwnerDestructorTraceHookCreated = false;
static bool g_lodOwnerDestructorTraceHookEnabled = false;
static bool g_markForUnloadTraceHookCreated = false;
static bool g_markForUnloadTraceHookEnabled = false;
static bool g_ensureRenderResourceTraceHookCreated = false;
static bool g_ensureRenderResourceTraceHookEnabled = false;

static std::vector<ProfilerResourceInfo> g_profilerResources;
static std::unordered_map<void*, UINT> g_profilerTextureToResource;
static std::unordered_map<void*, UINT> g_profilerSurfaceToResource;
static std::unordered_map<UINT, int> g_profilerLastWriter;

static ProfilerSurfaceInfo g_profilerCurrentRt0{};
static ProfilerSurfaceInfo g_profilerCurrentRt1{};
static ProfilerSurfaceInfo g_profilerCurrentRenderTargets[4] = {};
static ProfilerSurfaceInfo g_profilerCurrentDepth{};
static UINT g_profilerCurrentTextureResources[16] = {};
static D3DVIEWPORT9 g_profilerCurrentViewport{};
static UINT g_profilerCurrentVs = 0;
static UINT g_profilerCurrentPs = 0;
static int g_profilerOpenPass = -1;
static bool g_profilerZWriteEnable = true;

static bool g_profilerGpuRequested = true;
static bool g_profilerGpuSupported = false;
static bool g_profilerGpuValid = false;
static UINT64 g_profilerGpuFrequency = 0;
static BOOL g_profilerGpuDisjoint = TRUE;
static double g_profilerGpuFrameMs = 0.0;

static IDirect3DQuery9* g_profilerGpuFreqQuery = nullptr;
static IDirect3DQuery9* g_profilerGpuDisjointQuery = nullptr;
static IDirect3DQuery9* g_profilerGpuFrameStartQuery = nullptr;
static IDirect3DQuery9* g_profilerGpuFrameEndQuery = nullptr;
static UINT64 g_profilerGpuFrameStartTicks = 0;
static UINT64 g_profilerGpuFrameEndTicks = 0;



