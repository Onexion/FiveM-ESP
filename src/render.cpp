// src/render.cpp
#include "render.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <iostream>

namespace Render
{
    static ID3D11Device* g_device = nullptr;
    static ID3D11DeviceContext* g_context = nullptr;
    static IDXGISwapChain* g_swapChain = nullptr;
    static ID3D11RenderTargetView* g_rtv = nullptr;
    static bool                     g_init = false;

    ID3D11Device* GetDevice() { return g_device; }
    ID3D11DeviceContext* GetContext() { return g_context; }
    IDXGISwapChain* GetSwapChain() { return g_swapChain; }

    static void CreateRenderTarget()
    {
        ID3D11Texture2D* backBuffer = nullptr;
        if (SUCCEEDED(g_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) && backBuffer)
        {
            g_device->CreateRenderTargetView(backBuffer, nullptr, &g_rtv);
            backBuffer->Release();
        }
    }

    static void CleanupRenderTarget()
    {
        if (g_rtv) { g_rtv->Release(); g_rtv = nullptr; }
    }

    static bool CreateDeviceD3D(HWND hwnd)
    {
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 2;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        D3D_FEATURE_LEVEL level;
        D3D_FEATURE_LEVEL levels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            levels, 2, D3D11_SDK_VERSION, &sd, &g_swapChain,
            &g_device, &level, &g_context);

        if (hr == DXGI_ERROR_UNSUPPORTED)
        {
            hr = D3D11CreateDeviceAndSwapChain(
                nullptr, D3D_DRIVER_TYPE_WARP, nullptr, 0,
                levels, 2, D3D11_SDK_VERSION, &sd, &g_swapChain,
                &g_device, &level, &g_context);
        }

        if (FAILED(hr)) return false;
        CreateRenderTarget();
        return true;
    }

    static void CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (g_swapChain) { g_swapChain->Release(); g_swapChain = nullptr; }
        if (g_context) { g_context->Release();   g_context = nullptr; }
        if (g_device) { g_device->Release();    g_device = nullptr; }
    }

    bool Init(HWND hwnd)
    {
        if (g_init) return true;
        if (!hwnd || !IsWindow(hwnd)) return false;
        if (!CreateDeviceD3D(hwnd)) { CleanupDeviceD3D(); return false; }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.IniFilename = nullptr;
        ImGui::StyleColorsDark();

        if (!ImGui_ImplWin32_Init(hwnd) ||
            !ImGui_ImplDX11_Init(g_device, g_context))
        {
            ImGui_ImplWin32_Shutdown();
            ImGui_ImplDX11_Shutdown();
            ImGui::DestroyContext();
            CleanupDeviceD3D();
            return false;
        }

        g_init = true;
        return true;
    }

    void Shutdown()
    {
        if (!g_init) return;
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        CleanupDeviceD3D();
        g_init = false;
    }

    void NewFrame()
    {
        if (!g_init) return;
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void EndFrame()
    {
        if (!g_init) return;

        ImGui::EndFrame();
        ImGui::Render();

        const float clear[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        g_context->OMSetRenderTargets(1, &g_rtv, nullptr);
        g_context->ClearRenderTargetView(g_rtv, clear);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_swapChain->Present(0, 0);
    }

    void OnResize(UINT w, UINT h)
    {
        if (!g_init || !g_swapChain || w == 0 || h == 0) return;
        CleanupRenderTarget();
        g_swapChain->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0);
        CreateRenderTarget();
    }
}