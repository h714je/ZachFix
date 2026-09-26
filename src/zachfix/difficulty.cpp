#include "difficulty.h"

#include "logging.h"
#include "main_exe.h"

#include <Windows.h>
#include <MinHook.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
constexpr size_t kDifficultyOffsetInManager = 0x8C60B;
uintptr_t g_difficultySelectorRva = 0;

uintptr_t g_historicalSwapContinue = 0;
uintptr_t g_menuResetContinue = 0;
uintptr_t g_stateHandlerContinue = 0;

void* g_historicalSwapTrampoline = nullptr;
void* g_menuResetTrampoline = nullptr;
void* g_stateHandlerTrampoline = nullptr;

const char* DifficultyName(unsigned int value)
{
    switch (value)
    {
    case 1: return "Normal";
    case 2: return "Hard";
    default: return "Easy";
    }
}

unsigned int ReadDifficultySelector()
{
    if (g_mainExeBase == 0 || g_difficultySelectorRva == 0)
        return 0;

    const auto* selector = reinterpret_cast<const unsigned int*>(
        g_mainExeBase + g_difficultySelectorRva);
    const unsigned int value = *selector;
    return value <= 2 ? value : 0;
}

void SetDifficultySelector(unsigned int difficulty)
{
    if (g_mainExeBase == 0 || g_difficultySelectorRva == 0)
        return;

    auto* selector = reinterpret_cast<unsigned int*>(
        g_mainExeBase + g_difficultySelectorRva);
    *selector = difficulty <= 2 ? difficulty : 0;
}

void __cdecl ApplyNativeSelectorDifficultyWrite(void* manager)
{
    if (manager == nullptr)
        return;

    auto* bytes = reinterpret_cast<unsigned char*>(manager);
    bytes[kDifficultyOffsetInManager] = static_cast<unsigned char>(ReadDifficultySelector());
}

void __cdecl ApplyNativeNewGameDifficultyWrite(void* manager)
{
    ApplyNativeSelectorDifficultyWrite(manager);
    const unsigned int difficulty = ReadDifficultySelector();

    char text[192] = {};
    sprintf_s(
        text,
        "[Difficulty] New Game committed native difficulty: %s (%u).\n",
        DifficultyName(difficulty),
        difficulty);
    AppendLog(text);
}

void __cdecl SyncNativeSelectorFromManager(void* manager)
{
    if (manager == nullptr)
        return;

    const auto* bytes = reinterpret_cast<const unsigned char*>(manager);
    const unsigned int difficulty = bytes[kDifficultyOffsetInManager] & 0x7Fu;
    if (difficulty <= 2)
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

#if defined(_M_IX86)
__declspec(naked) void HookNativeHistoricalSwapDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        push ebp
        call SyncNativeSelectorFromManager
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_historicalSwapContinue]
    }
}

__declspec(naked) void HookNativeMenuResetDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        push eax
        call ApplyNativeSelectorDifficultyWrite
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_menuResetContinue]
    }
}

__declspec(naked) void HookNativeStateHandlerDifficultyWrite()
{
    __asm
    {
        pushfd
        pushad
        push eax
        call ApplyNativeNewGameDifficultyWrite
        add esp, 4
        popad
        popfd
        jmp dword ptr [g_stateHandlerContinue]
    }
}
#endif

