#include "zachfix/input/providers/gamepad_provider_sdl.h"

#include "zachfix/core/logging.h"

#include <SDL3/SDL.h>

#include <Windows.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <iterator>
#include <mutex>

namespace
{
struct SdlGamepadSlot
{
    SDL_JoystickID id = 0;
    SDL_Gamepad* gamepad = nullptr;
    GamepadState lastState{};
    std::uint32_t sequence = 1;
    bool haveLastState = false;
};

std::array<SdlGamepadSlot, kMaxGamepads> g_slots{};
std::mutex g_sdlMutex;
bool g_sdlProviderRequested = false;
bool g_sdlInitialized = false;
bool g_sdlInitializationFailed = false;
std::uint64_t g_nextEnumerationTick = 0;
constexpr std::uint64_t kEnumerationIntervalMs = 250;

constexpr char kSdlDynamicApiEnv[] = "SDL3_DYNAMIC_API";
constexpr char kSdlOverrideRelativePath[] = "ZachFix\\SDL3.dll";
constexpr std::size_t kSdlDynamicApiBufferSize = 512;

enum class SdlOverrideSource
{
    Embedded,
    Environment,
    ZachFixDirectory
};

SdlOverrideSource g_sdlOverrideSource = SdlOverrideSource::Embedded;
char g_sdlOverrideRequest[kSdlDynamicApiBufferSize] = {};
bool g_sdlRuntimeVersionLogged = false;

bool BuildZachFixSdlOverridePath(char* path, std::size_t pathCount)
{
    if (path == nullptr || pathCount == 0)
        return false;

    const DWORD length = GetModuleFileNameA(
        nullptr,
        path,
        static_cast<DWORD>(pathCount));
    if (length == 0 || length >= pathCount)
        return false;

    char* slash = std::strrchr(path, '\\');
    if (slash == nullptr)
        return false;

    *(slash + 1) = '\0';
    return strcat_s(path, pathCount, kSdlOverrideRelativePath) == 0;
}

bool IsRegularFile(const char* path)
{
    if (path == nullptr || path[0] == '\0')
        return false;

    const DWORD attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void ConfigureSdlDynamicApiOverrideLocked()
{
    g_sdlOverrideSource = SdlOverrideSource::Embedded;
    g_sdlOverrideRequest[0] = '\0';

    char environmentOverride[kSdlDynamicApiBufferSize] = {};
    const DWORD environmentLength = GetEnvironmentVariableA(
        kSdlDynamicApiEnv,
        environmentOverride,
        static_cast<DWORD>(std::size(environmentOverride)));

    if (environmentLength != 0)
    {
        if (environmentLength < std::size(environmentOverride))
        {
            strcpy_s(g_sdlOverrideRequest, environmentOverride);
            g_sdlOverrideSource = SdlOverrideSource::Environment;

            char text[768] = {};
            sprintf_s(
                text,
                "[Input][GamepadBackend] SDL Dynamic API: honoring existing %s=%s; "
                "embedded SDL remains the compatibility fallback.\n",
                kSdlDynamicApiEnv,
                g_sdlOverrideRequest);
            AppendLog(text);
        }
        else
        {
            AppendLog(
                "[Input][GamepadBackend] SDL Dynamic API: existing SDL3_DYNAMIC_API "
                "value is too long for SDL3's Windows loader; leaving it untouched.\n");
        }
        return;
    }

    char overridePath[MAX_PATH] = {};
    if (!BuildZachFixSdlOverridePath(overridePath, std::size(overridePath)) ||
        !IsRegularFile(overridePath))
    {
        AppendLog(
            "[Input][GamepadBackend] SDL Dynamic API: no ZachFix\\SDL3.dll override; "
            "using embedded SDL3 fallback.\n");
        return;
    }

    if (!SetEnvironmentVariableA(kSdlDynamicApiEnv, overridePath))
    {
        char text[384] = {};
        sprintf_s(
            text,
            "[Input][GamepadBackend] SDL Dynamic API: failed to register "
            "ZachFix\\SDL3.dll override (Win32 error=%lu); using embedded SDL3.\n",
            static_cast<unsigned long>(GetLastError()));
        AppendLog(text);
        return;
    }

    strcpy_s(g_sdlOverrideRequest, overridePath);
    g_sdlOverrideSource = SdlOverrideSource::ZachFixDirectory;
    AppendLog(
        "[Input][GamepadBackend] SDL Dynamic API: ZachFix\\SDL3.dll override found; "
        "external SDL requested with embedded fallback retained.\n");
}

const char* SdlOverrideRequestName()
{
    switch (g_sdlOverrideSource)
    {
    case SdlOverrideSource::Environment:
        return "environment";
    case SdlOverrideSource::ZachFixDirectory:
        return "ZachFix\\SDL3.dll";
    case SdlOverrideSource::Embedded:
    default:
        return "none (embedded)";
    }
}

void LogSdlRuntimeVersionLocked()
{
    if (g_sdlRuntimeVersionLogged)
        return;

    g_sdlRuntimeVersionLogged = true;

    const int compiledVersion = SDL_VERSION;
    const int runtimeVersion = SDL_GetVersion();
    const char* revision = SDL_GetRevision();

    char text[640] = {};
    sprintf_s(
        text,
        "[Input][GamepadBackend] SDL runtime: compiled=%d.%d.%d runtime=%d.%d.%d "
        "override=%s revision=%s.\n",
        SDL_VERSIONNUM_MAJOR(compiledVersion),
        SDL_VERSIONNUM_MINOR(compiledVersion),
        SDL_VERSIONNUM_MICRO(compiledVersion),
        SDL_VERSIONNUM_MAJOR(runtimeVersion),
        SDL_VERSIONNUM_MINOR(runtimeVersion),
        SDL_VERSIONNUM_MICRO(runtimeVersion),
        SdlOverrideRequestName(),
        (revision != nullptr && revision[0] != '\0') ? revision : "unknown");
    AppendLog(text);
}

bool SamePhysicalState(const GamepadState& a, const GamepadState& b)
{
    return a.leftX == b.leftX &&
           a.leftY == b.leftY &&
           a.rightX == b.rightX &&
           a.rightY == b.rightY &&
           a.leftTrigger == b.leftTrigger &&
           a.rightTrigger == b.rightTrigger &&
           a.buttons == b.buttons;
}

std::int16_t InvertSdlYAxis(Sint16 value)
{
    const std::int32_t inverted = -static_cast<std::int32_t>(value);
    return static_cast<std::int16_t>(std::clamp<std::int32_t>(
        inverted,
        std::numeric_limits<std::int16_t>::min(),
        std::numeric_limits<std::int16_t>::max()));
}

std::uint8_t SdlTriggerToByte(Sint16 value)
{
    const std::int32_t clamped = std::clamp<std::int32_t>(value, 0, 32767);
    return static_cast<std::uint8_t>(
        (clamped * 255 + 16383) / 32767);
}

std::uint32_t ReadSdlButtons(SDL_Gamepad* gamepad)
{
    std::uint32_t buttons = 0;
    auto setButton = [&](SDL_GamepadButton sdlButton, GamepadButton button)
    {
        if (SDL_GetGamepadButton(gamepad, sdlButton))
            buttons |= static_cast<std::uint32_t>(button);
    };

    // SDL face buttons are positional. Map South/East/West/North onto the
    // Xbox-style canonical A/B/X/Y names used by the existing DP bridge.
    setButton(SDL_GAMEPAD_BUTTON_SOUTH, GamepadButton_A);
    setButton(SDL_GAMEPAD_BUTTON_EAST, GamepadButton_B);
    setButton(SDL_GAMEPAD_BUTTON_WEST, GamepadButton_X);
    setButton(SDL_GAMEPAD_BUTTON_NORTH, GamepadButton_Y);
    setButton(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, GamepadButton_LeftShoulder);
    setButton(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, GamepadButton_RightShoulder);
    setButton(SDL_GAMEPAD_BUTTON_BACK, GamepadButton_Back);
    setButton(SDL_GAMEPAD_BUTTON_START, GamepadButton_Start);
    setButton(SDL_GAMEPAD_BUTTON_LEFT_STICK, GamepadButton_LeftThumb);
    setButton(SDL_GAMEPAD_BUTTON_RIGHT_STICK, GamepadButton_RightThumb);
    setButton(SDL_GAMEPAD_BUTTON_DPAD_UP, GamepadButton_DpadUp);
    setButton(SDL_GAMEPAD_BUTTON_DPAD_DOWN, GamepadButton_DpadDown);
    setButton(SDL_GAMEPAD_BUTTON_DPAD_LEFT, GamepadButton_DpadLeft);
    setButton(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, GamepadButton_DpadRight);
    return buttons;
}

void CloseSlot(std::size_t slotIndex, const char* reason)
{
    SdlGamepadSlot& slot = g_slots[slotIndex];
    if (slot.gamepad == nullptr)
        return;

    const char* name = SDL_GetGamepadName(slot.gamepad);
    char text[384] = {};
    sprintf_s(
        text,
        "[Input][GamepadBackend] SDL slot %zu closed: %s (%s).\n",
        slotIndex,
        name ? name : "unnamed gamepad",
        reason ? reason : "refresh");
    AppendLog(text);

    SDL_CloseGamepad(slot.gamepad);
    slot = {};
}

bool DeviceListContains(
    const SDL_JoystickID* ids,
    int count,
    SDL_JoystickID id)
{
    for (int i = 0; i < count; ++i)
    {
        if (ids[i] == id)
            return true;
    }
    return false;
}

bool SlotOwnsId(SDL_JoystickID id)
{
    for (const SdlGamepadSlot& slot : g_slots)
    {
        if (slot.gamepad != nullptr && slot.id == id)
            return true;
    }
    return false;
}

void RefreshSdlSlots(bool force)
{
    const std::uint64_t now = SDL_GetTicks();
    if (!force && now < g_nextEnumerationTick)
        return;

    g_nextEnumerationTick = now + kEnumerationIntervalMs;

    int count = 0;
    SDL_ClearError();
    SDL_JoystickID* ids = SDL_GetGamepads(&count);
    if (ids == nullptr)
    {
        // Zero connected pads is a normal runtime state. SDL can also return
        // nullptr on an error, so only log when it supplies an error string.
        const char* error = SDL_GetError();
        if (error != nullptr && error[0] != '\0')
        {
            char text[384] = {};
            sprintf_s(
                text,
                "[Input][GamepadBackend] SDL enumeration warning: %s\n",
                error);
            AppendLog(text);
            SDL_ClearError();
        }
        return;
    }

    for (std::size_t slotIndex = 0; slotIndex < g_slots.size(); ++slotIndex)
    {
        const SdlGamepadSlot& slot = g_slots[slotIndex];
        if (slot.gamepad != nullptr &&
            (!SDL_GamepadConnected(slot.gamepad) ||
             !DeviceListContains(ids, count, slot.id)))
        {
            CloseSlot(slotIndex, "disconnect");
        }
    }

    for (int deviceIndex = 0; deviceIndex < count; ++deviceIndex)
    {
        const SDL_JoystickID id = ids[deviceIndex];
        if (SlotOwnsId(id))
            continue;

        auto empty = std::find_if(
            g_slots.begin(),
            g_slots.end(),
            [](const SdlGamepadSlot& slot) { return slot.gamepad == nullptr; });
        if (empty == g_slots.end())
            break;

        SDL_Gamepad* gamepad = SDL_OpenGamepad(id);
        if (gamepad == nullptr)
            continue;

        const std::size_t slotIndex = static_cast<std::size_t>(
            std::distance(g_slots.begin(), empty));
        empty->id = id;
        empty->gamepad = gamepad;
        empty->sequence = 1;
        empty->haveLastState = false;

        const char* name = SDL_GetGamepadName(gamepad);
        char text[384] = {};
        sprintf_s(
            text,
            "[Input][GamepadBackend] SDL slot %zu opened: %s (id=%u).\n",
            slotIndex,
            name ? name : "unnamed gamepad",
            static_cast<unsigned int>(id));
        AppendLog(text);
    }

    SDL_free(ids);
}

bool EnsureSdlInitializedLocked()
{
    if (g_sdlInitialized)
        return true;
    if (!g_sdlProviderRequested || g_sdlInitializationFailed)
        return false;

    // ZachFix startup hooks are installed from a worker thread, while SDL
    // requires subsystem initialization on the application's main thread.
    // Defer the actual SDL init until the first DP physical-input poll.
    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD))
    {
        LogSdlRuntimeVersionLocked();
        char text[512] = {};
        sprintf_s(
            text,
            "[Input][GamepadBackend] SDL3 main-thread initialization failed: %s\n",
            SDL_GetError());
        AppendLog(text);
        g_sdlInitializationFailed = true;
        return false;
    }

