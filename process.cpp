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

    std::vector<std::string> vars = {"x", "y", "z"};
    for (const auto& v : vars) {
        int value = rand() % 20;
        instructions.push_back({InstructionType::DECLARE, {v, std::to_string(value)}});
    }
    for (int i = 3; i < totalInstructions; ++i) { // Start from 3 because first 3 are DECLARE
        int r = rand() % 5; // 0=PRINT, 1=DECLARE, 2=ADD, 3=SUBTRACT, 4=SLEEP
        if (r == 0) {
            instructions.push_back({InstructionType::PRINT, {"Hello world from " + name + "!"}});
        } else if (r == 1) {
            instructions.push_back({InstructionType::DECLARE, {vars[rand() % vars.size()], std::to_string(rand() % 20)}});
        } else if (r == 2) {
            instructions.push_back({InstructionType::ADD, {vars[0], vars[1], vars[2]}});
        } else if (r == 3) {
            instructions.push_back({InstructionType::SUBTRACT, {vars[0], vars[1], vars[2]}});
        } else if (r == 4) {
            instructions.push_back({InstructionType::SLEEP, {std::to_string((rand() % 3) + 1)}});
        }
    }

    // TEMP: To see instructions for each process at creation

    // std::cout << name << " has " << instructions.size() << " instructions:\n";
    // for (const auto& ins : instructions) {
    //     if (ins.type == InstructionType::PRINT) std::cout << "PRINT: " << ins.args[0] << "\n";
    //     else if (ins.type == InstructionType::DECLARE) std::cout << "DECLARE " << ins.args[0] << "\n";
    //     else if (ins.type == InstructionType::ADD) std::cout << "ADD\n";
    //     else if (ins.type == InstructionType::SUBTRACT) std::cout << "SUBTRACT\n";
    //     else if (ins.type == InstructionType::SLEEP) std::cout << "SLEEP\n";
    // }
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

    std::cout << "Variables: ";
    for (const auto& pair : proc->variables) {
        std::cout << pair.first << "=" << pair.second << " ";
    }
    std::cout << "\n";
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

bool Process::executeNextInstruction() {
    if (instructionPointer >= instructions.size()) return false;
    Instruction& ins = instructions[instructionPointer];
    switch (ins.type) {
        case InstructionType::PRINT:
            logPrint(ins.args[0]);
            break;
        case InstructionType::DECLARE:
            variables[ins.args[0]] = static_cast<uint16_t>(std::stoi(ins.args[1]));
            break;
        case InstructionType::ADD: {
            uint16_t v2 = 0, v3 = 0;
            if (variables.count(ins.args[1])) {
                v2 = variables[ins.args[1]];
            } else {
                try {
                    v2 = static_cast<uint16_t>(std::stoi(ins.args[1]));
                } catch (...) {
                    v2 = 0;
                }
            }
            if (variables.count(ins.args[2])) {
                v3 = variables[ins.args[2]];
            } else {
                try {
                    v3 = static_cast<uint16_t>(std::stoi(ins.args[2]));
                } catch (...) {
                    v3 = 0;
                }
            }
            uint32_t sum = v2 + v3;
            if (sum > 65535) sum = 65535;
            variables[ins.args[0]] = static_cast<uint16_t>(sum);
            break;
        }
        case InstructionType::SUBTRACT: {
            uint16_t v2 = 0, v3 = 0;
            if (variables.count(ins.args[1])) {
                v2 = variables[ins.args[1]];
            } else {
                try {
                    v2 = static_cast<uint16_t>(std::stoi(ins.args[1]));
                } catch (...) {
                    v2 = 0;
                }
            }
            if (variables.count(ins.args[2])) {
                v3 = variables[ins.args[2]];
            } else {
                try {
                    v3 = static_cast<uint16_t>(std::stoi(ins.args[2]));
                } catch (...) {
                    v3 = 0;
                }
            }
            int diff = v2 - v3;
            if (diff < 0) diff = 0;
            variables[ins.args[0]] = static_cast<uint16_t>(diff);
            break;
        }
        case InstructionType::SLEEP:
            // Optional: implement sleep handling in scheduler
            break;
    }
    instructionPointer++;
    executedInstructions++;
    return true;
}
