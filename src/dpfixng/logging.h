#pragma once

#include <Windows.h>
#include <d3d9.h>

void SetLogModule(HMODULE module);
HMODULE GetLogModule();
void ResetLog();
void AppendLog(const char* text);

void LogResourceOnce(
    unsigned long long type,
    const char* name,
    UINT width,
    UINT height,
    DWORD usage,
    D3DFORMAT format,
    D3DPOOL pool,
    UINT levels,
    D3DMULTISAMPLE_TYPE multiSample,
    DWORD multiSampleQuality,
    BOOL extraFlag
);

void LogResolutionOverride(
    const char* resourceType,
    UINT originalWidth,
    UINT originalHeight,
    UINT newWidth,
    UINT newHeight,
    D3DFORMAT format,
    DWORD usage
);
