#pragma once

#include <Windows.h>
#include <d3d9.h>

struct EffectProbeIsolationSettings
{
    bool skipPs93AC9D9C = false;
    bool skipPsF4D0BDDE = false;
    bool dpfixEnemyShadowTrailFix = false;
};

EffectProbeIsolationSettings GetEffectProbeIsolationSettings();
void SetEffectProbeIsolationSettings(const EffectProbeIsolationSettings& settings);

bool InitializeSettingsUi(HWND window, IDirect3DDevice9* device);
void RenderSettingsUi(IDirect3DDevice9* device);
void RenderSettingsUiInScene(IDirect3DDevice9* device);
