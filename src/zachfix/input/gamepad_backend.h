#pragma once

#include <cstdint>

// Physical provider used by the ZachFix native gamepad path. Auto is the
// production default: prefer SDL3, then fall back to XInput if SDL cannot
// initialize on the game input thread.
enum class GamepadBackendType : std::uint32_t
{
    XInput = 0,
    SDL = 1,
    Auto = 2,
};

// Backend-neutral physical gamepad state. Values deliberately retain the
// precision and axis conventions of the original Xbox/XInput device contract:
// X right = positive, Y up = positive, triggers = 0..255.
enum GamepadButton : std::uint32_t
{
    GamepadButton_A             = 1u << 0,
    GamepadButton_B             = 1u << 1,
    GamepadButton_X             = 1u << 2,
    GamepadButton_Y             = 1u << 3,
    GamepadButton_LeftShoulder  = 1u << 4,
    GamepadButton_RightShoulder = 1u << 5,
    GamepadButton_Back          = 1u << 6,
    GamepadButton_Start         = 1u << 7,
    GamepadButton_LeftThumb     = 1u << 8,
    GamepadButton_RightThumb    = 1u << 9,
    GamepadButton_DpadUp        = 1u << 10,
    GamepadButton_DpadDown      = 1u << 11,
    GamepadButton_DpadLeft      = 1u << 12,
    GamepadButton_DpadRight     = 1u << 13,
};

struct GamepadState
{
    std::int16_t leftX = 0;
    std::int16_t leftY = 0;
    std::int16_t rightX = 0;
    std::int16_t rightY = 0;
    std::uint8_t leftTrigger = 0;
    std::uint8_t rightTrigger = 0;
    std::uint32_t buttons = 0;

    // Changes when the provider reports a new canonical physical state.
    // XInput forwards dwPacketNumber; SDL synthesizes an equivalent sequence
    // by comparing canonical state samples per opened gamepad slot.
    std::uint32_t stateSequence = 0;
};

constexpr std::uint32_t kMaxGamepads = 4;

bool InitializeGamepadBackend(GamepadBackendType requestedBackend);
bool IsGamepadBackendAvailable();
GamepadBackendType GetActiveGamepadBackend();
const char* GamepadBackendName(GamepadBackendType backend);
const char* GetActiveGamepadBackendName();

bool PollGamepadState(std::uint32_t index, GamepadState& state);

// Backend-neutral vibration output. Motor values retain DP/XInput's full
// 16-bit range.
bool GamepadBackendSupportsVibration();
bool SetGamepadVibration(
    std::uint32_t index,
    std::uint16_t leftMotor,
    std::uint16_t rightMotor);
