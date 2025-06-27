/* 
screen.cpp

Implements screen-related user interface functions, including printing headers, 
clearing the screen, and handling the main and process-specific CLI menus.
*/

#include "screen.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, ConsoleScreen> screens;

void createScreen(const std::string& name) {
    if (screens.find(name) != screens.end()) {
        std::cout << "[WARN] Screen '" << name << "' already exists. Use 'screen -r' instead.\n";
        return;
    }

    ConsoleScreen screen{ name, "Waiting for instruction...", getCurrentTimestamp() };
    screens[name] = screen;

    clearScreen();
    std::cout << "\nCreated screen '" << name << "'\n\n";

    while (true) {
        drawScreen(screen);
        std::string input;
        if (!std::getline(std::cin, input) || input == "exit") {
            clearScreen();
            printHeader();
            break;
        }
        screen.currentLine = input;
        screen.timestamp = getCurrentTimestamp();
        screens[name] = screen;
        clearScreen();
    }
}

void restoreScreen(const std::string& name) {
    if (screens.find(name) == screens.end()) {
        std::cout << "Screen '" << name << "' not found.\n";
        return;
    }

    while (true) {
        clearScreen();
        drawScreen(screens[name]);
        std::string input;
        if (!std::getline(std::cin, input) || input == "exit") { 
            clearScreen();
            printHeader();
            break;
        }
        screens[name].currentLine = input;
        screens[name].timestamp = getCurrentTimestamp();
        clearScreen();
    }
}

void listScreens() {
    std::cout << "\n--- Running Screens ---\n";
    for (const auto& pair : screens) {
        std::cout << "> " << pair.first << "\n";
    }
    std::cout << "\n";
}
