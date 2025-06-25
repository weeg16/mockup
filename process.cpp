#include "process.h"
#include <iomanip>
#include <ctime>

Process::Process(const std::string& name, int totalIns)
    : name(name), totalInstructions(totalIns), executedInstructions(0), assignedCore(-1) {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
    timestamp = buf;
}

bool Process::isFinished() const {
    return executedInstructions >= totalInstructions;
}
