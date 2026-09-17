#include "difficulty.h"

#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>

namespace
{
constexpr size_t kDifficultyOffsetInManager = 0x8C60B;
constexpr size_t kDifficultyOffsetInRecord = 0xA3;
constexpr uintptr_t kDifficultySelectorRva = 0x010736E0;

struct DifficultyBuildRvas
{
    uintptr_t managerInitWrite = 0;
    uintptr_t historicalSwapWrite = 0;
    uintptr_t menuResetWrite = 0;
    uintptr_t stateHandlerWrite = 0;
    uintptr_t liveRecordLoadCopy = 0;
    uintptr_t alternateRecordLoadCopy = 0;
    uintptr_t specialRecordLoadCopyA = 0;
    uintptr_t specialRecordLoadCopyB = 0;
    uintptr_t memoryCopy = 0;
};

DifficultyBuildRvas GetDifficultyBuildRvas(DpBuild build)
{
    switch (build)
    {
    case DpBuild::Steam101b:
        return {
            0x00052D5D,
            0x000549C2,
            0x002419F5,
            0x00243AB7,
            0x0006519D,
            0x0021A868,
            0x00220C32,
            0x0023DDEC,
            0x0034FA60
        };
    case DpBuild::Gog101b:
        return {
            0x00052D8D,
            0x000549F2,
            0x00241945,
            0x00243A07,
            0x000651CD,
            0x0021A7E8,
            0x00220BB2,
            0x0023DD3C,
            0x0034F770
        };
    default:
        return {};
    }
}

std::atomic_uint g_sessionDifficulty{ 0 };
std::atomic_bool g_sessionInitialized{ false };
std::atomic_bool g_mismatchLogged{ false };
DifficultyBuildRvas g_rvas{};

uintptr_t g_managerInitContinue = 0;
uintptr_t g_historicalSwapContinue = 0;
uintptr_t g_menuResetContinue = 0;
uintptr_t g_stateHandlerContinue = 0;
uintptr_t g_liveRecordLoadCopyContinue = 0;
uintptr_t g_alternateRecordLoadCopyContinue = 0;
uintptr_t g_specialRecordLoadCopyAContinue = 0;
uintptr_t g_specialRecordLoadCopyBContinue = 0;
uintptr_t g_memoryCopyTarget = 0;

void* g_managerInitTrampoline = nullptr;
void* g_historicalSwapTrampoline = nullptr;
void* g_menuResetTrampoline = nullptr;
void* g_stateHandlerTrampoline = nullptr;
void* g_liveRecordLoadCopyTrampoline = nullptr;
void* g_alternateRecordLoadCopyTrampoline = nullptr;
void* g_specialRecordLoadCopyATrampoline = nullptr;
void* g_specialRecordLoadCopyBTrampoline = nullptr;

const char* DifficultyName(unsigned int value)
{
    switch (value)
    {
    case 1: return "Normal";
    case 2: return "Hard";
    default: return "Easy";
    }
}

const char* DifficultyProfileName(unsigned int value)
{
    switch (value)
    {
    case 1: return "normal";
    case 2: return "hard";
    default: return "easy";
    }
}

void SetDifficultySelector(unsigned int difficulty)
{
    if (g_mainExeBase == 0)
        return;

    auto* selector = reinterpret_cast<unsigned char*>(
        g_mainExeBase + kDifficultySelectorRva);
    *selector = static_cast<unsigned char>(difficulty & 0x7Fu);
}

void __cdecl ApplySessionDifficultyWrite(void* manager, unsigned int originalRaw)
{
    if (manager == nullptr)
        return;

    const unsigned int difficulty = g_sessionDifficulty.load(std::memory_order_relaxed);
    auto* bytes = reinterpret_cast<unsigned char*>(manager);
    bytes[kDifficultyOffsetInManager] = static_cast<unsigned char>(
        (originalRaw & 0x80u) | (difficulty & 0x7Fu));
    SetDifficultySelector(difficulty);
}

void __cdecl ApplySessionDifficultyToRecord(void* record)
{
    if (record == nullptr)
        return;

    auto* bytes = reinterpret_cast<unsigned char*>(record);
    const unsigned int raw = bytes[kDifficultyOffsetInRecord];
    const unsigned int loadedDifficulty = raw & 0x7Fu;
    const unsigned int difficulty = g_sessionDifficulty.load(std::memory_order_relaxed);

    if (loadedDifficulty <= 2 && loadedDifficulty != difficulty)
    {
        bool expected = false;
        if (g_mismatchLogged.compare_exchange_strong(
                expected, true, std::memory_order_relaxed))
        {
            char text[384] = {};
            sprintf_s(
                text,
                "[Difficulty] Save/record mismatch accepted: record=%s, session=%s. "
                "Persistent world state is not migrated; the current session wins, and the next normal save will persist %s.\n",
                DifficultyName(loadedDifficulty),
                DifficultyName(difficulty),
                DifficultyName(difficulty));
            AppendLog(text);
        }
    }

    bytes[kDifficultyOffsetInRecord] = static_cast<unsigned char>(
        (raw & 0x80u) | (difficulty & 0x7Fu));
    SetDifficultySelector(difficulty);
}

bool VerifySignature(
    uintptr_t rva,
    const unsigned char* expected,
    size_t expectedSize,
    const char* name)
{
    if (rva == 0 || expected == nullptr || expectedSize == 0 || g_mainExeBase == 0)
        return false;

    const auto* target = reinterpret_cast<const unsigned char*>(g_mainExeBase + rva);
    if (std::memcmp(target, expected, expectedSize) == 0)
        return true;

    char text[320] = {};
    sprintf_s(
        text,
        "[Difficulty] WARNING: %s signature mismatch at DP.exe+0x%08lX; restoration disabled.\n",
        name,
        static_cast<unsigned long>(rva));
    AppendLog(text);
    return false;
}

bool VerifyRelativeCall(uintptr_t rva, uintptr_t expectedTargetRva, const char* name)
{
    if (rva == 0 || expectedTargetRva == 0 || g_mainExeBase == 0)
        return false;

    const auto* call = reinterpret_cast<const unsigned char*>(g_mainExeBase + rva);
    if (call[0] != 0xE8)
    {
        char text[320] = {};
        sprintf_s(
            text,
            "[Difficulty] WARNING: %s is not a relative CALL at DP.exe+0x%08lX; restoration disabled.\n",
            name,
            static_cast<unsigned long>(rva));
        AppendLog(text);
        return false;
    }

    std::int32_t displacement = 0;
    std::memcpy(&displacement, call + 1, sizeof(displacement));
    const uintptr_t target =
        g_mainExeBase + rva + 5 + static_cast<std::intptr_t>(displacement);
    if (target == g_mainExeBase + expectedTargetRva)
        return true;

    char text[320] = {};
    sprintf_s(
        text,
        "[Difficulty] WARNING: %s call target mismatch at DP.exe+0x%08lX; restoration disabled.\n",
        name,
        static_cast<unsigned long>(rva));
    AppendLog(text);
    return false;
}

#if defined(_M_IX86)
__declspec(naked) void HookManagerInitDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        movzx edx, al
        push edx
        push esi
        call ApplySessionDifficultyWrite
        add esp, 8
        popad
        popfd
        jmp dword ptr [g_managerInitContinue]
    }
}

