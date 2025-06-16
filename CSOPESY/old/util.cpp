#include "util.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// timestamp helper
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo;

#ifdef _WIN32
    localtime_s(&timeinfo, &t);
#else
    timeinfo = *std::localtime(&t);
#endif

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &timeinfo);

    return std::string(buffer);
}

// print the ASCII header
void printHeader() {
    std::cout << "_________________________________________________________________\n";
    std::cout << "      __       __       __     ____     _____      __     _     _\n";
    std::cout << "    /    )   /    )   /    )   /    )   /    '   /    )   |    /\n";
    std::cout << "---/---------\\-------/----/---/____/---/__-------\\--------|---/--\n";
    std::cout << "  /           \\     /    /   /        /           \\       |  /\n";
    std::cout << "_(____/___(____/___(____/___/________/____ ___(____/______|_/____\n";
    std::cout << "                                                           /\n";
    std::cout << "                                                       (_ /\n";
    std::cout << "\033[1;32mHello, welcome to CSOPESY commandline!\033[0m\n";
    std::cout << "\033[1;33mType 'exit' to quit, 'clear' to clear the screen.\033[0m\n\n";
}

// clear the screen
void clearScreen() {
#ifdef _WIN32
    system("cls"); // Windows command
#else
    system("clear"); // macOS or Linux command
#endif
}

// draw screen content
void drawScreen(const ConsoleScreen& screen) {
    std::cout << "\n+------------------------------------------------+\n";
    std::cout << "| Screen: " << screen.name << "\n";
    std::cout << "| Line: " << screen.currentLine << "\n";
    std::cout << "| Timestamp: " << screen.timestamp << "\n";
    std::cout << "+------------------------------------------------+\n";
    std::cout << "\n(Type 'exit' to return to main menu)\n\n";

    std::cout << "Enter a command: ";
}