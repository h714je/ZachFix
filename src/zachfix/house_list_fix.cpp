#include "house_list_fix.h"

#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>

namespace
{
constexpr int kHouseListResourceId = 0x39DF;
constexpr int kHouseListRecordCount = 0x51;
constexpr size_t kHouseListRecordSize = 0x50;

using LevelDayNightConfigLoadFn = void (__fastcall*)(void*, void*);
using LevelActiveVariantFn = int (__thiscall*)(void*);
using LevelResourceViewFn = void* (__thiscall*)(void*);
using LevelResourceKeyFn = int (__thiscall*)(void*, int, int);
using ResourceNameLookupFn = int (__thiscall*)(void*, const char*, int, int);

LevelDayNightConfigLoadFn g_originalLevelConfigLoad = nullptr;
LevelActiveVariantFn g_levelActiveVariant = nullptr;
LevelResourceViewFn g_levelResourceView = nullptr;
LevelResourceKeyFn g_levelResourceKey = nullptr;
ResourceNameLookupFn g_originalResourceNameLookup = nullptr;

std::atomic_uintptr_t g_resourceManager{ 0 };
std::atomic_bool g_fixInstalled{ false };
std::mutex g_installMutex;
std::recursive_mutex g_repairMutex;
std::atomic_bool g_writeWarningLogged{ false };

bool IsReadableRange(const void* pointer, size_t size)
{
    if (pointer == nullptr || size == 0)
        return false;

    const auto begin = reinterpret_cast<uintptr_t>(pointer);
    const auto end = begin + size;
    if (end < begin)
        return false;

    uintptr_t cursor = begin;
    while (cursor < end)
    {
        MEMORY_BASIC_INFORMATION info{};
        if (VirtualQuery(reinterpret_cast<const void*>(cursor), &info, sizeof(info)) == 0)
            return false;
        if (info.State != MEM_COMMIT || (info.Protect & PAGE_GUARD) != 0 ||
            (info.Protect & PAGE_NOACCESS) != 0)
        {
            return false;
        }

        const uintptr_t regionBegin = reinterpret_cast<uintptr_t>(info.BaseAddress);
        const uintptr_t regionEnd = regionBegin + info.RegionSize;
        if (regionEnd <= cursor)
            return false;
        cursor = regionEnd < end ? regionEnd : end;
    }

    return true;
}

bool IsWritableRange(void* pointer, size_t size)
{
    if (!IsReadableRange(pointer, size))
        return false;

    MEMORY_BASIC_INFORMATION info{};
    if (VirtualQuery(pointer, &info, sizeof(info)) == 0)
        return false;

    const DWORD protection = info.Protect & 0xFFu;
    return protection == PAGE_READWRITE ||
           protection == PAGE_WRITECOPY ||
           protection == PAGE_EXECUTE_READWRITE ||
           protection == PAGE_EXECUTE_WRITECOPY;
}

uint16_t ByteSwap16(uint16_t value)
{
    return static_cast<uint16_t>((value << 8) | (value >> 8));
}

class ScopedRecordKeyOverride
{
public:
    ScopedRecordKeyOverride(unsigned char* record, uint16_t replacement, uint16_t original)
        : record_(record), original_(original)
    {
        std::memcpy(record_, &replacement, sizeof(replacement));
    }

    ~ScopedRecordKeyOverride()
    {
        std::memcpy(record_, &original_, sizeof(original_));
    }

