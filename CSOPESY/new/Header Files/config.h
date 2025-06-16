#pragma once

#include <string>

struct Config {
	int numCPU;
	std::string schedulerType;
	int quantumCycles;
	int batchProcFreq;
	int minIns;
	int maxIns;
	int delayPerExec;
};

bool loadConfig(const std::string& filename, Config& config);