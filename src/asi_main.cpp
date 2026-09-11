#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>

#include <cstdio>
#include <cwchar>

static HMODULE g_module = nullptr;

using Direct3DCreate9Fn = IDirect3D9 * (WINAPI*)(UINT);

static Direct3DCreate9Fn g_originalDirect3DCreate9 = nullptr;


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

    return g_originalDirect3DCreate9(sdkVersion);
}


static DWORD WINAPI InitializeHooks(LPVOID)
{
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