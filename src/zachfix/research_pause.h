#pragma once

#include <Windows.h>

struct ResearchPauseStats
{
    bool hooksInstalled = false;
    bool active = false;
    UINT installedHooks = 0;
    unsigned long long qpcCalls = 0;
    unsigned long long tick32Calls = 0;
    unsigned long long tick64Calls = 0;
    unsigned long long timeGetTimeCalls = 0;
};

bool InitializeResearchPauseHooks();
void SetResearchPauseActive(bool active);
bool IsResearchPauseActive();
ResearchPauseStats GetResearchPauseStats();