__declspec(naked) void HookHistoricalSwapDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        push 0
        push ebp
        call ApplySessionDifficultyWrite
        add esp, 8
        popad
        popfd
        jmp dword ptr [g_historicalSwapContinue]
    }
}

__declspec(naked) void HookMenuResetDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        push 0
        push eax
        call ApplySessionDifficultyWrite
        add esp, 8
        popad
        popfd
        jmp dword ptr [g_menuResetContinue]
    }
}

__declspec(naked) void HookStateHandlerDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        movzx edx, bl
        push edx
        push eax
        call ApplySessionDifficultyWrite
        add esp, 8
        popad
        popfd
        jmp dword ptr [g_stateHandlerContinue]
    }
}

__declspec(naked) void HookLiveRecordLoadCopy()
{
    __asm
    {
        call dword ptr [g_memoryCopyTarget]
        pushfd
        pushad
        push eax
        call ApplySessionDifficultyToRecord
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_liveRecordLoadCopyContinue]
    }
}

__declspec(naked) void HookAlternateRecordLoadCopy()
{
    __asm
    {
        call dword ptr [g_memoryCopyTarget]
        pushfd
        pushad
        push eax
        call ApplySessionDifficultyToRecord
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_alternateRecordLoadCopyContinue]
    }
}

__declspec(naked) void HookSpecialRecordLoadCopyA()
{
    __asm
    {
        call dword ptr [g_memoryCopyTarget]
        pushfd
        pushad
        push eax
        call ApplySessionDifficultyToRecord
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_specialRecordLoadCopyAContinue]
    }
}

__declspec(naked) void HookSpecialRecordLoadCopyB()
{
    __asm
    {
        call dword ptr [g_memoryCopyTarget]
        pushfd
        pushad
        push eax
        call ApplySessionDifficultyToRecord
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_specialRecordLoadCopyBContinue]
    }
}
#endif

