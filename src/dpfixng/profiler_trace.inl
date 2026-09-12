static constexpr uintptr_t kObjectFactoryRva = 0x001E7620;
static constexpr uintptr_t kType2EEventThunkRva = 0x00074DB8;
static constexpr uintptr_t kLodOwnerDestructorRva = 0x001F3FB0;
static constexpr uintptr_t kMarkForUnloadRva = 0x002BAB20;
static constexpr uintptr_t kEnsureRenderResourceRva = 0x00005E40;
static constexpr int kTrackedObjectType = 0xB7;

static UINT LifecycleCallerRva(uintptr_t address)
{
    if (!g_mainExeInfoValid || address < g_mainExeBase ||
        address >= g_mainExeBase + g_mainExeSize)
    {
        return 0;
    }

    return static_cast<UINT>(address - g_mainExeBase);
}

static void RecordLifecycleEvent(
    ProfilerLifecycleKind kind,
    void* ownerPtr,
    uintptr_t caller,
    uintptr_t auxPointer = 0,
    uintptr_t result = 0,
    WORD eventCategory = 0,
    BYTE eventCode = 0)
{
    if (!g_profilerContinuousActive.load(std::memory_order_relaxed) ||
        ownerPtr == nullptr ||
        g_profilerLifecycleEvents.size() >= 4096)
    {
        return;
    }

    ProfilerLifecycleEvent e{};
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    e.ticks = now.QuadPart;
    e.kind = kind;
    e.owner = reinterpret_cast<uintptr_t>(ownerPtr);
    e.caller = caller;
    e.callerRva = LifecycleCallerRva(caller);
    e.auxPointer = auxPointer;
    e.result = result;

    const unsigned char* bytes =
        reinterpret_cast<const unsigned char*>(ownerPtr);

    e.classField458 =
        *reinterpret_cast<const WORD*>(bytes + 0x458);
    e.classField45A = *(bytes + 0x45A);
    e.state29 = *(bytes + 0x29);
    e.state2A = *(bytes + 0x2A);
    e.objectCategory2C =
        *reinterpret_cast<const WORD*>(bytes + 0x2C);
    e.objectType30 =
        *reinterpret_cast<const WORD*>(bytes + 0x30);
    e.resourceKey144 =
        *reinterpret_cast<const UINT*>(bytes + 0x144);
    e.streamFlags434 =
        *reinterpret_cast<const UINT*>(bytes + 0x434);
    e.eventCategory = eventCategory;
    e.eventCode = eventCode;
    e.flagsD8 =
        static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0xD8)) |
        (static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0xDC)) << 32);
    e.flags138 =
        static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0x138)) |
        (static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0x13C)) << 32);
    e.rawRenderObject =
        *reinterpret_cast<const uintptr_t*>(bytes + 0x148);

    g_profilerLifecycleEvents.push_back(e);
}

// DP.exe+0x001E7620 is a member factory: ECX is the manager and the two
// explicit stack arguments are objectType/createDesc. Type 0xB7 selects
// the 0x0077D064 class (constructor DP.exe+0x001F3F80).
static void* __fastcall HookObjectFactory(
    void* self,
    void*,
    int objectType,
    void* createDesc)
{
    const uintptr_t caller =
        reinterpret_cast<uintptr_t>(_ReturnAddress());

    void* result =
        g_originalObjectFactory(self, objectType, createDesc);

    if ((objectType & 0xFF) == kTrackedObjectType && result != nullptr)
    {
        RecordLifecycleEvent(
            ProfilerLifecycleKind::Create,
            result,
            caller,
            reinterpret_cast<uintptr_t>(createDesc),
            static_cast<uintptr_t>(objectType & 0xFF));
    }

    return result;
}

// DP.exe+0x00074DB8 is the category-0x2E dispatch thunk. DP.exe+0x00074830
// reaches it with a tail jump, so _ReturnAddress() here is the real upstream
// caller of the category dispatcher, not the intermediate 0x5C98FD site.
// Event 3 is the path that eventually marks this class for removal.
static void __cdecl HookType2EEventThunk(
    int category,
    void* owner,
    int eventCode,
    void* payload)
{
    const uintptr_t caller =
        reinterpret_cast<uintptr_t>(_ReturnAddress());

    if (g_profilerContinuousActive.load(std::memory_order_relaxed) &&
        owner != nullptr &&
        (eventCode & 0xFF) == 3)
    {
        const uintptr_t vtable =
            *reinterpret_cast<const uintptr_t*>(owner);

        if (vtable == 0x0077D064)
        {
            RecordLifecycleEvent(
                ProfilerLifecycleKind::StreamEvent3,
                owner,
                caller,
                reinterpret_cast<uintptr_t>(payload),
                0,
                static_cast<WORD>(category & 0xFFFF),
                static_cast<BYTE>(eventCode & 0xFF));
        }
    }

    g_originalType2EEventThunk(
        category,
        owner,
        eventCode,
        payload);
}

static void* __fastcall HookLodOwnerDestructor(
    void* self,
    void*,
    unsigned int deleteFlags)
{
    const uintptr_t caller =
        reinterpret_cast<uintptr_t>(_ReturnAddress());

    RecordLifecycleEvent(
        ProfilerLifecycleKind::Destroy,
        self,
        caller,
        static_cast<uintptr_t>(deleteFlags));

    return g_originalLodOwnerDestructor(self, deleteFlags);
}

// vtable+0x30 of 0x0077D064. The original function sets owner[0x29] = 0x80.
static void __fastcall HookMarkForUnload(
    void* self,
    void*)
{
    const uintptr_t caller =
        reinterpret_cast<uintptr_t>(_ReturnAddress());

    RecordLifecycleEvent(
        ProfilerLifecycleKind::MarkUnload,
        self,
        caller);

    g_originalMarkForUnload(self);
}

// vtable+0x44 of 0x0077D064. It can allocate/reassign owner+0x148.
// Only record actual pointer transitions, not every per-frame validation.
static void __fastcall HookEnsureRenderResource(
    void* self,
    void*,
    void* sceneContext)
{
    if (self == nullptr)
    {
        g_originalEnsureRenderResource(self, sceneContext);
        return;
    }

    const uintptr_t beforeRaw =
        *reinterpret_cast<const uintptr_t*>(
            reinterpret_cast<const unsigned char*>(self) + 0x148);

    g_originalEnsureRenderResource(self, sceneContext);

    const uintptr_t afterRaw =
        *reinterpret_cast<const uintptr_t*>(
            reinterpret_cast<const unsigned char*>(self) + 0x148);

    if (beforeRaw != afterRaw)
    {
        const uintptr_t caller =
            reinterpret_cast<uintptr_t>(_ReturnAddress());

        RecordLifecycleEvent(
            ProfilerLifecycleKind::ResourceAttach,
            self,
            caller,
            beforeRaw,
            afterRaw);
    }
}

static bool PrepareLifecycleTraceHooks()
{
    if (!g_config.profilerEnabled ||
        !g_config.profilerSceneObjectTracing)
    {
        return true;
    }

    if (!InitializeMainExeInfo())
    {
        AppendLog(
            "[Profiler] Streaming lifecycle tracing unavailable: DP.exe info failed.\n");
        return false;
    }

    if (g_mainExeSize != 0x010B5000 ||
        g_mainExeTimeDateStamp != 0x529721DC)
    {
        AppendLog(
            "[Profiler] Streaming lifecycle tracing skipped: unsupported DP.exe build.\n");
        return false;
    }

    struct HookSpec
    {
        uintptr_t rva;
        void* hook;
        void** original;
        void** target;
        bool* created;
        const unsigned char* signature;
        size_t signatureSize;
        const char* name;
    };

    static const unsigned char factorySignature[] = {
        0x6A, 0xFF, 0x68, 0x64, 0x78, 0x76, 0x00
    };
    static const unsigned char type2EEventThunkSignature[] = {
        0x8B, 0x44, 0x24, 0x10,
        0x8B, 0x4C, 0x24, 0x0C,
        0x8B, 0x54, 0x24, 0x08
    };
    static const unsigned char destructorSignature[] = {
        0x56, 0x8B, 0xF1, 0xC7, 0x06, 0x64, 0xD0, 0x77, 0x00
    };
    static const unsigned char markUnloadSignature[] = {
        0x55, 0x8B, 0xEC, 0x51, 0x89, 0x4D, 0xFC
    };
    static const unsigned char ensureResourceSignature[] = {
        0x56, 0x8B, 0xF1, 0x8B, 0x86, 0x60, 0x01, 0x00, 0x00, 0x57
    };

    HookSpec specs[] = {
        {
            kObjectFactoryRva,
            reinterpret_cast<void*>(&HookObjectFactory),
            reinterpret_cast<void**>(&g_originalObjectFactory),
            &g_objectFactoryTraceTarget,
            &g_objectFactoryTraceHookCreated,
            factorySignature,
            sizeof(factorySignature),
            "type-0xB7 factory (__thiscall)"
        },
        {
            kType2EEventThunkRva,
            reinterpret_cast<void*>(&HookType2EEventThunk),
            reinterpret_cast<void**>(&g_originalType2EEventThunk),
            &g_type2EEventThunkTraceTarget,
            &g_type2EEventThunkTraceHookCreated,
            type2EEventThunkSignature,
            sizeof(type2EEventThunkSignature),
            "category-0x2E event source"
        },
        {
            kLodOwnerDestructorRva,
            reinterpret_cast<void*>(&HookLodOwnerDestructor),
            reinterpret_cast<void**>(&g_originalLodOwnerDestructor),
            &g_lodOwnerDestructorTraceTarget,
            &g_lodOwnerDestructorTraceHookCreated,
            destructorSignature,
            sizeof(destructorSignature),
            "type-0xB7 destructor"
        },
        {
            kMarkForUnloadRva,
            reinterpret_cast<void*>(&HookMarkForUnload),
            reinterpret_cast<void**>(&g_originalMarkForUnload),
            &g_markForUnloadTraceTarget,
            &g_markForUnloadTraceHookCreated,
            markUnloadSignature,
            sizeof(markUnloadSignature),
            "type-0xB7 mark-unload"
        },
        {
            kEnsureRenderResourceRva,
            reinterpret_cast<void*>(&HookEnsureRenderResource),
            reinterpret_cast<void**>(&g_originalEnsureRenderResource),
            &g_ensureRenderResourceTraceTarget,
            &g_ensureRenderResourceTraceHookCreated,
            ensureResourceSignature,
            sizeof(ensureResourceSignature),
            "type-0xB7 resource-attach"
        }
    };

    bool ok = true;

    for (HookSpec& spec : specs)
    {
        unsigned char* target =
            reinterpret_cast<unsigned char*>(g_mainExeBase + spec.rva);

        if (memcmp(target, spec.signature, spec.signatureSize) != 0)
        {
            const std::string message =
                std::string("[Profiler] Streaming lifecycle hook signature mismatch: ") +
                spec.name + "\n";
            AppendLog(message.c_str());
            ok = false;
            continue;
        }

        const MH_STATUS status =
            MH_CreateHook(target, spec.hook, spec.original);

        if (status != MH_OK &&
            status != MH_ERROR_ALREADY_CREATED)
        {
            const std::string message =
                std::string("[Profiler] Streaming lifecycle MH_CreateHook failed: ") +
                spec.name + "\n";
            AppendLog(message.c_str());
            ok = false;
            continue;
        }

        *spec.target = target;
        *spec.created = true;
    }

    if (ok)
    {
        AppendLog(
            "[Profiler] Type-0xB7 streaming hooks prepared: "
            "factory + event3-source + mark-unload + destructor + resource-attach.\n");
    }

    return ok;
}

static void EnableLifecycleTraceHooksForCapture()
{
    struct HookState
    {
        void* target;
        bool created;
        bool* enabled;
    };

    HookState hooks[] = {
        {g_objectFactoryTraceTarget, g_objectFactoryTraceHookCreated,
         &g_objectFactoryTraceHookEnabled},
        {g_type2EEventThunkTraceTarget, g_type2EEventThunkTraceHookCreated,
         &g_type2EEventThunkTraceHookEnabled},
        {g_lodOwnerDestructorTraceTarget, g_lodOwnerDestructorTraceHookCreated,
         &g_lodOwnerDestructorTraceHookEnabled},
        {g_markForUnloadTraceTarget, g_markForUnloadTraceHookCreated,
         &g_markForUnloadTraceHookEnabled},
        {g_ensureRenderResourceTraceTarget, g_ensureRenderResourceTraceHookCreated,
         &g_ensureRenderResourceTraceHookEnabled}
    };

    for (HookState& hook : hooks)
    {
        if (!hook.created || *hook.enabled)
            continue;

        const MH_STATUS status = MH_EnableHook(hook.target);
        if (status == MH_OK || status == MH_ERROR_ENABLED)
            *hook.enabled = true;
    }
}

static void DisableLifecycleTraceHooksAfterCapture()
{
    struct HookState
    {
        void* target;
        bool* enabled;
    };

    HookState hooks[] = {
        {g_objectFactoryTraceTarget, &g_objectFactoryTraceHookEnabled},
        {g_type2EEventThunkTraceTarget, &g_type2EEventThunkTraceHookEnabled},
        {g_lodOwnerDestructorTraceTarget, &g_lodOwnerDestructorTraceHookEnabled},
        {g_markForUnloadTraceTarget, &g_markForUnloadTraceHookEnabled},
        {g_ensureRenderResourceTraceTarget, &g_ensureRenderResourceTraceHookEnabled}
    };

    for (HookState& hook : hooks)
    {
        if (!*hook.enabled)
            continue;

        MH_DisableHook(hook.target);
        *hook.enabled = false;
    }
}

static constexpr uintptr_t kCandidateVisibilityRva = 0x002BD320;
static constexpr UINT kCandidateVisibilitySceneReturnRva = 0x002D2F0E;
static constexpr uintptr_t kLodOwnerVtable = 0x0077D064;

static BYTE __fastcall HookCandidateVisibility(
    void* self,
    void*,
    void* sceneContext)
{
    const uintptr_t returnAddress =
        reinterpret_cast<uintptr_t>(_ReturnAddress());

    const BYTE result =
        g_originalCandidateVisibility(self, sceneContext);

    if (!ProfilerFocusedTraceActive() ||
        !g_mainExeInfoValid ||
        self == nullptr)
    {
        return result;
    }

    if (returnAddress < g_mainExeBase ||
        returnAddress >= g_mainExeBase + g_mainExeSize)
    {
        return result;
    }

    const UINT callerRva =
        static_cast<UINT>(returnAddress - g_mainExeBase);

    if (callerRva != kCandidateVisibilitySceneReturnRva ||
        g_profilerCandidateOwnerCalls.size() >= 4096)
    {
        return result;
    }

    const unsigned char* bytes =
        reinterpret_cast<const unsigned char*>(self);

    ProfilerCandidateOwnerCall record{};
    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    record.ticks = now.QuadPart;
    record.owner = reinterpret_cast<uintptr_t>(self);
    record.callerRva = callerRva;
    record.result = result;
    record.vtable =
        *reinterpret_cast<const uintptr_t*>(bytes + 0x00);
    record.type = *(bytes + 0x28);
    record.disableFlags = *(bytes + 0x38);
    record.flagsD8 =
        static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0xD8)) |
        (static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0xDC)) << 32);
    record.flags138 =
        static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0x138)) |
        (static_cast<unsigned long long>(
            *reinterpret_cast<const UINT*>(bytes + 0x13C)) << 32);
    record.rawRenderObject =
        *reinterpret_cast<const uintptr_t*>(bytes + 0x148);

    // The vtable 0x0077D064 class is the one that contains the observed
    // low/high LOD owners. Its constructor initializes 0x458/0x45A, so these
    // fields are safe to sample only for that class.
    if (record.vtable == kLodOwnerVtable)
    {
        record.classField458 =
            *reinterpret_cast<const WORD*>(bytes + 0x458);
        record.classField45A = *(bytes + 0x45A);
    }

    g_profilerCandidateOwnerCalls.push_back(record);

    return result;
}


