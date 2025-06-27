/* 
process.cpp

Implements the Process class, per-process instruction logic (PRINT, DECLARE, ADD, SUBTRACT, SLEEP, FOR),
variable management, logging, and process screen behavior.
*/

#include "process.h"
#include "util.h"

#include <iomanip>
#include <ctime>
#include <iostream>     
#include <thread>  
#include <chrono>     
#include <sstream>



Process::Process(const std::string& name, int id, int totalIns)
    : name(name), id(id), totalInstructions(totalIns), executedInstructions(0), assignedCore(-1) {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S %p", std::localtime(&now));
    timestamp = buf;

    std::vector<std::string> vars = {"x", "y", "z"};
    for (const auto& v : vars) {
        int value = rand() % 20;
        instructions.push_back({InstructionType::DECLARE, {v, std::to_string(value)}});
    }
    for (int i = 3; i < totalInstructions; ++i) {
        int r = rand() % 6;
        if (r == 5) {
            // FOR block logic...
            int repeats = (rand() % 3) + 2;
            std::vector<Instruction> block;
            int blockLen = (rand() % 2) + 1;
            for (int b = 0; b < blockLen; ++b) {
                int t = rand() % 5;
                if (t == 0)
                    block.push_back({InstructionType::PRINT, {"Hello world from " + name + "!"}});
                else if (t == 1)
                    block.push_back({InstructionType::DECLARE, {"x", std::to_string(rand() % 10)}});
                else if (t == 2)
                    block.push_back({InstructionType::ADD, {"x", "y", "z"}});
                else if (t == 3)
                    block.push_back({InstructionType::SUBTRACT, {"x", "y", "z"}});
                else if (t == 4)
                    block.push_back({InstructionType::SLEEP, {std::to_string((rand() % 3) + 1)}});
            }
            Instruction forInstr = {InstructionType::FOR, {std::to_string(repeats)}, block};
            instructions.push_back(forInstr);
            continue; // Skip the rest and go to next i
        }
        // These are for r = 0 to 4, if not FOR
        else if (r == 0) {
            instructions.push_back({InstructionType::PRINT, {"Hello world from " + name + "!"}});
        }
        else if (r == 1) {
            instructions.push_back({InstructionType::DECLARE, {"x", std::to_string(rand() % 10)}});
        }
        else if (r == 2) {
            instructions.push_back({InstructionType::ADD, {"x", "y", "z"}});
        }
        else if (r == 3) {
            instructions.push_back({InstructionType::SUBTRACT, {"x", "y", "z"}});
        }
        else if (r == 4) {
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

// not used
// bool processIsActive(const Process* proc) {
//     return proc && !proc->isFinished();
// }

void Process::executeSingleInstruction(const Instruction& ins) {
    switch (ins.type) {
        case InstructionType::PRINT:
            logPrint(ins.args[0]);
            break;
        case InstructionType::DECLARE:
            variables[ins.args[0]] = static_cast<uint16_t>(std::stoi(ins.args[1]));
            break;
        case InstructionType::ADD: {
            uint16_t v2 = 0, v3 = 0;
            if (variables.count(ins.args[1])) v2 = variables[ins.args[1]];
            else try { v2 = static_cast<uint16_t>(std::stoi(ins.args[1])); } catch (...) { v2 = 0; }
            if (variables.count(ins.args[2])) v3 = variables[ins.args[2]];
            else try { v3 = static_cast<uint16_t>(std::stoi(ins.args[2])); } catch (...) { v3 = 0; }
            uint32_t sum = v2 + v3;
            if (sum > 65535) sum = 65535;
            variables[ins.args[0]] = static_cast<uint16_t>(sum);
            break;
        }
        case InstructionType::SUBTRACT: {
            uint16_t v2 = 0, v3 = 0;
            if (variables.count(ins.args[1])) v2 = variables[ins.args[1]];
            else try { v2 = static_cast<uint16_t>(std::stoi(ins.args[1])); } catch (...) { v2 = 0; }
            if (variables.count(ins.args[2])) v3 = variables[ins.args[2]];
            else try { v3 = static_cast<uint16_t>(std::stoi(ins.args[2])); } catch (...) { v3 = 0; }
            int diff = v2 - v3;
            if (diff < 0) diff = 0;
            variables[ins.args[0]] = static_cast<uint16_t>(diff);
            break;
        }
        case InstructionType::SLEEP:
            sleepTicks = std::stoi(ins.args[0]);
            break;
        default: break;
    }
}

bool Process::executeNextInstruction() {
    if (sleepTicks > 0) {
        --sleepTicks;
        return true;
    }

    // Check if inside a FOR loop
    if (!forStack.empty()) {
        // Use std::get for C++11 compatibility
        auto& tup = forStack.back();
        size_t& instrIdx = std::get<0>(tup);
        size_t& blockPtr = std::get<1>(tup);
        int& left = std::get<2>(tup);
        Instruction& forIns = instructions[instrIdx];
        if (blockPtr < forIns.block.size()) {
            Instruction& curr = forIns.block[blockPtr];
            executeSingleInstruction(curr);
            ++blockPtr;
            ++executedInstructions;
        } else if (left > 1) {
            blockPtr = 0;
            --left;
        } else {
            // Finished all repeats of FOR
            forStack.pop_back();
            ++instructionPointer;
            ++executedInstructions;
        }
        return true;
    }

    // Normal instruction flow
    if (instructionPointer >= instructions.size()) return false;
    Instruction& ins = instructions[instructionPointer];

    if (ins.type == InstructionType::FOR) {
        int repeats = std::stoi(ins.args[0]);
        forStack.push_back(std::make_tuple(instructionPointer, 0, repeats));
        return true;
    } else {
        executeSingleInstruction(ins);
        ++instructionPointer;
        ++executedInstructions;
        return true;
    }
}