// src/overlay.h
#pragma once
#include <Windows.h>

namespace Overlay
{
    bool Create(HINSTANCE hInstance, const wchar_t* title);
    void Destroy();
    HWND GetHandle();
    bool IsValid();
    void SetClickThrough(bool enable);
    void SetExcludeFromCapture(bool enable);
}