struct HookSite
{
    uintptr_t rva = 0;
    size_t instructionSize = 0;
    const unsigned char* signature = nullptr;
    size_t signatureSize = 0;
    void* detour = nullptr;
    void** trampoline = nullptr;
    uintptr_t* continuation = nullptr;
    const char* name = nullptr;
};

void CleanupHooks(HookSite* sites, size_t count)
{
    if (sites == nullptr || g_mainExeBase == 0)
        return;

    for (size_t i = 0; i < count; ++i)
    {
        if (sites[i].rva == 0)
            continue;
        void* target = reinterpret_cast<void*>(g_mainExeBase + sites[i].rva);
        MH_DisableHook(target);
        MH_RemoveHook(target);
    }
}
} // namespace

void InitializeDifficultySession()
{
    bool expected = false;
    if (!g_sessionInitialized.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel))
    {
        return;
    }

    unsigned int difficulty = 0;
    bool found = false;
    bool valid = true;

    const wchar_t* commandLine = GetCommandLineW();
    static const wchar_t kPrefix[] = L"-zachfix-difficulty=";
    constexpr size_t kPrefixLength = (sizeof(kPrefix) / sizeof(kPrefix[0])) - 1;

    if (commandLine != nullptr)
    {
        for (const wchar_t* p = commandLine; *p != L'\0'; ++p)
        {
            const bool tokenStart =
                p == commandLine || p[-1] == L' ' || p[-1] == L'\t' || p[-1] == L'"';
            if (!tokenStart || _wcsnicmp(p, kPrefix, kPrefixLength) != 0)
                continue;

            found = true;
            const wchar_t value = p[kPrefixLength];
            const wchar_t tail = p[kPrefixLength + 1];
            if ((value == L'0' || value == L'1' || value == L'2') &&
                (tail == L'\0' || tail == L' ' || tail == L'\t' || tail == L'"'))
            {
                difficulty = static_cast<unsigned int>(value - L'0');
            }
            else
            {
                valid = false;
                difficulty = 0;
            }
            break;
        }
    }

    g_sessionDifficulty.store(difficulty, std::memory_order_release);

    char text[384] = {};
    if (found && !valid)
    {
        sprintf_s(
            text,
            "[Difficulty] Invalid -zachfix-difficulty value; falling back to Easy (0), profile=easy.\n");
    }
    else
    {
        sprintf_s(
            text,
            "[Difficulty] Session difficulty: %s (%u), profile=%s%s.\n",
            DifficultyName(difficulty),
            difficulty,
            DifficultyProfileName(difficulty),
            found ? "" : ", default because no -zachfix-difficulty switch was supplied");
    }
    AppendLog(text);
}

GameDifficulty GetSessionDifficulty()
{
    return static_cast<GameDifficulty>(
        g_sessionDifficulty.load(std::memory_order_acquire));
}

unsigned int GetSessionDifficultyValue()
{
    return g_sessionDifficulty.load(std::memory_order_acquire);
}

const char* GetSessionDifficultyName()
{
    return DifficultyName(GetSessionDifficultyValue());
}

const char* GetSessionDifficultyProfileName()
{
    return DifficultyProfileName(GetSessionDifficultyValue());
}

