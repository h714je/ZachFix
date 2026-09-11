#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>

#include <array>
#include <cstdio>
#include <cwchar>
#include <mutex>
#include <set>


static HMODULE g_module = nullptr;


// -----------------------------------------------------------------------------
// Function types
// -----------------------------------------------------------------------------

using SetRenderTargetFn = HRESULT(WINAPI*)(
    IDirect3DDevice9*,
    DWORD,
    IDirect3DSurface9*
    );

using SetViewportFn = HRESULT(WINAPI*)(
    IDirect3DDevice9*,
    const D3DVIEWPORT9*
    );

static SetRenderTargetFn g_originalSetRenderTarget = nullptr;
static SetViewportFn g_originalSetViewport = nullptr;
static IDirect3DSurface9* g_currentRenderTarget0 = nullptr;

static HRESULT WINAPI HookSetRenderTarget(
    IDirect3DDevice9* self,
    DWORD index,
    IDirect3DSurface9* target)
{
    const HRESULT result = g_originalSetRenderTarget(
        self,
        index,
        target
    );

    if (SUCCEEDED(result) && index == 0)
        g_currentRenderTarget0 = target;

    return result;
}

static HRESULT WINAPI HookSetViewport(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport)
{
    return g_originalSetViewport(
        self,
        viewport
    );
}


using Direct3DCreate9Fn = IDirect3D9 * (WINAPI*)(
    UINT sdkVersion
    );

using CreateDeviceFn = HRESULT(WINAPI*)(
    IDirect3D9* self,
    UINT adapter,
    D3DDEVTYPE deviceType,
    HWND focusWindow,
    DWORD behaviorFlags,
    D3DPRESENT_PARAMETERS* presentationParameters,
    IDirect3DDevice9** returnedDevice
    );

