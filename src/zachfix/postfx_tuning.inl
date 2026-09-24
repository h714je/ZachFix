// -----------------------------------------------------------------------------
// ZachFix PostFX tuning storage
// -----------------------------------------------------------------------------

namespace
{
struct PostFxTuningStorage
{
    std::atomic_uint aoMode{ static_cast<UINT>(PostFxAoSettings{}.mode) };
    std::atomic<float> aoRadius{ PostFxAoSettings{}.radius };
    std::atomic<float> aoStrength{ PostFxAoSettings{}.strength };
    std::atomic<float> aoBias{ PostFxAoSettings{}.bias };
    std::atomic<float> aoThickness{ PostFxAoSettings{}.thickness };
    std::atomic<float> aoPower{ PostFxAoSettings{}.power };
    std::atomic_uint aoResolutionDivisor{ PostFxAoSettings{}.resolutionDivisor };

    std::atomic_uint bloomMode{ static_cast<UINT>(PostFxBloomSettings{}.mode) };
    std::atomic<float> bloomThresholdEv{ PostFxBloomSettings{}.thresholdEv };
    std::atomic<float> bloomSoftKnee{ PostFxBloomSettings{}.softKnee };
    std::atomic<float> bloomIntensity{ PostFxBloomSettings{}.intensity };
    std::atomic<float> bloomScatter{ PostFxBloomSettings{}.scatter };
    std::atomic_uint bloomMaxLevels{ PostFxBloomSettings{}.maxLevels };

    std::atomic_uint dofMode{ static_cast<UINT>(PostFxDofSettings{}.mode) };
    std::atomic<float> dofMaxRadiusPixels{ PostFxDofSettings{}.maxRadiusPixels };
    std::atomic<float> dofNearStrength{ PostFxDofSettings{}.nearStrength };
    std::atomic<float> dofFarStrength{ PostFxDofSettings{}.farStrength };
    std::atomic<float> dofDepthReject{ PostFxDofSettings{}.depthReject };
    std::atomic<float> dofHighlightBoost{ PostFxDofSettings{}.highlightBoost };
    std::atomic_uint dofResolutionDivisor{ PostFxDofSettings{}.resolutionDivisor };

    std::atomic_uint exposureMode{ static_cast<UINT>(PostFxExposureSettings{}.mode) };
    std::atomic<float> exposureCompensationEv{ PostFxExposureSettings{}.compensationEv };
    std::atomic<float> exposureMeterMinEv{ PostFxExposureSettings{}.meterMinEv };
    std::atomic<float> exposureMeterMaxEv{ PostFxExposureSettings{}.meterMaxEv };
    std::atomic<float> exposureMinEv{ PostFxExposureSettings{}.minExposureEv };
    std::atomic<float> exposureMaxEv{ PostFxExposureSettings{}.maxExposureEv };
    std::atomic<float> exposureBrightenSpeed{ PostFxExposureSettings{}.brightenSpeed };
    std::atomic<float> exposureDarkenSpeed{ PostFxExposureSettings{}.darkenSpeed };
    std::atomic<float> exposureShoulderStrength{ PostFxExposureSettings{}.shoulderStrength };
    std::atomic<float> exposureWhitePoint{ PostFxExposureSettings{}.whitePoint };

