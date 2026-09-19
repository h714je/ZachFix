#pragma once

// Repairs the Director's Cut PC HOUSE_LIST.NOD runtime lookup-key endian regression.
// The raw PC/Xbox payloads are byte-identical; the PC runtime table is only
// partially converted to host endian before the native CLevel lookup.
// The fix is build/signature gated and preserves native CLevel day/night logic.
bool InstallHouseListEndianFix();