static bool PrepareCandidateVisibilityTraceHook()
{
    if (!g_config.profilerEnabled ||
        !g_config.profilerSceneObjectTracing)
    {
        return true;
    }

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Profiler] Candidate-owner tracing unavailable: DP.exe info failed.\n");
        return false;
    }

    if (g_mainExeSize != 0x010B5000 ||
        g_mainExeTimeDateStamp != 0x529721DC)
    {
        AppendLog("[Profiler] Candidate-owner tracing skipped: unsupported DP.exe build.\n");
        return false;
    }

    unsigned char* target =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + kCandidateVisibilityRva
        );

    static const unsigned char expectedPrologue[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x1C, 0x89, 0x4D, 0xFC
    };

    if (memcmp(target, expectedPrologue, sizeof(expectedPrologue)) != 0)
    {
        AppendLog("[Profiler] Candidate-owner tracing skipped: signature mismatch.\n");
        return false;
    }

    const MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookCandidateVisibility),
        reinterpret_cast<void**>(&g_originalCandidateVisibility)
    );

    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        AppendLog("[Profiler] Candidate-owner tracing MH_CreateHook failed.\n");
        return false;
    }

    g_candidateVisibilityTraceTarget = target;
    g_candidateVisibilityTraceHookCreated = true;

    AppendLog(
        "[Profiler] Candidate-owner gate probe prepared at DP.exe+0x002BD320 "
        "(scene caller DP.exe+0x002D2F0E).\n"
    );

    return true;
}


static void EnableCandidateVisibilityTraceHookForCapture()
{
    if (!g_candidateVisibilityTraceHookCreated ||
        g_candidateVisibilityTraceHookEnabled)
    {
        return;
    }

    const MH_STATUS status =
        MH_EnableHook(g_candidateVisibilityTraceTarget);

    if (status == MH_OK || status == MH_ERROR_ENABLED)
    {
        g_candidateVisibilityTraceHookEnabled = true;
    }
}


static void DisableCandidateVisibilityTraceHookAfterCapture()
{
    if (!g_candidateVisibilityTraceHookCreated ||
        !g_candidateVisibilityTraceHookEnabled)
    {
        return;
    }

    MH_DisableHook(g_candidateVisibilityTraceTarget);
    g_candidateVisibilityTraceHookEnabled = false;
}


static constexpr uintptr_t kGetRenderObjectRva = 0x002E1150;

static void* __fastcall HookGetRenderObject(
    void* self,
    void*)
{
    const uintptr_t returnAddress =
        reinterpret_cast<uintptr_t>(_ReturnAddress());

    void* result = g_originalGetRenderObject(self);

    if (!ProfilerFocusedTraceActive() ||
        !g_mainExeInfoValid)
    {
        return result;
    }

    if (returnAddress < g_mainExeBase ||
        returnAddress >= g_mainExeBase + g_mainExeSize)
    {
        return result;
    }

    const UINT callerRva =
        static_cast<UINT>(returnAddress - g_mainExeBase);

    // Calls made while the main scene renderer is deciding whether an owner
    // contributes a render object. Recording null results is important: it
    // distinguishes "candidate existed but failed a gate" from "candidate was
    // never presented to the renderer at all".
    const bool sceneSelectionCall =
        callerRva == 0x002D2F5F ||
        callerRva == 0x002D2F85 ||
        callerRva == 0x002D2FD8 ||
        callerRva == 0x002D3001 ||
        callerRva == 0x002D3237 ||
        callerRva == 0x002D3246;

    if (sceneSelectionCall && self != nullptr &&
        g_profilerSceneOwnerCalls.size() < 4096)
    {
        const unsigned char* bytes =
            reinterpret_cast<const unsigned char*>(self);

        ProfilerSceneOwnerCall record{};
        LARGE_INTEGER now{};
        QueryPerformanceCounter(&now);
        record.ticks = now.QuadPart;
        record.owner = reinterpret_cast<uintptr_t>(self);
        record.result = reinterpret_cast<uintptr_t>(result);
        record.callerRva = callerRva;
        record.vtable = *reinterpret_cast<const uintptr_t*>(bytes + 0x00);
        record.type = *(bytes + 0x28);
        record.disableFlags = *(bytes + 0x38);
        record.flagsD8 =
            static_cast<unsigned long long>(
                *reinterpret_cast<const UINT*>(bytes + 0xD8)) |
            (static_cast<unsigned long long>(
                *reinterpret_cast<const UINT*>(bytes + 0xDC)) << 32);
        record.flags138 =
            static_cast<unsigned long long>(
                *reinterpret_cast<const UINT*>(bytes + 0x138)) |
            (static_cast<unsigned long long>(
                *reinterpret_cast<const UINT*>(bytes + 0x13C)) << 32);
        record.rawRenderObject =
            *reinterpret_cast<const uintptr_t*>(bytes + 0x148);

        g_profilerSceneOwnerCalls.push_back(record);
    }

    // These are the scene-list insertion call sites immediately before
    // renderer->0x63A8 receives the render object.
    if (result != nullptr &&
        (callerRva == 0x002D3001 ||
         callerRva == 0x002D3237 ||
         callerRva == 0x002D3246))
    {
        g_profilerSceneObjectOwners[
            reinterpret_cast<uintptr_t>(result)
        ] = reinterpret_cast<uintptr_t>(self);
    }

    return result;
}


static bool PrepareSceneOwnerTraceHook()
{
    if (!g_config.profilerEnabled ||
        !g_config.profilerSceneObjectTracing)
    {
        return true;
    }

    if (!InitializeMainExeInfo())
    {
        return false;
    }

    if (g_mainExeSize != 0x010B5000 ||
        g_mainExeTimeDateStamp != 0x529721DC)
    {
        AppendLog("[Profiler] Scene-owner tracing skipped: unsupported DP.exe build.\n");
        return false;
    }

    unsigned char* target =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + kGetRenderObjectRva
        );

    static const unsigned char expectedPrologue[] = {
        0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x0C, 0x89, 0x4D, 0xFC
    };

    if (memcmp(target, expectedPrologue, sizeof(expectedPrologue)) != 0)
    {
        AppendLog("[Profiler] Scene-owner tracing skipped: signature mismatch.\n");
        return false;
    }

    const MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookGetRenderObject),
        reinterpret_cast<void**>(&g_originalGetRenderObject)
    );

    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        AppendLog("[Profiler] Scene-owner tracing MH_CreateHook failed.\n");
        return false;
    }

    g_sceneOwnerTraceTarget = target;
    g_sceneOwnerTraceHookCreated = true;

    AppendLog(
        "[Profiler] Scene-owner probe prepared at DP.exe+0x002E1150.\n"
    );

    return true;
}


static void EnableSceneOwnerTraceHookForCapture()
{
    if (!g_sceneOwnerTraceHookCreated ||
        g_sceneOwnerTraceHookEnabled)
    {
        return;
    }

    const MH_STATUS status = MH_EnableHook(g_sceneOwnerTraceTarget);

    if (status == MH_OK || status == MH_ERROR_ENABLED)
    {
        g_sceneOwnerTraceHookEnabled = true;
        g_profilerSceneObjectOwners.clear();
        g_profilerSceneOwner = 0;
    }
}


static void DisableSceneOwnerTraceHookAfterCapture()
{
    if (!g_sceneOwnerTraceHookCreated ||
        !g_sceneOwnerTraceHookEnabled)
    {
        return;
    }

    MH_DisableHook(g_sceneOwnerTraceTarget);
    g_sceneOwnerTraceHookEnabled = false;
    g_profilerSceneOwner = 0;
}


static bool PrepareSceneObjectTraceHook()
{
    if (!g_config.profilerEnabled ||
        !g_config.profilerSceneObjectTracing)
    {
        return true;
    }

    if (!InitializeMainExeInfo())
    {
        AppendLog("[Profiler] Scene-object tracing unavailable: DP.exe info failed.\n");
        return false;
    }

    if (g_mainExeSize != 0x010B5000 ||
        g_mainExeTimeDateStamp != 0x529721DC)
    {
        AppendLog("[Profiler] Scene-object tracing skipped: unsupported DP.exe build.\n");
        return false;
    }

    unsigned char* target =
        reinterpret_cast<unsigned char*>(
            g_mainExeBase + kRenderSceneObjectRva
        );

    static const unsigned char expectedPrologue[] = {
        0x55, 0x8B, 0xEC, 0x6A, 0xFF, 0x68, 0x0D, 0x8C, 0x76, 0x00
    };

    if (memcmp(target, expectedPrologue, sizeof(expectedPrologue)) != 0)
    {
        AppendLog("[Profiler] Scene-object tracing skipped: signature mismatch.\n");
        return false;
    }

    const MH_STATUS status = MH_CreateHook(
        target,
        reinterpret_cast<void*>(&HookRenderSceneObject),
        reinterpret_cast<void**>(&g_originalRenderSceneObject)
    );

    if (status != MH_OK && status != MH_ERROR_ALREADY_CREATED)
    {
        AppendLog("[Profiler] Scene-object tracing MH_CreateHook failed.\n");
        return false;
    }

    g_sceneObjectTraceTarget = target;
    g_sceneObjectTraceHookCreated = true;

    AppendLog(
        "[Profiler] Scene-object attribution probe prepared at DP.exe+0x002D69E0. "
        "It will be enabled only during F11 capture.\n"
    );

    return true;
}


static void EnableSceneObjectTraceHookForCapture()
{
    if (!g_sceneObjectTraceHookCreated ||
        g_sceneObjectTraceHookEnabled)
    {
        return;
    }

    const MH_STATUS status = MH_EnableHook(g_sceneObjectTraceTarget);

    if (status == MH_OK || status == MH_ERROR_ENABLED)
    {
        g_sceneObjectTraceHookEnabled = true;
        g_profilerSceneObject = 0;
        g_profilerSceneOwner = 0;
        g_profilerSceneObjectCallerRva = 0;
    }
}


static void DisableSceneObjectTraceHookAfterCapture()
{
    if (!g_sceneObjectTraceHookCreated ||
        !g_sceneObjectTraceHookEnabled)
    {
        return;
    }

    MH_DisableHook(g_sceneObjectTraceTarget);
    g_sceneObjectTraceHookEnabled = false;
    g_profilerSceneObject = 0;
    g_profilerSceneOwner = 0;
    g_profilerSceneObjectCallerRva = 0;
}


static UINT ProfilerCallerRva(
    uintptr_t address,
    bool& inMainExe)
{
    inMainExe = false;

    if (!g_mainExeInfoValid ||
        address < g_mainExeBase ||
        address >=
            g_mainExeBase + g_mainExeSize)
    {
        return 0;
    }

    inMainExe = true;

    return static_cast<UINT>(
        address - g_mainExeBase
    );
}


static void ProfilerRecordInternal(
    ProfilerEventType type,
    uintptr_t callerAddress,
    unsigned long long v0 = 0,
    unsigned long long v1 = 0,
    unsigned long long v2 = 0,
    unsigned long long v3 = 0,
    unsigned long long v4 = 0,
    unsigned long long v5 = 0,
    unsigned long long v6 = 0,
    unsigned long long v7 = 0)
{
    if (!g_profilerCaptureActive.load(
            std::memory_order_relaxed))
    {
        return;
    }

    if (g_profilerEvents.size() >=
        static_cast<size_t>(
            g_config.profilerMaxEvents))
    {
        g_profilerOverflow = true;
        return;
    }

    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    ProfilerEvent event{};
    event.type = type;
    event.ticks = now.QuadPart;
    event.v[0] = v0;
    event.v[1] = v1;
    event.v[2] = v2;
    event.v[3] = v3;
    event.v[4] = v4;
    event.v[5] = v5;
    event.v[6] = v6;
    event.v[7] = v7;

    if (callerAddress != 0)
    {
        event.callerAddress =
            callerAddress;

        event.callerRva =
            ProfilerCallerRva(
                callerAddress,
                event.callerInMainExe
            );

        event.passIndex =
            g_profilerOpenPass;

        event.vertexShaderId =
            g_profilerCurrentVs;

        event.pixelShaderId =
            g_profilerCurrentPs;
    }

    g_profilerEvents.push_back(event);
}


static void ProfilerRecord(
    ProfilerEventType type,
    unsigned long long v0 = 0,
    unsigned long long v1 = 0,
    unsigned long long v2 = 0,
    unsigned long long v3 = 0,
    unsigned long long v4 = 0,
    unsigned long long v5 = 0,
    unsigned long long v6 = 0,
    unsigned long long v7 = 0)
{
    ProfilerRecordInternal(
        type,
        0,
        v0,
        v1,
        v2,
        v3,
        v4,
        v5,
        v6,
        v7
    );
}


static void ProfilerRecordDraw(
    ProfilerEventType type,
    uintptr_t callerAddress,
    unsigned long long v0 = 0,
    unsigned long long v1 = 0,
    unsigned long long v2 = 0,
    unsigned long long v3 = 0,
    unsigned long long v4 = 0,
    unsigned long long v5 = 0,
    unsigned long long v6 = 0,
    unsigned long long v7 = 0)
{
    ProfilerRecordInternal(
        type,
        callerAddress,
        v0,
        v1,
        v2,
        v3,
        v4,
        v5,
        v6,
        v7
    );
}


static ProfilerSurfaceInfo MakeProfilerSurfaceInfo(
    IDirect3DSurface9* surface)
{
    surface = ResolveRuntimeLogicalSurface(surface);

    ProfilerSurfaceInfo info{};
    info.pointer = surface;

    if (surface == nullptr)
        return info;

    info.resourceId =
        FindProfilerResourceBySurface(surface);

    if (const ProfilerResourceInfo* resource =
            FindProfilerResource(info.resourceId))
    {
        info.tag = resource->tag;
    }

    D3DSURFACE_DESC desc = {};

    if (SUCCEEDED(surface->GetDesc(&desc)))
    {
        info.width = desc.Width;
        info.height = desc.Height;
        info.format =
            static_cast<UINT>(desc.Format);
        info.usage = desc.Usage;
    }

    return info;
}


static void GetSurfaceProfile(
    IDirect3DSurface9* surface,
    UINT& width,
    UINT& height,
    UINT& format,
    DWORD& usage)
{
    width = 0;
    height = 0;
    format = 0;
    usage = 0;

    if (surface == nullptr)
        return;

    D3DSURFACE_DESC desc = {};

    if (SUCCEEDED(surface->GetDesc(&desc)))
    {
        width = desc.Width;
        height = desc.Height;
        format = static_cast<UINT>(desc.Format);
        usage = desc.Usage;
    }
}