bool InstallDifficultyRestoration()
{
#if !defined(_M_IX86)
    AppendLog("[Difficulty] WARNING: restoration requires the 32-bit x86 build.\n");
    return false;
#else
    InitializeDifficultySession();

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Difficulty] WARNING: DP.exe information unavailable; restoration disabled.\n");
        return false;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[Difficulty] Unsupported DP.exe build; Director's Cut Easy behavior kept.\n");
        return false;
    }

    g_rvas = GetDifficultyBuildRvas(build->build);
    if (g_rvas.managerInitWrite == 0)
    {
        AppendLog("[Difficulty] Build mapping unavailable; restoration disabled.\n");
        return false;
    }

    static const unsigned char kManagerInitSignature[] = {
        0x88, 0x86, 0x0B, 0xC6, 0x08, 0x00
    };
    static const unsigned char kHistoricalSwapSignature[] = {
        0xC6, 0x85, 0x0B, 0xC6, 0x08, 0x00, 0x00
    };
    static const unsigned char kMenuResetSignature[] = {
        0xC6, 0x80, 0x0B, 0xC6, 0x08, 0x00, 0x00
    };
    static const unsigned char kStateHandlerSignature[] = {
        0x88, 0x98, 0x0B, 0xC6, 0x08, 0x00
    };

    HookSite sites[] = {
        { g_rvas.managerInitWrite, 6,
          kManagerInitSignature, sizeof(kManagerInitSignature),
          reinterpret_cast<void*>(&HookManagerInitDifficultyWrite),
          &g_managerInitTrampoline, &g_managerInitContinue,
          "manager-init difficulty write" },
        { g_rvas.historicalSwapWrite, 7,
          kHistoricalSwapSignature, sizeof(kHistoricalSwapSignature),
          reinterpret_cast<void*>(&HookHistoricalSwapDifficultyWrite),
          &g_historicalSwapTrampoline, &g_historicalSwapContinue,
          "historical-swap difficulty write" },
        { g_rvas.menuResetWrite, 7,
          kMenuResetSignature, sizeof(kMenuResetSignature),
          reinterpret_cast<void*>(&HookMenuResetDifficultyWrite),
          &g_menuResetTrampoline, &g_menuResetContinue,
          "menu-reset difficulty write" },
        { g_rvas.stateHandlerWrite, 6,
          kStateHandlerSignature, sizeof(kStateHandlerSignature),
          reinterpret_cast<void*>(&HookStateHandlerDifficultyWrite),
          &g_stateHandlerTrampoline, &g_stateHandlerContinue,
          "state-handler difficulty write" },
        { g_rvas.liveRecordLoadCopy, 5,
          nullptr, 0,
          reinterpret_cast<void*>(&HookLiveRecordLoadCopy),
          &g_liveRecordLoadCopyTrampoline, &g_liveRecordLoadCopyContinue,
          "save-buffer to live-record copy" },
        { g_rvas.alternateRecordLoadCopy, 5,
          nullptr, 0,
          reinterpret_cast<void*>(&HookAlternateRecordLoadCopy),
          &g_alternateRecordLoadCopyTrampoline, &g_alternateRecordLoadCopyContinue,
          "alternate-state to live-record copy" },
        { g_rvas.specialRecordLoadCopyA, 5,
          nullptr, 0,
          reinterpret_cast<void*>(&HookSpecialRecordLoadCopyA),
          &g_specialRecordLoadCopyATrampoline, &g_specialRecordLoadCopyAContinue,
          "special live-record copy A" },
        { g_rvas.specialRecordLoadCopyB, 5,
          nullptr, 0,
          reinterpret_cast<void*>(&HookSpecialRecordLoadCopyB),
          &g_specialRecordLoadCopyBTrampoline, &g_specialRecordLoadCopyBContinue,
          "special live-record copy B" }
    };

    g_memoryCopyTarget = g_mainExeBase + g_rvas.memoryCopy;
    if (!VerifyRelativeCall(g_rvas.liveRecordLoadCopy, g_rvas.memoryCopy, sites[4].name) ||
        !VerifyRelativeCall(g_rvas.alternateRecordLoadCopy, g_rvas.memoryCopy, sites[5].name) ||
        !VerifyRelativeCall(g_rvas.specialRecordLoadCopyA, g_rvas.memoryCopy, sites[6].name) ||
        !VerifyRelativeCall(g_rvas.specialRecordLoadCopyB, g_rvas.memoryCopy, sites[7].name))
    {
        return false;
    }

    for (const HookSite& site : sites)
    {
        if (site.signature != nullptr &&
            !VerifySignature(site.rva, site.signature, site.signatureSize, site.name))
        {
            return false;
        }
    }

    size_t createdCount = 0;
    for (HookSite& site : sites)
    {
        *site.continuation = g_mainExeBase + site.rva + site.instructionSize;
        void* target = reinterpret_cast<void*>(g_mainExeBase + site.rva);
        const MH_STATUS status = MH_CreateHook(target, site.detour, site.trampoline);
        if (status != MH_OK)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[Difficulty] WARNING: MH_CreateHook failed for %s (%d); restoration disabled.\n",
                site.name,
                static_cast<int>(status));
            AppendLog(text);
            CleanupHooks(sites, createdCount);
            return false;
        }
        ++createdCount;
    }

    for (HookSite& site : sites)
    {
        void* target = reinterpret_cast<void*>(g_mainExeBase + site.rva);
        const MH_STATUS status = MH_EnableHook(target);
        if (status != MH_OK)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[Difficulty] WARNING: MH_EnableHook failed for %s (%d); restoration disabled.\n",
                site.name,
                static_cast<int>(status));
            AppendLog(text);
            CleanupHooks(sites, createdCount);
            return false;
        }
    }

    SetDifficultySelector(GetSessionDifficultyValue());

    char text[384] = {};
    sprintf_s(
        text,
        "[Difficulty] Native difficulty restoration active: %s. "
        "No persistent save-state migration is performed.\n",
        GetSessionDifficultyName());
    AppendLog(text);
    return true;
#endif
}
