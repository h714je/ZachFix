#include "zachfix/ui/native_settings.h"

#include <Windows.h>
#include <MinHook.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cmath>
#include <cstring>

#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

namespace
{
static_assert(sizeof(void*) == 4, "Deadly Premonition native UI bridge requires Win32");

constexpr std::uint32_t kEventUpdate = 0x01;
constexpr std::uint32_t kEventRender = 0x12;
constexpr int kCOptionMainPage = 2;
constexpr int kCOptionMainInteractiveState = 1;
constexpr int kCOptionLastSettingRow = 8;
constexpr int kCOptionExitRow = 9;
constexpr int kCOptionLayoutSlot = 0;
constexpr int kRowCount = 3;

constexpr std::uint32_t kActionUp = 0x4001;
constexpr std::uint32_t kActionDown = 0x8002;

using NativeTaskCallback = void (__cdecl*)(void* object, std::uint32_t event, std::uintptr_t payload);
using COptionControllerFn = void (__thiscall*)(
    void* object,
    std::uint32_t event,
    std::uintptr_t payload);
using COptionStyleHelperFn = void (__thiscall*)(
    void* object,
    int activeRow,
    int activeChoice,
    char editMode);
using CreateTaskFn = void* (__thiscall*)(
    void* manager,
    std::uint8_t selector,
    int bucket,
    std::uint8_t dispatchClass,
    std::uint8_t alternateList);
using SetCallbackFn = void (__thiscall*)(
    void* object,
    NativeTaskCallback callback,
    std::uintptr_t initialPayload);
using InputPrepareFn = void* (__cdecl*)();
using InputPollFn = int (__thiscall*)(
    void* input,
    std::uint32_t player,
    int mode,
    std::uint32_t action);
using DrawFormattedTextFn = void (__cdecl*)(
    void* message,
    float x,
    float y,
    int alignment,
    const float* color,
    const char* format,
    ...);
using LayoutSlotAccessorFn = void* (__cdecl*)(int slot);
using LayoutRowElementAccessorFn = void* (__thiscall*)(void* layout, int index);
using RequestRemovalFn = void (__thiscall*)(void* object);

COptionControllerFn g_originalCOptionController = nullptr;
COptionStyleHelperFn g_cOptionStyleHelper = nullptr;
CreateTaskFn g_createTask = nullptr;
SetCallbackFn g_setCallback = nullptr;
InputPrepareFn g_inputPrepare = nullptr;
InputPollFn g_inputPoll = nullptr;
DrawFormattedTextFn g_drawFormattedText = nullptr;
LayoutSlotAccessorFn g_layoutSlotAccessor = nullptr;
LayoutRowElementAccessorFn g_layoutRowElementAccessor = nullptr;
void* g_cOptionControllerTarget = nullptr;

const DpBuildProfile* g_build = nullptr;
std::atomic_bool g_available{ false };

struct COptionEntryState
{
    void* object = nullptr;
    bool customSelected = false;
};

COptionEntryState g_cOptionEntry;

struct NativeSettingsState
{
    void* object = nullptr;
    int selectedRow = 0;
    bool testOption = true;
    bool active = false;
    bool closing = false;
    bool completed = false;
    bool parentResumeBlocked = false;
    void* parentObject = nullptr;
};

NativeSettingsState g_state;

bool MatchesBytes(uintptr_t address, const unsigned char* expected, size_t size)
{
    return address != 0 && expected != nullptr &&
        std::memcmp(reinterpret_cast<const void*>(address), expected, size) == 0;
}

bool AddressRangeInMainExe(uintptr_t address, size_t size)
{
    if (address == 0 || size == 0)
        return false;

    return IsMainExeAddress(reinterpret_cast<const void*>(address)) &&
        IsMainExeAddress(reinterpret_cast<const void*>(address + size - 1));
}

const std::uint8_t* COptionRowElementTableAddress()
{
    if (g_build == nullptr || g_mainExeBase == 0)
        return nullptr;

    const uintptr_t address = g_mainExeBase + g_build->nativeUi.cOptionRowElementTableRva;
    if (!AddressRangeInMainExe(address, 30))
        return nullptr;

    return reinterpret_cast<const std::uint8_t*>(address);
}

float* COptionSelectedColorAddress()
{
    if (g_build == nullptr || g_mainExeBase == 0)
        return nullptr;

    const uintptr_t address = g_mainExeBase + g_build->nativeUi.cOptionSelectedColorRva;
    if (!AddressRangeInMainExe(address, sizeof(float) * 4))
        return nullptr;

    return reinterpret_cast<float*>(address);
}

const float* COptionNormalColorAddress()
{
    if (g_build == nullptr || g_mainExeBase == 0)
        return nullptr;

    const uintptr_t address = g_mainExeBase + g_build->nativeUi.cOptionNormalColorRva;
    if (!AddressRangeInMainExe(address, sizeof(float) * 4))
        return nullptr;

    return reinterpret_cast<const float*>(address);
}

int& COptionPage(void* object)
{
    return *reinterpret_cast<int*>(static_cast<std::uint8_t*>(object) + 0x170);
}

int& COptionState(void* object)
{
    return *reinterpret_cast<int*>(static_cast<std::uint8_t*>(object) + 0x174);
}

int& COptionSelection(void* object)
{
    return *reinterpret_cast<int*>(static_cast<std::uint8_t*>(object) + 0x1FC);
}

bool IsCOptionMainInteractive(void* object)
{
    return object != nullptr &&
        COptionPage(object) == kCOptionMainPage &&
        COptionState(object) == kCOptionMainInteractiveState;
}

bool IsCOptionMainVisible(void* object)
{
    return object != nullptr && COptionPage(object) == kCOptionMainPage;
}

void** ManagerSingletonAddress()
{
    if (g_build == nullptr || g_mainExeBase == 0)
        return nullptr;

    const uintptr_t address = g_mainExeBase + g_build->nativeUi.managerSingletonPtrRva;
    if (!AddressRangeInMainExe(address, sizeof(void*)))
        return nullptr;

    return reinterpret_cast<void**>(address);
}

void** MessageSingletonAddress()
{
    if (g_build == nullptr || g_mainExeBase == 0)
        return nullptr;

    const uintptr_t address = g_mainExeBase + g_build->nativeUi.messageSingletonPtrRva;
    if (!AddressRangeInMainExe(address, sizeof(void*)))
        return nullptr;

    return reinterpret_cast<void**>(address);
}

std::uint8_t CurrentInputPlayer()
{
    if (g_build == nullptr || g_mainExeBase == 0)
        return 0;

    const uintptr_t address = g_mainExeBase + g_build->nativeUi.inputPlayerIndexRva;
    if (!AddressRangeInMainExe(address, sizeof(std::uint8_t)))
        return 0;

    return *reinterpret_cast<const std::uint8_t*>(address);
}

std::uint32_t ReadAction(std::uintptr_t rva, std::uint32_t fallback)
{
    if (g_mainExeBase == 0)
        return fallback;

    const uintptr_t address = g_mainExeBase + rva;
    if (!AddressRangeInMainExe(address, sizeof(std::uint32_t)))
        return fallback;

    const std::uint32_t value = *reinterpret_cast<const std::uint32_t*>(address);
    return value != 0 ? value : fallback;
}

bool PollAction(int mode, std::uint32_t action)
{
    if (g_inputPrepare == nullptr || g_inputPoll == nullptr)
        return false;

    void* input = g_inputPrepare();
    if (input == nullptr)
        return false;

    return g_inputPoll(input, CurrentInputPlayer(), mode, action) != 0;
}

bool PollConfirm()
{
    return PollAction(
        1,
        ReadAction(g_build->nativeUi.confirmActionRva, 0x0800));
}

bool PollCancel()
{
    return PollAction(
        1,
        ReadAction(g_build->nativeUi.cancelActionRva, 0x1000));
}

bool RequestChildRemoval(void* object)
{
    if (object == nullptr || g_build == nullptr || g_mainExeBase == 0)
        return false;

    void** vtable = *reinterpret_cast<void***>(object);
    if (vtable == nullptr)
        return false;

    auto remove = reinterpret_cast<RequestRemovalFn>(vtable[0x30 / sizeof(void*)]);
    const uintptr_t expected = g_mainExeBase + g_build->nativeUi.removalRequestRva;

    if (reinterpret_cast<uintptr_t>(remove) != expected)
    {
        AppendLog(
            "[NativeUI] WARNING: child +0x30 removal virtual did not match the verified CRdObject target; leaving the task alive rather than calling an unknown virtual.\n");
        return false;
    }

    remove(object);
    return true;
}

void BeginClose()
{
    if (!g_state.active || g_state.closing)
        return;

    void* object = g_state.object;
    if (!RequestChildRemoval(object))
        return;

    g_state.closing = true;
    g_state.completed = true;
    AppendLog("[NativeUI] ZachFix Settings child requested normal manager removal.\n");
}

void DrawLine(float y, bool selected, const char* text)
{
    if (g_drawFormattedText == nullptr || text == nullptr)
        return;

    void** messageAddress = MessageSingletonAddress();
    if (messageAddress == nullptr || *messageAddress == nullptr)
        return;

    // Retail lightweight menus use a null color pointer for the selected row
    // and a 0.5/0.5/0.5/1.0 RGBA vector for inactive rows.
    static constexpr float kInactiveColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
    const float* color = selected ? nullptr : kInactiveColor;

    g_drawFormattedText(
        *messageAddress,
        640.0f,
        y,
        0,
        color,
        "%s",
        text);
}

struct COptionRenderInsertion
{
    float customX = 0.0f;
    float customY = 0.0f;
};

std::uint8_t* COptionRowElement(int row)
{
    if (row < 0 || row > kCOptionExitRow ||
        g_layoutSlotAccessor == nullptr || g_layoutRowElementAccessor == nullptr)
    {
        return nullptr;
    }

    const std::uint8_t* rowElements = COptionRowElementTableAddress();
    if (rowElements == nullptr)
        return nullptr;

    // Column 0 is the stock label element styled by FUN_006245B0 / FUN_00624530.
    const std::uint8_t elementIndex = rowElements[row * 3];
    if (elementIndex == 0xFF)
        return nullptr;

    void* layout = g_layoutSlotAccessor(kCOptionLayoutSlot);
    if (layout == nullptr)
        return nullptr;

    return static_cast<std::uint8_t*>(
        g_layoutRowElementAccessor(layout, elementIndex));
}

int COptionRowChoice(void* object, int row)
{
    if (object == nullptr || row < 0 || row > kCOptionExitRow)
        return 0;

    return *reinterpret_cast<const int*>(
        static_cast<const std::uint8_t*>(object) + 0x204 + row * sizeof(int));
}

void SyncCOptionStockVisual(void* object, int selectedStockRow)
{
    if (object == nullptr || g_cOptionStyleHelper == nullptr)
        return;

    // FUN_006245B0 / FUN_00624530 is stock-specific and must not be treated
    // as a generic ZachFix highlighter. Here it is reused only for its exact
    // owner, COption page 2, after our pseudo-row intercepts navigation that
    // retail would normally finish by calling this helper itself.
    //
    // activeRow == -1 is safe for this helper: the retail body only compares
    // the value against loop rows 0..9 and never indexes a table with it. This
    // yields a fully neutral stock selection while ZachFix owns focus.
    const int activeChoice = selectedStockRow >= 0
        ? COptionRowChoice(object, selectedStockRow)
        : 0;
    g_cOptionStyleHelper(object, selectedStockRow, activeChoice, 0);
}

bool ReadCOptionElementPosition(
    std::uint8_t* element,
    float& x,
    float& y)
{
    if (element == nullptr)
        return false;

    // The page-2 styling records are 0x50-byte CLayout elements returned by
    // 004588C0 / 004588F0. Their live draw position is +0x40/+0x44.
    const float candidateX = *reinterpret_cast<const float*>(element + 0x40);
    const float candidateY = *reinterpret_cast<const float*>(element + 0x44);
    if (!std::isfinite(candidateX) || !std::isfinite(candidateY) ||
        candidateX < -2048.0f || candidateX > 4096.0f ||
        candidateY < -2048.0f || candidateY > 4096.0f)
    {
        return false;
    }

    x = candidateX;
    y = candidateY;
    return true;
}

bool ResolveCOptionRenderInsertion(COptionRenderInsertion& insertion)
{
    float row8X = 0.0f;
    float row8Y = 0.0f;
    if (!ReadCOptionElementPosition(
            COptionRowElement(kCOptionLastSettingRow), row8X, row8Y))
    {
        return false;
    }

    // Derive one native row step from the nearest preceding page-2 label.
    // Hidden stock rows still retain their XLY geometry, so this stays tied to
    // the active localized layout instead of a hardcoded 1280x720 coordinate.
    float rowStep = 0.0f;
    for (int row = kCOptionLastSettingRow - 1; row >= 0; --row)
    {
        float previousX = 0.0f;
        float previousY = 0.0f;
        if (!ReadCOptionElementPosition(COptionRowElement(row), previousX, previousY))
            continue;

        const float candidateStep = row8Y - previousY;
        if (std::fabs(candidateStep) >= 16.0f &&
            std::fabs(candidateStep) <= 160.0f)
        {
            rowStep = candidateStep;
            break;
        }
    }

    float exitX = 0.0f;
    float exitY = 0.0f;
    const bool haveExit = ReadCOptionElementPosition(
        COptionRowElement(kCOptionExitRow), exitX, exitY);

    if (rowStep == 0.0f)
    {
        // A valid Exit anchor still gives a layout-derived placement if a
        // localized asset omits all preceding row labels.
        if (!haveExit)
            return false;

        const float gap = exitY - row8Y;
        if (std::fabs(gap) < 32.0f || std::fabs(gap) > 768.0f)
            return false;

        rowStep = gap * 0.5f;
    }

    // The runtime C-string primitive does not share the baked page-2 label's
    // exact glyph box/alignment, even when fed the same CLayout anchor. Keep
    // placement layout-relative, but apply a small fraction-of-row visual
    // correction so the custom label sits naturally under row 8 rather than
    // looking slightly low/right inside the otherwise empty gap.
    float customY = row8Y + rowStep * 0.75f;
    if (haveExit)
    {
        const bool between = exitY >= row8Y
            ? (customY > row8Y && customY < exitY)
            : (customY < row8Y && customY > exitY);
        if (!between)
            customY = row8Y + (exitY - row8Y) * 0.45f;
    }

    if (!std::isfinite(customY))
        return false;

    float horizontalNudge = std::fabs(rowStep) * 0.5f;
    if (horizontalNudge > 32.0f)
        horizontalNudge = 32.0f;

    insertion.customX = row8X - horizontalNudge;
    insertion.customY = customY;
    return true;
}

void DrawCOptionEntry(
    void* object,
    const COptionRenderInsertion& insertion,
    bool selected,
    const float selectedColor[4])
{
    if (object == nullptr || g_drawFormattedText == nullptr)
        return;

    void** messageAddress = MessageSingletonAddress();
    const float* normalColor = COptionNormalColorAddress();
    if (messageAddress == nullptr || *messageAddress == nullptr || normalColor == nullptr)
        return;

    const float* source = selected ? selectedColor : normalColor;
    float color[4] = { source[0], source[1], source[2], source[3] };
    color[3] = *reinterpret_cast<const float*>(
        static_cast<const std::uint8_t*>(object) + 0x27C);

    // Page-2 native message draws use presentation mode 4. The generic child
    // uses mode 0, but carrying mode 0 into COption produces the wrong visual
    // treatment on this surface.
    auto* message = static_cast<std::uint8_t*>(*messageAddress);
    const std::uint16_t savedTextMode =
        *reinterpret_cast<const std::uint16_t*>(message + 0x6C);
    *reinterpret_cast<std::uint16_t*>(message + 0x6C) = 0x10;

    g_drawFormattedText(
        *messageAddress,
        insertion.customX,
        insertion.customY,
        4,
        color,
        "%s",
        "ZachFix Settings");

    *reinterpret_cast<std::uint16_t*>(message + 0x6C) = savedTextMode;
}

void DrawNativeSettingsPage()
{
    DrawLine(180.0f, true, "ZachFix Settings");

    char testOption[64] = {};
    sprintf_s(
        testOption,
        sizeof(testOption),
        "%s Test Option        %s",
        g_state.selectedRow == 0 ? ">" : " ",
        g_state.testOption ? "ON" : "OFF");
    DrawLine(244.0f, g_state.selectedRow == 0, testOption);

    char valueLine[64] = {};
    sprintf_s(
        valueLine,
        sizeof(valueLine),
        "%s Another Value      123",
        g_state.selectedRow == 1 ? ">" : " ");
    DrawLine(276.0f, g_state.selectedRow == 1, valueLine);

    char closeLine[64] = {};
    sprintf_s(
        closeLine,
        sizeof(closeLine),
        "%s Close",
        g_state.selectedRow == 2 ? ">" : " ");
    DrawLine(308.0f, g_state.selectedRow == 2, closeLine);
}

void UpdateNativeSettingsPage()
{
    if (PollAction(2, kActionUp))
    {
        g_state.selectedRow = (g_state.selectedRow + kRowCount - 1) % kRowCount;
        return;
    }

    if (PollAction(2, kActionDown))
    {
        g_state.selectedRow = (g_state.selectedRow + 1) % kRowCount;
        return;
    }

    if (PollCancel())
    {
        BeginClose();
        return;
    }

    if (!PollConfirm())
        return;

    switch (g_state.selectedRow)
    {
    case 0:
        g_state.testOption = !g_state.testOption;
        break;
    case 1:
        // Deliberately inert in the lifecycle proof. It exists only to prove
        // native row navigation before real ZachFix settings are connected.
        break;
    case 2:
        BeginClose();
        break;
    default:
        break;
    }
}

void __cdecl NativeSettingsTaskCallback(
    void* object,
    std::uint32_t event,
    std::uintptr_t)
{
    if (!g_state.active || object == nullptr || object != g_state.object)
        return;

    // The callback setter dispatches event 0 synchronously. All external state
    // is installed before SetCallback precisely so this first callback is safe.
    if (event == 0)
        return;

    // Manager removal is deferred and retail can still issue render event 0x12
    // in the same frame after +0x30 has been called. Keep a tombstone and no-op.
    if (g_state.closing)
        return;

    if (event == kEventUpdate)
    {
        UpdateNativeSettingsPage();
        return;
    }

    if (event == kEventRender)
        DrawNativeSettingsPage();
}

bool CreateNativeSettingsTask(void* parentObject)
{
    if (g_state.active || g_createTask == nullptr || g_setCallback == nullptr)
        return false;

    void** managerAddress = ManagerSingletonAddress();
    if (managerAddress == nullptr || *managerAddress == nullptr)
    {
        AppendLog("[NativeUI] WARNING: native manager singleton unavailable; ZachFix Settings was not opened.\n");
        return false;
    }

    void** messageAddress = MessageSingletonAddress();
    if (messageAddress == nullptr || *messageAddress == nullptr)
    {
        AppendLog("[NativeUI] WARNING: native message singleton unavailable; refusing to open a page that could not render.\n");
        return false;
    }

    if (g_inputPrepare == nullptr || g_inputPrepare() == nullptr)
    {
        AppendLog("[NativeUI] WARNING: native input controller unavailable; refusing to open a page that could not be closed safely.\n");
        return false;
    }

    void* object = g_createTask(*managerAddress, 0, 0, 0, 1);
    if (object == nullptr)
    {
        AppendLog("[NativeUI] WARNING: selector-0 task allocation failed.\n");
        return false;
    }

    // Publish the whole external state before SetCallback: retail immediately
    // invokes event 0 from inside the setter.
    g_state.object = object;
    g_state.selectedRow = 0;
    g_state.testOption = true;
    g_state.active = true;
    g_state.closing = false;
    g_state.completed = false;
    g_state.parentResumeBlocked = false;
    g_state.parentObject = parentObject;

    g_setCallback(object, &NativeSettingsTaskCallback, 0);

    AppendLog("[NativeUI] Opened ZachFix Settings selector-0 native task.\n");
    return true;
}

void ResetCompletedState()
{
    const bool testOption = g_state.testOption;
    g_state = {};
    g_state.testOption = testOption;
}

bool HandleCOptionMainUpdate(void* object)
{
    if (!IsCOptionMainInteractive(object))
        return false;

    if (g_cOptionEntry.object != object)
    {
        g_cOptionEntry.object = object;
        g_cOptionEntry.customSelected = false;
    }

    int& selection = COptionSelection(object);
    if (selection < 0 || selection > kCOptionExitRow)
    {
        // Never propagate an invalid synthetic selection into stock COption.
        // This also fails closed if another mod has changed the row contract.
        g_cOptionEntry.customSelected = false;
        return false;
    }

    COptionRenderInsertion insertion;
    if (!ResolveCOptionRenderInsertion(insertion))
    {
        // Geometry is part of the entry contract. Without a real page-2 anchor,
        // leave retail navigation untouched rather than creating an invisible
        // ZachFix selection.
        if (g_cOptionEntry.customSelected)
        {
            g_cOptionEntry.customSelected = false;
            selection = kCOptionExitRow;
            SyncCOptionStockVisual(object, kCOptionExitRow);
        }
        return false;
    }

    const bool up = PollAction(2, kActionUp);
    const bool down = PollAction(2, kActionDown);

    if (g_cOptionEntry.customSelected)
    {
        if (PollConfirm())
        {
            CreateNativeSettingsTask(object);
            return true;
        }

        if (up)
        {
            g_cOptionEntry.customSelected = false;
            selection = kCOptionLastSettingRow;
            SyncCOptionStockVisual(object, kCOptionLastSettingRow);
            return true;
        }

        if (down)
        {
            g_cOptionEntry.customSelected = false;
            selection = kCOptionExitRow;
            SyncCOptionStockVisual(object, kCOptionExitRow);
            return true;
        }

        // Cancel remains a stock COption exit action. Keep the shadow row at
        // Exit so the stock path sees a valid, blank-description selection.
        if (PollCancel())
            g_cOptionEntry.customSelected = false;

        return false;
    }

    // Insert one external pseudo-row between the final visible stock setting
    // (row 8) and stock Exit (row 9). While ZachFix is selected, row 9 is the
    // safe shadow value: its stock description/value surface is blank, and the
    // visual highlight is neutralized separately.
    if (down && selection == kCOptionLastSettingRow)
    {
        selection = kCOptionExitRow;
        g_cOptionEntry.customSelected = true;
        SyncCOptionStockVisual(object, -1);
        return true;
    }

    if (up && selection == kCOptionExitRow)
    {
        g_cOptionEntry.customSelected = true;
        SyncCOptionStockVisual(object, -1);
        return true;
    }

    return false;
}

void RenderCOptionWithInjectedEntry(
    void* object,
    std::uint32_t event,
    std::uintptr_t payload)
{
    if (g_originalCOptionController == nullptr)
        return;

    float selectedColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    if (float* selectedColorAddress = COptionSelectedColorAddress())
        std::memcpy(selectedColor, selectedColorAddress, sizeof(selectedColor));

    const bool customSelected =
        g_cOptionEntry.object == object &&
        g_cOptionEntry.customSelected &&
        IsCOptionMainInteractive(object);

    COptionRenderInsertion insertion;
    if (!ResolveCOptionRenderInsertion(insertion))
    {
        // Fail closed: if the stock page-2 layout cannot provide real geometry,
        // do not draw an invisible or hardcoded synthetic row.
        g_originalCOptionController(object, event, payload);
        return;
    }

    if (customSelected)
        SyncCOptionStockVisual(object, -1);

    g_originalCOptionController(object, event, payload);
    DrawCOptionEntry(object, insertion, customSelected, selectedColor);
}

void __fastcall HookCOptionController(
    void* object,
    void*,
    std::uint32_t event,
    std::uintptr_t payload)
{
    if (g_originalCOptionController == nullptr)
        return;

    const bool ownsChild =
        g_state.active &&
        g_state.parentObject == object;

    if (event == kEventUpdate && ownsChild)
    {
        // If another retail transition invalidates the parent page while the
        // ZachFix child is alive, request normal child removal rather than
        // leaving an orphaned selector-0 task behind.
        if (!g_state.completed && !IsCOptionMainVisible(object))
        {
            BeginClose();
            return;
        }

        if (g_state.completed)
        {
            if (!g_state.parentResumeBlocked)
            {
                g_state.parentResumeBlocked = true;
                return;
            }

            ResetCompletedState();
            g_originalCOptionController(object, event, payload);
            return;
        }

        // COption remains manager-owned and loaded, but receives no menu
        // input while the ZachFix child owns focus.
        return;
    }

    if (event == kEventRender && ownsChild)
    {
        // Match the stock parent/child presentation contract: do not render
        // the Options rows behind the active ZachFix page.
        return;
    }

    if (event == kEventUpdate && IsCOptionMainInteractive(object))
    {
        if (HandleCOptionMainUpdate(object))
            return;
    }

    if (event == kEventRender && IsCOptionMainVisible(object))
    {
        if (g_cOptionEntry.object != object)
        {
            g_cOptionEntry.object = object;
            g_cOptionEntry.customSelected = false;
        }

        RenderCOptionWithInjectedEntry(object, event, payload);
        return;
    }

    g_originalCOptionController(object, event, payload);

    if (g_cOptionEntry.object == object && !IsCOptionMainVisible(object))
        g_cOptionEntry.customSelected = false;
}

bool ValidateNativeUiProfile(const DpBuildProfile* build)
{
    if (build == nullptr || g_mainExeBase == 0)
        return false;

    const auto& ui = build->nativeUi;
    const uintptr_t factoryTarget = g_mainExeBase + ui.genericTaskFactoryRva;
    const uintptr_t setterTarget = g_mainExeBase + ui.callbackSetterRva;
    const uintptr_t prepareTarget = g_mainExeBase + ui.inputPrepareRva;
    const uintptr_t pollTarget = g_mainExeBase + ui.inputPollRva;
    const uintptr_t drawTarget = g_mainExeBase + ui.formattedTextRva;
    const uintptr_t layoutSlotTarget = g_mainExeBase + ui.layoutSlotAccessorRva;
    const uintptr_t layoutRowElementTarget =
        g_mainExeBase + ui.layoutRowElementAccessorRva;
    const uintptr_t cOptionCallbackTarget = g_mainExeBase + ui.cOptionCallbackRva;
    const uintptr_t cOptionControllerTarget = g_mainExeBase + ui.cOptionControllerRva;
    const uintptr_t cOptionStyleHelperTarget = g_mainExeBase + ui.cOptionStyleHelperRva;

    if (!AddressRangeInMainExe(factoryTarget, 16) ||
        !AddressRangeInMainExe(setterTarget, 16) ||
        !AddressRangeInMainExe(prepareTarget, 12) ||
        !AddressRangeInMainExe(pollTarget, 16) ||
        !AddressRangeInMainExe(drawTarget, 16) ||
        !AddressRangeInMainExe(layoutSlotTarget, 16) ||
        !AddressRangeInMainExe(layoutRowElementTarget, 16) ||
        !AddressRangeInMainExe(cOptionCallbackTarget, 16) ||
        !AddressRangeInMainExe(cOptionControllerTarget, 16) ||
        !AddressRangeInMainExe(cOptionStyleHelperTarget, 16) ||
        COptionRowElementTableAddress() == nullptr ||
        COptionSelectedColorAddress() == nullptr ||
        COptionNormalColorAddress() == nullptr ||
        ManagerSingletonAddress() == nullptr ||
        MessageSingletonAddress() == nullptr)
    {
        return false;
    }

    static constexpr unsigned char kSteamInputPrepare[] = {
        0x56, 0xE8, 0x2A, 0xD7, 0x2D, 0x00,
        0x8B, 0x35, 0x10, 0x9E, 0xBD, 0x00,
    };
    static constexpr unsigned char kGogInputPrepare[] = {
        0x56, 0xE8, 0x8A, 0xD7, 0x2D, 0x00,
        0x8B, 0x35, 0x10, 0x9E, 0xBD, 0x00,
    };
    static constexpr unsigned char kFactoryPrefix[] = {
        0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68,
    };
    static constexpr unsigned char kSetterPrefix[] = {
        0x55, 0x8B, 0xEC, 0x51, 0x89, 0x4D, 0xFC,
        0x8B, 0x45, 0xFC, 0x8B, 0x4D, 0x08, 0x89, 0x48, 0x44,
    };
    static constexpr unsigned char kPollPrefix[] = {
        0x55, 0x8B, 0xEC, 0x51, 0x89, 0x4D, 0xFC,
        0x0F, 0xB6, 0x45, 0x08,
    };
    static constexpr unsigned char kDrawPrefix[] = {
        0x81, 0xEC, 0x08, 0x01, 0x00, 0x00,
        0xA1, 0xCC, 0x5E, 0xBD, 0x00,
    };
    static constexpr unsigned char kLayoutSlotPrefix[] = {
        0x8B, 0x44, 0x24, 0x04, 0x85, 0xC0, 0x7D, 0x0E, 0x33, 0xC0,
    };
    static constexpr unsigned char kLayoutRowElementPrefix[] = {
        0x8B, 0x44, 0x24, 0x04, 0x56, 0x8B, 0xF1, 0x33,
        0xC9, 0x3B, 0x46, 0x7C, 0x0F, 0x9D, 0xC1, 0x33,
    };
    static constexpr unsigned char kCOptionCallbackPrefix[] = {
        0x8B, 0x44, 0x24, 0x0C, 0x8B, 0x4C, 0x24, 0x08,
        0x50, 0x51, 0x8B, 0x4C, 0x24, 0x0C,
    };
    static constexpr unsigned char kCOptionControllerPrefix[] = {
        0x0F, 0xB6, 0x44, 0x24, 0x04, 0x83, 0xEC, 0x08,
        0x55, 0x8B, 0xE9, 0x83, 0xF8, 0x12,
    };
    static constexpr unsigned char kCOptionStyleHelperPrefix[] = {
        0x51, 0x53, 0x55, 0x8B, 0x6C, 0x24, 0x10,
        0x56, 0x57, 0x8B, 0xF9, 0x33, 0xDB,
    };
    static constexpr unsigned char kCOptionPage2Rows[] = {
        0x05, 0x0E, 0x14,
        0x06, 0xFF, 0xFF,
        0x07, 0xFF, 0xFF,
        0x08, 0x10, 0x1A,
        0x09, 0x11, 0x1E,
        0x0A, 0xFF, 0xFF,
        0x0B, 0xFF, 0xFF,
        0x0C, 0xFF, 0xFF,
        0x0D, 0x12, 0x22,
        0x25, 0x13, 0xFF,
    };

    const bool commonTargetsMatch =
        MatchesBytes(factoryTarget, kFactoryPrefix, sizeof(kFactoryPrefix)) &&
        MatchesBytes(setterTarget, kSetterPrefix, sizeof(kSetterPrefix)) &&
        MatchesBytes(pollTarget, kPollPrefix, sizeof(kPollPrefix)) &&
        MatchesBytes(drawTarget, kDrawPrefix, sizeof(kDrawPrefix)) &&
        MatchesBytes(layoutSlotTarget, kLayoutSlotPrefix, sizeof(kLayoutSlotPrefix)) &&
        MatchesBytes(layoutRowElementTarget, kLayoutRowElementPrefix, sizeof(kLayoutRowElementPrefix)) &&
        MatchesBytes(cOptionCallbackTarget, kCOptionCallbackPrefix, sizeof(kCOptionCallbackPrefix)) &&
        MatchesBytes(cOptionControllerTarget, kCOptionControllerPrefix, sizeof(kCOptionControllerPrefix)) &&
        MatchesBytes(cOptionStyleHelperTarget, kCOptionStyleHelperPrefix, sizeof(kCOptionStyleHelperPrefix)) &&
        MatchesBytes(
            reinterpret_cast<uintptr_t>(COptionRowElementTableAddress()),
            kCOptionPage2Rows,
            sizeof(kCOptionPage2Rows));
    if (!commonTargetsMatch)
        return false;

    switch (build->build)
    {
    case DpBuild::Steam101b:
        return MatchesBytes(prepareTarget, kSteamInputPrepare, sizeof(kSteamInputPrepare));
    case DpBuild::Gog101b:
        return MatchesBytes(prepareTarget, kGogInputPrepare, sizeof(kGogInputPrepare));
    default:
        return false;
    }
}
} // namespace

