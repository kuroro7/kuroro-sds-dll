#ifndef GAME_MINIMIZE_INCLUDED
#define GAME_MINIMIZE_INCLUDED

#include "ReadMem.h"

typedef int(__cdecl* RegisterLuaSub_t)(int, HWND (*)());

HWND Minimize() {
	HWND hwnd = (HWND)0x01868920;
	return (HWND)ShowWindow(hwnd, 6);
}

int __cdecl GameMinimizeFunc(int v) {
	
	RegisterLuaSub_t sub = (RegisterLuaSub_t)0x00D60260;
	return sub(v, Minimize);
}

#endif