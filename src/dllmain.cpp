#include <Windows.h>
#include <d3d9.h>
#include <mutex>
#include <cwchar>

static HMODULE g_module = nullptr;
static HMODULE g_backend = nullptr;

using Direct3DCreate9Fn = IDirect3D9 * (WINAPI*)(UINT);

static Direct3DCreate9Fn g_direct3DCreate9 = nullptr;
static std::once_flag g_backendInitFlag;


static void InitializeBackend()
{
    wchar_t path[MAX_PATH] = {};

    const DWORD length =
        GetModuleFileNameW(g_module, path, MAX_PATH);

    if (length == 0 || length >= MAX_PATH)
        return;

    wchar_t* lastSlash = wcsrchr(path, L'\\');

    if (lastSlash == nullptr)
        return;

    *(lastSlash + 1) = L'\0';

    if (wcscat_s(path, L"dpfix-ng\\backend\\d3d9.dll") != 0)
        return;

    g_backend = LoadLibraryW(path);

    if (g_backend == nullptr)
        return;

    g_direct3DCreate9 =
        reinterpret_cast<Direct3DCreate9Fn>(
            GetProcAddress(
                g_backend,
                "Direct3DCreate9"
            )
            );
}


extern "C"
IDirect3D9* WINAPI Direct3DCreate9(UINT sdkVersion)
{
    std::call_once(
        g_backendInitFlag,
        InitializeBackend
    );

    if (g_direct3DCreate9 == nullptr)
        return nullptr;

    return g_direct3DCreate9(sdkVersion);
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
    }

    return TRUE;
}