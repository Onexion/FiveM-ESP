// src/overlay.cpp
#include "overlay.h"
#include "imgui.h"
#include "imgui_impl_win32.h"

#include <iostream>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Overlay
{
    static HWND         g_hwnd = nullptr;
    static HINSTANCE    g_hInstance = nullptr;
    static std::wstring g_className = L"FiveM_External_Overlay_Class";
    static bool         g_clickThrough = true;

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static bool RegisterWindowClass()
    {
        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(wc);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = g_hInstance;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr;
        wc.lpszClassName = g_className.c_str();

        if (!RegisterClassExW(&wc))
            return false;
        return true;
    }

    bool Create(HINSTANCE hInstance, const wchar_t* title)
    {
        if (g_hwnd) return true;
        g_hInstance = hInstance;
        if (!RegisterWindowClass()) return false;

        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);

        DWORD exStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW |
            WS_EX_TRANSPARENT | WS_EX_LAYERED;
        DWORD style = WS_POPUP;

        g_hwnd = CreateWindowExW(exStyle, g_className.c_str(), title, style,
            0, 0, sw, sh, nullptr, nullptr, hInstance, nullptr);

        if (!g_hwnd) return false;

        SetLayeredWindowAttributes(g_hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

        MARGINS margins = { 0, 0, sw, sh };
        DwmExtendFrameIntoClientArea(g_hwnd, &margins);

        BOOL disableTransitions = TRUE;
        DwmSetWindowAttribute(g_hwnd, DWMWA_TRANSITIONS_FORCEDISABLED,
            &disableTransitions, sizeof(disableTransitions));

        ShowWindow(g_hwnd, SW_SHOWDEFAULT);

        SetWindowDisplayAffinity(g_hwnd, WDA_EXCLUDEFROMCAPTURE);

        UpdateWindow(g_hwnd);
        return true;
    }

    void Destroy()
    {
        if (g_hwnd) { DestroyWindow(g_hwnd); g_hwnd = nullptr; }
        if (g_hInstance)
            UnregisterClassW(g_className.c_str(), g_hInstance);
    }

    void SetExcludeFromCapture(bool enable)
    {
        if (!g_hwnd) return;
        SetWindowDisplayAffinity(g_hwnd, enable ? WDA_EXCLUDEFROMCAPTURE : WDA_NONE);
    }

    HWND GetHandle() { return g_hwnd; }
    bool IsValid() { return g_hwnd && IsWindow(g_hwnd); }

    void SetClickThrough(bool enable)
    {
        if (!g_hwnd) return;
        g_clickThrough = enable;

        LONG_PTR ex = GetWindowLongPtrW(g_hwnd, GWL_EXSTYLE);
        if (enable) ex |= WS_EX_TRANSPARENT;
        else        ex &= ~WS_EX_TRANSPARENT;
        SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, ex);

        SetWindowPos(g_hwnd, nullptr, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }

    LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
            break;
        case WM_ERASEBKGND:
            return 1;
        case WM_NCHITTEST:
            if (g_clickThrough) return HTTRANSPARENT;
            return HTCLIENT;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }
}