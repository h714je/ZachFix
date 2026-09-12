// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

static DWORD WINAPI InitializeHooks(LPVOID)
{
    wchar_t exePath[MAX_PATH] = {};

    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0)
        return 0;

    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = (exeName != nullptr) ? exeName + 1 : exePath;

    // Do nothing inside DPLauncher.exe or other processes.
    if (_wcsicmp(exeName, L"DP.exe") != 0)
        return 0;

    ResetLog();

    AppendLog("DPFix-NG v0.0.36 Runtime Audit + SSAO exp1 fix10\n");
    AppendLog("Initialization started.\n");

    LoadConfig();

    HMODULE d3d9 = nullptr;

    for (int i = 0; i < 500 && d3d9 == nullptr; ++i)
    {
        d3d9 = GetModuleHandleW(L"d3d9.dll");

        if (d3d9 == nullptr)
            Sleep(10);
    }

    if (d3d9 == nullptr)
    {
        AppendLog("ERROR: d3d9.dll was not found.\n");
        return 0;
    }

    AppendLog("d3d9.dll found.\n");

    FARPROC target =
        GetProcAddress(d3d9, "Direct3DCreate9");

    if (target == nullptr)
    {
        AppendLog("ERROR: Direct3DCreate9 export not found.\n");
        return 0;
    }

    MH_STATUS status = MH_Initialize();

    if (status != MH_OK &&
        status != MH_ERROR_ALREADY_INITIALIZED)
    {
        AppendLog("ERROR: MH_Initialize failed.\n");
        return 0;
    }

    // Version-gated internal world experiment. Failure is non-fatal.
    PrepareWorldCellDetailClassifyHook();
    ApplyWorldDetailDistanceScale(g_config.highDetailDistanceScale);

    // Version-gated internal probe. Failure is non-fatal and does not affect
    // the established D3D9 fixes/profiler.
    PrepareLifecycleTraceHooks();
    PrepareCandidateVisibilityTraceHook();
    PrepareSceneOwnerTraceHook();
    PrepareSceneObjectTraceHook();

    status = MH_CreateHook(
        reinterpret_cast<void*>(target),
        reinterpret_cast<void*>(&HookDirect3DCreate9),
        reinterpret_cast<void**>(&g_originalDirect3DCreate9)
    );

    if (status != MH_OK)
    {
        AppendLog("ERROR: Direct3DCreate9 MH_CreateHook failed.\n");
        return 0;
    }

    status = MH_EnableHook(
        reinterpret_cast<void*>(target)
    );

    if (status != MH_OK)
    {
        AppendLog("ERROR: Direct3DCreate9 MH_EnableHook failed.\n");
        return 0;
    }

    AppendLog("Direct3DCreate9 hook installed.\n");

    return 0;
}


BOOL WINAPI DllMain(
    HINSTANCE instance,
    DWORD reason,
    LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        SetLogModule(instance);

        DisableThreadLibraryCalls(instance);

        HANDLE thread = CreateThread(
            nullptr,
            0,
            InitializeHooks,
            nullptr,
            0,
            nullptr
        );

        if (thread != nullptr)
            CloseHandle(thread);
    }

    return TRUE;
}
