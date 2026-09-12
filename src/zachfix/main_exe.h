#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>

extern uintptr_t g_mainExeBase;
extern size_t g_mainExeSize;
extern DWORD g_mainExeTimeDateStamp;
extern bool g_mainExeInfoValid;

bool InitializeMainExeInfo();
