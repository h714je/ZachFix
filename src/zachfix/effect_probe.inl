#include <cstdarg>

// -----------------------------------------------------------------------------
// Effect Frame Probe (research build only)
// -----------------------------------------------------------------------------
//
// F8 toggles a compact D3D9 trace on/off. The trace is intentionally focused
// on render-target/viewport/scissor/composite/draw state so short-lived 720p
// post effects can be identified without restoring the old full profiler.
//

static std::atomic_bool g_effectProbeActive{ false };
static std::mutex g_effectProbeMutex;
static FILE* g_effectProbeFile = nullptr;
static unsigned long long g_effectProbeFrame = 0;
static unsigned long long g_effectProbeEvent = 0;
static bool g_effectProbeF8Held = false;
static unsigned long long g_effectProbeCapturedShaderPairs[256] = {};
static size_t g_effectProbeCapturedShaderPairCount = 0;
static std::atomic_bool g_effectProbeSkipPs93AC9D9C{ false };
static std::atomic_bool g_effectProbeSkipPsF4D0BDDE{ false };
static std::atomic_bool g_effectProbeDpfixEnemyShadowTrailFix{ false };
static std::atomic_uint g_effectProbeCurrentPsHash{ 0 };

struct EffectProbeShaderHashEntry
{
    IDirect3DPixelShader9* shader = nullptr;
    unsigned hash = 0;
};

static EffectProbeShaderHashEntry g_effectProbePixelShaderHashes[256] = {};
static size_t g_effectProbePixelShaderHashCount = 0;

EffectProbeIsolationSettings GetEffectProbeIsolationSettings()
{
    EffectProbeIsolationSettings settings{};
    settings.skipPs93AC9D9C =
        g_effectProbeSkipPs93AC9D9C.load(std::memory_order_relaxed);
    settings.skipPsF4D0BDDE =
        g_effectProbeSkipPsF4D0BDDE.load(std::memory_order_relaxed);
    settings.dpfixEnemyShadowTrailFix =
        g_effectProbeDpfixEnemyShadowTrailFix.load(std::memory_order_relaxed);
    return settings;
}

void SetEffectProbeIsolationSettings(const EffectProbeIsolationSettings& settings)
{
    g_effectProbeSkipPs93AC9D9C.store(
        settings.skipPs93AC9D9C,
        std::memory_order_relaxed);
    g_effectProbeSkipPsF4D0BDDE.store(
        settings.skipPsF4D0BDDE,
        std::memory_order_relaxed);
    g_effectProbeDpfixEnemyShadowTrailFix.store(
        settings.dpfixEnemyShadowTrailFix,
        std::memory_order_relaxed);
}

static D3DVIEWPORT9 g_effectProbeViewport =
{
    0, 0, kBaseRenderWidth, kBaseRenderHeight, 0.0f, 1.0f
};
static RECT g_effectProbeScissor =
{
    0, 0,
    static_cast<LONG>(kBaseRenderWidth),
    static_cast<LONG>(kBaseRenderHeight)
};
static DWORD g_effectProbeFvf = 0;
static bool g_effectProbeUsesFvf = false;
static IDirect3DVertexDeclaration9* g_effectProbeVertexDecl = nullptr;
static IDirect3DVertexShader9* g_effectProbeVs = nullptr;
static IDirect3DPixelShader9* g_effectProbePs = nullptr;
static IDirect3DBaseTexture9* g_effectProbeTexture0 = nullptr;
static UINT g_effectProbeRt0Width = 0;
static UINT g_effectProbeRt0Height = 0;
static UINT g_effectProbeTexture0Width = 0;
static UINT g_effectProbeTexture0Height = 0;
static DWORD g_effectProbeScissorEnabled = 0;
static DWORD g_effectProbeAlphaBlendEnabled = 0;
static DWORD g_effectProbeZWriteEnabled = 0;

static bool GetEffectProbePath(wchar_t* path, size_t pathCount)
{
    if (path == nullptr || pathCount == 0)
        return false;

    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&g_effectProbeActive),
            &module))
    {
        return false;
    }

    const DWORD length = GetModuleFileNameW(
        module,
        path,
        static_cast<DWORD>(pathCount));

    if (length == 0 || length >= pathCount)
        return false;

    wchar_t* slash = wcsrchr(path, L'\\');
    if (slash == nullptr)
        return false;

    *(slash + 1) = L'\0';
    return wcscat_s(
        path,
        pathCount,
        L"ZachFix-effect-probe.log") == 0;
}

static unsigned EffectProbeCallerRva(const void* caller)
{
    if (!g_mainExeInfoValid)
        InitializeMainExeInfo();

    const uintptr_t address = reinterpret_cast<uintptr_t>(caller);
    if (!g_mainExeInfoValid ||
        address < g_mainExeBase ||
        address >= g_mainExeBase + g_mainExeSize)
    {
        return 0;
    }

    return static_cast<unsigned>(address - g_mainExeBase);
}

static void EffectProbeWriteUnlocked(const char* format, ...)
{
    if (g_effectProbeFile == nullptr)
        return;

    std::fprintf(
        g_effectProbeFile,
        "F=%llu E=%llu ",
        g_effectProbeFrame,
        ++g_effectProbeEvent);

    va_list args;
    va_start(args, format);
    std::vfprintf(g_effectProbeFile, format, args);
    va_end(args);

    std::fputc('\n', g_effectProbeFile);
}

static void EffectProbeWrite(const char* format, ...)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed))
        return;

    std::lock_guard<std::mutex> lock(g_effectProbeMutex);
    if (g_effectProbeFile == nullptr)
        return;

    std::fprintf(
        g_effectProbeFile,
        "F=%llu E=%llu ",
        g_effectProbeFrame,
        ++g_effectProbeEvent);

    va_list args;
    va_start(args, format);
    std::vfprintf(g_effectProbeFile, format, args);
    va_end(args);

    std::fputc('\n', g_effectProbeFile);
}

static bool EffectProbeGetSurfaceDesc(
    IDirect3DSurface9* surface,
    D3DSURFACE_DESC* desc)
{
    if (surface == nullptr || desc == nullptr)
        return false;

    std::memset(desc, 0, sizeof(*desc));
    return SUCCEEDED(surface->GetDesc(desc));
}

static void EffectProbeFormatRect(
    const RECT* rect,
    char* text,
    size_t textSize)
{
    if (text == nullptr || textSize == 0)
        return;

    if (rect == nullptr)
    {
        strcpy_s(text, textSize, "FULL");
        return;
    }

    sprintf_s(
        text,
        textSize,
        "%ld,%ld-%ld,%ld (%ldx%ld)",
        rect->left,
        rect->top,
        rect->right,
        rect->bottom,
        rect->right - rect->left,
        rect->bottom - rect->top);
}

