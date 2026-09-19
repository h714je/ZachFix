#pragma once

#include <Windows.h>

enum class TextureDimensionMode : UINT
{
    DPFix = 0,
    Preserve = 1
};

enum class TextureFilteringMode : UINT
{
    Original = 0,
    Bilinear = 1,
    Anisotropic = 2
};

enum class GamepadInputProfile : UINT
{
    PC = 0,
    Xbox360 = 1
};

// Shared render/config safety caps. These are used both by config validation
// and by render-target scaling helpers in the main D3D9 translation unit.
inline constexpr UINT kMaxResolutionWidth = 16384;
inline constexpr UINT kMaxResolutionHeight = 16384;

struct ZachFixConfig
{
    UINT displayWidth = 0;
    UINT displayHeight = 0;
    bool borderless = true;

    UINT internalWidth = 0;
    UINT internalHeight = 0;
    float internalScale = 1.0f;

    UINT shadowScale = 1;
    bool improveShadowPrecision = false;
    UINT reflectionScale = 1;
    bool improveDofResolution = false;
    UINT additionalDofBlur = 0;
    bool fixPixelOffset = true;

    // 1 = original inner 2x2 full-detail cells.
    // 2 = promote the existing outer 4x4 ring to full detail.
    UINT highDetailDistanceScale = 1;

    // Native per-object active-list distance gate.
    // 1 = original 1000-unit radius, 2 = extended 2000-unit radius.
    UINT objectActivationDistanceScale = 1;

    // Native PC per-object LOD metric multiplier. ZachFix leaves DP's LOD
    // selector, resource flags and mesh lists intact and only makes the
    // existing camera-distance metric appear closer by this factor.
    // 1 = original, 2/3/4 = progressively farther native LOD transitions.
    UINT objectLodDistanceScale = 1;

    // Director's Cut regression: one outer-world interior visibility-volume
    // call can incorrectly reject visible objects near mirrors/walls. The fix
    // bypasses only that confirmed callsite; normal frustum culling remains native.
    bool fixInteriorOcclusionBugs = true;

    // Research-only: normalize the legacy discrete tire-parameter blend when
    // Director's Cut runs player-car physics above the selected reference rate.
    // 30 = original Xbox-style cadence, 60 = PC-port research target. Startup-only.
    bool fixVehicleTireTiming = true;
    UINT vehicleTireTimingReferenceHz = 60;

    // Research-only causal A/B for the confirmed PhysX rigid-body timebase bug.
    // false = native scene-0 queue/maxIter; true = QPC wall-clock scene time
    // with the existing fixed 1/60 accumulator allowed up to four substeps.
    // Startup-only: restart after changing.
    bool physXRealTimeAB = false;

    // Static-RE-derived scheduling repair. The Xbox 360 original gates the
    // CObjectCar phase-5 vehicle dispatcher at a discrete 30 Hz game cadence;
    // Director's Cut invokes the homologous path at render cadence.
    bool vehicleXboxTickCadence = false;

    // Retained only for backward-compatible research INI parsing. Static Xbox
    // comparison confirmed that delta-scaled motor/brake is original behavior,
    // so v2.2 never mutates the setters even if this old switch is true.
    bool vehicleMotorBrakeNormalizeAB = false;

    bool enableTextureOverride = true;
    bool textureDeveloperMode = false;
    bool dumpTextures = false;
    TextureDimensionMode textureDimensionMode = TextureDimensionMode::DPFix;

    TextureFilteringMode textureFilteringMode = TextureFilteringMode::Original;
    UINT maxAnisotropy = 16;

    bool uiEnabled = true;
    UINT uiToggleKey = VK_F10;
    bool pauseGameWhileUiOpen = false;

    // Optional native XInput source. DP still consumes its familiar JOYINFOEX
    // shape, synthesized by ZachFix from XInputGetState.
    bool nativeXInputEnabled = false;

    // Runtime-selectable stick/aim behavior for the native XInput bridge.
    // PC keeps Director's Cut's original evaluator/filtering. Xbox360 keeps
    // the Director's Cut routing, but restores the proven Xbox 360 stick
    // normalization and live-aim shaping.
    GamepadInputProfile gamepadInputProfile = GamepadInputProfile::Xbox360;

    // Restores the three proven Xbox 360 analog LT/RT vehicle consumers.
    // This is intentionally independent from the stick/aim input profile.
    bool analogVehicleTriggers = true;

    // Deadzone for the restored Xbox 360 analog vehicle trigger path, in raw
    // XInput trigger counts. The original Xbox build uses 30: raw <= threshold
    // becomes zero, while raw > threshold remains raw/255 without renormalizing.
    UINT vehicleTriggerDeadzone = 30;

    // Restores DP's surviving native two-channel vibration path through XInput.
    // These are hot-applicable while NativeXInput is active.
    bool vibrationEnabled = true;
    float vibrationStrength = 1.0f;

    // Runtime switch around DP's vanilla USEJOY mode flag.
    bool autoInputModeSwitch = false;

    // Transactional protection for DP's destructive single-file save path.
    // DP writes to a temp file first; the previous live save is backed up only
    // after the temp file passes the conservative validator and before commit.
    bool saveSafetyEnabled = true;
    UINT saveSafetyBackupCount = 10;

    // Dynamically substitutes the glyph atlas selected by the current USEJOY
    // mode. Theme names resolve to files under ZachFix\glyphs\keyboard and
    // ZachFix\glyphs\gamepad; Native uses the atlas captured from DP.
    bool dynamicGlyphAtlas = false;
    bool glyphHotReload = true;
    wchar_t keyboardGlyphSet[64] = L"Native";
    wchar_t gamepadGlyphSet[64] = L"xbox";
};

extern ZachFixConfig g_config;

extern UINT g_displayWidth;
extern UINT g_displayHeight;
extern UINT g_internalWidth;
extern UINT g_internalHeight;

bool LoadConfigFromIni(const wchar_t* path, ZachFixConfig& result);
void LoadConfig();
bool ResolveConfigForWindow(HWND window);
bool GetConfigFilePath(wchar_t* path, size_t pathCount);
bool SaveEditableConfig(const ZachFixConfig& config);
bool ReloadPostFxConfigFromIni();
