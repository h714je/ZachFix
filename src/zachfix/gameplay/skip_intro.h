#pragma once

// Optional startup quality-of-life patch. On the supported PC builds the
// boot flow seeds a native startup state with 0xB3; changing only that immediate
// to 0 bypasses the publisher/logo intro path. The exact instruction and the
// following state write are signature checked before ZachFix changes anything.
bool InstallSkipIntroPatch(bool enabled);

bool IsSkipIntroPatchAvailable();
bool IsSkipIntroPatchActive();