static void EffectProbeSnapshot(IDirect3DDevice9* device)
{
    if (device == nullptr ||
        !g_effectProbeActive.load(std::memory_order_relaxed))
    {
        return;
    }

    D3DVIEWPORT9 viewport{};
    if (SUCCEEDED(device->GetViewport(&viewport)))
        g_effectProbeViewport = viewport;

    RECT scissor{};
    if (SUCCEEDED(device->GetScissorRect(&scissor)))
        g_effectProbeScissor = scissor;

    DWORD fvf = 0;
    if (SUCCEEDED(device->GetFVF(&fvf)))
    {
        g_effectProbeFvf = fvf;
        g_effectProbeUsesFvf = fvf != 0;
    }

    IDirect3DVertexDeclaration9* decl = nullptr;
    if (SUCCEEDED(device->GetVertexDeclaration(&decl)))
    {
        g_effectProbeVertexDecl = decl;
        if (decl != nullptr)
            decl->Release();
    }

    IDirect3DVertexShader9* vs = nullptr;
    if (SUCCEEDED(device->GetVertexShader(&vs)))
    {
        g_effectProbeVs = vs;
        if (vs != nullptr)
            vs->Release();
    }

    IDirect3DPixelShader9* ps = nullptr;
    if (SUCCEEDED(device->GetPixelShader(&ps)))
    {
        g_effectProbePs = ps;
        if (ps != nullptr)
            ps->Release();
    }

    IDirect3DBaseTexture9* texture0 = nullptr;
    if (SUCCEEDED(device->GetTexture(0, &texture0)))
    {
        g_effectProbeTexture0 = texture0;
        g_effectProbeTexture0Width = 0;
        g_effectProbeTexture0Height = 0;

        if (texture0 != nullptr && texture0->GetType() == D3DRTYPE_TEXTURE)
        {
            D3DSURFACE_DESC textureDesc{};
            IDirect3DTexture9* texture = static_cast<IDirect3DTexture9*>(texture0);
            if (SUCCEEDED(texture->GetLevelDesc(0, &textureDesc)))
            {
                g_effectProbeTexture0Width = textureDesc.Width;
                g_effectProbeTexture0Height = textureDesc.Height;
            }
        }

        if (texture0 != nullptr)
            texture0->Release();
    }

    IDirect3DSurface9* rt = nullptr;
    D3DSURFACE_DESC rtDesc{};
    const bool haveRt =
        SUCCEEDED(device->GetRenderTarget(0, &rt)) &&
        rt != nullptr &&
        EffectProbeGetSurfaceDesc(rt, &rtDesc);

    DWORD scissorEnabled = 0;
    DWORD alphaBlendEnabled = 0;
    DWORD zWriteEnabled = 0;
    device->GetRenderState(D3DRS_SCISSORTESTENABLE, &scissorEnabled);
    device->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphaBlendEnabled);
    device->GetRenderState(D3DRS_ZWRITEENABLE, &zWriteEnabled);

    g_effectProbeScissorEnabled = scissorEnabled;
    g_effectProbeAlphaBlendEnabled = alphaBlendEnabled;
    g_effectProbeZWriteEnabled = zWriteEnabled;
    if (haveRt)
    {
        g_effectProbeRt0Width = rtDesc.Width;
        g_effectProbeRt0Height = rtDesc.Height;
    }

    {
        std::lock_guard<std::mutex> lock(g_effectProbeMutex);
        if (g_effectProbeFile != nullptr)
        {
            EffectProbeWriteUnlocked(
                "SNAPSHOT RT0=%p %ux%u fmt=%u VP=%u,%u %ux%u SC=%ld,%ld-%ld,%ld "
                "SC_EN=%lu ALPHA=%lu ZWRITE=%lu FVF=0x%08lX DECL=%p VS=%p PS=%p TEX0=%p",
                rt,
                haveRt ? rtDesc.Width : 0,
                haveRt ? rtDesc.Height : 0,
                haveRt ? static_cast<unsigned>(rtDesc.Format) : 0,
                viewport.X,
                viewport.Y,
                viewport.Width,
                viewport.Height,
                scissor.left,
                scissor.top,
                scissor.right,
                scissor.bottom,
                scissorEnabled,
                alphaBlendEnabled,
                zWriteEnabled,
                fvf,
                g_effectProbeVertexDecl,
                g_effectProbeVs,
                g_effectProbePs,
                g_effectProbeTexture0);
            std::fflush(g_effectProbeFile);
        }
    }

    if (rt != nullptr)
        rt->Release();
}

static void EffectProbeStart(IDirect3DDevice9* device)
{
    std::lock_guard<std::mutex> lock(g_effectProbeMutex);
    if (g_effectProbeFile != nullptr)
        return;

    wchar_t path[MAX_PATH] = {};
    if (!GetEffectProbePath(path, MAX_PATH))
    {
        AppendLog("[EffectProbe] ERROR: could not resolve trace path.\n");
        return;
    }

    FILE* file = nullptr;
    if (_wfopen_s(&file, path, L"w") != 0 || file == nullptr)
    {
        AppendLog("[EffectProbe] ERROR: could not create ZachFix-effect-probe.log.\n");
        return;
    }

    g_effectProbeFile = file;
    g_effectProbeFrame = 0;
    g_effectProbeEvent = 0;
    g_effectProbeCapturedShaderPairCount = 0;
    std::memset(g_effectProbeCapturedShaderPairs, 0, sizeof(g_effectProbeCapturedShaderPairs));
    g_effectProbeActive.store(true, std::memory_order_release);

    InitializeMainExeInfo();

    std::fprintf(
        g_effectProbeFile,
        "ZachFix Effect Frame Probe\n"
        "Build: %s\n"
        "F8 toggles capture. Shader Isolation v4 adds DPFix trail testing and targeted shader skips.\n"
        "Internal=%u x %u Display=%u x %u\n"
        "DP.exe base=0x%08llX size=0x%llX timestamp=0x%08lX\n\n",
        kZachFixDisplayName,
        g_internalWidth,
        g_internalHeight,
        g_displayWidth,
        g_displayHeight,
        static_cast<unsigned long long>(g_mainExeBase),
        static_cast<unsigned long long>(g_mainExeSize),
        static_cast<unsigned long>(g_mainExeTimeDateStamp));
    std::fflush(g_effectProbeFile);

    AppendLog("[EffectProbe] Capture started. Press F8 again after the effect occurs.\n");

    // Snapshot after releasing this lock; Snapshot takes the same mutex while
    // writing its single state line.
    (void)device;
}