    ScopedRecordKeyOverride(const ScopedRecordKeyOverride&) = delete;
    ScopedRecordKeyOverride& operator=(const ScopedRecordKeyOverride&) = delete;

private:
    unsigned char* record_;
    uint16_t original_;
};

const unsigned char* GetHouseListPayload()
{
    const uintptr_t managerAddress = g_resourceManager.load(std::memory_order_acquire);
    if (managerAddress == 0)
        return nullptr;

    auto* manager = reinterpret_cast<unsigned char*>(managerAddress);
    if (!IsReadableRange(manager, 0x10))
        return nullptr;

    const int count = *reinterpret_cast<const int*>(manager + 0x04);
    const uintptr_t entriesAddress = *reinterpret_cast<const uintptr_t*>(manager + 0x0C);
    if (count <= kHouseListResourceId || entriesAddress == 0)
        return nullptr;

    const auto* entry = reinterpret_cast<const unsigned char*>(entriesAddress) +
        static_cast<size_t>(kHouseListResourceId) * 0x30;
    if (!IsReadableRange(entry, 0x30) ||
        *reinterpret_cast<const uint16_t*>(entry + 0x2C) == 0)
    {
        return nullptr;
    }

    const uintptr_t payload = *reinterpret_cast<const uintptr_t*>(entry + 0x18);
    const size_t bytes = static_cast<size_t>(kHouseListRecordCount) * kHouseListRecordSize;
    if (payload == 0 || !IsReadableRange(reinterpret_cast<const void*>(payload), bytes))
        return nullptr;

    return reinterpret_cast<const unsigned char*>(payload);
}

int FindDirectRecord(const unsigned char* table, uint16_t requestedKey)
{
    if (table == nullptr)
        return -1;

    for (int i = 0; i < kHouseListRecordCount; ++i)
    {
        const auto* record = table + static_cast<size_t>(i) * kHouseListRecordSize;
        uint16_t storedKey = 0;
        std::memcpy(&storedKey, record, sizeof(storedKey));
        if (storedKey == requestedKey)
            return i;
    }

    return -1;
}

// The normalized PC and Xbox HOUSE_LIST payloads are byte-identical. At runtime,
// however, the PC table is only partially host-endian: 17/81 keys already match
// normal PC lookup keys while 64 remain in Xbox byte order. Preserve every direct
// match and repair only a unique swapped miss. Return -1 for no match and -2 for
// an ambiguous swapped match.
int FindUniqueSwappedRecord(
    const unsigned char* table,
    uint16_t requestedKey,
    uint16_t& storedKeyOut)
{
    storedKeyOut = 0;
    if (table == nullptr)
        return -1;

    int match = -1;
    for (int i = 0; i < kHouseListRecordCount; ++i)
    {
        const auto* record = table + static_cast<size_t>(i) * kHouseListRecordSize;
        uint16_t storedKey = 0;
        std::memcpy(&storedKey, record, sizeof(storedKey));
        if (ByteSwap16(storedKey) != requestedKey)
            continue;
        if (match >= 0)
            return -2;
        match = i;
        storedKeyOut = storedKey;
    }

    return match;
}

bool MatchBytes(const unsigned char* address, const unsigned char* expected, size_t size)
{
    return address != nullptr && std::memcmp(address, expected, size) == 0;
}

bool ValidateMappedFunctions(const HouseListFixBuildProfile& profile)
{
    if (profile.levelConfigLoadRva == 0 ||
        profile.levelActiveVariantRva == 0 ||
        profile.levelResourceViewRva == 0 ||
        profile.levelResourceKeyRva == 0 ||
        profile.resourceNameLookupRva == 0)
    {
        return false;
    }

    const auto* levelConfig = reinterpret_cast<const unsigned char*>(
        g_mainExeBase + profile.levelConfigLoadRva);
    const auto* activeVariant = reinterpret_cast<const unsigned char*>(
        g_mainExeBase + profile.levelActiveVariantRva);
    const auto* resourceView = reinterpret_cast<const unsigned char*>(
        g_mainExeBase + profile.levelResourceViewRva);
    const auto* resourceKey = reinterpret_cast<const unsigned char*>(
        g_mainExeBase + profile.levelResourceKeyRva);
    const auto* resourceLookup = reinterpret_cast<const unsigned char*>(
        g_mainExeBase + profile.resourceNameLookupRva);

    static const unsigned char levelConfigPrefix[] = {
        0x83, 0xEC, 0x14, 0xA1, 0xCC, 0x5E, 0xBD, 0x00
    };
    static const unsigned char activeVariantPrefix[] = {
        0x8B, 0x41, 0x18, 0x69, 0xC0, 0x2C, 0x23, 0x00, 0x00
    };
    static const unsigned char resourceViewPrefix[] = {
        0x8B, 0x41, 0x18, 0x69, 0xC0, 0x2C, 0x23, 0x00, 0x00
    };
    static const unsigned char resourceKeyPrefix[] = {
        0x80, 0x79, 0x04, 0x00, 0x75, 0x06, 0x83, 0xC8, 0xFF
    };
    static const unsigned char resourceLookupPrefix[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x08, 0x89, 0x4D, 0xF8,
        0x83, 0x7D, 0x08, 0x00, 0x74, 0x35
    };

    return MatchBytes(levelConfig, levelConfigPrefix, sizeof(levelConfigPrefix)) &&
           MatchBytes(activeVariant, activeVariantPrefix, sizeof(activeVariantPrefix)) &&
           MatchBytes(resourceView, resourceViewPrefix, sizeof(resourceViewPrefix)) &&
           MatchBytes(resourceKey, resourceKeyPrefix, sizeof(resourceKeyPrefix)) &&
           MatchBytes(resourceLookup, resourceLookupPrefix, sizeof(resourceLookupPrefix));
}

int __fastcall HookResourceNameLookup(
    void* manager,
    void*,
    const char* path,
    int lookupMode,
    int requireLoaded)
{
    if (manager != nullptr)
    {
        g_resourceManager.store(
            reinterpret_cast<uintptr_t>(manager),
            std::memory_order_release);
    }

    return g_originalResourceNameLookup != nullptr
        ? g_originalResourceNameLookup(manager, path, lookupMode, requireLoaded)
        : -1;
}

void __fastcall HookLevelConfigLoad(void* level, void*)
{
    if (g_originalLevelConfigLoad == nullptr)
        return;

    int key = -1;
    if (level != nullptr && g_levelActiveVariant != nullptr &&
        g_levelResourceView != nullptr && g_levelResourceKey != nullptr)
    {
        const int variant = g_levelActiveVariant(level);
        void* view = g_levelResourceView(level);
        if (view != nullptr)
            key = g_levelResourceKey(view, 0, variant);
    }

    std::lock_guard<std::recursive_mutex> lock(g_repairMutex);

    const unsigned char* table = GetHouseListPayload();
    if (table == nullptr || key < 0 || key > 0xFFFF)
    {
        g_originalLevelConfigLoad(level, nullptr);
        return;
    }

    const uint16_t requestedKey = static_cast<uint16_t>(key);
    if (FindDirectRecord(table, requestedKey) >= 0)
    {
        g_originalLevelConfigLoad(level, nullptr);
        return;
    }

    uint16_t storedKey = 0;
    const int recordIndex = FindUniqueSwappedRecord(table, requestedKey, storedKey);
    if (recordIndex < 0)
    {
        g_originalLevelConfigLoad(level, nullptr);
        return;
    }

    auto* record = const_cast<unsigned char*>(table) +
        static_cast<size_t>(recordIndex) * kHouseListRecordSize;
    if (!IsWritableRange(record, sizeof(uint16_t)))
    {
        if (!g_writeWarningLogged.exchange(true, std::memory_order_acq_rel))
        {
            AppendLog(
                "[DayNight] WARNING: HOUSE_LIST.NOD key storage is not writable; "
                "endian repair skipped.\n");
        }
        g_originalLevelConfigLoad(level, nullptr);
        return;
    }

    ScopedRecordKeyOverride keyOverride(record, requestedKey, storedKey);
    g_originalLevelConfigLoad(level, nullptr);
}
} // namespace

