#if defined(ENABLE_DX11) || defined(ENABLE_DX12)

#include "fast/backends/gfx_dx12_bridge.h"

#include <cstdio>

// =============================================================================
// DX12 Bridge Default Implementations
// =============================================================================
// These provide the bridge infrastructure.  The RTX integration layer (in soh/)
// registers override functions via GfxDX12Bridge_Register*() during init.
// When overrides are registered, they take precedence over the default behavior.
// =============================================================================

// Internal state managed by the bridge
static bool s_bridgeActive = false;
static IUnknown* s_commandQueue = nullptr;
static IDXGISwapChain1* s_swapChain = nullptr;
static HWND s_hwnd = nullptr;

// Function pointers that the RTX layer can set
static bool (*s_isActiveOverride)() = nullptr;
static IUnknown* (*s_getCommandQueueOverride)() = nullptr;
static void (*s_setSwapChainOverride)(IDXGISwapChain1*) = nullptr;
static void (*s_presentOverride)(IDXGISwapChain1*) = nullptr;
static void (*s_setHWNDOverride)(HWND) = nullptr;

extern "C" {

// ==================== Registration functions ====================

void GfxDX12Bridge_RegisterIsActive(bool (*fn)()) {
    s_isActiveOverride = fn;
    printf("[DX12-Bridge] IsActive override registered: %p\n", (void*)(uintptr_t)fn);
}

void GfxDX12Bridge_RegisterGetCommandQueue(IUnknown* (*fn)()) {
    s_getCommandQueueOverride = fn;
    printf("[DX12-Bridge] GetCommandQueue override registered: %p\n", (void*)(uintptr_t)fn);
}

void GfxDX12Bridge_RegisterSetSwapChain(void (*fn)(IDXGISwapChain1*)) {
    s_setSwapChainOverride = fn;
    printf("[DX12-Bridge] SetSwapChain override registered: %p\n", (void*)(uintptr_t)fn);
}

void GfxDX12Bridge_RegisterPresent(void (*fn)(IDXGISwapChain1*)) {
    s_presentOverride = fn;
    printf("[DX12-Bridge] Present override registered: %p\n", (void*)(uintptr_t)fn);
}

void GfxDX12Bridge_RegisterSetHWND(void (*fn)(HWND)) {
    s_setHWNDOverride = fn;
    printf("[DX12-Bridge] SetHWND override registered: %p\n", (void*)(uintptr_t)fn);
}

// ==================== Core API implementations ====================

bool GfxDX12Bridge_IsActive() {
    if (s_isActiveOverride) {
        return s_isActiveOverride();
    }
    return s_bridgeActive;
}

IUnknown* GfxDX12Bridge_GetCommandQueue() {
    if (s_getCommandQueueOverride) {
        return s_getCommandQueueOverride();
    }
    return s_commandQueue;
}

void GfxDX12Bridge_SetSwapChain(IDXGISwapChain1* swapChain) {
    s_swapChain = swapChain;
    printf("[DX12-Bridge] SetSwapChain: %p\n", (void*)swapChain);
    if (s_setSwapChainOverride) {
        s_setSwapChainOverride(swapChain);
    }
}

void GfxDX12Bridge_Present(IDXGISwapChain1* swapChain) {
    if (s_presentOverride) {
        s_presentOverride(swapChain);
    }
    // Default: no-op (no DX12 renderer to present)
}

void GfxDX12Bridge_SetHWND(HWND hwnd) {
    s_hwnd = hwnd;
    printf("[DX12-Bridge] SetHWND: %p\n", (void*)hwnd);
    if (s_setHWNDOverride) {
        s_setHWNDOverride(hwnd);
    }
}

// ==================== Simple state setters ====================

void GfxDX12Bridge_SetActive(bool active) {
    s_bridgeActive = active;
    printf("[DX12-Bridge] SetActive: %s\n", active ? "true" : "false");
}

void GfxDX12Bridge_SetCommandQueue(IUnknown* queue) {
    s_commandQueue = queue;
    printf("[DX12-Bridge] SetCommandQueue: %p\n", (void*)queue);
}

HWND GfxDX12Bridge_GetHWND() {
    return s_hwnd;
}

IDXGISwapChain1* GfxDX12Bridge_GetStoredSwapChain() {
    return s_swapChain;
}

} // extern "C"

#endif // ENABLE_DX11 || ENABLE_DX12
