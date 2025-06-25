#include "config.h"
#include "util.h"
#include "screen.h"
#include "scheduler.h"

#include <iostream>
#include <string>
#include <thread>

CoreManager coreManager;
bool schedulerStarted = false;
bool isInitialized = false;

int main() {
    std::string command;
    Config config;
    bool isRunning = true;

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
            if (schedulerStarted) {
                coreManager.stopScheduler();  // Also stops ticks + threads
            }
            std::cout << "\nExiting...\n\n";
            isRunning = false;
        }
        else if (command == "initialize") {
            if (loadConfig("config.txt", config)) {
                coreManager.configure(
                    config.numCPU,
                    config.schedulerType,
                    config.quantumCycles,
                    config.batchProcFreq,
                    config.minIns,
                    config.maxIns,
                    config.delayPerExec
                );
                std::cout << "\n[OK] Configuration loaded.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
                isInitialized = true;
                schedulerStarted = false;
            } else {
                std::cout << "\n[ERROR] Failed to load config.txt.\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();
            }
        }
        else if (command == "scheduler-start") {
            if (!isInitialized) {
                std::cout << "\n[WARN] Please run 'initialize' first.\n\n";
                continue;
            }

            if (!schedulerStarted) {
                std::cout << "\n[INFO] Starting " << config.schedulerType << " Scheduler...\n\n";
                std::this_thread::sleep_for(std::chrono::seconds(2));
                clearScreen();
                printHeader();

                coreManager.start();              
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
