#include "save_diag.h"

#include "config.h"
#include "difficulty.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>
#include <miniz.h>
#include <intrin.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <new>
#include <string>
#include <vector>

namespace
{
constexpr size_t kTrackedHandleCount = 8;
constexpr size_t kPathCapacity = 1024;
constexpr unsigned kWriteLogLimit = 12;
constexpr unsigned long long kExpectedDpSaveSize = 0x7A2620ull;
constexpr size_t kDpHeaderSize = 0x120;
constexpr size_t kDpRecord0Offset = kDpHeaderSize;
constexpr size_t kDpItemTableOffset = 0xBD08;
constexpr size_t kDpEventBitsetOffset = 0x890;

using CreateFileAFn = HANDLE (WINAPI*)(
    LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
using WriteFileFn = BOOL (WINAPI*)(HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED);
using SetFilePointerFn = DWORD (WINAPI*)(HANDLE, LONG, PLONG, DWORD);
using SetEndOfFileFn = BOOL (WINAPI*)(HANDLE);
using FlushFileBuffersFn = BOOL (WINAPI*)(HANDLE);
using DeleteFileAFn = BOOL (WINAPI*)(LPCSTR);
using CloseHandleFn = BOOL (WINAPI*)(HANDLE);

CreateFileAFn g_originalCreateFileA = nullptr;
WriteFileFn g_originalWriteFile = nullptr;
SetFilePointerFn g_originalSetFilePointer = nullptr;
SetEndOfFileFn g_originalSetEndOfFile = nullptr;
FlushFileBuffersFn g_originalFlushFileBuffers = nullptr;
DeleteFileAFn g_originalDeleteFileA = nullptr;
CloseHandleFn g_originalCloseHandle = nullptr;

// Save Safety performs its own filesystem I/O while the Win32 hooks are active.
// The bypass keeps temp validation, backups, logging, and commit operations
// from recursively re-entering the game's observed save path.
thread_local bool g_saveSafetyBypass = false;

struct ScopedSaveSafetyBypass
{
    ScopedSaveSafetyBypass()
        : previous(g_saveSafetyBypass)
    {
        g_saveSafetyBypass = true;
    }

    ~ScopedSaveSafetyBypass()
    {
        g_saveSafetyBypass = previous;
    }

    bool previous;
};

struct TrackedFile
{
    HANDLE handle = nullptr;
    char path[kPathCapacity] = {};
    char tempPath[kPathCapacity] = {};
    unsigned writeCalls = 0;
    unsigned long long bytesWritten = 0;
    bool writeSuppressionLogged = false;
    bool transactional = false;
};

struct TrackedSnapshot
{
    bool found = false;
    char path[kPathCapacity] = {};
    char tempPath[kPathCapacity] = {};
    unsigned writeCalls = 0;
    unsigned long long bytesWritten = 0;
    bool transactional = false;
};

std::array<TrackedFile, kTrackedHandleCount> g_trackedFiles{};
std::mutex g_trackedMutex;
std::mutex g_backupMutex;
std::atomic_bool g_transactionalSaveReady{false};
std::atomic_bool g_transactionActive{false};

bool QueryPathState(const char* path, unsigned long long* size);
bool EnsureDirectoryExists(const char* path);

void DiagLog(const char* text)
{
    ScopedSaveSafetyBypass bypass;
    AppendLog(text);
}

char LowerAscii(char value)
{
    if (value >= 'A' && value <= 'Z')
        return static_cast<char>(value - 'A' + 'a');
    return value;
}

bool EndsWithInsensitive(const char* text, const char* suffix)
{
    if (text == nullptr || suffix == nullptr)
        return false;

    const size_t textLength = std::strlen(text);
    const size_t suffixLength = std::strlen(suffix);
    if (suffixLength > textLength)
        return false;

    const char* tail = text + textLength - suffixLength;
    for (size_t i = 0; i < suffixLength; ++i)
    {
        if (LowerAscii(tail[i]) != LowerAscii(suffix[i]))
            return false;
    }
    return true;
}

bool ContainsInsensitivePathFragment(const char* text, const char* fragment)
{
    if (text == nullptr || fragment == nullptr || fragment[0] == '\0')
        return false;

    const size_t textLength = std::strlen(text);
    const size_t fragmentLength = std::strlen(fragment);
    if (fragmentLength > textLength)
        return false;

    for (size_t start = 0; start + fragmentLength <= textLength; ++start)
    {
        bool matches = true;
        for (size_t i = 0; i < fragmentLength; ++i)
        {
            char a = text[start + i];
            char b = fragment[i];
            if (a == '/') a = '\\';
            if (b == '/') b = '\\';
            if (LowerAscii(a) != LowerAscii(b))
            {
                matches = false;
                break;
            }
        }
        if (matches)
            return true;
    }
    return false;
}

bool IsCandidateSavePath(const char* path)
{
    return EndsWithInsensitive(path, ".sav");
}

bool IsSaveDirectoryPath(const char* path)
{
    return ContainsInsensitivePathFragment(path, "\\savedata\\") ||
           ContainsInsensitivePathFragment(path, "savedata\\");
}

bool IsPrimaryDpSavePath(const char* path)
{
    return ContainsInsensitivePathFragment(path, "\\savedata\\dp.sav") &&
           EndsWithInsensitive(path, "dp.sav");
}

bool BuildDifficultyProfileSavePath(
    const char* legacyPath,
    char* output,
    size_t outputCount,
    bool ensureDirectory)
{
    if (legacyPath == nullptr || output == nullptr || outputCount == 0 ||
        !IsPrimaryDpSavePath(legacyPath))
    {
        return false;
    }

    const char* slash = std::strrchr(legacyPath, '\\');
    if (slash == nullptr)
        return false;

    const size_t parentLength = static_cast<size_t>(slash - legacyPath);
    char profileDirectory[kPathCapacity] = {};
    if (sprintf_s(
            profileDirectory,
            "%.*s\\%s",
            static_cast<int>(parentLength),
            legacyPath,
            GetSessionDifficultyProfileName()) < 0)
    {
        return false;
    }

    if (ensureDirectory && !EnsureDirectoryExists(profileDirectory))
        return false;

    return sprintf_s(output, outputCount, "%s\\dp.sav", profileDirectory) >= 0;
}

bool IsDestructiveOpen(DWORD desiredAccess, DWORD creationDisposition)
{
    if ((desiredAccess & (GENERIC_WRITE | GENERIC_ALL)) == 0)
        return false;

    return creationDisposition == CREATE_ALWAYS ||
           creationDisposition == TRUNCATE_EXISTING;
}

bool BuildTransactionalTempPath(const char* livePath, char* output, size_t outputCount)
{
    if (livePath == nullptr || output == nullptr || outputCount == 0)
        return false;

    output[0] = '\0';
    return sprintf_s(output, outputCount, "%s.zachtmp", livePath) >= 0;
}

bool EnsureDirectoryExists(const char* path)
{
    if (path == nullptr || path[0] == '\0')
        return false;

    const DWORD attributes = GetFileAttributesA(path);
    if (attributes != INVALID_FILE_ATTRIBUTES)
        return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

    if (CreateDirectoryA(path, nullptr))
        return true;

    return GetLastError() == ERROR_ALREADY_EXISTS;
}

bool PrepareDifficultySaveProfile()
{
    char exePath[kPathCapacity] = {};
    const DWORD length = GetModuleFileNameA(
        nullptr, exePath, static_cast<DWORD>(sizeof(exePath)));
    if (length == 0 || length >= sizeof(exePath))
        return false;

    char* slash = std::strrchr(exePath, '\\');
    if (slash == nullptr)
        return false;
    *(slash + 1) = '\0';

    char saveDirectory[kPathCapacity] = {};
    char legacyPath[kPathCapacity] = {};
    if (sprintf_s(saveDirectory, "%ssavedata", exePath) < 0 ||
        sprintf_s(legacyPath, "%s\\dp.sav", saveDirectory) < 0 ||
        !EnsureDirectoryExists(saveDirectory))
    {
        return false;
    }

    char profilePath[kPathCapacity] = {};
    if (!BuildDifficultyProfileSavePath(
            legacyPath, profilePath, sizeof(profilePath), true))
    {
        return false;
    }

    char text[1536] = {};
    sprintf_s(
        text,
        "[Difficulty] Save profile: %s -> \"%s\".\n",
        GetSessionDifficultyName(),
        profilePath);
    DiagLog(text);

    if (GetSessionDifficulty() != GameDifficulty::Easy ||
        GetFileAttributesA(profilePath) != INVALID_FILE_ATTRIBUTES ||
        GetFileAttributesA(legacyPath) == INVALID_FILE_ATTRIBUTES)
    {
        return true;
    }

    if (CopyFileA(legacyPath, profilePath, TRUE))
    {
        sprintf_s(
            text,
            "[Difficulty] Imported legacy Director's Cut save into Easy profile: "
            "\"%s\" -> \"%s\". Original left untouched.\n",
            legacyPath,
            profilePath);
        DiagLog(text);
        return true;
    }

    const DWORD error = GetLastError();
    sprintf_s(
        text,
        "[Difficulty] WARNING: Could not copy legacy save into Easy profile "
        "error=%lu. Original save was left untouched.\n",
        static_cast<unsigned long>(error));
    DiagLog(text);
    SetLastError(error);
    return false;
}

struct ZipFileInput
{
    const char* archiveName = nullptr;
    const char* sourcePath = nullptr;
};

void SetZipFailureReason(
    char* output,
    size_t outputCount,
    const char* stage,
    mz_zip_error error = MZ_ZIP_NO_ERROR)
{
    if (output == nullptr || outputCount == 0)
        return;

    if (error == MZ_ZIP_NO_ERROR)
        sprintf_s(output, outputCount, "%s", stage != nullptr ? stage : "ZIP operation failed");
    else
        sprintf_s(
            output,
            outputCount,
            "%s: %s",
            stage != nullptr ? stage : "ZIP operation failed",
            mz_zip_get_error_string(error));
}

bool FlushPathToDisk(const char* path, char* failureReason, size_t failureReasonCount)
{
    if (path == nullptr || path[0] == '\0')
    {
        SetZipFailureReason(failureReason, failureReasonCount, "ZIP flush path is invalid");
        return false;
    }

    ScopedSaveSafetyBypass bypass;
    HANDLE file = CreateFileA(
        path,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (file == INVALID_HANDLE_VALUE)
    {
        const DWORD error = GetLastError();
        if (failureReason != nullptr && failureReasonCount != 0)
            sprintf_s(failureReason, failureReasonCount,
                "could not reopen ZIP for durable flush (Win32 error %lu)",
                static_cast<unsigned long>(error));
        SetLastError(error);
        return false;
    }

    const BOOL flushed = FlushFileBuffers(file);
    const DWORD flushError = flushed ? ERROR_SUCCESS : GetLastError();
    CloseHandle(file);
    if (!flushed)
    {
        if (failureReason != nullptr && failureReasonCount != 0)
            sprintf_s(failureReason, failureReasonCount,
                "could not flush ZIP data to disk (Win32 error %lu)",
                static_cast<unsigned long>(flushError));
        SetLastError(flushError);
        return false;
    }

    return true;
}

bool CreateVerifiedZipArchive(
    const char* finalPath,
    const ZipFileInput* files,
    size_t fileCount,
    char* failureReason,
    size_t failureReasonCount)
{
    if (failureReason != nullptr && failureReasonCount != 0)
        failureReason[0] = '\0';

    if (finalPath == nullptr || files == nullptr || fileCount == 0)
    {
        SetZipFailureReason(
            failureReason,
            failureReasonCount,
            "invalid ZIP creation request");
        return false;
    }

    char tempPath[kPathCapacity] = {};
    if (sprintf_s(tempPath, "%s.zachtmp", finalPath) < 0)
    {
        SetZipFailureReason(
            failureReason,
            failureReasonCount,
            "ZIP temp path is too long");
        return false;
    }

    DeleteFileA(tempPath);

    mz_zip_archive zip{};
    mz_zip_zero_struct(&zip);
    if (!mz_zip_writer_init_file(&zip, tempPath, 0))
    {
        SetZipFailureReason(
            failureReason,
            failureReasonCount,
            "could not create ZIP archive",
            mz_zip_peek_last_error(&zip));
        DeleteFileA(tempPath);
        return false;
    }

    bool writerOk = true;
    mz_zip_error writerError = MZ_ZIP_NO_ERROR;
    const char* writerStage = "ZIP writer failed";

    for (size_t i = 0; i < fileCount; ++i)
    {
        if (files[i].archiveName == nullptr || files[i].sourcePath == nullptr ||
            !mz_zip_writer_add_file(
                &zip,
                files[i].archiveName,
                files[i].sourcePath,
                nullptr,
                0,
                MZ_DEFAULT_LEVEL))
        {
            writerOk = false;
            writerError = mz_zip_peek_last_error(&zip);
            writerStage = "could not add file to ZIP archive";
            break;
        }
    }

    if (writerOk && !mz_zip_writer_finalize_archive(&zip))
    {
        writerOk = false;
        writerError = mz_zip_peek_last_error(&zip);
        writerStage = "could not finalize ZIP archive";
    }

    const bool endOk = mz_zip_writer_end(&zip) != MZ_FALSE;
    if (writerOk && !endOk)
    {
        writerOk = false;
        writerError = mz_zip_peek_last_error(&zip);
        writerStage = "could not close ZIP archive";
    }

    if (!writerOk)
    {
        SetZipFailureReason(
            failureReason,
            failureReasonCount,
            writerStage,
            writerError);
        DeleteFileA(tempPath);
        return false;
    }

    mz_zip_error validationError = MZ_ZIP_NO_ERROR;
    if (!mz_zip_validate_file_archive(tempPath, 0, &validationError))
    {
        SetZipFailureReason(
            failureReason,
            failureReasonCount,
            "ZIP verification failed",
            validationError);
        DeleteFileA(tempPath);
        return false;
    }

    // The backup is the last known-good recovery copy once the live save is
    // replaced. Make the archive data durable before publishing the name and
    // before allowing the dp.sav transaction to commit.
    if (!FlushPathToDisk(tempPath, failureReason, failureReasonCount))
    {
        DeleteFileA(tempPath);
        return false;
    }

    if (!MoveFileExA(
            tempPath,
            finalPath,
            MOVEFILE_WRITE_THROUGH))
    {
        const DWORD error = GetLastError();
        if (failureReason != nullptr && failureReasonCount != 0)
        {
            sprintf_s(
                failureReason,
                failureReasonCount,
                "could not publish verified ZIP archive (Win32 error %lu)",
                static_cast<unsigned long>(error));
        }
        DeleteFileA(tempPath);
        SetLastError(error);
        return false;
    }

    return true;
}

bool BuildBackupDirectory(char* output, size_t outputCount)
{
    if (output == nullptr || outputCount == 0)
        return false;

    output[0] = '\0';
    const DWORD length = GetModuleFileNameA(nullptr, output, static_cast<DWORD>(outputCount));
    if (length == 0 || length >= outputCount)
        return false;

    char* slash = std::strrchr(output, '\\');
    if (slash == nullptr)
        return false;

    *(slash + 1) = '\0';

    char zachFixDir[kPathCapacity] = {};
    char backupsDir[kPathCapacity] = {};
    char profileDir[kPathCapacity] = {};
    if (sprintf_s(zachFixDir, "%sZachFix", output) < 0 ||
        sprintf_s(backupsDir, "%s\\save_backups", zachFixDir) < 0 ||
        sprintf_s(profileDir, "%s\\%s", backupsDir, GetSessionDifficultyProfileName()) < 0 ||
        sprintf_s(output, outputCount, "%s\\dp.sav", profileDir) < 0)
    {
        return false;
    }

    return EnsureDirectoryExists(zachFixDir) &&
           EnsureDirectoryExists(backupsDir) &&
           EnsureDirectoryExists(profileDir) &&
           EnsureDirectoryExists(output);
}

bool BuildUniqueBackupPath(
    const char* backupDirectory,
    char* output,
    size_t outputCount)
{
    if (backupDirectory == nullptr || output == nullptr || outputCount == 0)
        return false;

    SYSTEMTIME localTime{};
    GetLocalTime(&localTime);

    char stem[128] = {};
    sprintf_s(
        stem,
        "dp_%04u-%02u-%02u_%02u%02u%02u_%03u",
        static_cast<unsigned>(localTime.wYear),
        static_cast<unsigned>(localTime.wMonth),
        static_cast<unsigned>(localTime.wDay),
        static_cast<unsigned>(localTime.wHour),
        static_cast<unsigned>(localTime.wMinute),
        static_cast<unsigned>(localTime.wSecond),
        static_cast<unsigned>(localTime.wMilliseconds));

    for (unsigned suffix = 0; suffix < 100; ++suffix)
    {
        if (suffix == 0)
            sprintf_s(output, outputCount, "%s\\%s.zip", backupDirectory, stem);
        else
            sprintf_s(output, outputCount, "%s\\%s_%02u.zip", backupDirectory, stem, suffix);

        if (GetFileAttributesA(output) == INVALID_FILE_ATTRIBUTES)
            return true;
    }

    return false;
}

void RotateBackups(const char* backupDirectory, UINT keepCount)
{
    if (backupDirectory == nullptr || keepCount == 0)
        return;

    std::vector<std::string> names;
    static const char* const patterns[] = {
        "dp_*.zip",
        "dp_*.sav"
    };

    for (const char* filePattern : patterns)
    {
        char pattern[kPathCapacity] = {};
        if (sprintf_s(pattern, "%s\\%s", backupDirectory, filePattern) < 0)
            continue;

        WIN32_FIND_DATAA findData{};
        HANDLE find = FindFirstFileA(pattern, &findData);
        if (find == INVALID_HANDLE_VALUE)
            continue;

        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                names.emplace_back(findData.cFileName);
        }
        while (FindNextFileA(find, &findData));
        FindClose(find);
    }

    if (names.size() <= keepCount)
        return;

    std::sort(names.begin(), names.end());
    const size_t removeCount = names.size() - static_cast<size_t>(keepCount);
    for (size_t i = 0; i < removeCount; ++i)
    {
        char oldPath[kPathCapacity] = {};
        if (sprintf_s(oldPath, "%s\\%s", backupDirectory, names[i].c_str()) < 0)
            continue;

        if (!DeleteFileA(oldPath))
        {
            const DWORD error = GetLastError();
            char text[1400] = {};
            sprintf_s(
                text,
                "[SaveSafety] WARNING: Could not prune old backup path=\"%s\" error=%lu.\n",
                oldPath,
                static_cast<unsigned long>(error));
            DiagLog(text);
        }
    }
}

bool BackupExistingSave(const char* sourcePath, unsigned long long oldSize)
{
    if (!g_config.saveSafetyEnabled || sourcePath == nullptr)
        return false;

    std::lock_guard<std::mutex> lock(g_backupMutex);
    ScopedSaveSafetyBypass bypass;

    char backupDirectory[kPathCapacity] = {};
    if (!BuildBackupDirectory(backupDirectory, sizeof(backupDirectory)))
    {
        const DWORD error = GetLastError();
        char text[1400] = {};
        sprintf_s(
            text,
            "[SaveSafety] ERROR: Could not create backup directory before commit path=\"%s\" error=%lu. Live save will be preserved.\n",
            sourcePath,
            static_cast<unsigned long>(error));
        DiagLog(text);
        return false;
    }

    char backupPath[kPathCapacity] = {};
    if (!BuildUniqueBackupPath(backupDirectory, backupPath, sizeof(backupPath)))
    {
        DiagLog(
            "[SaveSafety] ERROR: Could not allocate a unique timestamped backup name. "
            "Live save will be preserved.\n");
        return false;
    }

    const ZipFileInput entry{ "dp.sav", sourcePath };
    char zipFailure[256] = {};
    if (!CreateVerifiedZipArchive(
            backupPath,
            &entry,
            1,
            zipFailure,
            sizeof(zipFailure)))
    {
        char text[1800] = {};
        sprintf_s(
            text,
            "[SaveSafety] ERROR: Compressed backup failed source=\"%s\" destination=\"%s\" reason=\"%s\". Live save will be preserved.\n",
            sourcePath,
            backupPath,
            zipFailure[0] != '\0' ? zipFailure : "unknown ZIP error");
        DiagLog(text);
        return false;
    }

    unsigned long long archiveSize = 0;
    QueryPathState(backupPath, &archiveSize);

    char text[1800] = {};
    sprintf_s(
        text,
        "[SaveSafety] Compressed backup created before transactional commit: source=\"%s\" destination=\"%s\" saveBytes=%llu zipBytes=%llu keep=%u.\n",
        sourcePath,
        backupPath,
        oldSize,
        archiveSize,
        g_config.saveSafetyBackupCount);
    DiagLog(text);

    try
    {
        RotateBackups(backupDirectory, g_config.saveSafetyBackupCount);
    }
    catch (const std::bad_alloc&)
    {
        DiagLog(
            "[SaveSafety] WARNING: Backup rotation skipped because memory allocation failed.\n");
    }
    return true;
}

bool BuildUniqueFailureDirectory(
    const char* backupDirectory,
    char* output,
    size_t outputCount)
{
    if (backupDirectory == nullptr || output == nullptr || outputCount == 0)
        return false;

    char failedRoot[kPathCapacity] = {};
    if (sprintf_s(failedRoot, "%s\\failed", backupDirectory) < 0 ||
        !EnsureDirectoryExists(failedRoot))
    {
        return false;
    }

    SYSTEMTIME localTime{};
    GetLocalTime(&localTime);

    char stem[128] = {};
    sprintf_s(
        stem,
        "failure_%04u-%02u-%02u_%02u%02u%02u_%03u",
        static_cast<unsigned>(localTime.wYear),
        static_cast<unsigned>(localTime.wMonth),
        static_cast<unsigned>(localTime.wDay),
        static_cast<unsigned>(localTime.wHour),
        static_cast<unsigned>(localTime.wMinute),
        static_cast<unsigned>(localTime.wSecond),
        static_cast<unsigned>(localTime.wMilliseconds));

    for (unsigned suffix = 0; suffix < 100; ++suffix)
    {
        if (suffix == 0)
            sprintf_s(output, outputCount, "%s\\%s", failedRoot, stem);
        else
            sprintf_s(output, outputCount, "%s\\%s_%02u", failedRoot, stem, suffix);

        char zipPath[kPathCapacity] = {};
        if (sprintf_s(zipPath, "%s.zip", output) < 0)
            return false;
        if (GetFileAttributesA(zipPath) != INVALID_FILE_ATTRIBUTES)
            continue;

        if (CreateDirectoryA(output, nullptr))
            return true;

        if (GetLastError() != ERROR_ALREADY_EXISTS)
            return false;
    }

    return false;
}

void RemoveFailureBundleDirectory(const char* path)
{
    if (path == nullptr || path[0] == '\0')
        return;

    static const char* const files[] = {
        "before.sav",
        "failed.sav",
        "ZachFix.log",
        "reason.txt"
    };

    for (const char* name : files)
    {
        char filePath[kPathCapacity] = {};
        if (sprintf_s(filePath, "%s\\%s", path, name) >= 0)
            DeleteFileA(filePath);
    }

    RemoveDirectoryA(path);
}

void RotateFailureBundles(const char* backupDirectory, UINT keepCount)
{
    if (backupDirectory == nullptr || keepCount == 0)
        return;

    char failedRoot[kPathCapacity] = {};
    char pattern[kPathCapacity] = {};
    if (sprintf_s(failedRoot, "%s\\failed", backupDirectory) < 0 ||
        sprintf_s(pattern, "%s\\failure_*", failedRoot) < 0)
    {
        return;
    }

    WIN32_FIND_DATAA findData{};
    HANDLE find = FindFirstFileA(pattern, &findData);
    if (find == INVALID_HANDLE_VALUE)
        return;

    struct FailureEntry
    {
        std::string name;
        bool directory = false;
    };

    std::vector<FailureEntry> entries;
    do
    {
        if (std::strcmp(findData.cFileName, ".") == 0 ||
            std::strcmp(findData.cFileName, "..") == 0)
        {
            continue;
        }

        entries.push_back({
            findData.cFileName,
            (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0
        });
    }
    while (FindNextFileA(find, &findData));
    FindClose(find);

    if (entries.size() <= keepCount)
        return;

    std::sort(
        entries.begin(),
        entries.end(),
        [](const FailureEntry& a, const FailureEntry& b)
        {
            return a.name < b.name;
        });

    const size_t removeCount = entries.size() - static_cast<size_t>(keepCount);
    for (size_t i = 0; i < removeCount; ++i)
    {
        char oldPath[kPathCapacity] = {};
        if (sprintf_s(oldPath, "%s\\%s", failedRoot, entries[i].name.c_str()) < 0)
            continue;

        if (entries[i].directory)
        {
            RemoveFailureBundleDirectory(oldPath);
            continue;
        }

        if (!DeleteFileA(oldPath))
        {
            const DWORD error = GetLastError();
            char text[1400] = {};
            sprintf_s(
                text,
                "[SaveSafety] WARNING: Could not prune old failure bundle path=\"%s\" error=%lu.\n",
                oldPath,
                static_cast<unsigned long>(error));
            DiagLog(text);
        }
    }
}

bool WriteFailureReasonFile(
    const char* path,
    const TrackedSnapshot& tracked,
    const char* reason,
    bool liveExisted,
    bool beforeCopied,
    bool failedCopied,
    bool logCopied)
{
    if (path == nullptr)
        return false;

    FILE* file = nullptr;
    if (fopen_s(&file, path, "wb") != 0 || file == nullptr)
        return false;

    std::fprintf(file, "ZachFix Save Safety failure bundle\r\n");
    std::fprintf(file, "Reason: %s\r\n", reason != nullptr ? reason : "unknown");
    std::fprintf(file, "Live save: %s\r\n", tracked.path);
    std::fprintf(file, "Transactional temp: %s\r\n", tracked.tempPath);
    std::fprintf(file, "before.sav: %s\r\n",
        !liveExisted ? "not present (no previous live save)" :
        (beforeCopied ? "copied" : "COPY FAILED"));
    std::fprintf(file, "failed.sav: %s\r\n", failedCopied ? "copied" : "COPY FAILED");
    std::fprintf(file, "ZachFix.log: %s\r\n", logCopied ? "copied" : "COPY FAILED");
    std::fclose(file);
    return true;
}

bool CaptureFailureBundle(const TrackedSnapshot& tracked, const char* reason)
{
    if (!g_config.saveSafetyEnabled)
        return false;

    std::lock_guard<std::mutex> lock(g_backupMutex);
    ScopedSaveSafetyBypass bypass;

    char backupDirectory[kPathCapacity] = {};
    if (!BuildBackupDirectory(backupDirectory, sizeof(backupDirectory)))
    {
        const DWORD error = GetLastError();
        char text[1400] = {};
        sprintf_s(
            text,
            "[SaveSafety] ERROR: Could not create failure-bundle root path error=%lu. Temp save remains at \"%s\".\n",
            static_cast<unsigned long>(error),
            tracked.tempPath);
        DiagLog(text);
        return false;
    }

    char failureDirectory[kPathCapacity] = {};
    if (!BuildUniqueFailureDirectory(
            backupDirectory,
            failureDirectory,
            sizeof(failureDirectory)))
    {
        const DWORD error = GetLastError();
        char text[1400] = {};
        sprintf_s(
            text,
            "[SaveSafety] ERROR: Could not create unique failure-bundle directory error=%lu. Temp save remains at \"%s\".\n",
            static_cast<unsigned long>(error),
            tracked.tempPath);
        DiagLog(text);
        return false;
    }

    char beforePath[kPathCapacity] = {};
    char failedPath[kPathCapacity] = {};
    char logPath[kPathCapacity] = {};
    char reasonPath[kPathCapacity] = {};
    sprintf_s(beforePath, "%s\\before.sav", failureDirectory);
    sprintf_s(failedPath, "%s\\failed.sav", failureDirectory);
    sprintf_s(logPath, "%s\\ZachFix.log", failureDirectory);
    sprintf_s(reasonPath, "%s\\reason.txt", failureDirectory);

    unsigned long long liveSize = 0;
    const bool liveExisted = QueryPathState(tracked.path, &liveSize);
    const bool beforeCopied = !liveExisted || CopyFileA(tracked.path, beforePath, TRUE) != FALSE;
    const bool failedCopied = CopyFileA(tracked.tempPath, failedPath, TRUE) != FALSE;

    char announce[1800] = {};
    sprintf_s(
        announce,
        "[SaveSafety] Capturing failure bundle directory=\"%s\" reason=\"%s\" before=%s failed=%s.\n",
        failureDirectory,
        reason != nullptr ? reason : "unknown",
        !liveExisted ? "n/a" : (beforeCopied ? "OK" : "FAIL"),
        failedCopied ? "OK" : "FAIL");
    DiagLog(announce);

    // Copy the log after the rejection and bundle announcement have been written,
    // so the diagnostic snapshot contains the actual failure context.
    const bool logCopied = CopyCurrentLogTo(logPath);
    const bool reasonWritten = WriteFailureReasonFile(
        reasonPath,
        tracked,
        reason,
        liveExisted,
        beforeCopied,
        failedCopied,
        logCopied);

    char failureZipPath[kPathCapacity] = {};
    const bool zipPathBuilt = sprintf_s(
        failureZipPath,
        "%s.zip",
        failureDirectory) >= 0;

    std::array<ZipFileInput, 4> zipInputs{};
    size_t zipInputCount = 0;
    if (liveExisted && beforeCopied)
        zipInputs[zipInputCount++] = { "before.sav", beforePath };
    if (failedCopied)
        zipInputs[zipInputCount++] = { "failed.sav", failedPath };
    if (logCopied)
        zipInputs[zipInputCount++] = { "ZachFix.log", logPath };
    if (reasonWritten)
        zipInputs[zipInputCount++] = { "reason.txt", reasonPath };

    char zipFailure[256] = {};
    const bool zipCreated = zipPathBuilt && zipInputCount != 0 &&
        CreateVerifiedZipArchive(
            failureZipPath,
            zipInputs.data(),
            zipInputCount,
            zipFailure,
            sizeof(zipFailure));

    const bool bundleComplete =
        failedCopied && logCopied && reasonWritten && beforeCopied;

    if (zipCreated)
    {
        unsigned long long archiveSize = 0;
        QueryPathState(failureZipPath, &archiveSize);
        RemoveFailureBundleDirectory(failureDirectory);

        bool tempRemoved = true;
        if (failedCopied && !DeleteFileA(tracked.tempPath))
        {
            const DWORD error = GetLastError();
            tempRemoved = error == ERROR_FILE_NOT_FOUND;
            if (!tempRemoved)
            {
                char warning[1600] = {};
                sprintf_s(
                    warning,
                    "[SaveSafety] WARNING: Failure ZIP is safe, but rejected temp could not be removed path=\"%s\" error=%lu.\n",
                    tracked.tempPath,
                    static_cast<unsigned long>(error));
                DiagLog(warning);
            }
        }

        char text[1900] = {};
        sprintf_s(
            text,
            "[SaveSafety] Failure bundle %s ZIP=\"%s\" zipBytes=%llu before=%s failed=%s log=%s reason=%s temp=%s.\n",
            bundleComplete ? "complete" : "partial",
            failureZipPath,
            archiveSize,
            !liveExisted ? "n/a" : (beforeCopied ? "OK" : "FAIL"),
            failedCopied ? "OK" : "FAIL",
            logCopied ? "OK" : "FAIL",
            reasonWritten ? "OK" : "FAIL",
            tempRemoved ? "archived/removed" : "retained");
        DiagLog(text);
    }
    else
    {
        char text[1900] = {};
        sprintf_s(
            text,
            "[SaveSafety] WARNING: Failure bundle ZIP compression failed reason=\"%s\". Raw fallback directory=\"%s\" retained; rejected temp remains at \"%s\".\n",
            zipFailure[0] != '\0' ? zipFailure :
                (zipPathBuilt ? "no files were available to archive" : "ZIP path is too long"),
            failureDirectory,
            tracked.tempPath);
        DiagLog(text);
    }

    try
    {
        RotateFailureBundles(backupDirectory, g_config.saveSafetyBackupCount);
    }
    catch (const std::bad_alloc&)
    {
        DiagLog(
            "[SaveSafety] WARNING: Failure-bundle rotation skipped because memory allocation failed.\n");
    }
    return bundleComplete && zipCreated;
}

bool WantsWrite(DWORD desiredAccess, DWORD creationDisposition)
{
    if ((desiredAccess & (GENERIC_WRITE | GENERIC_ALL)) != 0)
        return true;

    return creationDisposition == CREATE_NEW ||
           creationDisposition == CREATE_ALWAYS ||
           creationDisposition == OPEN_ALWAYS ||
           creationDisposition == TRUNCATE_EXISTING;
}

bool IsGameCaller(void* returnAddress)
{
    if (!g_mainExeInfoValid && !InitializeMainExeInfo())
        return false;

    const uintptr_t caller = reinterpret_cast<uintptr_t>(returnAddress);
    return caller >= g_mainExeBase && caller < g_mainExeBase + g_mainExeSize;
}

unsigned long CallerRva(void* returnAddress)
{
    if (!g_mainExeInfoValid && !InitializeMainExeInfo())
        return 0;

    const uintptr_t caller = reinterpret_cast<uintptr_t>(returnAddress);
    if (caller < g_mainExeBase || caller >= g_mainExeBase + g_mainExeSize)
        return 0;

    return static_cast<unsigned long>(caller - g_mainExeBase);
}

void ResolveDisplayPath(const char* fileName, char* output, size_t outputCount)
{
    if (output == nullptr || outputCount == 0)
        return;

    output[0] = '\0';
    if (fileName == nullptr)
        return;

    const DWORD length = GetFullPathNameA(
        fileName,
        static_cast<DWORD>(outputCount),
        output,
        nullptr);

    if (length == 0 || length >= outputCount)
        strncpy_s(output, outputCount, fileName, _TRUNCATE);
}

bool QueryPathState(const char* path, unsigned long long* size)
{
    if (size != nullptr)
        *size = 0;

    WIN32_FILE_ATTRIBUTE_DATA data{};
    if (path == nullptr || !GetFileAttributesExA(path, GetFileExInfoStandard, &data))
        return false;

    if (size != nullptr)
    {
        *size = (static_cast<unsigned long long>(data.nFileSizeHigh) << 32) |
                static_cast<unsigned long long>(data.nFileSizeLow);
    }
    return true;
}

bool QueryHandleSize(HANDLE handle, unsigned long long* size)
{
    if (size != nullptr)
        *size = 0;

    LARGE_INTEGER value{};
    if (handle == nullptr || handle == INVALID_HANDLE_VALUE || !GetFileSizeEx(handle, &value))
        return false;

    if (value.QuadPart < 0)
        return false;

    if (size != nullptr)
        *size = static_cast<unsigned long long>(value.QuadPart);
    return true;
}

uint32_t ReadU32(const std::vector<uint8_t>& data, size_t offset)
{
    uint32_t value = 0;
    if (offset + sizeof(value) <= data.size())
        std::memcpy(&value, data.data() + offset, sizeof(value));
    return value;
}

uint64_t ReadU64(const std::vector<uint8_t>& data, size_t offset)
{
    uint64_t value = 0;
    if (offset + sizeof(value) <= data.size())
        std::memcpy(&value, data.data() + offset, sizeof(value));
    return value;
}

bool ValidateTransactionalSaveFile(
    const char* path,
    char* failureReason,
    size_t failureReasonCount)
{
    auto fail = [&](const char* reason)
    {
        if (failureReason != nullptr && failureReasonCount != 0)
            strncpy_s(failureReason, failureReasonCount, reason, _TRUNCATE);
        return false;
    };

    if (failureReason != nullptr && failureReasonCount != 0)
        failureReason[0] = '\0';

    unsigned long long size = 0;
    if (!QueryPathState(path, &size))
        return fail("temp file is missing after close");
    if (size != kExpectedDpSaveSize)
        return fail("file size is not 0x7A2620");

    ScopedSaveSafetyBypass bypass;
    HANDLE file = g_originalCreateFileA(
        path,
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return fail("temp file could not be reopened for validation");

    std::vector<uint8_t> data;
    try
    {
        data.resize(static_cast<size_t>(kExpectedDpSaveSize));
    }
    catch (const std::bad_alloc&)
    {
        g_originalCloseHandle(file);
        return fail("not enough memory to read back temp save");
    }

    DWORD bytesRead = 0;
    const BOOL readResult = ReadFile(
        file,
        data.data(),
        static_cast<DWORD>(data.size()),
        &bytesRead,
        nullptr);
    const DWORD readError = GetLastError();
    g_originalCloseHandle(file);
    SetLastError(readError);

    if (!readResult || bytesRead != data.size())
        return fail("temp file could not be read back completely");

    const size_t record0 = kDpRecord0Offset;

    if (std::memcmp(data.data() + 0x20, data.data() + record0 + 0x88, 4) != 0)
        return fail("cash mirror mismatch");

    if (std::memcmp(data.data() + 0x3C, data.data() + record0 + 0xA0, 4) != 0)
        return fail("progress mirror mismatch");

    if (std::memcmp(data.data() + 0x110, data.data() + record0 + 0x439B4, 0x10) != 0)
        return fail("header state-block mirror mismatch");

    const uint64_t playtime = ReadU64(data, record0 + 0x42800);
    const uint32_t hours = ReadU32(data, 0x30);
    const uint32_t minutes = ReadU32(data, 0x34);
    const uint32_t seconds = ReadU32(data, 0x38);
    if (minutes >= 60 || seconds >= 60)
        return fail("header playtime minute/second field is out of range");

    const uint64_t headerPlaytime =
        static_cast<uint64_t>(hours) * 3600ull +
        static_cast<uint64_t>(minutes) * 60ull +
        static_cast<uint64_t>(seconds);
    if (headerPlaytime != playtime)
        return fail("playtime H:M:S does not match record0 total seconds");

    const uint32_t playtimeLow = static_cast<uint32_t>(playtime & 0xFFFFFFFFull);
    if (ReadU32(data, 0xE8) != playtimeLow || ReadU32(data, 0xEC) != playtimeLow)
        return fail("playtime statistic mirrors do not match record0");

    unsigned cards = 0;
    const size_t itemTable = record0 + kDpItemTableOffset;
    for (size_t itemId = 236; itemId <= 300; ++itemId)
    {
        if (ReadU32(data, itemTable + itemId * sizeof(uint32_t)) > 0)
            ++cards;
    }
    const uint32_t expectedCardPercent = static_cast<uint32_t>((cards * 100u) / 65u);
    if (ReadU32(data, 0xC8) != expectedCardPercent)
        return fail("Trading Card percentage does not match item state");

    unsigned completedSideMissions = 0;
    const size_t eventBits = record0 + kDpEventBitsetOffset;
    for (unsigned mission = 0; mission < 50; ++mission)
    {
        const size_t bitIndex = 0xB00u + 50u + mission;
        const uint8_t value = data[eventBits + bitIndex / 8u];
        if ((value & static_cast<uint8_t>(1u << (bitIndex & 7u))) != 0)
            ++completedSideMissions;
    }
    if (ReadU32(data, 0x8C) != completedSideMissions * 2u)
        return fail("Side Mission percentage does not match event state");

    return true;
}

bool CommitTransactionalSave(
    const TrackedSnapshot& tracked,
    DWORD* failureError)
{
    if (failureError != nullptr)
        *failureError = ERROR_WRITE_FAULT;

    char validationReason[256] = {};
    if (!ValidateTransactionalSaveFile(
            tracked.tempPath,
            validationReason,
            sizeof(validationReason)))
    {
        char text[1600] = {};
        sprintf_s(
            text,
            "[SaveSafety] REJECTED temp save path=\"%s\" reason=\"%s\". Existing dp.sav preserved.\n",
            tracked.tempPath,
            validationReason);
        DiagLog(text);

        char bundleReason[512] = {};
        sprintf_s(bundleReason, "validation failed: %s", validationReason);
        CaptureFailureBundle(tracked, bundleReason);
        if (failureError != nullptr)
            *failureError = ERROR_INVALID_DATA;
        return false;
    }

    unsigned long long liveSize = 0;
    const bool liveExists = QueryPathState(tracked.path, &liveSize);
    if (liveExists && !BackupExistingSave(tracked.path, liveSize))
    {
        DiagLog(
            "[SaveSafety] REJECTED transactional commit because the previous live save could not be backed up. Existing dp.sav preserved.\n");
        CaptureFailureBundle(tracked, "previous live save backup failed before commit");
        if (failureError != nullptr)
            *failureError = ERROR_WRITE_FAULT;
        return false;
    }

    ScopedSaveSafetyBypass bypass;
    if (!MoveFileExA(
            tracked.tempPath,
            tracked.path,
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        const DWORD error = GetLastError();
        if (failureError != nullptr)
            *failureError = error;

        char text[1600] = {};
        sprintf_s(
            text,
            "[SaveSafety] ERROR: Transactional replace failed temp=\"%s\" live=\"%s\" error=%lu. Existing dp.sav preserved; temp retained.\n",
            tracked.tempPath,
            tracked.path,
            static_cast<unsigned long>(error));
        DiagLog(text);

        char bundleReason[256] = {};
        sprintf_s(
            bundleReason,
            "transactional replace failed (Win32 error %lu)",
            static_cast<unsigned long>(error));
        CaptureFailureBundle(tracked, bundleReason);
        return false;
    }

    char text[1600] = {};
    sprintf_s(
        text,
        "[SaveSafety] Transactional commit complete temp=\"%s\" -> live=\"%s\" bytes=%llu validation=PASS.\n",
        tracked.tempPath,
        tracked.path,
        kExpectedDpSaveSize);
    DiagLog(text);
    return true;
}

const char* CreationDispositionName(DWORD value)
{
    switch (value)
    {
    case CREATE_NEW: return "CREATE_NEW";
    case CREATE_ALWAYS: return "CREATE_ALWAYS";
    case OPEN_EXISTING: return "OPEN_EXISTING";
    case OPEN_ALWAYS: return "OPEN_ALWAYS";
    case TRUNCATE_EXISTING: return "TRUNCATE_EXISTING";
    default: return "UNKNOWN";
    }
}

const char* MoveMethodName(DWORD value)
{
    switch (value)
    {
    case FILE_BEGIN: return "FILE_BEGIN";
    case FILE_CURRENT: return "FILE_CURRENT";
    case FILE_END: return "FILE_END";
    default: return "UNKNOWN";
    }
}

bool TrackHandle(
    HANDLE handle,
    const char* path,
    bool transactional = false,
    const char* tempPath = nullptr)
{
    if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
        return false;

    std::lock_guard<std::mutex> lock(g_trackedMutex);

    for (TrackedFile& tracked : g_trackedFiles)
    {
        if (tracked.handle == handle)
        {
            tracked = {};
            tracked.handle = handle;
            strncpy_s(tracked.path, sizeof(tracked.path), path != nullptr ? path : "", _TRUNCATE);
            strncpy_s(tracked.tempPath, sizeof(tracked.tempPath), tempPath != nullptr ? tempPath : "", _TRUNCATE);
            tracked.transactional = transactional;
            return true;
        }
    }

    for (TrackedFile& tracked : g_trackedFiles)
    {
        if (tracked.handle == nullptr)
        {
            tracked.handle = handle;
            strncpy_s(tracked.path, sizeof(tracked.path), path != nullptr ? path : "", _TRUNCATE);
            strncpy_s(tracked.tempPath, sizeof(tracked.tempPath), tempPath != nullptr ? tempPath : "", _TRUNCATE);
            tracked.transactional = transactional;
            return true;
        }
    }

    return false;
}

TrackedSnapshot GetTrackedSnapshot(HANDLE handle)
{
    if (handle == nullptr || handle == INVALID_HANDLE_VALUE)
        return {};

    std::lock_guard<std::mutex> lock(g_trackedMutex);
    for (const TrackedFile& tracked : g_trackedFiles)
    {
        if (tracked.handle != handle)
            continue;

        TrackedSnapshot snapshot{};
        snapshot.found = true;
        strncpy_s(snapshot.path, sizeof(snapshot.path), tracked.path, _TRUNCATE);
        strncpy_s(snapshot.tempPath, sizeof(snapshot.tempPath), tracked.tempPath, _TRUNCATE);
        snapshot.writeCalls = tracked.writeCalls;
        snapshot.bytesWritten = tracked.bytesWritten;
        snapshot.transactional = tracked.transactional;
        return snapshot;
    }
    return {};
}

void RemoveTrackedHandle(HANDLE handle)
{
    std::lock_guard<std::mutex> lock(g_trackedMutex);
    for (TrackedFile& tracked : g_trackedFiles)
    {
        if (tracked.handle == handle)
        {
            tracked = {};
            return;
        }
    }
}

bool UpdateWriteStats(
    HANDLE handle,
    DWORD bytesWritten,
    unsigned* callIndex,
    bool* logSuppression)
{
    std::lock_guard<std::mutex> lock(g_trackedMutex);
    for (TrackedFile& tracked : g_trackedFiles)
    {
        if (tracked.handle != handle)
            continue;

        ++tracked.writeCalls;
        tracked.bytesWritten += bytesWritten;

        if (callIndex != nullptr)
            *callIndex = tracked.writeCalls;

        if (logSuppression != nullptr)
        {
            *logSuppression =
                tracked.writeCalls == kWriteLogLimit + 1 &&
                !tracked.writeSuppressionLogged;
            if (*logSuppression)
                tracked.writeSuppressionLogged = true;
        }
        return true;
    }
    return false;
}

HANDLE WINAPI HookCreateFileA(
    LPCSTR fileName,
    DWORD desiredAccess,
    DWORD shareMode,
    LPSECURITY_ATTRIBUTES securityAttributes,
    DWORD creationDisposition,
    DWORD flagsAndAttributes,
    HANDLE templateFile)
{
    if (g_saveSafetyBypass || g_originalCreateFileA == nullptr)
    {
        return g_originalCreateFileA != nullptr
            ? g_originalCreateFileA(
                fileName, desiredAccess, shareMode, securityAttributes,
                creationDisposition, flagsAndAttributes, templateFile)
            : INVALID_HANDLE_VALUE;
    }

    const DWORD entryLastError = GetLastError();

    void* returnAddress = _ReturnAddress();
    if (!IsGameCaller(returnAddress))
    {
        SetLastError(entryLastError);
        return g_originalCreateFileA(
            fileName, desiredAccess, shareMode, securityAttributes,
            creationDisposition, flagsAndAttributes, templateFile);
    }

    char path[kPathCapacity] = {};
    ResolveDisplayPath(fileName, path, sizeof(path));

    const bool primaryDpSave = IsPrimaryDpSavePath(path);
    char profilePath[kPathCapacity] = {};
    if (primaryDpSave &&
        !BuildDifficultyProfileSavePath(path, profilePath, sizeof(profilePath), true))
    {
        DiagLog("[Difficulty] ERROR: Could not resolve the active difficulty save profile path.\n");
        SetLastError(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    const char* physicalPath = primaryDpSave ? profilePath : path;

    const bool candidate = IsCandidateSavePath(path);
    const bool nearbyWrite = IsSaveDirectoryPath(path) && WantsWrite(desiredAccess, creationDisposition);
    if (!candidate && !nearbyWrite)
    {
        SetLastError(entryLastError);
        return g_originalCreateFileA(
            fileName, desiredAccess, shareMode, securityAttributes,
            creationDisposition, flagsAndAttributes, templateFile);
    }

    unsigned long long oldSize = 0;
    const bool existedBefore = QueryPathState(physicalPath, &oldSize);

    char text[1536] = {};
    sprintf_s(
        text,
        "[SaveDiag] CreateFileA PRE path=\"%s\" access=0x%08lX share=0x%08lX "
        "disposition=%s(%lu) flags=0x%08lX existed=%s oldSize=%llu caller=DP.exe+0x%08lX%s%s\n",
        path,
        static_cast<unsigned long>(desiredAccess),
        static_cast<unsigned long>(shareMode),
        CreationDispositionName(creationDisposition),
        static_cast<unsigned long>(creationDisposition),
        static_cast<unsigned long>(flagsAndAttributes),
        existedBefore ? "yes" : "no",
        oldSize,
        CallerRva(returnAddress),
        candidate ? " candidate=.sav" : " nearby=savedata",
        primaryDpSave ? " profiled" : "");
    DiagLog(text);

    const bool transactional =
        g_transactionalSaveReady.load(std::memory_order_acquire) &&
        g_config.saveSafetyEnabled &&
        candidate &&
        primaryDpSave &&
        IsDestructiveOpen(desiredAccess, creationDisposition);

    bool transactionGateHeld = false;
    if (transactional)
    {
        bool expected = false;
        if (!g_transactionActive.compare_exchange_strong(
                expected, true, std::memory_order_acq_rel))
        {
            DiagLog(
                "[SaveSafety] REJECTED overlapping destructive dp.sav open; "
                "another save transaction is still active. Existing dp.sav preserved.\n");
            SetLastError(ERROR_BUSY);
            return INVALID_HANDLE_VALUE;
        }
        transactionGateHeld = true;
    }

    char tempPath[kPathCapacity] = {};
    if (transactional && !BuildTransactionalTempPath(physicalPath, tempPath, sizeof(tempPath)))
    {
        DiagLog(
            "[SaveSafety] ERROR: Could not build transactional temp path. Existing dp.sav preserved.\n");
        if (transactionGateHeld)
            g_transactionActive.store(false, std::memory_order_release);
        SetLastError(ERROR_BUFFER_OVERFLOW);
        return INVALID_HANDLE_VALUE;
    }

    // Path probing/logging above must not change the LastError state that the
    // original CreateFileA would have observed.
    SetLastError(entryLastError);
    HANDLE result = g_originalCreateFileA(
        transactional ? tempPath : (primaryDpSave ? physicalPath : fileName),
        desiredAccess,
        shareMode,
        securityAttributes,
        transactional ? CREATE_ALWAYS : creationDisposition,
        flagsAndAttributes,
        templateFile);
    const DWORD lastError = GetLastError();

    if (transactionGateHeld && result == INVALID_HANDLE_VALUE)
    {
        g_transactionActive.store(false, std::memory_order_release);
        transactionGateHeld = false;
    }

    sprintf_s(
        text,
        "[SaveDiag] CreateFileA POST path=\"%s\" physical=\"%s\" handle=%p result=%s lastError=%lu\n",
        path,
        transactional ? tempPath : physicalPath,
        result,
        result != INVALID_HANDLE_VALUE ? "OK" : "FAIL",
        static_cast<unsigned long>(lastError));
    DiagLog(text);

    bool trackedSuccessfully = true;
    if (candidate && result != INVALID_HANDLE_VALUE && WantsWrite(desiredAccess, creationDisposition))
    {
        trackedSuccessfully = TrackHandle(
            result, physicalPath, transactional, transactional ? tempPath : nullptr);
    }

    if (transactional && result != INVALID_HANDLE_VALUE && !trackedSuccessfully)
    {
        DiagLog(
            "[SaveSafety] ERROR: Transactional handle could not be tracked; "
            "aborting save before any live-file commit.\n");

        BOOL closeResult = FALSE;
        DWORD closeError = ERROR_SUCCESS;
        {
            ScopedSaveSafetyBypass bypass;
            closeResult = g_originalCloseHandle(result);
            closeError = closeResult ? ERROR_SUCCESS : GetLastError();
            if (closeResult)
                DeleteFileA(tempPath);
        }

        if (closeResult)
        {
            g_transactionActive.store(false, std::memory_order_release);
        }
        else
        {
            char failure[512] = {};
            sprintf_s(
                failure,
                "[SaveSafety] ERROR: Emergency CloseHandle for untracked temp failed "
                "error=%lu; transaction gate remains locked to prevent temp reuse.\n",
                static_cast<unsigned long>(closeError));
            DiagLog(failure);
        }

        SetLastError(closeResult ? ERROR_BUSY : closeError);
        return INVALID_HANDLE_VALUE;
    }
    else if (!trackedSuccessfully)
    {
        DiagLog("[SaveDiag] WARNING: candidate handle table is full; save handle not tracked.\n");
    }

    if (transactional)
    {
        sprintf_s(
            text,
            "[SaveSafety] %s destructive dp.sav open -> temp=\"%s\". Live save remains untouched until close/validation/commit.\n",
            result != INVALID_HANDLE_VALUE ? "Redirected" : "FAILED to redirect",
            tempPath);
        DiagLog(text);
    }

    SetLastError(lastError);
    return result;
}

BOOL WINAPI HookWriteFile(
    HANDLE file,
    LPCVOID buffer,
    DWORD bytesToWrite,
    LPDWORD bytesWritten,
    LPOVERLAPPED overlapped)
{
    if (g_originalWriteFile == nullptr)
        return FALSE;

    if (g_saveSafetyBypass)
        return g_originalWriteFile(file, buffer, bytesToWrite, bytesWritten, overlapped);

    const TrackedSnapshot before = GetTrackedSnapshot(file);
    if (!before.found)
        return g_originalWriteFile(file, buffer, bytesToWrite, bytesWritten, overlapped);

    const BOOL result = g_originalWriteFile(file, buffer, bytesToWrite, bytesWritten, overlapped);
    const DWORD lastError = GetLastError();

    const DWORD completedBytes = result && bytesWritten != nullptr ? *bytesWritten : 0;
    unsigned callIndex = 0;
    bool logSuppression = false;
    UpdateWriteStats(file, completedBytes, &callIndex, &logSuppression);

    if (callIndex <= kWriteLogLimit)
    {
        char text[1400] = {};
        sprintf_s(
            text,
            "[SaveDiag] WriteFile #%u path=\"%s\" request=%lu wrote=%lu result=%s "
            "overlapped=%s lastError=%lu\n",
            callIndex,
            before.path,
            static_cast<unsigned long>(bytesToWrite),
            static_cast<unsigned long>(completedBytes),
            result ? "OK" : "FAIL",
            overlapped != nullptr ? "yes" : "no",
            static_cast<unsigned long>(lastError));
        DiagLog(text);
    }
    else if (logSuppression)
    {
        DiagLog("[SaveDiag] WriteFile: further per-call entries suppressed; totals will be logged on CloseHandle.\n");
    }

    SetLastError(lastError);
    return result;
}

DWORD WINAPI HookSetFilePointer(
    HANDLE file,
    LONG distanceLow,
    PLONG distanceHigh,
    DWORD moveMethod)
{
    if (g_originalSetFilePointer == nullptr)
        return INVALID_SET_FILE_POINTER;

    if (g_saveSafetyBypass)
        return g_originalSetFilePointer(file, distanceLow, distanceHigh, moveMethod);

    const TrackedSnapshot tracked = GetTrackedSnapshot(file);
    if (!tracked.found)
        return g_originalSetFilePointer(file, distanceLow, distanceHigh, moveMethod);

    LONG inputHigh = distanceHigh != nullptr ? *distanceHigh : 0;
    DWORD result = g_originalSetFilePointer(file, distanceLow, distanceHigh, moveMethod);
    const DWORD lastError = GetLastError();
    LONG outputHigh = distanceHigh != nullptr ? *distanceHigh : 0;

    char text[1400] = {};
    sprintf_s(
        text,
        "[SaveDiag] SetFilePointer path=\"%s\" low=%ld highIn=%ld method=%s(%lu) "
        "resultLow=%lu highOut=%ld lastError=%lu\n",
        tracked.path,
        static_cast<long>(distanceLow),
        static_cast<long>(inputHigh),
        MoveMethodName(moveMethod),
        static_cast<unsigned long>(moveMethod),
        static_cast<unsigned long>(result),
        static_cast<long>(outputHigh),
        static_cast<unsigned long>(lastError));
    DiagLog(text);

    SetLastError(lastError);
    return result;
}

BOOL WINAPI HookSetEndOfFile(HANDLE file)
{
    if (g_originalSetEndOfFile == nullptr)
        return FALSE;

    if (g_saveSafetyBypass)
        return g_originalSetEndOfFile(file);

    const TrackedSnapshot tracked = GetTrackedSnapshot(file);
    if (!tracked.found)
        return g_originalSetEndOfFile(file);

    const DWORD entryLastError = GetLastError();
    unsigned long long sizeBefore = 0;
    const bool hadSizeBefore = QueryHandleSize(file, &sizeBefore);

    char text[1400] = {};
    sprintf_s(
        text,
        "[SaveDiag] SetEndOfFile PRE path=\"%s\" sizeBefore=%s%llu\n",
        tracked.path,
        hadSizeBefore ? "" : "unknown/",
        sizeBefore);
    DiagLog(text);

    SetLastError(entryLastError);
    const BOOL result = g_originalSetEndOfFile(file);
    const DWORD lastError = GetLastError();

    unsigned long long sizeAfter = 0;
    const bool hadSizeAfter = QueryHandleSize(file, &sizeAfter);
    sprintf_s(
        text,
        "[SaveDiag] SetEndOfFile POST path=\"%s\" result=%s sizeAfter=%s%llu lastError=%lu\n",
        tracked.path,
        result ? "OK" : "FAIL",
        hadSizeAfter ? "" : "unknown/",
        sizeAfter,
        static_cast<unsigned long>(lastError));
    DiagLog(text);

    SetLastError(lastError);
    return result;
}

BOOL WINAPI HookFlushFileBuffers(HANDLE file)
{
    if (g_originalFlushFileBuffers == nullptr)
        return FALSE;

    if (g_saveSafetyBypass)
        return g_originalFlushFileBuffers(file);

    const TrackedSnapshot tracked = GetTrackedSnapshot(file);
    if (!tracked.found)
        return g_originalFlushFileBuffers(file);

    const BOOL result = g_originalFlushFileBuffers(file);
    const DWORD lastError = GetLastError();

    char text[1400] = {};
    sprintf_s(
        text,
        "[SaveDiag] FlushFileBuffers path=\"%s\" result=%s lastError=%lu\n",
        tracked.path,
        result ? "OK" : "FAIL",
        static_cast<unsigned long>(lastError));
    DiagLog(text);

    SetLastError(lastError);
    return result;
}

BOOL WINAPI HookDeleteFileA(LPCSTR fileName)
{
    if (g_originalDeleteFileA == nullptr)
        return FALSE;

    if (g_saveSafetyBypass)
        return g_originalDeleteFileA(fileName);

    const DWORD entryLastError = GetLastError();
    if (!IsGameCaller(_ReturnAddress()))
    {
        SetLastError(entryLastError);
        return g_originalDeleteFileA(fileName);
    }

    char path[kPathCapacity] = {};
    ResolveDisplayPath(fileName, path, sizeof(path));
    const bool primaryDpSave = IsPrimaryDpSavePath(path);
    char profilePath[kPathCapacity] = {};
    if (primaryDpSave &&
        !BuildDifficultyProfileSavePath(path, profilePath, sizeof(profilePath), true))
    {
        SetLastError(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    const char* physicalPath = primaryDpSave ? profilePath : path;
    if (!IsCandidateSavePath(path) && !IsSaveDirectoryPath(path))
    {
        SetLastError(entryLastError);
        return g_originalDeleteFileA(fileName);
    }

    unsigned long long oldSize = 0;
    const bool existedBefore = QueryPathState(physicalPath, &oldSize);

    char text[1400] = {};
    sprintf_s(
        text,
        "[SaveDiag] DeleteFileA PRE path=\"%s\" existed=%s oldSize=%llu\n",
        path,
        existedBefore ? "yes" : "no",
        oldSize);
    DiagLog(text);

    SetLastError(entryLastError);
    const BOOL result = g_originalDeleteFileA(primaryDpSave ? physicalPath : fileName);
    const DWORD lastError = GetLastError();

    sprintf_s(
        text,
        "[SaveDiag] DeleteFileA POST path=\"%s\" result=%s lastError=%lu\n",
        path,
        result ? "OK" : "FAIL",
        static_cast<unsigned long>(lastError));
    DiagLog(text);

    SetLastError(lastError);
    return result;
}

BOOL WINAPI HookCloseHandle(HANDLE handle)
{
    if (g_originalCloseHandle == nullptr)
        return FALSE;

    if (g_saveSafetyBypass)
        return g_originalCloseHandle(handle);

    const TrackedSnapshot tracked = GetTrackedSnapshot(handle);
    if (!tracked.found)
        return g_originalCloseHandle(handle);

    BOOL flushResult = TRUE;
    DWORD flushError = ERROR_SUCCESS;
    if (tracked.transactional)
    {
        if (g_originalFlushFileBuffers != nullptr)
        {
            flushResult = g_originalFlushFileBuffers(handle);
            flushError = flushResult ? ERROR_SUCCESS : GetLastError();
        }
        else
        {
            flushResult = FALSE;
            flushError = ERROR_PROC_NOT_FOUND;
        }
    }

    unsigned long long finalSize = 0;
    const bool hadFinalSize = QueryHandleSize(handle, &finalSize);

    const BOOL result = g_originalCloseHandle(handle);
    const DWORD lastError = GetLastError();

    char text[1536] = {};
    sprintf_s(
        text,
        "[SaveDiag] CloseHandle path=\"%s\" result=%s writes=%u bytesWritten=%llu "
        "finalSize=%s%llu flush=%s flushError=%lu lastError=%lu\n",
        tracked.path,
        result ? "OK" : "FAIL",
        tracked.writeCalls,
        tracked.bytesWritten,
        hadFinalSize ? "" : "unknown/",
        finalSize,
        tracked.transactional ? (flushResult ? "OK" : "FAIL") : "n/a",
        static_cast<unsigned long>(tracked.transactional ? flushError : ERROR_SUCCESS),
        static_cast<unsigned long>(lastError));
    DiagLog(text);

    if (result)
        RemoveTrackedHandle(handle);

    if (!tracked.transactional || !result)
    {
        SetLastError(lastError);
        return result;
    }

    struct TransactionGateRelease
    {
        ~TransactionGateRelease()
        {
            g_transactionActive.store(false, std::memory_order_release);
        }
    } transactionGateRelease;

    if (!flushResult)
    {
        sprintf_s(
            text,
            "[SaveSafety] REJECTED temp save because FlushFileBuffers failed error=%lu. Existing dp.sav preserved; temp retained.\n",
            static_cast<unsigned long>(flushError));
        DiagLog(text);

        char bundleReason[256] = {};
        sprintf_s(
            bundleReason,
            "FlushFileBuffers failed (Win32 error %lu)",
            static_cast<unsigned long>(flushError));
        CaptureFailureBundle(tracked, bundleReason);
        SetLastError(flushError != ERROR_SUCCESS ? flushError : ERROR_WRITE_FAULT);
        return FALSE;
    }

    if (!hadFinalSize || finalSize != kExpectedDpSaveSize)
    {
        sprintf_s(
            text,
            "[SaveSafety] REJECTED temp save because final size is %s%llu, expected=%llu. Existing dp.sav preserved; temp retained.\n",
            hadFinalSize ? "" : "unknown/",
            finalSize,
            kExpectedDpSaveSize);
        DiagLog(text);

        char bundleReason[320] = {};
        sprintf_s(
            bundleReason,
            "final temp size mismatch: actual=%s%llu expected=%llu",
            hadFinalSize ? "" : "unknown/",
            finalSize,
            kExpectedDpSaveSize);
        CaptureFailureBundle(tracked, bundleReason);
        SetLastError(ERROR_BAD_LENGTH);
        return FALSE;
    }

    DWORD commitError = ERROR_WRITE_FAULT;
    if (!CommitTransactionalSave(tracked, &commitError))
    {
        SetLastError(commitError);
        return FALSE;
    }

    SetLastError(lastError);
    return TRUE;
}

bool InstallOneHook(
    HMODULE module,
    const char* name,
    void* detour,
    void** original)
{
    FARPROC proc = module != nullptr ? GetProcAddress(module, name) : nullptr;
    if (proc == nullptr)
    {
        char text[192] = {};
        sprintf_s(text, "[SaveDiag] WARNING: %s export not found.\n", name);
        DiagLog(text);
        return false;
    }

    const MH_STATUS createStatus = MH_CreateHook(
        reinterpret_cast<void*>(proc), detour, original);
    if (createStatus != MH_OK)
    {
        char text[224] = {};
        sprintf_s(
            text,
            "[SaveDiag] WARNING: MH_CreateHook(%s) failed: %d.\n",
            name,
            static_cast<int>(createStatus));
        DiagLog(text);
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(reinterpret_cast<void*>(proc));
    if (enableStatus != MH_OK)
    {
        char text[224] = {};
        sprintf_s(
            text,
            "[SaveDiag] WARNING: MH_EnableHook(%s) failed: %d.\n",
            name,
            static_cast<int>(enableStatus));
        DiagLog(text);
        return false;
    }

    return true;
}
} // namespace

bool InstallSaveDiagHooks()
{
    g_transactionalSaveReady.store(false, std::memory_order_release);
    g_transactionActive.store(false, std::memory_order_release);

    if (!PrepareDifficultySaveProfile())
    {
        DiagLog("[Difficulty] WARNING: Save profile preparation failed; dp.sav routing may be unavailable.\n");
    }

    HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");
    if (kernel32 == nullptr)
    {
        AppendLog("[SaveDiag] ERROR: kernel32.dll unavailable; diagnostics disabled.\n");
        return false;
    }

    unsigned installed = 0;
    installed += InstallOneHook(
        kernel32, "CreateFileA",
        reinterpret_cast<void*>(&HookCreateFileA),
        reinterpret_cast<void**>(&g_originalCreateFileA)) ? 1u : 0u;
    installed += InstallOneHook(
        kernel32, "WriteFile",
        reinterpret_cast<void*>(&HookWriteFile),
        reinterpret_cast<void**>(&g_originalWriteFile)) ? 1u : 0u;
    installed += InstallOneHook(
        kernel32, "SetFilePointer",
        reinterpret_cast<void*>(&HookSetFilePointer),
        reinterpret_cast<void**>(&g_originalSetFilePointer)) ? 1u : 0u;
    installed += InstallOneHook(
        kernel32, "SetEndOfFile",
        reinterpret_cast<void*>(&HookSetEndOfFile),
        reinterpret_cast<void**>(&g_originalSetEndOfFile)) ? 1u : 0u;
    installed += InstallOneHook(
        kernel32, "FlushFileBuffers",
        reinterpret_cast<void*>(&HookFlushFileBuffers),
        reinterpret_cast<void**>(&g_originalFlushFileBuffers)) ? 1u : 0u;
    installed += InstallOneHook(
        kernel32, "DeleteFileA",
        reinterpret_cast<void*>(&HookDeleteFileA),
        reinterpret_cast<void**>(&g_originalDeleteFileA)) ? 1u : 0u;
    installed += InstallOneHook(
        kernel32, "CloseHandle",
        reinterpret_cast<void*>(&HookCloseHandle),
        reinterpret_cast<void**>(&g_originalCloseHandle)) ? 1u : 0u;

    char text[320] = {};
    sprintf_s(
        text,
        "[SaveDiag] Save tracing active (%u/7 hooks).\n",
        installed);
    DiagLog(text);

    const bool fullHookSetInstalled = installed == 7;
    g_transactionalSaveReady.store(fullHookSetInstalled, std::memory_order_release);

    sprintf_s(
        text,
        "[SaveSafety] Transactional dp.sav protection %s, profile=%s, keep=%u backups. Temp writes are validated before backup/commit.\n",
        g_config.saveSafetyEnabled && fullHookSetInstalled ? "enabled" : "disabled",
        GetSessionDifficultyProfileName(),
        g_config.saveSafetyBackupCount);
    DiagLog(text);

    if (g_config.saveSafetyEnabled && !fullHookSetInstalled)
    {
        DiagLog(
            "[SaveSafety] WARNING: Full save hook set was not installed; transactional redirection is disabled and DP will use its vanilla save path.\n");
    }

    return fullHookSetInstalled;
}