    LogSdlRuntimeVersionLocked();
    SDL_SetGamepadEventsEnabled(false);
    SDL_SetJoystickEventsEnabled(false);
    SDL_UpdateJoysticks();
    SDL_UpdateGamepads();
    g_sdlInitialized = true;
    RefreshSdlSlots(true);
    AppendLog(
        "[Input][GamepadBackend] SDL3 provider initialized on DP input thread (manual polling).\n");
    return true;
}

GamepadState ReadSdlState(SdlGamepadSlot& slot)
{
    GamepadState state{};
    state.leftX = SDL_GetGamepadAxis(slot.gamepad, SDL_GAMEPAD_AXIS_LEFTX);
    state.leftY = InvertSdlYAxis(
        SDL_GetGamepadAxis(slot.gamepad, SDL_GAMEPAD_AXIS_LEFTY));
    state.rightX = SDL_GetGamepadAxis(slot.gamepad, SDL_GAMEPAD_AXIS_RIGHTX);
    state.rightY = InvertSdlYAxis(
        SDL_GetGamepadAxis(slot.gamepad, SDL_GAMEPAD_AXIS_RIGHTY));
    state.leftTrigger = SdlTriggerToByte(
        SDL_GetGamepadAxis(slot.gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER));
    state.rightTrigger = SdlTriggerToByte(
        SDL_GetGamepadAxis(slot.gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER));
    state.buttons = ReadSdlButtons(slot.gamepad);

    if (!slot.haveLastState)
    {
        slot.haveLastState = true;
        slot.lastState = state;
    }
    else if (!SamePhysicalState(state, slot.lastState))
    {
        ++slot.sequence;
        if (slot.sequence == 0)
            slot.sequence = 1;
        slot.lastState = state;
    }

    state.stateSequence = slot.sequence;
    return state;
}
} // namespace

