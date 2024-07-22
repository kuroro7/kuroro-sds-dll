#ifndef REGISTER_LUA_INCLUDED
#define REGISTER_LUA_INCLUDED

#include "GameMinimize.h"

typedef int(__cdecl* RegisterLua_t)(const char*, const char*, int (__cdecl*)(int));

int RegisterLuaFunctions() {
	RegisterLua_t RegisterLuaFunc = (RegisterLua_t)0x00FA6240;
	return RegisterLuaFunc("GameApi", "GameMinimize", GameMinimizeFunc);
}

int AppendLuaFunctions() {
	DWORD flOldProtect;

	char* v5 = (char*)VirtualAlloc(0, 5u, 0x1000u, 0x40u);
	*v5 = 0xE9;
	//*(DWORD*)(v5 + 1) = (DWORD)((char*)RegisterLuaFunctions - 14235877);
	*(DWORD*)(v5 + 1) = (DWORD)RegisterLuaFunctions - 14235877;
	VirtualProtect((LPVOID)0x00D938E0, 5u, 0x40u, &flOldProtect);
	*(DWORD*)0x00D938E0 = *(DWORD*)v5;
	*(BYTE*)0x00D938E4 = v5[4];
	return VirtualProtect((LPVOID)0x00D938E0, 5u, flOldProtect, &flOldProtect);
}


#endif