    std::atomic_uint colorGradeMode{
        static_cast<UINT>(PostFxTuningSnapshot{}.colorGradeMode)
    };
    std::atomic_uint displayGammaMode{
        static_cast<UINT>(PostFxTuningSnapshot{}.displayGammaMode)
    };
};

PostFxTuningStorage g_postFxTuning;

UINT SanitizePostFxAoDivisor(UINT divisor)
{
    if (divisor <= 1)
        return 1;
    if (divisor <= 2)
        return 2;
    return 4;
}

bool IsCustomExposureMode(PostFxExposureMode mode)
{
    return mode == PostFxExposureMode::ExposureOnly ||
           mode == PostFxExposureMode::ExposureAndShoulder;
}

float ClampPostFxTuning(float value, float minimum, float maximum)
{
    if (!std::isfinite(value))
        return minimum;
    return std::max(minimum, std::min(maximum, value));
}

PostFxAoSettings LoadPostFxAoSettings()
{
    PostFxAoSettings settings{};
    settings.mode = static_cast<PostFxAoMode>(
        g_postFxTuning.aoMode.load(std::memory_order_relaxed));
    settings.radius = g_postFxTuning.aoRadius.load(std::memory_order_relaxed);
    settings.strength = g_postFxTuning.aoStrength.load(std::memory_order_relaxed);
    settings.bias = g_postFxTuning.aoBias.load(std::memory_order_relaxed);
    settings.thickness = g_postFxTuning.aoThickness.load(std::memory_order_relaxed);
    settings.power = g_postFxTuning.aoPower.load(std::memory_order_relaxed);
    settings.resolutionDivisor =
        g_postFxTuning.aoResolutionDivisor.load(std::memory_order_relaxed);
    return settings;
}

PostFxBloomSettings LoadPostFxBloomSettings()
{
    PostFxBloomSettings settings{};
    settings.mode = static_cast<PostFxBloomMode>(
        g_postFxTuning.bloomMode.load(std::memory_order_relaxed));
    settings.thresholdEv =
        g_postFxTuning.bloomThresholdEv.load(std::memory_order_relaxed);
    settings.softKnee =
        g_postFxTuning.bloomSoftKnee.load(std::memory_order_relaxed);
    settings.intensity =
        g_postFxTuning.bloomIntensity.load(std::memory_order_relaxed);
    settings.scatter =
        g_postFxTuning.bloomScatter.load(std::memory_order_relaxed);
    settings.maxLevels =
        g_postFxTuning.bloomMaxLevels.load(std::memory_order_relaxed);
    return settings;
}

PostFxDofSettings LoadPostFxDofSettings()
{
    PostFxDofSettings settings{};
    settings.mode = static_cast<PostFxDofMode>(
        g_postFxTuning.dofMode.load(std::memory_order_relaxed));
    settings.maxRadiusPixels =
        g_postFxTuning.dofMaxRadiusPixels.load(std::memory_order_relaxed);
    settings.nearStrength =
        g_postFxTuning.dofNearStrength.load(std::memory_order_relaxed);
    settings.farStrength =
        g_postFxTuning.dofFarStrength.load(std::memory_order_relaxed);
    settings.depthReject =
        g_postFxTuning.dofDepthReject.load(std::memory_order_relaxed);
    settings.highlightBoost =
        g_postFxTuning.dofHighlightBoost.load(std::memory_order_relaxed);
    settings.resolutionDivisor =
        g_postFxTuning.dofResolutionDivisor.load(std::memory_order_relaxed);
    return settings;
}

PostFxExposureSettings LoadPostFxExposureSettings()
{
    PostFxExposureSettings settings{};
    settings.mode = static_cast<PostFxExposureMode>(
        g_postFxTuning.exposureMode.load(std::memory_order_relaxed));
    settings.compensationEv =
        g_postFxTuning.exposureCompensationEv.load(std::memory_order_relaxed);
    settings.meterMinEv =
        g_postFxTuning.exposureMeterMinEv.load(std::memory_order_relaxed);
    settings.meterMaxEv =
        g_postFxTuning.exposureMeterMaxEv.load(std::memory_order_relaxed);
    settings.minExposureEv =
        g_postFxTuning.exposureMinEv.load(std::memory_order_relaxed);
    settings.maxExposureEv =
        g_postFxTuning.exposureMaxEv.load(std::memory_order_relaxed);
    settings.brightenSpeed =
        g_postFxTuning.exposureBrightenSpeed.load(std::memory_order_relaxed);
    settings.darkenSpeed =
        g_postFxTuning.exposureDarkenSpeed.load(std::memory_order_relaxed);
    settings.shoulderStrength =
        g_postFxTuning.exposureShoulderStrength.load(std::memory_order_relaxed);
    settings.whitePoint =
        g_postFxTuning.exposureWhitePoint.load(std::memory_order_relaxed);
    return settings;
}
} // namespace

PostFxTuningSnapshot GetPostFxTuningSnapshot()
{
    PostFxTuningSnapshot tuning{};
    tuning.ao = LoadPostFxAoSettings();
    tuning.bloom = LoadPostFxBloomSettings();
    tuning.dof = LoadPostFxDofSettings();
    tuning.exposure = LoadPostFxExposureSettings();
    tuning.colorGradeMode = static_cast<PostFxColorGradeMode>(
        g_postFxTuning.colorGradeMode.load(std::memory_order_relaxed));
    tuning.displayGammaMode = static_cast<PostFxDisplayGammaMode>(
        g_postFxTuning.displayGammaMode.load(std::memory_order_relaxed));
    return tuning;
}

PostFxAoSettings GetPostFxAoSettings()
{
    return LoadPostFxAoSettings();
}

bool IsPostFxAoEnabled()
{
    return static_cast<PostFxAoMode>(
        g_postFxTuning.aoMode.load(std::memory_order_relaxed)) !=
        PostFxAoMode::Off;
}

