#include <windows.h>
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <Psapi.h>
#include "Debug.h"

class MemoryScanner
{
public:
    MemoryScanner(HMODULE hModule)
        : baseAddress(reinterpret_cast<DWORD_PTR>(hModule))
    {
        if (hModule == NULL)
        {
            throw std::runtime_error("Falha ao obter o handle do módulo.");
        }
    }

    DWORD_PTR FindUnicodeString(const wchar_t* target)
    {
        MEMORY_BASIC_INFORMATION mbi;
        uintptr_t start = 0;
        size_t targetLen = wcslen(target) * sizeof(wchar_t);
        HANDLE hProcess = GetCurrentProcess();
        SYSTEM_INFO sysInfo;
        GetSystemInfo(&sysInfo);
        uintptr_t end = (uintptr_t)sysInfo.lpMaximumApplicationAddress;

        while (start < end)
        {
            if (VirtualQueryEx(hProcess, (LPCVOID)start, &mbi, sizeof(mbi)) == 0)
                break;

            if (mbi.State == MEM_COMMIT &&
                (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_READONLY || mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ))
            {
                // Verificar se a região é legível
                if (mbi.Protect != PAGE_NOACCESS && mbi.Protect != PAGE_GUARD)
                {
                    for (uintptr_t addr = (uintptr_t)mbi.BaseAddress; addr < (uintptr_t)mbi.BaseAddress + mbi.RegionSize - targetLen; addr += sizeof(wchar_t))
                    {
                        // Adiciona uma verificação de tentativa de leitura
                        __try {
                            if (memcmp((void*)addr, target, targetLen) == 0) {
                                //Debug("STR FOUND", (DWORD_PTR)addr);
                                return (DWORD_PTR)addr;
                            }
                        }
                        __except (EXCEPTION_EXECUTE_HANDLER) {
                            // Captura exceções de acesso à memória
                            continue;
                        }
                    }
                }
            }

            start += mbi.RegionSize;
        }

        return 0;
    }

private:
    DWORD_PTR baseAddress;
};