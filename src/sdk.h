// src/sdk.h
#pragma once

#include <cstdint>
#include <vector>
#include "mathutils.h"

namespace SDK
{
    using Vec3 = Math::Vec3;

    struct ViewMatrix
    {
        float m[16];

        bool WorldToScreen(const Vec3& world, Math::Vec2& screen,
            int screenW, int screenH) const
        {
            return Math::WorldToScreen(world, screen, m, screenW, screenH);
        }
    };

    struct Player
    {
        uintptr_t address = 0;

        float health = 0.0f;
        float armor = 0.0f;
        int   team = 0;
        bool  visible = false;

        Vec3  position;
        float distance = 0.0f;
    };

    bool Init();
    void Shutdown();
    bool IsReady();

    bool GetViewMatrix(ViewMatrix& out);
    bool GetLocalPlayer(Player& out);
    bool GetAllPlayers(std::vector<Player>& out);
    bool ReadPlayer(uintptr_t address, Player& out);
}