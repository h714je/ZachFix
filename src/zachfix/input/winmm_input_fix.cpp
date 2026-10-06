#include "zachfix/input/winmm_input_fix.h"

#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <intrin.h>

namespace
{
static_assert(sizeof(void*) == 4, "WinMM polling fix requires the 32-bit ZachFix build.");

using JoyGetPosExFn = MMRESULT (WINAPI*)(UINT, LPJOYINFOEX);

constexpr std::size_t kTrackedJoySlots = 16;

// Steam 1.01b: 00709D95 FF 15 ... joyGetPosEx, return 00709D9B.
// GOG keeps the same offset inside its paired input-update function.
constexpr uintptr_t kAcquisitionJoyReturnOffset = 0x15Bu;

// Main-frame leftover is shared by both supported executables. Its JOYINFOEX
// is uninitialized, but the call itself is harmless in the healthy state and
// becomes expensive only when the Windows legacy slot is pathological.
constexpr uintptr_t kDeadMainFrameJoyReturnRva = 0x00001CAFu;

struct JoySlotState
{
    std::atomic_bool suppressed{ false };
    std::atomic_bool pendingRevalidationLog{ false };
};

std::array<JoySlotState, kTrackedJoySlots> g_slots{};
std::atomic_uint32_t g_generation{ 0 };
std::atomic_bool g_requested{ false };
std::atomic_bool g_available{ false };
std::atomic_bool g_active{ false };
std::atomic_bool g_windowAttached{ false };
std::atomic_bool g_iatPatched{ false };
JoyGetPosExFn g_realJoyGetPosEx = nullptr;
void** g_joyGetPosExIatSlot = nullptr;
HWND g_gameWindow = nullptr;
WNDPROC g_originalWndProc = nullptr;
uintptr_t g_acquisitionJoyReturnAddress = 0;
uintptr_t g_deadMainFrameJoyReturnAddress = 0;
bool g_acquisitionCallValidated = false;
bool g_deadCallValidated = false;

bool AsciiEqualsIgnoreCase(const char* a, const char* b)
{
    if (!a || !b)
        return false;

    for (;; ++a, ++b)
    {
        unsigned char ca = static_cast<unsigned char>(*a);
        unsigned char cb = static_cast<unsigned char>(*b);
        if (ca >= 'A' && ca <= 'Z')
            ca = static_cast<unsigned char>(ca - 'A' + 'a');
        if (cb >= 'A' && cb <= 'Z')
            cb = static_cast<unsigned char>(cb - 'A' + 'a');

        if (ca != cb)
            return false;
        if (ca == 0)
            return true;
    }
}

void** FindImportAddressSlot(
    HMODULE module,
    const char* importedDll,
    const char* importedName)
{
    if (!module || !importedDll || !importedName)
        return nullptr;

    auto* base = reinterpret_cast<unsigned char*>(module);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0)
        return nullptr;

    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        return nullptr;
    }

    const auto& importDirectory =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (importDirectory.VirtualAddress == 0 || importDirectory.Size == 0)
        return nullptr;

    auto* descriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(
        base + importDirectory.VirtualAddress);

    for (; descriptor->Name != 0; ++descriptor)
    {
        const char* dllName = reinterpret_cast<const char*>(base + descriptor->Name);
        if (!AsciiEqualsIgnoreCase(dllName, importedDll))
            continue;

        if (descriptor->OriginalFirstThunk == 0 || descriptor->FirstThunk == 0)
            return nullptr;

        auto* originalThunk = reinterpret_cast<IMAGE_THUNK_DATA32*>(
            base + descriptor->OriginalFirstThunk);
        auto* firstThunk = reinterpret_cast<IMAGE_THUNK_DATA32*>(
            base + descriptor->FirstThunk);

        for (; originalThunk->u1.AddressOfData != 0;
             ++originalThunk, ++firstThunk)
        {
            if (IMAGE_SNAP_BY_ORDINAL32(originalThunk->u1.Ordinal))
                continue;

            auto* importByName = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(
                base + originalThunk->u1.AddressOfData);
            if (AsciiEqualsIgnoreCase(
                    reinterpret_cast<const char*>(importByName->Name),
                    importedName))
            {
                return reinterpret_cast<void**>(&firstThunk->u1.Function);
            }
        }

        return nullptr;
    }

    return nullptr;
}


bool ValidateIndirectIatCall(uintptr_t returnAddress, void** iatSlot)
{
    if (returnAddress < 6u || iatSlot == nullptr)
        return false;

    const auto* call = reinterpret_cast<const unsigned char*>(returnAddress - 6u);
    if (call[0] != 0xFF || call[1] != 0x15)
        return false;

    std::uint32_t encodedSlot = 0;
    std::memcpy(&encodedSlot, call + 2, sizeof(encodedSlot));
    return encodedSlot == static_cast<std::uint32_t>(
        reinterpret_cast<uintptr_t>(iatSlot));
}

