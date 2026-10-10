// Experimental: correct right/bottom 1-pixel seams on DP's fullscreen 2D
// fills when ZachFix scales the original 1280x720 viewport above native size.
// One native draw hook, three verified CALL sites per supported executable.
// This is restart-only and disabled unless explicitly enabled in ZachFix.ini.

using Fullscreen2DDrawFn = void (__thiscall*)(
    void* renderer, void* geometry, const float* rgba,
    const void* colors, const void* uv, const void* transform);

static Fullscreen2DDrawFn g_fullscreen2DOriginalDraw = nullptr;
static uintptr_t g_fullscreen2DCallers[3] = {};

// The native renderer uses the logical canvas dimensions when geometry is null.
// Both supported builds use these same float globals.
static bool Fullscreen2DImplicitCanvasIsNative()
{
    constexpr uintptr_t kWidthRva = 0x01080974;
    constexpr uintptr_t kHeightRva = 0x01080970;
    if (g_mainExeBase == 0 || g_mainExeSize < kWidthRva + sizeof(float))
        return false;

    const float width = *reinterpret_cast<const float*>(g_mainExeBase + kWidthRva);
    const float height = *reinterpret_cast<const float*>(g_mainExeBase + kHeightRva);
    return std::isfinite(width) && std::isfinite(height) &&
        std::fabs(width - 1280.0f) < 0.01f &&
        std::fabs(height - 720.0f) < 0.01f;
}

static bool Fullscreen2DIsNativeCanvas(const float* geometry)
{
    if (geometry == nullptr)
        return Fullscreen2DImplicitCanvasIsNative();

    return std::isfinite(geometry[0]) && std::isfinite(geometry[1]) &&
        std::isfinite(geometry[2]) && std::isfinite(geometry[3]) &&
        std::fabs(geometry[0]) < 0.01f &&
        std::fabs(geometry[1]) < 0.01f &&
        std::fabs(geometry[2] - 1280.0f) < 0.01f &&
        std::fabs(geometry[3] - 720.0f) < 0.01f;
}

static bool Fullscreen2DHasSafeBackbufferViewport()
{
    const auto* current = g_currentRenderTarget0.load(std::memory_order_acquire);
    const auto* backbuffer = g_backBuffer0.load(std::memory_order_acquire);
    if (current == nullptr || backbuffer == nullptr || current != backbuffer)
        return false;

    IDirect3DDevice9* device = g_gameD3D9Device.load(std::memory_order_acquire);
    if (device == nullptr)
        return false;

    D3DVIEWPORT9 viewport{};
    DWORD scissor = TRUE;
    if (FAILED(device->GetViewport(&viewport)) ||
        FAILED(device->GetRenderState(D3DRS_SCISSORTESTENABLE, &scissor)))
        return false;

    return viewport.X == 0 && viewport.Y == 0 &&
        viewport.Width == g_displayWidth && viewport.Height == g_displayHeight &&
        scissor == FALSE;
}

static void __fastcall HookFullscreen2DDraw(
    void* renderer, void* /*edx*/, void* geometry, const float* rgba,
    const void* colors, const void* uv, const void* transform)
{
    if (g_fullscreen2DOriginalDraw == nullptr)
        return;

    // Cheap return-address gate first. Leave every unrelated UI draw intact.
    const uintptr_t caller = reinterpret_cast<uintptr_t>(_ReturnAddress());
    if ((caller == g_fullscreen2DCallers[0] ||
         caller == g_fullscreen2DCallers[1] ||
         caller == g_fullscreen2DCallers[2]) &&
        transform == nullptr &&
        g_displayWidth > 1280 && g_displayHeight > 720 &&
        Fullscreen2DIsNativeCanvas(static_cast<const float*>(geometry)) &&
        Fullscreen2DHasSafeBackbufferViewport())
    {
        const float* original = static_cast<const float*>(geometry);
        float adjusted[4] = {
            original != nullptr ? original[0] : 0.0f,
            original != nullptr ? original[1] : 0.0f,
            (original != nullptr ? original[2] : 1280.0f) +
                1280.0f / static_cast<float>(g_displayWidth),
            (original != nullptr ? original[3] : 720.0f) +
                720.0f / static_cast<float>(g_displayHeight)
        };
        // Preserve original color, UV and transform. The native renderer
        // consumes this stack-local rectangle synchronously.
        g_fullscreen2DOriginalDraw(renderer, adjusted, rgba, colors, uv, transform);
        return;
    }
    g_fullscreen2DOriginalDraw(renderer, geometry, rgba, colors, uv, transform);
}

