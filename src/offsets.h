// src/offsets.h
#pragma once
#include <cstdint>

namespace Offsets
{
    extern uintptr_t World;
    extern uintptr_t ReplayInterface;
    extern uintptr_t ViewPort;
    extern uintptr_t CamGameplayDirector;
    extern uintptr_t BlipList;

    extern uintptr_t LocalPlayer;
    extern uintptr_t PlayerList;
    extern uintptr_t PedListOffset;
    extern uintptr_t MaxPedOffset;
    extern uintptr_t PedStep;

    extern uintptr_t Health;
    extern uintptr_t MaxHealth;
    extern uintptr_t Armor;
    extern uintptr_t Team;
    extern uintptr_t PosX;
    extern uintptr_t PosY;
    extern uintptr_t PosZ;
    extern uintptr_t IsVisible;
    extern uintptr_t PlayerInfo;
    extern uintptr_t PlayerId;
    extern uintptr_t EntityType;
    extern uintptr_t FragInst;

    extern uintptr_t ViewMatrixOffset;

    void Resolve();
}