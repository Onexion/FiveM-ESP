// src/memory.cpp
#include "memory.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>

#pragma comment(lib, "Psapi.lib")

namespace Memory
{
    static HANDLE      g_hProcess = nullptr;
    static DWORD       g_processId = 0;
    static uintptr_t   g_baseAddress = 0;
    static size_t      g_baseSize = 0;

    static DWORD FindProcessIdByWindowClass(const char* windowClass)
    {
        HWND hwnd = nullptr;

        for (int i = 0; i < 300 && !hwnd; ++i)
        {
            hwnd = FindWindowA(windowClass, nullptr);
            if (!hwnd) Sleep(100);
        }

        if (!hwnd)
        {
            return 0;
        }

        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);

        if (!pid)
        {
            return 0;
        }

        return pid;
    }

    bool Init(const char* processName)
    {
        if (g_hProcess) Shutdown();

        g_processId = FindProcessIdByWindowClass("grcWindow");

        if (!g_processId)
        {
            HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hSnap != INVALID_HANDLE_VALUE)
            {
                PROCESSENTRY32 pe32{};
                pe32.dwSize = sizeof(pe32);
                if (Process32First(hSnap, &pe32))
                {
                    do {
                        WCHAR wName[MAX_PATH];
                        MultiByteToWideChar(CP_ACP, 0, processName, -1, wName, MAX_PATH);
                        if (_wcsicmp(pe32.szExeFile, wName) == 0)
                        {
                            g_processId = pe32.th32ProcessID;
                            break;
                        }
                    } while (Process32Next(hSnap, &pe32));
                }
                CloseHandle(hSnap);
            }
        }

        if (!g_processId)
        {
            return false;
        }

        g_hProcess = OpenProcess(
            PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION |
            PROCESS_QUERY_INFORMATION, FALSE, g_processId);

        if (!g_hProcess)
        {
            return false;
        }

        g_baseAddress = GetModuleBase(processName);
        g_baseSize = GetModuleSize(processName);

        return true;
    }

    void Shutdown()
    {
        if (g_hProcess) { CloseHandle(g_hProcess); g_hProcess = nullptr; }
        g_processId = 0; g_baseAddress = 0; g_baseSize = 0;
    }

    bool      IsAttached() { return g_hProcess != nullptr; }
    HANDLE    GetHandle() { return g_hProcess; }
    DWORD     GetProcessId() { return g_processId; }
    uintptr_t GetBaseAddress() { return g_baseAddress; }

    bool ReadRaw(uintptr_t address, void* buffer, size_t size)
    {
        if (!g_hProcess || !address || !buffer || !size) return false;
        SIZE_T bytesRead = 0;
        BOOL ok = ReadProcessMemory(g_hProcess,
            reinterpret_cast<LPCVOID>(address), buffer, size, &bytesRead);
        return ok && bytesRead == size;
    }

    bool Read(uintptr_t address, void* buffer, size_t size)
    {
        return ReadRaw(address, buffer, size);
    }

    bool WriteRaw(uintptr_t address, const void* buffer, size_t size)
    {
        if (!g_hProcess || !address || !buffer || !size) return false;
        SIZE_T bytesWritten = 0;
        BOOL ok = WriteProcessMemory(g_hProcess,
            reinterpret_cast<LPVOID>(address), buffer, size, &bytesWritten);
        return ok && bytesWritten == size;
    }

    uintptr_t GetModuleBase(const char* moduleName)
    {

        if (!g_processId)
        {
            return 0;
        }

        HANDLE hSnap = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, g_processId);

        if (hSnap == INVALID_HANDLE_VALUE)
        {
            return 0;
        }

        MODULEENTRY32 me32{};
        me32.dwSize = sizeof(me32);

        WCHAR wName[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, moduleName, -1, wName, MAX_PATH);

        uintptr_t base = 0;
        int moduleCount = 0;

        if (Module32First(hSnap, &me32))
        {
            do {
                ++moduleCount;
                if (_wcsicmp(me32.szModule, wName) == 0)
                {
                    base = reinterpret_cast<uintptr_t>(me32.modBaseAddr);
                    break;
                }
            } while (Module32Next(hSnap, &me32));
        }
        else
        {

        }

        CloseHandle(hSnap);

        if (!base)
        {

        }

        return base;
    }

    size_t GetModuleSize(const char* moduleName)
    {
        if (!g_processId) return 0;
        HANDLE hSnap = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, g_processId);
        if (hSnap == INVALID_HANDLE_VALUE) return 0;

        MODULEENTRY32 me32{};
        me32.dwSize = sizeof(me32);
        size_t size = 0;

        WCHAR wName[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, moduleName, -1, wName, MAX_PATH);

        if (Module32First(hSnap, &me32))
        {
            do {
                if (_wcsicmp(me32.szModule, wName) == 0)
                {
                    size = me32.modBaseSize;
                    break;
                }
            } while (Module32Next(hSnap, &me32));
        }
        CloseHandle(hSnap);
        return size;
    }
}