#include "instruction_for.h"
#include "instruction_print.h"
#include "instruction_declare.h"
#include "instruction_add.h"
#include "instruction_subtract.h"
#include "instruction_sleep.h"
#include <cstdlib>

Instruction generateFor(const std::string& processName) {
    int repeats = (rand() % 3) + 2;
    int blockLen = (rand() % 2) + 1;
    std::vector<Instruction> block;

    for (int i = 0; i < blockLen; ++i) {
        int t = rand() % 5;
        if (t == 0) block.push_back(generatePrint(processName));
        else if (t == 1) block.push_back(generateDeclare("x", rand() % 10));
        else if (t == 2) block.push_back(generateAdd("x", "y", "z"));
        else if (t == 3) block.push_back(generateSubtract("x", "y", "z"));
        else if (t == 4) block.push_back(generateSleep((rand() % 3) + 1));
    }

    return {InstructionType::FOR, {std::to_string(repeats)}, block};
}