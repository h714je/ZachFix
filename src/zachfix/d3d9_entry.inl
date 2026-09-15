// -----------------------------------------------------------------------------
// IDirect3D9 hook
// -----------------------------------------------------------------------------

static HRESULT WINAPI HookCreateDevice(
    IDirect3D9* self,
    UINT adapter,
    D3DDEVTYPE deviceType,
    HWND focusWindow,
    DWORD behaviorFlags,
    D3DPRESENT_PARAMETERS* pp,
    IDirect3DDevice9** returnedDevice)
{
    AppendLog("IDirect3D9::CreateDevice intercepted.\n");

    if (pp != nullptr)
    {
        char text[1024] = {};

        sprintf_s(
            text,
            "CreateDevice:\n"
            "  Adapter                  = %u\n"
            "  DeviceType               = %u\n"
            "  BehaviorFlags            = 0x%08X\n"
            "  BackBufferWidth           = %u\n"
            "  BackBufferHeight          = %u\n"
            "  BackBufferFormat          = %u\n"
            "  BackBufferCount           = %u\n"
            "  MultiSampleType           = %u\n"
            "  MultiSampleQuality        = %u\n"
            "  SwapEffect                = %u\n"
            "  Windowed                  = %s\n"
            "  EnableAutoDepthStencil    = %s\n"
            "  AutoDepthStencilFormat    = %u\n"
            "  Flags                     = 0x%08X\n"
            "  RefreshRate               = %u\n"
            "  PresentationInterval      = 0x%08X\n",
            adapter,
            static_cast<unsigned>(deviceType),
            behaviorFlags,
            pp->BackBufferWidth,
            pp->BackBufferHeight,
            static_cast<unsigned>(pp->BackBufferFormat),
            pp->BackBufferCount,
            static_cast<unsigned>(pp->MultiSampleType),
            pp->MultiSampleQuality,
            static_cast<unsigned>(pp->SwapEffect),
            pp->Windowed ? "true" : "false",
            pp->EnableAutoDepthStencil ? "true" : "false",
            static_cast<unsigned>(pp->AutoDepthStencilFormat),
            pp->Flags,
            pp->FullScreen_RefreshRateInHz,
            pp->PresentationInterval
        );

        AppendLog(text);
    }

    HWND deviceWindow = focusWindow;

    if (pp != nullptr &&
        pp->hDeviceWindow != nullptr)
    {
        deviceWindow = pp->hDeviceWindow;
    }

    ResolveConfigForWindow(deviceWindow);

    if (pp != nullptr)
    {
        if (pp->Windowed)
        {
            ConfigureGameWindow(
                deviceWindow,
                g_displayWidth,
                g_displayHeight,
                g_config.borderless
            );

            // Windowed mode follows ZachFix window configuration. Zero
            // dimensions make D3D9 use the configured client area.
            pp->BackBufferWidth = 0;
            pp->BackBufferHeight = 0;
            pp->FullScreen_RefreshRateInHz = 0;

            char text[384] = {};
            sprintf_s(
                text,
                "[Window] Windowed CreateDevice selected: BackBuffer=%u x %u, "
                "Format=%u, Borderless=%s, RefreshRate=%u.\n",
                pp->BackBufferWidth,
                pp->BackBufferHeight,
                static_cast<unsigned>(pp->BackBufferFormat),
                g_config.borderless ? "true" : "false",
                pp->FullScreen_RefreshRateInHz
            );
            AppendLog(text);
        }
        else
        {
            // Preserve genuine D3D9 exclusive fullscreen. Display=0x0 has
            // already resolved to the current monitor size; explicit Display
            // values take priority. Borderless is intentionally ignored here.
            NormalizeExclusiveFullscreenPresentation(pp);

            char text[384] = {};
            sprintf_s(
                text,
                "[Window] Exclusive fullscreen CreateDevice selected: "
                "BackBuffer=%u x %u, Format=%u, RefreshRate=%u, "
                "Borderless ignored.\n",
                pp->BackBufferWidth,
                pp->BackBufferHeight,
                static_cast<unsigned>(pp->BackBufferFormat),
                pp->FullScreen_RefreshRateInHz
            );
            AppendLog(text);
        }
    }

    const HRESULT result = g_originalCreateDevice(
        self,
        adapter,
        deviceType,
        focusWindow,
        behaviorFlags,
        pp,
        returnedDevice
    );

    if (FAILED(result) ||
        returnedDevice == nullptr ||
        *returnedDevice == nullptr)
    {
        char text[128] = {};
        sprintf_s(
            text,
            "CreateDevice failed: HRESULT=0x%08X.\n",
            static_cast<unsigned>(result)
        );
        AppendLog(text);
        return result;
    }

    AppendLog("CreateDevice succeeded.\n");

    LogActivePresentation(*returnedDevice);
    LogBackBufferInfo(*returnedDevice);

    std::call_once(
        g_deviceHooksOnce,
        [returnedDevice]()
        {
            if (InstallDeviceHooks(*returnedDevice))
                AppendLog("All D3D9 hooks installed.\n");
            else
                AppendLog("ERROR: D3D9 hook installation failed.\n");
        }
    );

    if (!InitializeSettingsUi(deviceWindow, *returnedDevice))
        AppendLog("[UI] WARNING: In-game settings UI initialization failed.\n");

    return result;
}


static IDirect3D9* WINAPI HookDirect3DCreate9(UINT sdkVersion)
{
    AppendLog("Direct3DCreate9 intercepted.\n");

    IDirect3D9* d3d =
        g_originalDirect3DCreate9(sdkVersion);

    if (d3d == nullptr)
    {
        AppendLog("ERROR: Direct3DCreate9 returned nullptr.\n");
        return nullptr;
    }

    std::call_once(
        g_createDeviceHookOnce,
        [d3d]()
        {
            void** vtable =
                *reinterpret_cast<void***>(d3d);

            // IDirect3D9::CreateDevice = slot 16.
            void* target = vtable[16];

            MH_STATUS status = MH_CreateHook(
                target,
                reinterpret_cast<void*>(&HookCreateDevice),
                reinterpret_cast<void**>(&g_originalCreateDevice)
            );

            if (status != MH_OK)
            {
                AppendLog("ERROR: CreateDevice hook creation failed.\n");
                return;
            }

            status = MH_EnableHook(target);

            if (status != MH_OK)
            {
                AppendLog("ERROR: CreateDevice hook enable failed.\n");
                return;
            }

            AppendLog("IDirect3D9::CreateDevice hook installed.\n");
        }
    );

    return d3d;
}
