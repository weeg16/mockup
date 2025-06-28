#include "instruction_add.h"
#include "instruction_utils.h"
#include <cstdlib>

Instruction generateAdd(const std::string& dest, const std::string&, const std::string&) {
    std::string src1 = randomVarOrValue();
    std::string src2 = randomVarOrValue();
    return {InstructionType::ADD, {dest, src1, src2}};
}