bool PatchIatSlot(void** slot, void* replacement, void** originalOut)
{
    if (!slot || !*slot || !replacement || !originalOut)
        return false;

    DWORD oldProtect = 0;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect))
        return false;

    *originalOut = *slot;
    *slot = replacement;

    DWORD ignored = 0;
    if (!VirtualProtect(slot, sizeof(*slot), oldProtect, &ignored))
    {
        AppendLog(
            "[Input][WinMM] WARNING: joyGetPosEx IAT hook installed but the "
            "slot's original page protection could not be restored.\n");
    }

    FlushInstructionCache(GetCurrentProcess(), slot, sizeof(*slot));
    return *slot == replacement;
}

void ResetSuppressedSlotsForDeviceChange()
{
    bool hadSuppressedSlot = false;
    for (auto& slot : g_slots)
    {
        if (slot.suppressed.exchange(false, std::memory_order_acq_rel))
        {
            slot.pendingRevalidationLog.store(true, std::memory_order_release);
            hadSuppressedSlot = true;
        }
    }

    if (!hadSuppressedSlot)
        return;

    const std::uint32_t generation =
        g_generation.fetch_add(1, std::memory_order_acq_rel) + 1;

    char text[192] = {};
    sprintf_s(
        text,
        "[Input][WinMM] Device configuration changed (generation %lu); "
        "suppressed joystick slots will be revalidated once.\n",
        static_cast<unsigned long>(generation));
    AppendLog(text);
}

MMRESULT CallGuardedJoyGetPosEx(UINT joyId, LPJOYINFOEX info, bool cacheJoyErrParms)
{
    if (g_realJoyGetPosEx == nullptr)
        return MMSYSERR_NODRIVER;

    if (!g_requested.load(std::memory_order_acquire) ||
        !cacheJoyErrParms ||
        joyId >= static_cast<UINT>(g_slots.size()))
    {
        return g_realJoyGetPosEx(joyId, info);
    }

    JoySlotState& slot = g_slots[joyId];
    if (slot.suppressed.load(std::memory_order_acquire))
        return JOYERR_PARMS;

    const MMRESULT result = g_realJoyGetPosEx(joyId, info);

    if (result == JOYERR_PARMS)
    {
        const bool wasSuppressed =
            slot.suppressed.exchange(true, std::memory_order_acq_rel);
        slot.pendingRevalidationLog.store(false, std::memory_order_release);

        if (!wasSuppressed)
        {
            char text[256] = {};
            sprintf_s(
                text,
                "[Input][WinMM] joyGetPosEx slot %u returned JOYERR_PARMS; "
                "suppressing repeated legacy polling until device configuration changes.\n",
                joyId);
            AppendLog(text);
        }
        return result;
    }

    if (slot.pendingRevalidationLog.exchange(false, std::memory_order_acq_rel))
    {
        char text[224] = {};
        if (result == JOYERR_NOERROR)
        {
            sprintf_s(
                text,
                "[Input][WinMM] Joystick slot %u recovered after device change.\n",
                joyId);
        }
        else
        {
            sprintf_s(
                text,
                "[Input][WinMM] Joystick slot %u revalidated after device change "
                "with result %u; repeated polling remains enabled.\n",
                joyId,
                static_cast<unsigned>(result));
        }
        AppendLog(text);
    }

    return result;
}

MMRESULT WINAPI HookJoyGetPosEx(UINT joyId, LPJOYINFOEX info)
{
    const uintptr_t caller = reinterpret_cast<uintptr_t>(_ReturnAddress());
    const bool cacheJoyErrParms =
        (g_acquisitionCallValidated && caller == g_acquisitionJoyReturnAddress) ||
        (g_deadCallValidated && caller == g_deadMainFrameJoyReturnAddress);

    return CallGuardedJoyGetPosEx(joyId, info, cacheJoyErrParms);
}

LRESULT CALLBACK LegacyJoystickWndProc(
    HWND hwnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam)
{
    if (msg == WM_DEVICECHANGE && g_requested.load(std::memory_order_acquire))
        ResetSuppressedSlotsForDeviceChange();

    if (g_originalWndProc != nullptr)
        return CallWindowProcW(g_originalWndProc, hwnd, msg, wParam, lParam);

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
} // namespace

