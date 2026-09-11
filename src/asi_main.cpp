#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>

#include <cstdio>
#include <cwchar>
#include <mutex>

static HMODULE g_module = nullptr;

using Direct3DCreate9Fn = IDirect3D9 * (WINAPI*)(UINT);

static Direct3DCreate9Fn g_originalDirect3DCreate9 = nullptr;

using CreateDeviceFn = HRESULT(WINAPI*)(
    IDirect3D9* self,
    UINT adapter,
    D3DDEVTYPE deviceType,
    HWND focusWindow,
    DWORD behaviorFlags,
    D3DPRESENT_PARAMETERS* presentationParameters,
    IDirect3DDevice9** returnedDevice
    );

static CreateDeviceFn g_originalCreateDevice = nullptr;
static std::once_flag g_createDeviceHookOnce;

static void AppendLog(const char* text);

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

    if (SUCCEEDED(result) &&
        returnedDevice != nullptr &&
        *returnedDevice != nullptr)
    {
        AppendLog("CreateDevice succeeded.\n");
    }
    else
    {
        AppendLog("CreateDevice failed.\n");
    }

    return result;
}

static void AppendLog(const char* text)
{
    wchar_t path[MAX_PATH] = {};

    const DWORD length =
        GetModuleFileNameW(g_module, path, MAX_PATH);

    if (length == 0 || length >= MAX_PATH)
        return;

    wchar_t* slash = wcsrchr(path, L'\\');

    if (slash == nullptr)
        return;

    *(slash + 1) = L'\0';

    if (wcscat_s(path, L"DPFixNG.log") != 0)
        return;

    FILE* file = nullptr;

    if (_wfopen_s(&file, path, L"a") != 0 || file == nullptr)
        return;

    std::fputs(text, file);
    std::fclose(file);
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

            // IDirect3D9::CreateDevice = slot 16
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


static DWORD WINAPI InitializeHooks(LPVOID)
{
    wchar_t exePath[MAX_PATH] = {};

    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0)
        return 0;

    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = (exeName != nullptr) ? exeName + 1 : exePath;

    if (_wcsicmp(exeName, L"DP.exe") != 0)
        return 0;

    AppendLog("DPFix-NG v0.0.2 initialization started.\n");

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
        reinterpret_cast<LPVOID>(target),
        reinterpret_cast<LPVOID>(&HookDirect3DCreate9),
        reinterpret_cast<LPVOID*>(
            &g_originalDirect3DCreate9
            )
    );

    if (status != MH_OK)
    {
        AppendLog("ERROR: MH_CreateHook failed.\n");
        return 0;
    }

    status = MH_EnableHook(
        reinterpret_cast<LPVOID>(target)
    );

    if (status != MH_OK)
    {
        AppendLog("ERROR: MH_EnableHook failed.\n");
        return 0;
    }

    AppendLog("Direct3DCreate9 hook installed.\n");

    return 0;
}


BOOL WINAPI DllMain(
    HINSTANCE instance,
    DWORD reason,
    LPVOID
)
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