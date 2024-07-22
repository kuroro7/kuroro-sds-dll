#ifndef DETOUR_FUNCTION
#define DETOUR_FUNCTION

#include <Windows.h>

void* DetourFunction(void* pSrc, void* pDst, int len)
{
    DWORD curProtection;
    VirtualProtect(pSrc, len, PAGE_EXECUTE_READWRITE, &curProtection);

    uintptr_t relativeAddress = ((uintptr_t)pDst - (uintptr_t)pSrc) - 5;

    *(BYTE*)pSrc = 0xE9;
    *(uintptr_t*)((uintptr_t)pSrc + 1) = relativeAddress;

    DWORD temp;
    VirtualProtect(pSrc, len, curProtection, &temp);

    return (void*)((uintptr_t)pSrc + len);
}

#endif