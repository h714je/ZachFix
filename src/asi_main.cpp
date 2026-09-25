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
#include <initializer_list>
#include <mutex>
#include <vector>

#include "zachfix/config.h"
#include "zachfix/version.h"
#include "zachfix/logging.h"
#include "zachfix/main_exe.h"
#include "zachfix/d3d9_scope.h"
#include "zachfix/save_diag.h"
#include "zachfix/difficulty.h"
#include "zachfix/vanilla_nan_fix.h"
#include "zachfix/world_streaming.h"
#include "zachfix/world_alternate3d_distance.h"
#include "zachfix/native_xinput.h"
#include "zachfix/input_mode.h"
#include "zachfix/combat_strafe.h"
#include "zachfix/runtime_resources.h"
#include "zachfix/screenshot_presets.h"
#include "zachfix/ui_settings.h"
#include "zachfix/texture_override.h"
#include "zachfix/house_list_fix.h"
#include "zachfix/postfx.h"
#include "zachfix/postfx_tuning.h"
#include "zachfix/postfx_ao.h"
#include "zachfix/postfx_bloom.h"
#include "zachfix/postfx_dof.h"
#include "zachfix/postfx_exposure.h"

// Implementation is split by subsystem but intentionally kept in one translation unit.
#include "zachfix/core.inl"
#include "zachfix/rendering.inl"
#include "zachfix/dof_blur.inl"
#include "zachfix/texture_filtering.inl"
#include "zachfix/runtime_resources.inl"
#include "zachfix/postfx.inl"
#include "zachfix/postfx_tuning.inl"
#include "zachfix/postfx_ao.inl"
#include "zachfix/postfx_bloom.inl"
#include "zachfix/postfx_dof.inl"
#include "zachfix/postfx_exposure.inl"
#include "zachfix/d3d9_device.inl"
#include "zachfix/d3d9_entry.inl"
#include "zachfix/initialization.inl"

