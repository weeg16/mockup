#include "instruction_random.h"
#include "instruction_add.h"
#include "instruction_subtract.h"
#include "instruction_declare.h"
#include "instruction_print.h"
#include "instruction_sleep.h"
#include "instruction_for.h"
#include <cstdlib>

std::vector<Instruction> generateInstructionSet(const std::string& processName, int totalInstructions) {
    std::vector<Instruction> instructions;

    for (const auto& var : {"x", "y", "z"}) {
        instructions.push_back(generateDeclare(var, rand() % 20));
    }

    for (int i = 3; i < totalInstructions; ++i) {
        int r = rand() % 6;

        if (r == 5) {
            instructions.push_back(generateFor(processName));
        } else if (r == 0) {
            instructions.push_back(generatePrint(processName));
        } else if (r == 1) {
            instructions.push_back(generateDeclare("x", rand() % 10));
        } else if (r == 2) {
            instructions.push_back(generateAdd("x", "y", "z"));
        } else if (r == 3) {
            instructions.push_back(generateSubtract("x", "y", "z"));
        } else if (r == 4) {
            instructions.push_back(generateSleep(rand() % 3 + 1));
        }
    }

    return instructions;
}
