#pragma once

// Repairs the Director's Cut PC HOUSE_LIST.NOD runtime endian regression.
// Stock Steam/GOG data is fingerprinted and fully normalized back to the
// byte-identical Xbox/PC source semantics before the native CLevel consumer.
// Unknown/modded payloads keep a conservative per-lookup fallback.
bool InstallHouseListEndianFix();
