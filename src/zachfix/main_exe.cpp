#include "main_exe.h"

uintptr_t g_mainExeBase = 0;
size_t g_mainExeSize = 0;
DWORD g_mainExeTimeDateStamp = 0;

bool g_mainExeInfoValid = false;

bool InitializeMainExeInfo()
{
    if (g_mainExeInfoValid)
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

    g_mainExeInfoValid =
        g_mainExeBase != 0 &&
        g_mainExeSize != 0;

    return g_mainExeInfoValid;
}
