#pragma once
#include <string>
#include <atomic>
#include <ctime>
#include <vector>

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
};

void enterProcessScreen(Process* proc);
void printProcessInfo(const Process* proc);
bool processIsActive(const Process* proc);
