#pragma once

#include <Windows.h>
#include <d3d9.h>

// Research-only renderer diagnostics for the day/night-building investigation.
// No state is persisted to ZachFix.ini and all controls reset on process exit.
void RegisterRenderTraceTextureSource(IDirect3DBaseTexture9* texture, UINT hash);

// True only while the research capture code is inside D3DX surface export.
// Device resource hooks use this to avoid treating D3DX scratch resources as
// game render targets and applying ZachFix resolution virtualization to them.
bool IsRenderTraceInternalCaptureCall();

// Called once at the end of each completed DP frame, immediately before Present.
// Polls F6/F7/F8/F9 controls and advances the indexed-draw isolation state.
void AdvanceRenderMaterialTraceFrame(IDirect3DDevice9* device);

// Multi-stage one-shot final-composite capture. The game-state callback must run
// before ZachFix changes final-composite bindings, the bound-state callback runs
// immediately before the physical draw, and the after callback runs afterwards.
void NotifyRenderTraceFinalCompositeGameState(IDirect3DDevice9* device);
void NotifyRenderTraceFinalCompositeBoundState(IDirect3DDevice9* device);
void NotifyRenderTraceFinalCompositeAfter(IDirect3DDevice9* device);

// Returns true when a main-scene DrawIndexedPrimitive should be submitted.
// When draw isolation is disabled this is a very cheap counter-only path.
bool ShouldSubmitRenderMaterialIndexedDraw(
    IDirect3DDevice9* device,
    bool mainSceneGeometry,
    D3DPRIMITIVETYPE primitiveType,
    INT baseVertexIndex,
    UINT minVertexIndex,
    UINT numVertices,
    UINT startIndex,
    UINT primitiveCount);


void ResetRenderMaterialTraceForDeviceReset();
