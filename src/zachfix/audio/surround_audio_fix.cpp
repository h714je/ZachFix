#include "zachfix/audio/surround_audio_fix.h"

#include "zachfix/core/logging.h"
#include "zachfix/core/main_exe.h"

#include <Windows.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
static_assert(sizeof(void*) == 4, "Surround audio restoration requires the 32-bit ZachFix build.");

// Confirmed PC callsites inside FUN_0072CB00. The Director's Cut port collapsed
// both Xbox surround paths to SetMatrixCoefficients(cue, 2, 2, ...).
constexpr unsigned char kExpected3DMatrixCallBytes[] = {
    0x8D, 0x45, 0xE8,
    0x50,
    0x6A, 0x02,
    0x6A, 0x02,
    0x8B, 0x4D, 0xF8,
    0x8B, 0x51, 0x14,
    0x8B, 0x45, 0xF8,
    0x8B, 0x48, 0x14,
    0x8B, 0x12,
    0x51,
    0x8B, 0x42, 0x10,
    0xFF, 0xD0
};

constexpr unsigned char kExpectedNon3DMatrixCallBytes[] = {
    0x8D, 0x4D, 0xAC,
    0x51,
    0x6A, 0x02,
    0x6A, 0x02,
    0x8B, 0x55, 0xF8,
    0x8B, 0x42, 0x14,
    0x8B, 0x4D, 0xF8,
    0x8B, 0x51, 0x14,
    0x8B, 0x00,
    0x52,
    0x8B, 0x48, 0x10,
    0xFF, 0xD1
};

constexpr uintptr_t kX3dDspSettingsRva = 0x010B02F0;
constexpr std::uint32_t kStereoChannels = 2;
constexpr std::uint32_t kXbox3DSourceChannels = 1;
constexpr std::uint32_t kXboxInternalChannels = 6;
constexpr std::uint32_t kSupportedSurroundChannelsA = 6;
constexpr std::uint32_t kSupportedSurroundChannelsB = 8;

// 32-bit DirectX SDK layouts used only for one-call copies at the X3DAudio
// boundary. X3DAUDIO_LISTENER ends with pCone at +0x30.
constexpr std::size_t kX3dListenerSize = 0x34;
constexpr std::size_t kX3dListenerOrientFrontXOffset = 0x00;
constexpr std::size_t kX3dListenerPositionXOffset = 0x18;
constexpr std::size_t kX3dEmitterSize = 0x64;
constexpr std::size_t kX3dEmitterPositionXOffset = 0x1C;
constexpr std::size_t kX3dEmitterChannelCountOffset = 0x3C;
constexpr std::size_t kX3dEmitterChannelRadiusOffset = 0x40;
constexpr std::size_t kX3dEmitterChannelAzimuthsOffset = 0x44;

constexpr float kXboxPanScale = 0.5f;
constexpr float kXboxRearSend = 0.8f;
constexpr float kRadiansToDegrees = 57.295776f;

struct X3DAudioDspSettingsView
{
    float* matrixCoefficients;
    float* delayTimes;
    std::uint32_t sourceChannels;
    std::uint32_t destinationChannels;
    float lpfDirectCoefficient;
    float lpfReverbCoefficient;
    float reverbLevel;
    float dopplerFactor;
    float emitterToListenerAngle;
    float emitterToListenerDistance;
    float emitterVelocityComponent;
    float listenerVelocityComponent;
};
static_assert(sizeof(X3DAudioDspSettingsView) == 0x30,
              "Unexpected 32-bit X3DAUDIO_DSP_SETTINGS layout.");

using X3DAudioCalculateFn = void (__cdecl*)(
    const void* instance,
    const void* listener,
    const void* emitter,
    std::uint32_t flags,
    void* dspSettings);
using SetMatrixCoefficientsFn = HRESULT (STDMETHODCALLTYPE*)(
    void* cue,
    std::uint32_t sourceChannels,
    std::uint32_t destinationChannels,
    float* matrixCoefficients);
using GetVariableIndexFn = std::uint16_t (STDMETHODCALLTYPE*)(
    void* cue,
    const char* friendlyName);
using SetVariableFn = HRESULT (STDMETHODCALLTYPE*)(
    void* cue,
    std::uint16_t variableIndex,
    float value);