static void EffectProbeStop()
{
    std::lock_guard<std::mutex> lock(g_effectProbeMutex);

    if (g_effectProbeFile == nullptr)
    {
        g_effectProbeActive.store(false, std::memory_order_release);
        return;
    }

    EffectProbeWriteUnlocked("CAPTURE_STOP");
    std::fflush(g_effectProbeFile);
    std::fclose(g_effectProbeFile);
    g_effectProbeFile = nullptr;
    g_effectProbeActive.store(false, std::memory_order_release);

    AppendLog("[EffectProbe] Capture stopped. Saved ZachFix-effect-probe.log.\n");
}

static void EffectProbePollHotkey(IDirect3DDevice9* device)
{
    const bool down = (GetAsyncKeyState(VK_F8) & 0x8000) != 0;

    if (down && !g_effectProbeF8Held)
    {
        if (g_effectProbeActive.load(std::memory_order_acquire))
        {
            EffectProbeStop();
        }
        else
        {
            EffectProbeStart(device);
            if (g_effectProbeActive.load(std::memory_order_acquire))
                EffectProbeSnapshot(device);
        }
    }

    g_effectProbeF8Held = down;
}

static void EffectProbeFrameBoundary(IDirect3DDevice9* device, const char* path)
{
    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        std::lock_guard<std::mutex> lock(g_effectProbeMutex);
        if (g_effectProbeFile != nullptr)
        {
            EffectProbeWriteUnlocked("PRESENT path=%s", path != nullptr ? path : "?");
            std::fflush(g_effectProbeFile);
            ++g_effectProbeFrame;
        }
    }

    EffectProbePollHotkey(device);
}

static void EffectProbeRecordSetRenderTarget(
    DWORD index,
    IDirect3DSurface9* logicalTarget,
    IDirect3DSurface9* effectiveTarget,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed))
        return;

    D3DSURFACE_DESC logicalDesc{};
    D3DSURFACE_DESC effectiveDesc{};
    const bool haveLogical = EffectProbeGetSurfaceDesc(logicalTarget, &logicalDesc);
    const bool haveEffective = EffectProbeGetSurfaceDesc(effectiveTarget, &effectiveDesc);

    if (index == 0 && haveEffective)
    {
        g_effectProbeRt0Width = effectiveDesc.Width;
        g_effectProbeRt0Height = effectiveDesc.Height;
    }

    EffectProbeWrite(
        "SetRT idx=%lu caller=+0x%06X logical=%p %ux%u fmt=%u effective=%p %ux%u fmt=%u",
        index,
        EffectProbeCallerRva(caller),
        logicalTarget,
        haveLogical ? logicalDesc.Width : 0,
        haveLogical ? logicalDesc.Height : 0,
        haveLogical ? static_cast<unsigned>(logicalDesc.Format) : 0,
        effectiveTarget,
        haveEffective ? effectiveDesc.Width : 0,
        haveEffective ? effectiveDesc.Height : 0,
        haveEffective ? static_cast<unsigned>(effectiveDesc.Format) : 0);
}

static void EffectProbeRecordSetDepth(
    IDirect3DSurface9* logicalDepth,
    IDirect3DSurface9* effectiveDepth,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed))
        return;

    D3DSURFACE_DESC logicalDesc{};
    D3DSURFACE_DESC effectiveDesc{};
    const bool haveLogical = EffectProbeGetSurfaceDesc(logicalDepth, &logicalDesc);
    const bool haveEffective = EffectProbeGetSurfaceDesc(effectiveDepth, &effectiveDesc);

    EffectProbeWrite(
        "SetDS caller=+0x%06X logical=%p %ux%u fmt=%u effective=%p %ux%u fmt=%u",
        EffectProbeCallerRva(caller),
        logicalDepth,
        haveLogical ? logicalDesc.Width : 0,
        haveLogical ? logicalDesc.Height : 0,
        haveLogical ? static_cast<unsigned>(logicalDesc.Format) : 0,
        effectiveDepth,
        haveEffective ? effectiveDesc.Width : 0,
        haveEffective ? effectiveDesc.Height : 0,
        haveEffective ? static_cast<unsigned>(effectiveDesc.Format) : 0);
}

static void EffectProbeRecordViewportRequest(
    IDirect3DDevice9* device,
    const D3DVIEWPORT9* viewport,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed) || viewport == nullptr)
        return;

    IDirect3DSurface9* rt = nullptr;
    D3DSURFACE_DESC rtDesc{};
    const bool haveRt =
        device != nullptr &&
        SUCCEEDED(device->GetRenderTarget(0, &rt)) &&
        rt != nullptr &&
        EffectProbeGetSurfaceDesc(rt, &rtDesc);

    EffectProbeWrite(
        "SetViewportReq caller=+0x%06X req=%u,%u %ux%u z=%.3f..%.3f RT0=%p %ux%u fmt=%u",
        EffectProbeCallerRva(caller),
        viewport->X,
        viewport->Y,
        viewport->Width,
        viewport->Height,
        viewport->MinZ,
        viewport->MaxZ,
        rt,
        haveRt ? rtDesc.Width : 0,
        haveRt ? rtDesc.Height : 0,
        haveRt ? static_cast<unsigned>(rtDesc.Format) : 0);

    if (rt != nullptr)
        rt->Release();
}

static void EffectProbeRecordViewportSubmit(
    const D3DVIEWPORT9* viewport)
{
    if (viewport == nullptr)
        return;

    g_effectProbeViewport = *viewport;

    EffectProbeWrite(
        "SetViewportSubmit effective=%u,%u %ux%u z=%.3f..%.3f RT=%ux%u",
        viewport->X,
        viewport->Y,
        viewport->Width,
        viewport->Height,
        viewport->MinZ,
        viewport->MaxZ,
        g_effectProbeRt0Width,
        g_effectProbeRt0Height);
}


static void EffectProbeRecordStretchRect(
    IDirect3DSurface9* logicalSource,
    IDirect3DSurface9* effectiveSource,
    const RECT* sourceRect,
    IDirect3DSurface9* logicalDest,
    IDirect3DSurface9* effectiveDest,
    const RECT* destRect,
    D3DTEXTUREFILTERTYPE filter,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed))
        return;

    D3DSURFACE_DESC sourceDesc{};
    D3DSURFACE_DESC destDesc{};
    const bool haveSource = EffectProbeGetSurfaceDesc(effectiveSource, &sourceDesc);
    const bool haveDest = EffectProbeGetSurfaceDesc(effectiveDest, &destDesc);
    char srcRectText[96] = {};
    char dstRectText[96] = {};
    EffectProbeFormatRect(sourceRect, srcRectText, sizeof(srcRectText));
    EffectProbeFormatRect(destRect, dstRectText, sizeof(dstRectText));

    EffectProbeWrite(
        "StretchRect caller=+0x%06X srcL=%p src=%p %ux%u fmt=%u rect=%s "
        "dstL=%p dst=%p %ux%u fmt=%u rect=%s filter=%u",
        EffectProbeCallerRva(caller),
        logicalSource,
        effectiveSource,
        haveSource ? sourceDesc.Width : 0,
        haveSource ? sourceDesc.Height : 0,
        haveSource ? static_cast<unsigned>(sourceDesc.Format) : 0,
        srcRectText,
        logicalDest,
        effectiveDest,
        haveDest ? destDesc.Width : 0,
        haveDest ? destDesc.Height : 0,
        haveDest ? static_cast<unsigned>(destDesc.Format) : 0,
        dstRectText,
        static_cast<unsigned>(filter));
}

