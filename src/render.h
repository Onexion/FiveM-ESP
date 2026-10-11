// src/render.h
#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

namespace Render
{
    bool Init(HWND hwnd);
    void Shutdown();
    void NewFrame();
    void EndFrame();
    void OnResize(UINT w, UINT h);

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetContext();
    IDXGISwapChain* GetSwapChain();
}