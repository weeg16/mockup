#pragma once
#include <string>
#include <fstream>
#include <atomic>
#include <iostream>
#include <thread>
#include <mutex>
#include <ctime>
#include <condition_variable>
#include <queue>
#include <vector>
#include <random>

struct Process {
    std::string name;
    int totalInstructions;
    std::atomic<int> executedInstructions;
    int assignedCore = -1;
    std::string timestamp;

    Process(const std::string& name, int totalIns)
        : name(name), totalInstructions(totalIns), executedInstructions(0) {
        std::time_t now = std::time(nullptr);
        char buf[100];
        std::strftime(buf, sizeof(buf), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
        timestamp = buf;
    }

    bool isFinished() const {
        return executedInstructions >= totalInstructions;
    }
};

class CoreManager {
public:
    CoreManager() {
        stop.store(false);
    }

    ~CoreManager() {
        stopScheduler();
    }

    void configure(int coresCount, const std::string& schedType, int quantum, int batchFreq, int minI, int maxI, int delay) {
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

    void start() {
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

    void stopScheduler() {
        stop = true;
        queueCond.notify_all();

        for (auto& t : cores) {
            if (t.joinable()) t.join();
        }

        if (tickThread.joinable()) tickThread.join();

        std::cout << "\n[INFO] Scheduler stopped. All cores joined.\n\n";
    }

    void addProcess(Process* proc) {
        std::lock_guard<std::mutex> lock(queueMutex);
        readyQueue.push(proc);
        allProcesses.push_back(proc);
        queueCond.notify_one();
    }

    void reportUtil() {
        std::cout << "\n=== CPU Utilization Report ===\n";
        for (int i = 0; i < numCores; ++i) {
            std::cout << "Core " << i << ": " << coreInstructions[i] << " instructions executed.\n";
        }
        std::cout << "===============================\n\n";
    }

    void listProcessStatus() {
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

private:
    int numCores = 2;
    std::string schedulerType = "fcfs";
    int quantumCycles = 1;
    int batchProcessFreq = 5;
    int minIns = 10;
    int maxIns = 50;
    int delayPerExec = 0;

    std::vector<std::thread> cores;
    std::vector<bool> coreBusy;
    std::queue<Process*> readyQueue;
    std::mutex queueMutex;
    std::condition_variable queueCond;
    std::atomic<bool> stop{false};

    std::vector<int> coreInstructions;
    std::vector<Process*> allProcesses;

    std::default_random_engine rng{std::random_device{}()};
    std::thread tickThread;

    int processCounter = 1;
    int cpuTicks = 0;

    void tickLoop() {
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

    void coreWorker(int coreId) {
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
};
