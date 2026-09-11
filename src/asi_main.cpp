#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>

#include <array>
#include <atomic>
#include <cstdio>
#include <cwchar>
#include <mutex>
#include <set>


static HMODULE g_module = nullptr;


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

using SetViewportFn = HRESULT (WINAPI*)(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport
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
static SetRenderTargetFn g_originalSetRenderTarget = nullptr;
static SetViewportFn g_originalSetViewport = nullptr;


// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

static std::once_flag g_createDeviceHookOnce;
static std::once_flag g_deviceHooksOnce;

static std::mutex g_resourceLogMutex;

// Log only unique resource descriptions.
static std::set<std::array<unsigned long long, 10>> g_seenResources;

// Deadly Premonition's original internal render resolution.
static constexpr UINT kBaseRenderWidth = 1280;
static constexpr UINT kBaseRenderHeight = 720;

// v0.0.5 test target. This will become configurable later.
static constexpr UINT kRenderWidth = 2560;
static constexpr UINT kRenderHeight = 1440;

// Main render surfaces are few and long-lived. Atomic slots keep SetViewport
// free of mutexes and COM queries.
static std::array<std::atomic<IDirect3DSurface9*>, 16> g_mainRenderSurfaces{};
static std::atomic<IDirect3DSurface9*> g_currentRenderTarget0{ nullptr };
static std::atomic<IDirect3DSurface9*> g_backBuffer0{ nullptr };
static std::atomic_bool g_loggedViewportOverride{ false };


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


static void LogResolutionOverride(
    const char* resourceType,
    UINT originalWidth,
    UINT originalHeight,
    UINT newWidth,
    UINT newHeight,
    D3DFORMAT format,
    DWORD usage)
{
    char text[512] = {};

    sprintf_s(
        text,
        "[Resolution] %s %u x %u -> %u x %u, Format=%u (0x%08X), Usage=0x%08X\n",
        resourceType,
        originalWidth,
        originalHeight,
        newWidth,
        newHeight,
        static_cast<unsigned>(format),
        static_cast<unsigned>(format),
        usage
    );

    AppendLog(text);
}


// -----------------------------------------------------------------------------
// Resolution helpers
// -----------------------------------------------------------------------------

static bool IsBaseRenderSize(UINT width, UINT height)
{
    return width == kBaseRenderWidth &&
           height == kBaseRenderHeight;
}


static bool IsMainColorResource(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    if (!IsBaseRenderSize(width, height))
        return false;

    if ((usage & D3DUSAGE_RENDERTARGET) == 0)
        return false;

    // Observed in the Steam build:
    // 21  = D3DFMT_A8R8G8B8
    // 113 = D3DFMT_A16B16G16R16F
    return format == D3DFMT_A8R8G8B8 ||
           format == D3DFMT_A16B16G16R16F;
}


static bool IsMainDepthResource(
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format)
{
    return IsBaseRenderSize(width, height) &&
           (usage & D3DUSAGE_DEPTHSTENCIL) != 0 &&
           format == D3DFMT_D24S8;
}


static void RegisterMainRenderSurface(IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return;

    for (auto& slot : g_mainRenderSurfaces)
    {
        if (slot.load(std::memory_order_relaxed) == surface)
            return;
    }

    for (auto& slot : g_mainRenderSurfaces)
    {
        IDirect3DSurface9* expected = nullptr;

        if (slot.compare_exchange_strong(
                expected,
                surface,
                std::memory_order_release,
                std::memory_order_relaxed))
        {
            char text[256] = {};
            sprintf_s(
                text,
                "[Resolution] Registered main render surface: %p\n",
                static_cast<void*>(surface)
            );
            AppendLog(text);
            return;
        }
    }

    AppendLog("WARNING: No free slot for main render surface.\n");
}


static void RegisterMainTextureSurface(IDirect3DTexture9* texture)
{
    if (texture == nullptr)
        return;

    IDirect3DSurface9* surface = nullptr;

    if (SUCCEEDED(texture->GetSurfaceLevel(0, &surface)) &&
        surface != nullptr)
    {
        // Keep only the pointer identity. The texture owns the surface lifetime.
        RegisterMainRenderSurface(surface);
        surface->Release();
    }
}


static bool IsRegisteredMainRenderSurface(IDirect3DSurface9* surface)
{
    if (surface == nullptr)
        return false;

    for (auto& slot : g_mainRenderSurfaces)
    {
        if (slot.load(std::memory_order_acquire) == surface)
            return true;
    }

    return false;
}


static void LogBackBufferInfo(IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    IDirect3DSurface9* backBuffer = nullptr;

    if (FAILED(device->GetBackBuffer(
            0,
            0,
            D3DBACKBUFFER_TYPE_MONO,
            &backBuffer)) ||
        backBuffer == nullptr)
    {
        AppendLog("WARNING: Could not query back buffer 0.\n");
        return;
    }

    D3DSURFACE_DESC desc = {};

    if (SUCCEEDED(backBuffer->GetDesc(&desc)))
    {
        char text[256] = {};
        sprintf_s(
            text,
            "BackBuffer 0: %u x %u, Format=%u (0x%08X)\n",
            desc.Width,
            desc.Height,
            static_cast<unsigned>(desc.Format),
            static_cast<unsigned>(desc.Format)
        );
        AppendLog(text);
    }

    g_backBuffer0.store(
        backBuffer,
        std::memory_order_release
    );

    // Immediately after CreateDevice the backbuffer is render target 0
    // even if the game has not called SetRenderTarget yet.
    g_currentRenderTarget0.store(
        backBuffer,
        std::memory_order_release
    );

    backBuffer->Release();
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
    const UINT originalWidth = width;
    const UINT originalHeight = height;

    const bool isMainColor =
        IsMainColorResource(width, height, usage, format);

    const bool isMainDepth =
        IsMainDepthResource(width, height, usage, format);

    if (isMainColor || isMainDepth)
    {
        width = kRenderWidth;
        height = kRenderHeight;
    }

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

    if (SUCCEEDED(result) && (isMainColor || isMainDepth))
    {
        LogResolutionOverride(
            "CreateTexture",
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            usage
        );

        if (isMainColor &&
            texture != nullptr &&
            *texture != nullptr)
        {
            RegisterMainTextureSurface(*texture);
        }
    }

    if (SUCCEEDED(result) &&
        (usage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL)) != 0)
    {
        // Keep discovery output in terms of what the game requested.
        LogResourceOnce(
            1,
            "CreateTexture",
            originalWidth,
            originalHeight,
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
    const UINT originalWidth = width;
    const UINT originalHeight = height;

    const bool isMainColor =
        IsMainColorResource(
            width,
            height,
            D3DUSAGE_RENDERTARGET,
            format
        );

    if (isMainColor)
    {
        width = kRenderWidth;
        height = kRenderHeight;
    }

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

    if (SUCCEEDED(result) && isMainColor)
    {
        LogResolutionOverride(
            "CreateRenderTarget",
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            D3DUSAGE_RENDERTARGET
        );

        if (surface != nullptr && *surface != nullptr)
            RegisterMainRenderSurface(*surface);
    }

    if (SUCCEEDED(result))
    {
        LogResourceOnce(
            3,
            "CreateRenderTarget",
            originalWidth,
            originalHeight,
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
    const UINT originalWidth = width;
    const UINT originalHeight = height;

    const bool isMainDepth =
        IsMainDepthResource(
            width,
            height,
            D3DUSAGE_DEPTHSTENCIL,
            format
        );

    if (isMainDepth)
    {
        width = kRenderWidth;
        height = kRenderHeight;
    }

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

    if (SUCCEEDED(result) && isMainDepth)
    {
        LogResolutionOverride(
            "CreateDepthStencilSurface",
            originalWidth,
            originalHeight,
            width,
            height,
            format,
            D3DUSAGE_DEPTHSTENCIL
        );
    }

    if (SUCCEEDED(result))
    {
        LogResourceOnce(
            4,
            "CreateDepthStencilSurface",
            originalWidth,
            originalHeight,
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
    {
        g_currentRenderTarget0.store(
            target,
            std::memory_order_release
        );
    }

    return result;
}


static HRESULT WINAPI HookSetViewport(
    IDirect3DDevice9* self,
    const D3DVIEWPORT9* viewport)
{
    if (viewport == nullptr)
        return g_originalSetViewport(self, viewport);

    //
    // Deadly Premonition uses several hard-coded 1280x720
    // viewports for its maps. These need to be scaled to
    // the presentation resolution independently.
    //
    const bool isMiniMap =
        viewport->X == 76 &&
        viewport->Y == 368 &&
        viewport->Width == 232 &&
        viewport->Height == 200;

    const bool isMenuMap =
        viewport->X == 252 &&
        viewport->Y == 60 &&
        viewport->Width == 776 &&
        viewport->Height == 520;

    const bool isLargeMap =
        viewport->X == 64 &&
        viewport->Y == 164 &&
        viewport->Width == 512 &&
        viewport->Height == 512;

    if (isMiniMap || isMenuMap || isLargeMap)
    {
        D3DVIEWPORT9 modified = *viewport;

        modified.X =
            viewport->X * kRenderWidth / kBaseRenderWidth;

        modified.Y =
            viewport->Y * kRenderHeight / kBaseRenderHeight;

        modified.Width =
            viewport->Width * kRenderWidth / kBaseRenderWidth;

        modified.Height =
            viewport->Height * kRenderHeight / kBaseRenderHeight;

        static std::atomic_bool loggedMiniMap{ false };
        static std::atomic_bool loggedMenuMap{ false };
        static std::atomic_bool loggedLargeMap{ false };

        std::atomic_bool* logFlag = nullptr;
        const char* name = nullptr;

        if (isMiniMap)
        {
            logFlag = &loggedMiniMap;
            name = "Minimap";
        }
        else if (isMenuMap)
        {
            logFlag = &loggedMenuMap;
            name = "Menu map";
        }
        else
        {
            logFlag = &loggedLargeMap;
            name = "Large map";
        }

        bool expected = false;

        if (logFlag->compare_exchange_strong(
            expected,
            true,
            std::memory_order_relaxed))
        {
            char text[256] = {};

            sprintf_s(
                text,
                "[Resolution] %s viewport "
                "%u,%u %ux%u -> %u,%u %ux%u\n",
                name,
                viewport->X,
                viewport->Y,
                viewport->Width,
                viewport->Height,
                modified.X,
                modified.Y,
                modified.Width,
                modified.Height
            );

            AppendLog(text);
        }

        return g_originalSetViewport(
            self,
            &modified
        );
    }

    //
    // Normal 1280x720 viewport.
    //
    if (viewport->Width != kBaseRenderWidth ||
        viewport->Height != kBaseRenderHeight)
    {
        return g_originalSetViewport(
            self,
            viewport
        );
    }

    IDirect3DSurface9* current =
        g_currentRenderTarget0.load(
            std::memory_order_acquire
        );

    IDirect3DSurface9* backBuffer =
        g_backBuffer0.load(
            std::memory_order_acquire
        );

    const bool isMainRenderSurface =
        IsRegisteredMainRenderSurface(current);

    const bool isBackBuffer =
        current != nullptr &&
        current == backBuffer;

    if (!isMainRenderSurface && !isBackBuffer)
    {
        return g_originalSetViewport(
            self,
            viewport
        );
    }

    D3DVIEWPORT9 modified = *viewport;

    modified.Width = kRenderWidth;
    modified.Height = kRenderHeight;

    if (isBackBuffer)
    {
        static std::atomic_bool loggedBackBufferViewport{ false };

        bool expected = false;

        if (loggedBackBufferViewport.compare_exchange_strong(
            expected,
            true,
            std::memory_order_relaxed))
        {
            AppendLog(
                "[Resolution] Backbuffer viewport "
                "1280 x 720 -> 2560 x 1440\n"
            );
        }
    }
    else
    {
        bool expected = false;

        if (g_loggedViewportOverride.compare_exchange_strong(
            expected,
            true,
            std::memory_order_relaxed))
        {
            AppendLog(
                "[Resolution] Main viewport "
                "1280 x 720 -> 2560 x 1440\n"
            );
        }
    }

    return g_originalSetViewport(
        self,
        &modified
    );
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

static bool ConfigureGameWindow(
    HWND window,
    UINT clientWidth,
    UINT clientHeight)
{
    if (window == nullptr)
    {
        AppendLog("[Window] ERROR: null window handle.\n");
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(
        window,
        MONITOR_DEFAULTTONEAREST
    );

    if (monitor == nullptr)
    {
        AppendLog("[Window] ERROR: MonitorFromWindow failed.\n");
        return false;
    }

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);

    if (!GetMonitorInfoW(monitor, &monitorInfo))
    {
        AppendLog("[Window] ERROR: GetMonitorInfoW failed.\n");
        return false;
    }

    //
    // Borderless window.
    //
    LONG_PTR style =
        GetWindowLongPtrW(window, GWL_STYLE);

    style &= ~static_cast<LONG_PTR>(
        WS_CAPTION |
        WS_THICKFRAME |
        WS_MINIMIZEBOX |
        WS_MAXIMIZEBOX |
        WS_SYSMENU
        );

    style |= WS_POPUP;

    SetWindowLongPtrW(
        window,
        GWL_STYLE,
        style
    );

    LONG_PTR exStyle =
        GetWindowLongPtrW(window, GWL_EXSTYLE);

    exStyle &= ~static_cast<LONG_PTR>(
        WS_EX_DLGMODALFRAME |
        WS_EX_WINDOWEDGE |
        WS_EX_CLIENTEDGE |
        WS_EX_STATICEDGE
        );

    SetWindowLongPtrW(
        window,
        GWL_EXSTYLE,
        exStyle
    );

    const int x = monitorInfo.rcMonitor.left;
    const int y = monitorInfo.rcMonitor.top;

    const BOOL positioned = SetWindowPos(
        window,
        HWND_TOP,
        x,
        y,
        static_cast<int>(clientWidth),
        static_cast<int>(clientHeight),
        SWP_FRAMECHANGED |
        SWP_NOOWNERZORDER |
        SWP_NOACTIVATE
    );

    if (!positioned)
    {
        AppendLog("[Window] ERROR: SetWindowPos failed.\n");
        return false;
    }

    RECT clientRect = {};

    if (!GetClientRect(window, &clientRect))
    {
        AppendLog("[Window] ERROR: GetClientRect failed.\n");
        return false;
    }

    const UINT actualWidth =
        static_cast<UINT>(clientRect.right - clientRect.left);

    const UINT actualHeight =
        static_cast<UINT>(clientRect.bottom - clientRect.top);

    char text[512] = {};

    sprintf_s(
        text,
        "[Window] Borderless client area: "
        "%u x %u, requested %u x %u, "
        "position %d,%d\n",
        actualWidth,
        actualHeight,
        clientWidth,
        clientHeight,
        x,
        y
    );

    AppendLog(text);

    return
        actualWidth == clientWidth &&
        actualHeight == clientHeight;
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

    HWND deviceWindow = focusWindow;

    if (pp != nullptr &&
        pp->hDeviceWindow != nullptr)
    {
        deviceWindow = pp->hDeviceWindow;
    }

    ConfigureGameWindow(
        deviceWindow,
        kRenderWidth,
        kRenderHeight
    );

    if (pp != nullptr)
    {
        pp->BackBufferWidth = 0;
        pp->BackBufferHeight = 0;
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

    

    LogBackBufferInfo(*returnedDevice);

    std::call_once(
        g_deviceHooksOnce,
        [returnedDevice]()
        {
            if (InstallDeviceHooks(*returnedDevice))
                AppendLog("All D3D9 hooks installed.\n");
            else
                AppendLog("ERROR: D3D9 hook installation failed.\n");
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

    AppendLog("DPFix-NG v0.0.5 Internal Resolution Test\n");
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
