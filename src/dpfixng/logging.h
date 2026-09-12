#pragma once

#include <Windows.h>
#include <d3d9.h>

void SetLogModule(HMODULE module);
void ResetLog();
void AppendLog(const char* text);

void LogResolutionOverride(
    const char* resourceType,
    UINT originalWidth,
    UINT originalHeight,
    UINT newWidth,
    UINT newHeight,
    D3DFORMAT format,
    DWORD usage
);
