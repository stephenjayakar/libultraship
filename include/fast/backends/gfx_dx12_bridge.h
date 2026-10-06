#pragma once

#ifdef _WIN32

#include <Windows.h>
#include <dxgi1_2.h>

// =============================================================================
// DX12 Bridge Interface
// =============================================================================
// This header declares functions that allow the DXGI backend (gfx_dxgi.cpp) to
// cooperate with an external DX12 renderer (e.g. RTX path tracing).
//
// The KEY INSIGHT: A DXGI swap chain can be created with either a D3D11 device
// or a D3D12 command queue - both implement IUnknown.  When the bridge is
// active, gfx_dxgi.cpp passes the DX12 command queue to CreateSwapChainForHwnd
// instead of the D3D11 device, making the swap chain DX12-owned.
//
// The bridge uses a function-pointer indirection pattern:
// - Default implementations are in gfx_dx12_bridge.cpp (libultraship)
// - The RTX layer registers override functions via GfxDX12Bridge_Register*()
//   during initialization
// - gfx_dxgi.cpp calls the API functions which dispatch to overrides if set
// =============================================================================

#ifdef __cplusplus
extern "C" {
#endif

// ===================== Core API (called by gfx_dxgi.cpp) =====================

// Returns true if DX12 should own the swap chain and handle rendering.
bool GfxDX12Bridge_IsActive();

// Returns the DX12 command queue as IUnknown* for swap chain creation.
// gfx_dxgi.cpp passes this to CreateSwapChainForHwnd instead of the DX11 device.
IUnknown* GfxDX12Bridge_GetCommandQueue();

// Called by gfx_dxgi.cpp after creating the swap chain so DX12 code can grab it.
void GfxDX12Bridge_SetSwapChain(IDXGISwapChain1* swapChain);

// Called by gfx_dxgi.cpp instead of normal Present when bridge is active.
void GfxDX12Bridge_Present(IDXGISwapChain1* swapChain);

// Called by gfx_dxgi.cpp to provide the HWND to the DX12 renderer.
void GfxDX12Bridge_SetHWND(HWND hwnd);

// =============== Registration API (called by RTX layer) ==================

// The RTX layer calls these to register its override implementations.
void GfxDX12Bridge_RegisterIsActive(bool (*fn)());
void GfxDX12Bridge_RegisterGetCommandQueue(IUnknown* (*fn)());
void GfxDX12Bridge_RegisterSetSwapChain(void (*fn)(IDXGISwapChain1*));
void GfxDX12Bridge_RegisterPresent(void (*fn)(IDXGISwapChain1*));
void GfxDX12Bridge_RegisterSetHWND(void (*fn)(HWND));

// ================ Simple State Setters (convenience) =====================

// Directly set bridge state (alternative to registering overrides)
void GfxDX12Bridge_SetActive(bool active);
void GfxDX12Bridge_SetCommandQueue(IUnknown* queue);

// Retrieve stored state
HWND GfxDX12Bridge_GetHWND();
IDXGISwapChain1* GfxDX12Bridge_GetStoredSwapChain();

#ifdef __cplusplus
}
#endif

#endif // _WIN32
