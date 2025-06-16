#include "util.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
#else
#include <unistd.h>
#endif

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm timeinfo;

    std::tm* tm_ptr = std::localtime(&t);
    timeinfo = *tm_ptr;

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%m/%d/%Y, %I:%M:%S %p", &timeinfo);

    return std::string(buffer);
}


void printHeader() {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    #endif

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

void clearScreen() {
#ifdef _WIN32
    system("cls"); // windows
#else
    system("clear"); // macos or linux 
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