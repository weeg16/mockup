/* 
config.h

Declares the Config struct and function prototypes for config file parsing and validation.
*/

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