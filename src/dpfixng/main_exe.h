#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>

extern uintptr_t g_mainExeBase;
extern size_t g_mainExeSize;
extern uintptr_t g_mainExePreferredBase;
extern UINT g_mainExeEntryRva;
extern DWORD g_mainExeTimeDateStamp;
extern WORD g_mainExeCharacteristics;
extern bool g_mainExeInfoValid;

bool InitializeMainExeInfo();
