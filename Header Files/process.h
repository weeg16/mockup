#pragma once
#include <string>
#include <atomic>
#include <ctime>

class Process {
public:
    std::string name;
    int totalInstructions;
    std::atomic<int> executedInstructions;
    int assignedCore;
    std::string timestamp;

    Process(const std::string& name, int totalIns);
    bool isFinished() const;
};
