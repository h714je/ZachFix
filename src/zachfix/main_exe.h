#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <atomic>

enum class DpBuild
{
    Unknown,
    Steam101b,
    Gog101b
};

struct DpBuildProfile
{
    DpBuild build;
    const char* name;

    DWORD timeDateStamp;
    size_t sizeOfImage;

    uintptr_t direct3DCreate9IatRva;
    uintptr_t speedDivideRva;
    uintptr_t frameDeltaRva;
    uintptr_t controllerBindingEvaluatorRva;
    uintptr_t useJoyModeRva;
    uintptr_t inputUpdateRva;
    uintptr_t worldCellDetailClassifyRva;
    uintptr_t worldIncrementalOuterClassifyRva;
};

extern uintptr_t g_mainExeBase;
extern size_t g_mainExeSize;
extern DWORD g_mainExeTimeDateStamp;
extern std::atomic_bool g_mainExeInfoValid;

// Pure PE-header lookup used by the earliest DllMain hook. This performs no
// logging, file I/O, allocation, or synchronization.
const DpBuildProfile* DetectDpBuildProfile(HMODULE module);

// Cached main-executable information for normal worker-thread initialization.
bool InitializeMainExeInfo();
const DpBuildProfile* GetDpBuildProfile();
