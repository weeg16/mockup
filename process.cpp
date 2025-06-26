#include "process.h"
#include "util.h"

#include <iomanip>
#include <ctime>
#include <iostream>     
#include <thread>  
#include <chrono>     
#include <sstream>

#define ORANGE "\033[38;5;208m"
#define BLUE "\033[34m"
#define RESET "\033[0m"

Process::Process(const std::string& name, int id, int totalIns)
    : name(name), id(id), totalInstructions(totalIns), executedInstructions(0), assignedCore(-1) {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
    timestamp = buf;

    // TEMP: Add logs for testing
    logPrint("Hello world from " + name + "!");
    logPrint("Test message 1 from " + name);
    logPrint("Another log from " + name);
}

bool Process::isFinished() const {
    return executedInstructions >= totalInstructions;
}

void Process::logPrint(const std::string& message) {
    std::ostringstream oss;
    oss << "(" << getCurrentTimestamp() << ") "
        << "Core:" << assignedCore << " \"" << message << "\"";
    logs.push_back(oss.str());
}

void printProcessInfo(const Process* proc) {
    std::cout << "Process name: " << proc->name << "\n";
    std::cout << "ID: " << ORANGE << proc->id << RESET << "\n";
}

void printProcessLogsAndDetails(const Process* proc) {
    std::cout << "Logs:\n";
    for (const auto& msg : proc->logs) {
        size_t p = 0;

        std::cout << "  " << BLUE << "(" << RESET;
        p++;

        size_t first_slash = msg.find('/', p);
        std::cout << ORANGE << msg.substr(p, first_slash - p) << RESET;
        p = first_slash;

        std::cout << BLUE << "/" << RESET;
        p++;

        size_t second_slash = msg.find('/', p);
        std::cout << ORANGE << msg.substr(p, second_slash - p) << RESET;
        p = second_slash;

        std::cout << BLUE << "/" << RESET;
        p++;

        size_t year_end = msg.find(' ', p);
        std::cout << ORANGE << msg.substr(p, year_end - p) << RESET;
        p = year_end;

        std::cout << " ";
        p++;

        size_t colon1 = msg.find(':', p);
        std::cout << ORANGE << msg.substr(p, colon1 - p) << RESET;
        p = colon1;

        std::cout << BLUE << ":" << RESET;
        p++;

        size_t colon2 = msg.find(':', p);
        std::cout << ORANGE << msg.substr(p, colon2 - p) << RESET;
        p = colon2;

        std::cout << BLUE << ":" << RESET;
        p++;

        size_t space_pm = msg.find(' ', p);
        std::cout << ORANGE << msg.substr(p, space_pm - p) << RESET;
        p = space_pm;

        std::cout << " " << ORANGE << msg.substr(p + 1, 2) << RESET;
        p += 3; 

        size_t close_paren = msg.find(')', p);
        std::cout << BLUE << ")" << RESET;
        p = close_paren + 1;

        std::cout << msg.substr(p, msg.find("Core:", p) - p);

        size_t core_start = msg.find("Core:", p);
        std::cout << "Core:";

        size_t core_num_start = core_start + 5;
        size_t core_num_end = msg.find(' ', core_num_start);
        std::cout << ORANGE << msg.substr(core_num_start, core_num_end - core_num_start) << RESET;

        std::cout << msg.substr(core_num_end) << "\n";
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


bool processIsActive(const Process* proc) {
    return proc && !proc->isFinished();
}
