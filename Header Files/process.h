#pragma once
#include <string>
#include <atomic>
#include <ctime>
#include <vector>
#include <unordered_map>

enum class InstructionType {
    PRINT,
    DECLARE,
    ADD,
    SUBTRACT,
    SLEEP
    // Add FOR later if you want
};

struct Instruction {
    InstructionType type;
    std::vector<std::string> args;
};


class Process {
public:
    std::string name;
    int id;
    int totalInstructions;
    std::atomic<int> executedInstructions;
    int assignedCore;
    std::string timestamp;
    std::vector<std::string> logs;

    Process(const std::string& name, int id, int totalIns);
    bool isFinished() const;
    void logPrint(const std::string& message);

    std::vector<Instruction> instructions;
    size_t instructionPointer = 0;
    std::unordered_map<std::string, uint16_t> variables;

    bool executeNextInstruction();
};

void enterProcessScreen(Process* proc);
void printProcessInfo(const Process* proc);
bool processIsActive(const Process* proc);
