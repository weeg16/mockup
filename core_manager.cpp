#include "core_manager.h"
#include "process.h"

#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>

CoreManager::CoreManager() {
    stop.store(false);
}

CoreManager::~CoreManager() {
    stopScheduler();
}

void CoreManager::configure(int coresCount, const std::string& schedType, int quantum, int batchFreq, int minI, int maxI, int delay) {
    numCores = coresCount;
    schedulerType = schedType;
    quantumCycles = std::max(1, quantum);
    batchProcessFreq = batchFreq;
    minIns = minI;
    maxIns = maxI;
    delayPerExec = delay;
    coreBusy.assign(numCores, false);
    coreInstructions.assign(numCores, 0);
}

void CoreManager::start() {
    stop = false;
    cpuTicks = 0;

    for (auto& t : cores) {
        if (t.joinable()) t.join();
    }
    cores.clear();

    for (int i = 0; i < numCores; ++i) {
        cores.emplace_back(&CoreManager::coreWorker, this, i);
    }

    tickThread = std::thread(&CoreManager::tickLoop, this);
}

void CoreManager::stopScheduler() {
    stop = true;
    queueCond.notify_all();

    for (auto& t : cores) {
        if (t.joinable()) t.join();
    }

    if (tickThread.joinable()) tickThread.join();

    std::cout << "\n[INFO] Scheduler stopped. All cores joined.\n\n";
}

void CoreManager::addProcess(Process* proc) {
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(proc);
    allProcesses.push_back(proc);
    queueCond.notify_one();
}

void CoreManager::reportUtil() {
    std::cout << "\n=== CPU Utilization Report ===\n";
    for (int i = 0; i < numCores; ++i) {
        std::cout << "Core " << i << ": " << coreInstructions[i] << " instructions executed.\n";
    }
    std::cout << "===============================\n\n";
}

void CoreManager::listProcessStatus() {
    std::cout << "\n--- Process Status ---\n\n";
    for (const auto& proc : allProcesses) {
        std::string status = proc->isFinished() ? "Finished" : (proc->assignedCore == -1 ? "Queued" : "Running");
        std::cout << proc->name << "  | " << status
                  << "  | Core " << proc->assignedCore
                  << "  | " << proc->executedInstructions << " / " << proc->totalInstructions
                  << "  | " << proc->timestamp << "\n";
    }
    std::cout << "\n------------------------\n\n";
}

void CoreManager::tickLoop() {
    while (!stop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec));
        cpuTicks++;

        if (cpuTicks % batchProcessFreq == 0) {
            std::string pname = "auto_proc_" + std::to_string(processCounter++);
            std::uniform_int_distribution<int> insDist(minIns, maxIns);
            addProcess(new Process(pname, insDist(rng)));
        }
    }
}

void CoreManager::coreWorker(int coreId) {
    while (!stop) {
        Process* proc = nullptr;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCond.wait(lock, [&] {
                return !readyQueue.empty() || stop;
            });

            if (stop) return;

            proc = readyQueue.front();
            readyQueue.pop();
            coreBusy[coreId] = true;
            proc->assignedCore = coreId;
        }

        if (schedulerType == "fcfs") {
            while (!proc->isFinished()) {
                ++proc->executedInstructions;
                ++coreInstructions[coreId];
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec));
            }
        } else if (schedulerType == "rr") {
            int cycles = 0;
            while (!proc->isFinished() && cycles < quantumCycles) {
                ++proc->executedInstructions;
                ++coreInstructions[coreId];
                ++cycles;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec));
            }

            if (!proc->isFinished()) {
                std::lock_guard<std::mutex> lock(queueMutex);
                readyQueue.push(proc);
                queueCond.notify_one();
            }
        }

        coreBusy[coreId] = false;
    }
}
