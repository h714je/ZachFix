#include "world_alternate3d_distance.h"

#include "config.h"
#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
static_assert(sizeof(void*) == 4, "DP alternate-3D distance extension requires Win32");

constexpr uintptr_t kWorldObjectRangeStride = 0x232C;
constexpr int kAlternatePackageCount = 75;
constexpr int kMaxWorldObjectSlots = 0xA000;
constexpr float kNativeAlternate3DNearHalfExtent = 2500.0f;

using SpatialResidencyFn = int (__thiscall*)(void* manager, void* arg0, void* arg1);
using ResidencySetTargetFn = void (__thiscall*)(void* manager, int objectIndex, int targetState);

SpatialResidencyFn g_originalSpatialResidency = nullptr;
ResidencySetTargetFn g_residencySetTarget = nullptr;
std::atomic_bool g_hookReady{false};
std::atomic_uint g_alternate3dDistanceScale{1};
std::atomic_bool g_refreshRequested{false};

bool MatchesBytes(uintptr_t address, const unsigned char* expected, size_t size)
{
    return address != 0 &&
        expected != nullptr &&
        std::memcmp(reinterpret_cast<const void*>(address), expected, size) == 0;
}

bool ReadResidencyFocus(float& x, float& z)
{
    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
        return false;

    const uintptr_t address =
        g_mainExeBase + build->world.residencyFocusPositionRva;
    if (!IsMainExeAddress(reinterpret_cast<const void*>(address)) ||
        !IsMainExeAddress(reinterpret_cast<const void*>(address + 8u)))
    {
        return false;
    }

    x = *reinterpret_cast<const float*>(address + 0u);
    z = *reinterpret_cast<const float*>(address + 8u);
    return std::isfinite(x) && std::isfinite(z);
}

bool ReadWorldObjectRange(void* manager, int& firstObject, int& endObject)
{
    firstObject = 0;
    endObject = 0;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (manager == nullptr || build == nullptr || g_mainExeBase == 0)
        return false;

    const auto* bytes = static_cast<const unsigned char*>(manager);
    const int worldIndex = *reinterpret_cast<const int*>(bytes + 0x28004);
    if (worldIndex < 0 || worldIndex > 255)
        return false;

    const uintptr_t worldOffset =
        static_cast<uintptr_t>(worldIndex) * kWorldObjectRangeStride;
    const uintptr_t firstAddress =
        g_mainExeBase + build->world.objectRangeStartRva + worldOffset;
    const uintptr_t endAddress =
        g_mainExeBase + build->world.objectRangeEndRva + worldOffset;

    if (!IsMainExeAddress(reinterpret_cast<const void*>(firstAddress)) ||
        !IsMainExeAddress(reinterpret_cast<const void*>(endAddress)))
    {
        return false;
    }

    firstObject = *reinterpret_cast<const int*>(firstAddress);
    endObject = *reinterpret_cast<const int*>(endAddress);
    return firstObject >= 0 &&
        endObject >= firstObject &&
        endObject <= kMaxWorldObjectSlots;
}

