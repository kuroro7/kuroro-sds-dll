#include <Windows.h>
#include "Debug.h"

BOOL WriteMemStringW(DWORD_PTR address, std::wstring value, size_t length)
{
    HANDLE hProcess = GetCurrentProcess();

    if (!WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(address), value.c_str(), length * sizeof(wchar_t), NULL))
        return FALSE;

    return TRUE;
}
