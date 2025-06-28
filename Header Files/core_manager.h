#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>
#include <random>
#include <atomic>
#include <cstdint>  
#include "process.h"

class CoreManager {
public:
    CoreManager();
    ~CoreManager();
    Process* getProcessByName(const std::string& name);

    void configure(uint32_t coresCount, const std::string& schedType, uint32_t quantum,
                   uint32_t batchFreq, uint32_t minI, uint32_t maxI, uint32_t delay);
    void start();
    void stopScheduler();
    void addProcess(Process* proc);
    void reportUtil();
    void listProcessStatus();
    void printProcessSummary(std::ostream& out, bool colorize);
    int generateRandomInstructionCount() const;
    Process* spawnNewNamedProcess(const std::string& name);

private:
    void tickLoop();
    void coreWorker(int coreId);

    // Config
    uint32_t numCores = 2;
    std::string schedulerType = "fcfs";
    uint32_t quantumCycles = 1;
    uint32_t batchProcessFreq = 5;
    uint32_t minIns = 10;
    uint32_t maxIns = 50;
    uint32_t delayPerExec = 0;

    // Runtime state
    std::atomic<uint64_t> cpuTicks{0};  // atomic to sync with tick loop
    int processCounter = 1;

    // Threads and Sync
    std::vector<std::thread> cores;
    std::thread tickThread;
    std::vector<bool> coreBusy;
    std::mutex queueMutex;
    std::condition_variable queueCond;
    std::atomic<bool> stop{false};

    // Data
    std::queue<Process*> readyQueue;
    std::vector<Process*> allProcesses;
    std::vector<int> coreInstructions;
    std::default_random_engine rng{std::random_device{}()};
};
