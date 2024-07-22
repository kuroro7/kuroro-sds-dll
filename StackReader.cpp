#include "pch.h"
#include "StackReader.h"

#include <Windows.h>

class StackReader {
public:

    static DWORD StackReader::ReadVar(DWORD offset) {
        CONTEXT context;
        DWORD varValue = 0;

        // Obter o contexto da thread atual
        context.ContextFlags = CONTEXT_FULL;
        GetCurrentThread();
        GetThreadContext(GetCurrentThread(), &context);

        // Calcular o endere�o da vari�vel local usando o registro rsp (ponteiro de pilha)
        DWORD stackPointer = context.Esp; // rsp em x64, esp em x86
        DWORD varAddress = stackPointer + offset;

        // Ler o valor da vari�vel da mem�ria do processo
        if (!ReadProcessMemory(GetCurrentProcess(), (LPCVOID)varAddress, &varValue, sizeof(DWORD), NULL))
        {
            // Tratamento de erro
            return 0;
        }

        return varValue;
    }
};


// Exemplo de uso para ler var_60
void ExemploUso()
{
    DWORD valorVar60 = ReadLocalVariable(0x60); // Offset de var_60 em rela��o ao ponteiro de pilha
    // Agora voc� pode usar valorVar60 como necess�rio
}
