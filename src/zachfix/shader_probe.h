#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <cstdint>

struct ShaderProbeStats
{
    unsigned long long registeredVertexShaders = 0;
    unsigned long long registeredPixelShaders = 0;
    unsigned long long gameVertexShaders = 0;
    unsigned long long gamePixelShaders = 0;
    unsigned long long gameShaderBinds = 0;
    unsigned long long unknownGameShaderBinds = 0;
    unsigned long long dumpSuccesses = 0;
    unsigned long long dumpFailures = 0;

    std::uint64_t currentVertexShaderHash = 0;
    std::uint64_t currentPixelShaderHash = 0;

    bool captureRequested = false;
    bool captureActive = false;
    bool captureAvailable = false;
    unsigned long long capturedEvents = 0;
    UINT capturedUniqueVertexShaders = 0;
    UINT capturedUniquePixelShaders = 0;
    UINT capturedTargetDraws = 0;
};

void RegisterShaderProbeVertexShader(IDirect3DVertexShader9* shader);
void RegisterShaderProbePixelShader(IDirect3DPixelShader9* shader);
void NotifyShaderProbeVertexShaderBound(IDirect3DVertexShader9* shader);
void NotifyShaderProbePixelShaderBound(IDirect3DPixelShader9* shader);
void NotifyShaderProbeDraw(IDirect3DDevice9* device, const char* drawKind);

// Called once at the end of a presented game frame. A pending capture is armed
// here so a request made from the F10 UI captures the following complete frame.
void AdvanceShaderProbeFrame();

ShaderProbeStats GetShaderProbeStats();
bool DumpShaderProbeShaders();
bool RequestShaderProbeFrameCapture();

// Research-only runtime control for the identified final-composite bloom term.
// 1.0 = vanilla. The override is applied only around the game's final-composite
// draw and the original constant is restored immediately afterwards.
float GetShaderProbeBloomMultiplier();
void SetShaderProbeBloomMultiplier(float multiplier);
bool BeginShaderProbeBloomOverride(IDirect3DDevice9* device, float previousConstant[4]);
void EndShaderProbeBloomOverride(IDirect3DDevice9* device, const float previousConstant[4]);

// Research-only runtime control for the final-composite exposure term (c10).
// 1.0 = vanilla. Like bloom, the original constant is restored after the draw.
float GetShaderProbeExposureMultiplier();
void SetShaderProbeExposureMultiplier(float multiplier);
bool BeginShaderProbeExposureOverride(IDirect3DDevice9* device, float previousConstant[4]);
void EndShaderProbeExposureOverride(IDirect3DDevice9* device, const float previousConstant[4]);
