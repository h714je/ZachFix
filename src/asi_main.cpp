#include <Windows.h>
#include <d3d9.h>
#include <MinHook.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <mutex>

#include "dpfixng/config.h"
#include "dpfixng/logging.h"
#include "dpfixng/world_streaming.h"
#include "dpfixng/runtime_resources.h"
#include "dpfixng/ui_settings.h"
#include "dpfixng/texture_override.h"

// Implementation is split by subsystem but intentionally kept in one translation unit.
#include "dpfixng/core.inl"
#include "dpfixng/rendering.inl"
#include "dpfixng/runtime_resources.inl"
#include "dpfixng/d3d9_device.inl"
#include "dpfixng/d3d9_entry.inl"
#include "dpfixng/initialization.inl"

