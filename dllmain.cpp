#include "pch.h"
#include <Windows.h>
#include <sstream>
#include <vector>
#include <thread>
#include <locale> 
#include <regex>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <direct.h>

#include "MemoryScanner.h"
#include "ReadMem.h"
#include "WriteMem.h"
#include "HookGetHostMoneyInPack.h"
#include "RegisterLua.h"
#include "Validation.h"

#ifndef GWL_WNDPROC
#define GWL_WNDPROC (-4)
#endif

#define SHIWORD(x)   (*((__int16*)&(x)+1))

WNDPROC oldWndProc = 0;
DWORD_PTR cashGlobalAddr = 0;
HMODULE DLLHModule;
std::vector<HWND> hwnds;
DWORD processId;

LRESULT CALLBACK KuroroWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WNDPROC GetOldWndProc(HWND hWnd)
{
    WNDPROC oldWndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWL_WNDPROC);
    return oldWndProc;
}

void SetOldWndProc(HWND hWnd)
{
    SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)oldWndProc);
}

void UpdateWindowProc(HWND hWnd)
{
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)KuroroWindowProc);
}

struct Params
{
    DWORD processId;
    HWND hwnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    Params* params = (Params*)lParam;
    DWORD processId;
    if (GetWindowThreadProcessId(hwnd, &processId) && processId == params->processId) {
        params->hwnd = hwnd;
        hwnds.push_back(hwnd);
        return TRUE;
    }
    return TRUE;
}

BOOL CALLBACK EnumChildWindowsProc(HWND hwnd, LPARAM lParam) {
    Params* params = (Params*)lParam;
    DWORD processId;
    if (GetWindowThreadProcessId(hwnd, &processId) && processId == params->processId) {
        params->hwnd = hwnd;
        hwnds.push_back(hwnd);
        return TRUE;
    }

    return TRUE;
}

HWND GetMainWindow()
{
    HWND result;
    result = (HWND)0x001868920;
    return result;
}

HWND GetHwndFromProcessId(DWORD processId) {
    Params params = { processId, NULL };
    EnumWindows(EnumWindowsProc, (LPARAM)&params);
    if (!params.hwnd) {
        EnumChildWindows(GetDesktopWindow(), EnumChildWindowsProc, (LPARAM)&params);
    }
    return params.hwnd;
}


LRESULT CALLBACK KuroroWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result;
    __int16 v5;
    RECT Rect;

    if (uMsg != WM_NCHITTEST)
        CallWindowProcA(oldWndProc, hwnd, uMsg, wParam, lParam);
    
    if (ReadMem<BYTE>((LPVOID)0x01869E42))
        return 1;

    GetWindowRect(hwnd, &Rect);
    
    if ((__int16)lParam < Rect.left || (__int16)lParam >= Rect.left + 5)
        v5 = HIWORD(lParam);
    else
    {
        v5 = HIWORD(lParam);
        if (SHIWORD(lParam) >= Rect.top && SHIWORD(lParam) < Rect.top + 5)
            return HTTOPLEFT;
    }

    if ((__int16)lParam < Rect.right && (__int16)lParam >= Rect.right - 5 && v5 >= Rect.top && v5 < Rect.top + 5)
        return HTTOPRIGHT;

    if ((__int16)lParam >= Rect.left
        && (__int16)lParam < Rect.left + 5
        && SHIWORD(lParam) < Rect.bottom
        && SHIWORD(lParam) >= Rect.bottom - 5)
        return HTBOTTOMLEFT;
    
    if ((__int16)lParam < Rect.right
        && (__int16)lParam >= Rect.right - 5
        && SHIWORD(lParam) < Rect.bottom
        && SHIWORD(lParam) >= Rect.bottom - 5)
        return HTBOTTOMRIGHT;

    if ((__int16)lParam >= Rect.left && (__int16)lParam < Rect.left + 3)
        return HTLEFT;

    if ((__int16)lParam < Rect.right && (__int16)lParam >= Rect.right - 3)
        return HTRIGHT;

    if (SHIWORD(lParam) >= Rect.top && SHIWORD(lParam) < Rect.top + 2)
        return HTTOP;

    if (SHIWORD(lParam) + 3 >= Rect.top
        && SHIWORD(lParam) < Rect.top + 25
        && (__int16)lParam < Rect.right - 200
        && (BYTE)1)
        return HTCAPTION;

    if (SHIWORD(lParam) >= Rect.bottom)
        return HTCLIENT;
    
    result = HTCLIENT;
    
    if (SHIWORD(lParam) >= Rect.bottom - 3)
        return HTBOTTOM;

    return result;
}

HWND GetVisibleHWND() {
    for (HWND hwnd : hwnds)
        if (IsWindowVisible(hwnd))
            return hwnd;

    return 0;
}

std::wstring GetClassNameFromHwnd(HWND hwnd) {
    wchar_t className[128];
    GetClassName(hwnd, className, sizeof(className));
    std::wstring lpClassName(className);
    return lpClassName;
}

void GetAllWindows() {
    hwnds.clear();

    GetHwndFromProcessId(processId);
}

std::wstring CGetWindowText(HWND hwnd) {
    wchar_t windowTitle[256];

    int length = GetWindowTextW(hwnd, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));

    if (length > 0) {
        std::wstring title(windowTitle);
        return title;
    }
    
    return L"";
}

bool ContainsChannelNumber(const std::wstring& str) {
    std::wregex pattern(LR"(\(\d+\))");
    return std::regex_search(str, pattern);
}

