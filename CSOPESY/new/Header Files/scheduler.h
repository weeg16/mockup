#pragma once
#include <string>
#include <fstream>
#include <atomic>
#include <iostream>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>

struct Process {
    std::string name;
    int totalInstructions;
    std::atomic<int> executedInstructions;
    int assignedCore = -1;
    std::ofstream outputFile;

    Process(const std::string& name, int totalIns)
        : name(name),
          totalInstructions(totalIns),
          executedInstructions(0),
          outputFile(name + ".txt") {}

    ~Process() {
        if (outputFile.is_open()) {
            outputFile.close();
        }
    }

    void logPrint(int coreId, const std::string& timestamp) { // disable before submission
        outputFile << "(" << timestamp << ") "
                << "Core: " << coreId << " "
                << "\"Hello world from " << name << "!\"\n";
        outputFile.flush();
    }

    bool isFinished() const {
        return executedInstructions >= totalInstructions;
    }
};

class CoreManager {
private:
    static const int NUM_CORES = 4;

    std::vector<std::thread> cores;
    std::vector<bool> coreBusy;
    std::queue<Process*> readyQueue;

    std::mutex queueMutex;
    std::condition_variable queueCond;

    std::atomic<bool> stop;

    std::vector<int> coreInstructions;

    std::vector<Process*> allProcesses;

public:
    CoreManager() : coreBusy(NUM_CORES, false), coreInstructions(NUM_CORES, 0) {
        stop.store(false); 
    }

    ~CoreManager() {
        stop = true;
        queueCond.notify_all();
        for (auto& t : cores) {
            if (t.joinable()) t.join();
        }
    }

    void start() {
        for (int i = 0; i < NUM_CORES; ++i) {
            cores.emplace_back(&CoreManager::coreWorker, this, i);
        }
    }

    void addProcess(Process* proc) {
        std::lock_guard<std::mutex> lock(queueMutex);
        readyQueue.push(proc);
        allProcesses.push_back(proc);
        queueCond.notify_one();
    }

    void stopScheduler() {
        stop = true;
        queueCond.notify_all();

        for (auto& t : cores) {
            if (t.joinable()) t.join();
        }

        std::cout << "\n[INFO] Scheduler stopped. All cores joined.\n\n";
    }

    void reportUtil() {
        std::cout << "\n=== CPU Utilization Report ===\n";
        for (int i = 0; i < NUM_CORES; ++i) {
            std::cout << "Core " << i << ": "
                    << coreInstructions[i] << " instructions executed.\n";
        }
        std::cout << "===============================\n\n";
    }

    void listProcessStatus() {
        std::cout << "\n--- Process Status ---\n\n";
        for (const auto& proc : allProcesses) {
            std::string status = proc->isFinished()
                ? "Finished"
                : (proc->assignedCore == -1 ? "Queued" : "Running");
            std::cout << proc->name << "  | "
                    << status << "  | "
                    << "Core " << proc->assignedCore << "  | "
                    << proc->executedInstructions << " / " << proc->totalInstructions
                    << "\n";
        }
        std::cout << "\n------------------------\n\n";
    }

private:
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

            while (!proc->isFinished()) {
                proc->logPrint(coreId, getCurrentTimestamp());
                ++proc->executedInstructions;
                ++coreInstructions[coreId]; 

                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // can delay further if needed
            }

            coreBusy[coreId] = false;
        }
    }
};