#include <Windows.h>
#include <cwchar>
#include <iterator>

static HMODULE g_module = nullptr;

static void WriteLoadedMarker()
{
    wchar_t path[MAX_PATH] = {};

    const DWORD length = GetModuleFileNameW(
        g_module,
        path,
        static_cast<DWORD>(std::size(path))
    );

    if (length == 0 || length >= std::size(path))
        return;

    wchar_t* slash = wcsrchr(path, L'\\');

    if (slash == nullptr)
        return;

    *(slash + 1) = L'\0';

    if (wcscat_s(path, L"DPFixNG.loaded.txt") != 0)
        return;

    HANDLE file = CreateFileW(
        path,
        GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (file == INVALID_HANDLE_VALUE)
        return;

    static constexpr char message[] =
        "DPFix-NG v0.0.1 loaded successfully.\r\n"
        "No hooks or patches are active.\r\n";

    DWORD written = 0;

    WriteFile(
        file,
        message,
        static_cast<DWORD>(sizeof(message) - 1),
        &written,
        nullptr
    );

    CloseHandle(file);
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

        WriteLoadedMarker();
    }

    return TRUE;
}