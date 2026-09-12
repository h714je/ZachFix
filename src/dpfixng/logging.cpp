#include "logging.h"

#include <cstdio>
#include <cwchar>

namespace
{
HMODULE g_logModule = nullptr;

bool GetLogPath(wchar_t* path, size_t pathCount)
{
    if (g_logModule == nullptr || path == nullptr || pathCount == 0)
        return false;

    const DWORD length = GetModuleFileNameW(
        g_logModule,
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
} // namespace

void SetLogModule(HMODULE module)
{
    g_logModule = module;
}

void ResetLog()
{
    wchar_t path[MAX_PATH] = {};

    if (GetLogPath(path, MAX_PATH))
        DeleteFileW(path);
}

void AppendLog(const char* text)
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

void LogResolutionOverride(
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
