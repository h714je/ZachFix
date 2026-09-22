#pragma once

#include <d3d9.h>

// Loads the optional [Screenshots] / [ScreenshotPreset.*] configuration and
// captures the startup ZachFix configuration as the "Configured" preset base.
void InitializeScreenshotPresetSystem();

// Called once for the outermost game Present after the optional Xbox HDTV
// display transfer and before ZachFix draws its Present-time UI. Polls the
// comparison hotkeys, advances capture-all sequencing, and writes PNGs.
void ProcessScreenshotPresetFrame(IDirect3DDevice9* device);
