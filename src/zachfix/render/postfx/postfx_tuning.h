#pragma once

#include <Windows.h>

// -----------------------------------------------------------------------------
// ZachFix PostFX tuning: the single source of truth for user-facing PostFX
// modes, values, and defaults.
//
// Effect modules own render resources and runtime telemetry only. Persistent
// tuning state lives in postfx_tuning.inl and is exposed through the existing
// Get/Set APIs so config/UI/render code all see the same values.
// -----------------------------------------------------------------------------

enum class PostFxAoMode : UINT
{
    Off = 0,
    ShowRaw,
    ShowFiltered,
    ShowEnhanced,
    Composite
};

struct PostFxAoSettings
{
    PostFxAoMode mode = PostFxAoMode::Off;
    float radius = 4.0f;
    float strength = 1.0f;
    float bias = 0.04f;
    float thickness = 0.35f;
    float power = 1.0f;
    UINT resolutionDivisor = 2;
};

enum class PostFxBloomMode : UINT
{
    Legacy = 0,
    Bloom,
    ShowBloom
};

struct PostFxBloomSettings
{
    PostFxBloomMode mode = PostFxBloomMode::Legacy;
    float thresholdEv = 0.0f;
    float softKnee = 0.50f;
    float intensity = 0.35f;
    float scatter = 0.70f;
    UINT maxLevels = 5;
};

enum class PostFxDofMode : UINT
{
    Legacy = 0,
    DepthOfField,
    ShowCoC,
    ShowNear,
    ShowFar
};

struct PostFxDofSettings
{
    PostFxDofMode mode = PostFxDofMode::Legacy;
    float maxRadiusPixels = 12.0f;
    float nearStrength = 1.0f;
    float farStrength = 1.0f;
    float depthReject = 1.5f;
    float highlightBoost = 0.25f;
    UINT resolutionDivisor = 2;
};


enum class PostFxExposureMode : UINT
{
    Legacy = 0,
    ExposureOnly,
    ShoulderOnly,
    ExposureAndShoulder
};

enum class PostFxColorGradeMode : UINT
{
    PcDirectorsCut = 0,
    Xbox360Grading,
    Xbox360Full
};

enum class PostFxDisplayGammaMode : UINT
{
    PcSrgb = 0,
    Xbox360HdtvBt709
};

struct PostFxExposureSettings
{
    PostFxExposureMode mode = PostFxExposureMode::Legacy;
    float compensationEv = 0.0f;
    float meterMinEv = -10.0f;
    float meterMaxEv = 6.0f;
    float minExposureEv = -8.0f;
    float maxExposureEv = 4.0f;
    float brightenSpeed = 1.5f;
    float darkenSpeed = 3.0f;
    float shoulderStrength = 1.0f;
    float whitePoint = 4.0f;
};

struct PostFxTuningSnapshot
{
    PostFxAoSettings ao{};
    PostFxBloomSettings bloom{};
    PostFxDofSettings dof{};
    PostFxExposureSettings exposure{};
    PostFxColorGradeMode colorGradeMode = PostFxColorGradeMode::PcDirectorsCut;
    PostFxDisplayGammaMode displayGammaMode = PostFxDisplayGammaMode::PcSrgb;
};

// Coherent API-level snapshot for config/UI/diagnostics. Individual fields are
// still backed by relaxed atomics because render/UI paths hot-apply settings.
PostFxTuningSnapshot GetPostFxTuningSnapshot();

PostFxAoSettings GetPostFxAoSettings();
bool IsPostFxAoEnabled();
bool IsPostFxGBufferCaptureRequired();
bool IsPostFxExposureReplacementRequiredByTuning();
void SetPostFxAoMode(PostFxAoMode mode);
void SetPostFxAoRadius(float radius);
void SetPostFxAoStrength(float strength);
void SetPostFxAoBias(float bias);
void SetPostFxAoThickness(float thickness);
void SetPostFxAoPower(float power);
void SetPostFxAoResolutionDivisor(UINT divisor);
void ResetPostFxAoSettings();

PostFxBloomSettings GetPostFxBloomSettings();
void SetPostFxBloomMode(PostFxBloomMode mode);
void SetPostFxBloomThresholdEv(float ev);
void SetPostFxBloomSoftKnee(float softKnee);
void SetPostFxBloomIntensity(float intensity);
void SetPostFxBloomScatter(float scatter);
void SetPostFxBloomMaxLevels(UINT levels);
void ResetPostFxBloomSettings();

PostFxDofSettings GetPostFxDofSettings();
void SetPostFxDofMode(PostFxDofMode mode);
void SetPostFxDofMaxRadiusPixels(float radiusPixels);
void SetPostFxDofNearStrength(float strength);
void SetPostFxDofFarStrength(float strength);
void SetPostFxDofDepthReject(float depthReject);
void SetPostFxDofHighlightBoost(float highlightBoost);
void SetPostFxDofResolutionDivisor(UINT divisor);
void ResetPostFxDofSettings();

PostFxExposureSettings GetPostFxExposureSettings();
void SetPostFxExposureMode(PostFxExposureMode mode);
void SetPostFxExposureCompensationEv(float ev);
void SetPostFxExposureMeterMinEv(float ev);
void SetPostFxExposureMeterMaxEv(float ev);
void SetPostFxExposureMinEv(float ev);
void SetPostFxExposureMaxEv(float ev);
void SetPostFxExposureBrightenSpeed(float speed);
void SetPostFxExposureDarkenSpeed(float speed);
void SetPostFxExposureShoulderStrength(float strength);
void SetPostFxExposureWhitePoint(float whitePoint);
void ResetPostFxExposureSettings();

PostFxColorGradeMode GetPostFxColorGradeMode();
void SetPostFxColorGradeMode(PostFxColorGradeMode mode);
PostFxDisplayGammaMode GetPostFxDisplayGammaMode();
void SetPostFxDisplayGammaMode(PostFxDisplayGammaMode mode);
void ResetPostFxColorGradeSettings();
