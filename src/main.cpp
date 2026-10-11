#include <Windows.h>

#include "overlay.h"
#include "render.h"
#include "memory.h"
#include "offsets.h"
#include "sdk.h"
#include "esp.h"
#include "menu.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    if (!Overlay::Create(hInstance, L"FiveM External Overlay"))
        return 1;

    HWND hwnd = Overlay::GetHandle();
    Overlay::SetClickThrough(true);

    if (!Render::Init(hwnd))
    {
        Overlay::Destroy();
        return 1;
    }

    Memory::Init("FiveM_b3258_GTAProcess.exe");
    Offsets::Resolve();
    SDK::Init();
    ESP::Init();
    Menu::Init();

    HWND fiveMWindow = FindWindowA("grcWindow", nullptr);

    if (!fiveMWindow)
    {
        for (int i = 0; i < 300 && !fiveMWindow; ++i)
        {
            fiveMWindow = FindWindowA("grcWindow", nullptr);
            if (!fiveMWindow) Sleep(100);
        }
    }

    bool running = true;
    while (running)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) running = false;
        }
        if (!running) break;

        HWND foreground = GetForegroundWindow();
        bool isFiveMFocused = (foreground == fiveMWindow || foreground == hwnd);
        bool isMenuOpen = Menu::IsOpen();

        Render::NewFrame();
        Menu::Draw();

        if (isFiveMFocused || isMenuOpen)
        {
            ESP::Draw();
        }

        Render::EndFrame();

        Sleep(1);
    }

    ESP::Shutdown();
    SDK::Shutdown();
    Memory::Shutdown();
    Render::Shutdown();
    Overlay::Destroy();
    return 0;
}