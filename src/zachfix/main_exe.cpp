#include "main_exe.h"

#include <mutex>

uintptr_t g_mainExeBase = 0;
size_t g_mainExeSize = 0;
DWORD g_mainExeTimeDateStamp = 0;

std::atomic_bool g_mainExeInfoValid{ false };

namespace
{
std::mutex g_mainExeInfoMutex;

constexpr DpBuildProfile kSteam101bProfile{
    DpBuild::Steam101b,
    "Steam 1.01b",
    0x529721DC,
    0x010B5000,
    0x0036E264,
    0x0018CB09,
    0x010AFFE0,
    0x002B1780,
    0x00309400,
    0x00308A30,
    0x010810F0,
    0x00309C40,
    0x001E6D40,
    0x001EC37F,
    0x002C6296,
    0x002D396F,
    0x002DBD30,
    0x002E15F0,
    0x00335BB0,
    0x0014C1D1
};

constexpr DpBuildProfile kGog101bProfile{
    DpBuild::Gog101b,
    "GOG 1.01b",
    0x52970AF6,
    0x010B5000,
    0x0036E264,
    0x0018CBD9,
    0x010AFFE0,
    0x002B1780,
    0x003093B0,
    0x003089E0,
    0x010810F0,
    0x00309BA0,
    0x001E6E10,
    0x001EC44F,
    0x002C5D96,
    0x002D353F,
    0x002DB900,
    0x002E1630,
    0x003358C0,
    0x0014C2A1
};

const DpBuildProfile* FindDpBuildProfile(
    DWORD timeDateStamp,
    size_t sizeOfImage)
{
    if (timeDateStamp == kSteam101bProfile.timeDateStamp &&
        sizeOfImage == kSteam101bProfile.sizeOfImage)
    {
        return &kSteam101bProfile;
    }

    if (timeDateStamp == kGog101bProfile.timeDateStamp &&
        sizeOfImage == kGog101bProfile.sizeOfImage)
    {
        return &kGog101bProfile;
    }

    return nullptr;
}
} // namespace

const DpBuildProfile* DetectDpBuildProfile(HMODULE module)
{
    if (module == nullptr)
        return nullptr;

    const auto* base =
        reinterpret_cast<const unsigned char*>(module);
    const auto* dos =
        reinterpret_cast<const IMAGE_DOS_HEADER*>(base);

    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
        return nullptr;

    const auto* nt =
        reinterpret_cast<const IMAGE_NT_HEADERS32*>(
            base + dos->e_lfanew);

    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        return nullptr;
    }

    return FindDpBuildProfile(
        nt->FileHeader.TimeDateStamp,
        static_cast<size_t>(nt->OptionalHeader.SizeOfImage));
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

    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
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

const DpBuildProfile* GetDpBuildProfile()
{
    if (!InitializeMainExeInfo())
        return nullptr;

    return FindDpBuildProfile(
        g_mainExeTimeDateStamp,
        g_mainExeSize);
}