bool InitializeSdlGamepadProvider()
{
    std::lock_guard<std::mutex> lock(g_sdlMutex);
    if (g_sdlProviderRequested && !g_sdlInitializationFailed)
        return true;

    g_sdlProviderRequested = true;
    g_sdlInitializationFailed = false;
    ConfigureSdlDynamicApiOverrideLocked();
    AppendLog(
        "[Input][GamepadBackend] SDL3 provider selected; subsystem init deferred to first DP input poll.\n");
    return true;
}

bool SdlGamepadProviderInitializationFailed()
{
    std::lock_guard<std::mutex> lock(g_sdlMutex);
    return g_sdlInitializationFailed;
}

bool PollSdlGamepadState(std::uint32_t index, GamepadState& state)
{
    state = {};
    if (index >= kMaxGamepads)
        return false;

    std::lock_guard<std::mutex> lock(g_sdlMutex);
    if (!EnsureSdlInitializedLocked())
        return false;

    SDL_UpdateJoysticks();
    SDL_UpdateGamepads();
    RefreshSdlSlots(false);

    SdlGamepadSlot& slot = g_slots[index];
    if (slot.gamepad == nullptr)
        return false;

    if (!SDL_GamepadConnected(slot.gamepad))
    {
        CloseSlot(index, "disconnect during poll");
        g_nextEnumerationTick = 0;
        RefreshSdlSlots(true);
        if (g_slots[index].gamepad == nullptr)
            return false;
    }

    state = ReadSdlState(g_slots[index]);
    return true;
}

