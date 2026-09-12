// -----------------------------------------------------------------------------
// v0.0.27 reverse-engineering probe: continuous candidate/owner/draw tracing
// -----------------------------------------------------------------------------

static constexpr uintptr_t kRenderSceneObjectRva = 0x002D69E0;

static bool ProfilerFocusedTraceActive()
{
    return g_profilerCaptureActive.load(std::memory_order_relaxed) ||
           g_profilerContinuousActive.load(std::memory_order_relaxed);
}

static void __fastcall HookRenderSceneObject(
    void* self,
    void*,
    void* sceneObject,
    const short* visibleSubmeshList,
    int passMode)
{
    const uintptr_t previousObject = g_profilerSceneObject;
    const uintptr_t previousOwner = g_profilerSceneOwner;
    const UINT previousCallerRva = g_profilerSceneObjectCallerRva;

    if (ProfilerFocusedTraceActive())
    {
        g_profilerSceneObject = reinterpret_cast<uintptr_t>(sceneObject);
        g_profilerSceneOwner = 0;

        const auto ownerIt =
            g_profilerSceneObjectOwners.find(g_profilerSceneObject);

        if (ownerIt != g_profilerSceneObjectOwners.end())
        {
            g_profilerSceneOwner = ownerIt->second;
        }

        const uintptr_t returnAddress =
            reinterpret_cast<uintptr_t>(_ReturnAddress());

        if (g_mainExeInfoValid &&
            returnAddress >= g_mainExeBase &&
            returnAddress < g_mainExeBase + g_mainExeSize)
        {
            g_profilerSceneObjectCallerRva =
                static_cast<UINT>(returnAddress - g_mainExeBase);
        }
        else
        {
            g_profilerSceneObjectCallerRva = 0;
        }
    }

    g_originalRenderSceneObject(
        self,
        sceneObject,
        visibleSubmeshList,
        passMode
    );

    g_profilerSceneObject = previousObject;
    g_profilerSceneOwner = previousOwner;
    g_profilerSceneObjectCallerRva = previousCallerRva;
}




