#pragma once

#include <atomic>
#include <d3d9.h>

// Renderer-scope identity captured from Deadly Premonition's own D3D9 path.
// MinHook detours target shared D3D9 implementations, so every hooked entry
// point must fail open for unrelated D3D9 objects created by overlays,
// capture tools, wrappers, or other injectors in the same process.
inline std::atomic<IDirect3D9*> g_gameDirect3D9{ nullptr };
inline std::atomic<IDirect3DDevice9*> g_gameD3D9Device{ nullptr };

inline void SetGameDirect3D9(IDirect3D9* d3d)
{
    g_gameDirect3D9.store(d3d, std::memory_order_release);
}

inline bool IsGameDirect3D9(IDirect3D9* d3d)
{
    return d3d != nullptr &&
           d3d == g_gameDirect3D9.load(std::memory_order_acquire);
}

inline void SetGameD3D9Device(IDirect3DDevice9* device)
{
    g_gameD3D9Device.store(device, std::memory_order_release);
}

inline bool IsGameD3D9Device(IDirect3DDevice9* device)
{
    return device != nullptr &&
           device == g_gameD3D9Device.load(std::memory_order_acquire);
}