void ApplyExtendedNearTargets(void* manager, unsigned int scale)
{
    if (manager == nullptr || scale <= 1 || scale > 4 || g_residencySetTarget == nullptr)
        return;

    float focusX = 0.0f;
    float focusZ = 0.0f;
    if (!ReadResidencyFocus(focusX, focusZ))
        return;

    int firstObject = 0;
    int endObject = 0;
    if (!ReadWorldObjectRange(manager, firstObject, endObject))
        return;

    const float halfExtent =
        kNativeAlternate3DNearHalfExtent * static_cast<float>(scale);
    auto* managerBytes = static_cast<unsigned char*>(manager);

    for (int objectIndex = firstObject; objectIndex < endObject; ++objectIndex)
    {
        void* object = *reinterpret_cast<void**>(
            managerBytes + static_cast<size_t>(objectIndex) * sizeof(void*));
        if (object == nullptr)
            continue;

        const auto* objectBytes = static_cast<const unsigned char*>(object);
        const std::int8_t activeRepresentation =
            *reinterpret_cast<const std::int8_t*>(objectBytes + 0x12);
        const std::int8_t alternateIndex =
            *reinterpret_cast<const std::int8_t*>(objectBytes + 0x416);
        const std::int32_t targetRepresentation =
            *reinterpret_cast<const std::int32_t*>(objectBytes + 0x444);
        const float x = *reinterpret_cast<const float*>(objectBytes + 0x58);
        const float z = *reinterpret_cast<const float*>(objectBytes + 0x60);

        if (alternateIndex < 0 || alternateIndex >= kAlternatePackageCount ||
            (activeRepresentation != 0 && activeRepresentation != 1) ||
            (targetRepresentation != 0 && targetRepresentation != 1) ||
            !std::isfinite(x) || !std::isfinite(z))
        {
            continue;
        }

        const float dx = std::fabs(x - focusX);
        const float dz = std::fabs(z - focusZ);
        const float squareDistance = dx > dz ? dx : dz;
        if (squareDistance > halfExtent || targetRepresentation == 0)
            continue;

        // Keep DP authoritative: this setter changes the target state and, for
        // a far object, initiates the native high-detail streaming request. The
        // game's own swapper waits for readiness before rebinding the full XMD.
        g_residencySetTarget(manager, objectIndex, 0);
    }
}

int __fastcall HookSpatialResidency(void* manager, void*, void* arg0, void* arg1)
{
    if (manager == nullptr)
        return g_originalSpatialResidency(manager, arg0, arg1);

    auto* bytes = static_cast<unsigned char*>(manager);
    const int phaseBefore = *reinterpret_cast<const int*>(bytes + 0x28410);

    const int result = g_originalSpatialResidency(manager, arg0, arg1);

    const int phaseAfter = *reinterpret_cast<const int*>(bytes + 0x28410);
    const unsigned int distanceScale =
        g_alternate3dDistanceScale.load(std::memory_order_acquire);

    // DP's 0/3 -> 7 transition completes the native object sweep that can mark
    // objects outside the original 4x4 near grid as far. Re-apply only the
    // extended near requests after that sweep. An increased scale also gets one
    // refresh on the next safe residency tick so far-starting objects can begin
    // streaming without first entering the native near footprint.
    const bool nativeSweepCompleted =
        phaseAfter == 7 && (phaseBefore == 0 || phaseBefore == 3);
    bool refreshNow = false;
    if (g_refreshRequested.load(std::memory_order_acquire) &&
        (phaseAfter == 0 || nativeSweepCompleted))
    {
        refreshNow = g_refreshRequested.exchange(false, std::memory_order_acq_rel);
    }

    if (distanceScale > 1 && (nativeSweepCompleted || refreshNow))
        ApplyExtendedNearTargets(manager, distanceScale);

    return result;
}

