/* 
screen.h

Declares user interface utility functions for screen management, header printing,
and console interactions used throughout the OS Emulator.
*/

#pragma once

#include <string>
#include <unordered_map>

class Process;
void printProcessInfo(const Process* proc);
void printProcessLogsAndDetails(const Process* proc);
void enterProcessScreen(Process* proc);

struct ConsoleScreen {
    std::string name;
    std::string currentLine;
    std::string timestamp;
};

extern std::unordered_map<std::string, ConsoleScreen> screens;

void createScreen(const std::string& name);
void restoreScreen(const std::string& name);
void listScreens();
bool screenExists(const std::string& name);