static void EffectProbeRecordTexture(
    DWORD stage,
    IDirect3DBaseTexture9* logicalTexture,
    IDirect3DBaseTexture9* effectiveTexture,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed) || stage != 0)
        return;

    g_effectProbeTexture0 = effectiveTexture;

    UINT width = 0;
    UINT height = 0;
    D3DFORMAT format = D3DFMT_UNKNOWN;

    if (effectiveTexture != nullptr &&
        effectiveTexture->GetType() == D3DRTYPE_TEXTURE)
    {
        D3DSURFACE_DESC desc{};
        IDirect3DTexture9* texture =
            static_cast<IDirect3DTexture9*>(effectiveTexture);
        if (SUCCEEDED(texture->GetLevelDesc(0, &desc)))
        {
            width = desc.Width;
            height = desc.Height;
            format = desc.Format;
        }
    }

    g_effectProbeTexture0Width = width;
    g_effectProbeTexture0Height = height;

    EffectProbeWrite(
        "SetTexture0 caller=+0x%06X logical=%p effective=%p %ux%u fmt=%u",
        EffectProbeCallerRva(caller),
        logicalTexture,
        effectiveTexture,
        width,
        height,
        static_cast<unsigned>(format));
}

static unsigned EffectProbeHashBytes(const unsigned char* data, UINT size)
{
    unsigned hash = 2166136261u;
    if (data == nullptr)
        return hash;
    for (UINT i = 0; i < size; ++i)
    {
        hash ^= data[i];
        hash *= 16777619u;
    }
    return hash;
}

template <typename TShader>
static bool EffectProbeReadShaderBytecode(
    TShader* shader,
    unsigned char** bytes,
    UINT* size,
    unsigned* hash)
{
    if (bytes == nullptr || size == nullptr || hash == nullptr || shader == nullptr)
        return false;
    *bytes = nullptr;
    *size = 0;
    *hash = 0;

    UINT byteCount = 0;
    if (FAILED(shader->GetFunction(nullptr, &byteCount)) || byteCount == 0)
        return false;

    unsigned char* buffer = static_cast<unsigned char*>(
        HeapAlloc(GetProcessHeap(), 0, byteCount));
    if (buffer == nullptr)
        return false;

    UINT actual = byteCount;
    if (FAILED(shader->GetFunction(buffer, &actual)) || actual == 0)
    {
        HeapFree(GetProcessHeap(), 0, buffer);
        return false;
    }

    *bytes = buffer;
    *size = actual;
    *hash = EffectProbeHashBytes(buffer, actual);
    return true;
}

static unsigned EffectProbeGetPixelShaderHash(IDirect3DPixelShader9* shader)
{
    if (shader == nullptr)
        return 0;

    for (size_t i = 0; i < g_effectProbePixelShaderHashCount; ++i)
    {
        if (g_effectProbePixelShaderHashes[i].shader == shader)
            return g_effectProbePixelShaderHashes[i].hash;
    }

    unsigned char* bytes = nullptr;
    UINT size = 0;
    unsigned hash = 0;
    if (!EffectProbeReadShaderBytecode(shader, &bytes, &size, &hash))
        return 0;

    HeapFree(GetProcessHeap(), 0, bytes);

    const size_t capacity =
        sizeof(g_effectProbePixelShaderHashes) / sizeof(g_effectProbePixelShaderHashes[0]);
    if (g_effectProbePixelShaderHashCount < capacity)
    {
        g_effectProbePixelShaderHashes[g_effectProbePixelShaderHashCount++] =
            { shader, hash };
    }

    return hash;
}

static bool EffectProbeShouldSkipCurrentPixelShader()
{
    const unsigned hash =
        g_effectProbeCurrentPsHash.load(std::memory_order_relaxed);

    return
        (hash == 0x93AC9D9Cu &&
         g_effectProbeSkipPs93AC9D9C.load(std::memory_order_relaxed)) ||
        (hash == 0xF4D0BDDEu &&
         g_effectProbeSkipPsF4D0BDDE.load(std::memory_order_relaxed));
}

static bool EffectProbeIsDpfixTrailStream(UINT offsetInBytes, UINT stride)
{
    return
        stride == 24 &&
        (offsetInBytes == 96 || offsetInBytes == 192);
}

static void EffectProbeRecordVertexConstant254(
    UINT startRegister,
    const float* constantData,
    UINT vector4fCount,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed) ||
        constantData == nullptr ||
        startRegister > 254 ||
        startRegister + vector4fCount <= 254)
    {
        return;
    }

    const UINT vectorIndex = 254 - startRegister;
    const float* value = constantData + vectorIndex * 4;
    EffectProbeWrite(
        "SetVSConst254 caller=+0x%06X value={%.9g,%.9g,%.9g,%.9g} start=%u count=%u",
        EffectProbeCallerRva(caller),
        value[0], value[1], value[2], value[3],
        startRegister,
        vector4fCount);
}

static bool EffectProbeGetModuleDirectory(wchar_t* path, size_t pathCount)
{
    if (path == nullptr || pathCount == 0)
        return false;
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&g_effectProbeActive),
            &module))
        return false;

    const DWORD length = GetModuleFileNameW(module, path, static_cast<DWORD>(pathCount));
    if (length == 0 || length >= pathCount)
        return false;
    wchar_t* slash = wcsrchr(path, L'\\');
    if (slash == nullptr)
        return false;
    *slash = L'\0';
    return true;
}

