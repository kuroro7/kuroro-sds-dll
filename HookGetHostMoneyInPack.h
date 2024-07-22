#ifndef HOOK_GET_HOST_MONEY_IN_PACK
#define HOOK_GET_HOST_MONEY_IN_PACK

#include "avector.h"
#include "DetourFunction.h"

#include "ReadMem.h"

#define GET_HOST_MONEY_IN_PACK_ADDRESS 0x00D9B750

#define MONEY_ADDRESS 0x0187A188
#define BIND_MONEY_ADDRESS 0x0187A198
#define CASH_ADDRESS 0x0187A1A8
#define STAR_ADDRESS 0x0187A1A0

typedef void (*GetHostMoneyInPack_t)(abase::vector<__int64>&);
GetHostMoneyInPack_t OriginalGetHostMoneyInPack = nullptr;

void HookGetHostMoneyInPack(abase::vector<__int64>& vecMoney)
{
    vecMoney.push_back(ReadMem<__int64>((LPVOID)BIND_MONEY_ADDRESS));
    vecMoney.push_back(ReadMem<__int64>((LPVOID)MONEY_ADDRESS));
    vecMoney.push_back(ReadMem<__int64>((LPVOID)STAR_ADDRESS));
    vecMoney.push_back(ReadMem<__int64>((LPVOID)CASH_ADDRESS));
}

void CreateHookGetHostMoneyInPack() {
    uintptr_t targetFunction = GET_HOST_MONEY_IN_PACK_ADDRESS;
    OriginalGetHostMoneyInPack = (GetHostMoneyInPack_t)targetFunction;
    DetourFunction((void*)targetFunction, HookGetHostMoneyInPack, 5);
}

#endif