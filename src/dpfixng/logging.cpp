#include "logging.h"

#include <array>
#include <cstdio>
#include <cwchar>
#include <mutex>
#include <set>

namespace
{
HMODULE g_logModule = nullptr;
std::mutex g_resourceLogMutex;
std::set<std::array<unsigned long long, 10>> g_seenResources;

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

HMODULE GetLogModule()
{
    return g_logModule;
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

void LogResourceOnce(
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