static void EffectProbeDumpShaderBinary(
    const wchar_t* stage,
    unsigned hash,
    const unsigned char* bytes,
    UINT size)
{
    if (stage == nullptr || bytes == nullptr || size == 0)
        return;

    wchar_t directory[MAX_PATH] = {};
    if (!EffectProbeGetModuleDirectory(directory, MAX_PATH))
        return;
    if (wcscat_s(directory, MAX_PATH, L"\\ZachFix-probe-shaders") != 0)
        return;

    if (!CreateDirectoryW(directory, nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        return;

    wchar_t path[MAX_PATH] = {};
    if (swprintf_s(path, MAX_PATH, L"%s\\%s_%08X.bin", directory, stage, hash) < 0)
        return;

    if (GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES)
        return;

    FILE* file = nullptr;
    if (_wfopen_s(&file, path, L"wb") != 0 || file == nullptr)
        return;
    std::fwrite(bytes, 1, size, file);
    std::fclose(file);
}

static bool EffectProbeRememberShaderPair(unsigned vsHash, unsigned psHash)
{
    const unsigned long long key =
        (static_cast<unsigned long long>(vsHash) << 32) |
        static_cast<unsigned long long>(psHash);
    for (size_t i = 0; i < g_effectProbeCapturedShaderPairCount; ++i)
    {
        if (g_effectProbeCapturedShaderPairs[i] == key)
            return false;
    }
    const size_t capacity =
        sizeof(g_effectProbeCapturedShaderPairs) / sizeof(g_effectProbeCapturedShaderPairs[0]);
    if (g_effectProbeCapturedShaderPairCount >= capacity)
        return false;
    g_effectProbeCapturedShaderPairs[g_effectProbeCapturedShaderPairCount++] = key;
    return true;
}

static bool EffectProbeLooksLikeScreenValue(float x)
{
    const float internalWidth = static_cast<float>(g_internalWidth);
    const float internalHeight = static_cast<float>(g_internalHeight);
    const float values[] =
    {
        1280.0f, 720.0f, 640.0f, 360.0f, 320.0f, 180.0f,
        internalWidth, internalHeight, internalWidth * 0.5f, internalHeight * 0.5f,
        1.0f / 1280.0f, 1.0f / 720.0f,
        0.5f / 1280.0f, 0.5f / 720.0f,
        1.0f / internalWidth, 1.0f / internalHeight,
        0.5f / internalWidth, 0.5f / internalHeight
    };
    for (float target : values)
    {
        const float eps = std::fabs(target) >= 1.0f ? 0.01f : 0.000002f;
        if (std::fabs(x - target) <= eps)
            return true;
    }
    return false;
}

static void EffectProbeDumpConstantsRange(
    IDirect3DDevice9* device,
    bool vertex,
    UINT firstRegister,
    UINT registerCount)
{
    const UINT endRegister = firstRegister + registerCount;
    for (UINT reg = firstRegister; reg < endRegister; ++reg)
    {
        float v[4] = {};
        const HRESULT hr = vertex
            ? device->GetVertexShaderConstantF(reg, v, 1)
            : device->GetPixelShaderConstantF(reg, v, 1);
        if (FAILED(hr))
            break;

        const bool nonZero =
            std::fabs(v[0]) > 0.0000001f || std::fabs(v[1]) > 0.0000001f ||
            std::fabs(v[2]) > 0.0000001f || std::fabs(v[3]) > 0.0000001f;
        if (!nonZero)
            continue;

        const bool screen =
            EffectProbeLooksLikeScreenValue(v[0]) || EffectProbeLooksLikeScreenValue(v[1]) ||
            EffectProbeLooksLikeScreenValue(v[2]) || EffectProbeLooksLikeScreenValue(v[3]);
        EffectProbeWrite(
            "%s c%u={%.9g,%.9g,%.9g,%.9g}%s",
            vertex ? "VS_CONST" : "PS_CONST",
            reg,
            v[0], v[1], v[2], v[3],
            screen ? " SCREEN_CANDIDATE" : "");
    }
}

static void EffectProbeDumpTextures(IDirect3DDevice9* device)
{
    for (DWORD stage = 0; stage < 16; ++stage)
    {
        IDirect3DBaseTexture9* texture = nullptr;
        UINT width = 0;
        UINT height = 0;
        D3DFORMAT format = D3DFMT_UNKNOWN;
        D3DRESOURCETYPE type = D3DRTYPE_SURFACE;
        if (SUCCEEDED(device->GetTexture(stage, &texture)) && texture != nullptr)
        {
            type = texture->GetType();
            if (type == D3DRTYPE_TEXTURE)
            {
                D3DSURFACE_DESC desc{};
                IDirect3DTexture9* texture2d = static_cast<IDirect3DTexture9*>(texture);
                if (SUCCEEDED(texture2d->GetLevelDesc(0, &desc)))
                {
                    width = desc.Width;
                    height = desc.Height;
                    format = desc.Format;
                }
            }
        }

        DWORD minFilter = 0, magFilter = 0, mipFilter = 0, addressU = 0, addressV = 0;
        device->GetSamplerState(stage, D3DSAMP_MINFILTER, &minFilter);
        device->GetSamplerState(stage, D3DSAMP_MAGFILTER, &magFilter);
        device->GetSamplerState(stage, D3DSAMP_MIPFILTER, &mipFilter);
        device->GetSamplerState(stage, D3DSAMP_ADDRESSU, &addressU);
        device->GetSamplerState(stage, D3DSAMP_ADDRESSV, &addressV);

        EffectProbeWrite(
            "TEX%lu ptr=%p type=%u %ux%u fmt=%u filter=%lu/%lu/%lu addr=%lu/%lu",
            stage, texture, static_cast<unsigned>(type), width, height,
            static_cast<unsigned>(format), minFilter, magFilter, mipFilter, addressU, addressV);
        if (texture != nullptr)
            texture->Release();
    }
}

static void EffectProbeCaptureSuspectShaderState(IDirect3DDevice9* device, UINT primitiveCount)
{
    if (device == nullptr ||
        !g_effectProbeActive.load(std::memory_order_relaxed))
        return;

    IDirect3DVertexShader9* vs = nullptr;
    IDirect3DPixelShader9* ps = nullptr;
    if (FAILED(device->GetVertexShader(&vs)) || FAILED(device->GetPixelShader(&ps)) ||
        vs == nullptr || ps == nullptr)
    {
        if (vs != nullptr) vs->Release();
        if (ps != nullptr) ps->Release();
        return;
    }

    unsigned char* vsBytes = nullptr;
    unsigned char* psBytes = nullptr;
    UINT vsSize = 0, psSize = 0;
    unsigned vsHash = 0, psHash = 0;
    const bool haveVs = EffectProbeReadShaderBytecode(vs, &vsBytes, &vsSize, &vsHash);
    const bool havePs = EffectProbeReadShaderBytecode(ps, &psBytes, &psSize, &psHash);

    if (!haveVs || !havePs || !EffectProbeRememberShaderPair(vsHash, psHash))
    {
        if (vsBytes != nullptr) HeapFree(GetProcessHeap(), 0, vsBytes);
        if (psBytes != nullptr) HeapFree(GetProcessHeap(), 0, psBytes);
        vs->Release();
        ps->Release();
        return;
    }

    EffectProbeDumpShaderBinary(L"VS", vsHash, vsBytes, vsSize);
    EffectProbeDumpShaderBinary(L"PS", psHash, psBytes, psSize);
    EffectProbeWrite(
        "=== SHADER_SWEEP prim=%u VS=%p hash=%08X bytes=%u PS=%p hash=%08X bytes=%u ===",
        primitiveCount, vs, vsHash, vsSize, ps, psHash, psSize);

    EffectProbeDumpTextures(device);

    // Avoid dumping the 216-register skinning palette for every material.
    // Low VS registers plus the high projection/fog block are enough to expose
    // fixed-resolution screen math. Pixel shader c0..c63 covers every external
    // constant used by the shaders seen in this game so far.
    EffectProbeDumpConstantsRange(device, true, 0, 23);
    EffectProbeDumpConstantsRange(device, true, 239, 17);
    EffectProbeDumpConstantsRange(device, false, 0, 64);
    EffectProbeWrite("=== END_SHADER_SWEEP ===");

    HeapFree(GetProcessHeap(), 0, vsBytes);
    HeapFree(GetProcessHeap(), 0, psBytes);
    vs->Release();
    ps->Release();
}

static UINT EffectProbePrimitiveVertexCount(
    D3DPRIMITIVETYPE type,
    UINT primitiveCount)
{
    switch (type)
    {
    case D3DPT_POINTLIST:
        return primitiveCount;
    case D3DPT_LINELIST:
        return primitiveCount * 2;
    case D3DPT_LINESTRIP:
        return primitiveCount + 1;
    case D3DPT_TRIANGLELIST:
        return primitiveCount * 3;
    case D3DPT_TRIANGLESTRIP:
    case D3DPT_TRIANGLEFAN:
        return primitiveCount + 2;
    default:
        return 0;
    }
}

static bool EffectProbeScreenSpaceBounds(
    const void* vertexData,
    UINT vertexCount,
    UINT stride,
    float* minX,
    float* minY,
    float* maxX,
    float* maxY)
{
    if (vertexData == nullptr ||
        vertexCount == 0 ||
        stride < sizeof(float) * 4 ||
        !g_effectProbeUsesFvf ||
        (g_effectProbeFvf & D3DFVF_XYZRHW) == 0 ||
        minX == nullptr || minY == nullptr ||
        maxX == nullptr || maxY == nullptr)
    {
        return false;
    }

    const UINT count = (std::min)(vertexCount, 128u);
    const unsigned char* bytes =
        static_cast<const unsigned char*>(vertexData);

    const float* first = reinterpret_cast<const float*>(bytes);
    *minX = *maxX = first[0];
    *minY = *maxY = first[1];

    for (UINT i = 1; i < count; ++i)
    {
        const float* position = reinterpret_cast<const float*>(bytes + i * stride);
        *minX = (std::min)(*minX, position[0]);
        *minY = (std::min)(*minY, position[1]);
        *maxX = (std::max)(*maxX, position[0]);
        *maxY = (std::max)(*maxY, position[1]);
    }

    return true;
}

static void EffectProbeRecordDraw(
    const char* name,
    D3DPRIMITIVETYPE type,
    UINT primitiveCount,
    const void* caller)
{
    if (!g_effectProbeActive.load(std::memory_order_relaxed))
        return;

    EffectProbeWrite(
        "%s caller=+0x%06X type=%u prim=%u RT=%ux%u VP=%u,%u %ux%u "
        "SC=%ld,%ld-%ld,%ld SC_EN=%lu ALPHA=%lu ZWRITE=%lu "
        "FVF=0x%08lX DECL=%p VS=%p PS=%p TEX0=%p %ux%u",
        name,
        EffectProbeCallerRva(caller),
        static_cast<unsigned>(type),
        primitiveCount,
        g_effectProbeRt0Width,
        g_effectProbeRt0Height,
        g_effectProbeViewport.X,
        g_effectProbeViewport.Y,
        g_effectProbeViewport.Width,
        g_effectProbeViewport.Height,
        g_effectProbeScissor.left,
        g_effectProbeScissor.top,
        g_effectProbeScissor.right,
        g_effectProbeScissor.bottom,
        g_effectProbeScissorEnabled,
        g_effectProbeAlphaBlendEnabled,
        g_effectProbeZWriteEnabled,
        g_effectProbeFvf,
        g_effectProbeVertexDecl,
        g_effectProbeVs,
        g_effectProbePs,
        g_effectProbeTexture0,
        g_effectProbeTexture0Width,
        g_effectProbeTexture0Height);
}

static HRESULT WINAPI HookProbeBeginScene(IDirect3DDevice9* self)
{
    EffectProbeWrite(
        "BeginScene caller=+0x%06X",
        EffectProbeCallerRva(_ReturnAddress()));
    return g_originalBeginScene(self);
}

static HRESULT WINAPI HookProbeClear(
    IDirect3DDevice9* self,
    DWORD count,
    const D3DRECT* rects,
    DWORD flags,
    D3DCOLOR color,
    float z,
    DWORD stencil)
{
    EffectProbeWrite(
        "Clear caller=+0x%06X rects=%lu flags=0x%08lX color=0x%08lX z=%.3f stencil=%lu",
        EffectProbeCallerRva(_ReturnAddress()),
        count,
        flags,
        color,
        z,
        stencil);

    return g_originalClear(self, count, rects, flags, color, z, stencil);
}

static HRESULT WINAPI HookProbeSetRenderState(
    IDirect3DDevice9* self,
    D3DRENDERSTATETYPE state,
    DWORD value)
{
    switch (state)
    {
    case D3DRS_SCISSORTESTENABLE:
        g_effectProbeScissorEnabled = value;
        break;
    case D3DRS_ALPHABLENDENABLE:
        g_effectProbeAlphaBlendEnabled = value;
        break;
    case D3DRS_ZWRITEENABLE:
        g_effectProbeZWriteEnabled = value;
        break;
    default:
        break;
    }

    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        switch (state)
        {
        case D3DRS_ZENABLE:
        case D3DRS_ZWRITEENABLE:
        case D3DRS_ALPHABLENDENABLE:
        case D3DRS_SRCBLEND:
        case D3DRS_DESTBLEND:
        case D3DRS_SCISSORTESTENABLE:
            EffectProbeWrite(
                "SetRenderState caller=+0x%06X state=%u value=%lu",
                EffectProbeCallerRva(_ReturnAddress()),
                static_cast<unsigned>(state),
                value);
            break;
        default:
            break;
        }
    }

    return g_originalSetRenderState(self, state, value);
}

static HRESULT WINAPI HookProbeSetScissorRect(
    IDirect3DDevice9* self,
    const RECT* rect)
{
    if (rect != nullptr)
        g_effectProbeScissor = *rect;

    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        IDirect3DSurface9* rt = nullptr;
        D3DSURFACE_DESC rtDesc{};
        const bool haveRt =
            SUCCEEDED(self->GetRenderTarget(0, &rt)) &&
            rt != nullptr &&
            EffectProbeGetSurfaceDesc(rt, &rtDesc);

        if (rect != nullptr)
        {
            EffectProbeWrite(
                "SetScissor caller=+0x%06X rect=%ld,%ld-%ld,%ld (%ldx%ld) RT0=%p %ux%u fmt=%u",
                EffectProbeCallerRva(_ReturnAddress()),
                rect->left,
                rect->top,
                rect->right,
                rect->bottom,
                rect->right - rect->left,
                rect->bottom - rect->top,
                rt,
                haveRt ? rtDesc.Width : 0,
                haveRt ? rtDesc.Height : 0,
                haveRt ? static_cast<unsigned>(rtDesc.Format) : 0);
        }
        else
        {
            EffectProbeWrite(
                "SetScissor caller=+0x%06X rect=NULL RT0=%p %ux%u fmt=%u",
                EffectProbeCallerRva(_ReturnAddress()),
                rt,
                haveRt ? rtDesc.Width : 0,
                haveRt ? rtDesc.Height : 0,
                haveRt ? static_cast<unsigned>(rtDesc.Format) : 0);
        }

        if (rt != nullptr)
            rt->Release();
    }

    return g_originalSetScissorRect(self, rect);
}