bool InstallHouseListEndianFix()
{
    if (g_fixInstalled.load(std::memory_order_acquire))
        return true;

    std::lock_guard<std::mutex> installLock(g_installMutex);
    if (g_fixInstalled.load(std::memory_order_relaxed))
        return true;

#if !defined(_M_IX86)
    AppendLog("[DayNight] HOUSE_LIST.NOD endian repair requires the supported x86 build.\n");
    return false;
#else
    if (!InitializeMainExeInfo())
    {
        AppendLog("[DayNight] DP.exe information unavailable; HOUSE_LIST.NOD endian repair disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[DayNight] Unsupported DP.exe build; HOUSE_LIST.NOD endian repair disabled.\n");
        return false;
    }

    const HouseListFixBuildProfile& profile = build->houseListFix;
    if (!ValidateMappedFunctions(profile))
    {
        AppendLog("[DayNight] HOUSE_LIST.NOD native function signature mismatch; repair disabled.\n");
        return false;
    }

    auto* resourceTarget = reinterpret_cast<void*>(
        g_mainExeBase + profile.resourceNameLookupRva);
    auto* levelConfigTarget = reinterpret_cast<void*>(
        g_mainExeBase + profile.levelConfigLoadRva);

    g_levelActiveVariant = reinterpret_cast<LevelActiveVariantFn>(
        g_mainExeBase + profile.levelActiveVariantRva);
    g_levelResourceView = reinterpret_cast<LevelResourceViewFn>(
        g_mainExeBase + profile.levelResourceViewRva);
    g_levelResourceKey = reinterpret_cast<LevelResourceKeyFn>(
        g_mainExeBase + profile.levelResourceKeyRva);

    MH_STATUS status = MH_CreateHook(
        resourceTarget,
        reinterpret_cast<void*>(&HookResourceNameLookup),
        reinterpret_cast<void**>(&g_originalResourceNameLookup));
    if (status != MH_OK || g_originalResourceNameLookup == nullptr)
    {
        AppendLog("[DayNight] Could not hook the resource cache; HOUSE_LIST.NOD endian repair disabled.\n");
        return false;
    }

    status = MH_CreateHook(
        levelConfigTarget,
        reinterpret_cast<void*>(&HookLevelConfigLoad),
        reinterpret_cast<void**>(&g_originalLevelConfigLoad));
    if (status != MH_OK || g_originalLevelConfigLoad == nullptr)
    {
        MH_RemoveHook(resourceTarget);
        g_originalResourceNameLookup = nullptr;
        AppendLog("[DayNight] Could not hook the CLevel HOUSE_LIST loader; endian repair disabled.\n");
        return false;
    }

    status = MH_EnableHook(resourceTarget);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        MH_RemoveHook(levelConfigTarget);
        MH_RemoveHook(resourceTarget);
        g_originalLevelConfigLoad = nullptr;
        g_originalResourceNameLookup = nullptr;
        AppendLog("[DayNight] Could not enable the resource-cache hook; HOUSE_LIST.NOD endian repair disabled.\n");
        return false;
    }

    status = MH_EnableHook(levelConfigTarget);
    if (status != MH_OK && status != MH_ERROR_ENABLED)
    {
        MH_DisableHook(resourceTarget);
        MH_RemoveHook(levelConfigTarget);
        MH_RemoveHook(resourceTarget);
        g_originalLevelConfigLoad = nullptr;
        g_originalResourceNameLookup = nullptr;
        AppendLog("[DayNight] Could not enable the CLevel HOUSE_LIST hook; endian repair disabled.\n");
        return false;
    }

    g_fixInstalled.store(true, std::memory_order_release);

    char text[320] = {};
    sprintf_s(
        text,
        "[DayNight] HOUSE_LIST.NOD runtime endian repair enabled on %s: native direct matches preserved; unique byte-swapped key fallback active.\n",
        build->name);
    AppendLog(text);
    return true;
#endif
}