static void GetTextureProfile(
    IDirect3DBaseTexture9* baseTexture,
    UINT& width,
    UINT& height,
    UINT& format,
    DWORD& usage,
    UINT& resourceType)
{
    width = 0;
    height = 0;
    format = 0;
    usage = 0;
    resourceType = 0;

    if (baseTexture == nullptr)
        return;

    resourceType =
        static_cast<UINT>(baseTexture->GetType());

    if (baseTexture->GetType() == D3DRTYPE_TEXTURE)
    {
        IDirect3DTexture9* texture = nullptr;

        if (SUCCEEDED(baseTexture->QueryInterface(
                __uuidof(IDirect3DTexture9),
                reinterpret_cast<void**>(&texture))) &&
            texture != nullptr)
        {
            D3DSURFACE_DESC desc = {};

            if (SUCCEEDED(texture->GetLevelDesc(0, &desc)))
            {
                width = desc.Width;
                height = desc.Height;
                format = static_cast<UINT>(desc.Format);
                usage = desc.Usage;
            }

            texture->Release();
        }
    }
    else if (baseTexture->GetType() == D3DRTYPE_CUBETEXTURE)
    {
        IDirect3DCubeTexture9* texture = nullptr;

        if (SUCCEEDED(baseTexture->QueryInterface(
                __uuidof(IDirect3DCubeTexture9),
                reinterpret_cast<void**>(&texture))) &&
            texture != nullptr)
        {
            D3DSURFACE_DESC desc = {};

            if (SUCCEEDED(texture->GetLevelDesc(0, &desc)))
            {
                width = desc.Width;
                height = desc.Height;
                format = static_cast<UINT>(desc.Format);
                usage = desc.Usage;
            }

            texture->Release();
        }
    }
}


static UINT DecodeShaderRegisterType(
    DWORD parameterToken)
{
    constexpr DWORD kRegTypeMask1 = 0x70000000u;
    constexpr DWORD kRegTypeMask2 = 0x00001800u;

    return
        ((parameterToken & kRegTypeMask1) >> 28) |
        ((parameterToken & kRegTypeMask2) >> 8);
}


static UINT DecodeShaderRegisterNumber(
    DWORD parameterToken)
{
    constexpr DWORD kRegNumMask = 0x000007FFu;
    return parameterToken & kRegNumMask;
}


static void AnalyzeShaderSamplerUsage(
    const unsigned char* bytecode,
    size_t bytecodeSize,
    ProfilerShaderInfo& info)
{
    info.shaderMajor = 0;
    info.shaderMinor = 0;
    info.samplerMask = 0;
    info.declaredSamplerMask = 0;
    info.samplerMaskValid = false;

    if (bytecode == nullptr ||
        bytecodeSize < sizeof(DWORD) * 2 ||
        (bytecodeSize % sizeof(DWORD)) != 0)
    {
        return;
    }

    const DWORD* tokens =
        reinterpret_cast<const DWORD*>(bytecode);

    const size_t tokenCount =
        bytecodeSize / sizeof(DWORD);

    const DWORD versionToken = tokens[0];

    const WORD shaderType =
        static_cast<WORD>(versionToken >> 16);

    // 0xFFFF = pixel shader, 0xFFFE = vertex shader.
    if (shaderType != 0xFFFFu &&
        shaderType != 0xFFFEu)
    {
        return;
    }

    info.shaderMajor =
        (versionToken >> 8) & 0xFFu;
    info.shaderMinor =
        versionToken & 0xFFu;

    // SM2/SM3 instructions contain an explicit parameter count.
    // SM1.x uses a different encoding, so keep the conservative fallback.
    if (info.shaderMajor < 2 ||
        info.shaderMajor > 3)
    {
        return;
    }

    constexpr DWORD kOpcodeMask = 0x0000FFFFu;
    constexpr DWORD kInstructionLengthMask = 0x0F000000u;
    constexpr DWORD kInstructionLengthShift = 24;
    constexpr DWORD kCommentSizeMask = 0x7FFF0000u;
    constexpr DWORD kCommentSizeShift = 16;
    constexpr DWORD kParameterTokenBit = 0x80000000u;

    // D3DSPR_SAMPLER == 10.
    constexpr UINT kSamplerRegisterType = 10;

    size_t cursor = 1;
    bool sawEnd = false;

    while (cursor < tokenCount)
    {
        const DWORD instructionToken =
            tokens[cursor];

        const DWORD opcode =
            instructionToken & kOpcodeMask;

        if (opcode ==
            static_cast<DWORD>(D3DSIO_END))
        {
            sawEnd = true;
            break;
        }

        if (opcode ==
            static_cast<DWORD>(D3DSIO_COMMENT))
        {
            const size_t commentDwords =
                (instructionToken &
                    kCommentSizeMask) >>
                kCommentSizeShift;

            if (cursor + 1 + commentDwords >
                tokenCount)
            {
                return;
            }

            cursor += 1 + commentDwords;
            continue;
        }

        const size_t parameterCount =
            (instructionToken &
                kInstructionLengthMask) >>
            kInstructionLengthShift;

        if (cursor + 1 + parameterCount >
            tokenCount)
        {
            return;
        }

        const bool isDeclaration =
            opcode ==
                static_cast<DWORD>(D3DSIO_DCL);

        // DEF/DEFI/DEFB contain literal DWORDs. Do not interpret their
        // immediate data as register tokens.
        const bool hasInlineImmediateData =
            opcode ==
                static_cast<DWORD>(D3DSIO_DEF) ||
            opcode ==
                static_cast<DWORD>(D3DSIO_DEFI) ||
            opcode ==
                static_cast<DWORD>(D3DSIO_DEFB);

        if (!hasInlineImmediateData)
        {
            for (size_t i = 0;
                 i < parameterCount;
                 ++i)
            {
                const DWORD parameter =
                    tokens[cursor + 1 + i];

                if ((parameter &
                        kParameterTokenBit) == 0)
                {
                    continue;
                }

                if (DecodeShaderRegisterType(parameter) !=
                    kSamplerRegisterType)
                {
                    continue;
                }

                const UINT sampler =
                    DecodeShaderRegisterNumber(parameter);

                if (sampler >= 16)
                    continue;

                const UINT bit =
                    1u << sampler;

                if (isDeclaration)
                    info.declaredSamplerMask |= bit;
                else
                    info.samplerMask |= bit;
            }
        }

        cursor += 1 + parameterCount;
    }

    // Only trust the result if the complete token stream reached END.
    info.samplerMaskValid = sawEnd;
}


