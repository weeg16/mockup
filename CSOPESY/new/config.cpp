#include "config.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

bool loadConfig(const std::string& filename, Config& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open config file: " << filename << "\n"; 
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        iss >> key;

        if (key == "num-cpu") iss >> config.numCPU;
        else if (key == "scheduler") {
            char quote;
            iss >> quote >> config.schedulerType >> quote;
        }
        else if (key == "quantum-cycles") iss >> config.quantumCycles;
        else if (key == "batch-process-freq") iss >> config.batchProcFreq;
        else if (key == "min-ins") iss >> config.minIns;
        else if (key == "max-ins") iss >> config.maxIns;
        else if (key == "delay-per-exec") iss >> config.delayPerExec;
    }

    return true;
}
