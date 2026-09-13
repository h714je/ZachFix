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
#include <intrin.h>
#include <mutex>

#include "zachfix/config.h"
#include "zachfix/version.h"
#include "zachfix/logging.h"
#include "zachfix/main_exe.h"
#include "zachfix/world_streaming.h"
#include "zachfix/runtime_resources.h"
#include "zachfix/ui_settings.h"
#include "zachfix/texture_override.h"

// Implementation is split by subsystem but intentionally kept in one translation unit.
#include "zachfix/core.inl"
#include "zachfix/rendering.inl"
#include "zachfix/dof_blur.inl"
#include "zachfix/texture_filtering.inl"
#include "zachfix/runtime_resources.inl"
#include "zachfix/effect_probe.inl"
#include "zachfix/d3d9_device.inl"
#include "zachfix/d3d9_entry.inl"
#include "zachfix/initialization.inl"

