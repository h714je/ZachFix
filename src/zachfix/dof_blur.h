#pragma once

#include <Windows.h>

// Changes only the optional additional DoF blur amount. This is a lightweight
// runtime toggle and does not rebuild the game's DoF render targets.
bool SetAdditionalDofBlurLive(UINT amount);
