#include "esp.h"
#include "sdk.h"
#include "mathutils.h"
#include "menu.h"
#include "imgui.h"

#include <vector>
#include <cmath>
#include <cstdio>

namespace ESP
{
    static ImDrawList* g_DrawList = nullptr;

    static const ImU32 COLOR_BOX = IM_COL32(255, 255, 255, 255);
    static const ImU32 COLOR_HEALTH = IM_COL32(0, 255, 0, 255);
    static const ImU32 COLOR_DISTANCE = IM_COL32(200, 200, 200, 255);

    static bool WorldToScreen(const SDK::Vec3& world, ImVec2& screen, const SDK::ViewMatrix& vm)
    {
        ImGuiIO& io = ImGui::GetIO();
        Math::Vec2 out;
        if (!Math::WorldToScreen(world, out, vm.m, (int)io.DisplaySize.x, (int)io.DisplaySize.y))
            return false;
        screen.x = out.x;
        screen.y = out.y;
        return true;
    }

    void Init() {}
    void Shutdown() {}

    void Draw()
    {
        if (!SDK::IsReady()) return;

        g_DrawList = ImGui::GetBackgroundDrawList();
        if (!g_DrawList) return;

        SDK::ViewMatrix vm;
        if (!SDK::GetViewMatrix(vm)) return;

        std::vector<SDK::Player> players;
        if (!SDK::GetAllPlayers(players)) return;

        float maxDist = Menu::GetMaxDistance();
        float boxThick = Menu::GetBoxThickness();

        for (auto& p : players)
        {
            if (p.distance > maxDist) continue;

            SDK::Vec3 headPos = p.position; headPos.z += 0.8f;
            SDK::Vec3 footPos = p.position; footPos.z -= 0.9f;

            ImVec2 head, foot;
            if (!WorldToScreen(headPos, head, vm)) continue;
            if (!WorldToScreen(footPos, foot, vm)) continue;

            float height = foot.y - head.y;
            float width = height * 0.4f;

            ImVec2 tl(head.x - width / 2.0f, head.y);
            ImVec2 br(head.x + width / 2.0f, foot.y);

            if (Menu::GetBoxes())
            {
                g_DrawList->AddRect(tl, br, COLOR_BOX, 0.0f, 0, boxThick);
            }

            if (Menu::GetHealth())
            {
                float barWidth = 3.0f;
                float barX = tl.x - 6.0f;

                g_DrawList->AddRectFilled(
                    ImVec2(barX, tl.y),
                    ImVec2(barX + barWidth, br.y),
                    IM_COL32(0, 0, 0, 150));

                float healthPercent = Math::Clamp(p.health / 200.0f, 0.0f, 1.0f);
                float healthHeight = height * healthPercent;

                g_DrawList->AddRectFilled(
                    ImVec2(barX, br.y - healthHeight),
                    ImVec2(barX + barWidth, br.y),
                    COLOR_HEALTH);
            }

            if (Menu::GetNames())
            {
                char name[64];
                sprintf_s(name, "Player [%d]", p.team);
                ImVec2 textSize = ImGui::CalcTextSize(name);
                g_DrawList->AddText(
                    ImVec2(tl.x + width / 2.0f - textSize.x / 2.0f, tl.y - 16.0f),
                    IM_COL32(255, 255, 255, 255), name);
            }

            if (Menu::GetDistance())
            {
                char dist[32];
                sprintf_s(dist, "[%.0fm]", p.distance);
                ImVec2 textSize = ImGui::CalcTextSize(dist);
                g_DrawList->AddText(
                    ImVec2(foot.x - textSize.x / 2.0f, foot.y + 4.0f),
                    COLOR_DISTANCE, dist);
            }
        }
    }
}