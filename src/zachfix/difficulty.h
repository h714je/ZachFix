#pragma once

#include <cstdint>

enum class GameDifficulty : unsigned int
{
    Easy = 0,
    Normal = 1,
    Hard = 2
};

// Parses -zachfix-difficulty=0/1/2 once for the current process.
// Missing or invalid values fall back to Easy.
void InitializeDifficultySession();

GameDifficulty GetSessionDifficulty();
unsigned int GetSessionDifficultyValue();
const char* GetSessionDifficultyName();
const char* GetSessionDifficultyProfileName();

// Restores the surviving native difficulty byte after Director's Cut reset
// writes and full live-record restores. No save-state migration is performed.
bool InstallDifficultyRestoration();