static HRESULT WINAPI HookProbeSetVertexDeclaration(
    IDirect3DDevice9* self,
    IDirect3DVertexDeclaration9* declaration)
{
    g_effectProbeVertexDecl = declaration;
    g_effectProbeUsesFvf = false;
    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        EffectProbeWrite(
            "SetVertexDecl caller=+0x%06X decl=%p",
            EffectProbeCallerRva(_ReturnAddress()),
            declaration);
    }

    return g_originalSetVertexDeclaration(self, declaration);
}

static HRESULT WINAPI HookProbeSetFVF(
    IDirect3DDevice9* self,
    DWORD fvf)
{
    g_effectProbeFvf = fvf;
    g_effectProbeUsesFvf = true;
    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        EffectProbeWrite(
            "SetFVF caller=+0x%06X fvf=0x%08lX",
            EffectProbeCallerRva(_ReturnAddress()),
            fvf);
    }

    return g_originalSetFVF(self, fvf);
}

static HRESULT WINAPI HookProbeSetVertexShader(
    IDirect3DDevice9* self,
    IDirect3DVertexShader9* shader)
{
    g_effectProbeVs = shader;
    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        EffectProbeWrite(
            "SetVS caller=+0x%06X shader=%p",
            EffectProbeCallerRva(_ReturnAddress()),
            shader);
    }

    return g_originalSetVertexShader(self, shader);
}