bool IsPostFxGBufferCaptureRequired()
{
    const PostFxAoMode aoMode = static_cast<PostFxAoMode>(
        g_postFxTuning.aoMode.load(std::memory_order_relaxed));
    const PostFxDofMode dofMode = static_cast<PostFxDofMode>(
        g_postFxTuning.dofMode.load(std::memory_order_relaxed));
    if (aoMode != PostFxAoMode::Off || dofMode != PostFxDofMode::Legacy)
        return true;

    const PostFxExposureMode exposureMode = static_cast<PostFxExposureMode>(
        g_postFxTuning.exposureMode.load(std::memory_order_relaxed));
    const PostFxColorGradeMode colorGradeMode = static_cast<PostFxColorGradeMode>(
        g_postFxTuning.colorGradeMode.load(std::memory_order_relaxed));
    const PostFxBloomMode bloomMode = static_cast<PostFxBloomMode>(
        g_postFxTuning.bloomMode.load(std::memory_order_relaxed));

    return exposureMode != PostFxExposureMode::Legacy ||
           colorGradeMode != PostFxColorGradeMode::PcDirectorsCut ||
           bloomMode != PostFxBloomMode::Legacy;
}

bool IsPostFxExposureReplacementRequiredByTuning()
{
    const PostFxExposureMode exposureMode = static_cast<PostFxExposureMode>(
        g_postFxTuning.exposureMode.load(std::memory_order_relaxed));
    const PostFxColorGradeMode colorGradeMode = static_cast<PostFxColorGradeMode>(
        g_postFxTuning.colorGradeMode.load(std::memory_order_relaxed));
    const PostFxAoMode aoMode = static_cast<PostFxAoMode>(
        g_postFxTuning.aoMode.load(std::memory_order_relaxed));
    const PostFxBloomMode bloomMode = static_cast<PostFxBloomMode>(
        g_postFxTuning.bloomMode.load(std::memory_order_relaxed));
    const PostFxDofMode dofMode = static_cast<PostFxDofMode>(
        g_postFxTuning.dofMode.load(std::memory_order_relaxed));

    return exposureMode != PostFxExposureMode::Legacy ||
           colorGradeMode != PostFxColorGradeMode::PcDirectorsCut ||
           aoMode == PostFxAoMode::Composite ||
           bloomMode != PostFxBloomMode::Legacy ||
           dofMode != PostFxDofMode::Legacy;
}

void SetPostFxAoMode(PostFxAoMode mode)
{
    g_postFxTuning.aoMode.store(
        std::min<UINT>(static_cast<UINT>(mode), static_cast<UINT>(PostFxAoMode::Composite)),
        std::memory_order_relaxed);
}

void SetPostFxAoRadius(float radius)
{
    g_postFxTuning.aoRadius.store(std::clamp(radius, 0.10f, 32.0f), std::memory_order_relaxed);
}

void SetPostFxAoStrength(float strength)
{
    g_postFxTuning.aoStrength.store(std::clamp(strength, 0.0f, 4.0f), std::memory_order_relaxed);
}

void SetPostFxAoBias(float bias)
{
    g_postFxTuning.aoBias.store(std::clamp(bias, 0.0f, 0.45f), std::memory_order_relaxed);
}

void SetPostFxAoThickness(float thickness)
{
    g_postFxTuning.aoThickness.store(std::clamp(thickness, 0.05f, 1.50f), std::memory_order_relaxed);
}

void SetPostFxAoPower(float power)
{
    g_postFxTuning.aoPower.store(std::clamp(power, 0.25f, 4.0f), std::memory_order_relaxed);
}

void SetPostFxAoResolutionDivisor(UINT divisor)
{
    g_postFxTuning.aoResolutionDivisor.store(
        SanitizePostFxAoDivisor(divisor), std::memory_order_relaxed);
}

void ResetPostFxAoSettings()
{
    const PostFxAoSettings defaults{};
    SetPostFxAoMode(defaults.mode);
    SetPostFxAoRadius(defaults.radius);
    SetPostFxAoStrength(defaults.strength);
    SetPostFxAoBias(defaults.bias);
    SetPostFxAoThickness(defaults.thickness);
    SetPostFxAoPower(defaults.power);
    SetPostFxAoResolutionDivisor(defaults.resolutionDivisor);
}

PostFxBloomSettings GetPostFxBloomSettings()
{
    return LoadPostFxBloomSettings();
}

