#pragma once

#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <atomic>

extern uintptr_t g_mainExeBase;
extern size_t g_mainExeSize;
extern DWORD g_mainExeTimeDateStamp;
extern std::atomic_bool g_mainExeInfoValid;

bool InitializeMainExeInfo();