std::atomic_bool g_available{false};
std::atomic_bool g_active{false};
std::atomic_bool g_runtimeWarningLogged{false};
X3DAudioCalculateFn g_originalX3DAudioCalculate = nullptr;
X3DAudioDspSettingsView* g_x3dDspSettings = nullptr;

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

    auto* nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE)
        return nullptr;

    const auto& importDirectory =
        nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    if (importDirectory.VirtualAddress == 0 || importDirectory.Size == 0)
        return nullptr;

    auto* descriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(
        base + importDirectory.VirtualAddress);

    for (; descriptor->Name != 0; ++descriptor)
    {
        const char* dllName = reinterpret_cast<const char*>(
            base + descriptor->Name);
        if (!AsciiEqualsIgnoreCase(dllName, importedDll))
            continue;

        if (descriptor->OriginalFirstThunk == 0 || descriptor->FirstThunk == 0)
            return nullptr;

        auto* originalThunk = reinterpret_cast<IMAGE_THUNK_DATA*>(
            base + descriptor->OriginalFirstThunk);
        auto* firstThunk = reinterpret_cast<IMAGE_THUNK_DATA*>(
            base + descriptor->FirstThunk);

        for (; originalThunk->u1.AddressOfData != 0;
             ++originalThunk, ++firstThunk)
        {
            if (IMAGE_SNAP_BY_ORDINAL(originalThunk->u1.Ordinal))
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

bool IsRangeInsideMainExe(uintptr_t address, size_t size)
{
    if (address == 0 || size == 0 || g_mainExeBase == 0 || g_mainExeSize == 0)
        return false;

    const uintptr_t end = address + size;
    const uintptr_t imageEnd = g_mainExeBase + g_mainExeSize;
    return end >= address && address >= g_mainExeBase && end <= imageEnd;
}

bool IsSupportedSurroundCount(std::uint32_t channels)
{
    return channels == kSupportedSurroundChannelsA ||
           channels == kSupportedSurroundChannelsB;
}

void LogRuntimeWarningOnce(const char* reason)
{
    if (g_runtimeWarningLogged.exchange(true, std::memory_order_acq_rel))
        return;

    char text[320] = {};
    sprintf_s(
        text,
        "[Audio][SurroundFix] WARNING: %s; falling back to the PC update for that cue.\n",
        reason ? reason : "restored routing was rejected");
    AppendLog(text);
}

HRESULT CallSetMatrix(
    void* cue,
    std::uint32_t sourceChannels,
    std::uint32_t destinationChannels,
    float* matrix)
{
    if (!cue || !matrix)
        return E_POINTER;

    auto*** instance = reinterpret_cast<void***>(cue);
    if (!instance || !*instance || !(*instance)[4])
        return E_POINTER;

    const auto setMatrix =
        reinterpret_cast<SetMatrixCoefficientsFn>((*instance)[4]);
    return setMatrix(cue, sourceChannels, destinationChannels, matrix);
}

HRESULT CallVanilla2x2(void* cue, float* matrix)
{
    return CallSetMatrix(cue, 2, 2, matrix);
}

HRESULT ApplyFullXact3D(void* cue, X3DAudioDspSettingsView* dsp)
{
    if (!cue || !dsp || !dsp->matrixCoefficients)
        return E_POINTER;

    auto*** instance = reinterpret_cast<void***>(cue);
    if (!instance || !*instance || !(*instance)[4] ||
        !(*instance)[5] || !(*instance)[6])
    {
        return E_POINTER;
    }

    const auto setMatrix =
        reinterpret_cast<SetMatrixCoefficientsFn>((*instance)[4]);
    const auto getVariableIndex =
        reinterpret_cast<GetVariableIndexFn>((*instance)[5]);
    const auto setVariable =
        reinterpret_cast<SetVariableFn>((*instance)[6]);

    HRESULT hr = setMatrix(
        cue,
        dsp->sourceChannels,
        dsp->destinationChannels,
        dsp->matrixCoefficients);
    if (FAILED(hr))
        return hr;

    std::uint16_t index = getVariableIndex(cue, "Distance");
    hr = setVariable(cue, index, dsp->emitterToListenerDistance);
    if (FAILED(hr))
        return hr;

    index = getVariableIndex(cue, "DopplerPitchScalar");
    hr = setVariable(cue, index, dsp->dopplerFactor);
    if (FAILED(hr))
        return hr;

    index = getVariableIndex(cue, "OrientationAngle");
    return setVariable(
        cue,
        index,
        dsp->emitterToListenerAngle * kRadiansToDegrees);
}

HRESULT __cdecl ApplyRestored3DUpdate(void* cue, float* pcMatrix)
{
    if (!g_active.load(std::memory_order_acquire) ||
        !g_x3dDspSettings || !pcMatrix)
    {
        return CallVanilla2x2(cue, pcMatrix);
    }

    auto* dsp = g_x3dDspSettings;

    // Stereo is already correct in the PC port and is deliberately left exact.
    if (dsp->destinationChannels <= kStereoChannels)
        return CallVanilla2x2(cue, pcMatrix);

    if (dsp->sourceChannels != kXbox3DSourceChannels ||
        !IsSupportedSurroundCount(dsp->destinationChannels) ||
        !dsp->matrixCoefficients)
    {
        LogRuntimeWarningOnce("unexpected X3DAudio topology");
        return CallVanilla2x2(cue, pcMatrix);
    }

    const float gain = pcMatrix[0];
    const std::size_t coefficientCount =
        static_cast<std::size_t>(dsp->sourceChannels) *
        static_cast<std::size_t>(dsp->destinationChannels);

    for (std::size_t i = 0; i < coefficientCount; ++i)
        dsp->matrixCoefficients[i] *= gain;

    const HRESULT hr = ApplyFullXact3D(cue, dsp);
    if (SUCCEEDED(hr))
        return hr;

    LogRuntimeWarningOnce("restored 3D XACT apply failed");
    return CallVanilla2x2(cue, pcMatrix);
}

HRESULT __cdecl ApplyRestoredNon3DMatrix(void* cue, const float* parameters)
{
    if (!parameters)
        return E_POINTER;

    const float gain = parameters[0];
    float vanilla[4] = { gain, 0.0f, 0.0f, gain };

    if (!g_active.load(std::memory_order_acquire) || !g_x3dDspSettings)
        return CallVanilla2x2(cue, vanilla);

    const std::uint32_t destinationChannels =
        g_x3dDspSettings->destinationChannels;

    // Preserve the exact PC stereo path. The bug is specific to surround.
    if (destinationChannels <= kStereoChannels)
        return CallVanilla2x2(cue, vanilla);

    if (!IsSupportedSurroundCount(destinationChannels))
    {
        LogRuntimeWarningOnce("unsupported surround channel count");
        return CallVanilla2x2(cue, vanilla);
    }

    std::uint32_t mode = 0;
    std::memcpy(&mode, parameters + 9, sizeof(mode));

    HRESULT hr = E_FAIL;
    if (mode <= 1)
    {
        const float pan = parameters[8];
        const float leftDelta = 1.0f - pan;
        const float rightDelta = -1.0f - pan;
        const float leftWeight =
            (leftDelta < 0.0f ? -leftDelta : leftDelta) * kXboxPanScale;
        const float rightWeight =
            (rightDelta < 0.0f ? -rightDelta : rightDelta) * kXboxPanScale;
        const float leftGain = gain * leftWeight;
        const float rightGain = gain * rightWeight;

        // Windows XACT matrices are destination-row / source-column:
        // index = sourceChannels * destination + source.
        // Preserve the Xbox six-channel mix in the first six standard output
        // destinations. On 7.1, the additional side pair remains untouched.
        std::array<float, 16> matrix{};
        matrix[0] = leftGain;                    // destination 0 <- L
        matrix[3] = rightGain;                   // destination 1 <- R
        matrix[8] = leftGain * kXboxRearSend;    // destination 4 <- L
        matrix[11] = rightGain * kXboxRearSend;  // destination 5 <- R

        hr = CallSetMatrix(
            cue,
            2,
            destinationChannels,
            matrix.data());
    }
    else if (mode <= 3)
    {
        // Xbox modes 2/3 already carry six source channels. Keep that original
        // 5.1 identity in the first six destinations; a 7.1 side pair receives
        // zero rather than inventing an upmix that the original game did not have.
        std::array<float, 48> matrix{};
        for (std::size_t channel = 0; channel < kXboxInternalChannels; ++channel)
        {
            matrix[channel * kXboxInternalChannels + channel] = gain;
        }

        hr = CallSetMatrix(
            cue,
            kXboxInternalChannels,
            destinationChannels,
            matrix.data());
    }
    else
    {
        return CallVanilla2x2(cue, vanilla);
    }

    if (SUCCEEDED(hr))
        return hr;

    LogRuntimeWarningOnce("restored non-3D matrix was rejected");
    return CallVanilla2x2(cue, vanilla);
}

void __cdecl HookX3DAudioCalculate(
    const void* instance,
    const void* listener,
    const void* emitter,
    std::uint32_t flags,
    void* dspSettings)
{
    if (!g_originalX3DAudioCalculate)
        return;

    if (!g_active.load(std::memory_order_acquire) ||
        !listener || !emitter || !dspSettings ||
        dspSettings != g_x3dDspSettings)
    {
        g_originalX3DAudioCalculate(
            instance, listener, emitter, flags, dspSettings);
        return;
    }

    auto* dsp = static_cast<X3DAudioDspSettingsView*>(dspSettings);
    if (!IsSupportedSurroundCount(dsp->destinationChannels))
    {
        // Stereo and unknown topologies stay byte-for-byte on the PC path.
        g_originalX3DAudioCalculate(
            instance, listener, emitter, flags, dspSettings);
        return;
    }

    alignas(void*) std::array<unsigned char, kX3dListenerSize> xboxListener{};
    alignas(void*) std::array<unsigned char, kX3dEmitterSize> xboxEmitter{};
    std::memcpy(xboxListener.data(), listener, xboxListener.size());
    std::memcpy(xboxEmitter.data(), emitter, xboxEmitter.size());

    // Original Xbox handedness at the X3DAudio boundary. Flip listener and
    // emitter together so distances remain unchanged while left/right spatial
    // orientation matches the original console path.
    float listenerX = 0.0f;
    float listenerFrontX = 0.0f;
    float emitterX = 0.0f;
    std::memcpy(
        &listenerX,
        xboxListener.data() + kX3dListenerPositionXOffset,
        sizeof(listenerX));
    std::memcpy(
        &listenerFrontX,
        xboxListener.data() + kX3dListenerOrientFrontXOffset,
        sizeof(listenerFrontX));
    std::memcpy(
        &emitterX,
        xboxEmitter.data() + kX3dEmitterPositionXOffset,
        sizeof(emitterX));
    listenerX = -listenerX;
    listenerFrontX = -listenerFrontX;
    emitterX = -emitterX;
    std::memcpy(
        xboxListener.data() + kX3dListenerPositionXOffset,
        &listenerX,
        sizeof(listenerX));
    std::memcpy(
        xboxListener.data() + kX3dListenerOrientFrontXOffset,
        &listenerFrontX,
        sizeof(listenerFrontX));
    std::memcpy(
        xboxEmitter.data() + kX3dEmitterPositionXOffset,
        &emitterX,
        sizeof(emitterX));

    // Original Xbox positional emitters are mono. The PC port changed this to
    // two channels, widening point sources before they reach the surround mix.
    const std::uint32_t monoChannels = kXbox3DSourceChannels;
    const float channelRadius = 0.0f;
    float monoAzimuth = 0.0f;
    const uintptr_t monoAzimuthAddress =
        reinterpret_cast<uintptr_t>(&monoAzimuth);

    std::memcpy(
        xboxEmitter.data() + kX3dEmitterChannelCountOffset,
        &monoChannels,
        sizeof(monoChannels));
    std::memcpy(
        xboxEmitter.data() + kX3dEmitterChannelRadiusOffset,
        &channelRadius,
        sizeof(channelRadius));
    std::memcpy(
        xboxEmitter.data() + kX3dEmitterChannelAzimuthsOffset,
        &monoAzimuthAddress,
        sizeof(monoAzimuthAddress));
    dsp->sourceChannels = monoChannels;

    g_originalX3DAudioCalculate(
        instance,
        xboxListener.data(),
        xboxEmitter.data(),
        flags,
        dspSettings);
}

template <std::size_t N>
bool BuildCallsitePatch(
    std::array<unsigned char, N>& patch,
    void* helper,
    bool non3D)
{
    std::size_t cursor = 0;
    auto emit8 = [&](unsigned char value) {
        if (cursor < patch.size())
            patch[cursor++] = value;
    };
    auto emit32 = [&](std::uint32_t value) {
        if (cursor + sizeof(value) <= patch.size())
        {
            std::memcpy(patch.data() + cursor, &value, sizeof(value));
            cursor += sizeof(value);
        }
        else
        {
            cursor = patch.size() + 1;
        }
    };

    // Both callsites use local_c=[ebp-8] and local_c+0x14 = IXACT3Cue*.
    emit8(0x8B); emit8(0x45); emit8(0xF8); // mov eax,[ebp-8]
    emit8(0x8B); emit8(0x40); emit8(0x14); // mov eax,[eax+14h]

    if (non3D)
    {
        emit8(0x8B); emit8(0x4D); emit8(0x0C); // mov ecx,[ebp+0Ch] params
    }
    else
    {
        emit8(0x8D); emit8(0x4D); emit8(0xE8); // lea ecx,[ebp-18h] matrix
    }

    emit8(0x51); // push second argument
    emit8(0x50); // push cue
    emit8(0xB8); // mov eax, helper
    emit32(static_cast<std::uint32_t>(reinterpret_cast<uintptr_t>(helper)));
    emit8(0xFF); emit8(0xD0);              // call eax
    emit8(0x83); emit8(0xC4); emit8(0x08); // add esp,8

    if (cursor > patch.size())
        return false;

    while (cursor < patch.size())
        patch[cursor++] = 0x90;
    return true;
}

template <std::size_t N>
bool WriteCodePatch(
    unsigned char* target,
    const std::array<unsigned char, N>& patch,
    std::array<unsigned char, N>& original)
{
    DWORD oldProtect = 0;
    if (!VirtualProtect(target, N, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    std::memcpy(original.data(), target, N);
    std::memcpy(target, patch.data(), N);
    FlushInstructionCache(GetCurrentProcess(), target, N);
    const bool verified = std::memcmp(target, patch.data(), N) == 0;

    if (!verified)
    {
        std::memcpy(target, original.data(), N);
        FlushInstructionCache(GetCurrentProcess(), target, N);
    }

    DWORD ignored = 0;
    VirtualProtect(target, N, oldProtect, &ignored);
    return verified;
}

template <std::size_t N>
void RestoreCodePatch(
    unsigned char* target,
    const std::array<unsigned char, N>& original)
{
    if (!target)
        return;

    DWORD oldProtect = 0;
    if (!VirtualProtect(target, N, PAGE_EXECUTE_READWRITE, &oldProtect))
        return;

    std::memcpy(target, original.data(), N);
    FlushInstructionCache(GetCurrentProcess(), target, N);
    DWORD ignored = 0;
    VirtualProtect(target, N, oldProtect, &ignored);
}

bool PatchIatSlot(void** slot, void* replacement, void** originalOut)
{
    if (!slot || !*slot || !replacement || !originalOut)
        return false;

    MEMORY_BASIC_INFORMATION targetInfo = {};
    if (VirtualQuery(*slot, &targetInfo, sizeof(targetInfo)) != sizeof(targetInfo) ||
        targetInfo.State != MEM_COMMIT)
    {
        return false;
    }

    const DWORD protection = targetInfo.Protect & 0xFF;
    if (protection != PAGE_EXECUTE &&
        protection != PAGE_EXECUTE_READ &&
        protection != PAGE_EXECUTE_READWRITE &&
        protection != PAGE_EXECUTE_WRITECOPY)
    {
        return false;
    }

    DWORD oldProtect = 0;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect))
        return false;

    *originalOut = *slot;
    *slot = replacement;

    DWORD ignored = 0;
    VirtualProtect(slot, sizeof(*slot), oldProtect, &ignored);
    return *slot == replacement;
}

void RestoreIatSlot(void** slot, void* original)
{
    if (!slot || !original)
        return;

    DWORD oldProtect = 0;
    if (!VirtualProtect(slot, sizeof(*slot), PAGE_READWRITE, &oldProtect))
        return;

    *slot = original;
    DWORD ignored = 0;
    VirtualProtect(slot, sizeof(*slot), oldProtect, &ignored);
}

bool InstallSurroundAudioFix()
{
    if (g_active.load(std::memory_order_acquire))
        return true;

    if (!InitializeMainExeInfo())
        return false;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (!build)
        return false;

    uintptr_t matrixCallRva = 0;
    uintptr_t non3DMatrixCallRva = 0;
    switch (build->build)
    {
    case DpBuild::Steam101b:
        matrixCallRva = 0x0032CCCE;
        non3DMatrixCallRva = 0x0032CD57;
        break;
    case DpBuild::Gog101b:
        matrixCallRva = 0x0032C9DE;
        non3DMatrixCallRva = 0x0032CA67;
        break;
    default:
        return false;
    }

    const uintptr_t matrixAddress = g_mainExeBase + matrixCallRva;
    const uintptr_t non3DAddress = g_mainExeBase + non3DMatrixCallRva;
    const uintptr_t dspAddress = g_mainExeBase + kX3dDspSettingsRva;
    if (!IsRangeInsideMainExe(matrixAddress, sizeof(kExpected3DMatrixCallBytes)) ||
        !IsRangeInsideMainExe(non3DAddress, sizeof(kExpectedNon3DMatrixCallBytes)) ||
        !IsRangeInsideMainExe(dspAddress, sizeof(X3DAudioDspSettingsView)))
    {
        return false;
    }

    auto* matrixTarget = reinterpret_cast<unsigned char*>(matrixAddress);
    auto* non3DTarget = reinterpret_cast<unsigned char*>(non3DAddress);
    if (std::memcmp(
            matrixTarget,
            kExpected3DMatrixCallBytes,
            sizeof(kExpected3DMatrixCallBytes)) != 0 ||
        std::memcmp(
            non3DTarget,
            kExpectedNon3DMatrixCallBytes,
            sizeof(kExpectedNon3DMatrixCallBytes)) != 0)
    {
        return false;
    }

    HMODULE exe = GetModuleHandleW(nullptr);
    if (!exe)
        return false;

    void** x3dSlot = FindImportAddressSlot(
        exe,
        "X3DAudio1_7.dll",
        "X3DAudioCalculate");
    if (!x3dSlot || !*x3dSlot)
        return false;

    std::array<unsigned char, sizeof(kExpected3DMatrixCallBytes)> matrixPatch{};
    std::array<unsigned char, sizeof(kExpectedNon3DMatrixCallBytes)> non3DPatch{};
    if (!BuildCallsitePatch(
            matrixPatch,
            reinterpret_cast<void*>(&ApplyRestored3DUpdate),
            false) ||
        !BuildCallsitePatch(
            non3DPatch,
            reinterpret_cast<void*>(&ApplyRestoredNon3DMatrix),
            true))
    {
        return false;
    }

    std::array<unsigned char, sizeof(kExpected3DMatrixCallBytes)> matrixOriginal{};
    std::array<unsigned char, sizeof(kExpectedNon3DMatrixCallBytes)> non3DOriginal{};
    void* x3dOriginal = *x3dSlot;
    g_originalX3DAudioCalculate =
        reinterpret_cast<X3DAudioCalculateFn>(x3dOriginal);

    // Keep g_active false until the complete transaction succeeds. If DP ever
    // reaches the IAT hook during installation it therefore chains to the
    // original X3DAudioCalculate rather than observing a partial fix.
    if (!PatchIatSlot(
            x3dSlot,
            reinterpret_cast<void*>(&HookX3DAudioCalculate),
            &x3dOriginal))
    {
        g_originalX3DAudioCalculate = nullptr;
        return false;
    }

    if (!WriteCodePatch(matrixTarget, matrixPatch, matrixOriginal))
    {
        RestoreIatSlot(x3dSlot, x3dOriginal);
        g_originalX3DAudioCalculate = nullptr;
        return false;
    }

    if (!WriteCodePatch(non3DTarget, non3DPatch, non3DOriginal))
    {
        RestoreCodePatch(matrixTarget, matrixOriginal);
        RestoreIatSlot(x3dSlot, x3dOriginal);
        g_originalX3DAudioCalculate = nullptr;
        return false;
    }

    g_x3dDspSettings =
        reinterpret_cast<X3DAudioDspSettingsView*>(dspAddress);
    g_active.store(true, std::memory_order_release);
    return true;
}
} // namespace

void ConfigureSurroundAudioFix(bool requested)
{
    if (!InitializeMainExeInfo() || !GetDpBuildProfile())
    {
        g_available.store(false, std::memory_order_release);
        g_active.store(false, std::memory_order_release);
        return;
    }

    g_available.store(true, std::memory_order_release);
    if (!requested)
        return;

    if (!InstallSurroundAudioFix())
    {
        g_available.store(false, std::memory_order_release);
        g_active.store(false, std::memory_order_release);
        AppendLog(
            "[Audio][SurroundFix] ERROR: build/import/signature validation failed; audio remains vanilla.\n");
        return;
    }

    AppendLog(
        "[Audio][SurroundFix] Active: restored Xbox 360 positional and non-3D surround routing; stereo remains vanilla and 5.1/7.1 keep the native Windows output graph.\n");
}

bool IsSurroundAudioFixAvailable()
{
    return g_available.load(std::memory_order_acquire);
}

bool IsSurroundAudioFixActive()
{
    return g_active.load(std::memory_order_acquire);
}
