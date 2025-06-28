#pragma once
#include <string>
#include <cstdlib>

inline std::string randomVarOrValue() {
    const char* vars[] = {"x", "y", "z"};
    if (rand() % 2 == 0) {
        return vars[rand() % 3];
    } else {
        return std::to_string(rand() % 21);  // 0–20
    }
}