static HRESULT WINAPI HookProbeSetPixelShader(
    IDirect3DDevice9* self,
    IDirect3DPixelShader9* shader)
{
    g_effectProbePs = shader;
    const unsigned hash = EffectProbeGetPixelShaderHash(shader);
    g_effectProbeCurrentPsHash.store(hash, std::memory_order_relaxed);

    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        EffectProbeWrite(
            "SetPS caller=+0x%06X shader=%p hash=%08X",
            EffectProbeCallerRva(_ReturnAddress()),
            shader,
            hash);
    }

    return g_originalSetPixelShader(self, shader);
}

static HRESULT WINAPI HookProbeSetStreamSource(
    IDirect3DDevice9* self,
    UINT streamNumber,
    IDirect3DVertexBuffer9* streamData,
    UINT offsetInBytes,
    UINT stride)
{
    const bool trailSignature =
        EffectProbeIsDpfixTrailStream(offsetInBytes, stride);

    IDirect3DSurface9* currentRt =
        g_currentRenderTarget0.load(std::memory_order_relaxed);
    IDirect3DSurface9* backBuffer =
        g_backBuffer0.load(std::memory_order_relaxed);
    const bool onBackbuffer =
        currentRt != nullptr && currentRt == backBuffer;

    if (trailSignature && g_effectProbeActive.load(std::memory_order_relaxed))
    {
        float before[4] = {};
        self->GetVertexShaderConstantF(254, before, 1);
        EffectProbeWrite(
            "SetStreamSource TRAIL caller=+0x%06X stream=%u offset=%u stride=%u RT=%p BB=%p onBB=%u c254={%.9g,%.9g,%.9g,%.9g}",
            EffectProbeCallerRva(_ReturnAddress()),
            streamNumber,
            offsetInBytes,
            stride,
            currentRt,
            backBuffer,
            onBackbuffer ? 1u : 0u,
            before[0], before[1], before[2], before[3]);
    }

    // Original DPFix 0.5 behavior: for the enemy shadow-trail stream signature,
    // restore the game's base half-resolution constants before the draw.
    if (trailSignature &&
        !onBackbuffer &&
        g_effectProbeDpfixEnemyShadowTrailFix.load(std::memory_order_relaxed))
    {
        const float trailConstant[4] =
        {
            640.0f, 360.0f, 640.0f, 360.0f
        };

        g_originalSetVertexShaderConstantF(
            self,
            254,
            trailConstant,
            1);

        if (g_effectProbeActive.load(std::memory_order_relaxed))
        {
            EffectProbeWrite(
                "DPFIX_TRAIL_FIX applied stream=%u offset=%u stride=%u c254={640,360,640,360}",
                streamNumber,
                offsetInBytes,
                stride);
        }
    }

    return g_originalSetStreamSource(
        self,
        streamNumber,
        streamData,
        offsetInBytes,
        stride);
}

static HRESULT WINAPI HookProbeDrawPrimitive(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE type,
    UINT startVertex,
    UINT primitiveCount)
{
    EffectProbeCaptureSuspectShaderState(self, primitiveCount);

    if (EffectProbeShouldSkipCurrentPixelShader())
    {
        EffectProbeWrite(
            "SKIP DrawPrimitive PS=%08X prim=%u",
            g_effectProbeCurrentPsHash.load(std::memory_order_relaxed),
            primitiveCount);
        return D3D_OK;
    }

    EffectProbeRecordDraw(
        "DrawPrimitive",
        type,
        primitiveCount,
        _ReturnAddress());

    return g_originalDrawPrimitive(self, type, startVertex, primitiveCount);
}

