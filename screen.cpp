/* 
screen.cpp

Implements screen-related user interface functions, including printing headers, 
clearing the screen, and handling the main and process-specific CLI menus.
*/

#include "screen.h"
#include "process.h"
#include "util.h"

#include <iostream>
#include <unordered_map>
#include <string>
#include <thread>
#include <chrono>

#define ORANGE "\033[38;5;208m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

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
        // drawScreen(screen);
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

void listScreens() {
    std::cout << "\n--- Running Screens ---\n";
    for (const auto& pair : screens) {
        std::cout << "> " << pair.first << "\n";
    }
    std::cout << "\n";
}

void printProcessInfo(const Process* proc) {
    std::cout << "Process name: " << proc->name << "\n";
    std::cout << "ID: " << ORANGE << proc->id << RESET << "\n";

    std::cout << "Variables: ";
    for (const auto& pair : proc->variables) {
        std::cout << pair.first << "=" << pair.second << " ";
    }
    std::cout << "\n";
}

void printProcessLogsAndDetails(const Process* proc) {
    std::cout << "Logs:\n";
    for (const auto& msg : proc->logs) {
        // Assume log format: "(timestamp) Core:X \"msg\""
        size_t close_paren = msg.find(')');
        if (close_paren != std::string::npos) {
            std::string ts = msg.substr(1, close_paren - 1); // without '('
            std::cout << "  ";
            printColoredTimestamp(std::cout, ts);
            std::cout << msg.substr(close_paren + 1) << "\n";
        } else {
            std::cout << msg << "\n";
        }
    }
    std::cout << "\n";
    std::cout << "Current instruction line: " << ORANGE << proc->executedInstructions << RESET << "\n";
    std::cout << "Lines of code: " << ORANGE << proc->totalInstructions << RESET << "\n";
}

void enterProcessScreen(Process* proc) {
    clearScreen();
    printHeader();
    std::cout << "[Attached to process: " << proc->name << "]\n";
    while (true) {
        std::cout << "\n(Type 'process-smi' for info, 'exit' to return)\n\n";
        std::cout << "Enter a command: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") {
            clearScreen();
            printHeader();
            break;
        } else if (input == "process-smi") {
            clearScreen();
            printHeader();
            printProcessInfo(proc);
            printProcessLogsAndDetails(proc);

            if (proc->isFinished()) {
                std::cout << "\nProcess finished. Exiting screen in 2 seconds...\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
                break;
            }
        } else {
            std::cout << "Unknown command.\n";
        }
    }
}