bool SdlGamepadProviderSupportsVibration()
{
    std::lock_guard<std::mutex> lock(g_sdlMutex);
    return g_sdlProviderRequested && !g_sdlInitializationFailed;
}

bool SetSdlGamepadVibration(
    std::uint32_t index,
    std::uint16_t leftMotor,
    std::uint16_t rightMotor)
{
    if (index >= kMaxGamepads)
        return false;

    std::lock_guard<std::mutex> lock(g_sdlMutex);
    if (!EnsureSdlInitializedLocked())
        return false;

    SDL_UpdateJoysticks();
    SDL_UpdateGamepads();
    RefreshSdlSlots(false);

    SdlGamepadSlot& slot = g_slots[index];
    if (slot.gamepad == nullptr || !SDL_GamepadConnected(slot.gamepad))
        return false;

    // XInput motor state is persistent until changed. Use SDL's longest
    // practical duration to preserve that contract; every subsequent native
    // DP rumble update replaces it, and an all-zero call stops immediately.
    const Uint32 duration = (leftMotor == 0 && rightMotor == 0)
        ? 0u
        : std::numeric_limits<Uint32>::max();
    const bool result =
        SDL_RumbleGamepad(slot.gamepad, leftMotor, rightMotor, duration);
    SDL_UpdateJoysticks();
    return result;
}
