#include "instruction_subtract.h"
#include "instruction_utils.h"
#include <cstdlib>


Instruction generateSubtract(const std::string& dest, const std::string& /*unused1*/, const std::string& /*unused2*/) {
    std::string src1 = randomVarOrValue();
    std::string src2 = randomVarOrValue();
    return {InstructionType::SUBTRACT, {dest, src1, src2}};
}
