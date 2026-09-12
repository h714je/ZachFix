#pragma once

#include <Windows.h>
#include <d3d9.h>

bool InitializeSettingsUi(HWND window, IDirect3DDevice9* device);
void RenderSettingsUi(IDirect3DDevice9* device);
void ShutdownSettingsUi();
