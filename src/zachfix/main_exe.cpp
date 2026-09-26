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
    {
        0x0036E264, // direct3DCreate9IatRva
        0x0018CB09, // speedDivideRva
        0x010AFFE0, // frameDeltaRva
        0x00001010  // currentGameStateGetterRva
    },
    {
        0x002B1780, // controllerBindingEvaluatorRva
        0x00309400, // stickAxisPostProcessorRva
        0x00308A30, // stickFloatGetterRva
        { 0x0013C010, 0x0013C043, 0x0013C268, 0x0013C29B },
        0x010810F0, // useJoyModeRva
        0x00309C40, // inputUpdateRva
        0x002E15F0, // rdInputSetActuatorRva
        0x00335BB0, // inputActuatorSetSecondRva
        0x0014C1D1  // vehicleAnalogInputInjectRva
    },
    {
        0x00104497, // combatStrafeHookRva
        0x000FEE60, // combatCapabilityRva
        0x00128F40  // stateTransitionRva
    },
    {
        0x001E6D40, // cellDetailClassifyRva
        0x001EC37F, // incrementalOuterClassifyRva
        { 0x002B6572, 0x002B657E, 0x002B658A },
        { 0x00773C58, 0x00772648, 0x00772F04 },
        0x002C6296, // objectActivationThresholdLoadRva
        0x00773EB4, // objectActivationThresholdSourceAddress
        0x002DD3C0, // objectLodMetricRva
        0x001F19B0, // spatialResidencyRva
        0x001F1790, // residencySetTargetRva
        0x007D965C, // residencyFocusPositionRva
        0x01037A30, // objectRangeStartRva
        0x01037A34, // objectRangeEndRva
        0x002D396F, // interiorOcclusionCallsiteRva
        0x002DBD30  // frustumCullRva
    },
    {
        0x001CB640,
        0x000607A0,
        0x00060540,
        0x001E2240,
        0x002B2AD0
    },
    {
        0x010736E0,
        0x0024328F,
        0x002435C4,
        0x000549C2,
        0x002419F5,
        0x00243AB7
    }
};

constexpr DpBuildProfile kGog101bProfile{
    DpBuild::Gog101b,
    "GOG 1.01b",
    0x52970AF6,
    0x010B5000,
    {
        0x0036E264, // direct3DCreate9IatRva
        0x0018CBD9, // speedDivideRva
        0x010AFFE0, // frameDeltaRva
        0x00001010  // currentGameStateGetterRva
    },
    {
        0x002B1780, // controllerBindingEvaluatorRva
        0x003093B0, // stickAxisPostProcessorRva
        0x003089E0, // stickFloatGetterRva
        { 0x0013C0E0, 0x0013C113, 0x0013C338, 0x0013C36B },
        0x010810F0, // useJoyModeRva
        0x00309BA0, // inputUpdateRva
        0x002E1630, // rdInputSetActuatorRva
        0x003358C0, // inputActuatorSetSecondRva
        0x0014C2A1  // vehicleAnalogInputInjectRva
    },
    {
        0x00104567, // combatStrafeHookRva
        0x000FEF30, // combatCapabilityRva
        0x00129010  // stateTransitionRva
    },
    {
        0x001E6E10, // cellDetailClassifyRva
        0x001EC44F, // incrementalOuterClassifyRva
        { 0x002B64C2, 0x002B64CE, 0x002B64DA },
        { 0x00773C48, 0x00772638, 0x00772EF4 },
        0x002C5D96, // objectActivationThresholdLoadRva
        0x00773EA4, // objectActivationThresholdSourceAddress
        0x002DCF90, // objectLodMetricRva
        0x001F1A80, // spatialResidencyRva
        0x001F1860, // residencySetTargetRva
        0x007D965C, // residencyFocusPositionRva
        0x01037A30, // objectRangeStartRva
        0x01037A34, // objectRangeEndRva
        0x002D353F, // interiorOcclusionCallsiteRva
        0x002DB900  // frustumCullRva
    },
    {
        0x001CB710,
        0x000607D0,
        0x00060570,
        0x001E2310,
        0x002B2AD0
    },
    {
        0x010736E0,
        0x002431DF,
        0x00243514,
        0x000549F2,
        0x00241945,
        0x00243A07
    }
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

bool IsMainExeAddress(const void* address)
{
    if (address == nullptr)
        return false;

    if (!g_mainExeInfoValid.load(std::memory_order_acquire) &&
        !InitializeMainExeInfo())
    {
        return false;
    }

    const uintptr_t value = reinterpret_cast<uintptr_t>(address);
    return value >= g_mainExeBase &&
           value < g_mainExeBase + g_mainExeSize;
}