bool WriteCodeBytes(uintptr_t rva, const unsigned char* bytes, size_t size, const char* name)
{
    if (rva == 0 || bytes == nullptr || size == 0 || g_mainExeBase == 0)
        return false;

    auto* target = reinterpret_cast<unsigned char*>(g_mainExeBase + rva);
    DWORD oldProtect = 0;
    if (!VirtualProtect(target, size, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        char text[256] = {};
        sprintf_s(text, "[Difficulty] WARNING: VirtualProtect failed for %s.\n", name);
        AppendLog(text);
        return false;
    }

    std::memcpy(target, bytes, size);
    FlushInstructionCache(GetCurrentProcess(), target, size);

    DWORD ignored = 0;
    if (!VirtualProtect(target, size, oldProtect, &ignored))
    {
        char text[256] = {};
        sprintf_s(text, "[Difficulty] WARNING: Could not restore code protection after %s.\n", name);
        AppendLog(text);
    }
    return true;
}

bool InstallNativeDifficultyMenu(const DifficultyBuildProfile& rvas)
{
#if !defined(_M_IX86)
    (void)rvas;
    return false;
#else
    // Director's Cut has two New Game paths. With an existing save it routes
    // through the overwrite confirmation state; without a save it jumps directly
    // to title state 6. Restore the original difficulty selector (state 3) on
    // both paths. Keep the overwrite path's following selector/options helper alive.
    static const unsigned char kNewGameNoSaveExpected[] = {
        0xC7, 0x05, 0xD8, 0x36, 0x47, 0x01, 0x06, 0x00, 0x00, 0x00
    };
    static const unsigned char kNewGameNoSavePatched[] = {
        0xC7, 0x05, 0xD8, 0x36, 0x47, 0x01, 0x03, 0x00, 0x00, 0x00
    };
    static const unsigned char kNewGameBypassExpected[] = {
        0xC7, 0x05, 0xD8, 0x36, 0x47, 0x01, 0x06, 0x00, 0x00, 0x00,
        0x89, 0x1D, 0xE0, 0x36, 0x47, 0x01,
        0xE8, 0x17, 0xE4, 0xFF, 0xFF
    };
    static const unsigned char kNewGameBypassPatched[] = {
        0xC7, 0x05, 0xD8, 0x36, 0x47, 0x01, 0x03, 0x00, 0x00, 0x00,
        0x89, 0x1D, 0xE0, 0x36, 0x47, 0x01,
        0xE8, 0x17, 0xE4, 0xFF, 0xFF
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

    if (!VerifySignature(
            rvas.nativeNewGameNoSaveStateWriteRva,
            kNewGameNoSaveExpected,
            sizeof(kNewGameNoSaveExpected),
            "native New Game no-save difficulty bypass") ||
        !VerifySignature(
            rvas.nativeNewGameStateWriteRva,
            kNewGameBypassExpected,
            sizeof(kNewGameBypassExpected),
            "native New Game difficulty bypass") ||
        !VerifySignature(
            rvas.historicalSwapWriteRva,
            kHistoricalSwapSignature,
            sizeof(kHistoricalSwapSignature),
            "native historical-record difficulty preserve") ||
        !VerifySignature(
            rvas.menuResetWriteRva,
            kMenuResetSignature,
            sizeof(kMenuResetSignature),
            "native title difficulty write") ||
        !VerifySignature(
            rvas.stateHandlerWriteRva,
            kStateHandlerSignature,
            sizeof(kStateHandlerSignature),
            "native New Game difficulty commit"))
    {
        return false;
    }

    struct NativeHookSite
    {
        uintptr_t rva;
        size_t instructionSize;
        void* detour;
        void** trampoline;
        uintptr_t* continuation;
        const char* name;
    };

    NativeHookSite sites[] = {
        { rvas.historicalSwapWriteRva, sizeof(kHistoricalSwapSignature),
          reinterpret_cast<void*>(&HookNativeHistoricalSwapDifficultyWrite),
          &g_historicalSwapTrampoline, &g_historicalSwapContinue,
          "historical-record difficulty preserve" },
        { rvas.menuResetWriteRva, sizeof(kMenuResetSignature),
          reinterpret_cast<void*>(&HookNativeMenuResetDifficultyWrite),
          &g_menuResetTrampoline, &g_menuResetContinue,
          "title difficulty write" },
        { rvas.stateHandlerWriteRva, sizeof(kStateHandlerSignature),
          reinterpret_cast<void*>(&HookNativeStateHandlerDifficultyWrite),
          &g_stateHandlerTrampoline, &g_stateHandlerContinue,
          "New Game difficulty commit" }
    };

    size_t createdCount = 0;
    for (NativeHookSite& site : sites)
    {
        *site.continuation = g_mainExeBase + site.rva + site.instructionSize;
        void* target = reinterpret_cast<void*>(g_mainExeBase + site.rva);
        const MH_STATUS status = MH_CreateHook(target, site.detour, site.trampoline);
        if (status != MH_OK)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[Difficulty] WARNING: MH_CreateHook failed for native %s (%d).\n",
                site.name,
                static_cast<int>(status));
            AppendLog(text);
            for (size_t i = 0; i < createdCount; ++i)
                MH_RemoveHook(reinterpret_cast<void*>(g_mainExeBase + sites[i].rva));
            return false;
        }
        ++createdCount;
    }

    for (size_t i = 0; i < createdCount; ++i)
    {
        void* target = reinterpret_cast<void*>(g_mainExeBase + sites[i].rva);
        const MH_STATUS status = MH_EnableHook(target);
        if (status != MH_OK)
        {
            char text[320] = {};
            sprintf_s(
                text,
                "[Difficulty] WARNING: MH_EnableHook failed for native %s (%d).\n",
                sites[i].name,
                static_cast<int>(status));
            AppendLog(text);
            for (size_t j = 0; j < createdCount; ++j)
            {
                void* cleanupTarget = reinterpret_cast<void*>(g_mainExeBase + sites[j].rva);
                MH_DisableHook(cleanupTarget);
                MH_RemoveHook(cleanupTarget);
            }
            return false;
        }
    }

    if (!WriteCodeBytes(
            rvas.nativeNewGameNoSaveStateWriteRva,
            kNewGameNoSavePatched,
            sizeof(kNewGameNoSavePatched),
            "native New Game no-save difficulty selector restore") ||
        !WriteCodeBytes(
            rvas.nativeNewGameStateWriteRva,
            kNewGameBypassPatched,
            sizeof(kNewGameBypassPatched),
            "native New Game difficulty selector restore"))
    {
        for (NativeHookSite& site : sites)
        {
            void* target = reinterpret_cast<void*>(g_mainExeBase + site.rva);
            MH_DisableHook(target);
            MH_RemoveHook(target);
        }
        return false;
    }

    AppendLog(
        "[Difficulty] Original New Game Easy / Normal / Hard selector restored; "
        "difficulty is stored in the game's native save record and Continue preserves it.\n");
    return true;
#endif
}
} // namespace

unsigned int GetCurrentDifficultyValue()
{
    return ReadDifficultySelector();
}

const char* GetCurrentDifficultyName()
{
    return DifficultyName(GetCurrentDifficultyValue());
}

bool InstallDifficultyRestoration()
{
#if !defined(_M_IX86)
    AppendLog("[Difficulty] WARNING: restoration requires the 32-bit x86 build.\n");
    return false;
#else
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

    const DifficultyBuildProfile& rvas = build->difficulty;
    if (rvas.selectorRva == 0 ||
        rvas.nativeNewGameNoSaveStateWriteRva == 0 ||
        rvas.nativeNewGameStateWriteRva == 0 ||
        rvas.historicalSwapWriteRva == 0 ||
        rvas.menuResetWriteRva == 0 ||
        rvas.stateHandlerWriteRva == 0)
    {
        AppendLog("[Difficulty] Build mapping unavailable; restoration disabled.\n");
        return false;
    }

    g_difficultySelectorRva = rvas.selectorRva;
    return InstallNativeDifficultyMenu(rvas);
#endif
}
