#include <Windows.h>
#include <intrin.h>
#include <d3d9.h>
#include <MinHook.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cwchar>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <unordered_map>
#include <vector>


#include "dpfixng/config.h"
#include "dpfixng/logging.h"
#include "dpfixng/main_exe.h"
#include "dpfixng/world_streaming.h"
#include "dpfixng/ui_settings.h"

// Implementation is split by subsystem but intentionally kept in one translation unit.
#include "dpfixng/core.inl"
#include "dpfixng/rendering.inl"
#include "dpfixng/profiler_core.inl"
#include "dpfixng/profiler_trace.inl"
#include "dpfixng/d3d9_device.inl"
#include "dpfixng/d3d9_entry.inl"
#include "dpfixng/initialization.inl"