typedef unsigned long long QWORD;

void RemoveScreenBorder() {
    DWORD flOldProtect;

    VirtualProtect((LPVOID)0x004023F5, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x004023F5 = 0;
    VirtualProtect((LPVOID)0x004023F5, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x0044E181, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0044E181 = 0;
    VirtualProtect((LPVOID)0x0044E181, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x00753F70, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x00753F70 = 7;
    VirtualProtect((LPVOID)0x00753F70, 1u, flOldProtect, &flOldProtect);


    VirtualProtect((LPVOID)0x0044FAD0, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0044FAD0 = 1;
    VirtualProtect((LPVOID)0x0044FAD0, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x0044FAEC, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0044FAEC = 1;
    VirtualProtect((LPVOID)0x0044FAEC, 1u, flOldProtect, &flOldProtect);


    VirtualProtect((LPVOID)0x0066FB0A, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0066FB0A = 6;
    VirtualProtect((LPVOID)0x0066FB0A, 1u, flOldProtect, &flOldProtect);
    

    VirtualProtect((LPVOID)0x0066FB0C, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0066FB0C = 1;
    VirtualProtect((LPVOID)0x0066FB0C, 1u, flOldProtect, &flOldProtect);


    // NOT SURE IF THIS IS RIGHT. GOT FROM ANOTHER DLL. ================================
    LPVOID v8;
    DWORD flNewProtect = -1869610902;
    __int16 v23 = -0x6F70;
    char v25 = -112;
    char v9 = v25;
    WORD* v8_ptr;

    v8_ptr = (WORD*)VirtualAlloc(0, 7u, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

    *(DWORD*)v8_ptr = flNewProtect;
    *(v8_ptr + 2) = v23;
    *((char*)v8_ptr + 6) = v9;

    VirtualProtect((LPVOID)0x0066FB11, 7u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(DWORD*)0x0066FB11 = *(DWORD*)v8_ptr;
    *(__int16*)(0x0066FB11 + 4) = *(__int16*)((char*)v8_ptr + 4);
    *(char*)(0x0066FB17) = *((char*)v8_ptr + 6);
    VirtualProtect((LPVOID)0x0066FB11, 7u, flOldProtect, &flOldProtect);
    // ================================================================================

    VirtualProtect((LPVOID)0x0084071E, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0084071E = 1;
    VirtualProtect((LPVOID)0x0084071E, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x00840752, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x00840752 = 1;
    VirtualProtect((LPVOID)0x00840752, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x0084074E, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0084074E = 1;
    VirtualProtect((LPVOID)0x0084074E, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x0084074A, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0084074A = -51;
    VirtualProtect((LPVOID)0x0084074A, 1u, flOldProtect, &flOldProtect);

    VirtualProtect((LPVOID)0x0084074C, 1u, PAGE_EXECUTE_READWRITE, &flOldProtect);
    *(BYTE*)0x0084074A = 85;
    VirtualProtect((LPVOID)0x0084074C, 1u, flOldProtect, &flOldProtect);
}

bool IsWindowFullscreen(HWND hwnd) {
    WINDOWPLACEMENT placement;
    placement.length = sizeof(WINDOWPLACEMENT);
    if (GetWindowPlacement(hwnd, &placement))
        return (placement.showCmd == SW_MAXIMIZE || placement.showCmd == SW_SHOWMAXIMIZED);
    return false;
}

struct Resolution {
    int w;
    int h;
};

/*
    Fix a weird problem that makes the window lost it's original size when reopening.
*/
Resolution ReadSystemSettings() {
    Resolution resolution;

    char buffer[FILENAME_MAX];
    _getcwd(buffer, FILENAME_MAX);
    std::string currentDir(buffer);

    std::ifstream inputFile(".\\rolesettings\\SystemSettings.ini");
    std::string line;
    std::regex renderWidRegex(R"(RenderWid\s*=\s*(\d+))");
    std::regex renderHeiRegex(R"(RenderHei\s*=\s*(\d+))");
    std::smatch match;
    
    resolution.w = 1024;
    resolution.h = 768;

    while (std::getline(inputFile, line)) {
        if (std::regex_search(line, match, renderWidRegex))
            resolution.w = std::stoi(match[1].str());
        
        if (std::regex_search(line, match, renderHeiRegex))
            resolution.h = std::stoi(match[1].str());
    }

    return resolution;
}


void Monitor() {
    CreateHookGetHostMoneyInPack();
    RemoveScreenBorder();

    HWND hwnd = 0;

    while (TRUE) {
        GetAllWindows();
        hwnd = GetVisibleHWND();
        
        if (hwnd != 0)
            break;
    }

    oldWndProc = GetOldWndProc(hwnd);
    UpdateWindowProc(hwnd);

    if (!IsWindowFullscreen(hwnd)) {
        RECT rect;
        GetWindowRect(hwnd, &rect);
        Resolution resolution = ReadSystemSettings();
        SetWindowPos(hwnd, NULL, rect.left, rect.top, resolution.w + 1, resolution.h + 1, SWP_NOZORDER);
        Sleep(1000);
        SetWindowPos(hwnd, NULL, rect.left, rect.top, resolution.w, resolution.h, SWP_NOZORDER);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    DLLHModule = hModule;
    std::stringstream ss;
    HWND hwnd;
    HMODULE tempModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!ValidateName(hModule))
            return FALSE;

        processId = GetCurrentProcessId();

        new std::thread(Monitor);

        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) void DummyFunction()
{
    // entry point
}
