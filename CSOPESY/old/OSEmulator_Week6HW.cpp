// OSEmulator_Week6HW.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "config.h"
#include "util.h"
#include "screen.h"

#include <iostream>
#include <string>

int main()
{
    std::string command;
    bool isRunning = true;
    Config config;
    bool isInitialized = false;

    // show header when program starts
    printHeader();

    while (isRunning) {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);

        if (command == "clear") {
            clearScreen();
            printHeader();
        }
        else if (command == "exit") {
            std::cout << "Exiting...\n";
            isRunning = false;
        }
        else if (command == "initialize") {
            if (loadConfig("config.txt", config)) {
                std::cout << "[OK] Configuration loaded.\n";
                isInitialized = true;
            }
            else {
                std::cout << "[ERROR] Failed to load config.txt.\n";
            }
        }
        /*else if (!isInitialized) {
            std::cout << "Please run 'initialize' first.\n";
        }*/
        else if (command == "scheduler-start") {
            //Scheduler::start(config);
        }
        else if (command == "scheduler-stop") {
            //Scheduler::stop();
        }
        else if (command == "report-util") {
            //Scheduler::printCPUUtilization();
        }
        else if (command.rfind("screen -s ", 0) == 0) {
            createScreen(command.substr(10));
        }
        else if (command.rfind("screen -r ", 0) == 0) {
            restoreScreen(command.substr(10));
        }
        else if (command == "screen -ls") {
            listScreens();
        }
        else {
            std::cout << "Unrecognized command.\n";
        }
    }

    return 0;
}