void SetPostFxBloomMode(PostFxBloomMode mode)
{
    g_postFxTuning.bloomMode.store(
        std::min<UINT>(static_cast<UINT>(mode), static_cast<UINT>(PostFxBloomMode::ShowBloom)),
        std::memory_order_relaxed);
}

void SetPostFxBloomThresholdEv(float ev)
{
    g_postFxTuning.bloomThresholdEv.store(ClampPostFxTuning(ev, -6.0f, 12.0f), std::memory_order_relaxed);
}

void SetPostFxBloomSoftKnee(float softKnee)
{
    g_postFxTuning.bloomSoftKnee.store(ClampPostFxTuning(softKnee, 0.01f, 1.0f), std::memory_order_relaxed);
}

void SetPostFxBloomIntensity(float intensity)
{
    g_postFxTuning.bloomIntensity.store(ClampPostFxTuning(intensity, 0.0f, 4.0f), std::memory_order_relaxed);
}

void SetPostFxBloomScatter(float scatter)
{
    g_postFxTuning.bloomScatter.store(ClampPostFxTuning(scatter, 0.0f, 1.0f), std::memory_order_relaxed);
}

void SetPostFxBloomMaxLevels(UINT levels)
{
    g_postFxTuning.bloomMaxLevels.store(
        std::max<UINT>(2, std::min<UINT>(6, levels)), std::memory_order_relaxed);
}

void ResetPostFxBloomSettings()
{
    const PostFxBloomSettings defaults{};
    SetPostFxBloomMode(defaults.mode);
    SetPostFxBloomThresholdEv(defaults.thresholdEv);
    SetPostFxBloomSoftKnee(defaults.softKnee);
    SetPostFxBloomIntensity(defaults.intensity);
    SetPostFxBloomScatter(defaults.scatter);
    SetPostFxBloomMaxLevels(defaults.maxLevels);
}

PostFxDofSettings GetPostFxDofSettings()
{
    return LoadPostFxDofSettings();
}

void SetPostFxDofMode(PostFxDofMode mode)
{
    g_postFxTuning.dofMode.store(
        std::min<UINT>(static_cast<UINT>(mode), static_cast<UINT>(PostFxDofMode::ShowFar)),
        std::memory_order_relaxed);
}

void SetPostFxDofMaxRadiusPixels(float radiusPixels)
{
    g_postFxTuning.dofMaxRadiusPixels.store(ClampPostFxTuning(radiusPixels, 1.0f, 32.0f), std::memory_order_relaxed);
}

void SetPostFxDofNearStrength(float strength)
{
    g_postFxTuning.dofNearStrength.store(ClampPostFxTuning(strength, 0.0f, 2.0f), std::memory_order_relaxed);
}

void SetPostFxDofFarStrength(float strength)
{
    g_postFxTuning.dofFarStrength.store(ClampPostFxTuning(strength, 0.0f, 2.0f), std::memory_order_relaxed);
}

void SetPostFxDofDepthReject(float depthReject)
{
    g_postFxTuning.dofDepthReject.store(ClampPostFxTuning(depthReject, 0.05f, 8.0f), std::memory_order_relaxed);
}

void SetPostFxDofHighlightBoost(float highlightBoost)
{
    g_postFxTuning.dofHighlightBoost.store(ClampPostFxTuning(highlightBoost, 0.0f, 2.0f), std::memory_order_relaxed);
}

void SetPostFxDofResolutionDivisor(UINT divisor)
{
    const UINT sanitized = divisor <= 2 ? 2u : 4u;
    if (g_postFxTuning.dofResolutionDivisor.exchange(sanitized, std::memory_order_relaxed) != sanitized)
    {
        ReleasePostFxTarget(PostFxTargetSlot::DoFNear);
        ReleasePostFxTarget(PostFxTargetSlot::DoFFar);
    }
}

void ResetPostFxDofSettings()
{
    const PostFxDofSettings defaults{};
    SetPostFxDofMode(defaults.mode);
    SetPostFxDofMaxRadiusPixels(defaults.maxRadiusPixels);
    SetPostFxDofNearStrength(defaults.nearStrength);
    SetPostFxDofFarStrength(defaults.farStrength);
    SetPostFxDofDepthReject(defaults.depthReject);
    SetPostFxDofHighlightBoost(defaults.highlightBoost);
    SetPostFxDofResolutionDivisor(defaults.resolutionDivisor);
}

PostFxExposureSettings GetPostFxExposureSettings()
{
    return LoadPostFxExposureSettings();
}

