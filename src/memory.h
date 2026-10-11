// src/memory.h
#pragma once

#include <Windows.h>
#include <cstdint>
#include <string>

namespace Memory
{
    bool ReadRaw(uintptr_t address, void* buffer, size_t size);
    bool Read(uintptr_t address, void* buffer, size_t size);
    bool WriteRaw(uintptr_t address, const void* buffer, size_t size);

    template<typename T>
    T Read(uintptr_t address)
    {
        T buffer{};
        ReadRaw(address, &buffer, sizeof(T));
        return buffer;
    }

    template<typename T>
    bool Write(uintptr_t address, const T& value)
    {
        return WriteRaw(address, &value, sizeof(T));
    }

    bool      Init(const char* processName);
    void      Shutdown();
    bool      IsAttached();
    HANDLE    GetHandle();
    DWORD     GetProcessId();
    uintptr_t GetBaseAddress();

    uintptr_t GetModuleBase(const char* moduleName);
    size_t    GetModuleSize(const char* moduleName);
}