bool InstallLegacyJoystickPollingFix(bool enabled)
{
    g_requested.store(enabled, std::memory_order_release);

    HMODULE winmm = GetModuleHandleW(L"winmm.dll");
    if (winmm == nullptr)
        winmm = LoadLibraryW(L"winmm.dll");

    if (winmm != nullptr)
    {
        g_realJoyGetPosEx = reinterpret_cast<JoyGetPosExFn>(
            GetProcAddress(winmm, "joyGetPosEx"));
    }

    if (g_realJoyGetPosEx == nullptr)
    {
        AppendLog(
            "[Input][WinMM] WARNING: winmm!joyGetPosEx unavailable; "
            "legacy joystick polling guard unavailable.\n");
        return false;
    }

    g_available.store(true, std::memory_order_release);

    if (!enabled)
    {
        AppendLog(
            "[Input][WinMM] Legacy JOYERR_PARMS polling guard disabled by config.\n");
        return true;
    }

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr || g_mainExeBase == 0)
    {
        AppendLog(
            "[Input][WinMM] WARNING: unsupported DP.exe build; "
            "ZachFix-owned legacy polling is guarded but DP.exe IAT was not patched.\n");
        return false;
    }

    HMODULE mainModule = GetModuleHandleW(nullptr);
    g_joyGetPosExIatSlot = FindImportAddressSlot(
        mainModule,
        "WINMM.dll",
        "joyGetPosEx");

    if (g_joyGetPosExIatSlot == nullptr || *g_joyGetPosExIatSlot == nullptr)
    {
        AppendLog(
            "[Input][WinMM] WARNING: DP.exe joyGetPosEx IAT slot not found; "
            "ZachFix-owned legacy polling is guarded but vanilla polling is unchanged.\n");
        return false;
    }

    g_acquisitionJoyReturnAddress =
        g_mainExeBase + build->input.inputUpdateRva + kAcquisitionJoyReturnOffset;
    g_deadMainFrameJoyReturnAddress =
        g_mainExeBase + kDeadMainFrameJoyReturnRva;
    g_acquisitionCallValidated = ValidateIndirectIatCall(
        g_acquisitionJoyReturnAddress, g_joyGetPosExIatSlot);
    g_deadCallValidated = ValidateIndirectIatCall(
        g_deadMainFrameJoyReturnAddress, g_joyGetPosExIatSlot);

    if (!g_acquisitionCallValidated || !g_deadCallValidated)
    {
        AppendLog(
            "[Input][WinMM] WARNING: known joyGetPosEx callsite validation failed; "
            "DP.exe legacy polling left untouched.\n");
        return false;
    }

    // The IAT should normally point at the same export resolved above. Accept a
    // compatible pre-hook target as the canonical original so this chains with
    // loader/proxy arrangements rather than hard-coding a module address.
    g_realJoyGetPosEx = reinterpret_cast<JoyGetPosExFn>(*g_joyGetPosExIatSlot);

    void* originalTarget = nullptr;
    if (!PatchIatSlot(
            g_joyGetPosExIatSlot,
            reinterpret_cast<void*>(&HookJoyGetPosEx),
            &originalTarget))
    {
        AppendLog(
            "[Input][WinMM] WARNING: DP.exe joyGetPosEx IAT patch failed; "
            "vanilla polling remains unchanged.\n");
        return false;
    }

    g_realJoyGetPosEx = reinterpret_cast<JoyGetPosExFn>(originalTarget);
    g_iatPatched.store(true, std::memory_order_release);
    g_active.store(true, std::memory_order_release);

    AppendLog(
        "[Input][WinMM] Legacy joystick polling fix active: repeated "
        "JOYERR_PARMS results are cached per slot and revalidated on WM_DEVICECHANGE.\n");
    return true;
}

MMRESULT PollLegacyJoystickSafely(UINT joyId, LPJOYINFOEX info)
{
    // AutoSwitch constructs a canonical JOYINFOEX request itself, so a 165 here
    // is the same confirmed per-slot failure and can share the suppression state.
    return CallGuardedJoyGetPosEx(joyId, info, true);
}

bool AttachLegacyJoystickDeviceNotifications(HWND window)
{
    if (!g_requested.load(std::memory_order_acquire) ||
        !g_available.load(std::memory_order_acquire))
    {
        return true;
    }

    if (window == nullptr)
        return false;

    if (g_windowAttached.load(std::memory_order_acquire))
        return g_gameWindow == window;

    // Publish the chain target before replacing the window procedure so a
    // device-change message cannot observe a transient null chain.
    WNDPROC previous = reinterpret_cast<WNDPROC>(
        GetWindowLongPtrW(window, GWLP_WNDPROC));
    if (previous == nullptr)
        return false;

    g_originalWndProc = previous;
    SetLastError(0);
    const LONG_PTR replaced = SetWindowLongPtrW(
        window,
        GWLP_WNDPROC,
        reinterpret_cast<LONG_PTR>(&LegacyJoystickWndProc));

    if (replaced == 0)
    {
        const DWORD error = GetLastError();
        g_originalWndProc = nullptr;
        char text[224] = {};
        sprintf_s(
            text,
            "[Input][WinMM] WARNING: could not attach WM_DEVICECHANGE observer "
            "to the game window (error %lu); suppressed slots will require restart to recover.\n",
            static_cast<unsigned long>(error));
        AppendLog(text);
        return false;
    }

    g_gameWindow = window;
    g_windowAttached.store(true, std::memory_order_release);
    AppendLog(
        "[Input][WinMM] WM_DEVICECHANGE recovery observer attached to the game window.\n");
    return true;
}

bool IsLegacyJoystickPollingFunctionAvailable()
{
    return g_realJoyGetPosEx != nullptr;
}

bool IsLegacyJoystickPollingFixAvailable()
{
    return g_available.load(std::memory_order_acquire);
}

bool IsLegacyJoystickPollingFixActive()
{
    return g_active.load(std::memory_order_acquire) &&
        g_iatPatched.load(std::memory_order_acquire);
}
