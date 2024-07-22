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

        // Calcular o endereço da variável local usando o registro rsp (ponteiro de pilha)
        DWORD stackPointer = context.Esp; // rsp em x64, esp em x86
        DWORD varAddress = stackPointer + offset;

        // Ler o valor da variável da memória do processo
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
    DWORD valorVar60 = ReadLocalVariable(0x60); // Offset de var_60 em relação ao ponteiro de pilha
    // Agora você pode usar valorVar60 como necessário
}