// Fail closed if a binary was patched or a disassembler-export RVA is wrong.
// The 5-byte near CALL must lead to the exact shared quad renderer.
static bool Fullscreen2DVerifyCall(uintptr_t returnRva, uintptr_t drawRva)
{
    if (returnRva < 5 || g_mainExeBase == 0 ||
        g_mainExeSize < returnRva)
        return false;
    const auto* instruction = reinterpret_cast<const unsigned char*>(
        g_mainExeBase + returnRva - 5);
    if (instruction[0] != 0xE8)
        return false;
    int32_t relative = 0;
    std::memcpy(&relative, instruction + 1, sizeof(relative));
    return static_cast<int64_t>(returnRva) + relative ==
        static_cast<int64_t>(drawRva);
}

static void InstallFullscreen2DEdgeFix()
{
    if (!g_config.experimentalFullscreen2DEdgeFix)
        return;

    const DpBuildProfile* build = GetDpBuildProfile();
    if (build == nullptr)
    {
        AppendLog("[Render][Fullscreen2DEdge] Unsupported DP.exe; disabled.\n");
        return;
    }

    const bool steam = build->build == DpBuild::Steam101b;
    if (!steam && build->build != DpBuild::Gog101b)
        return;

    const uintptr_t drawRva = steam ? 0x002E3360 : 0x002E33D0;
    const uintptr_t callRvas[3] = {
        steam ? 0x0004A11Eu : 0x0004A14Eu, // Regular CFadeManager
        steam ? 0x0004ACC5u : 0x0004ACF5u, // One-shot CFadeManager
        steam ? 0x0021BF48u : 0x0021BEC8u  // Item notification event
    };
    constexpr unsigned char kExpected[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x68, 0x89, 0x4D, 0x98
    };
    if (g_mainExeBase == 0 ||
        g_mainExeSize < drawRva + sizeof(kExpected) ||
        std::memcmp(reinterpret_cast<const void*>(g_mainExeBase + drawRva),
                    kExpected, sizeof(kExpected)) != 0 ||
        !Fullscreen2DVerifyCall(callRvas[0], drawRva) ||
        !Fullscreen2DVerifyCall(callRvas[1], drawRva) ||
        !Fullscreen2DVerifyCall(callRvas[2], drawRva))
    {
        AppendLog("[Render][Fullscreen2DEdge] Native signature/call mismatch; disabled.\n");
        return;
    }

    Fullscreen2DDrawFn trampoline = nullptr;
    const MH_STATUS created = MH_CreateHook(
        reinterpret_cast<void*>(g_mainExeBase + drawRva),
        reinterpret_cast<void*>(&HookFullscreen2DDraw),
        reinterpret_cast<void**>(&trampoline));
    if (created != MH_OK || trampoline == nullptr)
    {
        AppendLog("[Render][Fullscreen2DEdge] Hook creation failed; disabled.\n");
        return;
    }

    g_fullscreen2DOriginalDraw = trampoline;
    for (unsigned i = 0; i != 3; ++i)
        g_fullscreen2DCallers[i] = g_mainExeBase + callRvas[i];

    if (MH_EnableHook(reinterpret_cast<void*>(g_mainExeBase + drawRva)) != MH_OK)
    {
        AppendLog("[Render][Fullscreen2DEdge] Hook enable failed; disabled.\n");
        return;
    }
    AppendLog("[Render][Fullscreen2DEdge] Active: three verified fullscreen 2D "
              "call sites, backbuffer/viewport/scissor guarded.\n");
}
