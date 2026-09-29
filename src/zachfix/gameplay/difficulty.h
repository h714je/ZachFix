#pragma once

// Current native difficulty state exposed by the restored title selector.
// 0 = Easy, 1 = Normal, 2 = Hard.
unsigned int GetCurrentDifficultyValue();
const char* GetCurrentDifficultyName();

// Restores the original New Game Easy / Normal / Hard selector and the native
// writes that persist the chosen value in Deadly Premonition's save record.
bool InstallDifficultyRestoration();
