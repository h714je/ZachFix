#include "logging.h"

#include <cstdio>
#include <cstdint>
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
        L"ZachFix.log"
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

void LogBuildIdentity()
{
    if (g_logModule == nullptr)
        return;

    DWORD peTimestamp = 0;
    const auto* imageBase = reinterpret_cast<const unsigned char*>(g_logModule);
    const auto* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(imageBase);
    if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE && dosHeader->e_lfanew > 0)
    {
        const auto* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(
            imageBase + dosHeader->e_lfanew);
        if (ntHeaders->Signature == IMAGE_NT_SIGNATURE)
            peTimestamp = ntHeaders->FileHeader.TimeDateStamp;
    }

    wchar_t modulePath[MAX_PATH] = {};
    const DWORD pathLength = GetModuleFileNameW(g_logModule, modulePath, MAX_PATH);

    std::uint64_t hash = 14695981039346656037ull;
    unsigned long long fileSize = 0;
    bool hashValid = false;

    if (pathLength != 0 && pathLength < MAX_PATH)
    {
        HANDLE file = CreateFileW(
            modulePath,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if (file != INVALID_HANDLE_VALUE)
        {
            LARGE_INTEGER size = {};
            if (GetFileSizeEx(file, &size) && size.QuadPart >= 0)
                fileSize = static_cast<unsigned long long>(size.QuadPart);

            unsigned char buffer[64 * 1024] = {};
            bool readOk = true;
            for (;;)
            {
                DWORD bytesRead = 0;
                if (!ReadFile(file, buffer, static_cast<DWORD>(sizeof(buffer)),
                              &bytesRead, nullptr))
                {
                    readOk = false;
                    break;
                }

                if (bytesRead == 0)
                    break;

                for (DWORD i = 0; i < bytesRead; ++i)
                {
                    hash ^= static_cast<std::uint64_t>(buffer[i]);
                    hash *= 1099511628211ull;
                }
            }

            hashValid = readOk;
            CloseHandle(file);
        }
    }

    char text[256] = {};
    if (hashValid)
    {
        sprintf_s(
            text,
            "[Build] Binary ID: fnv1a64=%016llX, size=%llu bytes, peTimestamp=0x%08X.\n",
            static_cast<unsigned long long>(hash),
            fileSize,
            static_cast<unsigned>(peTimestamp));
    }
    else
    {
        sprintf_s(
            text,
            "[Build] Binary ID: hash unavailable, peTimestamp=0x%08X.\n",
            static_cast<unsigned>(peTimestamp));
    }

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
