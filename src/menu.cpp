#include "menu.h"
#include "overlay.h"
#include "imgui.h"

#include <Windows.h>

namespace Menu
{
    static bool g_open = false;
    static bool g_boxes = true;
    static bool g_health = true;
    static bool g_names = false;
    static bool g_distance = true;
    static float g_maxDistance = 300.0f;
    static float g_boxThickness = 1.5f;
    static int  g_unloadKey = VK_END;
    static bool g_waitingForKey = false;
    static bool g_excludeFromCapture = true;

    static bool KeyPressedOnce(int vkKey)
    {
        static bool keyStates[256] = { false };
        bool isDown = (GetAsyncKeyState(vkKey) & 0x8000) != 0;
        bool wasDown = keyStates[vkKey];
        keyStates[vkKey] = isDown;
        return isDown && !wasDown;
    }

    void Init()
    {
        g_open = false;
        g_unloadKey = VK_END;
        g_waitingForKey = false;
        Overlay::SetClickThrough(true);
    }

    void Toggle()
    {
        g_open = !g_open;
        Overlay::SetClickThrough(!g_open);
    }

    bool IsOpen() { return g_open; }

    bool GetBoxes() { return g_boxes; }
    bool GetHealth() { return g_health; }
    bool GetNames() { return g_names; }
    bool GetDistance() { return g_distance; }
    float GetMaxDistance() { return g_maxDistance; }
    float GetBoxThickness() { return g_boxThickness; }

    static const char* GetKeyName(int vkKey)
    {
        switch (vkKey)
        {
        case VK_END: return "END";
        case VK_DELETE: return "DELETE";
        case VK_HOME: return "HOME";
        case VK_INSERT: return "INSERT";
        case VK_PRIOR: return "PAGE UP";
        case VK_NEXT: return "PAGE DOWN";
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        case VK_NUMPAD0: return "NUM 0";
        case VK_NUMPAD1: return "NUM 1";
        case VK_NUMPAD2: return "NUM 2";
        case VK_NUMPAD3: return "NUM 3";
        case VK_MULTIPLY: return "NUM *";
        case VK_ADD: return "NUM +";
        case VK_SUBTRACT: return "NUM -";
        case VK_DIVIDE: return "NUM /";
        case VK_DECIMAL: return "NUM .";
        case VK_OEM_3: return "TILDE";
        case VK_TAB: return "TAB";
        case VK_CAPITAL: return "CAPS";
        case VK_SPACE: return "SPACE";
        case VK_BACK: return "BACKSPACE";
        case VK_RETURN: return "ENTER";
        case VK_UP: return "UP";
        case VK_DOWN: return "DOWN";
        case VK_LEFT: return "LEFT";
        case VK_RIGHT: return "RIGHT";
        default: return "UNKNOWN";
        }
    }

    static void WaitForKeyPress()
    {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            g_waitingForKey = false;
            return;
        }

        for (int i = 1; i < 255; ++i)
        {
            if (i == VK_LBUTTON || i == VK_RBUTTON || i == VK_MBUTTON) continue;
            if (i == VK_SHIFT || i == VK_CONTROL || i == VK_MENU) continue;
            if (i == VK_ESCAPE) continue;
            if (i == VK_LWIN || i == VK_RWIN) continue;

            if (GetAsyncKeyState(i) & 0x8000)
            {
                g_unloadKey = i;
                g_waitingForKey = false;
                return;
            }
        }
    }

    void Draw()
    {
        static HWND s_fiveMWindow = nullptr;
        if (!s_fiveMWindow)
            s_fiveMWindow = FindWindowA("grcWindow", nullptr);

        HWND overlayHwnd = Overlay::GetHandle();
        HWND foreground = GetForegroundWindow();
        bool inGame = (foreground == s_fiveMWindow || foreground == overlayHwnd);

        if (!g_waitingForKey && inGame && KeyPressedOnce(VK_F10))
            Toggle();

        if (!g_waitingForKey && !g_open && inGame && KeyPressedOnce(g_unloadKey))
        {
            PostQuitMessage(0);
            return;
        }

        if (!g_open)
            return;

        ImGui::SetNextWindowSize(ImVec2(500, 450), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

        ImGui::Begin("FiveM ESP - Menu", &g_open, ImGuiWindowFlags_NoCollapse);

        if (ImGui::BeginTabBar("##tabs"))
        {
            if (ImGui::BeginTabItem("ESP"))
            {
                ImGui::Spacing();
                ImGui::Text("ESP Features");
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Checkbox("Boxes", &g_boxes);
                ImGui::Checkbox("Health Bar", &g_health);
                ImGui::Checkbox("Names", &g_names);
                ImGui::Checkbox("Distance", &g_distance);

                if (ImGui::Checkbox("Exclude from Capture", &g_excludeFromCapture))
                {
                    Overlay::SetExcludeFromCapture(g_excludeFromCapture);
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::SliderFloat("Max Distance", &g_maxDistance, 10.0f, 1000.0f, "%.0f m");
                ImGui::SliderFloat("Box Thickness", &g_boxThickness, 0.5f, 5.0f, "%.1f");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Settings"))
            {
                ImGui::Spacing();
                ImGui::Text("Settings");
                ImGui::Separator();
                ImGui::Spacing();

                if (g_waitingForKey)
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Press any key...");
                    WaitForKeyPress();
                    if (ImGui::Button("Cancel", ImVec2(120, 30)))
                        g_waitingForKey = false;
                }
                else
                {
                    ImGui::Text("Unload Hotkey:");
                    ImGui::SameLine();

                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.4f, 0.7f, 1.0f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
                    ImGui::Button(GetKeyName(g_unloadKey), ImVec2(150, 25));
                    ImGui::PopStyleColor(2);

                    ImGui::SameLine();

                    if (ImGui::Button("Change", ImVec2(100, 25)))
                        g_waitingForKey = true;
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::Text("Menu Toggle: F10");
                ImGui::Text("Unload Hotkey: %s", GetKeyName(g_unloadKey));
                ImGui::Text("FiveM must be in Borderless Windowed Mode");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));

                if (ImGui::Button("UNLOAD", ImVec2(150, 50)))
                {
                    PostQuitMessage(0);
                }

                ImGui::PopStyleColor(3);

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        if (!g_open)
            Overlay::SetClickThrough(true);
    }
}