static HRESULT WINAPI HookProbeDrawIndexedPrimitive(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE type,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount)
{
    EffectProbeCaptureSuspectShaderState(self, primitiveCount);

    if (EffectProbeShouldSkipCurrentPixelShader())
    {
        EffectProbeWrite(
            "SKIP DrawIndexedPrimitive PS=%08X prim=%u",
            g_effectProbeCurrentPsHash.load(std::memory_order_relaxed),
            primitiveCount);
        return D3D_OK;
    }

    EffectProbeRecordDraw(
        "DrawIndexedPrimitive",
        type,
        primitiveCount,
        _ReturnAddress());

    return g_originalDrawIndexedPrimitive(
        self,
        type,
        baseVertexIndex,
        minVertexIndex,
        numVertices,
        startIndex,
        primitiveCount);
}

static HRESULT WINAPI HookProbeDrawPrimitiveUP(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE type,
    UINT primitiveCount,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride)
{
    EffectProbeCaptureSuspectShaderState(self, primitiveCount);

    if (EffectProbeShouldSkipCurrentPixelShader())
    {
        EffectProbeWrite(
            "SKIP DrawPrimitiveUP PS=%08X prim=%u",
            g_effectProbeCurrentPsHash.load(std::memory_order_relaxed),
            primitiveCount);
        return D3D_OK;
    }

    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        const UINT vertexCount =
            EffectProbePrimitiveVertexCount(type, primitiveCount);
        float minX = 0.0f;
        float minY = 0.0f;
        float maxX = 0.0f;
        float maxY = 0.0f;
        const bool haveBounds = EffectProbeScreenSpaceBounds(
            vertexStreamZeroData,
            vertexCount,
            vertexStreamZeroStride,
            &minX,
            &minY,
            &maxX,
            &maxY);

        if (haveBounds)
        {
            EffectProbeWrite(
                "DrawPrimitiveUP caller=+0x%06X type=%u prim=%u verts=%u stride=%u "
                "XYRHW=[%.1f,%.1f]-[%.1f,%.1f] RT=%ux%u VP=%u,%u %ux%u SC=%ld,%ld-%ld,%ld "
                "SC_EN=%lu ALPHA=%lu ZWRITE=%lu FVF=0x%08lX VS=%p PS=%p TEX0=%p %ux%u",
                EffectProbeCallerRva(_ReturnAddress()),
                static_cast<unsigned>(type),
                primitiveCount,
                vertexCount,
                vertexStreamZeroStride,
                minX,
                minY,
                maxX,
                maxY,
                g_effectProbeRt0Width,
                g_effectProbeRt0Height,
                g_effectProbeViewport.X,
                g_effectProbeViewport.Y,
                g_effectProbeViewport.Width,
                g_effectProbeViewport.Height,
                g_effectProbeScissor.left,
                g_effectProbeScissor.top,
                g_effectProbeScissor.right,
                g_effectProbeScissor.bottom,
                g_effectProbeScissorEnabled,
                g_effectProbeAlphaBlendEnabled,
                g_effectProbeZWriteEnabled,
                g_effectProbeFvf,
                g_effectProbeVs,
                g_effectProbePs,
                g_effectProbeTexture0,
                g_effectProbeTexture0Width,
                g_effectProbeTexture0Height);
        }
        else
        {
            EffectProbeRecordDraw(
                "DrawPrimitiveUP",
                type,
                primitiveCount,
                _ReturnAddress());
        }
    }

    return g_originalDrawPrimitiveUP(
        self,
        type,
        primitiveCount,
        vertexStreamZeroData,
        vertexStreamZeroStride);
}

static HRESULT WINAPI HookProbeDrawIndexedPrimitiveUP(
    IDirect3DDevice9* self,
    D3DPRIMITIVETYPE type,
    UINT minVertexIndex,
    UINT numVertices,
    UINT primitiveCount,
    const void* indexData,
    D3DFORMAT indexDataFormat,
    const void* vertexStreamZeroData,
    UINT vertexStreamZeroStride)
{
    EffectProbeCaptureSuspectShaderState(self, primitiveCount);

    if (EffectProbeShouldSkipCurrentPixelShader())
    {
        EffectProbeWrite(
            "SKIP DrawIndexedPrimitiveUP PS=%08X prim=%u",
            g_effectProbeCurrentPsHash.load(std::memory_order_relaxed),
            primitiveCount);
        return D3D_OK;
    }

    if (g_effectProbeActive.load(std::memory_order_relaxed))
    {
        float minX = 0.0f;
        float minY = 0.0f;
        float maxX = 0.0f;
        float maxY = 0.0f;
        const bool haveBounds = EffectProbeScreenSpaceBounds(
            vertexStreamZeroData,
            numVertices,
            vertexStreamZeroStride,
            &minX,
            &minY,
            &maxX,
            &maxY);

        if (haveBounds)
        {
            EffectProbeWrite(
                "DrawIndexedPrimitiveUP caller=+0x%06X type=%u prim=%u verts=%u stride=%u "
                "XYRHW=[%.1f,%.1f]-[%.1f,%.1f] RT=%ux%u VP=%u,%u %ux%u SC=%ld,%ld-%ld,%ld "
                "SC_EN=%lu ALPHA=%lu ZWRITE=%lu FVF=0x%08lX VS=%p PS=%p TEX0=%p %ux%u",
                EffectProbeCallerRva(_ReturnAddress()),
                static_cast<unsigned>(type),
                primitiveCount,
                numVertices,
                vertexStreamZeroStride,
                minX,
                minY,
                maxX,
                maxY,
                g_effectProbeRt0Width,
                g_effectProbeRt0Height,
                g_effectProbeViewport.X,
                g_effectProbeViewport.Y,
                g_effectProbeViewport.Width,
                g_effectProbeViewport.Height,
                g_effectProbeScissor.left,
                g_effectProbeScissor.top,
                g_effectProbeScissor.right,
                g_effectProbeScissor.bottom,
                g_effectProbeScissorEnabled,
                g_effectProbeAlphaBlendEnabled,
                g_effectProbeZWriteEnabled,
                g_effectProbeFvf,
                g_effectProbeVs,
                g_effectProbePs,
                g_effectProbeTexture0,
                g_effectProbeTexture0Width,
                g_effectProbeTexture0Height);
        }
        else
        {
            EffectProbeRecordDraw(
                "DrawIndexedPrimitiveUP",
                type,
                primitiveCount,
                _ReturnAddress());
        }
    }

    return g_originalDrawIndexedPrimitiveUP(
        self,
        type,
        minVertexIndex,
        numVertices,
        primitiveCount,
        indexData,
        indexDataFormat,
        vertexStreamZeroData,
        vertexStreamZeroStride);
}
