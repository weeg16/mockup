#include "core_manager.h"
#include "process.h"
#include "util.h"

#include <iostream>
#include <random>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <string>
#include <fstream>

static const char* ORANGE = "\033[38;5;208m";
static const char* RESET = "\033[0m";

CoreManager::CoreManager() {
    stop.store(false);
    cpuTicks.store(0);
}

CoreManager::~CoreManager() {
    stopScheduler();
    for (auto* proc : allProcesses) {
        delete proc;
    }
}

void CoreManager::configure(uint32_t coresCount, const std::string& schedType, uint32_t quantum,
                            uint32_t batchFreq, uint32_t minI, uint32_t maxI, uint32_t delay) {
    numCores = coresCount;
    schedulerType = schedType;
    quantumCycles = std::max<uint32_t>(1, quantum);
    batchProcessFreq = batchFreq;
    minIns = minI;
    maxIns = maxI;
    delayPerExec = delay;
    coreBusy.assign(numCores, false);
    coreInstructions.assign(numCores, 0);
}

void CoreManager::start() {
    stop = false;
    cpuTicks.store(0);

    for (auto& t : cores) {
        if (t.joinable()) t.join();
    }
    cores.clear();

    for (uint32_t i = 0; i < numCores; ++i) {
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
    std::this_thread::sleep_for(std::chrono::seconds(2));
    clearScreen();
    printHeader();
}

void CoreManager::addProcess(Process* proc) {
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(proc);
    allProcesses.push_back(proc);
    queueCond.notify_one();
}

void CoreManager::reportUtil() {
    std::cout << "\n=== CPU Utilization Report ===\n";
    for (uint32_t i = 0; i < numCores; ++i) {
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 1 tick = 1ms
        cpuTicks.fetch_add(1);

        if (cpuTicks.load() % batchProcessFreq == 0) {
            std::string pname = "process" + std::to_string(processCounter);
            std::uniform_int_distribution<uint32_t> insDist(minIns, maxIns);
            int numIns = insDist(rng);
            addProcess(new Process(pname, processCounter++, numIns));
        }

        queueCond.notify_all(); 
    }
}

void CoreManager::coreWorker(int coreId) {
    while (true) {
        Process* proc = nullptr;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCond.wait(lock, [&] {
                return stop || !readyQueue.empty();
            });

            // Check again after wake-up to safely exit
            if (stop && readyQueue.empty()) {
                coreBusy[coreId] = false; // Optional cleanup
                return;
            }

            proc = readyQueue.front();
            readyQueue.pop();
            coreBusy[coreId] = true;
            proc->assignedCore = coreId;
        }

        int cycles = 0;
        while (!proc->isFinished() && (schedulerType == "fcfs" || cycles < quantumCycles)) {
            int startTick = cpuTicks.load();
            while (cpuTicks.load() - startTick < delayPerExec) {
                if (stop) return; // optional safety net
                std::this_thread::yield(); // prevent CPU spin
            }

            proc->executeNextInstruction();
            ++coreInstructions[coreId];
            ++cycles;
        }

        if (schedulerType == "rr" && !proc->isFinished()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(proc);
            queueCond.notify_one();
        }

        coreBusy[coreId] = false;
    }
}

Process* CoreManager::getProcessByName(const std::string& name) {
    for (auto* p : allProcesses) {
        if (p->name == name) return p;
    }
    return nullptr;
}

Process* CoreManager::spawnNewNamedProcess(const std::string& name) {
    std::uniform_int_distribution<uint32_t> dist(minIns, maxIns);
    int numIns = dist(rng);
    Process* proc = new Process(name, processCounter++, numIns);
    addProcess(proc);
    return proc;
}

void CoreManager::printProcessSummary(std::ostream& out, bool colorize) {
    int usedCores = 0;
    for (bool b : coreBusy) if (b) ++usedCores;
    int availableCores = numCores - usedCores;
    int percent = (numCores > 0) ? int((usedCores * 100.0) / numCores + 0.5) : 0;

    auto outc = [&](const std::string& s, const char* color = "") {
        if (colorize && color) out << color << s << RESET;
        else out << s;
    };

    out << "\nCPU utilization: ";
    outc(std::to_string(percent) + "%", ORANGE);
    out << "\nCores used: " << usedCores << "\nCores available: " << availableCores << "\n";
    out << "\n----------------------------------------\n";

    out << "\nRunning processes:\n\n";
    for (const auto& proc : allProcesses) {
        if (!proc->isFinished()) {
            out << proc->name << "  ";
            printColoredTimestamp(out, proc->timestamp);
            out << "  Core: ";
            outc(std::to_string(proc->assignedCore), ORANGE);
            out << "  ";
            outc(std::to_string(proc->executedInstructions), ORANGE);
            out << " / ";
            outc(std::to_string(proc->totalInstructions), ORANGE);
            out << "\n";
        }
    }

    out << "\nFinished processes:\n\n";
    for (const auto& proc : allProcesses) {
        if (proc->isFinished()) {
            out << proc->name << "  ";
            printColoredTimestamp(out, proc->timestamp);
            out << "  Finished  ";
            outc(std::to_string(proc->totalInstructions), ORANGE);
            out << " / ";
            outc(std::to_string(proc->totalInstructions), ORANGE);
            out << "\n";
        }
    }

    out << "\n----------------------------------------\n\n";
}

int CoreManager::generateRandomInstructionCount() const {
    std::uniform_int_distribution<uint32_t> dist(minIns, maxIns);
    return dist(const_cast<std::default_random_engine&>(rng));
}
