/*

    BUILD: g++ -std=c++11 -I"Header Files" -o emulator OSEmulator_Week6HW.cpp config.cpp screen.cpp util.cpp
    RUN: emulator

*/

#include "config.h"
#include "util.h"
#include "screen.h"
#include "scheduler.h"

#include <iostream>
#include <string>

CoreManager coreManager;
bool schedulerStarted = false;

int main()
{
    std::string command;
    bool isRunning = true;
    Config config;
    bool isInitialized = false;

    clearScreen();
    printHeader();

    while (isRunning) {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (command == "clear") {
            clearScreen();
            printHeader();
        }
        else if (command == "exit") {
            std::cout << "\nExiting...\n\n";
            isRunning = false;
        }
        else if (command == "initialize") {
            if (loadConfig("config.txt", config)) {
                coreManager.setScheduler(config.schedulerType, config.quantumCycles);
                std::cout << "\n[OK] Configuration loaded.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
                isInitialized = true;
            }
            else {
                std::cout << "\n[ERROR] Failed to load config.txt.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            }
        }
        else if (command == "scheduler-start") {
            if (!schedulerStarted) {
                std::cout << "\n[INFO] Starting " << config.schedulerType << " Scheduler...\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();

                coreManager.start();  // Start 4 core threads

                // create 10 test processes
                for (int i = 0; i < 10; ++i) {
                    std::string pname = "screen_" + std::to_string(i + 1);
                    coreManager.addProcess(new Process(pname, 100));
                }

                schedulerStarted = true;
            } else {
                std::cout << "\n[WARN] Scheduler is already running.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            }
        }
        else if (command == "scheduler-stop") {
            if (schedulerStarted) {
                coreManager.stopScheduler();
                schedulerStarted = false;
            } else {
                std::cout << "[WARN] Scheduler is not running.\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            }
        }
        else if (command == "report-util") {
            if (schedulerStarted) {
                coreManager.reportUtil();
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            } else {
                std::cout << "\n[WARN] Scheduler is not running.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            }
        }
        else if (command.rfind("screen -s ", 0) == 0) {
            createScreen(command.substr(10));
        }
        else if (command.rfind("screen -r ", 0) == 0) {
            restoreScreen(command.substr(10));
        }
        else if (command == "screen -ls") {
            if (schedulerStarted) {
                coreManager.listProcessStatus();
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            } else {
                std::cout << "\n[WARN] Scheduler is not running.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            }
        }
        else {
            std::cout << "\nUnrecognized command.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                clearScreen();
                printHeader();
        }
    }

    return 0;
}
