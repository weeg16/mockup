#include "screen.h"
#include "util.h"
#include <iostream>
#include <unordered_map>
#include <string>

std::unordered_map<std::string, ConsoleScreen> screens;

void createScreen(const std::string& name) {
    ConsoleScreen screen{ name, "Waiting for instruction...", getCurrentTimestamp() };
    screens[name] = screen;

    clearScreen();
    std::cout << "\nCreated screen '" << name << "'\n\n";

    while (true) {
        drawScreen(screen);
        std::string input;
        std::getline(std::cin, input);
        if (input == "exit") {
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
        std::getline(std::cin, input);
        if (input == "exit") {
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