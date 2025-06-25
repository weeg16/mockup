#pragma once
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>
#include <random>
#include "process.h"

class CoreManager {
public:
    CoreManager();
    ~CoreManager();

    void configure(int coresCount, const std::string& schedType, int quantum, int batchFreq, int minI, int maxI, int delay);
    void start();
    void stopScheduler();
    void addProcess(Process* proc);
    void reportUtil();
    void listProcessStatus();

private:
    void tickLoop();
    void coreWorker(int coreId);

    // Config
    int numCores = 2;
    std::string schedulerType = "fcfs";
    int quantumCycles = 1;
    int batchProcessFreq = 5;
    int minIns = 10;
    int maxIns = 50;
    int delayPerExec = 0;

    // Runtime state
    int cpuTicks = 0;
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
