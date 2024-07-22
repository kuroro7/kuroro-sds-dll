#ifndef DEBUG_H
#define DEBUG_H

#include <Windows.h>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>

#define CONST_DEBUG_TITLE "KuroroSDS.dll"

template <typename T>
void Debug(const char* title, T message)
{
    std::stringstream ss;
    ss << title << ": " << message << std::endl;
    MessageBoxA(NULL, ss.str().c_str(), CONST_DEBUG_TITLE, MB_OK | MB_ICONINFORMATION);
}

template <>
void Debug(const char* title, DWORD_PTR message)
{
    std::stringstream ss;
    ss << title << ": 0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << (uintptr_t)message;
    MessageBoxA(NULL, ss.str().c_str(), CONST_DEBUG_TITLE, MB_OK | MB_ICONINFORMATION);
}

template <>
void Debug(const char* title, std::wstring message)
{
    std::stringstream ss;
    ss << title << ": " << message.c_str() << std::endl;
    MessageBoxA(NULL, ss.str().c_str(), CONST_DEBUG_TITLE, MB_OK | MB_ICONINFORMATION);
}

#endif // DEBUG_H
