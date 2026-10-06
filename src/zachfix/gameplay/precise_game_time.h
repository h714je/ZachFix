#pragma once

// Repairs two retail QPC-to-time helpers that execute after Direct3D9 has
// switched the gameplay thread's x87 precision to PC24. Both helpers convert
// an absolute QPC value before subtracting timestamps, so their resolution
// degrades with Windows uptime. ZachFix keeps the game's ambient precision
// unchanged and uses PC53 only while those two native helpers execute.
bool InstallPreciseGameTimeFix();

bool IsPreciseGameTimeFixActive();
