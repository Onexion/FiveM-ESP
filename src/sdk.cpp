// src/sdk.cpp
#include "sdk.h"
#include "memory.h"
#include "offsets.h"
#include "mathutils.h"

#include <cmath>
#include <iostream>

namespace SDK
{
    static bool g_ready = false;

    bool Init()
    {
        if (!Memory::IsAttached()) return false;
        if (!Offsets::World || !Offsets::ReplayInterface) return false;
        g_ready = true;
        return true;
    }

    void Shutdown() { g_ready = false; }
    bool IsReady() { return g_ready && Offsets::World && Offsets::ReplayInterface; }

    bool GetViewMatrix(ViewMatrix& out)
    {
        if (!IsReady() || !Offsets::ViewPort) return false;
        if (!Memory::ReadRaw(Offsets::ViewPort + Offsets::ViewMatrixOffset,
            out.m, sizeof(out.m)))
            return false;

        float temp;
        for (int i = 0; i < 4; ++i)
            for (int j = i + 1; j < 4; ++j)
            {
                temp = out.m[i * 4 + j];
                out.m[i * 4 + j] = out.m[j * 4 + i];
                out.m[j * 4 + i] = temp;
            }

        return true;
    }

    bool ReadPlayer(uintptr_t address, Player& out)
    {
        if (!address) return false;
        out = Player{};
        out.address = address;

        Memory::ReadRaw(address + Offsets::Health, &out.health, sizeof(float));
        Memory::ReadRaw(address + Offsets::Armor, &out.armor, sizeof(float));

        Memory::ReadRaw(address + Offsets::PosX, &out.position.x, sizeof(float));
        Memory::ReadRaw(address + Offsets::PosY, &out.position.y, sizeof(float));
        Memory::ReadRaw(address + Offsets::PosZ, &out.position.z, sizeof(float));

        if (out.health <= 0.0f || out.health > 200.0f) return false;
        if (out.position.x == 0.0f && out.position.y == 0.0f && out.position.z == 0.0f)
            return false;
        if (std::isnan(out.position.x) || std::isnan(out.position.y)
            || std::isnan(out.position.z)) return false;

        return true;
    }

    bool GetLocalPlayer(Player& out)
    {
        if (!IsReady()) return false;
        uintptr_t ptr = Memory::Read<uintptr_t>(Offsets::World + Offsets::LocalPlayer);
        if (!ptr) return false;
        return ReadPlayer(ptr, out);
    }

    bool GetAllPlayers(std::vector<Player>& out)
    {
        out.clear();
        if (!IsReady()) return false;

        Player local;
        bool hasLocal = GetLocalPlayer(local);

        uintptr_t pedInterface = Memory::Read<uintptr_t>(
            Offsets::ReplayInterface + Offsets::PlayerList);

        std::cout << "[SDK] ReplayInterface=0x" << std::hex << Offsets::ReplayInterface
            << " PlayerList=0x" << Offsets::PlayerList
            << " pedInterface=0x" << pedInterface << std::dec << std::endl;

        if (!pedInterface) return false;

        uintptr_t pedList = Memory::Read<uintptr_t>(
            pedInterface + Offsets::PedListOffset);

        std::cout << "[SDK] PedListOffset=0x" << std::hex << Offsets::PedListOffset
            << " pedList=0x" << pedList << std::dec << std::endl;

        if (!pedList) return false;

        int maxPed = Memory::Read<int>(pedInterface + Offsets::MaxPedOffset);
        std::cout << "[SDK] maxPed=" << maxPed << std::endl;

        if (maxPed <= 0 || maxPed > 300) maxPed = 256;

        for (int i = 0; i < maxPed; ++i)
        {
            uintptr_t ptr = Memory::Read<uintptr_t>(pedList + (i * Offsets::PedStep));
            if (!ptr) continue;
            if (hasLocal && ptr == local.address) continue;

            Player p;
            if (!ReadPlayer(ptr, p)) continue;

            if (hasLocal) p.distance = local.position.Distance(p.position);
            out.push_back(p);
        }

        std::cout << "[SDK] GetAllPlayers: " << out.size() << " Spieler" << std::endl;
        return !out.empty();
    }
}