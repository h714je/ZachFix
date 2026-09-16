#include "main_exe.h"

#include <mutex>

uintptr_t g_mainExeBase = 0;
size_t g_mainExeSize = 0;
DWORD g_mainExeTimeDateStamp = 0;

std::atomic_bool g_mainExeInfoValid{ false };

namespace
{
std::mutex g_mainExeInfoMutex;
}

bool InitializeMainExeInfo()
{
    if (g_mainExeInfoValid.load(std::memory_order_acquire))
        return true;

    std::lock_guard<std::mutex> lock(g_mainExeInfoMutex);
    if (g_mainExeInfoValid.load(std::memory_order_relaxed))
        return true;

    HMODULE module =
        GetModuleHandleW(nullptr);

    if (module == nullptr)
        return false;

    const auto* dos =
        reinterpret_cast<const IMAGE_DOS_HEADER*>(
            module
        );

    if (dos->e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    const auto* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS32*>(
            reinterpret_cast<const unsigned char*>(module) +
            dos->e_lfanew
        );

    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic !=
            IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        return false;
    }

    g_mainExeBase =
        reinterpret_cast<uintptr_t>(module);

    g_mainExeSize =
        static_cast<size_t>(
            nt->OptionalHeader.SizeOfImage
        );

    g_mainExeTimeDateStamp =
        nt->FileHeader.TimeDateStamp;

    const bool valid =
        g_mainExeBase != 0 &&
        g_mainExeSize != 0;
    g_mainExeInfoValid.store(valid, std::memory_order_release);

    return valid;
}
