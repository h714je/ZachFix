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
    InitializeScreenshotPresetSystem();

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

    // Repair the confirmed Director's Cut HOUSE_LIST.NOD endian regression
    // before the first world CLevel objects are configured. The stock runtime
    // table is fully normalized; unknown/modded payloads keep a conservative
    // direct-first lookup fallback.
    InstallHouseListEndianFix();

    // Restore the original New Game Easy / Normal / Hard selector and the
    // game's native save-backed difficulty flow.
    InstallDifficultyRestoration();

    // Save I/O tracing and transactional protection keep DP's vanilla
    // savedata\dp.sav path. Difficulty remains part of the native save record.
    const bool saveDiagReady = InstallSaveDiagHooks();

    // Vanilla stability fix: DP can produce a zero-delta frame, and one actor
    // speed path performs 0/0 when the actor also did not move. The resulting
    // NaN reaches a deliberate infinite-loop sentinel. Build/signature gated
    // and deliberately limited to that exact 0/0 case.
    InstallVanillaZeroDeltaNaNFix();

    // Optional native XInput backend. DP keeps its vanilla controller action
    // and binding logic; ZachFix supplies an XInput-backed JOYINFOEX view and
    // translates the legacy axis semantics at DP's common evaluator.
    const bool nativeXInputReady = InstallNativeXInputBackend();

    // Auto-switch around DP's own USEJOY byte. This deliberately
    // keeps the original keyboard/mouse and controller action paths intact;
    // only the active vanilla mode is changed in response to real device input.
    InstallInputModeAutoSwitch();

    // Original Xbox combat-only shoulder strafe. The PC states 09/0A and
    // animations survive intact; ZachFix restores only the missing ingress at
    // the Xbox-equivalent Player-update tail.
    const bool combatStrafeReady = PrepareCombatStrafeRestoration();
    if (combatStrafeReady)
        ApplyCombatStrafeRestoration(g_config.restoreCombatStrafe);

    // Texture override is independent of the D3D9 device hook surface.
    // Failure is non-fatal: rendering fixes must still start normally.
    InstallTextureOverrideHooks();

    // Version-gated world-detail hook. Failure is non-fatal.
    const bool worldDetailReady = PrepareWorldCellDetailClassifyHook();
    const bool worldDetailApplied =
        ApplyWorldDetailDistanceScale(g_config.highDetailDistanceScale);
    ApplyWorldMainFrustumDistanceMode(g_config.mainFrustumDistanceMode);
    ApplyWorldObjectActivationDistanceScale(g_config.objectActivationDistanceScale);
    ApplyWorldObjectLodDistanceScale(g_config.objectLodDistanceScale);

    // Alternate low-detail 3D representation distance. Scale 1 is fully
    // native and installs no hook; scales 2..4 lazily extend the native
    // residency target requests while preserving DP's streaming/swap path.
    const bool alternate3dApplied =
        ApplyWorldAlternate3DDistanceScale(g_config.alternate3dDistanceScale);
    const bool interiorOcclusionReady = PrepareWorldInteriorOcclusionFixBridge();
    const bool interiorOcclusionApplied = interiorOcclusionReady &&
        ApplyWorldInteriorOcclusionFix(g_config.fixInteriorOcclusionBugs);

    // Final startup state snapshot. The config line records what the user
    // requested; these lines report what survived validation/installation and
    // which runtime path is actually selected.
    {
        const bool xinputAvailable =
            nativeXInputReady && IsNativeXInputBackendAvailable();
        const bool analogAvailable =
            xinputAvailable && IsAnalogVehicleTriggerPatchAvailable();
        const bool vibrationAvailable =
            xinputAvailable && IsNativeVibrationAvailable();
        const bool detailAvailable =
            g_config.highDetailDistanceScale == 1 ||
            (worldDetailReady && IsWorldDetailExtensionAvailable());
        const bool detailActive =
            worldDetailApplied &&
            GetWorldDetailDistanceScale() == g_config.highDetailDistanceScale;
        const bool alternateAvailable =
            g_config.alternate3dDistanceScale == 1 ||
            IsWorldAlternate3DExtensionAvailable();
        const bool alternateActive =
            alternate3dApplied &&
            GetWorldAlternate3DDistanceScale() ==
                g_config.alternate3dDistanceScale;
        const bool occlusionAvailable =
            interiorOcclusionReady && IsWorldInteriorOcclusionFixAvailable();
        const bool occlusionActive =
            interiorOcclusionApplied && IsWorldInteriorOcclusionFixActive();
        const bool saveAvailable =
            saveDiagReady && IsSaveSafetyAvailable();

        char statusText[1024] = {};
        sprintf_s(
            statusText,
            "[Status] NativeXInput requested=%s available=%s active=%s; "
            "AnalogVehicleTriggers requested=%s available=%s active=%s; "
            "Vibration requested=%s available=%s active=%s.\n",
            g_config.nativeXInputEnabled ? "true" : "false",
            xinputAvailable ? "true" : "false",
            (g_config.nativeXInputEnabled && xinputAvailable) ? "true" : "false",
            g_config.analogVehicleTriggers ? "true" : "false",
            analogAvailable ? "true" : "false",
            (g_config.analogVehicleTriggers && analogAvailable) ? "true" : "false",
            g_config.vibrationEnabled ? "true" : "false",
            vibrationAvailable ? "true" : "false",
            (g_config.vibrationEnabled && vibrationAvailable) ? "true" : "false");
        AppendLog(statusText);

        sprintf_s(
            statusText,
            "[Status] RestoreCombatStrafe requested=%s available=%s active=%s.\n",
            g_config.restoreCombatStrafe ? "true" : "false",
            combatStrafeReady && IsCombatStrafeRestorationAvailable() ? "true" : "false",
            IsCombatStrafeRestorationActive() ? "true" : "false");
        AppendLog(statusText);

        sprintf_s(
            statusText,
            "[Status] HighDetailDistanceScale requested=%u available=%s active=%s actual=%u; "
            "Alternate3DDistanceScale requested=%u available=%s active=%s actual=%u.\n",
            g_config.highDetailDistanceScale,
            detailAvailable ? "true" : "false",
            detailActive ? "true" : "false",
            GetWorldDetailDistanceScale(),
            g_config.alternate3dDistanceScale,
            alternateAvailable ? "true" : "false",
            alternateActive ? "true" : "false",
            GetWorldAlternate3DDistanceScale());
        AppendLog(statusText);

        sprintf_s(
            statusText,
            "[Status] InteriorOcclusionFix requested=%s available=%s active=%s; "
            "SaveSafety requested=%s available=%s active=%s.\n",
            g_config.fixInteriorOcclusionBugs ? "true" : "false",
            occlusionAvailable ? "true" : "false",
            occlusionActive ? "true" : "false",
            g_config.saveSafetyEnabled ? "true" : "false",
            saveAvailable ? "true" : "false",
            (g_config.saveSafetyEnabled && saveAvailable) ? "true" : "false");
        AppendLog(statusText);
    }

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
            MH_RemoveHook(reinterpret_cast<void*>(target));
            g_originalDirect3DCreate9 = nullptr;
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