bool EnsureAlternate3DDistanceHook()
{
    if (g_hookReady.load(std::memory_order_acquire))
        return true;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
    {
        AppendLog("[World][Alternate3D] Unsupported DP.exe build; distance extension unavailable.\n");
        return false;
    }

    const uintptr_t gridTarget =
        g_mainExeBase + build->world.spatialResidencyRva;
    const uintptr_t setTarget =
        g_mainExeBase + build->world.residencySetTargetRva;

    static constexpr unsigned char kSteamGridSignature[] = {
        0xB8, 0x8C, 0x12, 0x00, 0x00, 0xE8, 0x36, 0xF6,
        0x15, 0x00, 0x53, 0x55, 0x8B, 0xE9, 0x8B, 0x85,
        0x10, 0x84, 0x02, 0x00
    };
    static constexpr unsigned char kGogGridSignature[] = {
        0xB8, 0x8C, 0x12, 0x00, 0x00, 0xE8, 0x76, 0xF2,
        0x15, 0x00, 0x53, 0x55, 0x8B, 0xE9, 0x8B, 0x85,
        0x10, 0x84, 0x02, 0x00
    };
    static constexpr unsigned char kSetTargetSignature[] = {
        0x8B, 0x54, 0x24, 0x04, 0x56, 0x8B, 0x34, 0x91,
        0x85, 0xF6, 0x0F, 0x84, 0x8D, 0x00, 0x00, 0x00
    };

    const unsigned char* gridSignature = build->build == DpBuild::Steam101b
        ? kSteamGridSignature
        : kGogGridSignature;

    if (!MatchesBytes(gridTarget, gridSignature, sizeof(kSteamGridSignature)))
    {
        AppendLog("[World][Alternate3D] ERROR: spatial-residency signature mismatch; distance extension disabled.\n");
        return false;
    }
    if (!MatchesBytes(setTarget, kSetTargetSignature, sizeof(kSetTargetSignature)))
    {
        AppendLog("[World][Alternate3D] ERROR: residency target-setter signature mismatch; distance extension disabled.\n");
        return false;
    }

    SpatialResidencyFn original = nullptr;
    const MH_STATUS createStatus = MH_CreateHook(
        reinterpret_cast<void*>(gridTarget),
        reinterpret_cast<void*>(&HookSpatialResidency),
        reinterpret_cast<void**>(&original));
    if (createStatus != MH_OK)
    {
        char text[192] = {};
        sprintf_s(text, "[World][Alternate3D] ERROR: MH_CreateHook failed: %d.\n", static_cast<int>(createStatus));
        AppendLog(text);
        return false;
    }

    // Publish dependencies before enabling the detour so another game thread
    // cannot enter the hook with an uninitialized trampoline/setter pointer.
    g_originalSpatialResidency = original;
    g_residencySetTarget = reinterpret_cast<ResidencySetTargetFn>(setTarget);

    const MH_STATUS enableStatus = MH_EnableHook(reinterpret_cast<void*>(gridTarget));
    if (enableStatus != MH_OK)
    {
        char text[192] = {};
        sprintf_s(text, "[World][Alternate3D] ERROR: MH_EnableHook failed: %d.\n", static_cast<int>(enableStatus));
        AppendLog(text);
        const MH_STATUS removeStatus =
            MH_RemoveHook(reinterpret_cast<void*>(gridTarget));
        if (removeStatus == MH_OK || removeStatus == MH_ERROR_NOT_CREATED)
        {
            g_originalSpatialResidency = nullptr;
            g_residencySetTarget = nullptr;
        }
        else
        {
            AppendLog(
                "[World][Alternate3D] ERROR: rollback could not remove the spatial-residency hook; trampoline/setter retained for safety.\n");
        }
        return false;
    }

    g_hookReady.store(true, std::memory_order_release);

    char text[320] = {};
    sprintf_s(
        text,
        "[World][Alternate3D] Native residency distance extension ready on %s at DP.exe+0x%08lX (SetTarget=DP.exe+0x%08lX).\n",
        build->name,
        static_cast<unsigned long>(build->world.spatialResidencyRva),
        static_cast<unsigned long>(build->world.residencySetTargetRva));
    AppendLog(text);
    return true;
}
} // namespace

bool ApplyWorldAlternate3DDistanceScale(unsigned int scale)
{
    if (scale < 1 || scale > 4)
        return false;

    if (scale > 1 && !EnsureAlternate3DDistanceHook())
        return false;

    const unsigned int previous = g_alternate3dDistanceScale.exchange(
        scale, std::memory_order_acq_rel);
    g_config.alternate3dDistanceScale = scale;

    // Expansion should start streaming on the next safe residency tick. Range
    // reductions are left to DP's next native sweep so objects are never forced
    // abruptly from full to alternate representation by ZachFix.
    if (scale > previous)
        g_refreshRequested.store(true, std::memory_order_release);
    else
        g_refreshRequested.store(false, std::memory_order_release);

    char text[256] = {};
    sprintf_s(
        text,
        "[World][Alternate3D] Distance scale set to %ux (approx square half-extent %.0f units; native=2500).\n",
        scale,
        static_cast<double>(kNativeAlternate3DNearHalfExtent * static_cast<float>(scale)));
    AppendLog(text);
    return true;
}

unsigned int GetWorldAlternate3DDistanceScale()
{
    return g_alternate3dDistanceScale.load(std::memory_order_acquire);
}

bool IsWorldAlternate3DExtensionAvailable()
{
    return g_hookReady.load(std::memory_order_acquire);
}
