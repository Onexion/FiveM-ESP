// src/offsets.cpp
#include "offsets.h"
#include "memory.h"
#include <iostream>

namespace Offsets
{
	//Build 3258
    uintptr_t World = 0;
    uintptr_t ReplayInterface = 0;
    uintptr_t ViewPort = 0;
    uintptr_t CamGameplayDirector = 0;
    uintptr_t BlipList = 0;

    uintptr_t LocalPlayer = 0x8;
    uintptr_t PlayerList = 0x18;
    uintptr_t PedListOffset = 0x100;
    uintptr_t MaxPedOffset = 0x108;
    uintptr_t PedStep = 0x10;

    uintptr_t Health = 0x280;
    uintptr_t MaxHealth = 0x284;
    uintptr_t Armor = 0x150C;
    uintptr_t Team = 0;
    uintptr_t PosX = 0x90;
    uintptr_t PosY = 0x94;
    uintptr_t PosZ = 0x98;
    uintptr_t IsVisible = 0x145C;
    uintptr_t PlayerInfo = 0x10A8;
    uintptr_t PlayerId = 0xE8;
    uintptr_t EntityType = 0x1098;
    uintptr_t FragInst = 0x1430;

    uintptr_t ViewMatrixOffset = 0x24C;

    void Resolve()
    {
        if (!Memory::IsAttached())
        {
            return;
        }

        uintptr_t base = Memory::GetBaseAddress();
        if (!base)
        {
            return;
        }

        World = Memory::Read<uintptr_t>(base + 0x25B14B0);
        ReplayInterface = Memory::Read<uintptr_t>(base + 0x1FBD4F0);
        ViewPort = Memory::Read<uintptr_t>(base + 0x201DBA0);
        CamGameplayDirector = Memory::Read<uintptr_t>(base + 0x201ED50);
        BlipList = Memory::Read<uintptr_t>(base + 0x2023400);
    }
}