using CreateTextureFn = HRESULT(WINAPI*)(
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

using CreateCubeTextureFn = HRESULT(WINAPI*)(
    IDirect3DDevice9* self,
    UINT edgeLength,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DCubeTexture9** texture,
    HANDLE* sharedHandle
    );

using CreateRenderTargetFn = HRESULT(WINAPI*)(
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

using CreateDepthStencilSurfaceFn = HRESULT(WINAPI*)(
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


// -----------------------------------------------------------------------------
// Originals
// -----------------------------------------------------------------------------

static Direct3DCreate9Fn g_originalDirect3DCreate9 = nullptr;
static CreateDeviceFn g_originalCreateDevice = nullptr;

static CreateTextureFn g_originalCreateTexture = nullptr;
static CreateCubeTextureFn g_originalCreateCubeTexture = nullptr;
static CreateRenderTargetFn g_originalCreateRenderTarget = nullptr;
static CreateDepthStencilSurfaceFn g_originalCreateDepthStencilSurface = nullptr;


// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::once_flag g_createDeviceHookOnce;
static std::once_flag g_deviceHooksOnce;

static std::mutex g_resourceLogMutex;

// We log only unique resource descriptions.
// This prevents another 3 GB log monument.
static std::set<std::array<unsigned long long, 10>> g_seenResources;

static constexpr UINT kBaseRenderWidth = 1280;
static constexpr UINT kBaseRenderHeight = 720;

static constexpr UINT kRenderWidth = 2560;
static constexpr UINT kRenderHeight = 1440;

static UINT g_presentWidth = 1280;
static UINT g_presentHeight = 720;

static IDirect3DSurface9* g_backBuffer0 = nullptr;
static IDirect3DSurface9* g_backBuffer1 = nullptr;

static std::mutex g_surfaceMutex;
static std::set<IDirect3DSurface9*> g_mainRenderSurfaces;

// -----------------------------------------------------------------------------
// Logging
// -----------------------------------------------------------------------------

static bool GetLogPath(wchar_t* path, size_t pathCount)
{
    if (g_module == nullptr || path == nullptr || pathCount == 0)
        return false;

    const DWORD length = GetModuleFileNameW(
        g_module,
        path,
        static_cast<DWORD>(pathCount)
    );

    if (length == 0 || length >= pathCount)
        return false;

    wchar_t* slash = wcsrchr(path, L'\\');

    if (slash == nullptr)
        return false;

    *(slash + 1) = L'\0';

    return wcscat_s(
        path,
        pathCount,
        L"DPFixNG.log"
    ) == 0;
}


static void ResetLog()
{
    wchar_t path[MAX_PATH] = {};

    if (GetLogPath(path, MAX_PATH))
        DeleteFileW(path);
}


static void AppendLog(const char* text)
{
    wchar_t path[MAX_PATH] = {};

    if (!GetLogPath(path, MAX_PATH))
        return;

    FILE* file = nullptr;

    if (_wfopen_s(&file, path, L"a") != 0 || file == nullptr)
        return;

    std::fputs(text, file);
    std::fclose(file);
}


static void LogResourceOnce(
    unsigned long long type,
    const char* name,
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    UINT levels,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL extraFlag)
{
    const std::array<unsigned long long, 10> key =
    {
        type,
        width,
        height,
        usage,
        static_cast<unsigned>(format),
        static_cast<unsigned>(pool),
        levels,
        static_cast<unsigned>(multiSample),
        multiSampleQuality,
        static_cast<unsigned>(extraFlag)
    };

    {
        std::lock_guard<std::mutex> lock(g_resourceLogMutex);

        const auto [iterator, inserted] =
            g_seenResources.insert(key);

        if (!inserted)
            return;
    }

    char text[1024] = {};

    sprintf_s(
        text,
        "%s:\n"
        "  Size                = %u x %u\n"
        "  Format              = %u (0x%08X)\n"
        "  Usage               = 0x%08X\n"
        "  Pool                = %u\n"
        "  Levels              = %u\n"
        "  MultiSampleType     = %u\n"
        "  MultiSampleQuality  = %u\n"
        "  ExtraFlag           = %s\n",
        name,
        width,
        height,
        static_cast<unsigned>(format),
        static_cast<unsigned>(format),
        usage,
        static_cast<unsigned>(pool),
        levels,
        static_cast<unsigned>(multiSample),
        multiSampleQuality,
        extraFlag ? "true" : "false"
    );

    AppendLog(text);
}


// -----------------------------------------------------------------------------
// Device hooks
// -----------------------------------------------------------------------------

static HRESULT WINAPI HookCreateTexture(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DTexture9** texture,
    HANDLE* sharedHandle)
{
    const HRESULT result = g_originalCreateTexture(
        self,
        width,
        height,
        levels,
        usage,
        format,
        pool,
        texture,
        sharedHandle
    );

    if (SUCCEEDED(result) &&
        (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL)) != 0)
    {
        LogResourceOnce(
            1,
            "CreateTexture",
            width,
            height,
            usage,
            format,
            pool,
            levels,
            D3DMULTISAMPLE_NONE,
            0,
            FALSE
        );
    }

    return result;
}


static HRESULT WINAPI HookCreateCubeTexture(
    IDirect3DDevice9* self,
    UINT edgeLength,
    UINT levels,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    IDirect3DCubeTexture9** texture,
    HANDLE* sharedHandle)
{
    const HRESULT result = g_originalCreateCubeTexture(
        self,
        edgeLength,
        levels,
        usage,
        format,
        pool,
        texture,
        sharedHandle
    );

    if (SUCCEEDED(result) &&
        (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL)) != 0)
    {
        LogResourceOnce(
            2,
            "CreateCubeTexture",
            edgeLength,
            edgeLength,
            usage,
            format,
            pool,
            levels,
            D3DMULTISAMPLE_NONE,
            0,
            FALSE
        );
    }

    return result;
}


static HRESULT WINAPI HookCreateRenderTarget(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL lockable,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle)
{
    const HRESULT result = g_originalCreateRenderTarget(
        self,
        width,
        height,
        format,
        multiSample,
        multiSampleQuality,
        lockable,
        surface,
        sharedHandle
    );

    if (SUCCEEDED(result))
    {
        LogResourceOnce(
            3,
            "CreateRenderTarget",
            width,
            height,
            D3DUSAGE_RENDERTARGET,
            format,
            D3DPOOL_DEFAULT,
            1,
            multiSample,
            multiSampleQuality,
            lockable
        );
    }

    return result;
}


static HRESULT WINAPI HookCreateDepthStencilSurface(
    IDirect3DDevice9* self,
    UINT width,
    UINT height,
    D3DFORMAT format,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL discard,
    IDirect3DSurface9** surface,
    HANDLE* sharedHandle)
{
    const HRESULT result = g_originalCreateDepthStencilSurface(
        self,
        width,
        height,
        format,
        multiSample,
        multiSampleQuality,
        discard,
        surface,
        sharedHandle
    );

    if (SUCCEEDED(result))
    {
        LogResourceOnce(
            4,
            "CreateDepthStencilSurface",
            width,
            height,
            D3DUSAGE_DEPTHSTENCIL,
            format,
            D3DPOOL_DEFAULT,
            1,
            multiSample,
            multiSampleQuality,
            discard
        );
    }

    return result;
}


static bool InstallDeviceHooks(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return false;

    void** vtable =
        *reinterpret_cast<void***>(device);

    struct HookEntry
    {
        void* target;
        void* hook;
        void** original;
        const char* name;
    };

    HookEntry hooks[] =
    {
        {
            vtable[23],
            reinterpret_cast<void*>(&HookCreateTexture),
            reinterpret_cast<void**>(&g_originalCreateTexture),
            "CreateTexture"
        },
        {
            vtable[25],
            reinterpret_cast<void*>(&HookCreateCubeTexture),
            reinterpret_cast<void**>(&g_originalCreateCubeTexture),
            "CreateCubeTexture"
        },
        {
            vtable[28],
            reinterpret_cast<void*>(&HookCreateRenderTarget),
            reinterpret_cast<void**>(&g_originalCreateRenderTarget),
            "CreateRenderTarget"
        },
        {
            vtable[29],
            reinterpret_cast<void*>(&HookCreateDepthStencilSurface),
            reinterpret_cast<void**>(&g_originalCreateDepthStencilSurface),
            "CreateDepthStencilSurface"
        },
        {
            vtable[37],
            reinterpret_cast<void*>(&HookSetRenderTarget),
            reinterpret_cast<void**>(&g_originalSetRenderTarget),
            "SetRenderTarget"
        },
        {
            vtable[47],
            reinterpret_cast<void*>(&HookSetViewport),
            reinterpret_cast<void**>(&g_originalSetViewport),
            "SetViewport"
        }
    };

    for (const HookEntry& entry : hooks)
    {
        MH_STATUS status = MH_CreateHook(
            entry.target,
            entry.hook,
            entry.original
        );

        if (status != MH_OK)
        {
            char text[256] = {};
            sprintf_s(
                text,
                "ERROR: MH_CreateHook failed for %s (%d).\n",
                entry.name,
                static_cast<int>(status)
            );
            AppendLog(text);
            return false;
        }

        status = MH_EnableHook(entry.target);

        if (status != MH_OK)
        {
            char text[256] = {};
            sprintf_s(
                text,
                "ERROR: MH_EnableHook failed for %s (%d).\n",
                entry.name,
                static_cast<int>(status)
            );
            AppendLog(text);
            return false;
        }

        char text[256] = {};
        sprintf_s(
            text,
            "%s hook installed.\n",
            entry.name
        );
        AppendLog(text);
    }

    return true;
}


// -----------------------------------------------------------------------------
// IDirect3D9 hook
// -----------------------------------------------------------------------------

static HRESULT WINAPI HookCreateDevice(
    IDirect3D9* self,
    UINT adapter,
    D3DDEVTYPE deviceType,
    HWND focusWindow,
    DWORD behaviorFlags,
    D3DPRESENT_PARAMETERS* pp,
    IDirect3DDevice9** returnedDevice)
{
    AppendLog("IDirect3D9::CreateDevice intercepted.\n");

    if (pp != nullptr)
    {
        char text[1024] = {};

        sprintf_s(
            text,
            "CreateDevice:\n"
            "  Adapter                  = %u\n"
            "  DeviceType               = %u\n"
            "  BehaviorFlags            = 0x%08X\n"
            "  BackBufferWidth           = %u\n"
            "  BackBufferHeight          = %u\n"
            "  BackBufferFormat          = %u\n"
            "  BackBufferCount           = %u\n"
            "  MultiSampleType           = %u\n"
            "  MultiSampleQuality        = %u\n"
            "  SwapEffect                = %u\n"
            "  Windowed                  = %s\n"
            "  EnableAutoDepthStencil    = %s\n"
            "  AutoDepthStencilFormat    = %u\n"
            "  Flags                     = 0x%08X\n"
            "  RefreshRate               = %u\n"
            "  PresentationInterval      = 0x%08X\n",
            adapter,
            static_cast<unsigned>(deviceType),
            behaviorFlags,
            pp->BackBufferWidth,
            pp->BackBufferHeight,
            static_cast<unsigned>(pp->BackBufferFormat),
            pp->BackBufferCount,
            static_cast<unsigned>(pp->MultiSampleType),
            pp->MultiSampleQuality,
            static_cast<unsigned>(pp->SwapEffect),
            pp->Windowed ? "true" : "false",
            pp->EnableAutoDepthStencil ? "true" : "false",
            static_cast<unsigned>(pp->AutoDepthStencilFormat),
            pp->Flags,
            pp->FullScreen_RefreshRateInHz,
            pp->PresentationInterval
        );

        AppendLog(text);
    }

    const HRESULT result = g_originalCreateDevice(
        self,
        adapter,
        deviceType,
        focusWindow,
        behaviorFlags,
        pp,
        returnedDevice
    );

    if (FAILED(result) ||
        returnedDevice == nullptr ||
        *returnedDevice == nullptr)
    {
        AppendLog("CreateDevice failed.\n");
        return result;
    }

    AppendLog("CreateDevice succeeded.\n");

    std::call_once(
        g_deviceHooksOnce,
        [returnedDevice]()
        {
            if (InstallDeviceHooks(*returnedDevice))
                AppendLog("All resource discovery hooks installed.\n");
            else
                AppendLog("ERROR: Resource discovery hook installation failed.\n");
        }
    );

    return result;
}


static IDirect3D9* WINAPI HookDirect3DCreate9(UINT sdkVersion)
{
    AppendLog("Direct3DCreate9 intercepted.\n");

    IDirect3D9* d3d =
        g_originalDirect3DCreate9(sdkVersion);

    if (d3d == nullptr)
    {
        AppendLog("ERROR: Direct3DCreate9 returned nullptr.\n");
        return nullptr;
    }

    std::call_once(
        g_createDeviceHookOnce,
        [d3d]()
        {
            void** vtable =
                *reinterpret_cast<void***>(d3d);

            // IDirect3D9::CreateDevice = slot 16.
            void* target = vtable[16];

            MH_STATUS status = MH_CreateHook(
                target,
                reinterpret_cast<void*>(&HookCreateDevice),
                reinterpret_cast<void**>(&g_originalCreateDevice)
            );

            if (status != MH_OK)
            {
                AppendLog("ERROR: CreateDevice hook creation failed.\n");
                return;
            }

            status = MH_EnableHook(target);

            if (status != MH_OK)
            {
                AppendLog("ERROR: CreateDevice hook enable failed.\n");
                return;
            }

            AppendLog("IDirect3D9::CreateDevice hook installed.\n");
        }
    );

    return d3d;
}


// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

static DWORD WINAPI InitializeHooks(LPVOID)
{
    wchar_t exePath[MAX_PATH] = {};

    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0)
        return 0;

    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = (exeName != nullptr) ? exeName + 1 : exePath;

    // Do nothing inside DPLauncher.exe or other processes.
    if (_wcsicmp(exeName, L"DP.exe") != 0)
        return 0;

    ResetLog();

    AppendLog("DPFix-NG v0.0.4 Resource Discovery\n");
    AppendLog("Initialization started.\n");

    HMODULE d3d9 = nullptr;

    for (int i = 0; i < 500 && d3d9 == nullptr; ++i)
    {
        d3d9 = GetModuleHandleW(L"d3d9.dll");

        if (d3d9 == nullptr)
            Sleep(10);
    }

    if (d3d9 == nullptr)
    {
        AppendLog("ERROR: d3d9.dll was not found.\n");
        return 0;
    }

    AppendLog("d3d9.dll found.\n");

    FARPROC target =
        GetProcAddress(d3d9, "Direct3DCreate9");

    if (target == nullptr)
    {
        AppendLog("ERROR: Direct3DCreate9 export not found.\n");
        return 0;
    }

    MH_STATUS status = MH_Initialize();

    if (status != MH_OK &&
        status != MH_ERROR_ALREADY_INITIALIZED)
    {
        AppendLog("ERROR: MH_Initialize failed.\n");
        return 0;
    }

    status = MH_CreateHook(
        reinterpret_cast<void*>(target),
        reinterpret_cast<void*>(&HookDirect3DCreate9),
        reinterpret_cast<void**>(&g_originalDirect3DCreate9)
    );

    if (status != MH_OK)
    {
        AppendLog("ERROR: Direct3DCreate9 MH_CreateHook failed.\n");
        return 0;
    }

    status = MH_EnableHook(
        reinterpret_cast<void*>(target)
    );

    if (status != MH_OK)
    {
        AppendLog("ERROR: Direct3DCreate9 MH_EnableHook failed.\n");
        return 0;
    }

    AppendLog("Direct3DCreate9 hook installed.\n");

    return 0;
}


BOOL WINAPI DllMain(
    HINSTANCE instance,
    DWORD reason,
    LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_module = instance;

        DisableThreadLibraryCalls(instance);

        HANDLE thread = CreateThread(
            nullptr,
            0,
            InitializeHooks,
            nullptr,
            0,
            nullptr
        );

        if (thread != nullptr)
            CloseHandle(thread);
    }

    return TRUE;
}