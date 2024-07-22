#ifndef READ_MEM_H
#define READ_MEM_H

#include <Windows.h>
#include "Debug.h"

template <typename T>
T ReadMem(LPVOID address)
{
    HMODULE hModule = GetModuleHandle(NULL);

    if (hModule != NULL)
    {
        T* variablePointer = reinterpret_cast<T*>(address);
        T variableValue = *variablePointer;
        return variableValue;
    }
    else
        Debug("ERROR", "Failed to read memory");

    return T();
}

#endif
