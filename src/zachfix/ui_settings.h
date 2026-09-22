#pragma once

#include <Windows.h>
#include <d3d9.h>

bool InitializeSettingsUi(HWND window, IDirect3DDevice9* device);
void InvalidateSettingsUiDeviceObjects();
void NotifySettingsUiResetResult(HRESULT resetResult);
void RenderSettingsUi(IDirect3DDevice9* device);
void RenderSettingsUiInScene(IDirect3DDevice9* device);

bool IsSettingsUiOpen();
