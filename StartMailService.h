#ifndef START_MAIL_SERVICE
#define START_MAIL_SERVICE

#include "ReadMem.h"

typedef int(__thiscall* sub_602A00_t)(void*);
typedef int(*sub_D597C0_t)(int a1, double a2);

sub_D597C0_t sub_D597C0_ptr = nullptr;

#define SUB_D598C0_ADDRESS 0x00D597C0
#define REGISTER_LUA_FUNC 0x00FA6240

int StartMailService(int num) {
	uintptr_t targetFunction = SUB_D598C0_ADDRESS;
	sub_D597C0_ptr = (sub_D597C0_t)targetFunction;
	return sub_D597C0_ptr(num, 0x4000000000000000LL);
}

#endif
