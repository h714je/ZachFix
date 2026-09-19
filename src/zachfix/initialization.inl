// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------

static DWORD WINAPI InitializeHooks(LPVOID)
{
    struct InitializationReadyOnExit
    {
        ~InitializationReadyOnExit()
        {
            g_initializationReady.store(true, std::memory_order_release);
        }
    } readyOnExit;

    wchar_t exePath[MAX_PATH] = {};

    if (GetModuleFileNameW(nullptr, exePath, MAX_PATH) == 0)
        return 0;

    const wchar_t* exeName = wcsrchr(exePath, L'\\');
    exeName = (exeName != nullptr) ? exeName + 1 : exePath;

    // Do nothing inside DPLauncher.exe or other processes.
    if (_wcsicmp(exeName, L"DP.exe") != 0)
        return 0;

    ResetLog();

    AppendLog(kZachFixDisplayName);
    AppendLog("\n");
    LogBuildIdentity();
    AppendLog("Initialization started.\n");

    if (InitializeMainExeInfo())
    {
        const DpBuildProfile* build = GetDpBuildProfile();
        char buildText[192] = {};

        if (build != nullptr)
        {
            sprintf_s(
                buildText,
                "[Build] Detected DP.exe: %s "
                "(TimeDateStamp=0x%08lX, SizeOfImage=0x%08lX).\n",
                build->name,
                static_cast<unsigned long>(g_mainExeTimeDateStamp),
                static_cast<unsigned long>(g_mainExeSize));
        }
        else
        {
            sprintf_s(
                buildText,
                "[Build] Unsupported DP.exe "
                "(TimeDateStamp=0x%08lX, SizeOfImage=0x%08lX). "
                "Build-specific hooks will stay disabled.\n",
                static_cast<unsigned long>(g_mainExeTimeDateStamp),
                static_cast<unsigned long>(g_mainExeSize));
        }

        AppendLog(buildText);
    }
    else
    {
        AppendLog("[Build] DP.exe PE information unavailable.\n");
    }

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

    // Repair the confirmed Director's Cut HOUSE_LIST.NOD key-endian regression
    // before the first world CLevel objects are configured. Native direct
    // matches remain untouched; only a unique byte-swapped miss is repaired
    // for the duration of the original native lookup call.
    InstallHouseListEndianFix();

    // Restore the original New Game Easy / Normal / Hard selector and the
    // game's native save-backed difficulty flow.
    InstallDifficultyRestoration();

    // Save I/O tracing and transactional protection keep DP's vanilla
    // savedata\dp.sav path. Difficulty remains part of the native save record.
    InstallSaveDiagHooks();

    // Vanilla stability fix: DP can produce a zero-delta frame, and one actor
    // speed path performs 0/0 when the actor also did not move. The resulting
    // NaN reaches a deliberate infinite-loop sentinel. Build/signature gated
    // and deliberately limited to that exact 0/0 case.
    InstallVanillaZeroDeltaNaNFix();

    // Optional native XInput backend. DP keeps its vanilla controller action
    // and binding logic; ZachFix supplies an XInput-backed JOYINFOEX view and
    // translates the legacy axis semantics at DP's common evaluator.
    InstallNativeXInputBackend();

    // Auto-switch around DP's own USEJOY byte. This deliberately
    // keeps the original keyboard/mouse and controller action paths intact;
    // only the active vanilla mode is changed in response to real device input.
    InstallInputModeAutoSwitch();

    // Texture override is independent of the D3D9 device hook surface.
    // Failure is non-fatal: rendering fixes must still start normally.
    InstallTextureOverrideHooks();

    // Version-gated world-detail hook. Failure is non-fatal.
    PrepareWorldCellDetailClassifyHook();
    ApplyWorldDetailDistanceScale(g_config.highDetailDistanceScale);
    ApplyWorldObjectActivationDistanceScale(g_config.objectActivationDistanceScale);
    if (PrepareWorldInteriorOcclusionFixBridge())
        ApplyWorldInteriorOcclusionFix(g_config.fixInteriorOcclusionBugs);

    if (g_earlyDirect3DCreate9HookInstalled.load(std::memory_order_acquire))
    {
        AppendLog("Direct3DCreate9 early IAT hook active.\n");
    }
    else
    {
        // Fallback for an unsupported/unexpected executable layout. This keeps
        // the previous behavior, but the supported build should always use the
        // synchronous IAT hook installed from DllMain.
        status = MH_CreateHook(
            reinterpret_cast<void*>(target),
            reinterpret_cast<void*>(&HookDirect3DCreate9),
            reinterpret_cast<void**>(&g_originalDirect3DCreate9)
        );

        if (status != MH_OK)
        {
            AppendLog("ERROR: Direct3DCreate9 MH_CreateHook fallback failed.\n");
            return 0;
        }

        status = MH_EnableHook(
            reinterpret_cast<void*>(target)
        );

        if (status != MH_OK)
        {
            AppendLog("ERROR: Direct3DCreate9 MH_EnableHook fallback failed.\n");
            return 0;
        }

        AppendLog("Direct3DCreate9 MinHook fallback installed.\n");
    }

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

        // Critical startup hook only: this is a build-gated IAT pointer swap
        // using VirtualProtect, with no logging/file I/O/MinHook work under the
        // loader lock. Everything else remains on InitializeHooks.
        InstallEarlyDirect3DCreate9IatHook();

        HANDLE thread = CreateThread(
            nullptr,
            0,
            InitializeHooks,
            nullptr,
            0,
            nullptr
        );

        if (thread != nullptr)
        {
            CloseHandle(thread);
        }
        else
        {
            // Never leave the early Direct3DCreate9 hook blocked forever if
            // worker creation fails. The D3D hook will fall through and log
            // its normal MinHook failure instead of deadlocking startup.
            g_initializationReady.store(true, std::memory_order_release);
        }
    }

    return TRUE;
}