PostFxColorGradeMode GetPostFxColorGradeMode()
{
    return static_cast<PostFxColorGradeMode>(
        g_postFxTuning.colorGradeMode.load(std::memory_order_relaxed));
}

void SetPostFxColorGradeMode(PostFxColorGradeMode mode)
{
    g_postFxTuning.colorGradeMode.store(
        std::min<UINT>(static_cast<UINT>(mode), static_cast<UINT>(PostFxColorGradeMode::Xbox360Full)),
        std::memory_order_relaxed);
}

PostFxDisplayGammaMode GetPostFxDisplayGammaMode()
{
    return static_cast<PostFxDisplayGammaMode>(
        g_postFxTuning.displayGammaMode.load(std::memory_order_relaxed));
}

void SetPostFxDisplayGammaMode(PostFxDisplayGammaMode mode)
{
    g_postFxTuning.displayGammaMode.store(
        std::min<UINT>(static_cast<UINT>(mode), static_cast<UINT>(PostFxDisplayGammaMode::Xbox360HdtvBt709)),
        std::memory_order_relaxed);
}

void ResetPostFxColorGradeSettings()
{
    const PostFxTuningSnapshot defaults{};
    SetPostFxColorGradeMode(defaults.colorGradeMode);
    SetPostFxDisplayGammaMode(defaults.displayGammaMode);
}

void SetPostFxExposureMode(PostFxExposureMode mode)
{
    const UINT sanitized = std::min<UINT>(
        static_cast<UINT>(mode),
        static_cast<UINT>(PostFxExposureMode::ExposureAndShoulder));
    const UINT previous = g_postFxTuning.exposureMode.exchange(
        sanitized, std::memory_order_relaxed);

    if (!IsCustomExposureMode(static_cast<PostFxExposureMode>(previous)) &&
        IsCustomExposureMode(static_cast<PostFxExposureMode>(sanitized)))
    {
        RequestPostFxExposureAdaptationReset();
    }
}

void SetPostFxExposureCompensationEv(float ev)
{
    g_postFxTuning.exposureCompensationEv.store(ClampPostFxTuning(ev, -6.0f, 6.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMeterMinEv(float ev)
{
    g_postFxTuning.exposureMeterMinEv.store(ClampPostFxTuning(ev, -16.0f, 8.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMeterMaxEv(float ev)
{
    g_postFxTuning.exposureMeterMaxEv.store(ClampPostFxTuning(ev, -8.0f, 16.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMinEv(float ev)
{
    g_postFxTuning.exposureMinEv.store(ClampPostFxTuning(ev, -12.0f, 4.0f), std::memory_order_relaxed);
}

void SetPostFxExposureMaxEv(float ev)
{
    g_postFxTuning.exposureMaxEv.store(ClampPostFxTuning(ev, -4.0f, 12.0f), std::memory_order_relaxed);
}

void SetPostFxExposureBrightenSpeed(float speed)
{
    g_postFxTuning.exposureBrightenSpeed.store(ClampPostFxTuning(speed, 0.05f, 10.0f), std::memory_order_relaxed);
}

void SetPostFxExposureDarkenSpeed(float speed)
{
    g_postFxTuning.exposureDarkenSpeed.store(ClampPostFxTuning(speed, 0.05f, 10.0f), std::memory_order_relaxed);
}

void SetPostFxExposureShoulderStrength(float strength)
{
    g_postFxTuning.exposureShoulderStrength.store(ClampPostFxTuning(strength, 0.0f, 1.0f), std::memory_order_relaxed);
}

void SetPostFxExposureWhitePoint(float whitePoint)
{
    g_postFxTuning.exposureWhitePoint.store(ClampPostFxTuning(whitePoint, 1.05f, 16.0f), std::memory_order_relaxed);
}

void ResetPostFxExposureSettings()
{
    const PostFxExposureSettings defaults{};
    SetPostFxExposureMode(defaults.mode);
    SetPostFxExposureCompensationEv(defaults.compensationEv);
    SetPostFxExposureMeterMinEv(defaults.meterMinEv);
    SetPostFxExposureMeterMaxEv(defaults.meterMaxEv);
    SetPostFxExposureMinEv(defaults.minExposureEv);
    SetPostFxExposureMaxEv(defaults.maxExposureEv);
    SetPostFxExposureBrightenSpeed(defaults.brightenSpeed);
    SetPostFxExposureDarkenSpeed(defaults.darkenSpeed);
    SetPostFxExposureShoulderStrength(defaults.shoulderStrength);
    SetPostFxExposureWhitePoint(defaults.whitePoint);
    RequestPostFxExposureAdaptationReset();
}