static void FormatSamplerMask(
    UINT mask,
    char* buffer,
    size_t bufferSize)
{
    if (buffer == nullptr ||
        bufferSize == 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (mask == 0)
    {
        strcpy_s(
            buffer,
            bufferSize,
            "-"
        );
        return;
    }

    bool first = true;

    for (UINT sampler = 0;
         sampler < 16;
         ++sampler)
    {
        if ((mask & (1u << sampler)) == 0)
            continue;

        char item[16] = {};

        sprintf_s(
            item,
            "s%u",
            sampler
        );

        if (!first)
        {
            strcat_s(
                buffer,
                bufferSize,
                ","
            );
        }

        strcat_s(
            buffer,
            bufferSize,
            item
        );

        first = false;
    }
}


static UINT RegisterVertexShaderForProfiler(
    IDirect3DVertexShader9* shader)
{
    if (shader == nullptr)
        return 0;

    for (const ProfilerShaderInfo& info : g_profilerShaders)
    {
        if (!info.pixel &&
            info.pointer == shader)
        {
            return info.id;
        }
    }

    ProfilerShaderInfo info{};
    info.id =
        static_cast<UINT>(g_profilerShaders.size() + 1);
    info.pixel = false;
    info.pointer = shader;

    UINT size = 0;

    if (SUCCEEDED(shader->GetFunction(nullptr, &size)) &&
        size > 0)
    {
        std::vector<unsigned char> bytes(size);

        if (SUCCEEDED(shader->GetFunction(
                bytes.data(),
                &size)))
        {
            bytes.resize(size);
            info.bytecodeSize = size;
            info.hash =
                Fnv1a64(
                    bytes.data(),
                    bytes.size()
                );

            AnalyzeShaderSamplerUsage(
                bytes.data(),
                bytes.size(),
                info
            );

            if (g_config.profilerDumpShaders)
                info.bytecode = std::move(bytes);
        }
    }

    g_profilerShaders.push_back(
        std::move(info)
    );

    return g_profilerShaders.back().id;
}


static UINT RegisterPixelShaderForProfiler(
    IDirect3DPixelShader9* shader)
{
    if (shader == nullptr)
        return 0;

    for (const ProfilerShaderInfo& info : g_profilerShaders)
    {
        if (info.pixel &&
            info.pointer == shader)
        {
            return info.id;
        }
    }

    ProfilerShaderInfo info{};
    info.id =
        static_cast<UINT>(g_profilerShaders.size() + 1);
    info.pixel = true;
    info.pointer = shader;

    UINT size = 0;

    if (SUCCEEDED(shader->GetFunction(nullptr, &size)) &&
        size > 0)
    {
        std::vector<unsigned char> bytes(size);

        if (SUCCEEDED(shader->GetFunction(
                bytes.data(),
                &size)))
        {
            bytes.resize(size);
            info.bytecodeSize = size;
            info.hash =
                Fnv1a64(
                    bytes.data(),
                    bytes.size()
                );

            AnalyzeShaderSamplerUsage(
                bytes.data(),
                bytes.size(),
                info
            );

            if (g_config.profilerDumpShaders)
                info.bytecode = std::move(bytes);
        }
    }

    g_profilerShaders.push_back(
        std::move(info)
    );

    return g_profilerShaders.back().id;
}


static void ProfilerRecordSurface(
    ProfilerEventType type,
    UINT slot,
    IDirect3DSurface9* surface)
{
    UINT width = 0;
    UINT height = 0;
    UINT format = 0;
    DWORD usage = 0;

    GetSurfaceProfile(
        surface,
        width,
        height,
        format,
        usage
    );

    const UINT resourceId =
        FindProfilerResourceBySurface(surface);

    ProfilerRecord(
        type,
        slot,
        reinterpret_cast<unsigned long long>(surface),
        width,
        height,
        format,
        usage,
        resourceId
    );
}


static void ProfilerRecordViewport(
    const D3DVIEWPORT9* viewport)
{
    if (viewport == nullptr)
        return;

    ProfilerRecord(
        ProfilerEventType::SetViewport,
        viewport->X,
        viewport->Y,
        viewport->Width,
        viewport->Height
    );
}


static void ProfilerSnapshotInitialState(
    IDirect3DDevice9* device)
{
    if (device == nullptr)
        return;

    ProfilerRecord(
        ProfilerEventType::FrameStart
    );

    for (DWORD index = 0; index < 4; ++index)
    {
        IDirect3DSurface9* surface = nullptr;

        if (SUCCEEDED(device->GetRenderTarget(
                index,
                &surface)) &&
            surface != nullptr)
        {
            ProfilerRecordSurface(
                ProfilerEventType::SetRenderTarget,
                index,
                surface
            );
            surface->Release();
        }
    }

    IDirect3DSurface9* depthStencil = nullptr;

    if (SUCCEEDED(device->GetDepthStencilSurface(
            &depthStencil)) &&
        depthStencil != nullptr)
    {
        ProfilerRecordSurface(
            ProfilerEventType::SetDepthStencilSurface,
            0,
            depthStencil
        );
        depthStencil->Release();
    }

    D3DVIEWPORT9 viewport = {};

    if (SUCCEEDED(device->GetViewport(&viewport)))
        ProfilerRecordViewport(&viewport);

    IDirect3DVertexShader9* vertexShader = nullptr;

    if (SUCCEEDED(device->GetVertexShader(
            &vertexShader)) &&
        vertexShader != nullptr)
    {
        const UINT id =
            RegisterVertexShaderForProfiler(
                vertexShader
            );

        ProfilerRecord(
            ProfilerEventType::SetVertexShader,
            id,
            reinterpret_cast<unsigned long long>(
                vertexShader
            )
        );

        g_profilerCurrentVs = id;

        vertexShader->Release();
    }

    IDirect3DPixelShader9* pixelShader = nullptr;

    if (SUCCEEDED(device->GetPixelShader(
            &pixelShader)) &&
        pixelShader != nullptr)
    {
        const UINT id =
            RegisterPixelShaderForProfiler(
                pixelShader
            );

        ProfilerRecord(
            ProfilerEventType::SetPixelShader,
            id,
            reinterpret_cast<unsigned long long>(
                pixelShader
            )
        );

        g_profilerCurrentPs = id;

        pixelShader->Release();
    }

    for (DWORD stage = 0; stage < 16; ++stage)
    {
        IDirect3DBaseTexture9* texture = nullptr;

        if (SUCCEEDED(device->GetTexture(
                stage,
                &texture)) &&
            texture != nullptr)
        {
            UINT width = 0;
            UINT height = 0;
            UINT format = 0;
            DWORD usage = 0;
            UINT resourceType = 0;

            GetTextureProfile(
                texture,
                width,
                height,
                format,
                usage,
                resourceType
            );

            const UINT resourceId =
                FindProfilerResourceByTexture(
                    texture
                );

            if (stage < 16)
                g_profilerCurrentTextureResources[stage] = resourceId;

            ProfilerRecord(
                ProfilerEventType::SetTexture,
                stage,
                reinterpret_cast<unsigned long long>(
                    texture
                ),
                width,
                height,
                format,
                usage,
                resourceType,
                resourceId
            );

            texture->Release();
        }
    }
}


static const ProfilerShaderInfo* FindProfilerShader(
    UINT id)
{
    if (id == 0)
        return nullptr;

    for (const ProfilerShaderInfo& shader :
         g_profilerShaders)
    {
        if (shader.id == id)
            return &shader;
    }

    return nullptr;
}


static bool GetCurrentPixelShaderSamplerMask(
    UINT& mask)
{
    mask = 0;

    // Fixed-function path has no shader bytecode to inspect.
    if (g_profilerCurrentPs == 0)
        return false;

    const ProfilerShaderInfo* shader =
        FindProfilerShader(
            g_profilerCurrentPs
        );

    if (shader == nullptr ||
        !shader->pixel ||
        !shader->samplerMaskValid)
    {
        return false;
    }

    mask = shader->samplerMask;
    return true;
}


static UINT FindDominantShader(
    const std::unordered_map<UINT, UINT>& counts)
{
    UINT bestId = 0;
    UINT bestCount = 0;

    for (const auto& entry : counts)
    {
        if (entry.second > bestCount)
        {
            bestId = entry.first;
            bestCount = entry.second;
        }
    }

    return bestId;
}




static const char* ClassifyProfilerPass(
    const ProfilerPass& pass)
{
    if (pass.rt0.pointer == nullptr)
        return "No RT0";

    switch (pass.rt0.tag)
    {
        case ProfilerResourceTag::BackBuffer:
            return "Backbuffer / UI";

        case ProfilerResourceTag::ShadowColor512:
            return "Shadow map 512-base";

        case ProfilerResourceTag::ShadowColor1024:
            return "Shadow map 1024-base";

        case ProfilerResourceTag::ReflectionSmallColor:
            return "Reflection SMALL";

        case ProfilerResourceTag::ReflectionLargeColor:
            return "Reflection LARGE";

        case ProfilerResourceTag::DofHdr448:
            return "DoF HDR";

        case ProfilerResourceTag::Storage448:
        case ProfilerResourceTag::Storage896:
            return "Scene storage / post-FX";

        case ProfilerResourceTag::ExposureChain:
            return "Exposure / luminance chain";

        case ProfilerResourceTag::PostFx224:
            return "Post-FX 224x126";

        case ProfilerResourceTag::PostFx256:
            return "Post-FX 256x256";

        case ProfilerResourceTag::HdrAux128:
            return "HDR auxiliary";

        case ProfilerResourceTag::MainHdr:
            if (pass.sawStage8FullRes)
                return "HDR main scene";

            if (pass.drawCalls <= 8)
                return "HDR fullscreen/post-FX";

            return "HDR scene";

        case ProfilerResourceTag::MainLdr:
            if (pass.sawMrt)
                return "G-buffer / MRT";

            if (pass.drawCalls <= 4)
                return "Fullscreen resolve";

            return "Main LDR scene/lighting";

        default:
            break;
    }

    if (pass.depth.tag ==
            ProfilerResourceTag::ShadowDepth512)
    {
        return "Shadow pass 512-base";
    }

    if (pass.depth.tag ==
            ProfilerResourceTag::ShadowDepth1024)
    {
        return "Shadow pass 1024-base";
    }

    if (pass.depth.pointer == nullptr &&
        pass.drawCalls <= 8)
    {
        return "Fullscreen post-FX candidate";
    }

    return "Scene / other";
}


static void ReleaseProfilerGpuQueries()
{
    auto releaseQuery =
        [](IDirect3DQuery9*& query)
        {
            if (query != nullptr)
            {
                query->Release();
                query = nullptr;
            }
        };

    releaseQuery(g_profilerGpuFreqQuery);
    releaseQuery(g_profilerGpuDisjointQuery);
    releaseQuery(g_profilerGpuFrameStartQuery);
    releaseQuery(g_profilerGpuFrameEndQuery);

    for (ProfilerPass& pass : g_profilerPasses)
    {
        releaseQuery(pass.gpuStartQuery);
        releaseQuery(pass.gpuEndQuery);
    }
}



static void IssueProfilerTimestamp(
    IDirect3DQuery9* query)
{
    if (query != nullptr)
        query->Issue(D3DISSUE_END);
}


static bool BeginProfilerGpuFrame(
    IDirect3DDevice9* device)
{
    g_profilerGpuRequested =
        g_config.profilerGpuTimings;

    g_profilerGpuSupported = false;
    g_profilerGpuValid = false;
    g_profilerGpuFrequency = 0;
    g_profilerGpuDisjoint = TRUE;
    g_profilerGpuFrameMs = 0.0;
    g_profilerGpuFrameStartTicks = 0;
    g_profilerGpuFrameEndTicks = 0;

    if (!g_profilerGpuRequested ||
        device == nullptr)
    {
        return false;
    }

    ReleaseProfilerGpuQueries();

    HRESULT result =
        device->CreateQuery(
            D3DQUERYTYPE_TIMESTAMPFREQ,
            &g_profilerGpuFreqQuery
        );

    if (FAILED(result) ||
        g_profilerGpuFreqQuery == nullptr)
    {
        AppendLog(
            "[Profiler] GPU timestamp frequency query unsupported. "
            "Continuing with CPU structural capture only.\\n"
        );
        return false;
    }

    result =
        device->CreateQuery(
            D3DQUERYTYPE_TIMESTAMPDISJOINT,
            &g_profilerGpuDisjointQuery
        );

    if (FAILED(result) ||
        g_profilerGpuDisjointQuery == nullptr)
    {
        AppendLog(
            "[Profiler] GPU disjoint query unsupported. "
            "Continuing with CPU structural capture only.\\n"
        );
        ReleaseProfilerGpuQueries();
        return false;
    }

    result =
        device->CreateQuery(
            D3DQUERYTYPE_TIMESTAMP,
            &g_profilerGpuFrameStartQuery
        );

    if (FAILED(result) ||
        g_profilerGpuFrameStartQuery == nullptr)
    {
        AppendLog(
            "[Profiler] GPU timestamp query unsupported. "
            "Continuing with CPU structural capture only.\\n"
        );
        ReleaseProfilerGpuQueries();
        return false;
    }

    result =
        device->CreateQuery(
            D3DQUERYTYPE_TIMESTAMP,
            &g_profilerGpuFrameEndQuery
        );

    if (FAILED(result) ||
        g_profilerGpuFrameEndQuery == nullptr)
    {
        AppendLog(
            "[Profiler] GPU timestamp end query unsupported. "
            "Continuing with CPU structural capture only.\\n"
        );
        ReleaseProfilerGpuQueries();
        return false;
    }

    g_profilerGpuSupported = true;

    g_profilerGpuDisjointQuery->Issue(
        D3DISSUE_BEGIN
    );

    g_profilerGpuFreqQuery->Issue(
        D3DISSUE_END
    );

    IssueProfilerTimestamp(
        g_profilerGpuFrameStartQuery
    );

    return true;
}


static void EndProfilerGpuFrame()
{
    if (!g_profilerGpuSupported)
        return;

    IssueProfilerTimestamp(
        g_profilerGpuFrameEndQuery
    );

    if (g_profilerGpuDisjointQuery != nullptr)
    {
        g_profilerGpuDisjointQuery->Issue(
            D3DISSUE_END
        );
    }
}


static bool WaitProfilerQuery(
    IDirect3DQuery9* query,
    void* data,
    DWORD dataSize,
    DWORD timeoutMs,
    bool flush)
{
    if (query == nullptr ||
        data == nullptr ||
        dataSize == 0)
    {
        return false;
    }

    const ULONGLONG deadline =
        GetTickCount64() +
        static_cast<ULONGLONG>(timeoutMs);

    for (;;)
    {
        const HRESULT result =
            query->GetData(
                data,
                dataSize,
                flush
                    ? D3DGETDATA_FLUSH
                    : 0
            );

        if (result == S_OK)
            return true;

        if (FAILED(result))
            return false;

        if (GetTickCount64() >= deadline)
            return false;

        Sleep(0);
    }
}


static void ResolveProfilerGpuTimings()
{
    if (!g_profilerGpuSupported)
        return;

    // Waiting for the frame-end marker once should make all earlier
    // timestamps available as well.
    if (!WaitProfilerQuery(
            g_profilerGpuFrameEndQuery,
            &g_profilerGpuFrameEndTicks,
            sizeof(g_profilerGpuFrameEndTicks),
            500,
            true))
    {
        AppendLog(
            "[Profiler] WARNING: GPU timestamp resolve timed out. "
            "Pass GPU timings are unavailable for this capture.\\n"
        );
        return;
    }

    if (!WaitProfilerQuery(
            g_profilerGpuFrameStartQuery,
            &g_profilerGpuFrameStartTicks,
            sizeof(g_profilerGpuFrameStartTicks),
            50,
            false) ||
        !WaitProfilerQuery(
            g_profilerGpuFreqQuery,
            &g_profilerGpuFrequency,
            sizeof(g_profilerGpuFrequency),
            50,
            false) ||
        !WaitProfilerQuery(
            g_profilerGpuDisjointQuery,
            &g_profilerGpuDisjoint,
            sizeof(g_profilerGpuDisjoint),
            50,
            false))
    {
        AppendLog(
            "[Profiler] WARNING: GPU query data incomplete. "
            "Pass GPU timings are unavailable for this capture.\\n"
        );
        return;
    }

    if (g_profilerGpuDisjoint ||
        g_profilerGpuFrequency == 0 ||
        g_profilerGpuFrameEndTicks <
            g_profilerGpuFrameStartTicks)
    {
        AppendLog(
            "[Profiler] WARNING: GPU timestamps were disjoint/invalid. "
            "Structural capture is still valid.\\n"
        );
        return;
    }

    g_profilerGpuFrameMs =
        static_cast<double>(
            g_profilerGpuFrameEndTicks -
            g_profilerGpuFrameStartTicks
        ) *
        1000.0 /
        static_cast<double>(
            g_profilerGpuFrequency
        );

    for (ProfilerPass& pass :
         g_profilerPasses)
    {
        if (pass.gpuStartQuery == nullptr ||
            pass.gpuEndQuery == nullptr)
        {
            continue;
        }

        UINT64 startTicks = 0;
        UINT64 endTicks = 0;

        const bool startOk =
            WaitProfilerQuery(
                pass.gpuStartQuery,
                &startTicks,
                sizeof(startTicks),
                20,
                false
            );

        const bool endOk =
            WaitProfilerQuery(
                pass.gpuEndQuery,
                &endTicks,
                sizeof(endTicks),
                20,
                false
            );

        if (!startOk ||
            !endOk ||
            endTicks < startTicks)
        {
            continue;
        }

        pass.gpuStartTicks = startTicks;
        pass.gpuEndTicks = endTicks;
        pass.gpuMs =
            static_cast<double>(
                endTicks - startTicks
            ) *
            1000.0 /
            static_cast<double>(
                g_profilerGpuFrequency
            );
        pass.gpuTimingValid = true;
    }

    g_profilerGpuValid = true;
}


static ProfilerPass* GetOpenProfilerPass();


static bool IsProfilerPipelineResource(
    UINT resourceId)
{
    if (resourceId == 0)
        return false;

    const ProfilerResourceInfo* resource =
        FindProfilerResource(resourceId);

    if (resource == nullptr)
        return false;

    return
        resource->tag != ProfilerResourceTag::Unknown ||
        (resource->usage &
            (D3DUSAGE_RENDERTARGET |
             D3DUSAGE_DEPTHSTENCIL)) != 0;
}


static void AddProfilerDependency(
    ProfilerPass& pass,
    DWORD stage,
    UINT resourceId)
{
    if (!IsProfilerPipelineResource(resourceId))
        return;

    int producerPass = -1;

    const auto writer =
        g_profilerLastWriter.find(resourceId);

    if (writer != g_profilerLastWriter.end())
        producerPass = writer->second;

    for (const ProfilerDependency& dependency :
         pass.dependencies)
    {
        if (dependency.resourceId == resourceId &&
            dependency.stage == stage &&
            dependency.producerPass == producerPass)
        {
            return;
        }
    }

    ProfilerDependency dependency{};
    dependency.resourceId = resourceId;
    dependency.stage = stage;
    dependency.producerPass = producerPass;

    pass.dependencies.push_back(dependency);
}


static void AddProfilerOutputResource(
    ProfilerPass& pass,
    UINT resourceId,
    bool depthOutput)
{
    if (!IsProfilerPipelineResource(resourceId))
        return;

    std::vector<UINT>& outputs =
        depthOutput
            ? pass.depthOutputResources
            : pass.outputResources;

    bool found = false;

    for (UINT existing : outputs)
    {
        if (existing == resourceId)
        {
            found = true;
            break;
        }
    }

    if (!found)
        outputs.push_back(resourceId);

    // Update at the exact draw/clear that produces this resource.
    g_profilerLastWriter[resourceId] =
        static_cast<int>(pass.index);
}


static void ProfilerCaptureDrawState(
    ProfilerPass& pass)
{
    // Resolve inputs at Draw* time. Then discard stale bound stages that the
    // current pixel shader does not actually reference.
    UINT samplerMask = 0;

    const bool exactSamplerMask =
        GetCurrentPixelShaderSamplerMask(
            samplerMask
        );

    if (exactSamplerMask)
    {
        pass.sampledStagesMask |=
            samplerMask;
    }
    else
    {
        ++pass.fallbackSamplerDraws;

        // Fixed-function, SM1.x or malformed/unknown bytecode: preserve the
        // old conservative behavior rather than losing a real dependency.
        samplerMask = 0xFFFFu;
    }

    for (DWORD stage = 0; stage < 16; ++stage)
    {
        if ((samplerMask &
                (1u << stage)) == 0)
        {
            continue;
        }

        const UINT resourceId =
            g_profilerCurrentTextureResources[stage];

        if (resourceId == 0)
            continue;

        AddProfilerDependency(
            pass,
            stage,
            resourceId
        );

        if (stage == 8)
        {
            const ProfilerResourceInfo* resource =
                FindProfilerResource(resourceId);

            if (resource != nullptr &&
                resource->effectiveWidth == g_internalWidth &&
                resource->effectiveHeight == g_internalHeight)
            {
                pass.sawStage8FullRes = true;
            }
        }
    }

    // Resolve all color outputs at draw time, including MRT slots 1..3.
    for (DWORD slot = 0; slot < 4; ++slot)
    {
        const ProfilerSurfaceInfo& surface =
            g_profilerCurrentRenderTargets[slot];

        if (surface.resourceId == 0)
            continue;

        AddProfilerOutputResource(
            pass,
            surface.resourceId,
            false
        );

        if (slot > 0)
        {
            pass.sawMrt = true;

            if (slot == 1)
                pass.rt1 = surface;
        }
    }

    if (g_profilerZWriteEnable &&
        g_profilerCurrentDepth.resourceId != 0)
    {
        pass.writesDepth = true;

        AddProfilerOutputResource(
            pass,
            g_profilerCurrentDepth.resourceId,
            true
        );
    }
}


static void ProfilerCaptureClearOutputs(
    ProfilerPass& pass,
    DWORD flags)
{
    if ((flags & D3DCLEAR_TARGET) != 0)
    {
        for (DWORD slot = 0; slot < 4; ++slot)
        {
            const UINT resourceId =
                g_profilerCurrentRenderTargets[slot].resourceId;

            if (resourceId != 0)
            {
                AddProfilerOutputResource(
                    pass,
                    resourceId,
                    false
                );
            }
        }
    }

    if ((flags & D3DCLEAR_ZBUFFER) != 0 &&
        g_profilerCurrentDepth.resourceId != 0)
    {
        pass.writesDepth = true;

        AddProfilerOutputResource(
            pass,
            g_profilerCurrentDepth.resourceId,
            true
        );
    }
}


static void CloseProfilerPass(
    IDirect3DDevice9* device)
{
    if (g_profilerOpenPass < 0 ||
        static_cast<size_t>(g_profilerOpenPass) >=
            g_profilerPasses.size())
    {
        return;
    }

    ProfilerPass& pass =
        g_profilerPasses[
            static_cast<size_t>(
                g_profilerOpenPass
            )
        ];

    if (g_profilerGpuSupported &&
        pass.gpuEndQuery == nullptr &&
        device != nullptr)
    {
        device->CreateQuery(
            D3DQUERYTYPE_TIMESTAMP,
            &pass.gpuEndQuery
        );

        IssueProfilerTimestamp(
            pass.gpuEndQuery
        );
    }

    pass.endEvent =
        g_profilerEvents.empty()
            ? 0
            : g_profilerEvents.size() - 1;

    pass.dominantVs =
        FindDominantShader(
            pass.vsDrawCounts
        );

    pass.dominantPs =
        FindDominantShader(
            pass.psDrawCounts
        );

    g_profilerOpenPass = -1;
}


static void StartProfilerPass(
    IDirect3DDevice9* device)
{
    ProfilerPass pass{};
    pass.index =
        static_cast<UINT>(
            g_profilerPasses.size()
        );
    pass.rt0 = g_profilerCurrentRenderTargets[0];
    pass.rt1 = g_profilerCurrentRenderTargets[1];
    pass.depth = g_profilerCurrentDepth;
    pass.viewport = g_profilerCurrentViewport;
    pass.startEvent = g_profilerEvents.size();
    pass.firstVs = g_profilerCurrentVs;
    pass.lastVs = g_profilerCurrentVs;
    pass.firstPs = g_profilerCurrentPs;
    pass.lastPs = g_profilerCurrentPs;
    pass.sawMrt =
        g_profilerCurrentRenderTargets[1].pointer != nullptr;

    if (g_profilerGpuSupported &&
        device != nullptr)
    {
        device->CreateQuery(
            D3DQUERYTYPE_TIMESTAMP,
            &pass.gpuStartQuery
        );

        IssueProfilerTimestamp(
            pass.gpuStartQuery
        );
    }

    g_profilerPasses.push_back(
        std::move(pass)
    );

    g_profilerOpenPass =
        static_cast<int>(
            g_profilerPasses.size() - 1
        );
}


static ProfilerPass* GetOpenProfilerPass()
{
    if (g_profilerOpenPass < 0 ||
        static_cast<size_t>(g_profilerOpenPass) >=
            g_profilerPasses.size())
    {
        return nullptr;
    }

    return &g_profilerPasses[
        static_cast<size_t>(
            g_profilerOpenPass
        )
    ];
}


static void ProfilerRecordCallerStats(
    uintptr_t callerAddress,
    unsigned long long primitiveCount)
{
    if (callerAddress == 0)
        return;

    ProfilerCallerStats& stats =
        g_profilerCallers[callerAddress];

    if (stats.address == 0)
    {
        stats.address = callerAddress;
        stats.rva =
            ProfilerCallerRva(
                callerAddress,
                stats.inMainExe
            );
    }

    ++stats.drawCalls;
    stats.primitives += primitiveCount;

    if (g_profilerOpenPass >= 0)
    {
        ++stats.passDrawCounts[
            static_cast<UINT>(
                g_profilerOpenPass
            )
        ];
    }

    if (g_profilerCurrentVs != 0)
        ++stats.vsDrawCounts[g_profilerCurrentVs];

    if (g_profilerCurrentPs != 0)
        ++stats.psDrawCounts[g_profilerCurrentPs];
}


static void ProfilerOnDraw(
    unsigned long long primitiveCount,
    uintptr_t callerAddress)
{
    ProfilerPass* pass =
        GetOpenProfilerPass();

    if (pass == nullptr)
        return;

    ++pass->drawCalls;
    pass->primitives += primitiveCount;

    ProfilerCaptureDrawState(*pass);

    if (g_profilerCurrentVs != 0)
    {
        ++pass->vsDrawCounts[
            g_profilerCurrentVs
        ];
        pass->lastVs =
            g_profilerCurrentVs;

        if (pass->firstVs == 0)
            pass->firstVs =
                g_profilerCurrentVs;
    }

    if (g_profilerCurrentPs != 0)
    {
        ++pass->psDrawCounts[
            g_profilerCurrentPs
        ];
        pass->lastPs =
            g_profilerCurrentPs;

        if (pass->firstPs == 0)
            pass->firstPs =
                g_profilerCurrentPs;
    }

    ProfilerRecordCallerStats(
        callerAddress,
        primitiveCount
    );
}



static double ContinuousTraceMilliseconds(LONGLONG ticks)
{
    if (g_profilerQpcFrequency.QuadPart == 0)
        return 0.0;

    return
        static_cast<double>(ticks - g_profilerContinuousStartCounter.QuadPart) *
        1000.0 /
        static_cast<double>(g_profilerQpcFrequency.QuadPart);
}


static void FinalizeContinuousTraceFrame()
{
    if (!g_profilerContinuousActive.load(std::memory_order_relaxed))
        return;

    if (g_profilerContinuousFrames.size() >=
        static_cast<size_t>(g_config.profilerContinuousMaxFrames))
    {
        return;
    }

    LARGE_INTEGER now{};
    QueryPerformanceCounter(&now);

    ProfilerContinuousFrame frame{};
    frame.index = static_cast<UINT>(g_profilerContinuousFrames.size());
    frame.startTicks = g_profilerContinuousFrameStartCounter.QuadPart;
    frame.endTicks = now.QuadPart;
    frame.candidates.swap(g_profilerCandidateOwnerCalls);
    frame.owners.swap(g_profilerSceneOwnerCalls);
    frame.lifecycle.swap(g_profilerLifecycleEvents);
    frame.draws.swap(g_profilerContinuousDraws);

    g_profilerContinuousFrames.push_back(std::move(frame));

    g_profilerCandidateOwnerCalls.clear();
    g_profilerSceneOwnerCalls.clear();
    g_profilerLifecycleEvents.clear();
    g_profilerContinuousDraws.clear();
    g_profilerSceneObjectOwners.clear();
    g_profilerContinuousFrameStartCounter = now;
}


static void DumpContinuousTraceJson()
{
    char path[MAX_PATH] = {};
    sprintf_s(path, "DPFixNG-lodtrace-%04u.json", g_profilerContinuousCurrent);

    FILE* file = nullptr;
    if (fopen_s(&file, path, "wb") != 0 || file == nullptr)
        return;

    fprintf(
        file,
        "{\n  \"trace\": %u,\n  \"frame_count\": %zu,\n  \"frames\": [\n",
        g_profilerContinuousCurrent,
        g_profilerContinuousFrames.size()
    );

    for (size_t fi = 0; fi < g_profilerContinuousFrames.size(); ++fi)
    {
        const ProfilerContinuousFrame& frame = g_profilerContinuousFrames[fi];
        fprintf(
            file,
            "    {\"index\": %u, \"start_ms\": %.6f, \"end_ms\": %.6f, \"candidates\": [\n",
            frame.index,
            ContinuousTraceMilliseconds(frame.startTicks),
            ContinuousTraceMilliseconds(frame.endTicks)
        );

        for (size_t i = 0; i < frame.candidates.size(); ++i)
        {
            const ProfilerCandidateOwnerCall& r = frame.candidates[i];
            fprintf(
                file,
                "      {\"t_ms\": %.6f, \"owner\": \"0x%08X\", \"caller_rva\": %u, \"result\": %u, \"vtable\": \"0x%08X\", \"type\": %u, \"disable_flags\": %u, \"flags_d8\": \"%016llX\", \"flags_138\": \"%016llX\", \"raw_render_object\": \"0x%08X\", \"class_458\": %u, \"class_45a\": %u}%s\n",
                ContinuousTraceMilliseconds(r.ticks),
                static_cast<UINT>(r.owner), r.callerRva,
                static_cast<unsigned>(r.result), static_cast<UINT>(r.vtable),
                static_cast<unsigned>(r.type), static_cast<unsigned>(r.disableFlags),
                r.flagsD8, r.flags138, static_cast<UINT>(r.rawRenderObject),
                static_cast<unsigned>(r.classField458), static_cast<unsigned>(r.classField45A),
                (i + 1 < frame.candidates.size()) ? "," : ""
            );
        }

        fprintf(file, "    ], \"owners\": [\n");
        for (size_t i = 0; i < frame.owners.size(); ++i)
        {
            const ProfilerSceneOwnerCall& r = frame.owners[i];
            fprintf(
                file,
                "      {\"t_ms\": %.6f, \"owner\": \"0x%08X\", \"caller_rva\": %u, \"result\": \"0x%08X\", \"vtable\": \"0x%08X\", \"type\": %u, \"disable_flags\": %u, \"flags_d8\": \"%016llX\", \"flags_138\": \"%016llX\", \"raw_render_object\": \"0x%08X\"}%s\n",
                ContinuousTraceMilliseconds(r.ticks), static_cast<UINT>(r.owner),
                r.callerRva, static_cast<UINT>(r.result), static_cast<UINT>(r.vtable),
                static_cast<unsigned>(r.type), static_cast<unsigned>(r.disableFlags),
                r.flagsD8, r.flags138, static_cast<UINT>(r.rawRenderObject),
                (i + 1 < frame.owners.size()) ? "," : ""
            );
        }

        fprintf(file, "    ], \"lifecycle\": [\n");
        for (size_t i = 0; i < frame.lifecycle.size(); ++i)
        {
            const ProfilerLifecycleEvent& r = frame.lifecycle[i];
            fprintf(
                file,
                "      {\"t_ms\": %.6f, \"kind\": %u, \"owner\": \"0x%08X\", \"caller_rva\": %u, \"caller\": \"0x%08X\", \"raw_render_object\": \"0x%08X\", \"aux\": \"0x%08X\", \"result\": \"0x%08X\", \"class_458\": %u, \"class_45a\": %u, \"state_29\": %u, \"state_2a\": %u, \"object_category_2c\": %u, \"object_type_30\": %u, \"allocation_size_144\": %u, \"stream_flags_434\": %u, \"event_category\": %u, \"event_code\": %u, \"flags_d8\": \"%016llX\", \"flags_138\": \"%016llX\"}%s\n",
                ContinuousTraceMilliseconds(r.ticks), static_cast<unsigned>(r.kind),
                static_cast<UINT>(r.owner), r.callerRva, static_cast<UINT>(r.caller),
                static_cast<UINT>(r.rawRenderObject), static_cast<UINT>(r.auxPointer),
                static_cast<UINT>(r.result), static_cast<unsigned>(r.classField458),
                static_cast<unsigned>(r.classField45A),
                static_cast<unsigned>(r.state29), static_cast<unsigned>(r.state2A),
                static_cast<unsigned>(r.objectCategory2C),
                static_cast<unsigned>(r.objectType30),
                r.resourceKey144, r.streamFlags434,
                static_cast<unsigned>(r.eventCategory),
                static_cast<unsigned>(r.eventCode),
                r.flagsD8, r.flags138,
                (i + 1 < frame.lifecycle.size()) ? "," : ""
            );
        }

        fprintf(file, "    ], \"draws\": [\n");
        for (size_t i = 0; i < frame.draws.size(); ++i)
        {
            const ProfilerContinuousDraw& r = frame.draws[i];
            fprintf(
                file,
                "      {\"t_ms\": %.6f, \"scene_object\": \"0x%08X\", \"scene_owner\": \"0x%08X\", \"caller_rva\": %u, \"object_caller_rva\": %u, \"base_vertex\": %d, \"min_vertex\": %u, \"vertices\": %u, \"start_index\": %u, \"primitives\": %u}%s\n",
                ContinuousTraceMilliseconds(r.ticks), static_cast<UINT>(r.sceneObject),
                static_cast<UINT>(r.sceneOwner), r.callerRva, r.objectCallerRva,
                r.baseVertex, r.minVertex, r.numVertices, r.startIndex, r.primitiveCount,
                (i + 1 < frame.draws.size()) ? "," : ""
            );
        }

        fprintf(
            file,
            "    ]}%s\n",
            (fi + 1 < g_profilerContinuousFrames.size()) ? "," : ""
        );
    }

    fprintf(file, "  ]\n}\n");
    fclose(file);
}


static void DumpContinuousTraceText()
{
    char path[MAX_PATH] = {};
    sprintf_s(path, "DPFixNG-lodtrace-%04u.txt", g_profilerContinuousCurrent);

    FILE* file = nullptr;
    if (fopen_s(&file, path, "wb") != 0 || file == nullptr)
        return;

    fprintf(file, "DPFix-NG Continuous Streaming Source Trace\n");
    fprintf(file, "Trace: %u\n", g_profilerContinuousCurrent);
    fprintf(file, "Frames: %zu\n\n", g_profilerContinuousFrames.size());

    for (const ProfilerContinuousFrame& frame : g_profilerContinuousFrames)
    {
        fprintf(
            file,
            "FRAME %04u  %.3f -> %.3f ms  candidates=%zu owners=%zu lifecycle=%zu draws=%zu\n",
            frame.index,
            ContinuousTraceMilliseconds(frame.startTicks),
            ContinuousTraceMilliseconds(frame.endTicks),
            frame.candidates.size(), frame.owners.size(), frame.lifecycle.size(), frame.draws.size()
        );

        for (const ProfilerCandidateOwnerCall& r : frame.candidates)
        {
            if (r.vtable == kLodOwnerVtable)
            {
                fprintf(
                    file,
                    "  CAND owner=0x%08X result=%u raw=0x%08X flagsD8=%016llX flags138=%016llX c458=%04X c45A=%02X\n",
                    static_cast<UINT>(r.owner), static_cast<unsigned>(r.result),
                    static_cast<UINT>(r.rawRenderObject), r.flagsD8, r.flags138,
                    static_cast<unsigned>(r.classField458), static_cast<unsigned>(r.classField45A)
                );
            }
        }

        for (const ProfilerLifecycleEvent& r : frame.lifecycle)
        {
            const char* kind = "?";
            if (r.kind == ProfilerLifecycleKind::Create) kind = "CREATE_B7";
            else if (r.kind == ProfilerLifecycleKind::Destroy) kind = "DESTROY_B7";
            else if (r.kind == ProfilerLifecycleKind::MarkUnload) kind = "MARK_UNLOAD";
            else if (r.kind == ProfilerLifecycleKind::ResourceAttach) kind = "RESOURCE_ATTACH";
            else if (r.kind == ProfilerLifecycleKind::StreamEvent3) kind = "EVENT3_SOURCE";
            fprintf(file,
                "  LIFE %-15s owner=0x%08X caller=+%08X raw=0x%08X c458=%04X "
                "s29=%02X s2A=%02X cat2C=%04X type30=%04X alloc144=%08X "
                "stream434=%08X evtCat=%04X evt=%02X flagsD8=%016llX flags138=%016llX "
                "aux=0x%08X result=0x%08X\n",
                kind, static_cast<UINT>(r.owner), r.callerRva,
                static_cast<UINT>(r.rawRenderObject), static_cast<unsigned>(r.classField458),
                static_cast<unsigned>(r.state29), static_cast<unsigned>(r.state2A),
                static_cast<unsigned>(r.objectCategory2C), static_cast<unsigned>(r.objectType30),
                r.resourceKey144, r.streamFlags434,
                static_cast<unsigned>(r.eventCategory), static_cast<unsigned>(r.eventCode),
                r.flagsD8, r.flags138,
                static_cast<UINT>(r.auxPointer), static_cast<UINT>(r.result));
        }

        for (const ProfilerContinuousDraw& r : frame.draws)
        {
            fprintf(
                file,
                "  DRAW owner=0x%08X obj=0x%08X caller=+%08X objCaller=+%08X verts=%u prim=%u base=%d start=%u\n",
                static_cast<UINT>(r.sceneOwner), static_cast<UINT>(r.sceneObject),
                r.callerRva, r.objectCallerRva, r.numVertices, r.primitiveCount,
                r.baseVertex, r.startIndex
            );
        }
    }

    fclose(file);
}


static void StartContinuousTrace()
{
    if (g_profilerContinuousActive.load(std::memory_order_relaxed))
        return;

    QueryPerformanceFrequency(&g_profilerQpcFrequency);
    QueryPerformanceCounter(&g_profilerContinuousStartCounter);
    g_profilerContinuousFrameStartCounter = g_profilerContinuousStartCounter;

    g_profilerContinuousCurrent =
        g_profilerContinuousCounter.fetch_add(1, std::memory_order_relaxed) + 1;

    g_profilerContinuousFrames.clear();
    g_profilerContinuousFrames.reserve(g_config.profilerContinuousMaxFrames);
    g_profilerCandidateOwnerCalls.clear();
    g_profilerSceneOwnerCalls.clear();
    g_profilerLifecycleEvents.clear();
    g_profilerContinuousDraws.clear();
    g_profilerLifecycleEvents.reserve(256);
    g_profilerContinuousDraws.reserve(512);
    g_profilerSceneObjectOwners.clear();

    EnableLifecycleTraceHooksForCapture();
    EnableCandidateVisibilityTraceHookForCapture();
    EnableSceneOwnerTraceHookForCapture();
    EnableSceneObjectTraceHookForCapture();

    g_profilerContinuousActive.store(true, std::memory_order_release);

    AppendLog(
        "[Profiler] Continuous LOD trace STARTED. Press CaptureKey again to stop.\n"
    );
}


static void StopContinuousTrace(bool frameLimitReached)
{
    if (!g_profilerContinuousActive.exchange(false, std::memory_order_acq_rel))
        return;

    DisableSceneObjectTraceHookAfterCapture();
    DisableSceneOwnerTraceHookAfterCapture();
    DisableCandidateVisibilityTraceHookAfterCapture();
    DisableLifecycleTraceHooksAfterCapture();

    DumpContinuousTraceText();
    DumpContinuousTraceJson();

    char text[512] = {};
    sprintf_s(
        text,
        "[Profiler] Continuous LOD trace #%u STOPPED: %zu frames%s. Files: "
        "DPFixNG-lodtrace-%04u.txt / .json\n",
        g_profilerContinuousCurrent,
        g_profilerContinuousFrames.size(),
        frameLimitReached ? " (frame limit reached)" : "",
        g_profilerContinuousCurrent
    );
    AppendLog(text);
}


static void StartProfilerCapture(
    IDirect3DDevice9* device)
{
    if (!g_config.profilerEnabled ||
        device == nullptr)
    {
        return;
    }

    g_profilerCurrentCapture =
        g_profilerCaptureCounter.fetch_add(
            1,
            std::memory_order_relaxed
        ) + 1;

    QueryPerformanceFrequency(
        &g_profilerQpcFrequency
    );

    QueryPerformanceCounter(
        &g_profilerStartCounter
    );

    g_profilerEvents.clear();
    g_profilerShaders.clear();
    g_profilerPasses.clear();
    g_profilerCallers.clear();
    g_profilerSceneOwnerCalls.clear();
    g_profilerCandidateOwnerCalls.clear();

    InitializeMainExeInfo();

    g_profilerCurrentRt0 = {};
    g_profilerCurrentRt1 = {};

    for (ProfilerSurfaceInfo& rt :
         g_profilerCurrentRenderTargets)
    {
        rt = {};
    }

    for (UINT& textureResource :
         g_profilerCurrentTextureResources)
    {
        textureResource = 0;
    }

    g_profilerCurrentDepth = {};
    g_profilerCurrentViewport = {};
    g_profilerCurrentVs = 0;
    g_profilerCurrentPs = 0;
    g_profilerOpenPass = -1;
    g_profilerLastWriter.clear();

    DWORD zWrite = TRUE;

    if (SUCCEEDED(device->GetRenderState(
            D3DRS_ZWRITEENABLE,
            &zWrite)))
    {
        g_profilerZWriteEnable =
            zWrite != FALSE;
    }
    else
    {
        g_profilerZWriteEnable = true;
    }

    ReleaseProfilerGpuQueries();

    g_profilerEvents.reserve(
        g_config.profilerMaxEvents
    );

    g_profilerShaders.reserve(256);
    g_profilerPasses.reserve(128);
    g_profilerSceneOwnerCalls.reserve(512);
    g_profilerLifecycleEvents.reserve(256);
    g_profilerCandidateOwnerCalls.reserve(512);

    g_profilerOverflow = false;

    // Normally enabled when F11 was detected in the preceding Present. Keep
    // these idempotent calls as a safety net for programmatic capture paths.
    EnableCandidateVisibilityTraceHookForCapture();
    EnableSceneOwnerTraceHookForCapture();
    EnableSceneObjectTraceHookForCapture();

    BeginProfilerGpuFrame(device);

    g_profilerCaptureActive.store(
        true,
        std::memory_order_release
    );

    ProfilerSnapshotInitialState(device);

    // SnapshotInitialState records the state, but pass grouping also needs
    // a compact current-state copy.
    {
        for (DWORD slot = 0; slot < 4; ++slot)
        {
            IDirect3DSurface9* rt = nullptr;

            if (SUCCEEDED(device->GetRenderTarget(
                    slot,
                    &rt)) &&
                rt != nullptr)
            {
                g_profilerCurrentRenderTargets[slot] =
                    MakeProfilerSurfaceInfo(rt);
                rt->Release();
            }
        }

        g_profilerCurrentRt0 =
            g_profilerCurrentRenderTargets[0];
        g_profilerCurrentRt1 =
            g_profilerCurrentRenderTargets[1];

        IDirect3DSurface9* depth = nullptr;

        if (SUCCEEDED(device->GetDepthStencilSurface(
                &depth)) &&
            depth != nullptr)
        {
            g_profilerCurrentDepth =
                MakeProfilerSurfaceInfo(depth);
            depth->Release();
        }

        device->GetViewport(
            &g_profilerCurrentViewport
        );

        IDirect3DVertexShader9* vs = nullptr;

        if (SUCCEEDED(device->GetVertexShader(&vs)) &&
            vs != nullptr)
        {
            g_profilerCurrentVs =
                RegisterVertexShaderForProfiler(vs);
            vs->Release();
        }

        IDirect3DPixelShader9* ps = nullptr;

        if (SUCCEEDED(device->GetPixelShader(&ps)) &&
            ps != nullptr)
        {
            g_profilerCurrentPs =
                RegisterPixelShaderForProfiler(ps);
            ps->Release();
        }
    }

    StartProfilerPass(device);

    char text[384] = {};

    sprintf_s(
        text,
        "[Profiler] Capture #%u started. "
        "Capturing one complete frame in memory. GPU timings=%s.\n",
        g_profilerCurrentCapture,
        g_profilerGpuSupported ? "enabled" : "unavailable"
    );

    AppendLog(text);

    if (g_config.profilerCallerTracing &&
        g_mainExeInfoValid)
    {
        char exeText[384] = {};

        sprintf_s(
            exeText,
            "[Profiler] Main EXE: base=0x%p size=0x%zX "
            "preferred=0x%p entryRVA=0x%08X timestamp=0x%08X.\n",
            reinterpret_cast<void*>(g_mainExeBase),
            g_mainExeSize,
            reinterpret_cast<void*>(
                g_mainExePreferredBase
            ),
            g_mainExeEntryRva,
            g_mainExeTimeDateStamp
        );

        AppendLog(exeText);
    }
}


static void DumpProfilerShaders()
{
    if (!g_config.profilerDumpShaders)
        return;

    for (const ProfilerShaderInfo& shader :
         g_profilerShaders)
    {
        if (shader.bytecode.empty())
            continue;

        wchar_t basePath[MAX_PATH] = {};

        if (!GetProfilerOutputPath(
                basePath,
                MAX_PATH,
                g_profilerCurrentCapture,
                L"tmp"))
        {
            continue;
        }

        wchar_t* dot = wcsrchr(
            basePath,
            L'.'
        );

        if (dot == nullptr)
            continue;

        *dot = L'\0';

        wchar_t suffix[64] = {};

        swprintf_s(
            suffix,
            L"-%ls-%04u.bin",
            shader.pixel ? L"PS" : L"VS",
            shader.id
        );

        if (wcscat_s(
                basePath,
                MAX_PATH,
                suffix) != 0)
        {
            continue;
        }

        FILE* file = nullptr;

        if (_wfopen_s(
                &file,
                basePath,
                L"wb") != 0 ||
            file == nullptr)
        {
            continue;
        }

        fwrite(
            shader.bytecode.data(),
            1,
            shader.bytecode.size(),
            file
        );

        fclose(file);
    }
}


static void DumpProfilerText()
{
    wchar_t path[MAX_PATH] = {};

    if (!GetProfilerOutputPath(
            path,
            MAX_PATH,
            g_profilerCurrentCapture,
            L"txt"))
    {
        return;
    }

    FILE* file = nullptr;

    if (_wfopen_s(
            &file,
            path,
            L"w") != 0 ||
        file == nullptr)
    {
        return;
    }

    size_t drawCount = 0;
    size_t rtChanges = 0;
    size_t textureBinds = 0;
    size_t shaderBinds = 0;

    for (const ProfilerEvent& event :
         g_profilerEvents)
    {
        switch (event.type)
        {
            case ProfilerEventType::DrawPrimitive:
            case ProfilerEventType::DrawIndexedPrimitive:
            case ProfilerEventType::DrawPrimitiveUP:
            case ProfilerEventType::DrawIndexedPrimitiveUP:
                ++drawCount;
                break;

            case ProfilerEventType::SetRenderTarget:
                ++rtChanges;
                break;

            case ProfilerEventType::SetTexture:
                ++textureBinds;
                break;

            case ProfilerEventType::SetVertexShader:
            case ProfilerEventType::SetPixelShader:
                ++shaderBinds;
                break;

            default:
                break;
        }
    }

    double durationMs = 0.0;

    if (!g_profilerEvents.empty())
    {
        durationMs =
            ProfilerMilliseconds(
                g_profilerEvents.back().ticks
            );
    }

    fprintf(
        file,
        "DPFix-NG Frame Profiler\n"
        "Capture: %u\n"
        "Display: %u x %u\n"
        "Internal: %u x %u\n"
        "Duration: %.3f ms\n"
        "Events: %zu%s\n"
        "Draw calls: %zu\n"
        "RT changes: %zu\n"
        "Texture binds: %zu\n"
        "Shader binds: %zu\n"
        "Shaders seen: %zu\n"
        "\n",
        g_profilerCurrentCapture,
        g_displayWidth,
        g_displayHeight,
        g_internalWidth,
        g_internalHeight,
        durationMs,
        g_profilerEvents.size(),
        g_profilerOverflow ? " (TRUNCATED)" : "",
        drawCount,
        rtChanges,
        textureBinds,
        shaderBinds,
        g_profilerShaders.size()
    );

    if (g_mainExeInfoValid)
    {
        fprintf(
            file,
            "Main EXE: base=0x%p size=0x%zX preferred=0x%p "
            "entryRVA=0x%08X timestamp=0x%08X characteristics=0x%04X\n",
            reinterpret_cast<void*>(g_mainExeBase),
            g_mainExeSize,
            reinterpret_cast<void*>(
                g_mainExePreferredBase
            ),
            g_mainExeEntryRva,
            g_mainExeTimeDateStamp,
            g_mainExeCharacteristics
        );
    }

    fprintf(
        file,
        "GPU timings: %s\n",
        g_profilerGpuValid
            ? "available"
            : (g_profilerGpuRequested
                ? "unavailable/invalid"
                : "disabled")
    );

    if (g_profilerGpuValid)
    {
        fprintf(
            file,
            "GPU frame (pre-Present): %.3f ms\n",
            g_profilerGpuFrameMs
        );
    }

    fprintf(
        file,
        "\n=== RESOURCE TABLE ===\n"
    );

    for (const ProfilerResourceInfo& resource :
         g_profilerResources)
    {
        const bool isPipelineResource =
            resource.tag != ProfilerResourceTag::Unknown ||
            (resource.usage &
                (D3DUSAGE_RENDERTARGET |
                 D3DUSAGE_DEPTHSTENCIL)) != 0;

        if (!isPipelineResource)
            continue;

        fprintf(
            file,
            "RES #%04u  %-22s requested=%ux%u effective=%ux%u "
            "fmt=%u usage=0x%08X tex=0x%p surf=0x%p%s\n",
            resource.id,
            ProfilerResourceTagName(resource.tag),
            resource.requestedWidth,
            resource.requestedHeight,
            resource.effectiveWidth,
            resource.effectiveHeight,
            resource.format,
            resource.usage,
            resource.texturePointer,
            resource.surfacePointer,
            resource.textureBacked
                ? " texture-backed"
                : ""
        );
    }

    fprintf(
        file,
        "\n=== PASS SUMMARY ===\n"
    );

    for (const ProfilerPass& pass :
         g_profilerPasses)
    {
        if (pass.drawCalls == 0 &&
            pass.clears == 0 &&
            pass.stretchRects == 0)
        {
            continue;
        }

        const char* classification =
            ClassifyProfilerPass(pass);

        const ProfilerShaderInfo* dominantPs =
            FindProfilerShader(
                pass.dominantPs
            );

        const ProfilerShaderInfo* dominantVs =
            FindProfilerShader(
                pass.dominantVs
            );

        fprintf(
            file,
            "PASS %03u  %-28s  "
            "RT0=#%u/%s %ux%u fmt=%u ptr=0x%p  "
            "DS=#%u/%s %ux%u fmt=%u  "
            "VP=%ux%u  draws=%u prim=%llu  "
            "tex=%u shader=%u clear=%u stretch=%u",
            pass.index,
            classification,
            pass.rt0.resourceId,
            ProfilerResourceTagName(pass.rt0.tag),
            pass.rt0.width,
            pass.rt0.height,
            pass.rt0.format,
            pass.rt0.pointer,
            pass.depth.resourceId,
            ProfilerResourceTagName(pass.depth.tag),
            pass.depth.width,
            pass.depth.height,
            pass.depth.format,
            pass.viewport.Width,
            pass.viewport.Height,
            pass.drawCalls,
            pass.primitives,
            pass.textureBinds,
            pass.shaderBinds,
            pass.clears,
            pass.stretchRects
        );

        if (pass.sawMrt)
            fprintf(file, " MRT");

        if (pass.sawStage8FullRes)
            fprintf(file, " STAGE8");

        if (dominantVs != nullptr)
        {
            fprintf(
                file,
                " domVS=#%u/%016llX",
                dominantVs->id,
                dominantVs->hash
            );
        }

        if (dominantPs != nullptr)
        {
            fprintf(
                file,
                " domPS=#%u/%016llX",
                dominantPs->id,
                dominantPs->hash
            );
        }

        if (pass.gpuTimingValid)
        {
            fprintf(
                file,
                " GPU=%.3fms",
                pass.gpuMs
            );
        }
        else if (g_profilerGpuRequested)
        {
            fprintf(file, " GPU=n/a");
        }

        {
            char sampledStages[96] = {};

            FormatSamplerMask(
                pass.sampledStagesMask,
                sampledStages,
                sizeof(sampledStages)
            );

            fprintf(
                file,
                " PS_SAMPLERS={%s}",
                sampledStages
            );

            if (pass.fallbackSamplerDraws != 0)
            {
                fprintf(
                    file,
                    " SAMPLER_FALLBACK_DRAWS=%u",
                    pass.fallbackSamplerDraws
                );
            }
        }

        if (!pass.outputResources.empty())
        {
            fprintf(file, " WRITES=[");

            for (size_t i = 0; i < pass.outputResources.size(); ++i)
            {
                const UINT resourceId = pass.outputResources[i];
                const ProfilerResourceInfo* resource =
                    FindProfilerResource(resourceId);

                if (i != 0)
                    fprintf(file, ", ");

                fprintf(
                    file,
                    "#%u/%s",
                    resourceId,
                    resource != nullptr
                        ? ProfilerResourceTagName(resource->tag)
                        : "Unknown"
                );
            }

            fprintf(file, "]");
        }

        if (!pass.depthOutputResources.empty())
        {
            fprintf(file, " DEPTH_WRITES=[");

            for (size_t i = 0; i < pass.depthOutputResources.size(); ++i)
            {
                const UINT resourceId = pass.depthOutputResources[i];
                const ProfilerResourceInfo* resource =
                    FindProfilerResource(resourceId);

                if (i != 0)
                    fprintf(file, ", ");

                fprintf(
                    file,
                    "#%u/%s",
                    resourceId,
                    resource != nullptr
                        ? ProfilerResourceTagName(resource->tag)
                        : "Unknown"
                );
            }

            fprintf(file, "]");
        }

        if (!pass.dependencies.empty())
        {
            fprintf(file, " READS=[");

            for (size_t dependencyIndex = 0;
                 dependencyIndex < pass.dependencies.size();
                 ++dependencyIndex)
            {
                const ProfilerDependency& dependency =
                    pass.dependencies[dependencyIndex];

                const ProfilerResourceInfo* resource =
                    FindProfilerResource(
                        dependency.resourceId
                    );

                if (dependencyIndex != 0)
                    fprintf(file, ", ");

                fprintf(
                    file,
                    "S%u:#%u/%s",
                    dependency.stage,
                    dependency.resourceId,
                    resource != nullptr
                        ? ProfilerResourceTagName(resource->tag)
                        : "Unknown"
                );

                if (dependency.producerPass >= 0)
                {
                    fprintf(
                        file,
                        "<-P%03d",
                        dependency.producerPass
                    );
                }
            }

            fprintf(file, "]");
        }

        fputc('\n', file);
    }

    fprintf(file, "\n");

    if (g_config.profilerCallerTracing &&
        !g_profilerCallers.empty())
    {
        std::vector<const ProfilerCallerStats*> callers;
        callers.reserve(g_profilerCallers.size());

        for (const auto& entry :
             g_profilerCallers)
        {
            callers.push_back(&entry.second);
        }

        std::sort(
            callers.begin(),
            callers.end(),
            [](const ProfilerCallerStats* a,
               const ProfilerCallerStats* b)
            {
                if (a->drawCalls != b->drawCalls)
                    return a->drawCalls > b->drawCalls;

                return a->address < b->address;
            }
        );

        fprintf(
            file,
            "=== DRAW CALLERS ===\n"
        );

        for (const ProfilerCallerStats* stats :
             callers)
        {
            if (stats == nullptr)
                continue;

            if (stats->inMainExe)
            {
                fprintf(
                    file,
                    "CALLER DP.exe+0x%08X abs=0x%p "
                    "draws=%llu prim=%llu",
                    stats->rva,
                    reinterpret_cast<void*>(
                        stats->address
                    ),
                    stats->drawCalls,
                    stats->primitives
                );
            }
            else
            {
                fprintf(
                    file,
                    "CALLER external abs=0x%p "
                    "draws=%llu prim=%llu",
                    reinterpret_cast<void*>(
                        stats->address
                    ),
                    stats->drawCalls,
                    stats->primitives
                );
            }

            if (!stats->passDrawCounts.empty())
            {
                fprintf(file, " passes={");

                bool firstPass = true;

                for (const auto& passEntry :
                     stats->passDrawCounts)
                {
                    if (!firstPass)
                        fprintf(file, ",");

                    fprintf(
                        file,
                        "P%03u:%u",
                        passEntry.first,
                        passEntry.second
                    );

                    firstPass = false;
                }

                fprintf(file, "}");
            }

            if (!stats->psDrawCounts.empty())
            {
                fprintf(file, " ps={");

                bool firstPs = true;

                for (const auto& psEntry :
                     stats->psDrawCounts)
                {
                    if (!firstPs)
                        fprintf(file, ",");

                    const ProfilerShaderInfo* shader =
                        FindProfilerShader(
                            psEntry.first
                        );

                    if (shader != nullptr)
                    {
                        fprintf(
                            file,
                            "#%u/%016llX:%u",
                            psEntry.first,
                            shader->hash,
                            psEntry.second
                        );
                    }
                    else
                    {
                        fprintf(
                            file,
                            "#%u:%u",
                            psEntry.first,
                            psEntry.second
                        );
                    }

                    firstPs = false;
                }

                fprintf(file, "}");
            }

            fputc('\n', file);
        }

        fprintf(file, "\n");
    }

    if (!g_profilerCandidateOwnerCalls.empty())
    {
        fprintf(file, "=== CANDIDATE OWNER GATE ===\n");

        for (const ProfilerCandidateOwnerCall& record :
             g_profilerCandidateOwnerCalls)
        {
            fprintf(
                file,
                "CANDIDATE t=%.3f owner=0x%p caller=DP.exe+0x%08X result=%u "
                "vtable=0x%p type=%u disable=0x%02X flagsD8=%016llX "
                "flags138=%016llX rawRender=0x%p class458=0x%04X class45A=0x%02X\n",
                ProfilerMilliseconds(record.ticks),
                reinterpret_cast<void*>(record.owner),
                record.callerRva,
                static_cast<unsigned>(record.result),
                reinterpret_cast<void*>(record.vtable),
                static_cast<unsigned>(record.type),
                static_cast<unsigned>(record.disableFlags),
                record.flagsD8,
                record.flags138,
                reinterpret_cast<void*>(record.rawRenderObject),
                static_cast<unsigned>(record.classField458),
                static_cast<unsigned>(record.classField45A)
            );
        }

        fprintf(file, "\n");
    }

    if (!g_profilerSceneOwnerCalls.empty())
    {
        fprintf(file, "=== SCENE OWNER SELECTION ===\n");

        for (const ProfilerSceneOwnerCall& record :
             g_profilerSceneOwnerCalls)
        {
            fprintf(
                file,
                "OWNER t=%.3f owner=0x%p caller=DP.exe+0x%08X result=0x%p "
                "vtable=0x%p type=%u disable=0x%02X flagsD8=%016llX "
                "flags138=%016llX rawRender=0x%p\n",
                ProfilerMilliseconds(record.ticks),
                reinterpret_cast<void*>(record.owner),
                record.callerRva,
                reinterpret_cast<void*>(record.result),
                reinterpret_cast<void*>(record.vtable),
                static_cast<unsigned>(record.type),
                static_cast<unsigned>(record.disableFlags),
                record.flagsD8,
                record.flags138,
                reinterpret_cast<void*>(record.rawRenderObject)
            );
        }

        fprintf(file, "\n");
    }

    if (!g_profilerShaders.empty())
    {
        fprintf(
            file,
            "=== SHADERS ===\n"
        );

        for (const ProfilerShaderInfo& shader :
             g_profilerShaders)
        {
            char usedSamplers[96] = {};
            char declaredSamplers[96] = {};

            FormatSamplerMask(
                shader.samplerMask,
                usedSamplers,
                sizeof(usedSamplers)
            );

            FormatSamplerMask(
                shader.declaredSamplerMask,
                declaredSamplers,
                sizeof(declaredSamplers)
            );

            fprintf(
                file,
                "%s #%u ptr=0x%p hash=%016llX bytecode=%u bytes "
                "sm=%u_%u samplerParse=%s used={%s} declared={%s}\n",
                shader.pixel ? "PS" : "VS",
                shader.id,
                shader.pointer,
                shader.hash,
                shader.bytecodeSize,
                shader.shaderMajor,
                shader.shaderMinor,
                shader.samplerMaskValid ? "ok" : "fallback",
                usedSamplers,
                declaredSamplers
            );
        }

        fprintf(file, "\n");
    }

    fprintf(
        file,
        "=== EVENT TIMELINE ===\n"
    );

    for (const ProfilerEvent& event :
         g_profilerEvents)
    {
        const double ms =
            ProfilerMilliseconds(
                event.ticks
            );

        fprintf(
            file,
            "%9.3f  %-28s",
            ms,
            ProfilerEventName(event.type)
        );

        switch (event.type)
        {
            case ProfilerEventType::SetRenderTarget:
            {
                const ProfilerResourceInfo* resource =
                    FindProfilerResource(
                        static_cast<UINT>(
                            event.v[6]
                        )
                    );

                fprintf(
                    file,
                    " slot=%llu ptr=0x%p %llux%llu fmt=%llu usage=0x%08llX res=#%llu tag=%s",
                    event.v[0],
                    reinterpret_cast<void*>(event.v[1]),
                    event.v[2],
                    event.v[3],
                    event.v[4],
                    event.v[5],
                    event.v[6],
                    resource != nullptr
                        ? ProfilerResourceTagName(resource->tag)
                        : "Unknown"
                );
                break;
            }

            case ProfilerEventType::SetDepthStencilSurface:
            {
                const ProfilerResourceInfo* resource =
                    FindProfilerResource(
                        static_cast<UINT>(
                            event.v[6]
                        )
                    );

                fprintf(
                    file,
                    " ptr=0x%p %llux%llu fmt=%llu usage=0x%08llX res=#%llu tag=%s",
                    reinterpret_cast<void*>(event.v[1]),
                    event.v[2],
                    event.v[3],
                    event.v[4],
                    event.v[5],
                    event.v[6],
                    resource != nullptr
                        ? ProfilerResourceTagName(resource->tag)
                        : "Unknown"
                );
                break;
            }

            case ProfilerEventType::SetViewport:
                fprintf(
                    file,
                    " x=%llu y=%llu %llux%llu",
                    event.v[0],
                    event.v[1],
                    event.v[2],
                    event.v[3]
                );
                break;

            case ProfilerEventType::SetTexture:
            {
                const ProfilerResourceInfo* resource =
                    FindProfilerResource(
                        static_cast<UINT>(
                            event.v[7]
                        )
                    );

                fprintf(
                    file,
                    " stage=%llu ptr=0x%p %llux%llu fmt=%llu usage=0x%08llX type=%llu res=#%llu tag=%s",
                    event.v[0],
                    reinterpret_cast<void*>(event.v[1]),
                    event.v[2],
                    event.v[3],
                    event.v[4],
                    event.v[5],
                    event.v[6],
                    event.v[7],
                    resource != nullptr
                        ? ProfilerResourceTagName(resource->tag)
                        : "Unknown"
                );
                break;
            }

            case ProfilerEventType::SetVertexShader:
            case ProfilerEventType::SetPixelShader:
                fprintf(
                    file,
                    " id=%llu ptr=0x%p",
                    event.v[0],
                    reinterpret_cast<void*>(event.v[1])
                );
                break;

            case ProfilerEventType::SetRenderState:
                fprintf(
                    file,
                    " state=%llu value=0x%08llX",
                    event.v[0],
                    event.v[1]
                );
                break;

            case ProfilerEventType::Clear:
                fprintf(
                    file,
                    " flags=0x%08llX color=0x%08llX rects=%llu stencil=%llu",
                    event.v[0],
                    event.v[1],
                    event.v[2],
                    event.v[3]
                );
                break;

            case ProfilerEventType::StretchRect:
                fprintf(
                    file,
                    " src=0x%p dst=0x%p filter=%llu",
                    reinterpret_cast<void*>(event.v[0]),
                    reinterpret_cast<void*>(event.v[1]),
                    event.v[2]
                );
                break;

            case ProfilerEventType::DrawPrimitive:
                fprintf(
                    file,
                    " type=%llu startVertex=%llu primitives=%llu",
                    event.v[0],
                    event.v[1],
                    event.v[2]
                );

                if (event.callerAddress != 0)
                {
                    if (event.callerInMainExe)
                    {
                        fprintf(
                            file,
                            " caller=DP.exe+0x%08X",
                            event.callerRva
                        );
                    }
                    else
                    {
                        fprintf(
                            file,
                            " caller=external@0x%p",
                            reinterpret_cast<void*>(
                                event.callerAddress
                            )
                        );
                    }

                    fprintf(
                        file,
                        " pass=P%03d VS#%u PS#%u",
                        event.passIndex,
                        event.vertexShaderId,
                        event.pixelShaderId
                    );
                }
                break;

            case ProfilerEventType::DrawIndexedPrimitive:
                fprintf(
                    file,
                    " type=%llu baseVertex=%lld minVertex=%llu vertices=%llu startIndex=%llu primitives=%llu",
                    event.v[0],
                    static_cast<long long>(event.v[1]),
                    event.v[2],
                    event.v[3],
                    event.v[4],
                    event.v[5]
                );

                if (event.callerAddress != 0)
                {
                    if (event.callerInMainExe)
                    {
                        fprintf(
                            file,
                            " caller=DP.exe+0x%08X",
                            event.callerRva
                        );
                    }
                    else
                    {
                        fprintf(
                            file,
                            " caller=external@0x%p",
                            reinterpret_cast<void*>(
                                event.callerAddress
                            )
                        );
                    }

                    fprintf(
                        file,
                        " pass=P%03d VS#%u PS#%u",
                        event.passIndex,
                        event.vertexShaderId,
                        event.pixelShaderId
                    );
                }

                if (event.v[6] != 0)
                {
                    const UINT objectCallerRva =
                        static_cast<UINT>(event.v[7] & 0xFFFFFFFFull);
                    const uintptr_t sceneOwner =
                        static_cast<uintptr_t>(
                            static_cast<UINT>(event.v[7] >> 32)
                        );

                    fprintf(
                        file,
                        " sceneObject=0x%p sceneOwner=0x%p objectCaller=DP.exe+0x%08X",
                        reinterpret_cast<void*>(
                            static_cast<uintptr_t>(event.v[6])
                        ),
                        reinterpret_cast<void*>(sceneOwner),
                        objectCallerRva
                    );
                }
                break;

            case ProfilerEventType::DrawPrimitiveUP:
                fprintf(
                    file,
                    " type=%llu primitives=%llu stride=%llu",
                    event.v[0],
                    event.v[1],
                    event.v[2]
                );

                if (event.callerAddress != 0)
                {
                    if (event.callerInMainExe)
                    {
                        fprintf(
                            file,
                            " caller=DP.exe+0x%08X",
                            event.callerRva
                        );
                    }
                    else
                    {
                        fprintf(
                            file,
                            " caller=external@0x%p",
                            reinterpret_cast<void*>(
                                event.callerAddress
                            )
                        );
                    }

                    fprintf(
                        file,
                        " pass=P%03d VS#%u PS#%u",
                        event.passIndex,
                        event.vertexShaderId,
                        event.pixelShaderId
                    );
                }
                break;

            case ProfilerEventType::DrawIndexedPrimitiveUP:
                fprintf(
                    file,
                    " type=%llu minVertex=%llu vertices=%llu primitives=%llu indexFmt=%llu stride=%llu",
                    event.v[0],
                    event.v[1],
                    event.v[2],
                    event.v[3],
                    event.v[4],
                    event.v[5]
                );

                if (event.callerAddress != 0)
                {
                    if (event.callerInMainExe)
                    {
                        fprintf(
                            file,
                            " caller=DP.exe+0x%08X",
                            event.callerRva
                        );
                    }
                    else
                    {
                        fprintf(
                            file,
                            " caller=external@0x%p",
                            reinterpret_cast<void*>(
                                event.callerAddress
                            )
                        );
                    }

                    fprintf(
                        file,
                        " pass=P%03d VS#%u PS#%u",
                        event.passIndex,
                        event.vertexShaderId,
                        event.pixelShaderId
                    );
                }
                break;

            default:
                break;
        }

        fputc('\n', file);
    }

    fclose(file);
}


static void DumpProfilerJson()
{
    wchar_t path[MAX_PATH] = {};

    if (!GetProfilerOutputPath(
            path,
            MAX_PATH,
            g_profilerCurrentCapture,
            L"json"))
    {
        return;
    }

    FILE* file = nullptr;

    if (_wfopen_s(
            &file,
            path,
            L"w") != 0 ||
        file == nullptr)
    {
        return;
    }

    double durationMs = 0.0;

    if (!g_profilerEvents.empty())
    {
        durationMs =
            ProfilerMilliseconds(
                g_profilerEvents.back().ticks
            );
    }

    fprintf(
        file,
        "{\n"
        "  \"capture\": %u,\n"
        "  \"display\": [%u, %u],\n"
        "  \"internal\": [%u, %u],\n"
        "  \"duration_ms\": %.6f,\n"
        "  \"truncated\": %s,\n"
        "  \"gpu_timings_available\": %s,\n"
        "  \"gpu_frame_ms\": %.6f,\n"
        "  \"main_exe\": {"
        "\"valid\": %s, "
        "\"base\": \"0x%p\", "
        "\"size\": %zu, "
        "\"preferred_base\": \"0x%p\", "
        "\"entry_rva\": %u, "
        "\"timestamp\": %u, "
        "\"characteristics\": %u},\n"
        "  \"resources\": [\n",
        g_profilerCurrentCapture,
        g_displayWidth,
        g_displayHeight,
        g_internalWidth,
        g_internalHeight,
        durationMs,
        g_profilerOverflow ? "true" : "false",
        g_profilerGpuValid ? "true" : "false",
        g_profilerGpuValid
            ? g_profilerGpuFrameMs
            : 0.0,
        g_mainExeInfoValid ? "true" : "false",
        reinterpret_cast<void*>(g_mainExeBase),
        g_mainExeSize,
        reinterpret_cast<void*>(
            g_mainExePreferredBase
        ),
        g_mainExeEntryRva,
        g_mainExeTimeDateStamp,
        g_mainExeCharacteristics
    );

    bool firstResourceJson = true;

    for (const ProfilerResourceInfo& resource :
         g_profilerResources)
    {
        const bool isPipelineResource =
            resource.tag != ProfilerResourceTag::Unknown ||
            (resource.usage &
                (D3DUSAGE_RENDERTARGET |
                 D3DUSAGE_DEPTHSTENCIL)) != 0;

        if (!isPipelineResource)
            continue;

        if (!firstResourceJson)
            fprintf(file, ",\n");

        firstResourceJson = false;

        fprintf(
            file,
            "    {\"id\": %u, "
            "\"tag\": \"%s\", "
            "\"requested\": [%u, %u], "
            "\"effective\": [%u, %u], "
            "\"format\": %u, "
            "\"usage\": %u, "
            "\"texture_ptr\": \"0x%p\", "
            "\"surface_ptr\": \"0x%p\", "
            "\"texture_backed\": %s}",
            resource.id,
            ProfilerResourceTagName(resource.tag),
            resource.requestedWidth,
            resource.requestedHeight,
            resource.effectiveWidth,
            resource.effectiveHeight,
            resource.format,
            resource.usage,
            resource.texturePointer,
            resource.surfacePointer,
            resource.textureBacked
                ? "true"
                : "false"
        );
    }

    fprintf(
        file,
        "\n  ],\n"
        "  \"passes\": [\n"
    );

    bool firstPassJson = true;

    for (const ProfilerPass& pass :
         g_profilerPasses)
    {
        if (pass.drawCalls == 0 &&
            pass.clears == 0 &&
            pass.stretchRects == 0)
        {
            continue;
        }

        if (!firstPassJson)
            fprintf(file, ",\n");

        firstPassJson = false;

        const ProfilerShaderInfo* dominantPs =
            FindProfilerShader(
                pass.dominantPs
            );

        const ProfilerShaderInfo* dominantVs =
            FindProfilerShader(
                pass.dominantVs
            );

        fprintf(
            file,
            "    {\"index\": %u, "
            "\"classification\": \"%s\", "
            "\"rt0\": {\"resource_id\": %u, "
            "\"tag\": \"%s\", "
            "\"ptr\": \"0x%p\", "
            "\"width\": %u, \"height\": %u, "
            "\"format\": %u}, "
            "\"depth\": {\"resource_id\": %u, "
            "\"tag\": \"%s\", "
            "\"ptr\": \"0x%p\", "
            "\"width\": %u, \"height\": %u, "
            "\"format\": %u}, "
            "\"viewport\": [%u, %u, %u, %u], "
            "\"draw_calls\": %u, "
            "\"primitives\": %llu, "
            "\"texture_binds\": %u, "
            "\"shader_binds\": %u, "
            "\"clears\": %u, "
            "\"stretch_rects\": %u, "
            "\"mrt\": %s, "
            "\"stage8_fullres\": %s, "
            "\"dominant_vs\": %u, "
            "\"dominant_vs_hash\": \"%016llX\", "
            "\"dominant_ps\": %u, "
            "\"dominant_ps_hash\": \"%016llX\", "
            "\"sampled_stages_mask\": %u, "
            "\"sampler_fallback_draws\": %u, "
            "\"gpu_ms\": %.6f, "
            "\"outputs\": [",
            pass.index,
            ClassifyProfilerPass(pass),
            pass.rt0.resourceId,
            ProfilerResourceTagName(pass.rt0.tag),
            pass.rt0.pointer,
            pass.rt0.width,
            pass.rt0.height,
            pass.rt0.format,
            pass.depth.resourceId,
            ProfilerResourceTagName(pass.depth.tag),
            pass.depth.pointer,
            pass.depth.width,
            pass.depth.height,
            pass.depth.format,
            pass.viewport.X,
            pass.viewport.Y,
            pass.viewport.Width,
            pass.viewport.Height,
            pass.drawCalls,
            pass.primitives,
            pass.textureBinds,
            pass.shaderBinds,
            pass.clears,
            pass.stretchRects,
            pass.sawMrt ? "true" : "false",
            pass.sawStage8FullRes ? "true" : "false",
            dominantVs != nullptr
                ? dominantVs->id
                : 0,
            dominantVs != nullptr
                ? dominantVs->hash
                : 0ull,
            dominantPs != nullptr
                ? dominantPs->id
                : 0,
            dominantPs != nullptr
                ? dominantPs->hash
                : 0ull,
            pass.sampledStagesMask,
            pass.fallbackSamplerDraws,
            pass.gpuTimingValid
                ? pass.gpuMs
                : 0.0
        );

        for (size_t outputIndex = 0;
             outputIndex < pass.outputResources.size();
             ++outputIndex)
        {
            const UINT resourceId =
                pass.outputResources[outputIndex];

            const ProfilerResourceInfo* resource =
                FindProfilerResource(resourceId);

            if (outputIndex != 0)
                fprintf(file, ", ");

            fprintf(
                file,
                "{\"resource_id\": %u, "
                "\"tag\": \"%s\"}",
                resourceId,
                resource != nullptr
                    ? ProfilerResourceTagName(resource->tag)
                    : "Unknown"
            );
        }

        fprintf(file, "], \"depth_outputs\": [");

        for (size_t outputIndex = 0;
             outputIndex < pass.depthOutputResources.size();
             ++outputIndex)
        {
            const UINT resourceId =
                pass.depthOutputResources[outputIndex];

            const ProfilerResourceInfo* resource =
                FindProfilerResource(resourceId);

            if (outputIndex != 0)
                fprintf(file, ", ");

            fprintf(
                file,
                "{\"resource_id\": %u, "
                "\"tag\": \"%s\"}",
                resourceId,
                resource != nullptr
                    ? ProfilerResourceTagName(resource->tag)
                    : "Unknown"
            );
        }

        fprintf(file, "], \"dependencies\": [");

        for (size_t dependencyIndex = 0;
             dependencyIndex < pass.dependencies.size();
             ++dependencyIndex)
        {
            const ProfilerDependency& dependency =
                pass.dependencies[dependencyIndex];

            const ProfilerResourceInfo* resource =
                FindProfilerResource(
                    dependency.resourceId
                );

            if (dependencyIndex != 0)
                fprintf(file, ", ");

            fprintf(
                file,
                "{\"stage\": %u, "
                "\"resource_id\": %u, "
                "\"tag\": \"%s\", "
                "\"producer_pass\": %d}",
                dependency.stage,
                dependency.resourceId,
                resource != nullptr
                    ? ProfilerResourceTagName(resource->tag)
                    : "Unknown",
                dependency.producerPass
            );
        }

        fprintf(file, "]}");
    }

    fprintf(
        file,
        "\n  ],\n"
        "  \"shaders\": [\n"
    );

    for (size_t i = 0;
         i < g_profilerShaders.size();
         ++i)
    {
        const ProfilerShaderInfo& shader =
            g_profilerShaders[i];

        fprintf(
            file,
            "    {\"id\": %u, \"type\": \"%s\", "
            "\"pointer\": \"0x%p\", "
            "\"hash\": \"%016llX\", "
            "\"bytecode_size\": %u, "
            "\"shader_model\": [%u, %u], "
            "\"sampler_parse_valid\": %s, "
            "\"sampler_mask\": %u, "
            "\"declared_sampler_mask\": %u}%s\n",
            shader.id,
            shader.pixel ? "PS" : "VS",
            shader.pointer,
            shader.hash,
            shader.bytecodeSize,
            shader.shaderMajor,
            shader.shaderMinor,
            shader.samplerMaskValid ? "true" : "false",
            shader.samplerMask,
            shader.declaredSamplerMask,
            (i + 1 < g_profilerShaders.size())
                ? ","
                : ""
        );
    }

    fprintf(
        file,
        "  ],\n"
        "  \"draw_callers\": [\n"
    );

    std::vector<const ProfilerCallerStats*> callerJson;
    callerJson.reserve(g_profilerCallers.size());

    for (const auto& entry :
         g_profilerCallers)
    {
        callerJson.push_back(&entry.second);
    }

    std::sort(
        callerJson.begin(),
        callerJson.end(),
        [](const ProfilerCallerStats* a,
           const ProfilerCallerStats* b)
        {
            if (a->drawCalls != b->drawCalls)
                return a->drawCalls > b->drawCalls;

            return a->address < b->address;
        }
    );

    for (size_t callerIndex = 0;
         callerIndex < callerJson.size();
         ++callerIndex)
    {
        const ProfilerCallerStats* stats =
            callerJson[callerIndex];

        fprintf(
            file,
            "    {\"address\": \"0x%p\", "
            "\"in_main_exe\": %s, "
            "\"rva\": %u, "
            "\"draw_calls\": %llu, "
            "\"primitives\": %llu, "
            "\"passes\": [",
            reinterpret_cast<void*>(
                stats->address
            ),
            stats->inMainExe ? "true" : "false",
            stats->rva,
            stats->drawCalls,
            stats->primitives
        );

        bool firstPass = true;

        for (const auto& passEntry :
             stats->passDrawCounts)
        {
            if (!firstPass)
                fprintf(file, ", ");

            fprintf(
                file,
                "{\"pass\": %u, \"draws\": %u}",
                passEntry.first,
                passEntry.second
            );

            firstPass = false;
        }

        fprintf(file, "], \"pixel_shaders\": [");

        bool firstPs = true;

        for (const auto& psEntry :
             stats->psDrawCounts)
        {
            if (!firstPs)
                fprintf(file, ", ");

            const ProfilerShaderInfo* shader =
                FindProfilerShader(
                    psEntry.first
                );

            fprintf(
                file,
                "{\"id\": %u, "
                "\"hash\": \"%016llX\", "
                "\"draws\": %u}",
                psEntry.first,
                shader != nullptr
                    ? shader->hash
                    : 0ull,
                psEntry.second
            );

            firstPs = false;
        }

        fprintf(
            file,
            "]}%s\n",
            (callerIndex + 1 <
                callerJson.size())
                ? ","
                : ""
        );
    }

    fprintf(
        file,
        "  ],\n"
        "  \"candidate_owner_calls\": [\n"
    );

    for (size_t i = 0;
         i < g_profilerCandidateOwnerCalls.size();
         ++i)
    {
        const ProfilerCandidateOwnerCall& record =
            g_profilerCandidateOwnerCalls[i];

        fprintf(
            file,
            "    {\"t_ms\": %.6f, \"owner\": \"0x%08X\", "
            "\"caller_rva\": %u, \"result\": %u, "
            "\"vtable\": \"0x%08X\", \"type\": %u, "
            "\"disable_flags\": %u, \"flags_d8\": \"%016llX\", "
            "\"flags_138\": \"%016llX\", \"raw_render_object\": \"0x%08X\", "
            "\"class_458\": %u, \"class_45a\": %u}%s\n",
            ProfilerMilliseconds(record.ticks),
            static_cast<UINT>(record.owner),
            record.callerRva,
            static_cast<unsigned>(record.result),
            static_cast<UINT>(record.vtable),
            static_cast<unsigned>(record.type),
            static_cast<unsigned>(record.disableFlags),
            record.flagsD8,
            record.flags138,
            static_cast<UINT>(record.rawRenderObject),
            static_cast<unsigned>(record.classField458),
            static_cast<unsigned>(record.classField45A),
            (i + 1 < g_profilerCandidateOwnerCalls.size()) ? "," : ""
        );
    }

    fprintf(
        file,
        "  ],\n"
        "  \"scene_owner_calls\": [\n"
    );

    for (size_t i = 0;
         i < g_profilerSceneOwnerCalls.size();
         ++i)
    {
        const ProfilerSceneOwnerCall& record =
            g_profilerSceneOwnerCalls[i];

        fprintf(
            file,
            "    {\"t_ms\": %.6f, \"owner\": \"0x%08X\", "
            "\"caller_rva\": %u, \"result\": \"0x%08X\", "
            "\"vtable\": \"0x%08X\", \"type\": %u, "
            "\"disable_flags\": %u, \"flags_d8\": \"%016llX\", "
            "\"flags_138\": \"%016llX\", \"raw_render_object\": \"0x%08X\"}%s\n",
            ProfilerMilliseconds(record.ticks),
            static_cast<UINT>(record.owner),
            record.callerRva,
            static_cast<UINT>(record.result),
            static_cast<UINT>(record.vtable),
            static_cast<unsigned>(record.type),
            static_cast<unsigned>(record.disableFlags),
            record.flagsD8,
            record.flags138,
            static_cast<UINT>(record.rawRenderObject),
            (i + 1 < g_profilerSceneOwnerCalls.size()) ? "," : ""
        );
    }

    fprintf(
        file,
        "  ],\n"
        "  \"events\": [\n"
    );

    for (size_t i = 0;
         i < g_profilerEvents.size();
         ++i)
    {
        const ProfilerEvent& event =
            g_profilerEvents[i];

        fprintf(
            file,
            "    {\"t_ms\": %.6f, "
            "\"type\": \"%s\", "
            "\"v\": [%llu, %llu, %llu, %llu, %llu, %llu, %llu, %llu], "
            "\"caller_address\": \"0x%p\", "
            "\"caller_in_main_exe\": %s, "
            "\"caller_rva\": %u, "
            "\"pass_index\": %d, "
            "\"vs_id\": %u, "
            "\"ps_id\": %u, "
            "\"scene_object\": \"0x%08X\", "
            "\"scene_owner\": \"0x%08X\", "
            "\"object_caller_rva\": %u}%s\n",
            ProfilerMilliseconds(event.ticks),
            ProfilerEventName(event.type),
            event.v[0],
            event.v[1],
            event.v[2],
            event.v[3],
            event.v[4],
            event.v[5],
            event.v[6],
            event.v[7],
            reinterpret_cast<void*>(
                event.callerAddress
            ),
            event.callerInMainExe
                ? "true"
                : "false",
            event.callerRva,
            event.passIndex,
            event.vertexShaderId,
            event.pixelShaderId,
            static_cast<UINT>(event.v[6]),
            static_cast<UINT>(event.v[7] >> 32),
            static_cast<UINT>(event.v[7] & 0xFFFFFFFFull),
            (i + 1 < g_profilerEvents.size())
                ? ","
                : ""
        );
    }

    fprintf(
        file,
        "  ]\n"
        "}\n"
    );

    fclose(file);
}


static void FinishProfilerCapture()
{
    DumpProfilerText();
    DumpProfilerJson();
    DumpProfilerShaders();

    char text[512] = {};

    sprintf_s(
        text,
        "[Profiler] Capture #%u finished: %zu events, %zu passes%s, "
        "GPU=%s%s. Files: DPFixNG-frame-%04u.txt / .json\n",
        g_profilerCurrentCapture,
        g_profilerEvents.size(),
        g_profilerPasses.size(),
        g_profilerOverflow ? " (TRUNCATED)" : "",
        g_profilerGpuValid ? "valid" : "n/a",
        g_profilerGpuValid ? "" : "",
        g_profilerCurrentCapture
    );

    AppendLog(text);

    ReleaseProfilerGpuQueries();
}


