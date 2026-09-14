#pragma once

#include <Windows.h>

struct GameplayPauseStats
{
    bool hooksInstalled = false;
    bool active = false;
    UINT installedHooks = 0;
    unsigned long long qpcCalls = 0;
    unsigned long long tick32Calls = 0;
    unsigned long long tick64Calls = 0;
    unsigned long long timeGetTimeCalls = 0;
};

bool InitializeGameplayPauseHooks();
void SetGameplayPauseActive(bool active);
GameplayPauseStats GetGameplayPauseStats();
