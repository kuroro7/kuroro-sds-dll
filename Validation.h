#pragma once

#include <string>
#include <algorithm>

#include <windows.h>
#include <string>
#include <algorithm>

#include "Debug.h"

std::string DecryptString(const std::string& input) {
    const std::string key = "\x04\x08\x15\x16\x23\x42";
    std::string output;
    output.reserve(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        output.push_back(input[i] ^ key[i % key.size()]);
    }

    return output;
}

bool ValidateName(HINSTANCE hInstance) {
    char path[MAX_PATH];
    if (GetModuleFileNameA(hInstance, path, MAX_PATH) == 0) {
        return false;
    }

    std::string dllPath = path;
    std::string encryptedExpectedDllName = "\x4f\x7d\x67\x79\x51\x2d\x57\x4c\x46\x38\x47\x2e\x68";
    std::string expectedDllName = DecryptString(encryptedExpectedDllName);

    size_t pos = dllPath.find_last_of("\\/");
    std::string dllName = (pos == std::string::npos) ? dllPath : dllPath.substr(pos + 1);

    return (dllName == expectedDllName);
}