#pragma once

#include <string>
#include <unordered_map>

struct ConsoleScreen {
    std::string name;
    std::string currentLine;
    std::string timestamp;
};

extern std::unordered_map<std::string, ConsoleScreen> screens;

void createScreen(const std::string& name);
void restoreScreen(const std::string& name);
void listScreens();