bool InstallNativeSettingsUiBridge()
{
    if (g_available.load(std::memory_order_acquire))
        return true;

    g_build = GetDpBuildProfile();
    if (!ValidateNativeUiProfile(g_build))
    {
        AppendLog(
            "[NativeUI] Native Settings integration unavailable: unsupported build or signature mismatch.\n");
        g_build = nullptr;
        return false;
    }

    const auto& ui = g_build->nativeUi;
    g_createTask = reinterpret_cast<CreateTaskFn>(
        g_mainExeBase + ui.genericTaskFactoryRva);
    g_setCallback = reinterpret_cast<SetCallbackFn>(
        g_mainExeBase + ui.callbackSetterRva);
    g_inputPrepare = reinterpret_cast<InputPrepareFn>(
        g_mainExeBase + ui.inputPrepareRva);
    g_inputPoll = reinterpret_cast<InputPollFn>(
        g_mainExeBase + ui.inputPollRva);
    g_drawFormattedText = reinterpret_cast<DrawFormattedTextFn>(
        g_mainExeBase + ui.formattedTextRva);
    g_layoutSlotAccessor = reinterpret_cast<LayoutSlotAccessorFn>(
        g_mainExeBase + ui.layoutSlotAccessorRva);
    g_layoutRowElementAccessor = reinterpret_cast<LayoutRowElementAccessorFn>(
        g_mainExeBase + ui.layoutRowElementAccessorRva);
    g_cOptionStyleHelper = reinterpret_cast<COptionStyleHelperFn>(
        g_mainExeBase + ui.cOptionStyleHelperRva);
    g_cOptionControllerTarget = reinterpret_cast<void*>(
        g_mainExeBase + ui.cOptionControllerRva);

    const MH_STATUS createStatus = MH_CreateHook(
        g_cOptionControllerTarget,
        reinterpret_cast<void*>(&HookCOptionController),
        reinterpret_cast<void**>(&g_originalCOptionController));
    if (createStatus != MH_OK)
    {
        char text[192] = {};
        sprintf_s(
            text,
            sizeof(text),
            "[NativeUI] COption controller MH_CreateHook failed: %d.\n",
            static_cast<int>(createStatus));
        AppendLog(text);
        g_originalCOptionController = nullptr;
        g_cOptionControllerTarget = nullptr;
        return false;
    }

    const MH_STATUS enableStatus = MH_EnableHook(g_cOptionControllerTarget);
    if (enableStatus != MH_OK)
    {
        char text[224] = {};
        sprintf_s(
            text,
            sizeof(text),
            "[NativeUI] COption controller MH_EnableHook failed: %d; rolling back.\n",
            static_cast<int>(enableStatus));
        AppendLog(text);

        const MH_STATUS removeStatus = MH_RemoveHook(g_cOptionControllerTarget);
        if (removeStatus != MH_OK && removeStatus != MH_ERROR_NOT_CREATED)
        {
            AppendLog(
                "[NativeUI] WARNING: COption hook rollback was incomplete; trampoline retained for safety.\n");
        }
        else
        {
            g_originalCOptionController = nullptr;
            g_cOptionControllerTarget = nullptr;
        }
        return false;
    }

    g_available.store(true, std::memory_order_release);

    char text[256] = {};
    sprintf_s(
        text,
        sizeof(text),
        "[NativeUI] ZachFix Settings integrated into stock Options on %s: COption page-2 doorway and selector-0 child lifecycle active.\n",
        g_build->name);
    AppendLog(text);
    return true;
}

bool IsNativeSettingsUiAvailable()
{
    return g_available.load(std::memory_order_acquire);
}
